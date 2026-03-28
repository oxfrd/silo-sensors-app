#include "udsServer.h"
#include "iSnapshotProvider.h"

#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <cstring>
#include <cerrno>
#include <iostream>
#include <json/json.h>

UdsServer::UdsServer(std::string socketPath, ISnapshotProvider& provider, std::chrono::milliseconds minInterval)
    : socketPath_(std::move(socketPath)), provider_(provider), minInterval_(minInterval) {}

UdsServer::~UdsServer() {
    stop();
}

void UdsServer::start() {
    if (running_) return;
    running_ = true;
    worker_ = std::thread(&UdsServer::loop, this);
}

void UdsServer::stop() {
    running_ = false;

    if (serverFd_ >= 0) {
        shutdown(serverFd_, SHUT_RDWR);
    }

    if (worker_.joinable()) {
        worker_.join();
    }

    cleanup();
}

bool UdsServer::createAndBindSocket() {
    serverFd_ = socket(AF_UNIX, SOCK_STREAM, 0);
    if (serverFd_ < 0) {
        std::cerr << "socket() failed: " << strerror(errno) << "\n";
        return false;
    }

    unlink(socketPath_.c_str());

    sockaddr_un addr{};
    addr.sun_family = AF_UNIX;
    std::strncpy(addr.sun_path, socketPath_.c_str(), sizeof(addr.sun_path) - 1);

    if (bind(serverFd_, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0) {
        std::cerr << "[Socket] bind() failed: " << strerror(errno) << "\n";
        return false;
    }

    if (listen(serverFd_, 5) < 0) {
        std::cerr << "[Socket] listen() failed: " << strerror(errno) << "\n";
        return false;
    }

    return true;
}

void UdsServer::cleanup() {
    if (serverFd_ >= 0) {
        close(serverFd_);
        serverFd_ = -1;
    }
    unlink(socketPath_.c_str());
}

void UdsServer::loop() {
    if (!createAndBindSocket()) {
        running_ = false;
        return;
    }

    using clock = std::chrono::steady_clock;
    lastSendTime_ = clock::now() - minInterval_;

    while (running_) {
        int clientFd = accept(serverFd_, nullptr, nullptr);
        if (clientFd < 0) {
            if (running_) {
                std::cerr << "[Socket] accept() failed: " << strerror(errno) << "\n";
            }
            continue;
        }

        auto now = clock::now();
        if (now - lastSendTime_ < minInterval_) {
            std::cout << "[Socket] Rate limited: skipping send\n";
            close(clientFd);
            continue;
        }

        Json::Value snapshot = provider_.getSnapshot();
        Json::StreamWriterBuilder builder;
        builder["indentation"] = "";
        std::string payload = Json::writeString(builder, snapshot);
        payload.push_back('\n');

        const char* data = payload.c_str();
        size_t total = 0;
        while (total < payload.size()) {
            ssize_t n = send(clientFd, data + total, payload.size() - total, 0);
            if (n < 0) {
                std::cerr << "[Socket] send() failed: " << strerror(errno) << "\n";
                break;
            }
            total += static_cast<size_t>(n);
        }

        std::cout << std::endl;

        if (total == payload.size()) {
            std::cout << "[Socket] Sent JSON over UDS: " << payload << std::endl;
            lastSendTime_ = clock::now();
        } else {
            std::cerr << "[Socket] Partial send over UDS: " << total << "/" << payload.size() << "\n";
        }

        close(clientFd);
    }

    cleanup();
}
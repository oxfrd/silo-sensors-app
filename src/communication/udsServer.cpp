#include "udsServer.h"
#include "iSnapshotProvider.h"

#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <cstring>
#include <cerrno>
#include <iostream>
#include <json/json.h>

UdsServer::UdsServer(std::string socketPath, ISnapshotProvider& provider)
    : socketPath_(std::move(socketPath)), provider_(provider) {}

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
        std::cerr << "bind() failed: " << strerror(errno) << "\n";
        return false;
    }

    if (listen(serverFd_, 5) < 0) {
        std::cerr << "listen() failed: " << strerror(errno) << "\n";
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

    while (running_) {
        int clientFd = accept(serverFd_, nullptr, nullptr);
        if (clientFd < 0) {
            if (running_) {
                std::cerr << "accept() failed: " << strerror(errno) << "\n";
            }
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
                std::cerr << "send() failed: " << strerror(errno) << "\n";
                break;
            }
            total += static_cast<size_t>(n);
        }

        close(clientFd);
    }

    cleanup();
}
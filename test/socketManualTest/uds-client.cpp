#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include <cstring>
#include <iostream>
#include <string>

int main(int argc, char *argv[])
{
    const char *socketPath = "/run/silo-monitor.sock";
    int loops = 6;

    for (int i = 1; i < argc; ++i)
    {
        if (std::strcmp(argv[i], "--socket") == 0 && i + 1 < argc)
        {
            socketPath = argv[++i];
        }
        else
        {
            loops = std::stoi(argv[i]);
        }
    }

    for (int i = 0; i < loops; ++i)
    {
        int fd = socket(AF_UNIX, SOCK_STREAM, 0);
        if (fd < 0)
        {
            std::cerr << "socket() failed\n";
            return 1;
        }

        sockaddr_un addr{};
        addr.sun_family = AF_UNIX;
        std::strncpy(addr.sun_path, socketPath, sizeof(addr.sun_path) - 1);

        if (connect(fd, reinterpret_cast<sockaddr *>(&addr), sizeof(addr)) < 0)
        {
            std::cerr << "connect() failed: " << std::strerror(errno) << "\n";
            close(fd);
            return 1;
        }

        char buf[4096];
        ssize_t n = read(fd, buf, sizeof(buf) - 1);
        if (n < 0)
        {
            std::cerr << "read() failed: " << std::strerror(errno) << "\n";
            close(fd);
            return 1;
        }

        buf[n] = '\0';
        std::cout << buf << std::endl;
        close(fd);
    }

    return 0;
}
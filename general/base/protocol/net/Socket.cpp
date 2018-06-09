#include "Socket.h"

Socket::Socket() {
    socket_file_descriptor = socket(AF_INET, SOCK_STREAM, 0);
    if (!haveSocket())
    {
        std::cerr << "socket failed" << std::endl;
        exit(EXIT_FAILURE);
    }
}

Socket::Socket(int file_descriptor) {
    socket_file_descriptor = file_descriptor;
}

bool Socket::haveSocket() const {
    return socket_file_descriptor > 0;
}

int Socket::read() {
    if (!haveSocket()) {
        std::cerr << "no socket" << std::endl;
    }

    int result = ::read(socket_file_descriptor, buffer, 1024);

    return result;
}

int Socket::send(const std::string& message) {
    if (!haveSocket()) {
        std::cerr << "no socket" << std::endl;
    }

    int result = ::send(socket_file_descriptor, message.c_str(), message.size(), 0);

    return result;
}

std::string Socket::getBuffer() {
    return buffer;
}

#include "TCPSocket.h"

TCPSocket::TCPSocket() {
    socket_file_descriptor = socket(AF_INET, SOCK_STREAM, 0);
    if (!haveSocket())
    {
        std::cerr << "socket failed" << std::endl;
        exit(EXIT_FAILURE);
    }
}

TCPSocket::TCPSocket(int file_descriptor) {
    socket_file_descriptor = file_descriptor;
}

bool TCPSocket::haveSocket() const {
    return socket_file_descriptor > 0;
}

int TCPSocket::read() {
    if (!haveSocket()) {
        std::cerr << "no socket" << std::endl;
    }

    int result = ::read(socket_file_descriptor, buffer, 1024);

    return result;
}

int TCPSocket::send(const std::string& message) {
    if (!haveSocket()) {
        std::cerr << "no socket" << std::endl;
    }

    int result = ::send(socket_file_descriptor, message.c_str(), message.size(), 0);

    return result;
}

std::string TCPSocket::getBuffer() {
    return buffer;
}

#include "Server.h"

Server::Server() : TCPSocket() {
}

bool Server::create(const std::string& host, uint32_t port) {
    int result = setsockopt(socket_file_descriptor, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));
    if (result)
    {
        std::cerr << "setsockopt error" << std::endl;
        return false;
    }

    address.sin_family      = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port        = htons( PORT );

    return true;
}

bool Server::bind() {
    int result = ::bind(socket_file_descriptor, (struct sockaddr *)&address, sizeof(address));
    if (result < 0)
    {
        std::cerr << "bind failed" << std::endl;
        return false;
    }
    return true;
}

int Server::listen() {
    int listen_response = ::listen(socket_file_descriptor, 3);
    if (listen_response < 0)
    {
        std::cerr << "cant't listen" << std::endl;
        return false;
    }
    return true;
}

TCPSocket* Server::accept() {
    int addrlen = sizeof(address);
    int new_socket = ::accept(socket_file_descriptor, (struct sockaddr *)&address, (socklen_t*)&addrlen);

    if (new_socket < 0)
    {
        std::cerr << "accept new conn error" << std::endl;
        return nullptr;
    }
    client_socket = new TCPSocket(new_socket);

    return client_socket;
}

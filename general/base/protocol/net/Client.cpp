#include "Client.h"

Client::Client() {
    socket_handler = socket(AF_INET, SOCK_STREAM, 0);
    if (!haveSocket())
    {
        std::cerr << "Socket creation error" << std::endl;
    }
}


bool Client::haveSocket() const {
    return socket_handler > 0;
}

bool Client::create(std::string host, uint32_t port) {
    if (!haveSocket()) {
        return false;
    }

    memset(&serv_addr, '0', sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);

    int result = inet_pton(AF_INET, host.c_str(), &serv_addr.sin_addr);
    if (result <= 0)
    {
        std::cerr << "Invalid address or address not supported" << std::endl;
        return false;
    }

    return true;
}

bool Client::connect() {
    if (!haveSocket()) {
        return false;
    }

    int result = ::connect(socket_handler, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

    if (result < 0)
    {
        std::cerr << "Connection Failed" << std::endl;
        return false;
    }

    return true;
}

int Client::send(const std::string& message) {
    if (!haveSocket()) {
        return 0;
    }

    ::send(socket_handler, message.c_str(), message.size(), 0);

    return read(socket_handler, buffer, 1024);
}

std::string Client::getBuffer() const {
    return buffer;
}

#include "Client.h"

Client::Client() : TCPSocket() {
}

int Client::create(const std::string& host, uint32_t port) {
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

    int result = ::connect(socket_file_descriptor, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

    if (result < 0)
    {
        std::cerr << "Connection Failed" << std::endl;
        return false;
    }

    return true;
}

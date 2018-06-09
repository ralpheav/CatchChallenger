#ifndef SERVER_H
#define SERVER_H

#include "Socket.h"

#define PORT 8080

class Server : public Socket {

    Socket* client_socket;
    struct sockaddr_in address;
    int opt = 1;

public: 
    Server();
    bool create(const std::string& host, uint32_t port);
    bool bind();
    int listen();
    Socket* accept();
};

#endif

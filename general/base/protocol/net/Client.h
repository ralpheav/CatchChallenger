#ifndef CLIENT_H
#define CLIENT_H

#include "TCPSocket.h"

#define HOST "127.0.0.1"
#define PORT 8080

class Client : public TCPSocket {

    private:
        struct sockaddr_in address;
        struct sockaddr_in serv_addr;

    public:
        Client();
        bool create(std::string host, uint32_t port);
        bool connect();
};

#endif

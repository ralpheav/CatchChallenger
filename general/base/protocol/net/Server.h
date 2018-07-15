#ifndef CATCHCHALLENGER_SERVER_H
#define CATCHCHALLENGER_SERVER_H

#include "TCPSocket.h"

#define PORT 8080

namespace CatchChallenger
{
    class Server : public TCPSocket {

        TCPSocket* client_socket;
        struct sockaddr_in address;
        int opt = 1;

    public:
        Server();
        bool create(const std::string& host, uint32_t port);
        bool bind();
        int listen();
        TCPSocket* accept();
    };
}

#endif //CATCHCHALLENGER_SERVER_H

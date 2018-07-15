#ifndef CATCHCHALLENGER_CLIENT_H
#define CATCHCHALLENGER_CLIENT_H

#include "TCPSocket.h"

#define HOST "127.0.0.1"
#define PORT 8080

namespace CatchChallenger
{
    class Client : public TCPSocket {

        private:
            struct sockaddr_in address;
            struct sockaddr_in serv_addr;

        public:
            Client();
            bool create(const std::string& host, uint32_t port);
            bool connect();
    };
}

#endif //CATCHCHALLENGER_CLIENT_H

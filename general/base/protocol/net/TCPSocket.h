#ifndef SOCKET_H
#define SOCKET_H

#include <iostream>
#include <cstring>
#include <cstdint>
#include <string>
#include <unistd.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "ISocket.h"

namespace CatchChallenger
{
    class TCPSocket : public ISocket
    {
        protected:
            int socket_file_descriptor;

        private:
            char buffer[1024] = {0};

        public:
            TCPSocket();
            TCPSocket(int file_descriptor);

            int read();
            int send(const std::string& message);
            std::string getBuffer();

        protected:
            bool haveSocket() const;

    };
}

#endif

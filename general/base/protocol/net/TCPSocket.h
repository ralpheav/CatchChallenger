#ifndef TCPSOCKET_H
#define TCPSOCKET_H

#include <iostream>
#include <cstring>
#include <cstdint>
#include <string>
#include <unistd.h>

#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <termios.h>

#include "ISocket.h"

namespace CatchChallenger
{
    class TCPSocket : public ISocket
    {
        protected:
            SocketOption option;
            int socket_file_descriptor;

        private:
            int waitForDisconnectedTime;
            int waitForConnectTime;
            char buffer[1024] = {0}; //TODO: set maxsize, limit
            struct sockaddr_in address;
            struct sockaddr_in serv_addr;

        public:
            TCPSocket();
            TCPSocket(int file_descriptor);

            int read();
            int send(const std::string& message);
            std::string getBuffer();
            void setSocketOption(SocketOption option,int parameter);
            bool bytesAvailable();
            bool encryptedBytesAvailable();
            void deleteLater();
            void abort();
            void connectToHost(const std::string& host, int port);
            void disconnectFromHost();
            void error();
            void flush();
            bool isValid();
            void socketDescriptor();
            std::string localAddress();
            int localPort();
            std::string peerAddress();
            std::string peerName();
            int peerPort();
            SocketState state();
            bool waitForConnected(int msec);
            bool waitForDisconnected(int msec);
            bool openMode();
            std::string errorString();
            int readData(char* message, size_t max);
            int writeData(const char* message, size_t max);
        protected:
            bool haveSocket() const;

    };
}

#endif //TCPSOCKET_H

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
            void setSocketOption(SocketOption option, int mode);
            int64_t bytesAvailable() const;
            bool encryptedBytesAvailable();
            void abort();
            void connectToHost(const std::string& host, int port);
            void disconnectFromHost();
            void flush();
            bool isValid() const;
            bool socketDescriptor();
            std::string localAddress();
            int localPort();
            std::string peerAddress();
            std::string peerName();
            int peerPort();
            bool waitForConnected(int msec);
            bool waitForDisconnected(int msec);
            bool openMode();
            std::string errorString();
            uint64_t readData(char* data, int64_t maxSize);
            uint64_t writeData(const char* data, int64_t maxSize);

            void close();
            bool isSequential() const;
            bool canReadLine() const;
            void internal_writeData(std::vector<unsigned char> rawData);
            int64_t bytesAvailableWithMutex();
            uint64_t getRXSize();
            uint64_t getTXSize();
        protected:
            bool haveSocket() const;

    };
}

#endif //TCPSOCKET_H

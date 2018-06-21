#ifndef SSLSOCKET_H
#define SSLSOCKET_H

#include <iostream>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <malloc.h>
#include <string.h>
#include <string>

#include <sys/socket.h>
#include <resolv.h>
#include <netdb.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#include "ISocket.h"

namespace CatchChallenger
{
    class SSLSocket : public ISocket
    {
        int waitForDisconnectedTime;
        int waitForConnectTime;

        char buffer[1024]; //TODO, set maxsize
        SSL_CTX *ctx;
        SSL *ssl;

        SocketState state;
        int socket_descriptor;
        struct hostent *host;
        struct sockaddr_in addr;

    public:

        SSLSocket();
        ~SSLSocket();

        void loadCertificates(const char* CertFile, const char* KeyFile);
        int openConnection(const char* hostname, int port);
        SSL_CTX* initCTX();
        void showCerts();
        bool connect();
        bool getState();
        const char* getEncryptation();
        int send(const char* message);
        int read();
        const char* getBuffer();
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
    };
}

#endif //SSLSOCKET_H

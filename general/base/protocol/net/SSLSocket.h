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

namespace CatchChallenger
{
    class SSLSocket {

        char buffer[1024];
        SSL_CTX *ctx;
        SSL *ssl;

        int state;
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
    };
}
#endif


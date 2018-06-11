#ifndef SSLSERVER_H
#define SSLSERVER_H

#include <iostream>
#include <errno.h>
#include <unistd.h>
#include <malloc.h>
#include <string.h>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <resolv.h>
#include "openssl/ssl.h"
#include "openssl/err.h"

class SSLServer {
    char buffer[1024]; //TODO: need to increase dynamically or statically
    char reply[1024]; //TODO: need to increase dynamically or statically

    SSL_CTX *ctx;
    SSL *ssl;
    int socket_descriptor;
    struct sockaddr_in addr;

public:
    SSLServer();
    ~SSLServer();

    int openListener(int port);
    SSL_CTX* initServerCTX();
    void loadCertificates(const  char* CertFile, const char* KeyFile);
    void showCerts();
    SSL* accept();
    void servlet(SSL* ssl);
    bool connectClient(SSL* ssl);
    int readClient(SSL* ssl);
    const char* getBuffer();
    void setReply(const char* input);
    void sendClient(SSL* ssl);
    void closeClient(SSL* ssl);

};

#endif

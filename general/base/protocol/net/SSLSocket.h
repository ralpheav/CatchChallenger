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
    enum SSLCertificate {
        Organization,
        CommonName,
        LocalityName,
        OrganizationalUnitName,
        CountryName,
        StateOrProvinceName,
        EmailAddress
    };

    class SSLKey {
        std::string key;

        public:
            std::string getKey() {
                return key;
            }

            std::string toPem() {
                //TODO: get the Pem
                return key;
            }
    };

    class SSLInfo {
        SSLKey key;

        public:
            std::string issuerInfo(SSLCertificate certificate) {
                //TODO: get the certificate field
                return std::string();
            }

            SSLKey publicKey() {
                return key;
            }
    };

    class SSLSocket : public ISocket
    {
        SSLInfo info;
        int waitForDisconnectedTime;
        int waitForConnectTime;
        PeerVerifyMode verifyMode;
        bool ignoreErrors;

        char buffer[1024]; //TODO, set maxsize
        SSL_CTX *ctx;
        SSL *ssl;

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
        void abort();
        void connectToHost(const std::string& host, int port);
        void disconnectFromHost();
        SocketError error();
        void flush();
        bool isValid();
        bool socketDescriptor();
        std::string localAddress();
        int localPort();
        std::string peerAddress();
        std::string peerName();
        int peerPort();
        SocketState state();
        void ignoreSslErrors();
        void startClientEncryption();
        bool waitForConnected(int msec);
        bool waitForDisconnected(int msec);
        bool openMode();
        SslMode mode();
        void setPeerVerifyMode(PeerVerifyMode mode);
        std::string errorString();
        int readData(char* message, size_t max);
        int writeData(const char* message, size_t max);
        SSLInfo peerCertificate();
    };
}

#endif //SSLSOCKET_H

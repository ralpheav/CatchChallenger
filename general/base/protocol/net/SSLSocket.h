#ifndef SSLSOCKET_H
#define SSLSOCKET_H

#include <iostream>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <malloc.h>
#include <string.h>
#include <string>
#include <list>

#include <sys/ioctl.h>
#include <sys/socket.h>
#include <resolv.h>
#include <netdb.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <termios.h>

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
        struct hostent *hostAddress;
        struct sockaddr_in addr;
        SslMode m_sslMode;

    public:

        SSLSocket();
        ~SSLSocket();

        void open(DeviceMode mode);
        void loadCertificates(const char* CertFile, const char* KeyFile);
        int openConnection(const char* hostname, int port);
        SSL_CTX* initCTX();
        void showCerts();
        bool connect();
        bool getState();
        SSL_CTX* init();
        const char* getEncryptation();
        int send(const char* message);
        int read();
        const char* getBuffer();
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
        void ignoreSslErrors();
        void startClientEncryption();
        bool waitForConnected(int msec);
        bool waitForDisconnected(int msec);
        bool openMode();
        SslMode sslMode();
        void setSslMode(SslMode mode);
        void setPeerVerifyMode(PeerVerifyMode mode);
        std::string errorString();
        int readData(char* message, size_t max);
        int writeData(const char* message, size_t max);
        SSLInfo peerCertificate();
        void close();
        uint64_t getRXSize();
        uint64_t getTXSize();
        bool isSequential() const;
        bool canReadLine() const;
        uint64_t readData(char* data, int64_t maxSize);
        uint64_t writeData(const char* data, int64_t maxSize);
        void internal_writeData(std::vector<unsigned char> rawData);
        int64_t bytesAvailableWithMutex();
        std::list<SslError> sslErrors();
    };
}

#endif //SSLSOCKET_H

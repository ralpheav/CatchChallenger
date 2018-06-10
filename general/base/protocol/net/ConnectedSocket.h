#ifndef CATCHCHALLENGER_CONNECTEDSOCKET_H
#define CATCHCHALLENGER_CONNECTEDSOCKET_H

#if ! defined(EPOLLCATCHCHALLENGERSERVER) && ! defined (ONLYMAPRENDER)

#include <QIODevice>
#include <QSslSocket>
#include <vector>

#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include <stdint.h>
#include <string>
#include <list>


#include "FakeSocket.h"

namespace CatchChallenger {

class HostAddress {
    static std::string localhost;
    static std::string Null;

    std::string ip;
public:

    void setAddress(const std::string& ip) {
        this->ip = ip;
        //struct sockaddr_in sa;
        //return inet_pton(AF_INET, str.c_str(), &(sa.sin_addr))!=0;
        //struct sockaddr_in6 sa;
        //return inet_pton(AF_INET6, str.c_str(), &(sa.sin6_addr))!=0;
    }

    void clear() {
        ip.clear();
    }

    bool isEqual(HostAddress second) const {
        return ip == second.ip;
    }

    bool isMulticast() const {
        return false;
    }

    bool isLoopback() const {
        return false;
    }

    std::string toString() const {
        return ip;
    }

    std::string toIpv6() {
        std::vector<std::string> octets = split(ip, ".");
        unsigned char* octetBytes = new unsigned char[4];
        for (int index = 0; index < 4; ++index) {
             octetBytes[i] = static_cast<unsigned char>(octets[index]);
        }

        unsigned char* ipv4asIpV6addr = new unsigned char[16];
        ipv4asIpV6addr[10] = (unsigned char)0xff;
        ipv4asIpV6addr[11] = (unsigned char)0xff;
        ipv4asIpV6addr[12] = octetBytes[0];
        ipv4asIpV6addr[13] = octetBytes[1];
        ipv4asIpV6addr[14] = octetBytes[2];
        ipv4asIpV6addr[15] = octetBytes[3];
    }

private:
    std::vector<std::string> split(const std::string& target, char delimitator)
    {
        std::vector<std::string> result;
        std::istringstream iss(target);

        for (std::string token; std::getline(iss, token, delimitator);)
        {
            result.push_back(std::move(token));
        }

        return result;
    }
};

enum OpenMode {
    unknown = 0
};

class ConnectedSocket : public QIODevice
{
    public:
        explicit ConnectedSocket(FakeSocket* socket);
        explicit ConnectedSocket(QSslSocket* socket);
        explicit ConnectedSocket(QTcpSocket* socket);
        ~ConnectedSocket();

        void	abort();
        void	connectToHost(const std::string& hostName, uint16_t port);
        void	connectToHost(const HostAddress& address, uint16_t port);
        void	disconnectFromHost();
        int     error() const;
        bool	flush();
        bool	isValid() const;
        void    setTcpCork(const bool &cork);
        HostAddress localAddress() const;
        uint16_t localPort() const;
        HostAddress peerAddress() const;
        std::string peerName() const;
        uint16_t peerPort() const;
        enum {
            unconnected = 0,
            connected   = 1
        } m_state;
        int state() const;
        bool	waitForConnected(int msecs = 30000);
        bool	waitForDisconnected(int msecs = 30000);
        int64_t	bytesAvailable() const;
        OpenMode openMode() const;
        std::string errorString() const;
        int64_t	readData(char * data, int64_t maxSize);
        int64_t	writeData(const char * data, int64_t maxSize);
        void	close();
        FakeSocket *fakeSocket;
        QSslSocket *sslSocket;
        QTcpSocket *tcpSocket;
    protected:
        bool isSequential() const;
        bool canReadLine() const;
        std::list<QSslError> sslErrors() const;
        //workaround because QSslSocket don't return correct value for i2p via proxy
        std::string hostName;
        uint16_t    port;

        virtual void	connected() = 0;
        virtual void	disconnected() = 0;
        virtual void	error(int socketError) = 0;
        virtual void	stateChanged(int socketState) = 0;
        virtual void    sslErrors(const std::list<QSslError> &errors) = 0;
        virtual void    destroyed() = 0;
        virtual void    readyRead() = 0;
        virtual void    stateChanged() = 0;
        virtual void    encrypted() = 0;
    private:
        void destroyedSocket();
        void purgeBuffer();
    };
}
#endif

#endif

#ifndef CATCHCHALLENGER_CONNECTEDSOCKET_H
#define CATCHCHALLENGER_CONNECTEDSOCKET_H

#if ! defined(EPOLLCATCHCHALLENGERSERVER) && ! defined (ONLYMAPRENDER)

#include <QIODevice>
#include <QSslSocket>
#include <QAbstractSocket>
#include <QObject>
#include <QHostAddress>
#include <QByteArray>

#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include <stdint.h>
#include <string>


#include "FakeSocket.h"

namespace CatchChallenger {

class ConnectedSocket : public QIODevice
{
    public:
        explicit ConnectedSocket(QFakeSocket* socket);
        explicit ConnectedSocket(QSslSocket* socket);
        explicit ConnectedSocket(QTcpSocket* socket);
        ~ConnectedSocket();

        void	abort();
        void	connectToHost(const std::string& hostName, uint16_t port);
        void	connectToHost(const QHostAddress& address, uint16_t port);
        void	disconnectFromHost();
        QAbstractSocket::SocketError error() const;
        bool	flush();
        bool	isValid() const;
        void    setTcpCork(const bool &cork);
        QHostAddress localAddress() const;
        uint16_t localPort() const;
        QHostAddress peerAddress() const;
        std::string peerName() const;
        uint16_t peerPort() const;
        QAbstractSocket::SocketState state() const;
        bool	waitForConnected(int msecs = 30000);
        bool	waitForDisconnected(int msecs = 30000);
        int64_t	bytesAvailable() const;
        OpenMode openMode() const;
        std::string errorString() const;
        int64_t	readData(char * data, int64_t maxSize);
        int64_t	writeData(const char * data, int64_t maxSize);
        void	close();
        QFakeSocket *fakeSocket;
        QSslSocket *sslSocket;
        QTcpSocket *tcpSocket;
    protected:
        bool isSequential() const;
        bool canReadLine() const;
        QList<QSslError> sslErrors() const;
        //workaround because QSslSocket don't return correct value for i2p via proxy
        std::string hostName;
        uint16_t port;
    signals:
        void	connected();
        void	disconnected();
        void	error(QAbstractSocket::SocketError socketError);
        void	stateChanged(QAbstractSocket::SocketState socketState);
        void    sslErrors(const QList<QSslError> &errors);
    private:
        void destroyedSocket();
        void purgeBuffer();
    };
}
#endif

#endif

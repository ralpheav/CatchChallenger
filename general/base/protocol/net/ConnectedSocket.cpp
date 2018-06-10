#if ! defined(EPOLLCATCHCHALLENGERSERVER) && ! defined (ONLYMAPRENDER)

#include "ConnectedSocket.h"

#ifdef __linux__
#include <netinet/tcp.h>
#include <netdb.h>
#endif

using namespace CatchChallenger;

std::string HostAddress::localhost = "127.0.0.1";
std::string HostAddress::Null      = "0.0.0.0";

ConnectedSocket::ConnectedSocket(FakeSocket* socket) :
    //TODO: needs to be changed to a Interface call
    fakeSocket(socket),
    sslSocket(nullptr),
    tcpSocket(nullptr)
{
    //open(QIODevice::ReadWrite | QIODevice::Unbuffered);
}

ConnectedSocket::ConnectedSocket(QSslSocket* socket) :
    //TODO: needs to be changed to a Interface call
    fakeSocket(nullptr),
    sslSocket(socket),
    tcpSocket(nullptr)
{
    socket->setSocketOption(QAbstractSocket::KeepAliveOption, 1);

    purgeBuffer();
    //open(QIODevice::ReadWrite|QIODevice::Unbuffered);
}

ConnectedSocket::ConnectedSocket(QTcpSocket* socket) :
    //TODO: needs to be changed to a Interface call
    fakeSocket(nullptr),
    sslSocket(nullptr),
    tcpSocket(socket)
{
    socket->setSocketOption(QAbstractSocket::KeepAliveOption, 1);

    //open(QIODevice::ReadWrite|QIODevice::Unbuffered);
}

ConnectedSocket::~ConnectedSocket()
{
    //TODO: needs to be changed to a Interface call
    if (sslSocket != nullptr) {
        sslSocket->deleteLater();
    }

    if (tcpSocket != nullptr) {
        tcpSocket->deleteLater();
    }

    if (fakeSocket != nullptr) {
        fakeSocket->deleteLater();
    }
}

std::list<QSslError> ConnectedSocket::sslErrors() const
{
    if (sslSocket != nullptr) {
        return sslSocket->sslErrors();
    }

    return std::list<QSslError>();
}

void ConnectedSocket::purgeBuffer()
{
    if (sslSocket != nullptr) {
        if (sslSocket->bytesAvailable()) {
             readyRead();
        }

        if (sslSocket->encryptedBytesAvailable()) {
            readyRead();
        }
    }
}

void ConnectedSocket::destroyedSocket()
{
    sslSocket = nullptr;
    tcpSocket = nullptr;
    fakeSocket = nullptr;

    hostName.clear();
    port = 0;
}

void ConnectedSocket::abort()
{
    hostName.clear();
    port=0;
//TODO: needs to be changed to a Interface call
    if (sslSocket != nullptr) {
        sslSocket->abort();
    }

    if (tcpSocket != nullptr) {
        tcpSocket->abort();
    }

    if (fakeSocket != nullptr) {
        fakeSocket->abort();
    }
}

void ConnectedSocket::connectToHost(const std::string& hostName, uint16_t port)
{
    if (state() != ConnectedSocket::unconnected) {
        return;
    }

    if (fakeSocket != nullptr) {
        fakeSocket->connectToHost();
    } else {
        //workaround because QSslSocket don't return correct value for i2p via proxy
        this->hostName = hostName;
        this->port = port;
//TODO: needs to be changed to a Interface call
        if (sslSocket != nullptr) {
            sslSocket->connectToHost(hostName,port);
        }

        if (tcpSocket != nullptr) {
            tcpSocket->connectToHost(hostName,port);
        }
    }
}

void ConnectedSocket::connectToHost(const HostAddress& address, uint16_t port)
{
    if(state() != ConnectedSocket::unconnected) {
        return;
    }

    //TODO: needs to be changed to a Interface call
    if (sslSocket != nullptr) {
        sslSocket->connectToHost(address.toString(), port);
    }

    if (tcpSocket != nullptr) {
        tcpSocket->connectToHost(address, port);
    }

    if (fakeSocket != nullptr) {
        fakeSocket->connectToHost();
    }
}

void ConnectedSocket::disconnectFromHost()
{
    if (state() != ConnectedSocket::unconnected) {
        return;
    }

    hostName.clear();
    port = 0;

    //TODO: needs to be changed to a Interface call
    if (sslSocket != nullptr) {
        sslSocket->disconnectFromHost();
    }

    if (tcpSocket != nullptr) {
        tcpSocket->disconnectFromHost();
    }

    if (fakeSocket != nullptr) {
        fakeSocket->disconnectFromHost();
    }
}

int ConnectedSocket::error() const
{
    //TODO: needs to be changed to a Interface call
    if (sslSocket != nullptr) {
        return sslSocket->error();
    }

    if (tcpSocket != nullptr) {
        return tcpSocket->error();
    }

    if (fakeSocket != nullptr) {
        return fakeSocket->error();
    }

    return 0;//unknown
}

bool ConnectedSocket::flush()
{
    //TODO: needs to be changed to a Interface call
    if (sslSocket != nullptr) {
       return  sslSocket->flush();
    }

    if (tcpSocket != nullptr) {
        return tcpSocket->flush();
    }

    return false;
}

bool ConnectedSocket::isValid() const
{
    //TODO: needs to be changed to a Interface call
    if (sslSocket != nullptr) {
        return sslSocket->isValid();
    }

    if (tcpSocket != nullptr) {
        return tcpSocket->isValid();
    }

    if (fakeSocket != nullptr) {
        return fakeSocket->isValid();
    }

    return false;
}

void ConnectedSocket::setTcpCork(const bool& cork)
{
    #ifdef __linux__
        if (sslSocket != nullptr)
        {
            #if ! defined(EPOLLCATCHCHALLENGERSERVER) && ! defined (ONLYMAPRENDER)
                const int* &infd;
            #else
                const int32_t &infd;
            #endif

            sslSocket->socketDescriptor();
            if (infd != -1)
            {
                int state = cork;
                if (setsockopt(static_cast<int>(infd), IPPROTO_TCP, TCP_CORK, &state, sizeof(state)) != 0)
                    std::cerr << "Unable to apply tcp cork" << std::endl;
            }
        }
        if (tcpSocket != nullptr)
        {
            #if ! defined(EPOLLCATCHCHALLENGERSERVER) && ! defined (ONLYMAPRENDER)
                const qintptr &inf;
            #else
                const int32_t &infd;
            #endif

            tcpSocket->socketDescriptor();
            if (infd != -1)
            {
                int state = cork;
                if (setsockopt(static_cast<int>(infd), IPPROTO_TCP, TCP_CORK, &state, sizeof(state)) != 0)
                    std::cerr << "Unable to apply tcp cork" << std::endl;
            }
        }
    #endif
}

HostAddress ConnectedSocket::localAddress() const
{
    //deprecated form incorrect value for i2p
    std::cerr << "ConnectedSocket::localAddress(): deprecated form incorrect value for i2p" << std::endl;

    //TODO: needs to be changed to a Interface call
    if (sslSocket != nullptr) {
        return sslSocket->localAddress();
    }

    if (tcpSocket != nullptr) {
        return tcpSocket->localAddress();
    }

    if (fakeSocket != nullptr) {
        return HostAddress::localhost;
    }

    return HostAddress::Null;
}

uint16_t ConnectedSocket::localPort() const
{
    //TODO: needs to be changed to a Interface call
    if (sslSocket != nullptr) {
        return sslSocket->localPort();
    }

    if (tcpSocket != nullptr) {
        return tcpSocket->localPort();
    }

    if (fakeSocket != nullptr) {
        return 9999;//??
    }

    return 0;
}

HostAddress ConnectedSocket::peerAddress() const
{
    //deprecated form incorrect value for i2p
    std::cerr << "ConnectedSocket::peerAddress(): deprecated form incorrect value for i2p" << std::endl;

    //TODO: needs to be changed to a Interface call
    if (sslSocket != nullptr) {
        return sslSocket->peerAddress();
    }

    if (tcpSocket != nullptr) {
        return tcpSocket->peerAddress();
    }

    if (fakeSocket != nullptr) {
        return HostAddress::LocalHost;
    }

    return QHostAddress::Null;
}

std::string ConnectedSocket::peerName() const
{
    /// \warning via direct value for i2p. Never pass by peerAddress()
    QString pearName;
    //TODO: needs to be changed to a Interface call
    if (sslSocket != nullptr) {
        pearName = sslSocket->peerName();
    }

    if (tcpSocket != nullptr) {
        pearName = tcpSocket->peerName();
    }

    if (fakeSocket != nullptr) {
        return std::string();
    }

    return (pearName.isEmpty())? hostName : pearName;
}

uint16_t ConnectedSocket::peerPort() const
{
    //TODO: needs to be changed to a Interface call
    if (sslSocket != nullptr) {
        return sslSocket->peerPort();
    }

    if (tcpSocket != nullptr) {
        return tcpSocket->peerPort();
    }

    if (fakeSocket != nullptr) {
        return 15000; //?
    }

    return 0;
}

int ConnectedSocket::state() const
{
    //TODO: needs to be changed to a Interface call
    if (sslSocket != nullptr) {
        return sslSocket->state();
    }

    if (tcpSocket != nullptr) {
        return tcpSocket->state();
    }

    if (fakeSocket != nullptr) {
        return fakeSocket->state();
    }

    return ConnectedSocket::unconnected;
}

bool ConnectedSocket::waitForConnected(int msecs)
{
    //TODO: needs to be changed to a Interface call
    if (sslSocket != nullptr) {
        return sslSocket->waitForConnected(msecs);
    }

    if (tcpSocket != nullptr) {
        return tcpSocket->waitForConnected(msecs);
    }

    if (fakeSocket != nullptr) {
        return true;
    }

    return false;
}

bool ConnectedSocket::waitForDisconnected(int msecs)
{
    //TODO: needs to be changed to a Interface call
    if (sslSocket != nullptr) {
        return sslSocket->waitForDisconnected(msecs);
    }

    if (tcpSocket != nullptr) {
        return tcpSocket->waitForDisconnected(msecs);
    }

    if (fakeSocket != nullptr) {
        return true;
    }

    return false;
}

int64_t ConnectedSocket::bytesAvailable() const
{
    //TODO: needs to be changed to a Interface call
    if (sslSocket != nullptr) {
        return sslSocket->bytesAvailable();
    }

    if (tcpSocket != nullptr) {
        return tcpSocket->bytesAvailable();
    }

    if (fakeSocket != nullptr) {
        return fakeSocket->bytesAvailable();
    }

    return -1;
}

OpenMode ConnectedSocket::openMode() const
{
    //TODO: needs to be changed to a Interface call
    if (sslSocket != nullptr) {
        return sslSocket->openMode();
    }

    if (tcpSocket != nullptr) {
        return tcpSocket->openMode();
    }

    if (fakeSocket != nullptr) {
        return fakeSocket->openMode();
    }

    return 0;
}

std::string ConnectedSocket::errorString() const
{
    //TODO: needs to be changed to a Interface call
    if (sslSocket != nullptr) {
        return sslSocket->errorString();
    }

    if (tcpSocket != nullptr) {
        return tcpSocket->errorString();
    }

    if (fakeSocket != nullptr) {
        return fakeSocket->errorString();
    }

    return std::string();
}

void ConnectedSocket::close()
{
    disconnectFromHost();
}

int64_t ConnectedSocket::readData(char * data, int64_t maxSize)
{
    //TODO: needs to be changed to a Interface call
    if (sslSocket != nullptr) {
        return sslSocket->read(data,maxSize);
    }

    if (tcpSocket != nullptr) {
        return tcpSocket->read(data,maxSize);
    }

    if (fakeSocket != nullptr) {
        return fakeSocket->read(data,maxSize);
    }

    return -1;
}

int64_t ConnectedSocket::writeData(const char * data, int64_t maxSize)
{
    //TODO: needs to be changed to a Interface call
    if (sslSocket != nullptr) {
        return sslSocket->write(data,maxSize);
    }

    if (tcpSocket != nullptr) {
        return tcpSocket->write(data,maxSize);
    }

    if (fakeSocket != nullptr) {
        return fakeSocket->write(data,maxSize);
    }

    return -1;
}

bool ConnectedSocket::isSequential() const
{
    return true;
}

bool ConnectedSocket::canReadLine () const
{
    return false;
}

#endif

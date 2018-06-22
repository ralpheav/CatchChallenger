#if ! defined(EPOLLCATCHCHALLENGERSERVER) && ! defined (ONLYMAPRENDER)

#include "ConnectedSocket.h"

using namespace CatchChallenger;

std::string HostAddress::localhost = "127.0.0.1";
std::string HostAddress::Null      = "0.0.0.0";

ConnectedSocket::ConnectedSocket(ISocket* socket) : pSocket(socket)
{
    pSocket->setSocketOption(SocketOption::KeepAliveOption, 1);
    purgeBuffer();
    //open(QIODevice::ReadWrite | QIODevice::Unbuffered);
}

ConnectedSocket::~ConnectedSocket()
{
    pSocket->deleteLater();
}

//std::list<QSslError> ConnectedSocket::sslErrors() const
//{
//    return pSocket->sslErrors();
//}

void ConnectedSocket::purgeBuffer()
{
    if (pSocket->bytesAvailable()) {
         readyRead();
    }

    if (pSocket->encryptedBytesAvailable()) {
        readyRead();
    }
}

void ConnectedSocket::destroyedSocket()
{
    pSocket = nullptr;
    hostName.clear();
    port = 0;
}

void ConnectedSocket::abort()
{
    hostName.clear();
    port = 0;
    pSocket->abort();
}

void ConnectedSocket::connectToHost(const std::string& hostName, uint16_t port)
{
    if (state() != State::unconnected) {
        return;
    }

    //workaround because QSslSocket don't return correct value for i2p via proxy
    this->hostName = hostName;
    this->port = port;

    if (pSocket != nullptr) {
        pSocket->connectToHost(hostName, port);
    }
}

void ConnectedSocket::connectToHost(const HostAddress& address, uint16_t port)
{
    if (state() != State::unconnected) {
        return;
    }

    pSocket->connectToHost(address.toString(), port);

}

void ConnectedSocket::disconnectFromHost()
{
    if (state() != State::unconnected) {
        return;
    }

    hostName.clear();
    port = 0;

    pSocket->disconnectFromHost();
}

int ConnectedSocket::error() const
{
    pSocket->error();

    return 0;//unknown
}

bool ConnectedSocket::flush()
{
    if (pSocket) {
        return pSocket->flush();
    }

    return false;
}

bool ConnectedSocket::isValid() const
{
    if (pSocket) {
        return pSocket->isValid();
    }

    return false;
}

void ConnectedSocket::setTcpCork(const bool& cork)
{
    #ifdef __linux__
        #if ! defined(EPOLLCATCHCHALLENGERSERVER) && ! defined (ONLYMAPRENDER)
            const int* &infd;
        #else
            const int32_t& infd;
        #endif

        pSocket->socketDescriptor();
        if (infd != -1)
        {
            int state = cork;
            if (setsockopt(static_cast<int>(infd), IPPROTO_TCP, TCP_CORK, &state, sizeof(state)) != 0) {
                std::cerr << "Unable to apply tcp cork" << std::endl;
            }
        }
    #endif
}

HostAddress ConnectedSocket::localAddress() const
{
    //deprecated form incorrect value for i2p
    std::cerr << "ConnectedSocket::localAddress(): deprecated form incorrect value for i2p" << std::endl;

    if (pSocket != nullptr) {
        return pSocket->localAddress();
    }

    return HostAddress::Null;
}

uint16_t ConnectedSocket::localPort() const
{
    if (pSocket != nullptr) {
        return pSocket->localPort();
    }

    return 0;
}

HostAddress ConnectedSocket::peerAddress() const
{
    //deprecated form incorrect value for i2p
    std::cerr << "ConnectedSocket::peerAddress(): deprecated form incorrect value for i2p" << std::endl;

    if (pSocket != nullptr) {
        return pSocket->peerAddress();
    }

    return HostAddress::Null;
}

std::string ConnectedSocket::peerName() const
{
    /// \warning via direct value for i2p. Never pass by peerAddress()
    std::string pearName;

    if (pSocket != nullptr) {
        pearName = pSocket->peerName();
    }

    return (pearName.isEmpty())? hostName : pearName;
}

uint16_t ConnectedSocket::peerPort() const
{
    if (pSocket != nullptr) {
        return pSocket->peerPort();
    }

    return 0;
}

int ConnectedSocket::state() const
{
    if (pSocket != nullptr) {
        return pSocket->state();
    }

    return State::unconnected;
}

bool ConnectedSocket::waitForConnected(int msecs)
{
    if (pSocket != nullptr) {
        return pSocket->waitForConnected(msecs);
    }

    return false;
}

bool ConnectedSocket::waitForDisconnected(int msecs)
{
    if (pSocket != nullptr) {
        return pSocket->waitForDisconnected(msecs);
    }

    return false;
}

int64_t ConnectedSocket::bytesAvailable() const
{
    if (pSocket != nullptr) {
        return pSocket->bytesAvailable();
    }

    return -1;
}

OpenMode ConnectedSocket::openMode() const
{
    if (pSocket != nullptr) {
        return pSocket->openMode();
    }

    return 0;
}

std::string ConnectedSocket::errorString() const
{
    if (pSocket != nullptr) {
        return pSocket->errorString();
    }

    return std::string();
}

void ConnectedSocket::close()
{
    disconnectFromHost();
}

int64_t ConnectedSocket::readData(char* data, int64_t maxSize)
{
    if (pSocket != nullptr) {
        return pSocket->readData(data, maxSize);
    }

    return -1;
}

int64_t ConnectedSocket::writeData(const char* data, int64_t maxSize)
{
    if (pSocket != nullptr) {
        return pSocket->writeData(data, maxSize);
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

#endif  // ! defined(EPOLLCATCHCHALLENGERSERVER) && ! defined (ONLYMAPRENDER)

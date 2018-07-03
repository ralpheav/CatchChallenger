#if ! defined(EPOLLCATCHCHALLENGERSERVER) && ! defined (ONLYMAPRENDER)

#include "ConnectedSocket.h"

using namespace CatchChallenger;

ConnectedSocket::ConnectedSocket(ISocket* socket) : pSocket(socket)
{
    if (pSocket) {
        pSocket->setSocketOption(SocketOption::KeepAliveOption, 1);
        purgeBuffer();
        pSocket->open(DeviceMode::ReadWrite);//may it is going to be removed
    }
}

ConnectedSocket::~ConnectedSocket()
{
    //TODO: trigger the delete_later event
    //deleteLater();
}

SSLSocket* ConnectedSocket::getSSLSocket() const {
    if (typeid(pSocket).name() == "SSLScket") {
        return reinterpret_cast<SSLSocket *>(pSocket);
    }

    return new SSLSocket();
}

bool ConnectedSocket::isSSL() {
    return typeid(pSocket).name() == "SSLScket";
}

bool ConnectedSocket::isFake() {
    return typeid(pSocket).name() == "FakeSocket";
}

bool ConnectedSocket::isTCP() {
    return typeid(pSocket).name() == "TcpSocket";
}

bool ConnectedSocket::exists() {
    return pSocket;
}

std::list<SslError> ConnectedSocket::sslErrors() const
{
    return this->getSSLSocket()->sslErrors();
}

void ConnectedSocket::purgeBuffer()
{
    if (pSocket->bytesAvailable()) {
        //TODO:  call event ready Read
         //readyRead();
    }

    if (this->getSSLSocket()->encryptedBytesAvailable()) {
        //TODO:  call event ready Read
        //readyRead();
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
        pSocket->flush();
        return true;
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
            const int infd =
            //TODO: WARNING..........What is this??
        #else
            const int32_t infd =
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
        HostAddress host;
        host.setAddress(pSocket->localAddress());
        return host;
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
        HostAddress host;
        host.setAddress(pSocket->peerAddress());
        return host;
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

    return (pearName.empty())? hostName : pearName;
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
        if (pSocket->openMode()) {
            return OpenMode::open;
        }
    }

    return OpenMode::unknown;
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

bool ConnectedSocket::canReadLine() const
{
    return false;
}

#endif  // ! defined(EPOLLCATCHCHALLENGERSERVER) && ! defined (ONLYMAPRENDER)

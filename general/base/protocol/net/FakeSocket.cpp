#if ! defined(EPOLLCATCHCHALLENGERSERVER) && ! defined (ONLYMAPRENDER)
#include "FakeSocket.h"

using namespace CatchChallenger;

FakeSocket::FakeSocket()
{
    theOtherSocket = nullptr;
    RX_size        = 0;
}

FakeSocket::~FakeSocket()
{
    if (theOtherSocket != nullptr)
    {
        FakeSocket* tempOtherSocket = theOtherSocket;
        theOtherSocket = nullptr;
        tempOtherSocket->theOtherSocket = nullptr;
        tempOtherSocket->m_state = SocketState::UnconnectedState;
        //TODO: HOW the event disconected is going to be trigger
        //tempOtherSocket->disconnected();
    }

    //TODO: HOW the event disconected is going to be trigger
    //aboutToDelete();
}

void FakeSocket::open(DeviceMode mode) {
    //TODO write read file
    m_mode = mode;
}

int FakeSocket::socketDescriptor() {
    //TODO get the descriptor faked
    return 0;
}

void FakeSocket::abort()
{
    #ifdef FAKESOCKETDEBUG
        Logger::instance().log(Logger::Debug, std::stringLiteral("FakeSocket::abort()"));
    #endif

    disconnectFromHost();
}

void FakeSocket::disconnectFromHost()
{
    if (theOtherSocket == nullptr) {
        return;
    }

    #ifdef FAKESOCKETDEBUG
        Logger::instance().log(Logger::Debug, std::stringLiteral("FakeSocket::disconnectFromHost()"));
    #endif

    FakeSocket* tempOtherSocket = theOtherSocket;
    theOtherSocket = nullptr;
    tempOtherSocket->disconnectFromHost();
    {
        std::lock_guard<std::mutex> guard(FakeSocket::mutex);
        data.clear();
    }
    m_state = SocketState::UnconnectedState;

    //TODO:  disconected event
    //disconnected();
}

void FakeSocket::disconnectFromFakeServer()
{
    if (theOtherSocket == nullptr) {
        return;
    }

    #ifdef FAKESOCKETDEBUG
        Logger::instance().log(Logger::Debug, std::stringLiteral("FakeSocket::disconnectFromFakeServer()"));
    #endif

    theOtherSocket = nullptr;
    {
        std::lock_guard<std::mutex> guard(FakeSocket::mutex);
        data.clear();
    }
    m_state = SocketState::UnconnectedState;

    //TODO: event disconnected
    //disconnected();
}

void FakeSocket::connectToHost(const std::string& host, int port)
{
    #ifdef FAKESOCKETDEBUG
        Logger::instance().log(Logger::Debug, std::stringLiteral("FakeSocket::connectToHost()"));
    #endif

    if (theOtherSocket != NULL) {
        return;
    }

    FakeServer::server.addPendingConnection(this);
    m_state = SocketState::ConnectedState;

    //TODO: connected event
    //connected();
}

int64_t FakeSocket::bytesAvailableWithMutex()
{
    int64_t size;
    {
        std::lock_guard<std::mutex> guard(FakeSocket::mutex);

        #ifdef FAKESOCKETDEBUG
            Logger::instance().log(Logger::Debug, std::stringLiteral("bytesAvailable(): data.size(): %1").arg(data.size()));
        #endif

        size = data.size();
    }

    return size;
}

int64_t FakeSocket::bytesAvailable() const
{
    return const_cast<FakeSocket *>(this)->bytesAvailableWithMutex();
}

void FakeSocket::close()
{
    disconnectFromHost();
}

bool FakeSocket::isValid() const
{
    return theOtherSocket != nullptr;
}

FakeSocket * FakeSocket::getTheOtherSocket()
{
    return theOtherSocket;
}

uint64_t FakeSocket::getRXSize()
{
    return RX_size;
}

uint64_t FakeSocket::getTXSize()
{
    if (theOtherSocket == nullptr) {
        return 0;
    }

    return theOtherSocket->getRXSize();
}

void FakeSocket::state(SocketState socketState) {
    m_state = socketState;
}

SocketState FakeSocket::state() const
{
    return m_state;
}

SocketState FakeSocket::updateState()
{
    if (theOtherSocket == NULL) {
        m_state = SocketState::ConnectedState;
    }
    m_state = SocketState::UnconnectedState;

    return m_state;
}

uint64_t FakeSocket::readData(char* rawData, int64_t maxSize)
{
    std::lock_guard<std::mutex> guard(FakeSocket::mutex);

    std::vector<unsigned char> extractedData;
    std::vector<unsigned char>::iterator it = this->data.begin();
    for(int index = 0; it != this->data.end() && index < maxSize; it++, index++) {
        extractedData.push_back(*it);
    }

    #ifdef FAKESOCKETDEBUG
        Logger::instance().log(Logger::Debug, std::stringLiteral("readData(): extractedData.size(): %1, data.size(): %2, extractedData: %3").arg(extractedData.size()).arg(data.size()).arg(std::string(extractedData.toHex())));
    #endif

    rawData = new char[extractedData.size()];
    memcpy(rawData, &(extractedData[0]), sizeof(extractedData[0]) * extractedData.size());
    this->data.erase(this->data.begin(), it);

    return extractedData.size();
}

uint64_t FakeSocket::writeData(const char* rawData, int64_t size)
{
    if (theOtherSocket == nullptr)
    {
        Logger::instance().log(Logger::Debug, "writeData(): theOtherSocket==NULL");

        return size;
    }
    std::vector<unsigned char> dataToSend;
    {
        std::lock_guard<std::mutex> guard(FakeSocket::mutex);

        #ifdef FAKESOCKETDEBUG
            Logger::instance().log(Logger::Debug, std::stringLiteral("writeData(): size: %1").arg(size));
        #endif
        for(int index = 0; rawData[index] != 0; index++) {
            dataToSend.push_back(rawData[index]);
        }
    }
    theOtherSocket->internal_writeData(dataToSend);

    return size;
}

void FakeSocket::internal_writeData(std::vector<unsigned char> rawData)
{
    {
        std::lock_guard<std::mutex> guard(FakeSocket::mutex);

        #ifdef FAKESOCKETDEBUG
            Logger::instance().log(Logger::Debug, std::stringLiteral("internal_writeData(): size: %1").arg(rawData.size()));
        #endif

        RX_size += rawData.size();

        if (rawData.size() == this->data.size()) {
            std::vector<unsigned char>::iterator itRawData = rawData.begin();
            std::vector<unsigned char>::iterator itData    = this->data.begin();
            for(; itRawData != rawData.end(); itRawData++, itData++) {
                *itData += *itRawData;
            }
        }
    }
    //TODO:  event readyRead
    //readyRead();
}

void FakeSocket::flush() {
    //TODO
    //may not nnecesarrily
}

std::string FakeSocket::localAddress() {
    return std::string("0.0.0.0");
}

int FakeSocket::localPort() {
    return 80;
}

std::string FakeSocket::peerAddress() {
    return std::string("0.0.0.0");
}

std::string FakeSocket::peerName() {
    return std::string();
}

int FakeSocket::peerPort() {
    return 0;
}

std::string FakeSocket::errorString() {
    //TODO
    return std::string();
}

bool FakeSocket::isSequential() const
{
    return true;
}

bool FakeSocket::canReadLine() const
{
    return false;
}

bool FakeSocket::openMode() {
    return false;
}

#endif // #if ! defined(EPOLLCATCHCHALLENGERSERVER) && ! defined (ONLYMAPRENDER)

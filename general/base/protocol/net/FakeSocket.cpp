#if ! defined(EPOLLCATCHCHALLENGERSERVER) && ! defined (ONLYMAPRENDER)
#include "FakeSocket.h"

using namespace CatchChallenger;

FakeSocket::FakeSocket()
{
    theOtherSocket = nullptr;
    RX_size        = 0;
    //open(QIODevice::ReadWrite | QIODevice::Unbuffered);
}

FakeSocket::~FakeSocket()
{
    if (theOtherSocket != nullptr)
    {
        FakeSocket *tempOtherSocket = theOtherSocket;
        theOtherSocket = nullptr;
        tempOtherSocket->theOtherSocket = nullptr;
        tempOtherSocket->m_state = FakeSocket::unconnected;;
        tempOtherSocket->disconnected();
    }

    aboutToDelete();
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

    FakeSocket *tempOtherSocket = theOtherSocket;
    theOtherSocket = nullptr;
    tempOtherSocket->disconnectFromHost();
    {
        lock_guard<mutex> guard(FakeSocket::mutex);
        data.clear();
    }
    m_state = FakeSocket::unconnected;

    disconnected();
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
        lock_guard<mutex> guard(FakeSocket::mutex);
        data.clear();
    }
    m_state = FakeSocket::unconnected;

    disconnected();
}

void FakeSocket::connectToHost()
{
    #ifdef FAKESOCKETDEBUG
        Logger::instance().log(Logger::Debug, std::stringLiteral("FakeSocket::connectToHost()"));
    #endif
    if (theOtherSocket != NULL) {
        return;
    }

    FakeServer::server.addPendingConnection(this);
    m_state = FakeSocket::connected;

    connected();
}

int64_t FakeSocket::bytesAvailableWithMutex()
{
    int64_t size;
    {
        lock_guard<mutex> guard(FakeSocket::mutex);

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

bool FakeSocket::isValid()
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

int FakeSocket::error() const {
    return 0;
}

int FakeSocket::state() const
{
    if (theOtherSocket == NULL) {
        return FakeSocket::connected;
    }
    return FakeSocket::unconnected;
}

int64_t FakeSocket::readData(char* rawData, int64_t maxSize)
{
    lock_guard<mutex> guard(FakeSocket::mutex);

    std::vector<unsigned char> extractedData;
    std::vector<unsigned char>::iterator it;
    for(int index=0, it = this->data.begin(); it != this->data.end() && index < maxSize; it++, index++) {
        extractedData.push_back(it->first);
    }

    #ifdef FAKESOCKETDEBUG
        Logger::instance().log(Logger::Debug, std::stringLiteral("readData(): extractedData.size(): %1, data.size(): %2, extractedData: %3").arg(extractedData.size()).arg(data.size()).arg(std::string(extractedData.toHex())));
    #endif

    rawData = new char[extractedData.size()];
    memcpy(rawData, &(extractedData[0]), sizeof(extractedData[0]) * extractedData.size());
    this->data.erase(this->data.begin(), it);

    return extractedData.size();
}

int64_t FakeSocket::writeData(const char* rawData, int64_t size)
{
    if (theOtherSocket == nullptr)
    {
        Logger::instance().log(Logger::Debug, "writeData(): theOtherSocket==NULL");

        return size;
    }
    std::vector<unsigned char> dataToSend;
    {
        lock_guard<mutex> guard(FakeSocket::mutex);

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
        lock_guard<mutex> guard(FakeSocket::mutex);

        #ifdef FAKESOCKETDEBUG
            Logger::instance().log(Logger::Debug, std::stringLiteral("internal_writeData(): size: %1").arg(rawData.size()));
        #endif

        RX_size += rawData.size();
        this->data += rawData;
    }

    readyRead();
}

bool FakeSocket::isSequential() const
{
    return true;
}

bool FakeSocket::canReadLine () const
{
    return false;
}

#endif

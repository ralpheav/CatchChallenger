#if ! defined(EPOLLCATCHCHALLENGERSERVER) && ! defined (ONLYMAPRENDER)
#include "FakeSocket.h"

using namespace CatchChallenger;

FakeSocket::FakeSocket()
{
    theOtherSocket = NULL;
    RX_size        = 0;
    //open(QIODevice::ReadWrite | QIODevice::Unbuffered);
}

FakeSocket::~FakeSocket()
{
    if(theOtherSocket != NULL)
    {
        FakeSocket *tempOtherSocket = theOtherSocket;
        theOtherSocket = NULL;
        tempOtherSocket->theOtherSocket = NULL;
        //tempOtherSocket->state = QAbstractSocket::UnconnectedState;
        //tempOtherSocket->disconnected();
    }
    //emit aboutToDelete();//OWN
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
    if(theOtherSocket == NULL) {
        return;
    }

    #ifdef FAKESOCKETDEBUG
        Logger::instance().log(Logger::Debug, std::stringLiteral("FakeSocket::disconnectFromHost()"));
    #endif

    FakeSocket *tempOtherSocket = theOtherSocket;
    theOtherSocket = NULL;
    tempOtherSocket->disconnectFromHost();
    {
        lock_guard<mutex> guard(FakeSocket::mutex);
        data.clear();
    }
    //tempOtherSocket->state = QAbstractSocket::UnconnectedState;
    //emit stateChanged(QAbstractSocket::UnconnectedState);//own
    //emit disconnected();//own
}

void FakeSocket::disconnectFromFakeServer()
{
    if(theOtherSocket == NULL) {
        return;
    }

    #ifdef FAKESOCKETDEBUG
        Logger::instance().log(Logger::Debug, std::stringLiteral("FakeSocket::disconnectFromFakeServer()"));
    #endif

    theOtherSocket = NULL;
    {
        lock_guard<mutex> guard(FakeSocket::mutex);
        data.clear();
    }
    //emit stateChanged(QAbstractSocket::UnconnectedState);
    //emit disconnected();
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
    //emit stateChanged(QAbstractSocket::ConnectedState);
    //emit connected();
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
    return theOtherSocket != NULL;
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
    if (theOtherSocket == NULL) {
        return 0;
    }

    return theOtherSocket->getRXSize();
}

/*QAbstractSocket::SocketError FakeSocket::error() const
{
    return QAbstractSocket::UnknownSocketError;
}

QAbstractSocket::SocketState FakeSocket::state() const
{
    if (theOtherSocket == NULL) {
        return QAbstractSocket::UnconnectedState;
    } else {
        return QAbstractSocket::ConnectedState;
    }
}*/

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

    memcpy(rawData, extractedData.data(), extractedData.size());
    //this->data.remove(0, extractedData.size());
    this->data.erase(this->data.begin(), it);

    return extractedData.size();
}

int64_t FakeSocket::writeData(const char* rawData, int64_t size)
{
    if (theOtherSocket == NULL)
    {
        Logger::instance().log(Logger::Debug, "writeData(): theOtherSocket==NULL");
        //emit error(QAbstractSocket::NetworkError);

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
        //dataToSend=QByteArray(rawData,static_cast<int>(size));
    }
    theOtherSocket->internal_writeData(dataToSend);

    return size;
}

void FakeSocket::internal_writeData(std::vector<unsigned char>/*QByteArray*/ rawData)
{
    {
        lock_guard<mutex> guard(FakeSocket::mutex);

        #ifdef FAKESOCKETDEBUG
            Logger::instance().log(Logger::Debug, std::stringLiteral("internal_writeData(): size: %1").arg(rawData.size()));
        #endif

        RX_size += rawData.size();
        this->data += rawData;
    }

    //emit readyRead();
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

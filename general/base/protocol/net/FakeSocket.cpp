#if ! defined(EPOLLCATCHCHALLENGERSERVER) && ! defined (ONLYMAPRENDER)
#include "FakeSocket.h"

using namespace CatchChallenger;

FakeSocket::FakeSocket()
{
    theOtherSocket = NULL;
    RX_size        = 0;
    open(QIODevice::ReadWrite | QIODevice::Unbuffered);
}

FakeSocket::~FakeSocket()
{
    if(theOtherSocket != NULL)
    {
        FakeSocket *tempOtherSocket = theOtherSocket;
        theOtherSocket = NULL;
        tempOtherSocket->theOtherSocket = NULL;
        tempOtherSocket->stateChanged(QAbstractSocket::UnconnectedState);
        tempOtherSocket->disconnected();
    }
    emit aboutToDelete();
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
    if(theOtherSocket==NULL)
        return;
    #ifdef FAKESOCKETDEBUG
        Logger::instance().log(Logger::Debug, std::stringLiteral("FakeSocket::disconnectFromHost()"));
    #endif
    FakeSocket *tempOtherSocket=theOtherSocket;
    theOtherSocket=NULL;
    tempOtherSocket->disconnectFromHost();
    {
        QMutexLocker lock(&mutex);
        data.clear();
    }
    emit stateChanged(QAbstractSocket::UnconnectedState);
    emit disconnected();
}

void FakeSocket::disconnectFromFakeServer()
{
    if(theOtherSocket==NULL)
        return;
    #ifdef FAKESOCKETDEBUG
        Logger::instance().log(Logger::Debug, std::stringLiteral("FakeSocket::disconnectFromFakeServer()"));
    #endif
    theOtherSocket=NULL;
    {
        QMutexLocker lock(&mutex);
        data.clear();
    }
    emit stateChanged(QAbstractSocket::UnconnectedState);
    emit disconnected();
}

void FakeSocket::connectToHost()
{
    #ifdef FAKESOCKETDEBUG
        Logger::instance().log(Logger::Debug, std::stringLiteral("FakeSocket::connectToHost()"));
    #endif
    if(theOtherSocket!=NULL)
        return;
    QFakeServer::server.addPendingConnection(this);
    emit stateChanged(QAbstractSocket::ConnectedState);
    emit connected();
}

int64_t FakeSocket::bytesAvailableWithMutex()
{
    int64_t size;
    {
        QMutexLocker lock(&mutex);
        #ifdef FAKESOCKETDEBUG
            Logger::instance().log(Logger::Debug, std::stringLiteral("bytesAvailable(): data.size(): %1").arg(data.size()));
        #endif
        size=data.size();
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
    return theOtherSocket!=NULL;
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

QAbstractSocket::SocketError FakeSocket::error() const
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
}

int64_t FakeSocket::readData(char * rawData, int64_t maxSize)
{
    QMutexLocker lock(&mutex);
    QByteArray extractedData = this->data.mid(0, static_cast<int>(maxSize));

    #ifdef FAKESOCKETDEBUG
        Logger::instance().log(Logger::Debug, std::stringLiteral("readData(): extractedData.size(): %1, data.size(): %2, extractedData: %3").arg(extractedData.size()).arg(data.size()).arg(std::string(extractedData.toHex())));
    #endif

    memcpy(rawData,extractedData.data(), extractedData.size());
    this->data.remove(0, extractedData.size());

    return extractedData.size();
}

int64_t FakeSocket::writeData(const char * rawData, int64_t size)
{
    if (theOtherSocket == NULL)
    {
        Logger::instance().log(Logger::Debug, "writeData(): theOtherSocket==NULL");
        emit error(QAbstractSocket::NetworkError);

        return size;
    }
    QByteArray dataToSend;
    {
        QMutexLocker lock(&mutex);

        #ifdef FAKESOCKETDEBUG
            Logger::instance().log(Logger::Debug, std::stringLiteral("writeData(): size: %1").arg(size));
        #endif

        dataToSend=QByteArray(rawData,static_cast<int>(size));
    }
    theOtherSocket->internal_writeData(dataToSend);

    return size;
}

void FakeSocket::internal_writeData(QByteArray rawData)
{
    {
        QMutexLocker lock(&mutex);

        #ifdef FAKESOCKETDEBUG
            Logger::instance().log(Logger::Debug, std::stringLiteral("internal_writeData(): size: %1").arg(rawData.size()));
        #endif

        RX_size += rawData.size();
        this->data += rawData;
    }

    emit readyRead();
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

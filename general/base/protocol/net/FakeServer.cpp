#if ! defined(EPOLLCATCHCHALLENGERSERVER) && ! defined (ONLYMAPRENDER)

#include "FakeServer.h"

using namespace CatchChallenger;

FakeServer FakeServer::server;

FakeServer::FakeServer()
{
    m_isListening = false;
}

void FakeServer::addPendingConnection(FakeSocket* socket)
{
    std::pair<FakeSocket* ,FakeSocket*> newEntry;
    newEntry.first = socket;
    newEntry.second = new FakeSocket();
    {
        std::lock_guard<std::mutex> locker(mutex);
        m_listOfConnexion.push_back(newEntry);
        m_pendingConnection.push_back(newEntry);
    }
    newEntry.first->theOtherSocket = newEntry.second;
    newEntry.second->theOtherSocket = newEntry.first;
    newEntry.first->RX_size = 0;
    newEntry.second->RX_size = 0;

//    connect(newEntry.first, &QFakeSocket::disconnected, this, &FakeServer::disconnectedSocket, Qt::DirectConnection);
//    connect(newEntry.second,&QFakeSocket::disconnected, this, &FakeServer::disconnectedSocket, Qt::DirectConnection);
//    connect(newEntry.first, &QFakeSocket::aboutToDelete,this, &FakeServer::disconnectedSocket, Qt::DirectConnection);
//    connect(newEntry.second,&QFakeSocket::aboutToDelete,this, &FakeServer::disconnectedSocket, Qt::DirectConnection);
//    connect(newEntry.first, &QFakeSocket::destroyed,    this, &FakeServer::disconnectedSocket, Qt::DirectConnection);
//    connect(newEntry.second,&QFakeSocket::destroyed,    this, &FakeServer::disconnectedSocket, Qt::DirectConnection);

    //emit newConnection();
}

bool FakeServer::hasPendingConnections()
{
    std::lock_guard<std::mutex> locker(mutex);
    return m_pendingConnection.size();
}

bool FakeServer::isListening() const
{
    return m_isListening;
}

bool FakeServer::listen()
{
    m_isListening = true;
    return m_isListening;
}

FakeSocket* FakeServer::nextPendingConnection()
{
    std::lock_guard<std::mutex> locker(mutex);
    FakeSocket* socket = m_pendingConnection.begin()->second;
    m_pendingConnection.pop_front();

    return socket;
}

void FakeServer::close()
{
    m_isListening = false;
    std::lock_guard<std::mutex> locker(mutex);

    std::list<std::pair<FakeSocket*, FakeSocket*>>::iterator it;
    for(it = m_listOfConnexion.begin(); it != m_listOfConnexion.end(); it++) {
        it->first->disconnectFromHost();
    }
}

void FakeServer::disconnectedSocket()
{
    std::lock_guard<std::mutex> locker(mutex);
    FakeSocket* socket = null;//qobject_cast<FakeSocket*>(QObject::sender());
    {
        int index = 0;
        int loop_size = m_listOfConnexion.size();
        std::list<std::pair<FakeSocket*, FakeSocket*>>::iterator it = m_listOfConnexion.begin();

        while(it != m_listOfConnexion.end())
        {
            if (it->first->theOtherSocket == socket || it->second->theOtherSocket == socket)
            {
                it->first->disconnectFromFakeServer();
                it->second->disconnectFromFakeServer();
                m_listOfConnexion.erase(it);
                loop_size--;
            } else {
                index++;
            }
        }
        loop_size = m_pendingConnection.size();
        it = m_listOfConnexion.begin();
        while(it != m_listOfConnexion.end())
        {
            if(it->first->theOtherSocket == socket || it->second->theOtherSocket == socket)
            {
                it->first->disconnectFromFakeServer();
                it->second->disconnectFromFakeServer();
                m_pendingConnection.erase(it);
                loop_size--;
            } else {
                index++;
            }
        }
    }

    //drop the NULL co
    {
        int index = 0;
        int loop_size = m_listOfConnexion.size();
        it = m_listOfConnexion.begin();
        while(it != m_listOfConnexion.end())
        {
            if(it->first->theOtherSocket == NULL || it->second->theOtherSocket == NULL)
            {
                m_listOfConnexion.erase(it);
                loop_size--;
            } else {
                index++;
            }
        }
        loop_size = m_pendingConnection.size();
        it = m_listOfConnexion.begin();
        while(index < loop_size)
        {
            if(it->first->theOtherSocket == NULL || it->second->theOtherSocket == NULL)
            {
                m_pendingConnection.erase(it);
                loop_size--;
            } else {
                index++;
            }
        }
    }
}

#endif

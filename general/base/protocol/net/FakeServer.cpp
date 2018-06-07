#if ! defined(EPOLLCATCHCHALLENGERSERVER) && ! defined (ONLYMAPRENDER)

#include "FakeServer.h"

using namespace CatchChallenger;

FakeServer FakeServer::server;

FakeServer::FakeServer()
{
    m_isListening=false;
}

void FakeServer::addPendingConnection(FakeSocket * socket)
{
    QPair<FakeSocket *,FakeSocket *> newEntry;
    newEntry.first = socket;
    newEntry.second = new FakeSocket();
    {
        QMutexLocker locker(&mutex);
        m_listOfConnexion.push_back(newEntry);
        m_pendingConnection.push_back(newEntry);
    }
    newEntry.first->theOtherSocket = newEntry.second;
    newEntry.second->theOtherSocket = newEntry.first;
    newEntry.first->RX_size = 0;
    newEntry.second->RX_size = 0;

    connect(newEntry.first,&QFakeSocket::disconnected,this,&FakeServer::disconnectedSocket,Qt::DirectConnection);
    connect(newEntry.second,&QFakeSocket::disconnected,this,&FakeServer::disconnectedSocket,Qt::DirectConnection);
    connect(newEntry.first,&QFakeSocket::aboutToDelete,this,&FakeServer::disconnectedSocket,Qt::DirectConnection);
    connect(newEntry.second,&QFakeSocket::aboutToDelete,this,&FakeServer::disconnectedSocket,Qt::DirectConnection);
    connect(newEntry.first,&QFakeSocket::destroyed,this,&FakeServer::disconnectedSocket,Qt::DirectConnection);
    connect(newEntry.second,&QFakeSocket::destroyed,this,&FakeServer::disconnectedSocket,Qt::DirectConnection);

    emit newConnection();
}

bool FakeServer::hasPendingConnections()
{
    QMutexLocker locker(&mutex);
    return m_pendingConnection.size()>0;
}

bool FakeServer::isListening() const
{
    return m_isListening;
}

bool FakeServer::listen()
{
    m_isListening = true;
    return true;
}

FakeSocket* FakeServer::nextPendingConnection()
{
    QMutexLocker locker(&mutex);
    FakeSocket* socket = m_pendingConnection.first().second;
    m_pendingConnection.removeFirst();
    return socket;
}

void FakeServer::close()
{
    m_isListening = false;
    QMutexLocker locker(&mutex);
    int loop_size = m_listOfConnexion.size();

    while(loop_size)
    {
        m_listOfConnexion.first().first->disconnectFromHost();
        loop_size--;
    }
}

void FakeServer::disconnectedSocket()
{
    QMutexLocker locker(&mutex);
    FakeSocket* socket = qobject_cast<FakeSocket*>(QObject::sender());
    {
        int index = 0;
        int loop_size = m_listOfConnexion.size();
        while(index < loop_size)
        {
            if(m_listOfConnexion.at(index).first->theOtherSocket==socket || m_listOfConnexion.at(index).second->theOtherSocket==socket)
            {
                m_listOfConnexion.at(index).first->disconnectFromFakeServer();
                m_listOfConnexion.at(index).second->disconnectFromFakeServer();
                m_listOfConnexion.removeAt(index);
                loop_size--;
            }
            else
                index++;
        }
        loop_size=m_pendingConnection.size();
        while(index<loop_size)
        {
            if(m_pendingConnection.at(index).first->theOtherSocket==socket || m_pendingConnection.at(index).second->theOtherSocket==socket)
            {
                m_pendingConnection.at(index).first->disconnectFromFakeServer();
                m_pendingConnection.at(index).second->disconnectFromFakeServer();
                m_pendingConnection.removeAt(index);
                loop_size--;
            }
            else
                index++;
        }
    }

    //drop the NULL co
    {
        int index=0;
        int loop_size=m_listOfConnexion.size();
        while(index<loop_size)
        {
            if(m_listOfConnexion.at(index).first->theOtherSocket==NULL || m_listOfConnexion.at(index).second->theOtherSocket==NULL)
            {
                m_listOfConnexion.removeAt(index);
                loop_size--;
            }
            else
                index++;
        }
        loop_size=m_pendingConnection.size();
        while(index<loop_size)
        {
            if(m_pendingConnection.at(index).first->theOtherSocket==NULL || m_pendingConnection.at(index).second->theOtherSocket==NULL)
            {
                m_pendingConnection.removeAt(index);
                loop_size--;
            }
            else
                index++;
        }
    }
}

#endif

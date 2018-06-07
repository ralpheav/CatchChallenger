#if ! defined(EPOLLCATCHCHALLENGERSERVER) && ! defined (ONLYMAPRENDER)

#ifndef CATCHCHALLENGER_FAKESERVER_H
#define CATCHCHALLENGER_FAKESERVER_H

#include <QObject>
#include <QPair>
#include <QList>
#include <QMutex>
#include <QMutexLocker>
#include <QHostAddress>

#include "FakeSocket.h"

#include <vector>

namespace CatchChallenger {

    class FakeSocket;

    class FakeServer : public QObject
    {
        private:
            explicit FakeServer();
        public:
            friend class FakeSocket;
            static FakeServer server;

            virtual bool hasPendingConnections();
            bool isListening() const;
            bool listen();
            virtual FakeSocket* nextPendingConnection();
            void close();
        signals:
            void newConnection();
        private:
            QMutex mutex;
            bool m_isListening;
            QList<QPair<FakeSocket*, FakeSocket*> > m_listOfConnexion;
            QList<QPair<FakeSocket*, FakeSocket*> > m_pendingConnection;
        protected:
            //from the server
            void addPendingConnection(FakeSocket* socket);
        public slots:
            void disconnectedSocket();
    };
}

#endif // FAKESERVER_H
#endif

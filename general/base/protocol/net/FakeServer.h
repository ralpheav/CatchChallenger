#if ! defined(EPOLLCATCHCHALLENGERSERVER) && ! defined (ONLYMAPRENDER)

#ifndef CATCHCHALLENGER_FAKESERVER_H
#define CATCHCHALLENGER_FAKESERVER_H

#include "FakeSocket.h"
#include <vector>
#include <list>
#include <utility>
#include <mutex>

namespace CatchChallenger {

    class FakeSocket;

    class FakeServer
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
            std::mutex mutex;
            bool m_isListening;
            std::list<std::pair<FakeSocket*, FakeSocket*>> m_listOfConnexion;
            std::list<std::pair<FakeSocket*, FakeSocket*>> m_pendingConnection;
        protected:
            //from the server
            void addPendingConnection(FakeSocket* socket);
        public slots:
            void disconnectedSocket();
    };
}

#endif // FAKESERVER_H
#endif

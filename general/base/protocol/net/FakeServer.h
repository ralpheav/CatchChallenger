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
            //virtual void newConnection() = 0;
            //virtual void disconnected()  = 0;
            //virtual void aboutToDelete() = 0;
            //virtual void destroyed()     = 0;
        private:
            std::mutex mutex;
            bool m_isListening;
            enum {
                DISCONNECTED = 0,
                CONNECTED    = 1
            } m_state;
            std::list<std::pair<FakeSocket*, FakeSocket*>> m_listOfConnexion;
            std::list<std::pair<FakeSocket*, FakeSocket*>> m_pendingConnection;
        protected:
            //from the server
            void addPendingConnection(FakeSocket* socket);
        public:
            void disconnectedSocket();
    };
}

#endif // FAKESERVER_H
#endif

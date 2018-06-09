#if ! defined(EPOLLCATCHCHALLENGERSERVER) && ! defined (ONLYMAPRENDER)

#ifndef CATCHCHALLENGER_FAKESOCKET_H
#define CATCHCHALLENGER_FAKESOCKET_H

#include <mutex>
#include <vector>
#include <stdint.h>

#include "../config/GeneralVariable.h"
#include "../log/logger.h"
#include "FakeServer.h"

namespace CatchChallenger {

    class FakeSocket /*: public QIODevice*/
    {
        public:
            friend class FakeServer;

            explicit FakeSocket();
            ~FakeSocket();

            void abort();
            void disconnectFromHost();
            void disconnectFromFakeServer();
            void connectToHost();
            int64_t bytesAvailable() const;
            void close();
            bool isValid();
            FakeSocket* getTheOtherSocket();
            uint64_t getRXSize();
            uint64_t getTXSize();
            //QAbstractSocket::SocketError error() const;
            //QAbstractSocket::SocketState state() const;
            bool isValid() const;

        /*signals:
            void connected();
            void disconnected();
            void error(QAbstractSocket::SocketError socketError);
            void stateChanged(QAbstractSocket::SocketState socketState);
            void aboutToDelete();*/

        protected:
            FakeSocket* theOtherSocket;
            virtual bool isSequential() const;
            virtual bool canReadLine() const;
            virtual uint64_t readData(char* data, int64_t maxSize);
            virtual uint64_t writeData(const char* data, int64_t maxSize);

        private:
            std::vector<unsigned char> data;
            static std::mutex mutex;

            uint64_t RX_size;
            void internal_writeData(std::vector<unsigned char> rawData);
            int64_t bytesAvailableWithMutex();
    };
}

#endif // FAKESOCKET_H

#endif

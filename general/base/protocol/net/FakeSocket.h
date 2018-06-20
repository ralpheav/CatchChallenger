#if ! defined(EPOLLCATCHCHALLENGERSERVER) && ! defined (ONLYMAPRENDER)

#ifndef CATCHCHALLENGER_FAKESOCKET_H
#define CATCHCHALLENGER_FAKESOCKET_H

#include <mutex>
#include <vector>
#include <stdint.h>

#include "../config/GeneralVariable.h"
#include "../log/logger.h"
#include "FakeServer.h"
#include "ISocket.h"

namespace CatchChallenger {

    class FakeSocket : public ISocket
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
            int error() const;
            int state() const;
            bool isValid() const;

            enum {
                unconnected = 0,
                connected   = 1
            } m_state;

            virtual void connected() = 0;
            virtual void disconnected() = 0;
            virtual void error(QAbstractSocket::SocketError socketError) = 0;
            virtual void stateChanged(QAbstractSocket::SocketState socketState) = 0;
            virtual void aboutToDelete() = 0;
            virtual void readyRead() = 0;

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

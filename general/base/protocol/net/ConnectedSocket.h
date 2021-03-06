#ifndef CATCHCHALLENGER_CONNECTEDSOCKET_H
#define CATCHCHALLENGER_CONNECTEDSOCKET_H

#if ! defined(EPOLLCATCHCHALLENGERSERVER) && ! defined (ONLYMAPRENDER)

#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>

#ifdef __linux__
    #include <netinet/tcp.h>
    #include <netdb.h>
#endif

#include <iostream>
#include <vector>
#include <string>
#include <list>

#include "HostAddress.h"
#include "ISocket.h"
#include "SSLSocket.h"

namespace CatchChallenger
{
    class ConnectedSocket
    {
        public:
            explicit ConnectedSocket(ISocket* socket);
            ~ConnectedSocket();

            void	abort();
            void	connectToHost(const std::string& hostName, uint16_t port);
            void	connectToHost(const HostAddress& address, uint16_t port);
            void	disconnectFromHost();
            int         error() const;
            bool	flush();
            bool	isValid() const;
            void        setTcpCork(const bool &cork);
            HostAddress localAddress() const;
            uint16_t    localPort() const;
            HostAddress peerAddress() const;
            std::string peerName() const;
            uint16_t    peerPort() const;
            SSLSocket*  getSSLSocket() const;
            int         state() const;
            bool	waitForConnected(int msecs = 30000);
            bool	waitForDisconnected(int msecs = 30000);
            int64_t	bytesAvailable() const;
            OpenMode    openMode() const;
            std::string errorString() const;
            int64_t	readData(char* data, int64_t maxSize);
            int64_t	writeData(const char* data, int64_t maxSize);
            void	close();
            bool        isSSL();
            bool        isFake();
            bool        isTCP();
            bool        exists();
            bool        connect(SslMode mode);

        protected:

            bool isSequential() const;
            bool canReadLine() const;
            std::list<SslError> sslErrors() const;
            //workaround because QSslSocket don't return correct value for i2p via proxy
            std::string hostName;
            uint16_t    port;
            State       m_state;
            ISocket*    pSocket;

            //TODO: need to create events ???
            //virtual void	connected() = 0;
            //virtual void	disconnected() = 0;
            //virtual void	error(SocketError socketError) = 0;
            //virtual void	stateChanged(SocketState socketState) = 0;
            //virtual void    sslErrors(const std::list<QSslError> &errors) = 0;
            //virtual void    destroyed() = 0;
            //virtual void    readyRead() = 0;
            //virtual void    stateChanged() = 0;
            //virtual void    encrypted() = 0;
        private:
            void destroyedSocket();
            void purgeBuffer();
    };
}

#endif  // ! defined(EPOLLCATCHCHALLENGERSERVER) && ! defined (ONLYMAPRENDER)
#endif  // CATCHCHALLENGER_CONNECTEDSOCKET_H

#if ! defined(EPOLLCATCHCHALLENGERSERVER) && ! defined (ONLYMAPRENDER)

#ifndef CATCHCHALLENGER_ISOCKET_H
#define CATCHCHALLENGER_ISOCKET_H

#include <cstdint>
#include <vector>

namespace CatchChallenger
{
    enum SslError {
        NoError,
        UnableToGetIssuerCertificate,
        UnableToDecryptCertificateSignature,
        UnableToDecodeIssuerPublicKey,
        CertificateSignatureFailed,
        CertificateNotYetValid,
        CertificateExpired,
        InvalidNotBeforeField,
        InvalidNotAfterField,
        SelfSignedCertificate,
        SelfSignedCertificateInChain,
        UnableToGetLocalIssuerCertificate,
        UnableToVerifyFirstCertificate,
        CertificateRevoked,
        InvalidCaCertificate,
        PathLengthExceeded,
        InvalidPurpose,
        CertificateUntrusted,
        CertificateRejected,
        SubjectIssuerMismatch, // hostname mismatch?
        AuthorityIssuerSerialNumberMismatch,
        NoPeerCertificate,
        HostNameMismatch,
        NoSslSupport,
        CertificateBlacklisted,
        UnspecifiedError = -1
    };

    enum SocketError {
        ConnectionRefusedError,
        RemoteHostClosedError,
        HostNotFoundError,
        SocketAccessError,
        SocketResourceError,
        SocketTimeoutError,                     /* 5 */
        DatagramTooLargeError,
        NetworkError,
        AddressInUseError,
        SocketAddressNotAvailableError,
        UnsupportedSocketOperationError,        /* 10 */
        UnfinishedSocketOperationError,
        ProxyAuthenticationRequiredError,
        SslHandshakeFailedError,
        ProxyConnectionRefusedError,
        ProxyConnectionClosedError,             /* 15 */
        ProxyConnectionTimeoutError,
        ProxyNotFoundError,
        ProxyProtocolError,
        OperationError,
        SslInternalError,                       /* 20 */
        SslInvalidUserDataError,
        TemporaryError,
        UnknownSocketError = -1
    };

    enum SslMode {
        UnencryptedMode,
        SslClientMode,
        SslServerMode
    };

    enum PeerVerifyMode {
        VerifyNone,
        QueryPeer,
        VerifyPeer,
        AutoVerifyPeer
    };

    enum SocketState {
        UnconnectedState,
        HostLookupState,
        ConnectingState,
        ConnectedState,
        BoundState,
        ListeningState,
        ClosingState
    };

    enum State {
        unconnected = 0,
        connected   = 1
    };

    enum SocketOption {
        LowDelayOption, // TCP_NODELAY
        KeepAliveOption, // SO_KEEPALIVE
        MulticastTtlOption, // IP_MULTICAST_TTL
        MulticastLoopbackOption, // IP_MULTICAST_LOOPBACK
        TypeOfServiceOption, //IP_TOS
        SendBufferSizeSocketOption,    //SO_SNDBUF
        ReceiveBufferSizeSocketOption  //SO_RCVBUF
    };

    enum OpenMode {
        unknown = 0
    };

    enum DeviceMode {
        Read,
        write,
        ReadWrite,
        Unbuffered
    };

    class ISocket
    {
        protected:
            int port;
            char host[15];

            SocketState m_state;
            SocketError m_error;
            std::pair<SocketOption, int> m_option;
            DeviceMode m_mode;
        public:
            virtual void open(DeviceMode mode) = 0;
            virtual void abort() = 0;
            virtual void disconnectFromHost() = 0;
            //virtual void disconnectFromFakeServer() = 0;
            virtual void connectToHost(const std::string& host, int port) = 0;
            virtual int64_t bytesAvailable() const = 0;
            virtual void close() = 0;
            virtual uint64_t getRXSize() = 0;
            virtual uint64_t getTXSize() = 0;
            SocketError error() const
            {
                return m_error;
            }
            virtual SocketState state() const
            {
                return m_state;
            }
            void setSocketOption(SocketOption option, int parameter) {
                m_option = std::make_pair(option, parameter);
                //getsockopt() //SO_ERROR
                //TODO: is a list of options necessarily?
            }
            virtual bool isValid() const = 0;
            virtual bool socketDescriptor() = 0;

//            virtual void connected() = 0;
//            virtual void disconnected() = 0;
//            virtual void error(SocketError socketError) = 0;
//            virtual void stateChanged(SocketState socketState) = 0;
//            virtual void aboutToDelete() = 0;
//            virtual void readyRead() = 0;
//            virtual void deleteLater() = 0;

        protected:

            virtual bool isSequential() const = 0;
            virtual bool canReadLine() const = 0;
            virtual uint64_t readData(char* data, int64_t maxSize) = 0;
            virtual uint64_t writeData(const char* data, int64_t maxSize) = 0;
            virtual void internal_writeData(std::vector<unsigned char> rawData) = 0;
            virtual int64_t bytesAvailableWithMutex() = 0;
    };
}

#endif // CATCHCHALLENGER_ISOCKET_H
#endif // ! defined(EPOLLCATCHCHALLENGERSERVER) && ! defined (ONLYMAPRENDER)

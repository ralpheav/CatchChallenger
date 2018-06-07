#ifndef CATCHCHALLENGER_PROTOCOLPARSING_H
#define CATCHCHALLENGER_PROTOCOLPARSING_H

#include <unordered_set>
#include <unordered_map>
#include <vector>
#include <stdint.h>
#include <string>

#include "config/GeneralVariable.h"

#ifdef EPOLLCATCHCHALLENGERSERVER
    #ifdef SERVERSSL
        #include "../../server/epoll/EpollSslClient.h"
    #else
        #include "../../server/epoll/EpollClient.h"
    #endif
#else
    #include "net/ConnectedSocket.h"
#endif

#define CATCHCHALLENGER_COMMONBUFFERSIZE 4096

#ifdef CATCHCHALLENGER_SERVER_DATAPACK_ONLYBYMIRROR
    // without datapack send
    #define CATCHCHALLENGER_BIGBUFFERSIZE 256*1024
#else
    #define CATCHCHALLENGER_BIGBUFFERSIZE 8*1024*1024
#endif

#if CATCHCHALLENGER_BIGBUFFERSIZE < CATCHCHALLENGER_MAX_PACKET_SIZE
    #error CATCHCHALLENGER_BIGBUFFERSIZE can t be lower than CATCHCHALLENGER_MAX_PACKET_SIZE
#endif

#ifdef EPOLLCATCHCHALLENGERSERVER
    #define CATCHCHALLENGER_BIGBUFFERSIZE_FORTOPLAYER CATCHCHALLENGER_BIGBUFFERSIZE-16
    #ifndef CATCHCHALLENGER_EXTRA_CHECK
        #ifdef CATCHCHALLENGER_CLASS_ALLINONESERVER
            #define CATCHCHALLENGERSERVERDROPIFCLENT
        #endif
    #endif
#endif

#define CATCHCHALLENGER_PROTOCOL_REPLY_SERVER_TO_CLIENT 0x7F
#define CATCHCHALLENGER_PROTOCOL_REPLY_CLIENT_TO_SERVER 0x01

namespace CatchChallenger {

#if ! defined (ONLYMAPRENDER)
    class ProtocolParsingCheck;

    class ProtocolParsing
    {
        public:
            /// \brief Define the mode of transmission: client or server
            enum PacketModeTransmission
            {
                PacketModeTransmission_Server=0x00,
                PacketModeTransmission_Client=0x01
            };
            #ifndef EPOLLCATCHCHALLENGERSERVERNOCOMPRESSION
            enum CompressionType
            {
                None=0x00,
                Zstandard = 0x04
            };
            #endif
            enum InitialiseTheVariableType
            {
                AllInOne,
                LoginServer,
                GameServer,
                MasterServer
            };
            #ifndef EPOLLCATCHCHALLENGERSERVERNOCOMPRESSION
                #ifndef CATCHCHALLENGERSERVERDROPIFCLENT
                    #ifdef CATCHCHALLENGER_CLASS_ONLYGAMESERVER
                        #ifdef CATCHCHALLENGERSERVERDROPIFCLENT
                            #error Can t be CATCHCHALLENGER_CLASS_ONLYGAMESERVER and CATCHCHALLENGERSERVERDROPIFCLENT
                        #endif
                    #endif
                    static CompressionType compressionTypeClient;
                #endif
                static CompressionType compressionTypeServer;
                static uint8_t compressionLevel;
            #endif
            static uint8_t packetFixedSize[256+128];

            ProtocolParsing();
            static void initialiseTheVariable(const InitialiseTheVariableType &initialiseTheVariableType=InitialiseTheVariableType::AllInOne);
            static void setMaxPlayers(const uint16_t &maxPlayers);
            static int32_t decompressZstandard(const char * const input, const uint32_t &intputSize, char * const output, const uint32_t &maxOutputSize);
            static int32_t compressZstandard(const char * const input, const uint32_t &intputSize, char * const output, const uint32_t &maxOutputSize);

            #ifndef EPOLLCATCHCHALLENGERSERVERNOCOMPRESSION
                int32_t computeDecompression(const char* const source, char* const dest, const unsigned int &sourceSize, const unsigned int &maxDecompressedSize, const CompressionType &compressionType);
                int32_t computeCompression(const char* const source, char* const dest, const unsigned int &sourceSize, const unsigned int &maxCompressedSize, const CompressionType &compressionType);
            #endif
        protected:
            virtual void errorParsingLayer(const std::string &error) = 0;
            virtual void messageParsingLayer(const std::string &message) const = 0;
        private:
            virtual void reset() = 0;
    };

#else

    class ProtocolParsing
    {
        public:
            static uint8_t compressionLevel;
            static int32_t decompressZstandard(const char * const input, const uint32_t &intputSize, char * const output, const uint32_t &maxOutputSize);
            static int32_t compressZstandard(const char * const input, const uint32_t &intputSize, char * const output, const uint32_t &maxOutputSize);
    };
#endif
}

#endif // PROTOCOLPARSING_H

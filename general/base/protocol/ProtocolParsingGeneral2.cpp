#include "ProtocolParsing2.h"
#include "../GeneralVariable.h"
#include "../GeneralStructures.h"
#include "ProtocolParsingCheck2.h"

#include <iostream>
#include <cstring>
#include <openssl/sha.h>

#ifndef EPOLLCATCHCHALLENGERSERVER
    #include <QMetaType>
#endif

#ifndef EPOLLCATCHCHALLENGERSERVERNOCOMPRESSION
    #include "zstd.h"
#endif

using namespace CatchChallenger;

#if ! defined (ONLYMAPRENDER)
    char ProtocolParsingBase::tempBigBufferForOutput[];
    char ProtocolParsingBase::tempBigBufferForInput[];//to store the input buffer on linux READ() interface or with Qt
#ifndef EPOLLCATCHCHALLENGERSERVERNOCOMPRESSION
    char ProtocolParsingBase::tempBigBufferForUncompressedInput[];
    char ProtocolParsingBase::tempBigBufferForCompressedOutput[];
#endif

ProtocolParsingBase::ProtocolParsingBase(
        #ifndef CATCHCHALLENGERSERVERDROPIFCLENT
            PacketModeTransmission packetModeTransmission
        #endif
        ) :
    #ifdef EPOLLCATCHCHALLENGERSERVER
        #ifdef SERVERSSL
            ProtocolParsing(),
        #else
            ProtocolParsing(),
        #endif
    #else
    ProtocolParsing(),
    #endif
    flags(0),
    // for data
    dataSize(0),
    //to parse the netwrok stream
    packetCode(0),
    queryNumber(0)
{
    #ifndef EPOLLCATCHCHALLENGERSERVER
    //if(!connect(socket,&ConnectedSocket::readyRead,this,&ProtocolParsingInputOutput::parseIncommingData,Qt::QueuedConnection/*to virtual socket*/))
    //    messageParsingLayer(std::to_string(isClient)+std::stringLiteral(" ProtocolParsingInputOutput::ProtocolParsingInputOutput(): can't connect the object"));
    #endif
    #ifndef CATCHCHALLENGERSERVERDROPIFCLENT
    if(packetModeTransmission==PacketModeTransmission_Client)
        flags |= 0x10;
    #endif
    memset(outputQueryNumberToPacketCode,0x00,sizeof(outputQueryNumberToPacketCode));
}

ProtocolParsingBase::~ProtocolParsingBase()
{
}

#ifndef EPOLLCATCHCHALLENGERSERVER
uint64_t ProtocolParsingInputOutput::getRXSize() const
{
    return RXSize;
}

uint64_t ProtocolParsingInputOutput::getTXSize() const
{
    return TXSize;
}
#endif

void ProtocolParsingBase::reset()
{
    //outputQueryNumberToPacketCode.clear();

    dataClear();
}

void ProtocolParsingBase::resetForReconnect()
{
    flags &= 0x18;
    // ProtocolParsingBase
    // for data
    dataSize=0;
    //to parse the netwrok stream
    packetCode=0;
    queryNumber=0;
    memset(outputQueryNumberToPacketCode,0x00,sizeof(outputQueryNumberToPacketCode));
}

void ProtocolParsingInputOutput::resetForReconnect()
{
    ProtocolParsingBase::resetForReconnect();

    #ifdef CATCHCHALLENGER_EXTRA_CHECK
    parseIncommingDataCount=0;
    #endif
    //ProtocolParsingInputOutput
    #ifndef EPOLLCATCHCHALLENGERSERVER
    RXSize=0;
    TXSize=0;
    #endif
    memset(inputQueryNumberToPacketCode,0x00,sizeof(inputQueryNumberToPacketCode));
}

ProtocolParsingInputOutput::ProtocolParsingInputOutput(
        #if defined(EPOLLCATCHCHALLENGERSERVER) || defined (ONLYMAPRENDER)
            #ifdef SERVERSSL
                const int &infd, SSL_CTX *ctx
            #else
                const int &infd
            #endif
        #else
        ConnectedSocket *socket
        #endif
        #ifndef CATCHCHALLENGERSERVERDROPIFCLENT
        ,const PacketModeTransmission &packetModeTransmission
        #endif
        ) :
    ProtocolParsingBase(
        #ifndef CATCHCHALLENGERSERVERDROPIFCLENT
        packetModeTransmission
        #endif
        ),
    #ifdef EPOLLCATCHCHALLENGERSERVER
        #ifdef SERVERSSL
            epollSocket(infd,ctx)
        #else
            epollSocket(infd)
        #endif
    #else
        #if ! defined (ONLYMAPRENDER)
        socket(socket)
        #endif
    #endif
      #ifdef CATCHCHALLENGER_EXTRA_CHECK
      ,parseIncommingDataCount(0)
      #endif
{
    #ifdef CATCHCHALLENGER_EXTRA_CHECK
    #ifndef CATCHCHALLENGERSERVERDROPIFCLENT
        if(packetModeTransmission==PacketModeTransmission_Client)
            flags |= 0x10;
        if(packetModeTransmission==PacketModeTransmission_Client)
            protocolParsingCheck=new ProtocolParsingCheck(PacketModeTransmission_Server);
        else
            protocolParsingCheck=new ProtocolParsingCheck(PacketModeTransmission_Client);
    #else
        #error "Can't have both CATCHCHALLENGERSERVERDROPIFCLENT and CATCHCHALLENGER_EXTRA_CHECK enabled because ProtocolParsingCheck work as client"
    #endif
    #endif
    #ifndef EPOLLCATCHCHALLENGERSERVER
    RXSize=0;
    TXSize=0;
    #endif
    memset(inputQueryNumberToPacketCode,0x00,sizeof(inputQueryNumberToPacketCode));
}

ProtocolParsingInputOutput::~ProtocolParsingInputOutput()
{
    #ifdef CATCHCHALLENGER_EXTRA_CHECK
    if(protocolParsingCheck!=NULL)
    {
        delete protocolParsingCheck;
        protocolParsingCheck=NULL;
    }
    #endif
}

#endif

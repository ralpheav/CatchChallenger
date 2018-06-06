#include "Api_protocol_2.h"

using namespace CatchChallenger;

const unsigned char protocolHeaderToMatchLogin[]      = PROTOCOL_HEADER_LOGIN;
const unsigned char protocolHeaderToMatchGameServer[] = PROTOCOL_HEADER_GAMESERVER;

#ifdef Q_CC_GNU
    //this next header is needed to change file time/date under gcc
    #include <utime.h>
    #include <sys/stat.h>
#endif

#ifndef BOTTESTCONNECT
    #include "SslCert.h"
    #include "LanguagesSelect.h"
#endif

#ifdef BENCHMARKMUTIPLECLIENT
    char Api_protocol_2::hurgeBufferForBenchmark[4096];
    bool Api_protocol_2::precomputeDone=false;
    char Api_protocol_2::hurgeBufferMove[4];

    #include <iostream>
    #include <fstream>
    #include <unistd.h>
#endif

//need host + port here to have datapack base

std::unordered_set<std::string> Api_protocol_2::extensionAllowed;

bool Api_protocol_2::internalVersionDisplayed = false;

std::string Api_protocol_2::text_balise_root_start = "<root>";
std::string Api_protocol_2::text_balise_root_stop  = "</root>";
std::string Api_protocol_2::text_name              = "name";
std::string Api_protocol_2::text_description       = "description";
std::string Api_protocol_2::text_en                = "en";
std::string Api_protocol_2::text_lang              = "lang";
std::string Api_protocol_2::text_slash             = "/";

Api_protocol_2::Api_protocol_2(ConnectedSocket *socket, bool tolerantMode) :
    ProtocolParsingInputOutput(socket,PacketModeTransmission_Client),
    tolerantMode(tolerantMode)
{
    datapackStatus=DatapackStatus::Base;

    #ifdef BENCHMARKMUTIPLECLIENT
        if(!Api_protocol::precomputeDone)
        {
            Api_protocol::precomputeDone=true;
            hurgeBufferMove[0]=0x40;
        }
    #endif

    if(extensionAllowed.empty())
    {
        const std::vector<std::string> &v=stringsplit(std::string(CATCHCHALLENGER_EXTENSION_ALLOWED),';');
        extensionAllowed=std::unordered_set<std::string>(v.cbegin(),v.cend());
    }

    player_informations.recipes=NULL;
    player_informations.encyclopedia_monster=NULL;
    player_informations.encyclopedia_item=NULL;
    player_informations.bot_already_beaten=NULL;
    stageConnexion=StageConnexion::Stage1;
    resetAll();

    if(!QObject::connect(socket,&ConnectedSocket::destroyed,this,&Api_protocol::QtsocketDestroyed))
        abort();
    if(socket->sslSocket!=NULL)
    {
        if(!QObject::connect(socket,&ConnectedSocket::readyRead,this,&Api_protocol::readForFirstHeader))
            abort();
        if(socket->bytesAvailable())
            readForFirstHeader();
    }
    else
    {
        if(socket->fakeSocket!=NULL)
            haveFirstHeader=true;
        if(!QObject::connect(socket,&ConnectedSocket::readyRead,this,&Api_protocol::parseIncommingData,Qt::QueuedConnection))//put queued to don't have circular loop Client -> Server -> Client
            abort();
        if(socket->bytesAvailable())
            parseIncommingData();
    }

    if(!Api_protocol::internalVersionDisplayed)
    {
        Api_protocol::internalVersionDisplayed=true;
        std::cout << "Qt version: " << qVersion() << std::endl;
    }

    {
        lastQueryNumber.reserve(16);
        uint8_t index=1;
        while(index<16)
        {
            lastQueryNumber.push_back(index);
            index++;
        }
    }

    inProgress = false;
}

Api_protocol_2::~Api_protocol_2()
{
    qDebug() << "Api_protocol_2::~Api_protocol_2()";
    if(player_informations.encyclopedia_monster!=NULL)
    {
        delete player_informations.encyclopedia_monster;
        player_informations.encyclopedia_monster=NULL;
    }
    if(player_informations.encyclopedia_item!=NULL)
    {
        delete player_informations.encyclopedia_item;
        player_informations.encyclopedia_item=NULL;
    }
}

bool Api_protocol_2::disconnectClient()
{
    if(socket!=NULL)
        socket->disconnect();
    is_logged=false;
    character_selected=false;
    if(player_informations.encyclopedia_monster!=NULL)
    {
        delete player_informations.encyclopedia_monster;
        player_informations.encyclopedia_monster=NULL;
    }
    if(player_informations.encyclopedia_item!=NULL)
    {
        delete player_informations.encyclopedia_item;
        player_informations.encyclopedia_item=NULL;
    }
    return true;
}

void Api_protocol_2::QtsocketDestroyed()
{
    socketDestroyed();
}

void Api_protocol_2::socketDestroyed()
{
    socket=NULL;
}

std::map<uint8_t,uint64_t> Api_protocol_2::getQuerySendTimeList() const
{
    return querySendTime;
}

void Api_protocol_2::parseIncommingData()
{
    ProtocolParsingInputOutput::parseIncommingData();
}

void Api_protocol_2::errorParsingLayer(const std::string &error)
{
    #ifdef CATCHCHALLENGER_EXTRA_CHECK
        abort();
    #endif
    newError(("Internal error, file: "+std::string(__FILE__)+":"+std::to_string(__LINE__)),error);
}

void Api_protocol_2::messageParsingLayer(const std::string &message) const
{
    qDebug() << QString::fromStdString(message);
}

void Api_protocol_2::parseError(const std::string &userMessage,const std::string &errorString)
{
    if(tolerantMode)
        std::cerr << "packet ignored due to: " << errorString << std::endl;
    else
    {
        std::cerr << userMessage << " " << errorString << std::endl;
        newError(userMessage,errorString);
    }
    #ifdef CATCHCHALLENGER_EXTRA_CHECK
    abort();
    #endif
}

Player_private_and_public_informations &Api_protocol_2::get_player_informations()
{
    if(!getCaracterSelected())
    {
        std::cerr << "Api_protocol_2::get_player_informations(): !getCharacterSelected() (internal error)" << std::endl;
        return player_informations;
    }
    return player_informations;
}

const Player_private_and_public_informations &Api_protocol_2::get_player_informations_ro() const
{
    if(!getCaracterSelected())
    {
        std::cerr << "Api_protocol_2::get_player_informations_ro(): !getCharacterSelected() (internal error)" << std::endl;
        return player_informations;
    }
    return player_informations;
}

std::string Api_protocol_2::getPseudo()
{
    if(!getCaracterSelected())
    {
        std::cerr << "Api_protocol_2::getPseudo(): !getCaracterSelected() (internal error)" << std::endl;
        return std::string();
    }
    return player_informations.public_informations.pseudo;
}

uint16_t Api_protocol_2::getId()
{
    if(!getCaracterSelected())
    {
        std::cerr << "Api_protocol_2::getId(): !getCaracterSelected() (internal error)" << std::endl;
        return 0;
    }
    return player_informations.public_informations.simplifiedId;
}

uint8_t Api_protocol_2::queryNumber()
{
    if(lastQueryNumber.empty())
    {
        std::cerr << "Api_protocol_2::queryNumber(): no more lastQueryNumber" << std::endl;
        abort();
    }
    const uint8_t lastQueryNumberTemp=this->lastQueryNumber.back();
    const std::time_t result = std::time(nullptr);
    querySendTime[lastQueryNumberTemp]=result;
    this->lastQueryNumber.pop_back();
    return lastQueryNumberTemp;
}

bool Api_protocol_2::sendProtocol()
{
    if(have_send_protocol)
    {
        newError("Internal problem","Api_protocol_2::sendProtocol() Have already send the protocol");
        return false;
    }
    if(!haveFirstHeader)
    {
        newError("Internal problem","Api_protocol_2::sendProtocol() !haveFirstHeader");
        return false;
    }

    have_send_protocol=true;
    if(stageConnexion==StageConnexion::Stage1)
        packOutcommingQuery(0xA0,queryNumber(),reinterpret_cast<const char *>(protocolHeaderToMatchLogin),sizeof(protocolHeaderToMatchLogin));
    else if(stageConnexion==StageConnexion::Stage3)
    {
        stageConnexion=CatchChallenger::Api_protocol_2::StageConnexion::Stage4;
        packOutcommingQuery(0xA0,queryNumber(),reinterpret_cast<const char *>(protocolHeaderToMatchGameServer),sizeof(protocolHeaderToMatchGameServer));
    }
    else
        newError("Internal problem","stageConnexion!=StageConnexion::Stage1/3");
    return true;
}

std::string Api_protocol_2::socketDisconnectedForReconnect()
{
    if(stageConnexion!=StageConnexion::Stage2)
    {
        if(stageConnexion!=StageConnexion::Stage3)
        {
            newError("Internal problem","Api_protocol_2::socketDisconnectedForReconnect(): "+std::to_string((int)stageConnexion));
            return std::string();
        }
        else
        {
            std::cerr << "socketDisconnectedForReconnect() double call detected, just drop it" << std::endl;
            return std::string();
        }
    }
    if(selectedServerIndex==-1)
    {
        parseError("Internal error, file: "+std::string(__FILE__)+":"+std::to_string(__LINE__),std::string("selectedServerIndex==-1 with Api_protocol_2::socketDisconnectedForReconnect()"));
        return std::string();
    }
    const ServerFromPoolForDisplay &serverFromPoolForDisplay=serverOrdenedList.at(selectedServerIndex);
    if(serverFromPoolForDisplay.host.empty())
    {
        parseError("Internal error, file: "+std::string(__FILE__)+":"+std::to_string(__LINE__),std::string("serverFromPoolForDisplay.host.isEmpty() with Api_protocol_2::socketDisconnectedForReconnect()"));
        return std::string();
    }
    if(socket==NULL)
    {
        parseError("Internal error, file: "+std::string(__FILE__)+":"+std::to_string(__LINE__),std::string("socket==NULL with Api_protocol_2::socketDisconnectedForReconnect()"));
        return serverFromPoolForDisplay.host+":"+std::to_string(serverFromPoolForDisplay.port);
    }
    message("stageConnexion=CatchChallenger::Api_protocol_2::StageConnexion::Stage3 set at "+std::string(__FILE__)+":"+std::to_string(__LINE__));
    stageConnexion=CatchChallenger::Api_protocol_2::StageConnexion::Stage3;//prevent loop in stage2
    haveFirstHeader=false;
    std::cout << "Api_protocol_2::socketDisconnectedForReconnect(), Try connect to: " << serverFromPoolForDisplay.host << ":" << serverFromPoolForDisplay.port << std::endl;
    socket->connectToHost(QString::fromStdString(serverFromPoolForDisplay.host),serverFromPoolForDisplay.port);
    return serverFromPoolForDisplay.host+":"+std::to_string(serverFromPoolForDisplay.port);
}

const std::vector<ServerFromPoolForDisplay> &Api_protocol_2::getServerOrdenedList()
{
    return serverOrdenedList;
}

bool Api_protocol_2::protocolWrong() const
{
    return have_send_protocol && !have_receive_protocol;
}

bool Api_protocol_2::tryLogin(const std::string &login, const std::string &pass)
{
    if(!have_send_protocol)
    {
        newError(std::string("Internal problem"),std::string("Have not send the protocol"));
        return false;
    }
    if(is_logged)
    {
        newError(std::string("Internal problem"),std::string("Is already logged"));
        return false;
    }
    if(token.empty())
    {
        newError(std::string("Internal problem"),std::string("Token is empty"));
        return false;
    }
    std::string outputData;
    #ifdef CATCHCHALLENGER_EXTRA_CHECK
        std::string tempDoubleHash;
    #endif
    {
        QCryptographicHash hashLogin(QCryptographicHash::Sha224);
        hashLogin.addData((QString::fromStdString(login)+/*salt*/"RtR3bm9Z1DFMfAC3").toUtf8());
        loginHash=std::string(hashLogin.result().constData(),hashLogin.result().size());
        outputData+=loginHash;
        #ifdef CATCHCHALLENGER_EXTRA_CHECK
            {
                QCryptographicHash hashLogin2(QCryptographicHash::Sha224);
                hashLogin2.addData(QByteArray(loginHash.data(),loginHash.size()));
                tempDoubleHash=std::string(hashLogin2.result().data(),hashLogin2.result().size());
            }
        #endif
    }
    QCryptographicHash hashAndToken(QCryptographicHash::Sha224);
    {
        QCryptographicHash hashPass(QCryptographicHash::Sha224);
        hashPass.addData((QString::fromStdString(pass)+/*salt*/"AwjDvPIzfJPTTgHs"+QString::fromStdString(login)/*add unique salt*/).toUtf8());
        passHash=std::string(hashPass.result().data(),hashPass.result().size());

        hashAndToken.addData(QByteArray(passHash.data(),passHash.size()));
        hashAndToken.addData(QByteArray(token.data(),token.size()));
        outputData+=std::string(hashAndToken.result().data(),hashAndToken.result().size());
    }

    #ifdef CATCHCHALLENGER_EXTRA_CHECK
        std::cout << "Try auth: password " << binarytoHexa(passHash.data(),passHash.size())
                  << ", token: " << binarytoHexa(token.data(),token.size())
                  << ", password+token " << binarytoHexa(hashAndToken.result().data(),hashAndToken.result().size())
                  << " (" << binarytoHexa(tempDoubleHash.data(),tempDoubleHash.size())
                  << ") for the login: "
                  << binarytoHexa(passHash.data(),passHash.size());
    #endif

    std::string peerName=socket->peerName().toStdString();
    if(peerName.size()>255)
    {
        newError(QObject::tr("Hostname too big").toStdString(),std::string("Hostname too big"));
        return false;
    }

    packOutcommingQuery(0xA8,queryNumber(),outputData.data(),outputData.size());
    return true;
}

bool Api_protocol_2::tryCreateAccount()
{
    if(!have_send_protocol)
    {
        newError(std::string("Internal problem"),std::string("Have not send the protocol"));
        return false;
    }
    if(is_logged)
    {
        newError(std::string("Internal problem"),std::string("Is already logged"));
        return false;
    }
    /*double hashing on client part
     * '''Prevent login leak in case of MiM attack re-ask the password''' (Trafic modification, replace the server return code OK by ACCOUNT CREATION)
     * Do some DDOS protection because it offload the hashing */
    std::string outputData;
    {
        QCryptographicHash hashLogin(QCryptographicHash::Sha224);
        hashLogin.addData(QByteArray(loginHash.data(),loginHash.size()));
        outputData+=std::string(hashLogin.result().data(),hashLogin.result().size());
    }
    //pass
    outputData+=passHash;

    packOutcommingQuery(0xA9,queryNumber(),outputData.data(),outputData.size());
    std::cout << "Try create account: login: " << binarytoHexa(loginHash.data(),loginHash.size())
              << " and pass: " << binarytoHexa(passHash.data(),passHash.size()) << std::endl;
    return true;
}

void Api_protocol_2::send_player_move(const uint8_t &moved_unit,const Direction &direction)
{
    #ifdef BENCHMARKMUTIPLECLIENT
        hurgeBufferMove[1]=moved_unit;
        hurgeBufferMove[2]=direction;
        const int &infd=socket->sslSocket->socketDescriptor();
        if(infd!=-1)
            ::write(infd,hurgeBufferMove,3);
        else
            internalSendRawSmallPacket(hurgeBufferMove,3);
        return;
    #endif
    if(!is_logged)
    {
        std::cerr << "is not logged, line: " << __FILE__ << ": " << __LINE__ << std::endl;
        return;
    }
    if(!character_selected)
    {
        std::cerr << "character not selected, line: " << __FILE__ << ": " << __LINE__ << std::endl;
        return;
    }
    uint8_t directionInt=static_cast<uint8_t>(direction);
    if(directionInt<1 || directionInt>8)
    {
        std::cerr << "direction given wrong: " << directionInt << std::endl;
        abort();
    }
    if(last_direction_is_set==false)
        abort();
    //correct integration with MoveOnTheMap::newDirection()
    if(last_direction!=direction)
    {
        send_player_move_internal(last_step,last_direction);
        send_player_move_internal(moved_unit,direction);
    }
    else
    {
        bool isAMove=false;
        switch(direction)
        {
            case Direction_move_at_top:
            case Direction_move_at_right:
            case Direction_move_at_bottom:
            case Direction_move_at_left:
                isAMove=true;
            return;
            break;
            default:
            break;
        }
        if(isAMove)
        {
            last_step+=moved_unit;
            return;
        }
        else // if look
        {
            #ifdef CATCHCHALLENGER_EXTRA_CHECK
            if(moved_unit>0)
                abort();
            if(last_step>0)
                abort();
            #endif
            last_step=0;
            return;//2x time look on smae direction, drop
        }
    }
    last_step=0;
    last_direction=direction;
}

void Api_protocol_2::send_player_move_internal(const uint8_t &moved_unit,const CatchChallenger::Direction &direction)
{
    uint8_t directionInt=static_cast<uint8_t>(direction);
    if(directionInt<1 || directionInt>8)
    {
        std::cerr << "direction given wrong: " << directionInt << std::endl;
        abort();
    }
    QByteArray outputData;
    QDataStream out(&outputData, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_4);out.setByteOrder(QDataStream::LittleEndian);
    out << moved_unit;
    out << directionInt;
    packOutcommingData(0x02,outputData.constData(),outputData.size());
}

void Api_protocol_2::send_player_direction(const Direction &the_direction)
{
    if(!is_logged)
    {
        std::cerr << "is not logged, line: " << __FILE__ << ": " << __LINE__ << std::endl;
        return;
    }
    if(!character_selected)
    {
        std::cerr << "character not selected, line: " << __FILE__ << ": " << __LINE__ << std::endl;
        return;
    }
    newDirection(the_direction);
}

void Api_protocol_2::sendChatText(const Chat_type &chatType, const std::string &text)
{
    if(!is_logged)
    {
        std::cerr << "is not logged, line: " << __FILE__ << ": " << __LINE__ << std::endl;
        return;
    }
    if(!character_selected)
    {
        std::cerr << "character not selected, line: " << __FILE__ << ": " << __LINE__ << std::endl;
        return;
    }
    if(chatType!=Chat_type_local && chatType!=Chat_type_all && chatType!=Chat_type_clan && chatType!=Chat_type_aliance && chatType!=Chat_type_system && chatType!=Chat_type_system_important)
    {
        std::cerr << "chatType wrong: " << chatType << std::endl;
        return;
    }
    QByteArray outputData;
    QDataStream out(&outputData, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_4);out.setByteOrder(QDataStream::LittleEndian);
    out << (uint8_t)chatType;
    {
        const std::string &tempText=text;
        if(tempText.size()>255)
        {
            std::cerr << "text in Utf8 too big, line: " << __FILE__ << ": " << __LINE__ << std::endl;
            return;
        }
        out << (uint8_t)tempText.size();
        outputData+=QByteArray(tempText.data(),tempText.size());
        out.device()->seek(out.device()->pos()+tempText.size());
    }
    packOutcommingData(0x03,outputData.constData(),outputData.size());
}

void Api_protocol_2::sendPM(const std::string &text,const std::string &pseudo)
{
    if(!is_logged)
    {
        std::cerr << "is not logged, line: " << __FILE__ << ": " << __LINE__ << std::endl;
        return;
    }
    if(!character_selected)
    {
        std::cerr << "character not selected, line: " << __FILE__ << ": " << __LINE__ << std::endl;
        return;
    }
    if(this->player_informations.public_informations.pseudo==pseudo)
        return;
    QByteArray outputData;
    QDataStream out(&outputData, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_4);out.setByteOrder(QDataStream::LittleEndian);
    out << (uint8_t)Chat_type_pm;
    {
        const std::string &tempText=text;
        if(tempText.size()>255)
        {
            std::cerr << "text in Utf8 too big, line: " << __FILE__ << ": " << __LINE__ << std::endl;
            return;
        }
        out << (uint8_t)tempText.size();
        outputData+=QByteArray(tempText.data(),tempText.size());
        out.device()->seek(out.device()->pos()+tempText.size());
    }
    {
        const std::string &tempText=pseudo;
        if(tempText.size()>255)
        {
            std::cerr << "text in Utf8 too big, line: " << __FILE__ << ": " << __LINE__ << std::endl;
            return;
        }
        out << (uint8_t)tempText.size();
        outputData+=QByteArray(tempText.data(),tempText.size());
        out.device()->seek(out.device()->pos()+tempText.size());
    }
    packOutcommingData(0x03,outputData.constData(),outputData.size());
}

bool Api_protocol_2::teleportDone()
{
    if(!is_logged)
    {
        std::cerr << "is not logged, line: " << __FILE__ << ": " << __LINE__ << std::endl;
        return false;
    }
    if(!character_selected)
    {
        std::cerr << "character not selected, line: " << __FILE__ << ": " << __LINE__ << std::endl;
        return false;
    }
    const TeleportQueryInformation &teleportQueryInformation=teleportList.front();
    if(!last_direction_is_set)
    {
        parseError("Procotol wrong or corrupted",
                   "Api_protocol_2::teleportDone() !last_direction_is_set value, line: "+std::string(__FILE__)+":"+std::to_string(__LINE__));
        return false;
    }
    last_direction=teleportQueryInformation.direction;
    last_step=0;
    postReplyData(teleportQueryInformation.queryId,NULL,0);
    teleportList.erase(teleportList.cbegin());
    return true;
}

bool Api_protocol_2::addCharacter(const uint8_t &charactersGroupIndex,const uint8_t &profileIndex, const std::string &pseudo, const uint8_t &monsterGroupId, const uint8_t &skinId)
{
    if(!is_logged)
    {
        std::cerr << "is not logged, line: " << __FILE__ << ": " << __LINE__ << std::endl;
        return false;
    }
    if(skinId>=CommonDatapack::commonDatapack.skins.size())
    {
        newError(std::string("Internal problem"),"skin provided: "+std::to_string(skinId)+" is not into skin listed");
        return false;
    }
    const Profile &profile=CommonDatapack::commonDatapack.profileList.at(profileIndex);
    if(!profile.forcedskin.empty() && !vectorcontainsAtLeastOne(profile.forcedskin,skinId))
    {
        newError(std::string("Internal problem"),"skin provided: "+std::to_string(skinId)+" is not into profile "+std::to_string(profileIndex)+" forced skin list");
        return false;
    }
    QByteArray outputData;
    QDataStream out(&outputData, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_4);out.setByteOrder(QDataStream::LittleEndian);
    out << (uint8_t)charactersGroupIndex;
    out << (uint8_t)profileIndex;
    {
        const std::string &rawPseudo=toUTF8WithHeader(pseudo);
        if(rawPseudo.size()>255 || rawPseudo.empty())
        {
            std::cerr << "rawPseudo too big or not compatible with utf8" << std::endl;
            return false;
        }
        outputData+=QByteArray(rawPseudo.data(),rawPseudo.size());
        out.device()->seek(out.device()->size());
    }
    out << (uint8_t)monsterGroupId;
    out << (uint8_t)skinId;
    is_logged=packOutcommingQuery(0xAA,queryNumber(),outputData.constData(),outputData.size());
    return true;
}

bool Api_protocol_2::removeCharacter(const uint8_t &charactersGroupIndex,const uint32_t &characterId)
{
    if(!is_logged)
    {
        std::cerr << "is not logged, line: " << __FILE__ << ": " << __LINE__ << std::endl;
        return false;
    }
    QByteArray outputData;
    QDataStream out(&outputData, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_4);out.setByteOrder(QDataStream::LittleEndian);
    out << (uint8_t)charactersGroupIndex;
    out << characterId;
    is_logged=packOutcommingQuery(0xAB,queryNumber(),outputData.constData(),outputData.size());
    return true;
}

bool Api_protocol_2::selectCharacter(const uint8_t &charactersGroupIndex, const uint32_t &serverUniqueKey, const uint32_t &characterId)
{
    if(characterId==0)
    {
        std::cerr << "Api_protocol_2::selectCharacter() can't have characterId==0" << std::endl;
        abort();
    }
    unsigned int index=0;
    while(index<serverOrdenedList.size())
    {
        const ServerFromPoolForDisplay &server=serverOrdenedList.at(index);
        if(server.uniqueKey==serverUniqueKey && server.charactersGroupIndex==charactersGroupIndex)
            return selectCharacter(charactersGroupIndex,serverUniqueKey,characterId,index);
        index++;
    }
    std::cerr << "index of server not found, charactersGroupIndex: " << (uint32_t)charactersGroupIndex << ", serverUniqueKey: " << serverUniqueKey << ", line: " << __FILE__ << ": " << __LINE__ << std::endl;
    return false;
}

bool Api_protocol_2::selectCharacter(const uint8_t &charactersGroupIndex, const uint32_t &serverUniqueKey, const uint32_t &characterId,const uint32_t &serverIndex)
{
    if(characterId==0)
    {
        std::cerr << "Api_protocol_2::selectCharacter() with server index can't have characterId==0" << std::endl;
        abort();
    }
    if(!is_logged)
    {
        std::cerr << "is not logged, line: " << __FILE__ << ": " << __LINE__ << std::endl;
        return false;
    }
    if(character_selected)
    {
        std::cerr << "character already selected, line: " << __FILE__ << ": " << __LINE__ << std::endl;
        return false;
    }
    if(character_select_send)
    {
        std::cerr << "character select already send, line: " << __FILE__ << ": " << __LINE__ << std::endl;
        return false;
    }

    character_select_send=true;
    QByteArray outputData;
    QDataStream out(&outputData, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_4);out.setByteOrder(QDataStream::LittleEndian);
    out << (uint8_t)charactersGroupIndex;
    out << (uint32_t)serverUniqueKey;
    out << characterId;
    is_logged=packOutcommingQuery(0xAC,queryNumber(),outputData.constData(),outputData.size());
    this->selectedServerIndex=serverIndex;
    std::cerr << "this: " << this << ", socket: " << socket << ", select char: " << characterId << ", charactersGroupIndex: " << (uint32_t)charactersGroupIndex << ", serverUniqueKey: " << serverUniqueKey << ", line: " << __FILE__ << ": " << __LINE__ << std::endl;
    return true;
}

bool Api_protocol_2::character_select_is_send()
{
    return character_select_send;
}

void Api_protocol_2::useSeed(const uint8_t &plant_id)
{
    if(!is_logged)
    {
        std::cerr << "is not logged, line: " << __FILE__ << ": " << __LINE__ << std::endl;
        return;
    }
    if(!character_selected)
    {
        std::cerr << "character not selected, line: " << __FILE__ << ": " << __LINE__ << std::endl;
        return;
    }
    QByteArray outputData;
    outputData[0]=plant_id;
    if(CommonSettingsServer::commonSettingsServer.plantOnlyVisibleByPlayer==false)
        packOutcommingQuery(0x83,queryNumber(),outputData.constData(),outputData.size());
    else
        packOutcommingData(0x19,outputData.constData(),outputData.size());
}

void Api_protocol_2::monsterMoveUp(const uint8_t &number)
{
    if(!is_logged)
    {
        std::cerr << "is not logged, line: " << __FILE__ << ": " << __LINE__ << std::endl;
        return;
    }
    if(!character_selected)
    {
        std::cerr << "character not selected, line: " << __FILE__ << ": " << __LINE__ << std::endl;
        return;
    }
    QByteArray outputData;
    QDataStream out(&outputData, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_4);out.setByteOrder(QDataStream::LittleEndian);
    out << (uint8_t)0x01;
    out << number;
    packOutcommingData(0x0D,outputData.constData(),outputData.size());
}

void Api_protocol_2::confirmEvolutionByPosition(const uint8_t &monterPosition)
{
    if(!is_logged)
    {
        std::cerr << "is not logged, line: " << __FILE__ << ": " << __LINE__ << std::endl;
        return;
    }
    if(!character_selected)
    {
        std::cerr << "character not selected, line: " << __FILE__ << ": " << __LINE__ << std::endl;
        return;
    }
    QByteArray outputData;
    QDataStream out(&outputData, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_4);out.setByteOrder(QDataStream::LittleEndian);
    out << (uint8_t)monterPosition;
    packOutcommingData(0x0F,outputData.constData(),outputData.size());
}

void Api_protocol_2::monsterMoveDown(const uint8_t &number)
{
    if(!is_logged)
    {
        std::cerr << "is not logged, line: " << __FILE__ << ": " << __LINE__ << std::endl;
        return;
    }
    if(!character_selected)
    {
        std::cerr << "character not selected, line: " << __FILE__ << ": " << __LINE__ << std::endl;
        return;
    }
    std::cerr << "confirm evolution of monster position: " << std::to_string(number) << ", line: " << __FILE__ << ": " << __LINE__ << std::endl;
    QByteArray outputData;
    QDataStream out(&outputData, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_4);out.setByteOrder(QDataStream::LittleEndian);
    out << (uint8_t)0x02;
    out << number;
    packOutcommingData(0x0D,outputData.constData(),outputData.size());
}

//inventory
void Api_protocol_2::destroyObject(const uint16_t &object, const uint32_t &quantity)
{
    if(!is_logged)
    {
        std::cerr << "is not logged, line: " << __FILE__ << ": " << __LINE__ << std::endl;
        return;
    }
    if(!character_selected)
    {
        std::cerr << "character not selected, line: " << __FILE__ << ": " << __LINE__ << std::endl;
        return;
    }
    QByteArray outputData;
    QDataStream out(&outputData, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_4);out.setByteOrder(QDataStream::LittleEndian);
    out << object;
    out << quantity;
    packOutcommingData(0x13,outputData.constData(),outputData.size());
}

bool Api_protocol_2::useObject(const uint16_t &object)
{
    if(!is_logged)
    {
        std::cerr << "is not logged, line: " << __FILE__ << ": " << __LINE__ << std::endl;
        return false;
    }
    if(!character_selected)
    {
        std::cerr << "character not selected, line: " << __FILE__ << ": " << __LINE__ << std::endl;
        return false;
    }
    QByteArray outputData;
    QDataStream out(&outputData, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_4);out.setByteOrder(QDataStream::LittleEndian);
    out << object;
    packOutcommingQuery(0x86,queryNumber(),outputData.constData(),outputData.size());
    lastObjectUsed.push_back(object);
    return true;
}

bool Api_protocol_2::useObjectOnMonsterByPosition(const uint16_t &object,const uint8_t &monsterPosition)
{
    if(!is_logged)
    {
        std::cerr << "is not logged, line: " << __FILE__ << ": " << __LINE__ << std::endl;
        return false;
    }
    if(!character_selected)
    {
        std::cerr << "character not selected, line: " << __FILE__ << ": " << __LINE__ << std::endl;
        return false;
    }
    QByteArray outputData;
    QDataStream out(&outputData, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_4);out.setByteOrder(QDataStream::LittleEndian);
    out << object;
    out << monsterPosition;
    packOutcommingData(0x10,outputData.constData(),outputData.size());
    return true;
}


void Api_protocol_2::wareHouseStore(const int64_t &cash, const std::vector<std::pair<uint16_t,int32_t> > &items,
                                  const std::vector<uint32_t> &withdrawMonsters, const std::vector<uint32_t> &depositeMonsters)
{
    if(!is_logged)
    {
        std::cerr << "is not logged, line: " << __FILE__ << ": " << __LINE__ << std::endl;
        return;
    }
    if(!character_selected)
    {
        std::cerr << "character not selected, line: " << __FILE__ << ": " << __LINE__ << std::endl;
        return;
    }
    QByteArray outputData;
    QDataStream out(&outputData, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_4);out.setByteOrder(QDataStream::LittleEndian);
    out << (quint64)cash;

    out << (uint16_t)items.size();
    unsigned int index=0;
    while(index<items.size())
    {
        out << (uint16_t)items.at(index).first;
        out << (int32_t)items.at(index).second;
        index++;
    }

    out << (uint32_t)withdrawMonsters.size();
    index=0;
    while(index<withdrawMonsters.size())
    {
        out << (uint32_t)withdrawMonsters.at(index);
        index++;
    }
    out << (uint32_t)depositeMonsters.size();
    index=0;
    while(index<depositeMonsters.size())
    {
        out << (uint32_t)depositeMonsters.at(index);
        index++;
    }

    packOutcommingData(0x17,outputData.constData(),outputData.size());
}

void Api_protocol_2::takeAnObjectOnMap()
{
    //std::cout << "Api_protocol_2::takeAnObjectOnMap(): " << player_informations.public_informations.pseudo << std::endl;
    packOutcommingData(0x18,NULL,0);
}

void Api_protocol_2::getShopList(const uint16_t &shopId)/// \see CommonMap, std::unordered_map<std::pair<uint8_t,uint8_t>,std::vector<uint16_t>, pairhash> shops;
{
    if(!is_logged)
    {
        std::cerr << "is not logged, line: " << __FILE__ << ": " << __LINE__ << std::endl;
        return;
    }
    if(!character_selected)
    {
        std::cerr << "character not selected, line: " << __FILE__ << ": " << __LINE__ << std::endl;
        return;
    }
    QByteArray outputData;
    QDataStream out(&outputData, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_4);out.setByteOrder(QDataStream::LittleEndian);
    out << (uint16_t)shopId;
    packOutcommingQuery(0x87,queryNumber(),outputData.constData(),outputData.size());
}

void Api_protocol_2::buyObject(const uint16_t &shopId, const uint16_t &objectId, const uint32_t &quantity, const uint32_t &price)/// \see CommonMap, std::unordered_map<std::pair<uint8_t,uint8_t>,std::vector<uint16_t>, pairhash> shops;
{
    if(!is_logged)
    {
        std::cerr << "is not logged, line: " << __FILE__ << ": " << __LINE__ << std::endl;
        return;
    }
    if(!character_selected)
    {
        std::cerr << "character not selected, line: " << __FILE__ << ": " << __LINE__ << std::endl;
        return;
    }
    QByteArray outputData;
    QDataStream out(&outputData, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_4);out.setByteOrder(QDataStream::LittleEndian);
    out << (uint16_t)shopId;
    out << (uint16_t)objectId;
    out << (uint32_t)quantity;
    out << (uint32_t)price;
    packOutcommingQuery(0x88,queryNumber(),outputData.constData(),outputData.size());
}

void Api_protocol_2::sellObject(const uint16_t &shopId,const uint16_t &objectId,const uint32_t &quantity,const uint32_t &price)/// \see CommonMap, std::unordered_map<std::pair<uint8_t,uint8_t>,std::vector<uint16_t>, pairhash> shops;
{
    if(!is_logged)
    {
        std::cerr << "is not logged, line: " << __FILE__ << ": " << __LINE__ << std::endl;
        return;
    }
    if(!character_selected)
    {
        std::cerr << "character not selected, line: " << __FILE__ << ": " << __LINE__ << std::endl;
        return;
    }
    QByteArray outputData;
    QDataStream out(&outputData, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_4);out.setByteOrder(QDataStream::LittleEndian);
    out << (uint16_t)shopId;
    out << (uint16_t)objectId;
    out << (uint32_t)quantity;
    out << (uint32_t)price;
    packOutcommingQuery(0x89,queryNumber(),outputData.constData(),outputData.size());
}

void Api_protocol_2::getFactoryList(const uint16_t &factoryId)
{
    if(!is_logged)
    {
        std::cerr << "is not logged, line: " << __FILE__ << ": " << __LINE__ << std::endl;
        return;
    }
    if(!character_selected)
    {
        std::cerr << "character not selected, line: " << __FILE__ << ": " << __LINE__ << std::endl;
        return;
    }
    QByteArray outputData;
    QDataStream out(&outputData, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_4);out.setByteOrder(QDataStream::LittleEndian);
    out << (uint16_t)factoryId;
    packOutcommingQuery(0x8A,queryNumber(),outputData.constData(),outputData.size());
}

void Api_protocol_2::buyFactoryProduct(const uint16_t &factoryId,const uint16_t &objectId,const uint32_t &quantity,const uint32_t &price)
{
    if(!is_logged)
    {
        std::cerr << "is not logged, line: " << __FILE__ << ": " << __LINE__ << std::endl;
        return;
    }
    if(!character_selected)
    {
        std::cerr << "character not selected, line: " << __FILE__ << ": " << __LINE__ << std::endl;
        return;
    }
    QByteArray outputData;
    QDataStream out(&outputData, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_4);out.setByteOrder(QDataStream::LittleEndian);
    out << (uint16_t)factoryId;
    out << (uint16_t)objectId;
    out << (uint32_t)quantity;
    out << (uint32_t)price;
    packOutcommingQuery(0x8B,queryNumber(),outputData.constData(),outputData.size());
}

void Api_protocol_2::sellFactoryResource(const uint16_t &factoryId,const uint16_t &objectId,const uint32_t &quantity,const uint32_t &price)
{
    if(!is_logged)
    {
        std::cerr << "is not logged, line: " << __FILE__ << ": " << __LINE__ << std::endl;
        return;
    }
    if(!character_selected)
    {
        std::cerr << "character not selected, line: " << __FILE__ << ": " << __LINE__ << std::endl;
        return;
    }
    QByteArray outputData;
    QDataStream out(&outputData, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_4);out.setByteOrder(QDataStream::LittleEndian);
    out << (uint16_t)factoryId;
    out << (uint16_t)objectId;
    out << (uint32_t)quantity;
    out << (uint32_t)price;
    packOutcommingQuery(0x8C,queryNumber(),outputData.constData(),outputData.size());
}

void Api_protocol_2::tryEscape()
{
    if(!is_logged)
    {
        std::cerr << "is not logged, line: " << __FILE__ << ": " << __LINE__ << std::endl;
        return;
    }
    if(!character_selected)
    {
        std::cerr << "character not selected, line: " << __FILE__ << ": " << __LINE__ << std::endl;
        return;
    }
    packOutcommingData(0x07,NULL,0);
}

void Api_protocol_2::heal()
{
    if(!is_logged)
    {
        std::cerr << "is not logged, line: " << __FILE__ << ": " << __LINE__ << std::endl;
        return;
    }
    if(!character_selected)
    {
        std::cerr << "character not selected, line: " << __FILE__ << ": " << __LINE__ << std::endl;
        return;
    }
    packOutcommingData(0x0B,NULL,0);
}

void Api_protocol_2::requestFight(const uint16_t &fightId)
{
    if(!is_logged)
    {
        std::cerr << "is not logged, line: " << __FILE__ << ": " << __LINE__ << std::endl;
        return;
    }
    if(!character_selected)
    {
        std::cerr << "character not selected, line: " << __FILE__ << ": " << __LINE__ << std::endl;
        return;
    }
    if(player_informations.bot_already_beaten==NULL)
    {
        std::cerr << "player_informations.bot_already_beaten==NULL, line: " << __FILE__ << ": " << __LINE__ << std::endl;
        abort();
    }
    if(player_informations.bot_already_beaten[fightId/8] & (1<<(7-fightId%8)))
    {
        std::cerr << "player_informations.bot_already_beaten["+std::to_string(fightId)+"], line: " << __FILE__ << ": " << __LINE__ << std::endl;
        abort();
    }
    QByteArray outputData;
    QDataStream out(&outputData, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_4);out.setByteOrder(QDataStream::LittleEndian);
    out << (uint16_t)fightId;
    packOutcommingData(0x0C,outputData.constData(),outputData.size());
}

void Api_protocol_2::changeOfMonsterInFightByPosition(const uint8_t &monsterPosition)
{
    if(!is_logged)
    {
        std::cerr << "is not logged, line: " << __FILE__ << ": " << __LINE__ << std::endl;
        return;
    }
    if(!character_selected)
    {
        std::cerr << "character not selected, line: " << __FILE__ << ": " << __LINE__ << std::endl;
        return;
    }
    QByteArray outputData;
    QDataStream out(&outputData, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_4);out.setByteOrder(QDataStream::LittleEndian);
    out << (uint8_t)monsterPosition;
    packOutcommingData(0x0E,outputData.constData(),outputData.size());
}

void Api_protocol_2::useSkill(const uint16_t &skill)
{
    if(!is_logged)
    {
        std::cerr << "is not logged, line: " << __FILE__ << ": " << __LINE__ << std::endl;
        return;
    }
    if(!character_selected)
    {
        std::cerr << "character not selected, line: " << __FILE__ << ": " << __LINE__ << std::endl;
        return;
    }
    QByteArray outputData;
    QDataStream out(&outputData, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_4);out.setByteOrder(QDataStream::LittleEndian);
    out << (uint16_t)skill;
    packOutcommingData(0x11,outputData.constData(),outputData.size());
}

void Api_protocol_2::learnSkillByPosition(const uint8_t &monsterPosition,const uint16_t &skill)
{
    if(!is_logged)
    {
        std::cerr << "is not logged, line: " << __FILE__ << ": " << __LINE__ << std::endl;
        return;
    }
    if(!character_selected)
    {
        std::cerr << "character not selected, line: " << __FILE__ << ": " << __LINE__ << std::endl;
        return;
    }
    QByteArray outputData;
    QDataStream out(&outputData, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_4);out.setByteOrder(QDataStream::LittleEndian);
    out << (uint8_t)monsterPosition;
    out << (uint16_t)skill;
    packOutcommingData(0x09,outputData.constData(),outputData.size());
}

void Api_protocol_2::startQuest(const uint16_t &questId)
{
    if(!is_logged)
    {
        std::cerr << "is not logged, line: " << __FILE__ << ": " << __LINE__ << std::endl;
        return;
    }
    if(!character_selected)
    {
        std::cerr << "character not selected, line: " << __FILE__ << ": " << __LINE__ << std::endl;
        return;
    }
    QByteArray outputData;
    QDataStream out(&outputData, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_4);out.setByteOrder(QDataStream::LittleEndian);
    out << (uint16_t)questId;
    packOutcommingData(0x1B,outputData.constData(),outputData.size());
}

void Api_protocol_2::finishQuest(const uint16_t &questId)
{
    if(!is_logged)
    {
        std::cerr << "is not logged, line: " << __FILE__ << ": " << __LINE__ << std::endl;
        return;
    }
    if(!character_selected)
    {
        std::cerr << "character not selected, line: " << __FILE__ << ": " << __LINE__ << std::endl;
        return;
    }
    QByteArray outputData;
    QDataStream out(&outputData, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_4);out.setByteOrder(QDataStream::LittleEndian);
    out << (uint16_t)questId;
    packOutcommingData(0x1C,outputData.constData(),outputData.size());
}

void Api_protocol_2::cancelQuest(const uint16_t &questId)
{
    if(!is_logged)
    {
        std::cerr << "is not logged, line: " << __FILE__ << ": " << __LINE__ << std::endl;
        return;
    }
    if(!character_selected)
    {
        std::cerr << "character not selected, line: " << __FILE__ << ": " << __LINE__ << std::endl;
        return;
    }
    QByteArray outputData;
    QDataStream out(&outputData, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_4);out.setByteOrder(QDataStream::LittleEndian);
    out << (uint16_t)questId;
    packOutcommingData(0x1D,outputData.constData(),outputData.size());
}

void Api_protocol_2::nextQuestStep(const uint16_t &questId)
{
    if(!is_logged)
    {
        std::cerr << "is not logged, line: " << __FILE__ << ": " << __LINE__ << std::endl;
        return;
    }
    if(!character_selected)
    {
        std::cerr << "character not selected, line: " << __FILE__ << ": " << __LINE__ << std::endl;
        return;
    }
    QByteArray outputData;
    QDataStream out(&outputData, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_4);out.setByteOrder(QDataStream::LittleEndian);
    out << (uint16_t)questId;
    packOutcommingData(0x1E,outputData.constData(),outputData.size());
}

void Api_protocol_2::createClan(const std::string &name)
{
    if(!is_logged)
    {
        std::cerr << "is not logged, line: " << __FILE__ << ": " << __LINE__ << std::endl;
        return;
    }
    if(!character_selected)
    {
        std::cerr << "character not selected, line: " << __FILE__ << ": " << __LINE__ << std::endl;
        return;
    }
    QByteArray outputData;
    QDataStream out(&outputData, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_4);out.setByteOrder(QDataStream::LittleEndian);
    out << (uint8_t)0x01;
    {
        const std::string &rawText=toUTF8WithHeader(name);
        if(rawText.size()>255 || rawText.empty())
        {
            std::cerr << "rawText too big or not compatible with utf8" << std::endl;
            return;
        }
        outputData+=QByteArray(rawText.data(),rawText.size());
        out.device()->seek(out.device()->size());
    }
    packOutcommingQuery(0x92,queryNumber(),outputData.constData(),outputData.size());
}

void Api_protocol_2::leaveClan()
{
    if(!is_logged)
    {
        std::cerr << "is not logged, line: " << __FILE__ << ": " << __LINE__ << std::endl;
        return;
    }
    if(!character_selected)
    {
        std::cerr << "character not selected, line: " << __FILE__ << ": " << __LINE__ << std::endl;
        return;
    }
    QByteArray outputData;
    QDataStream out(&outputData, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_4);out.setByteOrder(QDataStream::LittleEndian);
    out << (uint8_t)0x02;
    packOutcommingQuery(0x92,queryNumber(),outputData.constData(),outputData.size());
}

void Api_protocol_2::dissolveClan()
{
    if(!is_logged)
    {
        std::cerr << "is not logged, line: " << __FILE__ << ": " << __LINE__ << std::endl;
        return;
    }
    if(!character_selected)
    {
        std::cerr << "character not selected, line: " << __FILE__ << ": " << __LINE__ << std::endl;
        return;
    }
    QByteArray outputData;
    QDataStream out(&outputData, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_4);out.setByteOrder(QDataStream::LittleEndian);
    out << (uint8_t)0x03;
    packOutcommingQuery(0x92,queryNumber(),outputData.constData(),outputData.size());
}

void Api_protocol_2::inviteClan(const std::string &pseudo)
{
    if(!is_logged)
    {
        std::cerr << "is not logged, line: " << __FILE__ << ": " << __LINE__ << std::endl;
        return;
    }
    if(!character_selected)
    {
        std::cerr << "character not selected, line: " << __FILE__ << ": " << __LINE__ << std::endl;
        return;
    }
    QByteArray outputData;
    QDataStream out(&outputData, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_4);out.setByteOrder(QDataStream::LittleEndian);
    out << (uint8_t)0x04;
    {
        const std::string &rawText=toUTF8WithHeader(pseudo);
        if(rawText.size()>255 || rawText.empty())
        {
            std::cerr << "rawText too big or not compatible with utf8" << std::endl;
            return;
        }
        outputData+=QByteArray(rawText.data(),rawText.size());
        out.device()->seek(out.device()->size());
    }
    packOutcommingQuery(0x92,queryNumber(),outputData.constData(),outputData.size());
}

void Api_protocol_2::ejectClan(const std::string &pseudo)
{
    if(!is_logged)
    {
        std::cerr << "is not logged, line: " << __FILE__ << ": " << __LINE__ << std::endl;
        return;
    }
    if(!character_selected)
    {
        std::cerr << "character not selected, line: " << __FILE__ << ": " << __LINE__ << std::endl;
        return;
    }
    QByteArray outputData;
    QDataStream out(&outputData, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_4);out.setByteOrder(QDataStream::LittleEndian);
    out << (uint8_t)0x05;
    {
        const std::string &rawText=toUTF8WithHeader(pseudo);
        if(rawText.size()>255 || rawText.empty())
        {
            std::cerr << "rawText too big or not compatible with utf8" << std::endl;
            return;
        }
        outputData+=QByteArray(rawText.data(),rawText.size());
        out.device()->seek(out.device()->size());
    }
    packOutcommingQuery(0x92,queryNumber(),outputData.constData(),outputData.size());
}

void Api_protocol_2::inviteAccept(const bool &accept)
{
    if(!is_logged)
    {
        std::cerr << "is not logged, line: " << __FILE__ << ": " << __LINE__ << std::endl;
        return;
    }
    if(!character_selected)
    {
        std::cerr << "character not selected, line: " << __FILE__ << ": " << __LINE__ << std::endl;
        return;
    }
    QByteArray outputData;
    QDataStream out(&outputData, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_4);out.setByteOrder(QDataStream::LittleEndian);
    if(accept)
        out << (uint8_t)0x01;
    else
        out << (uint8_t)0x02;
    packOutcommingData(0x04,outputData.constData(),outputData.size());
}

void Api_protocol_2::waitingForCityCapture(const bool &cancel)
{
    if(!is_logged)
    {
        std::cerr << "is not logged, line: " << __FILE__ << ": " << __LINE__ << std::endl;
        return;
    }
    if(!character_selected)
    {
        std::cerr << "character not selected, line: " << __FILE__ << ": " << __LINE__ << std::endl;
        return;
    }
    QByteArray outputData;
    QDataStream out(&outputData, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_4);out.setByteOrder(QDataStream::LittleEndian);
    if(!cancel)
        out << (uint8_t)0x00;
    else
        out << (uint8_t)0x01;
    packOutcommingData(0x1F,outputData.constData(),outputData.size());
}

//market
void Api_protocol_2::getMarketList()
{
    if(!is_logged)
    {
        std::cerr << "is not logged, line: " << __FILE__ << ": " << __LINE__ << std::endl;
        return;
    }
    if(!character_selected)
    {
        std::cerr << "character not selected, line: " << __FILE__ << ": " << __LINE__ << std::endl;
        return;
    }
    packOutcommingQuery(0x8D,queryNumber(),NULL,0);
}

void Api_protocol_2::buyMarketObject(const uint32_t &marketObjectId, const uint32_t &quantity)
{
    if(!is_logged)
    {
        std::cerr << "is not logged, line: " << __FILE__ << ": " << __LINE__ << std::endl;
        return;
    }
    if(!character_selected)
    {
        std::cerr << "character not selected, line: " << __FILE__ << ": " << __LINE__ << std::endl;
        return;
    }
    QByteArray outputData;
    QDataStream out(&outputData, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_4);out.setByteOrder(QDataStream::LittleEndian);
    out << (uint8_t)0x01;
    out << marketObjectId;
    out << quantity;
    packOutcommingQuery(0x8E,queryNumber(),outputData.constData(),outputData.size());
}

void Api_protocol_2::buyMarketMonster(const uint32_t &monsterMarketId)
{
    if(!is_logged)
    {
        std::cerr << "is not logged, line: " << __FILE__ << ": " << __LINE__ << std::endl;
        return;
    }
    if(!character_selected)
    {
        std::cerr << "character not selected, line: " << __FILE__ << ": " << __LINE__ << std::endl;
        return;
    }
    QByteArray outputData;
    QDataStream out(&outputData, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_4);out.setByteOrder(QDataStream::LittleEndian);
    out << (uint8_t)0x02;
    out << monsterMarketId;
    packOutcommingQuery(0x8E,queryNumber(),outputData.constData(),outputData.size());
}

void Api_protocol_2::putMarketObject(const uint16_t &objectId, const uint32_t &quantity, const uint64_t &price)
{
    if(!is_logged)
    {
        std::cerr << "is not logged, line: " << __FILE__ << ": " << __LINE__ << std::endl;
        return;
    }
    if(!character_selected)
    {
        std::cerr << "character not selected, line: " << __FILE__ << ": " << __LINE__ << std::endl;
        return;
    }
    QByteArray outputData;
    QDataStream out(&outputData, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_4);out.setByteOrder(QDataStream::LittleEndian);
    out << (uint8_t)0x01;
    out << (quint16)objectId;
    out << (quint32)quantity;
    out << (quint64)price;
    packOutcommingQuery(0x8F,queryNumber(),outputData.constData(),outputData.size());
}

void Api_protocol_2::putMarketMonsterByPosition(const uint8_t &monsterPosition,const uint64_t &price)
{
    if(!is_logged)
    {
        std::cerr << "is not logged, line: " << __FILE__ << ": " << __LINE__ << std::endl;
        return;
    }
    if(!character_selected)
    {
        std::cerr << "character not selected, line: " << __FILE__ << ": " << __LINE__ << std::endl;
        return;
    }
    QByteArray outputData;
    QDataStream out(&outputData, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_4);out.setByteOrder(QDataStream::LittleEndian);
    out << (uint8_t)0x02;
    out << (uint8_t)monsterPosition;
    out << (quint64)price;
    packOutcommingQuery(0x8F,queryNumber(),outputData.constData(),outputData.size());
}

void Api_protocol_2::recoverMarketCash()
{
    if(!is_logged)
    {
        std::cerr << "is not logged, line: " << __FILE__ << ": " << __LINE__ << std::endl;
        return;
    }
    if(!character_selected)
    {
        std::cerr << "character not selected, line: " << __FILE__ << ": " << __LINE__ << std::endl;
        return;
    }
    packOutcommingQuery(0x90,queryNumber(),NULL,0);
}

void Api_protocol_2::withdrawMarketObject(const uint16_t &objectPosition,const uint32_t &quantity)
{
    if(!is_logged)
    {
        std::cerr << "is not logged, line: " << __FILE__ << ": " << __LINE__ << std::endl;
        return;
    }
    if(!character_selected)
    {
        std::cerr << "character not selected, line: " << __FILE__ << ": " << __LINE__ << std::endl;
        return;
    }
    QByteArray outputData;
    QDataStream out(&outputData, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_4);out.setByteOrder(QDataStream::LittleEndian);
    out << (uint8_t)0x01;
    out << objectPosition;
    out << quantity;
    packOutcommingQuery(0x91,queryNumber(),outputData.constData(),outputData.size());
}

void Api_protocol_2::withdrawMarketMonster(const uint32_t &monsterMarketId)
{
    if(!is_logged)
    {
        std::cerr << "is not logged, line: " << __FILE__ << ": " << __LINE__ << std::endl;
        return;
    }
    if(!character_selected)
    {
        std::cerr << "character not selected, line: " << __FILE__ << ": " << __LINE__ << std::endl;
        return;
    }
    QByteArray outputData;
    QDataStream out(&outputData, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_4);out.setByteOrder(QDataStream::LittleEndian);
    out << (uint8_t)0x02;
    out << monsterMarketId;
    packOutcommingQuery(0x91,queryNumber(),outputData.constData(),outputData.size());
}

void Api_protocol_2::collectMaturePlant()
{
    if(!is_logged)
    {
        std::cerr << "is not logged, line: " << __FILE__ << ": " << __LINE__ << std::endl;
        return;
    }
    if(!character_selected)
    {
        std::cerr << "character not selected, line: " << __FILE__ << ": " << __LINE__ << std::endl;
        return;
    }
    if(CommonSettingsServer::commonSettingsServer.plantOnlyVisibleByPlayer==false)
        packOutcommingQuery(0x84,queryNumber(),NULL,0);
    else
        packOutcommingData(0x1A,NULL,0);
}

//crafting
void Api_protocol_2::useRecipe(const uint16_t &recipeId)
{
    if(!is_logged)
    {
        std::cerr << "is not logged, line: " << __FILE__ << ": " << __LINE__ << std::endl;
        return;
    }
    if(!character_selected)
    {
        std::cerr << "character not selected, line: " << __FILE__ << ": " << __LINE__ << std::endl;
        return;
    }
    QByteArray outputData;
    QDataStream out(&outputData, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_4);out.setByteOrder(QDataStream::LittleEndian);
    out << (uint16_t)recipeId;
    packOutcommingQuery(0x85,queryNumber(),outputData.constData(),outputData.size());
}

void Api_protocol_2::addRecipe(const uint16_t &recipeId)
{
    if(player_informations.recipes==NULL)
    {
        std::cerr << "player_informations.recipes NULL, line: " << __FILE__ << ": " << __LINE__ << std::endl;
        return;
    }
    player_informations.recipes[recipeId/8]|=(1<<(7-recipeId%8));
}

void Api_protocol_2::battleRefused()
{
    if(!is_logged)
    {
        std::cerr << "is not logged, line: " << __FILE__ << ": " << __LINE__ << std::endl;
        return;
    }
    if(!character_selected)
    {
        std::cerr << "character not selected, line: " << __FILE__ << ": " << __LINE__ << std::endl;
        return;
    }
    if(battleRequestId.empty())
    {
        newError(std::string("Internal problem"),std::string("no battle request to refuse"));
        return;
    }
    QByteArray outputData;
    QDataStream out(&outputData, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_4);out.setByteOrder(QDataStream::LittleEndian);
    out << (uint8_t)0x02;
    postReplyData(battleRequestId.front(),outputData.data(),outputData.size());
    battleRequestId.erase(battleRequestId.cbegin());
}

void Api_protocol_2::battleAccepted()
{
    if(!is_logged)
    {
        std::cerr << "is not logged, line: " << __FILE__ << ": " << __LINE__ << std::endl;
        return;
    }
    if(!character_selected)
    {
        std::cerr << "character not selected, line: " << __FILE__ << ": " << __LINE__ << std::endl;
        return;
    }
    if(battleRequestId.empty())
    {
        newError(std::string("Internal problem"),std::string("no battle request to accept"));
        return;
    }
    QByteArray outputData;
    QDataStream out(&outputData, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_4);out.setByteOrder(QDataStream::LittleEndian);
    out << (uint8_t)0x01;
    postReplyData(battleRequestId.front(),outputData.data(),outputData.size());
    battleRequestId.erase(battleRequestId.cbegin());
}

//trade
void Api_protocol_2::tradeRefused()
{
    if(!is_logged)
    {
        std::cerr << "is not logged, line: " << __FILE__ << ": " << __LINE__ << std::endl;
        return;
    }
    if(!character_selected)
    {
        std::cerr << "character not selected, line: " << __FILE__ << ": " << __LINE__ << std::endl;
        return;
    }
    if(tradeRequestId.empty())
    {
        newError(std::string("Internal problem"),std::string("no trade request to refuse"));
        return;
    }
    QByteArray outputData;
    QDataStream out(&outputData, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_4);out.setByteOrder(QDataStream::LittleEndian);
    out << (uint8_t)0x02;
    postReplyData(tradeRequestId.front(),outputData.data(),outputData.size());
    tradeRequestId.erase(tradeRequestId.cbegin());
}

void Api_protocol_2::tradeAccepted()
{
    if(!is_logged)
    {
        std::cerr << "is not logged, line: " << __FILE__ << ": " << __LINE__ << std::endl;
        return;
    }
    if(!character_selected)
    {
        std::cerr << "character not selected, line: " << __FILE__ << ": " << __LINE__ << std::endl;
        return;
    }
    if(tradeRequestId.empty())
    {
        newError(std::string("Internal problem"),std::string("no trade request to accept"));
        return;
    }
    QByteArray outputData;
    QDataStream out(&outputData, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_4);out.setByteOrder(QDataStream::LittleEndian);
    out << (uint8_t)0x01;
    postReplyData(tradeRequestId.front(),outputData.constData(),outputData.size());
    tradeRequestId.erase(tradeRequestId.cbegin());
    isInTrade=true;
}

void Api_protocol_2::tradeCanceled()
{
    if(!is_logged)
    {
        std::cerr << "is not logged, line: " << __FILE__ << ": " << __LINE__ << std::endl;
        return;
    }
    if(!character_selected)
    {
        std::cerr << "character not selected, line: " << __FILE__ << ": " << __LINE__ << std::endl;
        return;
    }
    if(!isInTrade)
    {
        newError(std::string("Internal problem"),std::string("in not in trade"));
        return;
    }
    isInTrade=false;
    packOutcommingData(0x16,NULL,0);
}

void Api_protocol_2::tradeFinish()
{
    if(!is_logged)
    {
        std::cerr << "is not logged, line: " << __FILE__ << ": " << __LINE__ << std::endl;
        return;
    }
    if(!character_selected)
    {
        std::cerr << "character not selected, line: " << __FILE__ << ": " << __LINE__ << std::endl;
        return;
    }
    if(!isInTrade)
    {
        newError(std::string("Internal problem"),std::string("in not in trade"));
        return;
    }
    packOutcommingData(0x15,NULL,0);
}

void Api_protocol_2::addTradeCash(const uint64_t &cash)
{
    if(!is_logged)
    {
        std::cerr << "is not logged, line: " << __FILE__ << ": " << __LINE__ << std::endl;
        return;
    }
    if(!character_selected)
    {
        std::cerr << "character not selected, line: " << __FILE__ << ": " << __LINE__ << std::endl;
        return;
    }
    if(cash==0)
    {
        newError(std::string("Internal problem"),std::string("can't send 0 for the cash"));
        return;
    }
    if(!isInTrade)
    {
        newError(std::string("Internal problem"),std::string("no in trade to send cash"));
        return;
    }
    QByteArray outputData;
    QDataStream out(&outputData, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_4);out.setByteOrder(QDataStream::LittleEndian);
    out << (uint8_t)0x01;
    out << (quint64)cash;
    packOutcommingData(0x14,outputData.data(),outputData.size());
}

void Api_protocol_2::addObject(const uint16_t &item, const uint32_t &quantity)
{
    if(!is_logged)
    {
        std::cerr << "is not logged, line: " << __FILE__ << ": " << __LINE__ << std::endl;
        return;
    }
    if(!character_selected)
    {
        std::cerr << "character not selected, line: " << __FILE__ << ": " << __LINE__ << std::endl;
        return;
    }
    if(quantity==0)
    {
        newError(std::string("Internal problem"),std::string("can't send a quantity of 0"));
        return;
    }
    if(!isInTrade)
    {
        newError(std::string("Internal problem"),std::string("no in trade to send object"));
        return;
    }
    QByteArray outputData;
    QDataStream out(&outputData, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_4);out.setByteOrder(QDataStream::LittleEndian);
    out << (uint8_t)0x02;
    out << item;
    out << quantity;
    packOutcommingData(0x14,outputData.constData(),outputData.size());
}

void Api_protocol_2::addMonsterByPosition(const uint8_t &monsterPosition)
{
    if(!is_logged)
    {
        std::cerr << "is not logged, line: " << __FILE__ << ": " << __LINE__ << std::endl;
        return;
    }
    if(!character_selected)
    {
        std::cerr << "character not selected, line: " << __FILE__ << ": " << __LINE__ << std::endl;
        return;
    }
    if(!isInTrade)
    {
        newError(std::string("Internal problem"),std::string("no in trade to send monster"));
        return;
    }
    QByteArray outputData;
    QDataStream out(&outputData, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_4);out.setByteOrder(QDataStream::LittleEndian);
    out << (uint8_t)0x03;
    out << monsterPosition;
    packOutcommingData(0x14,outputData.constData(),outputData.size());
}

Api_protocol_2::StageConnexion Api_protocol_2::stage() const
{
    return stageConnexion;
}

//to reset all
void Api_protocol_2::resetAll()
{
    if(stageConnexion==StageConnexion::Stage2)
        qDebug() << "Api_protocol_2::resetAll() Suspect internal bug";
    //status for the query
    token.clear();
    message("Api_protocol_2::resetAll(): stageConnexion=CatchChallenger::Api_protocol_2::StageConnexion::Stage1 set at "+std::string(__FILE__)+":"+std::to_string(__LINE__));
    stageConnexion=StageConnexion::Stage1;
    if(socket==NULL || socket->fakeSocket==NULL)
        haveFirstHeader=false;
    else
        haveFirstHeader=true;
    character_select_send=false;
    delayedLogin.data.clear();
    delayedLogin.packetCode=0;
    delayedLogin.queryNumber=0;
    number_of_map=0;
    delayedMessages.clear();
    haveTheServerList=false;
    haveTheLogicalGroupList=false;
    is_logged=false;
    character_selected=false;
    have_send_protocol=false;
    have_receive_protocol=false;
    max_players=65535;
    max_players_real=65535;
    selectedServerIndex=-1;
    player_informations.allow.clear();
    player_informations.cash=0;
    player_informations.clan=0;
    player_informations.clan_leader=false;
    player_informations.warehouse_cash=0;
    player_informations.warehouse_items.clear();
    player_informations.warehouse_playerMonster.clear();
    player_informations.public_informations.pseudo.clear();
    player_informations.public_informations.simplifiedId=0;
    player_informations.public_informations.skinId=0;
    player_informations.public_informations.speed=0;
    player_informations.public_informations.type=Player_type_normal;
    player_informations.repel_step=0;
    player_informations.playerMonster.clear();
    player_informations.items.clear();
    player_informations.reputation.clear();
    player_informations.quests.clear();
    player_informations.itemOnMap.clear();
    player_informations.plantOnMap.clear();
    tokenForGameServer.clear();
    //to move by unit
    last_step=255;
    last_direction=Direction_look_at_bottom;
    last_direction_is_set=false;

    unloadSelection();
    isInTrade=false;
    tradeRequestId.clear();
    isInBattle=false;
    battleRequestId.clear();
    mDatapackBase=QCoreApplication::applicationDirPath().toStdString()+"/datapack/";
    mDatapackMain=mDatapackBase+"map/main/[main]/";
    mDatapackSub=mDatapackMain+"sub/[sub]/";
    CommonSettingsServer::commonSettingsServer.mainDatapackCode="[main]";
    CommonSettingsServer::commonSettingsServer.subDatapackCode="[sub]";
    if(player_informations.recipes!=NULL)
    {
        delete player_informations.recipes;
        player_informations.recipes=NULL;
    }
    if(player_informations.encyclopedia_monster!=NULL)
    {
        delete player_informations.encyclopedia_monster;
        player_informations.encyclopedia_monster=NULL;
    }
    if(player_informations.encyclopedia_item!=NULL)
    {
        delete player_informations.encyclopedia_item;
        player_informations.encyclopedia_item=NULL;
    }
    if(player_informations.bot_already_beaten!=NULL)
    {
        delete player_informations.bot_already_beaten;
        player_informations.bot_already_beaten=NULL;
    }

    ProtocolParsingInputOutput::reset();
    flags|=0x08;
}

void Api_protocol_2::unloadSelection()
{
    selectedServerIndex=-1;
    logicialGroup.logicialGroupList.clear();
    serverOrdenedList.clear();
    characterListForSelection.clear();
    logicialGroup.name.clear();
    logicialGroup.servers.clear();
}

const ServerFromPoolForDisplay &Api_protocol_2::getCurrentServer(const unsigned int &index)
{
    if(index>=serverOrdenedList.size())
        abort();
    return serverOrdenedList.at(index);
}

std::string Api_protocol_2::datapackPathBase() const
{
    return mDatapackBase;
}

std::string Api_protocol_2::datapackPathMain() const
{
    return mDatapackMain;
}

std::string Api_protocol_2::datapackPathSub() const
{
    return mDatapackSub;
}

std::string Api_protocol_2::mainDatapackCode() const
{
    return CommonSettingsServer::commonSettingsServer.mainDatapackCode;
}

std::string Api_protocol_2::subDatapackCode() const
{
    return CommonSettingsServer::commonSettingsServer.subDatapackCode;
}

void Api_protocol_2::setDatapackPath(const std::string &datapack_path)
{
    if(stringEndsWith(datapack_path,'/'))
        mDatapackBase=datapack_path;
    else
        mDatapackBase=datapack_path+"/";
    mDatapackMain=mDatapackBase+"map/main/[main]/";
    mDatapackSub=mDatapackMain+"sub/[sub]/";
    CommonSettingsServer::commonSettingsServer.mainDatapackCode="[main]";
    CommonSettingsServer::commonSettingsServer.subDatapackCode="[sub]";
}

bool Api_protocol_2::getIsLogged() const
{
    return is_logged;
}

bool Api_protocol_2::getCaracterSelected() const
{
    return character_selected;
}

LogicialGroup * Api_protocol_2::addLogicalGroup(const std::string &path,const std::string &xml,const std::string &language)
{
    std::string nameString;

    tinyxml2::XMLDocument domDocument;
    const auto loadOkay = domDocument.Parse((Api_protocol_2::text_balise_root_start+xml+Api_protocol_2::text_balise_root_stop).c_str());
    if(loadOkay!=0)
    {
        std::cerr << "Api_protocol_2::addLogicalGroup(): "+tinyxml2errordoc(&domDocument) << std::endl;
        return NULL;
    }
    else
    {
        const tinyxml2::XMLElement *root = domDocument.RootElement();
        //load the name
        {
            bool name_found=false;
            const tinyxml2::XMLElement * name = root->FirstChildElement("name");
            if(!language.empty() && language!="en")
                while(name!=NULL)
                {
                    if(name->Attribute("lang")!=NULL && name->Attribute("lang")==language && name->GetText()!=NULL)
                    {
                        nameString=name->GetText();
                        name_found=true;
                        break;
                    }
                    name = name->NextSiblingElement("name");
                }
            if(!name_found)
            {
                name = root->FirstChildElement("name");
                while(name!=NULL)
                {
                    if(name->Attribute("lang")==NULL || strcmp(name->Attribute("lang"),"en")==0)
                        if(name->GetText()!=NULL)
                        {
                            nameString=name->GetText();
                            name_found=true;
                            break;
                        }
                    name = name->NextSiblingElement("name");
                }
            }
            if(!name_found)
            {
                //normal case, the group can be without any name
            }
        }
    }
    LogicialGroup *logicialGroupCursor=&this->logicialGroup;
    std::vector<std::string> pathSplited=stringsplit(path,'/');
    while(!pathSplited.empty())
    {
        const std::string &node=pathSplited.front();
        if(logicialGroupCursor->logicialGroupList.find(node)==logicialGroupCursor->logicialGroupList.cend())
            logicialGroupCursor->logicialGroupList[node]=new LogicialGroup;
        logicialGroupCursor=logicialGroupCursor->logicialGroupList[node];
        pathSplited.erase(pathSplited.cbegin());
    }
    if(!nameString.empty())
        logicialGroupCursor->name=nameString;
    return logicialGroupCursor;
}

ServerFromPoolForDisplay * Api_protocol_2::addLogicalServer(const ServerFromPoolForDisplayTemp &server, const std::string &language)
{
    std::string nameString;
    std::string descriptionString;

    tinyxml2::XMLDocument domDocument;
    const auto loadOkay = domDocument.Parse((Api_protocol_2::text_balise_root_start+server.xml+Api_protocol_2::text_balise_root_stop).c_str());
    if(loadOkay!=0)
    {
        std::cerr << "Api_protocol_2::addLogicalServer(): "+tinyxml2errordoc(&domDocument) << std::endl;
        return NULL;
    }
    else
    {
        const tinyxml2::XMLElement *root = domDocument.RootElement();

        //load the name
        {
            bool name_found=false;
            const tinyxml2::XMLElement *name = root->FirstChildElement("name");
            if(!language.empty() && language!="en")
                while(name!=NULL)
                {
                    if(name->Attribute("lang")!=NULL && name->Attribute("lang")==language && name->GetText()!=NULL)
                    {
                        nameString=name->GetText();
                        name_found=true;
                        break;
                    }
                    name = name->NextSiblingElement("name");
                }
            if(!name_found)
            {
                name = root->FirstChildElement("name");
                while(name!=NULL)
                {
                    if(name->Attribute("lang")==NULL || strcmp(name->Attribute("lang"),"en")==0)
                        if(name->GetText()!=NULL)
                        {
                            nameString=name->GetText();
                            name_found=true;
                            break;
                        }
                    name = name->NextSiblingElement("name");
                }
            }
            if(!name_found)
            {
                //normal case, the group can be without any name
            }
        }

        //load the description
        {
            bool description_found=false;
            const tinyxml2::XMLElement *description = root->FirstChildElement("description");
            if(!language.empty() && language!="en")
                while(description!=NULL)
                {
                    if(description->Attribute("lang")!=NULL && description->Attribute("lang")==language && description->GetText()!=NULL)
                    {
                        descriptionString=description->GetText();
                        description_found=true;
                        break;
                    }
                    description = description->NextSiblingElement("description");
                }
            if(!description_found)
            {
                description = root->FirstChildElement("description");
                while(description!=NULL)
                {
                    if(description->Attribute("lang")!=NULL || strcmp(description->Attribute("lang"),"en")==0)
                        if(description->GetText()!=NULL)
                        {
                            descriptionString=description->GetText();
                            description_found=true;
                            break;
                        }
                    description = description->NextSiblingElement("description");
                }
            }
            if(!description_found)
            {
                //normal case, the group can be without any description
            }
        }
    }

    LogicialGroup * logicialGroupCursor;
    if(server.logicalGroupIndex>=logicialGroupIndexList.size())
    {
        qDebug() << (QString("out of range for addLogicalGroup: %1, server.logicalGroupIndex %2 <= logicialGroupIndexList.size() %3 (defaulting to root folder)")
                     .arg(QString::fromStdString(server.xml))
                     .arg(server.logicalGroupIndex)
                     .arg(logicialGroupIndexList.size())
                     );
        logicialGroupCursor=&logicialGroup;
    }
    else
        logicialGroupCursor=logicialGroupIndexList.at(server.logicalGroupIndex);

    ServerFromPoolForDisplay newServer;
    newServer.charactersGroupIndex=server.charactersGroupIndex;
    newServer.currentPlayer=server.currentPlayer;
    newServer.description=descriptionString;
    newServer.host=server.host;
    newServer.maxPlayer=server.maxPlayer;
    newServer.name=nameString;
    newServer.port=server.port;
    newServer.uniqueKey=server.uniqueKey;
    newServer.lastConnect=0;
    newServer.playedTime=0;

    logicialGroupCursor->servers.push_back(newServer);
    return &logicialGroupCursor->servers.back();
}

LogicialGroup Api_protocol_2::getLogicialGroup() const
{
    return logicialGroup;
}

void Api_protocol_2::readForFirstHeader()
{
    if(haveFirstHeader)
        return;
    if(socket->sslSocket==NULL)
    {
        newError(std::string("Internal problem"),std::string("Api_protocol_2::readForFirstHeader() socket->sslSocket==NULL"));
        return;
    }
    if(stageConnexion!=StageConnexion::Stage1 && stageConnexion!=StageConnexion::Stage2 && stageConnexion!=StageConnexion::Stage3)
    {
        newError(std::string("Internal problem"),std::string("Api_protocol_2::readForFirstHeader() stageConnexion!=StageConnexion::Stage1 && stageConnexion!=StageConnexion::Stage2"));
        return;
    }
    if(stageConnexion==StageConnexion::Stage2)
    {
        message("stageConnexion=CatchChallenger::Api_protocol_2::StageConnexion::Stage3 set at "+std::string(__FILE__)+":"+std::to_string(__LINE__));
        stageConnexion=StageConnexion::Stage3;
    }
    {
        if(socket->sslSocket->mode()!=QSslSocket::UnencryptedMode)
        {
            newError(std::string("Internal problem"),std::string("socket->sslSocket->mode()!=QSslSocket::UnencryptedMode into Api_protocol_2::readForFirstHeader()"));
            return;
        }
        uint8_t value;
        if(socket->sslSocket->read((char*)&value,sizeof(value))==sizeof(value))
        {
            haveFirstHeader=true;
            if(value==0x01)
            {
                socket->sslSocket->setPeerVerifyMode(QSslSocket::VerifyNone);
                socket->sslSocket->ignoreSslErrors();
                socket->sslSocket->startClientEncryption();
                if(!QObject::connect(socket->sslSocket,&QSslSocket::encrypted,this,&Api_protocol_2::sslHandcheckIsFinished))
                    abort();
            }
            else
                connectTheExternalSocketInternal();
        }
    }
}

void Api_protocol_2::sslHandcheckIsFinished()
{
    connectTheExternalSocketInternal();
}

void Api_protocol_2::connectTheExternalSocketInternal()
{
    if(socket->sslSocket==NULL)
    {
        newError(std::string("Internal problem"),std::string("Api_protocol_2::connectTheExternalSocket() socket->sslSocket==NULL"));
        return;
    }
    if(socket->peerName().isEmpty() || socket->sslSocket->state()!=QSslSocket::SocketState::ConnectedState)
    {
        newError(std::string("Internal problem"),std::string("Api_protocol_2::connectTheExternalSocket() socket->sslSocket->peerAddress()==QHostAddress::Null: ")+
                 socket->peerName().toStdString()+"-"+std::to_string(socket->peerPort())+
                 ", state: "+std::to_string(socket->sslSocket->state())
                 );
        return;
    }
    //check the certificat
    {
        QDir datapackCert(QString("%1/cert/").arg(QStandardPaths::writableLocation(QStandardPaths::DataLocation)));
        datapackCert.mkpath(datapackCert.absolutePath());
        QFile certFile;
        if(stageConnexion==StageConnexion::Stage1)
            certFile.setFileName(datapackCert.absolutePath()+"/"+socket->peerName()+"-"+QString::number(socket->peerPort()));
        else if(stageConnexion==StageConnexion::Stage3 || stageConnexion==StageConnexion::Stage4)
        {
            if(selectedServerIndex==-1)
            {
                parseError("Internal error, file: "+std::string(__FILE__)+":"+std::to_string(__LINE__),std::string("Api_protocol_2::connectTheExternalSocket() selectedServerIndex==-1"));
                return;
            }
            const ServerFromPoolForDisplay &serverFromPoolForDisplay=serverOrdenedList.at(selectedServerIndex);
            certFile.setFileName(
                        datapackCert.absolutePath()+QString("/")+
                                 QString::fromStdString(serverFromPoolForDisplay.host)+QString("-")+
                        QString::number(serverFromPoolForDisplay.port)
                        );
        }
        else
        {
            parseError("Internal error, file: "+std::string(__FILE__)+":"+std::to_string(__LINE__),std::string("Api_protocol_2::connectTheExternalSocket() stageConnexion!=StageConnexion::Stage1/3"));
            return;
        }
        if(certFile.exists())
        {
            if(socket->sslSocket->mode()==QSslSocket::UnencryptedMode)
            {
                #if (!defined(CATCHCHALLENGER_VERSION_SOLO) || defined(CATCHCHALLENGER_MULTI)) && ! defined(BOTTESTCONNECT)
                SslCert sslCert(NULL);
                sslCert.exec();
                if(sslCert.validated())
                    saveCert(certFile.fileName().toStdString());
                else
                {
                    socket->sslSocket->disconnectFromHost();
                    return;
                }
                #endif
            }
            else if(certFile.open(QIODevice::ReadOnly))
            {
                if(socket->sslSocket->peerCertificate().publicKey().toPem()!=certFile.readAll())
                {
                    #if (!defined(CATCHCHALLENGER_VERSION_SOLO) || defined(CATCHCHALLENGER_MULTI)) && ! defined(BOTTESTCONNECT)
                    SslCert sslCert(NULL);
                    sslCert.exec();
                    if(sslCert.validated())
                        saveCert(certFile.fileName().toStdString());
                    else
                    {
                        socket->sslSocket->disconnectFromHost();
                        return;
                    }
                    #endif
                }
                certFile.close();
            }
        }
        else
        {
            if(socket->sslSocket->mode()!=QSslSocket::UnencryptedMode)
                saveCert(certFile.fileName().toStdString());

        }
    }
    //continue the normal procedure
    if(stageConnexion==StageConnexion::Stage1)
        connectedOnLoginServer();
    if(stageConnexion==StageConnexion::Stage2 || stageConnexion==StageConnexion::Stage3)
    {
        message("stageConnexion=CatchChallenger::Api_protocol_2::StageConnexion::Stage3 set at "+std::string(__FILE__)+":"+std::to_string(__LINE__));
        stageConnexion=StageConnexion::Stage3;
        connectedOnGameServer();
    }

    if(stageConnexion==StageConnexion::Stage1)
        if(!QObject::connect(socket,&ConnectedSocket::readyRead,this,&Api_protocol_2::parseIncommingData,Qt::QueuedConnection))//put queued to don't have circular loop Client -> Server -> Client
            abort();
    //need wait the sslHandcheck
    sendProtocol();
    if(socket->bytesAvailable())
        parseIncommingData();
}

void Api_protocol_2::saveCert(const std::string &file)
{
    if(socket->sslSocket==NULL)
        return;
    QFile certFile(QString::fromStdString(file));
    if(socket->sslSocket->mode()==QSslSocket::UnencryptedMode)
        certFile.remove();
    else
    {
        if(certFile.open(QIODevice::WriteOnly))
        {
            qDebug() << "Register the certificate into" << certFile.fileName();
            qDebug() << socket->sslSocket->peerCertificate().issuerInfo(QSslCertificate::Organization);
            qDebug() << socket->sslSocket->peerCertificate().issuerInfo(QSslCertificate::CommonName);
            qDebug() << socket->sslSocket->peerCertificate().issuerInfo(QSslCertificate::LocalityName);
            qDebug() << socket->sslSocket->peerCertificate().issuerInfo(QSslCertificate::OrganizationalUnitName);
            qDebug() << socket->sslSocket->peerCertificate().issuerInfo(QSslCertificate::CountryName);
            qDebug() << socket->sslSocket->peerCertificate().issuerInfo(QSslCertificate::StateOrProvinceName);
            qDebug() << socket->sslSocket->peerCertificate().issuerInfo(QSslCertificate::EmailAddress);
            certFile.write(socket->sslSocket->peerCertificate().publicKey().toPem());
            certFile.close();
        }
    }
}

bool Api_protocol_2::postReplyData(const uint8_t &queryNumber, const char * const data,const int &size)
{
    const quint8 packetCode=inputQueryNumberToPacketCode[queryNumber];
    removeFromQueryReceived(queryNumber);
    const uint8_t &fixedSize=ProtocolParsingBase::packetFixedSize[packetCode+128];
    if(fixedSize!=0xFE)
    {
        if(fixedSize!=size)
        {
            std::cout << "postReplyData() Sended packet size: " << size << ": " << binarytoHexa(data,size)
                      << ", but the fixed packet size defined at: " << std::to_string((int)fixedSize) << std::endl;
            #ifdef CATCHCHALLENGER_EXTRA_CHECK
            abort();
            #endif
            return false;
        }
        //fixed size
        //send the network message
        uint32_t posOutput=0;
        ProtocolParsingBase::tempBigBufferForOutput[posOutput]=0x01;
        posOutput+=1;
        ProtocolParsingBase::tempBigBufferForOutput[posOutput]=queryNumber;
        posOutput+=1;

        memcpy(ProtocolParsingBase::tempBigBufferForOutput+1+1,data,size);
        posOutput+=size;

        return internalSendRawSmallPacket(ProtocolParsingBase::tempBigBufferForOutput,posOutput);
    }
    else
    {
        //dynamic size
        //send the network message
        uint32_t posOutput=0;
        ProtocolParsingBase::tempBigBufferForOutput[posOutput]=0x01;
        posOutput+=1;
        ProtocolParsingBase::tempBigBufferForOutput[posOutput]=queryNumber;
        posOutput+=1+4;
        *reinterpret_cast<uint32_t *>(ProtocolParsingBase::tempBigBufferForOutput+1+1)=htole32(size);//set the dynamic size

        memcpy(ProtocolParsingBase::tempBigBufferForOutput+1+1+4,data,size);
        posOutput+=size;

        return internalSendRawSmallPacket(ProtocolParsingBase::tempBigBufferForOutput,posOutput);
    }
}

bool Api_protocol_2::packOutcommingData(const uint8_t &packetCode,const char * const data,const int &size)
{
    const uint8_t &fixedSize=ProtocolParsingBase::packetFixedSize[packetCode];
    if(fixedSize!=0xFE)
    {
        if(fixedSize!=size)
        {
            std::cout << "packOutcommingData(): Sended packet size: " << size << ": " << binarytoHexa(data,size)
                      << ", but the fixed packet size defined at: " << std::to_string((int)fixedSize) << std::endl;
            #ifdef CATCHCHALLENGER_EXTRA_CHECK
            abort();
            #endif
            return false;
        }
        //fixed size
        //send the network message
        uint32_t posOutput=0;
        ProtocolParsingBase::tempBigBufferForOutput[posOutput]=packetCode;
        posOutput+=1;

        memcpy(ProtocolParsingBase::tempBigBufferForOutput+1,data,size);
        posOutput+=size;

        return internalSendRawSmallPacket(ProtocolParsingBase::tempBigBufferForOutput,posOutput);
    }
    else
    {
        //dynamic size
        //send the network message
        uint32_t posOutput=0;
        ProtocolParsingBase::tempBigBufferForOutput[posOutput]=packetCode;
        posOutput+=1+4;
        *reinterpret_cast<uint32_t *>(ProtocolParsingBase::tempBigBufferForOutput+1)=htole32(size);//set the dynamic size

        memcpy(ProtocolParsingBase::tempBigBufferForOutput+1+4,data,size);
        posOutput+=size;

        return internalSendRawSmallPacket(ProtocolParsingBase::tempBigBufferForOutput,posOutput);
    }
}


bool Api_protocol_2::packOutcommingQuery(const uint8_t &packetCode,const uint8_t &queryNumber,const char * const data,const int &size)
{
    registerOutputQuery(queryNumber,packetCode);
    const uint8_t &fixedSize=ProtocolParsingBase::packetFixedSize[packetCode];
    if(fixedSize!=0xFE)
    {
        if(fixedSize!=size)
        {
            std::cout << "packOutcommingQuery(): Sended packet size: " << size << ": " << binarytoHexa(data,size)
                      << ", but the fixed packet size defined at: " << std::to_string((int)fixedSize) << std::endl;
            #ifdef CATCHCHALLENGER_EXTRA_CHECK
            abort();
            #endif
            return false;
        }
        //fixed size
        //send the network message
        uint32_t posOutput=0;
        ProtocolParsingBase::tempBigBufferForOutput[posOutput]=packetCode;
        posOutput+=1;
        ProtocolParsingBase::tempBigBufferForOutput[posOutput]=queryNumber;
        posOutput+=1;

        memcpy(ProtocolParsingBase::tempBigBufferForOutput+1+1,data,size);
        posOutput+=size;

        return internalSendRawSmallPacket(ProtocolParsingBase::tempBigBufferForOutput,posOutput);
    }
    else
    {
        //dynamic size
        //send the network message
        uint32_t posOutput=0;
        ProtocolParsingBase::tempBigBufferForOutput[posOutput]=packetCode;
        posOutput+=1;
        ProtocolParsingBase::tempBigBufferForOutput[posOutput]=queryNumber;
        posOutput+=1+4;
        *reinterpret_cast<uint32_t *>(ProtocolParsingBase::tempBigBufferForOutput+1+1)=htole32(size);//set the dynamic size

        memcpy(ProtocolParsingBase::tempBigBufferForOutput+1+1+4,data,size);
        posOutput+=size;

        return internalSendRawSmallPacket(ProtocolParsingBase::tempBigBufferForOutput,posOutput);
    }
}

std::string Api_protocol_2::toUTF8WithHeader(const std::string &text)
{
    if(text.empty())
        return std::string();
    if(text.size()>254)
        return std::string();
    std::string data;
    data.resize(1);
    data+=text;
    data[0]=static_cast<uint8_t>(text.size());
    return data;
}

void Api_protocol_2::have_main_and_sub_datapack_loaded()//can now load player_informations
{
    if(!delayedLogin.data.empty())
    {
        bool returnCode=parseCharacterBlockCharacter(delayedLogin.packetCode,delayedLogin.queryNumber,delayedLogin.data);
        delayedLogin.data.clear();
        delayedLogin.packetCode=0;
        delayedLogin.queryNumber=0;
        if(!returnCode)
            return;
    }
    unsigned int index=0;
    while(index<delayedMessages.size())
    {
        const DelayedMessage &delayedMessageTemp=delayedMessages.at(index);
        if(!parseMessage(delayedMessageTemp.packetCode,delayedMessageTemp.data))
        {
            delayedMessages.clear();
            return;
        }
        index++;
    }
    delayedMessages.clear();
}

bool Api_protocol_2::dataToPlayerMonster(QDataStream &in,PlayerMonster &monster)
{
    quint32 sub_index;
    PlayerBuff buff;
    PlayerMonster::PlayerSkill skill;
    /*if(in.device()->pos()<0 || !in.device()->isOpen() || (in.device()->size()-in.device()->pos())<(int)sizeof(uint32_t))
    {
        parseError("Procotol wrong or corrupted",std::string("wrong size to get the monster id bd, line: ")+std::string(__FILE__)+":"+std::to_string(__LINE__));
        return false;
    }
    in >> monster.id;*/
    if(in.device()->pos()<0 || !in.device()->isOpen() || (in.device()->size()-in.device()->pos())<(int)sizeof(uint16_t))
    {
        parseError("Procotol wrong or corrupted",std::string("wrong size to get the monster id, line: ")+std::string(__FILE__)+":"+std::to_string(__LINE__));
        return false;
    }
    in >> monster.monster;
    if(in.device()->pos()<0 || !in.device()->isOpen() || (in.device()->size()-in.device()->pos())<(int)sizeof(uint8_t))
    {
        parseError("Procotol wrong or corrupted",std::string("wrong size to get the monster level, line: ")+std::string(__FILE__)+":"+std::to_string(__LINE__));
        return false;
    }
    in >> monster.level;
    if(in.device()->pos()<0 || !in.device()->isOpen() || (in.device()->size()-in.device()->pos())<(int)sizeof(uint32_t))
    {
        parseError("Procotol wrong or corrupted",std::string("wrong size to get the monster remaining_xp, line: ")+std::string(__FILE__)+":"+std::to_string(__LINE__));
        return false;
    }
    in >> monster.remaining_xp;
    if(in.device()->pos()<0 || !in.device()->isOpen() || (in.device()->size()-in.device()->pos())<(int)sizeof(uint32_t))
    {
        parseError("Procotol wrong or corrupted",std::string("wrong size to get the monster hp, line: ")+std::string(__FILE__)+":"+std::to_string(__LINE__));
        return false;
    }
    in >> monster.hp;
    if(in.device()->pos()<0 || !in.device()->isOpen() || (in.device()->size()-in.device()->pos())<(int)sizeof(uint32_t))
    {
        parseError("Procotol wrong or corrupted",std::string("wrong size to get the monster sp, line: ")+std::string(__FILE__)+":"+std::to_string(__LINE__));
        return false;
    }
    in >> monster.sp;
    if(in.device()->pos()<0 || !in.device()->isOpen() || (in.device()->size()-in.device()->pos())<(int)sizeof(uint16_t))
    {
        parseError("Procotol wrong or corrupted",std::string("wrong size to get the monster captured_with, line: ")+std::string(__FILE__)+":"+std::to_string(__LINE__));
        return false;
    }
    in >> monster.catched_with;
    if(in.device()->pos()<0 || !in.device()->isOpen() || (in.device()->size()-in.device()->pos())<(int)sizeof(uint8_t))
    {
        parseError("Procotol wrong or corrupted",std::string("wrong size to get the monster captured_with, line: ")+std::string(__FILE__)+":"+std::to_string(__LINE__));
        return false;
    }
    quint8 gender;
    in >> gender;
    switch(gender)
    {
        case 0x01:
        case 0x02:
        case 0x03:
            monster.gender=(Gender)gender;
        break;
        default:
            parseError("Procotol wrong or corrupted",std::string("gender code wrong: ")+std::to_string(gender)+", line: "+std::string(__FILE__)+":"+std::to_string(__LINE__));
            return false;
        break;
    }
    if(in.device()->pos()<0 || !in.device()->isOpen() || (in.device()->size()-in.device()->pos())<(int)sizeof(uint32_t))
    {
        parseError("Procotol wrong or corrupted",std::string("wrong size to get the monster egg_step, line: ")+std::string(__FILE__)+":"+std::to_string(__LINE__));
        return false;
    }
    in >> monster.egg_step;
    if(in.device()->pos()<0 || !in.device()->isOpen() || (in.device()->size()-in.device()->pos())<(int)sizeof(uint8_t))
    {
        parseError("Procotol wrong or corrupted",std::string("wrong size to get the monster character_origin, line: ")+std::string(__FILE__)+":"+std::to_string(__LINE__));
        return false;
    }
    {
        quint8 character_origin;
        in >> character_origin;
        monster.character_origin=character_origin;
    }

    if(in.device()->pos()<0 || !in.device()->isOpen() || (in.device()->size()-in.device()->pos())<(int)sizeof(uint8_t))
    {
        parseError("Procotol wrong or corrupted",std::string("wrong size to get the monster size of list of the buff monsters, line: ")+std::string(__FILE__)+":"+std::to_string(__LINE__));
        return false;
    }
    uint8_t sub_size8;
    in >> sub_size8;
    sub_index=0;
    while(sub_index<sub_size8)
    {
        if(in.device()->pos()<0 || !in.device()->isOpen() || (in.device()->size()-in.device()->pos())<(int)sizeof(uint8_t))
        {
            parseError("Procotol wrong or corrupted",std::string("wrong size to get the monster buff, line: ")+std::string(__FILE__)+":"+std::to_string(__LINE__));
            return false;
        }
        in >> buff.buff;
        if(in.device()->pos()<0 || !in.device()->isOpen() || (in.device()->size()-in.device()->pos())<(int)sizeof(uint8_t))
        {
            parseError("Procotol wrong or corrupted",std::string("wrong size to get the monster buff level, line: ")+std::string(__FILE__)+":"+std::to_string(__LINE__));
            return false;
        }
        in >> buff.level;
        if(in.device()->pos()<0 || !in.device()->isOpen() || (in.device()->size()-in.device()->pos())<(int)sizeof(uint8_t))
        {
            parseError("Procotol wrong or corrupted",std::string("wrong size to get the monster buff level, line: ")+std::string(__FILE__)+":"+std::to_string(__LINE__));
            return false;
        }
        in >> buff.remainingNumberOfTurn;
        monster.buffs.push_back(buff);
        sub_index++;
    }

    if(in.device()->pos()<0 || !in.device()->isOpen() || (in.device()->size()-in.device()->pos())<(int)sizeof(uint16_t))
    {
        parseError("Procotol wrong or corrupted",std::string("wrong size to get the monster size of list of the skill monsters, line: ")+std::string(__FILE__)+":"+std::to_string(__LINE__));
        return false;
    }
    uint16_t sub_size16;
    in >> sub_size16;
    sub_index=0;
    while(sub_index<sub_size16)
    {
        if(in.device()->pos()<0 || !in.device()->isOpen() || (in.device()->size()-in.device()->pos())<(int)sizeof(uint16_t))
        {
            parseError("Procotol wrong or corrupted",std::string("wrong size to get the monster skill, line: ")+std::string(__FILE__)+":"+std::to_string(__LINE__));
            return false;
        }
        in >> skill.skill;
        if(in.device()->pos()<0 || !in.device()->isOpen() || (in.device()->size()-in.device()->pos())<(int)sizeof(uint8_t))
        {
            parseError("Procotol wrong or corrupted",std::string("wrong size to get the monster skill level, line: ")+std::string(__FILE__)+":"+std::to_string(__LINE__));
            return false;
        }
        in >> skill.level;
        if(in.device()->pos()<0 || !in.device()->isOpen() || (in.device()->size()-in.device()->pos())<(int)sizeof(uint8_t))
        {
            parseError("Procotol wrong or corrupted",std::string("wrong size to get the monster skill level, line: ")+std::string(__FILE__)+":"+std::to_string(__LINE__));
            return false;
        }
        in >> skill.endurance;
        monster.skills.push_back(skill);
        sub_index++;
    }
    return true;
}

bool Api_protocol_2::setMapNumber(const unsigned int number_of_map)
{
    if(number_of_map==0)
    {
        std::cerr << "to reset this number use resetAll()" << std::endl;
        return false;
    }
    std::cout << "number of map: " << std::to_string(number_of_map) << std::endl;
    this->number_of_map=number_of_map;
    return true;
}

void Api_protocol_2::newError(const std::string &error,const std::string &detailedError)
{
    emit QtnewError(error,detailedError);
}

void Api_protocol_2::message(const std::string &message)
{
    emit Qtmessage(message);
}

void Api_protocol_2::lastReplyTime(const uint32_t &time)
{
    emit QtlastReplyTime(time);
}

//protocol/connection info
void Api_protocol_2::disconnected(const std::string &reason)
{
    emit Qtdisconnected(reason);
}

void Api_protocol_2::notLogged(const std::string &reason)
{
    emit QtnotLogged(reason);
}

void Api_protocol_2::logged(const std::vector<std::vector<CharacterEntry> > &characterEntryList)
{
    emit Qtlogged(characterEntryList);
}

void Api_protocol_2::protocol_is_good()
{
    emit Qtprotocol_is_good();
}
void Api_protocol_2::connectedOnLoginServer()
{
    emit QtconnectedOnLoginServer();
}
void Api_protocol_2::connectingOnGameServer()
{
    emit QtconnectingOnGameServer();
}
void Api_protocol_2::connectedOnGameServer()
{
    emit QtconnectedOnGameServer();
}
void Api_protocol_2::haveDatapackMainSubCode()
{
    emit QthaveDatapackMainSubCode();
}
void Api_protocol_2::gatewayCacheUpdate(const uint8_t gateway,const uint8_t progression)
{
    emit QtgatewayCacheUpdate(gateway,progression);
}

//general info
void Api_protocol_2::number_of_player(const uint16_t &number,const uint16_t &max_players)
{
    emit Qtnumber_of_player(number,max_players);
}
void Api_protocol_2::random_seeds(const std::string &data)
{
    emit Qtrandom_seeds(data);
}

//character
void Api_protocol_2::newCharacterId(const uint8_t &returnCode,const uint32_t &characterId)
{
    emit QtnewCharacterId(returnCode,characterId);
}
void Api_protocol_2::haveCharacter()
{
    emit QthaveCharacter();
}
//events
void Api_protocol_2::setEvents(const std::vector<std::pair<uint8_t,uint8_t> > &events)
{
    emit QtsetEvents(events);
}
void Api_protocol_2::newEvent(const uint8_t &event,const uint8_t &event_value)
{
    emit QtnewEvent(event,event_value);
}

//map move
void Api_protocol_2::insert_player(const CatchChallenger::Player_public_informations &player,const uint32_t &mapId,const uint8_t &x,const uint8_t &y,const CatchChallenger::Direction &direction)
{
    emit Qtinsert_player(player,mapId,x,y,direction);
}
void Api_protocol_2::move_player(const uint16_t &id,const std::vector<std::pair<uint8_t,CatchChallenger::Direction> > &movement)
{
    emit Qtmove_player(id,movement);
}
void Api_protocol_2::remove_player(const uint16_t &id)
{
    emit Qtremove_player(id);
}
void Api_protocol_2::reinsert_player(const uint16_t &id,const uint8_t &x,const uint8_t &y,const CatchChallenger::Direction &direction)
{
    emit Qtreinsert_player(id,x,y,direction);
}
void Api_protocol_2::full_reinsert_player(const uint16_t &id,const uint32_t &mapId,const uint8_t &x,const uint8_t y,const CatchChallenger::Direction &direction)
{
    emit Qtfull_reinsert_player(id,mapId,x,y,direction);
}
void Api_protocol_2::dropAllPlayerOnTheMap()
{
    emit QtdropAllPlayerOnTheMap();
}
void Api_protocol_2::teleportTo(const uint32_t &mapId,const uint8_t &x,const uint8_t &y,const CatchChallenger::Direction &direction)
{
    emit QtteleportTo(mapId,x,y,direction);
}

//plant
void Api_protocol_2::insert_plant(const uint32_t &mapId,const uint8_t &x,const uint8_t &y,const uint8_t &plant_id,const uint16_t &seconds_to_mature)
{
    emit Qtinsert_plant(mapId,x,y,plant_id,seconds_to_mature);
}
void Api_protocol_2::remove_plant(const uint32_t &mapId,const uint8_t &x,const uint8_t &y)
{
    emit Qtremove_plant(mapId,x,y);
}
void Api_protocol_2::seed_planted(const bool &ok)
{
    emit Qtseed_planted(ok);
}
void Api_protocol_2::plant_collected(const CatchChallenger::Plant_collect &stat)
{
    emit Qtplant_collected(stat);
}
//crafting
void Api_protocol_2::recipeUsed(const RecipeUsage &recipeUsage)
{
    emit QtrecipeUsed(recipeUsage);
}
//inventory
void Api_protocol_2::objectUsed(const ObjectUsage &objectUsage)
{
    emit QtobjectUsed(objectUsage);
}
void Api_protocol_2::monsterCatch(const bool &success)
{
    emit QtmonsterCatch(success);
}

//chat
void Api_protocol_2::new_chat_text(const CatchChallenger::Chat_type &chat_type,const std::string &text,const std::string &pseudo,const CatchChallenger::Player_type &player_type)
{
    emit Qtnew_chat_text(chat_type,text,pseudo,player_type);
}
void Api_protocol_2::new_system_text(const CatchChallenger::Chat_type &chat_type,const std::string &text)
{
    emit Qtnew_system_text(chat_type,text);
}

//player info
void Api_protocol_2::have_current_player_info(const CatchChallenger::Player_private_and_public_informations &informations)
{
    emit Qthave_current_player_info(informations);
}
void Api_protocol_2::have_inventory(const std::unordered_map<uint16_t,uint32_t> &items,const std::unordered_map<uint16_t,uint32_t> &warehouse_items)
{
    emit Qthave_inventory(items,warehouse_items);
}
void Api_protocol_2::add_to_inventory(const std::unordered_map<uint16_t,uint32_t> &items)
{
    emit Qtadd_to_inventory(items);
}
void Api_protocol_2::remove_to_inventory(const std::unordered_map<uint16_t,uint32_t> &items)
{
    emit Qtremove_to_inventory(items);
}

//datapack
void Api_protocol_2::haveTheDatapack()
{
    //emit QthaveTheDatapack();

    //if (BaseWindow::client==NULL)
    //    return;

    //#ifdef DEBUG_BASEWINDOWS
        //qDebug() << "BaseWindow::haveTheDatapack()";
    //#endif

    if(BaseWindow::haveDatapack)
        return;

    BaseWindow::haveDatapack = true;

    BaseWindow::settings.setValue("DatapackHashBase-" + QString::fromStdString(BaseWindow::client->datapackPathBase()),
                      QByteArray(
                                 CommonSettingsCommon::commonSettingsCommon.datapackHashBase.data(),
                                  static_cast<int>(CommonSettingsCommon::commonSettingsCommon.datapackHashBase.size())
                                )
                      );

    //if(BaseWindow::client != NULL)
        this->parseDatapack(client->datapackPathBase());

}

void Api_protocol_2::haveTheDatapackMainSub()
{
    //emit QthaveTheDatapackMainSub();

    //#ifdef DEBUG_BASEWINDOWS
        //qDebug() << "BaseWindow::haveTheDatapackMainSub()";
    //#endif

    if(BaseWindow::haveDatapackMainSub)
        return;

    BaseWindow::haveDatapackMainSub=true;
    BaseWindow::settings.setValue("DatapackHashMain-"+QString::fromStdString(BaseWindow::client->datapackPathMain()),
                      QByteArray(
                          CommonSettingsServer::commonSettingsServer.datapackHashServerMain.data(),
                          static_cast<int>(CommonSettingsServer::commonSettingsServer.datapackHashServerMain.size())
                                  )
                      );
    BaseWindow::settings.setValue("DatapackHashSub-"+QString::fromStdString(BaseWindow::client->datapackPathSub()),
                      QByteArray(
                          CommonSettingsServer::commonSettingsServer.datapackHashServerSub.data(),
                          static_cast<int>(CommonSettingsServer::commonSettingsServer.datapackHashServerSub.size())
                                  )
                      );

    //if(BaseWindow::client != NULL)
        this->parseDatapackMainSub(BaseWindow::client->mainDatapackCode(), BaseWindow::client->subDatapackCode());

}

void Api_protocol_2::parseDatapack(const std::string &datapackPath)
{
    if(inProgress)//mutable?
    {
        //qDebug() << QStringLiteral("already in progress");
        return;
    }

    inProgress=true;

    if(!CommonSettingsCommon::commonSettingsCommon.httpDatapackMirrorBase.empty())//static
    {
        const std::vector<char> &hash = CatchChallenger::DatapackChecksum::doChecksumBase(datapackPath);
        if (hash.empty())
        {
            std::cerr << "DatapackClientLoader::parseDatapack(): hash is empty" << std::endl;
            this->datapackChecksumError();
            inProgress=false;
            return;
        }

        if(CommonSettingsCommon::commonSettingsCommon.datapackHashBase!=hash)
        {
            /*qDebug() << QStringLiteral("DatapackClientLoader::parseDatapack() CommonSettingsCommon::commonSettingsCommon.datapackHashBase!=hash.result(): %1!=%2")
                        .arg(QString::fromStdString(binarytoHexa(CommonSettingsCommon::commonSettingsCommon.datapackHashBase)))
                        .arg(QString::fromStdString(binarytoHexa(hash)));*/
            this->datapackChecksumError();
            inProgress=false;
            return;
        }
    }

    this->datapackPath = datapackPath;
    DatapackClientLoader::text_DATAPACK_BASE_PATH_MAPMAIN = DATAPACK_BASE_PATH_MAPMAIN + "na/";

    DatapackClientLoader::text_DATAPACK_BASE_PATH_MAPSUB=std::string(DATAPACK_BASE_PATH_MAPSUB1)+"na/"+std::string(DATAPACK_BASE_PATH_MAPSUB2)+"nabis/";

    if(mDefaultInventoryImage==NULL)
        mDefaultInventoryImage=new QPixmap(QStringLiteral(":/images/inventory/unknown-object.png"));

    #ifndef BOTTESTCONNECT
        CatchChallenger::CommonDatapack::commonDatapack.parseDatapack(datapackPath);
        language=LanguagesSelect::languagesSelect->getCurrentLanguages();
        parseVisualCategory();
        parseTypesExtra();
        parseItemsExtra();
        parseSkins();
        parseMonstersExtra();
        parseBuffExtra();
        parseSkillsExtra();
        parsePlantsExtra();
        parseAudioAmbiance();
        parseReputationExtra();
    #endif
    inProgress=false;
    this->datapackParsed();
}

void DatapackClientLoader::parseDatapackMainSub(const std::string &mainDatapackCode, const std::string &subDatapackCode)
{
    DatapackClientLoader::text_DATAPACK_BASE_PATH_MAPMAIN=DATAPACK_BASE_PATH_MAPMAIN+mainDatapackCode+"/";
    DatapackClientLoader::text_DATAPACK_BASE_PATH_MAPSUB=DATAPACK_BASE_PATH_MAPSUB1+mainDatapackCode+DATAPACK_BASE_PATH_MAPSUB2+subDatapackCode+"/";

    if(inProgress)
    {
        //qDebug() << QStringLiteral("already in progress");
        return;
    }
    inProgress=true;
    this->mainDatapackCode=mainDatapackCode;
    this->subDatapackCode=subDatapackCode;

    if(!CommonSettingsServer::commonSettingsServer.httpDatapackMirrorServer.empty())
    {
        {
            const std::vector<char> &hash=CatchChallenger::DatapackChecksum::doChecksumMain((datapackPath+DatapackClientLoader::text_DATAPACK_BASE_PATH_MAPMAIN));
            if(hash.empty())
            {
                std::cerr << "DatapackClientLoader::parseDatapackMainSub(): hash is empty" << std::endl;
                this->datapackChecksumError();
                inProgress=false;
                return;
            }

            if(CommonSettingsServer::commonSettingsServer.datapackHashServerMain!=hash)
            {
                //qDebug() << QStringLiteral("DatapackClientLoader::parseDatapack() Main CommonSettingsServer::commonSettingsServer.datapackHashServerMain!=hash.result(): %1!=%2")
                //            .arg(QString::fromStdString(binarytoHexa(CommonSettingsServer::commonSettingsServer.datapackHashServerMain)))
                //            .arg(QString::fromStdString(binarytoHexa(hash)));
                this->datapackChecksumError();
                inProgress=false;
                return;
            }
        }
        if(!CommonSettingsServer::commonSettingsServer.subDatapackCode.empty())
        {
            const std::vector<char> &hash=CatchChallenger::DatapackChecksum::doChecksumSub(
                        (datapackPath+DatapackClientLoader::text_DATAPACK_BASE_PATH_MAPSUB));
            if(hash.empty())
            {
                std::cerr << "DatapackClientLoader::parseDatapackSub(): hash is empty" << std::endl;
                this->datapackChecksumError();
                inProgress=false;
                return;
            }

            if(CommonSettingsServer::commonSettingsServer.datapackHashServerSub!=hash)
            {
                //qDebug() << QStringLiteral("DatapackClientLoader::parseDatapack() Sub CommonSettingsServer::commonSettingsServer.datapackHashServerSub!=hash.result(): %1!=%2")
                //            .arg(QString::fromStdString(binarytoHexa(CommonSettingsServer::commonSettingsServer.datapackHashServerSub)))
                //            .arg(QString::fromStdString(binarytoHexa(hash)));
                this->datapackChecksumError();
                inProgress=false;
                return;
            }
        }
    }
    if(mDefaultInventoryImage==NULL)
        mDefaultInventoryImage=new QPixmap(QStringLiteral(":/images/inventory/unknown-object.png"));

    CatchChallenger::CommonDatapackServerSpec::commonDatapackServerSpec.parseDatapack(
                datapackPath,mainDatapackCode,subDatapackCode);

    parseMaps();
    parseQuestsLink();
    parseQuestsExtra();
    parseQuestsText();
    parseBotFightsExtra();
    parseZoneExtra();
    parseTileset();

    inProgress=false;

    this->datapackParsedMainSub();
}

void /*BaseWindow::*/Api_protocol_2::datapackChecksumError()
{
    #ifdef DEBUG_BASEWINDOWS
        //qDebug() << "BaseWindow::datapackChecksumError()";
    #endif

    datapackIsParsed=false;

    //reset all the cached hash
    settings.remove("DatapackHashBase-"+QString::fromStdString(client->datapackPathBase()));
    settings.remove("DatapackHashMain-"+QString::fromStdString(client->datapackPathMain()));
    settings.remove("DatapackHashSub-"+QString::fromStdString(client->datapackPathSub()));

    this->newError(tr("Datapack on mirror is corrupted").toStdString(),
                  "The checksum sended by the server is not the same than have on the mirror");
}

void /*BaseWindow*/Api_protocol_2::datapackParsed()
{
    #ifdef DEBUG_BASEWINDOWS
        //qDebug() << "BaseWindow::datapackParsed()";
    #endif
    datapackIsParsed=true;
    updateConnectingStatus();
    loadSettingsWithDatapack();
    {
        if(QFile(QString::fromStdString(client->datapackPathBase())+QStringLiteral("/images/interface/fight/labelBottom.png")).exists())
            ui->frameFightBottom->setStyleSheet(QStringLiteral("#frameFightBottom{background-image: url('")+QString::fromStdString(client->datapackPathBase())+QStringLiteral("/images/interface/fight/labelBottom.png');padding:6px 6px 6px 14px;}"));
        else
            ui->frameFightBottom->setStyleSheet(QStringLiteral("#frameFightBottom{background-image: url(:/images/interface/fight/labelBottom.png);padding:6px 6px 6px 14px;}"));
        if(QFile(QString::fromStdString(client->datapackPathBase())+QStringLiteral("/images/interface/fight/labelTop.png")).exists())
            ui->frameFightTop->setStyleSheet(QStringLiteral("#frameFightTop{background-image: url('")+QString::fromStdString(client->datapackPathBase())+QStringLiteral("/images/interface/fight/labelTop.png');padding:6px 14px 6px 6px;}"));
        else
            ui->frameFightTop->setStyleSheet(QStringLiteral("#frameFightTop{background-image: url(:/images/interface/fight/labelTop.png);padding:6px 14px 6px 6px;}"));
    }
    //updatePlayerImage();
}

void /*BaseWindow*/Api_protocol_2::datapackParsedMainSub()
{
    if(client==NULL)
        return;
    if(mapController==NULL)
        return;
    //#ifdef DEBUG_BASEWINDOWS
        //qDebug() << "BaseWindow::datapackParsedMainSub()";
    //#endif

    mainSubDatapackIsParsed=true;

    //always after monster load on CatchChallenger::ClientFightEngine::fightEngine
    mapController->setDatapackPath(client->datapackPathBase(),client->mainDatapackCode());

    if(!client->setMapNumber(DatapackClientLoader::datapackLoader.mapToId.size()))
        this->newError(tr("Internal error").toStdString(),"No map loaded to call client->setMapNumber()");

    this->have_main_and_sub_datapack_loaded();

    this->datapackParsedMainSubMap();

    updateConnectingStatus();
}

void /*BaseWindow*/Api_protocol_2::updateConnectingStatus()
{
    if(BaseWindow::isLogged && BaseWindow::datapackIsParsed)
    {
        const std::vector<ServerFromPoolForDisplay> &serverOrdenedList=BaseWindow::client->getServerOrdenedList();
        if(BaseWindow::serverSelected==-1)
        {
            if(ui->stackedWidget->currentWidget()!=ui->page_serverList)
            {
                if(multiplayer)
                {
                    ui->stackedWidget->setCurrentWidget(ui->page_serverList);
                    ui->serverList->header()->setSectionResizeMode(QHeaderView::Fixed);
                    ui->serverList->header()->resizeSection(0,680);
                    updateServerList();
                }
                else
                {
                    serverSelected=0;
                    if(serverSelected<0 || serverSelected>=(int)serverOrdenedList.size())
                    {
                        error("BaseWindow::updateConnectingStatus(): serverSelected=selectedItem->data(99,99).toUInt() corrupted value");
                        return;
                    }
                    updateConnectingStatus();
                }
                return;
            }
        }
        else if(!haveCharacterPosition && !haveCharacterInformation && !client->character_select_is_send() &&
                (unsigned int)serverSelected<serverOrdenedList.size())
        {
            if(ui->stackedWidget->currentWidget()!=ui->page_character)
            {
                if(multiplayer)
                    ui->stackedWidget->setCurrentWidget(ui->page_character);
                const uint8_t &charactersGroupIndex=serverOrdenedList.at(serverSelected).charactersGroupIndex;
                const std::vector<CharacterEntry> &characterEntryList=characterListForSelection.at(charactersGroupIndex);
                ui->character_add->setEnabled(characterEntryList.size()<CommonSettingsCommon::commonSettingsCommon.max_character);
                ui->character_remove->setEnabled(characterEntryList.size()>CommonSettingsCommon::commonSettingsCommon.min_character);
                if(characterEntryList.empty())
                {
                    if(CommonSettingsCommon::commonSettingsCommon.max_character==0)
                        emit message("Can't create character but the list is empty");
                }
                updateCharacterList();
                if((characterListForSelection.empty() ||
                    characterListForSelection.at(charactersGroupIndex).empty()) &&
                        CommonSettingsCommon::commonSettingsCommon.max_character>0)
                {
                    if(CommonSettingsCommon::commonSettingsCommon.min_character>0)
                    {
                        ui->frameLoading->setStyleSheet("#frameLoading {border-image: url(:/images/empty.png);border-width: 0px;}");
                        ui->stackedWidget->setCurrentWidget(ui->page_init);
                        ui->label_connecting_status->setText(QString());
                    }
                    on_character_add_clicked();
                    return;
                }
                if(characterListForSelection.size()==1 && CommonSettingsCommon::commonSettingsCommon.min_character>=characterListForSelection.size() &&
                        CommonSettingsCommon::commonSettingsCommon.max_character<=characterListForSelection.size())
                {
                    if(characterListForSelection.at(charactersGroupIndex).size()==1)
                    {
                        characterSelected=true;
                        ui->characterEntryList->item(ui->characterEntryList->count()-1)->setSelected(true);
                        on_character_select_clicked();
                        return;
                    }
                    else
                        emit message("BaseWindow::updateConnectingStatus(): characterListForSelection.at(charactersGroupIndex).size()!=1, bug");
                }
                return;
            }
        }
    }

    QStringList waitedData;
    if((haveCharacterPosition && haveCharacterInformation) && !mainSubDatapackIsParsed)
        waitedData << tr("Loading of the specific datapack part");
    if(haveDatapack && (!haveInventory || !haveCharacterPosition || !haveCharacterInformation))
    {
        if(!haveCharacterPosition || !haveCharacterInformation)
            waitedData << tr("Loading of the player informations");
        else
            waitedData << tr("Loading of the inventory");
    }
    if(!haveDatapack)
    {
        if(!protocolIsGood)
            waitedData << tr("Try send the protocol...");
        else if(!isLogged)
        {
            if(datapackGatewayProgression.empty())
                waitedData << tr("Try login...");
            else if(datapackGatewayProgression.size()<2)
                waitedData << tr("Updating the gateway cache...");
            else
                waitedData << tr("Updating the %1 gateways cache...").arg(datapackGatewayProgression.size());
        }
        else
        {
            if(datapackFileSize==0)
                waitedData << tr("Loading of the datapack");
            else if(datapackFileSize<0)
                waitedData << tr("Loaded datapack size: %1KB").arg((datapackDownloadedSize+progressingDatapackFileSize)/1000);//when the http server don't send the size
            else if((datapackDownloadedSize+progressingDatapackFileSize)>=(uint32_t)datapackFileSize)
                waitedData << tr("Loaded datapack file: 100%");
            else
                waitedData << tr("Loaded datapack file: %1%").arg(((datapackDownloadedSize+progressingDatapackFileSize)*100)/datapackFileSize);
        }
    }
    else if(!datapackIsParsed)
        waitedData << tr("Opening the datapack");
    if(waitedData.isEmpty())
    {
        Player_private_and_public_informations &playerInformations=client->get_player_informations();
        if(playerInformations.bot_already_beaten==NULL)
        {
            std::cerr << "void BaseWindow::updateConnectingStatus(): waitedData.isEmpty(), playerInformations.bot_already_beaten==NULL" << std::endl;
            abort();
        }
        mapController->setBotsAlreadyBeaten(playerInformations.bot_already_beaten);
        mapController->setInformations(&playerInformations.items,&playerInformations.quests,&events,&playerInformations.itemOnMap,&playerInformations.plantOnMap);
        client->unloadSelection();
        load_inventory();
        load_plant_inventory();
        load_crafting_inventory();
        updateDisplayedQuests();
        if(!check_senddata())
            return;
        load_monsters();
        show_reputation();
        load_event();
        emit gameIsLoaded();
        this->setWindowTitle(QStringLiteral("CatchChallenger - %1").arg(QString::fromStdString(client->getPseudo())));
        ui->stackedWidget->setCurrentWidget(ui->page_map);
        showTip(tr("Welcome <b><i>%1</i></b> on <i>CatchChallenger</i>").arg(QString::fromStdString(client->getPseudo())).toStdString());
        return;
    }
    ui->label_connecting_status->setText(tr("Waiting: %1").arg(waitedData.join(", ")));
}


void /*BaseWindow*/Api_protocol_2::have_main_and_sub_datapack_loaded()
{
    BaseWindow::client->have_main_and_sub_datapack_loaded();
    if(!BaseWindow::client->getCaracterSelected())
    {
        error("BaseWindow::have_main_and_sub_datapack_loaded(): don't have player info, need to code this delay part");
        return;
    }
    const Player_private_and_public_informations &informations=BaseWindow::client->get_player_informations();

    //always after monster load on CatchChallenger::ClientFightEngine::fightEngine
    mapController->have_current_player_info(informations);

//    qDebug() << (QStringLiteral("%1 is logged with id: %2, cash: %3")
//                 .arg(QString::fromStdString(informations.public_informations.pseudo))
//                 .arg(informations.public_informations.simplifiedId)
//                 .arg(informations.cash)
//                 );
    updateConnectingStatus();
    updateClanDisplay();
    updatePlayerType();
}

void /*MapController*/Api_protocol_2::datapackParsedMainSub()
{
    if(mHaveTheDatapack)
        return;
    MapControllerMP::datapackParsedMainSub();
    unsigned int index=0;
    while(index<delayedPlantInsert.size())
    {
        insert_plant(delayedPlantInsert.at(index).mapId,delayedPlantInsert.at(index).x,delayedPlantInsert.at(index).y,delayedPlantInsert.at(index).plant_id,delayedPlantInsert.at(index).seconds_to_mature);
        index++;
    }
    delayedPlantInsert.clear();
}



//base
void Api_protocol_2::newFileBase(const std::string &fileName,const std::string &data)
{
    emit QtnewFileBase(fileName,data);
}
void Api_protocol_2::newHttpFileBase(const std::string &url,const std::string &fileName)
{
    emit QtnewHttpFileBase(url,fileName);
}
void Api_protocol_2::removeFileBase(const std::string &fileName)
{
    emit QtremoveFileBase(fileName);
}
void Api_protocol_2::datapackSizeBase(const uint32_t &datapckFileNumber,const uint32_t &datapckFileSize)
{
    emit QtdatapackSizeBase(datapckFileNumber,datapckFileSize);
}
//main
void Api_protocol_2::newFileMain(const std::string &fileName,const std::string &data)
{
    emit QtnewFileMain(fileName,data);
}
void Api_protocol_2::newHttpFileMain(const std::string &url,const std::string &fileName)
{
    emit QtnewHttpFileMain(url,fileName);
}
void Api_protocol_2::removeFileMain(const std::string &fileName)
{
    emit QtremoveFileMain(fileName);
}
void Api_protocol_2::datapackSizeMain(const uint32_t &datapckFileNumber,const uint32_t &datapckFileSize)
{
    emit QtdatapackSizeMain(datapckFileNumber,datapckFileSize);
}
//sub
void Api_protocol_2::newFileSub(const std::string &fileName,const std::string &data)
{
    emit QtnewFileSub(fileName,data);
}
void Api_protocol_2::newHttpFileSub(const std::string &url,const std::string &fileName)
{
    emit QtnewHttpFileSub(url,fileName);
}
void Api_protocol_2::removeFileSub(const std::string &fileName)
{
    emit QtremoveFileSub(fileName);
}
void Api_protocol_2::datapackSizeSub(const uint32_t &datapckFileNumber,const uint32_t &datapckFileSize)
{
    emit QtdatapackSizeSub(datapckFileNumber,datapckFileSize);
}

//shop
void Api_protocol_2::haveShopList(const std::vector<ItemToSellOrBuy> &items)
{
    emit QthaveShopList(items);
}
void Api_protocol_2::haveBuyObject(const BuyStat &stat,const uint32_t &newPrice)
{
    emit QthaveBuyObject(stat,newPrice);
}
void Api_protocol_2::haveSellObject(const SoldStat &stat,const uint32_t &newPrice)
{
    emit QthaveSellObject(stat,newPrice);
}

//factory
void Api_protocol_2::haveFactoryList(const uint32_t &remainingProductionTime,const std::vector<ItemToSellOrBuy> &resources,const std::vector<ItemToSellOrBuy> &products)
{
    emit QthaveFactoryList(remainingProductionTime,resources,products);
}
void Api_protocol_2::haveBuyFactoryObject(const BuyStat &stat,const uint32_t &newPrice)
{
    emit QthaveBuyFactoryObject(stat,newPrice);
}
void Api_protocol_2::haveSellFactoryObject(const SoldStat &stat,const uint32_t &newPrice)
{
    emit QthaveSellFactoryObject(stat,newPrice);
}

//trade
void Api_protocol_2::tradeRequested(const std::string &pseudo,const uint8_t &skinInt)
{
    emit QttradeRequested(pseudo,skinInt);
}
void Api_protocol_2::tradeAcceptedByOther(const std::string &pseudo,const uint8_t &skinInt)
{
    emit QttradeAcceptedByOther(pseudo,skinInt);
}
void Api_protocol_2::tradeCanceledByOther()
{
    emit QttradeCanceledByOther();
}
void Api_protocol_2::tradeFinishedByOther()
{
    emit QttradeFinishedByOther();
}
void Api_protocol_2::tradeValidatedByTheServer()
{
    emit QttradeValidatedByTheServer();
}
void Api_protocol_2::tradeAddTradeCash(const uint64_t &cash)
{
    emit QttradeAddTradeCash(cash);
}
void Api_protocol_2::tradeAddTradeObject(const uint32_t &item,const uint32_t &quantity)
{
    emit QttradeAddTradeObject(item,quantity);
}
void Api_protocol_2::tradeAddTradeMonster(const CatchChallenger::PlayerMonster &monster)
{
    emit QttradeAddTradeMonster(monster);
}

//battle
void Api_protocol_2::battleRequested(const std::string &pseudo,const uint8_t &skinInt)
{
    emit QtbattleRequested(pseudo,skinInt);
}
void Api_protocol_2::battleAcceptedByOther(const std::string &pseudo,const uint8_t &skinId,const std::vector<uint8_t> &stat,const uint8_t &monsterPlace,const PublicPlayerMonster &publicPlayerMonster)
{
    emit QtbattleAcceptedByOther(pseudo,skinId,stat,monsterPlace,publicPlayerMonster);
}
void Api_protocol_2::battleCanceledByOther()
{
    emit QtbattleCanceledByOther();
}
void Api_protocol_2::sendBattleReturn(const std::vector<Skill::AttackReturn> &attackReturn)
{
    emit QtsendBattleReturn(attackReturn);
}

//clan
void Api_protocol_2::clanActionSuccess(const uint32_t &clanId)
{
    emit QtclanActionSuccess(clanId);
}
void Api_protocol_2::clanActionFailed()
{
    emit QtclanActionFailed();
}
void Api_protocol_2::clanDissolved()
{
    emit QtclanDissolved();
}
void Api_protocol_2::clanInformations(const std::string &name)
{
    emit QtclanInformations(name);
}
void Api_protocol_2::clanInvite(const uint32_t &clanId,const std::string &name)
{
    emit QtclanInvite(clanId,name);
}
void Api_protocol_2::cityCapture(const uint32_t &remainingTime,const uint8_t &type)
{
    emit QtcityCapture(remainingTime,type);
}

//city
void Api_protocol_2::captureCityYourAreNotLeader()
{
    emit QtcaptureCityYourAreNotLeader();
}
void Api_protocol_2::captureCityYourLeaderHaveStartInOtherCity(const std::string &zone)
{
    emit QtcaptureCityYourLeaderHaveStartInOtherCity(zone);
}
void Api_protocol_2::captureCityPreviousNotFinished()
{
    emit QtcaptureCityPreviousNotFinished();
}
void Api_protocol_2::captureCityStartBattle(const uint16_t &player_count,const uint16_t &clan_count)
{
    emit QtcaptureCityStartBattle(player_count,clan_count);
}
void Api_protocol_2::captureCityStartBotFight(const uint16_t &player_count,const uint16_t &clan_count,const uint32_t &fightId)
{
    emit QtcaptureCityStartBotFight(player_count,clan_count,fightId);
}
void Api_protocol_2::captureCityDelayedStart(const uint16_t &player_count,const uint16_t &clan_count)
{
    emit QtcaptureCityDelayedStart(player_count,clan_count);
}
void Api_protocol_2::captureCityWin()
{
    emit QtcaptureCityWin();
}

//market
void Api_protocol_2::marketList(const uint64_t &price,const std::vector<MarketObject> &marketObjectList,const std::vector<MarketMonster> &marketMonsterList,const std::vector<MarketObject> &marketOwnObjectList,const std::vector<MarketMonster> &marketOwnMonsterList)
{
    emit QtmarketList(price,marketObjectList,marketMonsterList,marketOwnObjectList,marketOwnMonsterList);
}
void Api_protocol_2::marketBuy(const bool &success)
{
    emit QtmarketBuy(success);
}
void Api_protocol_2::marketBuyMonster(const PlayerMonster &playerMonster)
{
    emit QtmarketBuyMonster(playerMonster);
}
void Api_protocol_2::marketPut(const bool &success)
{
    emit QtmarketPut(success);
}
void Api_protocol_2::marketGetCash(const uint64_t &cash)
{
    emit QtmarketGetCash(cash);
}
void Api_protocol_2::marketWithdrawCanceled()
{
    emit QtmarketWithdrawCanceled();
}
void Api_protocol_2::marketWithdrawObject(const uint32_t &objectId,const uint32_t &quantity)
{
    emit QtmarketWithdrawObject(objectId,quantity);
}
void Api_protocol_2::marketWithdrawMonster(const PlayerMonster &playerMonster)
{
    emit QtmarketWithdrawMonster(playerMonster);
}

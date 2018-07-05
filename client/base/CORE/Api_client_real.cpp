#include "Api_client_real.h"

using namespace CatchChallenger;

//need host + port here to have datapack base

std::string Api_client_real::text_slash                = std::string("/");
std::string Api_client_real::text_dotcoma              = std::string(";");
std::string Api_client_real::regex_DATAPACK_FILE_REGEX = std::string(DATAPACK_FILE_REGEX);

std::regex Api_client_real::excludePathBase("^map[/\\\\]main[/\\\\]");
std::regex Api_client_real::excludePathMain("^sub[/\\\\]");

Api_client_real::Api_client_real(ConnectedSocket* socket, const Settings& settings, bool tolerantMode) :
    Api_protocol_2(socket, settings, tolerantMode),
    qnamQueueCount(0),
    qnamQueueCount2(0),
    qnamQueueCount3(0),
    qnamQueueCount4(0)
{
    datapackTarBase   = false;
    datapackTarMain   = false;
    datapackTarSub    = false;
    index_mirror_base = 0;
    index_mirror_main = 0;
    index_mirror_sub  = 0;
    host              = "localhost";
    port              = 42489;
//    if(!connect(socket, &ConnectedSocket::disconnected,	this,&Api_client_real::disconnect))
//        abort();
//    if(!connect(this,   &Api_client_real::QtnewFileBase,      this,&Api_client_real::writeNewFileBase))
//        abort();
//    if(!connect(this,   &Api_client_real::QtnewFileMain,      this,&Api_client_real::writeNewFileMain))
//        abort();
//    if(!connect(this,   &Api_client_real::QtnewFileSub,      this,&Api_client_real::writeNewFileSub))
//        abort();
//    if(!connect(this,   &Api_client_real::QtnewHttpFileBase,  this,&Api_client_real::getHttpFileBase))
//        abort();
//    if(!connect(this,   &Api_client_real::QtnewHttpFileMain,  this,&Api_client_real::getHttpFileMain))
//        abort();
//    if(!connect(this,   &Api_client_real::QtnewHttpFileSub,  this,&Api_client_real::getHttpFileSub))
//        abort();
//    if(!connect(this,   &Api_client_real::doDifferedChecksumBase,&datapackChecksum,&CatchChallenger::DatapackChecksum::doDifferedChecksumBase))
//        abort();
//    if(!connect(this,   &Api_client_real::doDifferedChecksumMain,&datapackChecksum,&CatchChallenger::DatapackChecksum::doDifferedChecksumMain))
//        abort();
//    if(!connect(this,   &Api_client_real::doDifferedChecksumSub,&datapackChecksum,&CatchChallenger::DatapackChecksum::doDifferedChecksumSub))
//        abort();
//    if(!connect(&datapackChecksum,&CatchChallenger::DatapackChecksum::datapackChecksumDoneBase,this,&Api_client_real::datapackChecksumDoneBase))
//        abort();
//    if(!connect(&datapackChecksum,&CatchChallenger::DatapackChecksum::datapackChecksumDoneMain,this,&Api_client_real::datapackChecksumDoneMain))
//        abort();
//    if(!connect(&datapackChecksum,&CatchChallenger::DatapackChecksum::datapackChecksumDoneSub,this,&Api_client_real::datapackChecksumDoneSub))
//        abort();
//    if(!connect(&zstdDecodeThreadBase,&QZstdDecodeThread::decodedIsFinish,this,&Api_client_real::decodedIsFinishBase))
//        abort();
//    if(!connect(&zstdDecodeThreadMain,&QZstdDecodeThread::decodedIsFinish,this,&Api_client_real::decodedIsFinishMain))
//        abort();
//    if(!connect(&zstdDecodeThreadSub,&QZstdDecodeThread::decodedIsFinish,this,&Api_client_real::decodedIsFinishSub))
//        abort();
    resetAll();
    //dataClear();do into disconnected()
}

Api_client_real::~Api_client_real()
{
    if (socket != NULL)
    {
        socket->abort();
        socket->disconnectFromHost();
        if (socket->state() != SocketState::UnconnectedState) {
            socket->waitForDisconnected();
        }
    }
    zstdDecodeThreadBase.exit();
    zstdDecodeThreadBase.quit();
    zstdDecodeThreadMain.exit();
    zstdDecodeThreadMain.quit();
    zstdDecodeThreadSub.exit();
    zstdDecodeThreadSub.quit();

    zstdDecodeThreadBase.wait();
    zstdDecodeThreadMain.wait();
    zstdDecodeThreadSub.wait();
}

bool Api_client_real::parseReplyData(const uint8_t& mainCodeType,const uint8_t &queryNumber, const char* const data, const unsigned int& size)
{
    return Api_client_real::parseReplyData(mainCodeType, queryNumber, std::string(data, size));
}

bool Api_client_real::parseReplyData(const uint8_t &mainCodeType, const uint8_t &queryNumber, const std::string &data)
{
    if (querySendTime.find(queryNumber) != querySendTime.cend())
    {
        std::time_t result = std::time(nullptr);
        if ((uint64_t)result >= (uint64_t)querySendTime.at(queryNumber)) {
            lastReplyTime(result - querySendTime.at(queryNumber));
        }
        querySendTime.erase(queryNumber);
    }

    DataStreamSerializer in(data.size());
    //in.setVersion(QDataStream::Qt_4_4);
    //in.setByteOrder(QDataStream::LittleEndian);
    in << data;

    switch(mainCodeType)
    {
        case 0xA1:
        {
            std::cout << "Select character: " << datapackStatus << std::endl;
            switch(datapackStatus)
            {
                case DatapackStatus::Base:
                {
                    if (datapackFilesListBase.empty() && data.size() == 1)
                    {
                        datapackStatus = DatapackStatus::Main;
                        if (!httpModeBase) {
                            haveTheDatapack();
                        }
                        return true;
                    }
                    std::list<bool> boolList;
                    while((in.size() - in.pos()) > 0)
                    {
                        uint8_t returnCode;
                        in >> returnCode;
                        boolList.push_back(returnCode&0x01);
                        boolList.push_back(returnCode&0x02);
                        boolList.push_back(returnCode&0x04);
                        boolList.push_back(returnCode&0x08);
                        boolList.push_back(returnCode&0x10);
                        boolList.push_back(returnCode&0x20);
                        boolList.push_back(returnCode&0x40);
                        boolList.push_back(returnCode&0x80);
                    }
                    if((uint32_t)boolList.size()<datapackFilesListBase.size())
                    {
                        newError(tr("Procotol wrong or corrupted").toStdString(),
                                 QStringLiteral("bool list too small with main ident: %1, and queryNumber: %2, type: query_type_protocol")
                                 .arg(mainCodeType).arg(queryNumber).toStdString());
                        return false;
                    }
                    unsigned int index = 0;
                    while (index<datapackFilesListBase.size())
                    {
                        if (boolList.front())
                        {
                            qDebug() << (QStringLiteral("remove the file: %1")
                                         .arg(QString::fromStdString(mDatapackBase)+text_slash+
                                              QString::fromStdString(datapackFilesListBase.at(index))));

                            File file(mDatapackBase + text_slash + datapackFilesListBase.at(index));
                            if (!file.remove()) {
                                qDebug() << (QStringLiteral("unable to remove the file: %1: %2").arg(QString::fromStdString(datapackFilesListBase.at(index))).arg(file.errorString()));
                            }
                            //removeFile(datapackFilesListBase.at(index));
                        }
                        boolList.pop_front();
                        index++;
                    }
                    datapackFilesListBase.clear();
                    cleanDatapackBase();
                    if (boolList.size() >= 8)
                    {
                        newError(std::string("Procotol wrong or corrupted"),
                                 std::string("bool list too big with main ident: %1, and queryNumber: %2, type: query_type_protocol")
                                 .arg(queryNumber).toStdString());
                        return false;
                    }
                    if (!httpModeBase) {
                        haveTheDatapack();
                    }
                    datapackStatus = DatapackStatus::Main;
                }
                break;
                case DatapackStatus::Main:
                {
                    if (datapackFilesListMain.empty() && data.size() == 1)
                    {
                        datapackStatus = DatapackStatus::Sub;
                        if (!httpModeMain) {
                            checkIfContinueOrFinished();
                        }
                        return true;
                    }
                    std::list<bool> boolList;
                    while((in.size() - in.pos()) > 0)
                    {
                        uint8_t returnCode;
                        in >> returnCode;
                        boolList.push_back(returnCode&0x01);
                        boolList.push_back(returnCode&0x02);
                        boolList.push_back(returnCode&0x04);
                        boolList.push_back(returnCode&0x08);
                        boolList.push_back(returnCode&0x10);
                        boolList.push_back(returnCode&0x20);
                        boolList.push_back(returnCode&0x40);
                        boolList.push_back(returnCode&0x80);
                    }
                    if ((uint32_t)boolList.size() < datapackFilesListMain.size())
                    {
                        newError(tr("Procotol wrong or corrupted").toStdString(),
                                 QStringLiteral("bool list too small with main ident: %1, and queryNumber: %2, type: query_type_protocol")
                                 .arg(mainCodeType).arg(queryNumber).toStdString());
                        return false;
                    }
                    unsigned int index = 0;
                    while (index < datapackFilesListMain.size())
                    {
                        if (boolList.front())
                        {
                            qDebug() << (QStringLiteral("remove the file: %1")
                                         .arg(QString::fromStdString(mDatapackMain)+text_slash+QString::fromStdString(datapackFilesListMain.at(index))));

                            File file(mDatapackMain + text_slash + datapackFilesListMain.at(index));
                            if (!file.remove()) {
                                qDebug() << (QStringLiteral("unable to remove the file: %1: %2").arg(QString::fromStdString(datapackFilesListMain.at(index))).arg(file.errorString()));
                            }
                        }
                        boolList.pop_front();
                        index++;
                    }
                    datapackFilesListMain.clear();
                    cleanDatapackMain();
                    if (boolList.size() >= 8)
                    {
                        newError(tr("Procotol wrong or corrupted").toStdString(),
                                 QStringLiteral("bool list too big with main ident: %1, and queryNumber: %2, type: query_type_protocol")
                                 .arg(mainCodeType).arg(queryNumber).toStdString());
                        return false;
                    }
                    if (!httpModeMain) {
                        //TODO
                        //checkIfContinueOrFinished();
                    }
                    datapackStatus = DatapackStatus::Sub;
                }
                break;
                case DatapackStatus::Sub:
                {
                    if (datapackFilesListSub.empty() && data.size() == 1)
                    {
                        datapackStatus = DatapackStatus::Finished;
                        if (!httpModeSub) {
                            datapackDownloadFinishedSub();
                        }
                        return true;
                    }
                    std::list<bool> boolList;
                    while ((in.size() - in.pos()) > 0)
                    {
                        uint8_t returnCode;
                        in >> returnCode;
                        boolList.push_back(returnCode & 0x01);
                        boolList.push_back(returnCode & 0x02);
                        boolList.push_back(returnCode & 0x04);
                        boolList.push_back(returnCode & 0x08);
                        boolList.push_back(returnCode & 0x10);
                        boolList.push_back(returnCode & 0x20);
                        boolList.push_back(returnCode & 0x40);
                        boolList.push_back(returnCode & 0x80);
                    }
                    if ((uint32_t)boolList.size() < datapackFilesListSub.size())
                    {
                        newError(tr("Procotol wrong or corrupted").toStdString(),
                                 QStringLiteral("bool list too small with sub ident: %1, and queryNumber: %2, type: query_type_protocol")
                                 .arg(mainCodeType).arg(queryNumber).toStdString());
                        return false;
                    }
                    unsigned int index = 0;
                    while (index < datapackFilesListSub.size())
                    {
                        if (boolList.front())
                        {
                            qDebug() << (QStringLiteral("remove the file: %1")
                                         .arg(QString::fromStdString(mDatapackSub)+text_slash+QString::fromStdString(datapackFilesListSub.at(index))));
                            File file(mDatapackSub + text_slash + datapackFilesListSub.at(index));
                            if (!file.remove()) {
                                qDebug() << (QStringLiteral("unable to remove the file: %1: %2").arg(QString::fromStdString(datapackFilesListSub.at(index))).arg(file.errorString()));
                            }
                        }
                        boolList.pop_front();
                        index++;
                    }
                    datapackFilesListSub.clear();
                    cleanDatapackSub();
                    if (boolList.size() >= 8)
                    {
                        newError(std::string("Procotol wrong or corrupted"),
                                 QStringLiteral("bool list too big with sub ident: %1, and queryNumber: %2, type: query_type_protocol")
                                 .arg(mainCodeType).arg(queryNumber).toStdString());
                        return false;
                    }
                    if (!httpModeSub) {
                        datapackDownloadFinishedSub();
                    }
                    datapackStatus = DatapackStatus::Finished;
                }
                break;
                default:
                return false;
            }
        }
        return true;
        default:
            return Api_protocol_2::parseReplyData(mainCodeType, queryNumber, data);
        break;
    }
    if ((in.size() - in.pos()) != 0)
    {
        ByteArray data_remaining = ByteArray(data.data(), data.size() - in.pos());

        parseError(std::string("Procotol wrong or corrupted"),
                   QStringLiteral("error: remaining data: Api_client_real::parseReplyData(%1,%2,%3): %4")
                   .arg(mainCodeType).arg(queryNumber).arg(QString(data_remaining.toHex()))
                   .toStdString());

        return false;
    }

    return true;
}

//general data
void Api_client_real::defineMaxPlayers(const uint16_t &maxPlayers)
{
    ProtocolParsing::setMaxPlayers(maxPlayers);
}

void Api_client_real::resetAll()
{
    RXSize = 0;
    TXSize = 0;

    httpModeBase = false;
    httpModeMain = false;
    httpModeSub  = false;
    httpError    = false;

    wait_datapack_content_base = false;
    wait_datapack_content_main = false;
    wait_datapack_content_sub  = false;

    query_files_list_base.clear();
    query_files_list_main.clear();
    query_files_list_sub.clear();
    urlInWaitingListBase.clear();
    urlInWaitingListMain.clear();
    urlInWaitingListSub.clear();

    Api_protocol_2::resetAll();
}

void Api_client_real::tryConnect(std::string host, uint16_t port)
{
    if (socket == NULL) {
        return;
    }

    std::stringstream ss;
    ss << "Try connect on: " << host << ":" << port;
    Logger::instance().log(Logger::Debug, ss.str());

    this->host = host;
    this->port = port;
    HostAddress hostaddress(host);
    socket->connectToHost(hostaddress, port);
}

void Api_client_real::tryDisconnect()
{
    if (socket != NULL) {
        socket->disconnectFromHost();
    }
}

std::string Api_client_real::getHost()
{
    return host;
}

uint16_t Api_client_real::getPort()
{
    return port;
}

void Api_client_real::sendDatapackContentMainSub(const std::string &hashMain, const std::string &hashSub)
{
    bool mainNeedUpdate = true;

    if (!hashMain.empty()) {
        if((unsigned int)hashMain.size() == (unsigned int)CommonSettingsServer::commonSettingsServer.datapackHashServerMain.size() &&
                memcmp(hashMain.data(), CommonSettingsServer::commonSettingsServer.datapackHashServerMain.data(),hashMain.size()) == 0)
        {
            mainNeedUpdate = false;
        }
    }

    bool subNeedUpdate = true;

    if (CommonSettingsServer::commonSettingsServer.datapackHashServerSub.empty() && hashSub.empty()) {
        subNeedUpdate = false;
    }
    else if (!hashSub.empty()) {
        if ((unsigned int)hashSub.size() == (unsigned int)CommonSettingsServer::commonSettingsServer.datapackHashServerSub.size() &&
                memcmp(hashSub.data(),CommonSettingsServer::commonSettingsServer.datapackHashServerSub.data(),hashSub.size()) == 0)
        {
            subNeedUpdate = false;
        }
    }

    if (!mainNeedUpdate && !subNeedUpdate)
    {
        datapackStatus=DatapackStatus::Finished;
        haveTheDatapackMainSub();
        return;
    }

    sendDatapackContentMain();
}

void Api_client_real::setProxy(const NetworkProxy& proxy)
{
    this->proxy = proxy;
    if (proxy.type() == NetworkProxy::Socks5Proxy)
    {
        qnam.setProxy(proxy);
        qnam2.setProxy(proxy);
        qnam3.setProxy(proxy);
        qnam4.setProxy(proxy);
    }
    else
    {
        qnam.setProxy(NetworkProxy::applicationProxy());
        qnam2.setProxy(NetworkProxy::applicationProxy());
        qnam3.setProxy(NetworkProxy::applicationProxy());
        qnam4.setProxy(NetworkProxy::applicationProxy());
    }
}

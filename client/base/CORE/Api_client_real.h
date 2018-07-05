#ifndef CATCHCHALLENGER_API_CLIENT_REAL_H
#define CATCHCHALLENGER_API_CLIENT_REAL_H

#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <list>
#include <cmath>
#include <sstream>

#include "../../../general/base/protocol/util/File.h"
#include "../../../general/base/protocol/net/HostAddress.h"
#include "../../../general/base/protocol/util/DataStreamSerializer.h"
#include "../../../general/base/protocol/util/ByteArray.h"
#include "../../../general/base/protocol/log/logger.h"
#include "../../general/base/GeneralVariable.h"
#include "../../general/base/GeneralStructures.h"
#include "../../general/base/CommonSettingsCommon.h"
#include "../../general/base/CommonSettingsServer.h"
#include "DatapackChecksum.h"
#include "ClientStructures2.h"
#include "Api_protocol_2.h"
#include "NetworkReply.h"
#include "NetworkProxy.h"
#include "NetworkAccessManager.h"
#include "ZstdDecodeThread.h"
//#include "qt-tar-compressed/QTarDecode.h"

#ifdef __GNUC__
    //this next header is needed to change file time/date under gcc
    #include <utime.h>
    #include <sys/stat.h>
#endif

namespace CatchChallenger
{
    class Api_client_real : public Api_protocol_2
    {

    public:
        explicit Api_client_real(ConnectedSocket* socket, const Settings& settings, bool tolerantMode = false);
        ~Api_client_real();
        void resetAll();

        //connection related
        void tryConnect(std::string host,uint16_t port);
        void tryDisconnect();
        std::string getHost();
        uint16_t getPort();

        //datapack related
        void sendDatapackContentBase(const std::string& hashBase = std::string());
        void sendDatapackContentMainSub(const std::string& hashMain = std::string(), const std::string& hashSub = std::string());
        void sendDatapackContentMain();
        void sendDatapackContentSub();
        void test_mirror_base();
        void test_mirror_main();
        void test_mirror_sub();
        void httpErrorEventBase();
        void httpErrorEventMain();
        void httpErrorEventSub();
        void decodedIsFinishBase();
        void decodedIsFinishMain();
        void decodedIsFinishSub();
        bool mirrorTryNextBase(const std::string& error);
        bool mirrorTryNextMain(const std::string& error);
        bool mirrorTryNextSub(const std::string& error);
        void httpFinishedForDatapackListBase();
        void httpFinishedForDatapackListMain();
        void httpFinishedForDatapackListSub();
        const std::vector<std::string> listDatapackBase(std::string suffix = "");
        const std::vector<std::string> listDatapackMain(std::string suffix = "");
        const std::vector<std::string> listDatapackSub(std::string suffix = "");
        void cleanDatapackBase(std::string suffix = "");
        void cleanDatapackMain(std::string suffix = "");
        void cleanDatapackSub(std::string suffix = "");
        void setProxy(const NetworkProxy& proxy);
    protected:
        bool parseReplyData(const uint8_t& mainCodeType, const uint8_t& queryNumber, const std::string& data);
        bool parseReplyData(const uint8_t& mainCodeType, const uint8_t& queryNumber, const char* const data, const unsigned int& size);

        //general data
        void defineMaxPlayers(const uint16_t& maxPlayers);
    private:
        static std::string regex_DATAPACK_FILE_REGEX;
        /// \todo group into one thread by change for queue
        ZstdDecodeThread zstdDecodeThreadBase;
        ZstdDecodeThread zstdDecodeThreadMain;
        ZstdDecodeThread zstdDecodeThreadSub;
        bool datapackTarBase;
        bool datapackTarMain;
        bool datapackTarSub;
        CatchChallenger::DatapackChecksum datapackChecksum;
        std::string host;
        uint16_t port;
        uint64_t RXSize,TXSize;
        int index_mirror_base;
        int index_mirror_main;
        int index_mirror_sub;
        NetworkProxy proxy;
        static std::regex excludePathBase;
        static std::regex excludePathMain;

        //file list
        struct query_files
        {
            uint8_t id;
            std::vector<std::string> filesName;
        };

        std::vector<query_files> query_files_list_base;
        std::vector<query_files> query_files_list_main;
        std::vector<query_files> query_files_list_sub;
        bool wait_datapack_content_base;
        bool wait_datapack_content_main;
        bool wait_datapack_content_sub;
        std::vector<std::string> datapackFilesListBase;
        std::vector<std::string> datapackFilesListMain;
        std::vector<std::string> datapackFilesListSub;
        std::vector<uint32_t> partialHashListBase;
        std::vector<uint32_t> partialHashListMain;
        std::vector<uint32_t> partialHashListSub;
        static std::string text_slash;
        static std::string text_dotcoma;
        bool httpError;
        bool httpModeBase;
        bool httpModeMain;
        bool httpModeSub;
        int qnamQueueCount;
        int qnamQueueCount2;
        int qnamQueueCount3;
        int qnamQueueCount4;
        NetworkAccessManager qnam;
        NetworkAccessManager qnam2;
        NetworkAccessManager qnam3;
        NetworkAccessManager qnam4;
        struct UrlInWaiting
        {
            std::string fileName;
        };
        std::unordered_map<NetworkReply*, UrlInWaiting> urlInWaitingListBase;
        std::unordered_map<NetworkReply*, UrlInWaiting> urlInWaitingListMain;
        std::unordered_map<NetworkReply*, UrlInWaiting> urlInWaitingListSub;

    protected:
        //virtual void writeNewFileBase(const std::string &fileName, const std::string &data);
        //virtual void writeNewFileMain(const std::string &fileName, const std::string &data);
        //virtual void writeNewFileSub(const std::string &fileName, const std::string &data);
        //virtual void checkIfContinueOrFinished();
        //virtual bool getHttpFileBase(const std::string &url, const std::string &fileName);
        //virtual bool getHttpFileMain(const std::string &url, const std::string &fileName);
        //virtual bool getHttpFileSub(const std::string &url, const std::string &fileName);
        //virtual void httpFinishedBase();
        //virtual void httpFinishedMain();
        //virtual void httpFinishedSub();
        //virtual void datapackDownloadFinishedBase();
        //virtual void datapackDownloadFinishedMain();
        //virtual void datapackDownloadFinishedSub();
        //virtual void datapackChecksumDoneBase(const std::vector<std::string> &datapackFilesList, const std::vector<char> &hash, const std::vector<uint32_t> &partialHash);
        //virtual void datapackChecksumDoneMain(const std::vector<std::string> &datapackFilesList,const std::vector<char> &hash,const std::vector<uint32_t> &partialHashList);
        //virtual void datapackChecksumDoneSub(const std::vector<std::string> &datapackFilesList,const std::vector<char> &hash,const std::vector<uint32_t> &partialHashList);
        //virtual void downloadProgressDatapackBase(int64_t bytesReceived, int64_t bytesTotal);
        //virtual void downloadProgressDatapackMain(int64_t bytesReceived, int64_t bytesTotal);
        //virtual void downloadProgressDatapackSub(int64_t bytesReceived, int64_t bytesTotal);

        //virtual void newDatapackFileBase(const uint32_t &size) const;
        //virtual void newDatapackFileMain(const uint32_t &size) const;
        //virtual void newDatapackFileSub(const uint32_t &size) const;
        //virtual void doDifferedChecksumBase(const std::string &datapackPath);
        //virtual void doDifferedChecksumMain(const std::string &datapackPath);
        //virtual void doDifferedChecksumSub(const std::string &datapackPath);
        //virtual void progressingDatapackFileBase(const uint32_t &size);
        //virtual void progressingDatapackFileMain(const uint32_t &size);
        //virtual void progressingDatapackFileSub(const uint32_t &size);

        //protocol/connection info
        //virtual  void disconnect();
    };
}

#endif // Protocol_and_data_H

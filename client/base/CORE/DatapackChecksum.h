#ifndef CATCHCHALLENGER_DATAPACKCHECKSUM_H
#define CATCHCHALLENGER_DATAPACKCHECKSUM_H

#include <iostream>
#include <vector>
#include <string>
#include <regex>
#include <unordered_set>
#include <thread>
//#include <pthread.h>
#include <openssl/sha.h>


#include "../../../general/base/protocol/general/GeneralVariable.h"
#include "../../../general/base/protocol/FacilityLib.h"
#include "../../../general/base/protocol/FacilityLibGeneral.h"
#include "../../../general/base/protocol/CommonSettingsServer.h"

namespace CatchChallenger
{
    class DatapackChecksum
    {
    public:
        explicit DatapackChecksum();
        ~DatapackChecksum();

        struct FullDatapackChecksumReturn
        {
            std::vector<std::string> datapackFilesList;
            std::vector<char>        hash;
            std::vector<uint32_t>    partialHashList;
        };

        static std::vector<char> doChecksumBase(const std::string &datapackPath);
        static std::vector<char> doChecksumMain(const std::string &datapackPath);
        static std::vector<char> doChecksumSub(const std::string &datapackPath);
        static FullDatapackChecksumReturn doFullSyncChecksumBase(const std::string &datapackPath);
        static FullDatapackChecksumReturn doFullSyncChecksumMain(const std::string &datapackPath);
        static FullDatapackChecksumReturn doFullSyncChecksumSub(const std::string &datapackPath);

        void start();
        static void work();
        void stopThread();

    private:
        #if ! defined(EPOLLCATCHCHALLENGERSERVER)
            static std::thread thread;
            //static pthread_t   pthread;
        #endif

        #if ! defined(EPOLLCATCHCHALLENGERSERVER)
            void doDifferedChecksumBase(const std::string &datapackPath);
            void doDifferedChecksumMain(const std::string &datapackPath);
            void doDifferedChecksumSub(const std::string &datapackPath);
        #endif

        //virtual void datapackChecksumDoneBase(const std::vector<std::string> &datapackFilesList,const std::vector<char> &hash,const std::vector<uint32_t> &partialHashList);
        //virtual void datapackChecksumDoneMain(const std::vector<std::string> &datapackFilesList,const std::vector<char> &hash,const std::vector<uint32_t> &partialHashList);
        //virtual void datapackChecksumDoneSub(const std::vector<std::string> &datapackFilesList,const std::vector<char> &hash,const std::vector<uint32_t> &partialHashList);

    };
}

#endif // CATCHCHALLENGER_DATAPACKCHECKSUM_H

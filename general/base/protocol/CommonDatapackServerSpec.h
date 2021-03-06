#ifndef COMMONDATAPACK_ServerSpec_H
#define COMMONDATAPACK_ServerSpec_H

#include <unordered_map>
#include <string>

#include "general/GeneralStructures.h"
#include "CommonDatapack.h"

namespace CatchChallenger
{
    class CommonDatapackServerSpec
    {
        public:
            explicit CommonDatapackServerSpec();
        public:
            void unload();
            void parseDatapack(const std::string& datapackPath, const std::string& mainDatapackCode, const std::string& subDatapackCode);
            bool isParsedContent() const;
        public:
            std::unordered_map<uint16_t, BotFight> botFights;
            uint16_t botFightsMaxId;
            std::unordered_map<uint16_t, Quest> quests;
            /// \see CommonMap, std::unordered_map<std::pair<uint8_t,uint8_t>,std::vector<uint16_t>, pairhash> shops;
            std::unordered_map<uint16_t, Shop> shops;
            std::vector<ServerSpecProfile> serverProfileList;
            std::unordered_map<uint16_t, std::vector<MonsterDrops>> monsterDrops;//to prevent send network packet for item when luck is 100%
            static CommonDatapackServerSpec commonDatapackServerSpec;
        private:
            bool isParsedSpec;
            bool parsingSpec;
            std::string datapackPath;
            std::string mainDatapackCode;
            std::string subDatapackCode;
        private:
            void parseQuests();
            void parseBotFights();//gold/item variables by server
            void parseShop();
            void parseServerProfileList();
            void parseIndustries();
            #ifdef CATCHCHALLENGER_CLIENT
                void applyMonstersRate();//xp,sp variable by server, only have this second pass on client
            #endif
            #ifndef CATCHCHALLENGER_CLASS_MASTER
                void parseMonstersDrop();//drop rate variable by server
            #endif
    };
}

#endif // COMMONDATAPACK_ServerSpec_H

#ifndef API_PROTOCOL_2_H
#define API_PROTOCOL_2_H

#include <iostream>
#include <vector>

#include <QObject>
#include <QString>
#include <QCoreApplication>
#include <QString>
#include <QList>
#include <QPair>
#include <QDir>
#include <QFileInfo>
#include <QDateTime>
#include <QStandardPaths>
#include <QSslKey>


#include "../../general/base/protocol/util/DataStreamSerializer.h"
#include "../../general/base/protocol/util/sha224.h"

#include "../../general/base/tinyXML2/tinyxml2.h"
#include "../../general/base/tinyXML2/customtinyxml2.h"

#include "ClientStructures.h"
#include "../../general/base/protocol/general/GeneralStructures.h"
#include "../../general/base/protocol/general/GeneralVariable.h"

#include "../../general/base/protocol/ProtocolParsing2.h"
#include "../../general/base/protocol/MoveOnTheMap.h"
#include "../../general/base/protocol/ProtocolVersion2.h"
#include "../../general/base/protocol/CommonDatapack.h"

#include "../../general/base/CommonSettingsCommon.h"

#include "../../general/base/protocol/CommonSettingsServer.h"
#include "../../general/base/protocol/FacilityLib.h"
#include "../../general/base/protocol/FacilityLibGeneral.h"
#include "../../general/base/protocol/general/GeneralType.h"
#include "../../general/base/protocol/log/logger.h"
#include "../../general/base/protocol/config/Settings.h"

namespace CatchChallenger
{
    class Api_protocol_2 : public ProtocolParsingInputOutput, public MoveOnTheMap
    {
            Settings setting;
        public:
            static bool internalVersionDisplayed;

        public:
            //setting.setAppPath(QCoreApplication::applicationDirPath().toStdString()).setDatapackDir("/datapack");
            explicit Api_protocol_2(ConnectedSocket* socket, bool tolerantMode = false, Settings setting);
            ~Api_protocol_2();

            bool disconnectClient();
            void unloadSelection();
            const ServerFromPoolForDisplay &getCurrentServer(const unsigned int &index);
            bool dataToPlayerMonster(QDataStream &in, PlayerMonster &monster);

            //protocol command
            bool tryLogin(const std::string &login,const std::string &pass);
            bool tryCreateAccount();
            bool sendProtocol();
            bool protocolWrong() const;
            virtual std::string socketDisconnectedForReconnect();
            const std::vector<ServerFromPoolForDisplay> &getServerOrdenedList();

            //get the stored data
            Player_private_and_public_informations &get_player_informations();
            const Player_private_and_public_informations &get_player_informations_ro() const;
            std::string getPseudo();
            uint16_t getId();

            virtual void sendDatapackContentBase(const std::string &hashBase=std::string()) = 0;
            virtual void sendDatapackContentMainSub(const std::string &hashMain=std::string(),const std::string &hashSub=std::string()) = 0;
            virtual void tryDisconnect() = 0;
            virtual std::string datapackPathBase() const;
            virtual std::string datapackPathMain() const;
            virtual std::string datapackPathSub() const;
            virtual std::string mainDatapackCode() const;
            virtual std::string subDatapackCode() const;
            void setDatapackPath(const std::string &datapackPath);
            std::string toUTF8WithHeader(const std::string &text);
            void have_main_and_sub_datapack_loaded();//can now load player_informations
            bool character_select_is_send();

            enum StageConnexion
            {
                Stage1 = 0x01, //Connect on login server
                Stage2 = 0x02, //reconnexion in progress
                Stage3 = 0x03, //connecting on game server
                Stage4 = 0x04, //connected on game server
            };
            StageConnexion stage() const;

            enum DatapackStatus
            {
                Base     = 0x01,
                Main     = 0x02,
                Sub      = 0x03,
                Finished = 0x04
            };
            DatapackStatus datapackStatus;

            //to reset all
            void resetAll();

            bool getIsLogged() const;
            bool getCaracterSelected() const;
            std::map<uint8_t,uint64_t> getQuerySendTimeList() const;

            //to manipulate the monsters
            Player_private_and_public_informations player_informations;

            enum ProxyMode
            {
                Reconnect = 0x01,
                Proxy     = 0x02
            };
            ProxyMode proxyMode;

            bool setMapNumber(const unsigned int number_of_map);
        private:
            //status for the query
            bool haveFirstHeader;
            bool is_logged;
            bool character_selected;
            bool character_select_send;
            bool have_send_protocol;
            bool have_receive_protocol;
            bool tolerantMode;
            bool haveTheServerList;
            bool haveTheLogicalGroupList;

            static std::string text_balise_root_start;
            static std::string text_balise_root_stop;
            static std::string text_name;
            static std::string text_description;
            static std::string text_en;
            static std::string text_lang;
            static std::string text_slash;

            LogicialGroup logicialGroup;

            StageConnexion stageConnexion;

            //to send trame
            std::vector<uint8_t> lastQueryNumber;

            #ifdef BENCHMARKMUTIPLECLIENT
                static char hurgeBufferForBenchmark[4096];
                static bool precomputeDone;
                static char hurgeBufferMove[4];
            #endif

            struct DelayedReply
            {
                uint8_t packetCode;
                uint8_t queryNumber;
                std::string data;
            };
            DelayedReply delayedLogin;
            struct DelayedMessage
            {
                uint8_t packetCode;
                std::string data;
            };
            std::vector<DelayedMessage> delayedMessages;
        protected:
            void QtsocketDestroyed();
            virtual void socketDestroyed();
            void parseIncommingData();
            void readForFirstHeader();
            void sslHandcheckIsFinished();
            void connectTheExternalSocketInternal();
            void saveCert(const std::string &file);

            void errorParsingLayer(const std::string &error);
            void messageParsingLayer(const std::string &message) const;

            bool parseCharacterBlockServer(const uint8_t &packetCode,const uint8_t &queryNumber,const std::string &data);
            bool parseCharacterBlockCharacter(const uint8_t &packetCode,const uint8_t &queryNumber,const std::string &data);

            /// \note This is note into server part to force to write manually the serial and improve the performance, this function is more easy but implies lot of memory copy via SIMD
            //send message without reply
            bool packOutcommingData(const uint8_t &packetCode,const char * const data,const int &size);
            //send query with reply
            bool packOutcommingQuery(const uint8_t &packetCode,const uint8_t &queryNumber,const char * const data,const int &size);
            //send reply
            bool postReplyData(const uint8_t &queryNumber, const char * const data,const int &size);
            //the internal serialiser
            void send_player_move_internal(const uint8_t &moved_unit,const CatchChallenger::Direction &direction);
        protected:
            //have message without reply
            virtual bool parseMessage(const uint8_t &packetCode,const char * const data,const unsigned int &size);
            //have query with reply
            virtual bool parseQuery(const uint8_t &packetCode,const uint8_t &queryNumber,const char * const data,const unsigned int &size);
            //send reply
            virtual bool parseReplyData(const uint8_t &packetCode,const uint8_t &queryNumber,const char * const data,const unsigned int &size);

            //have message without reply
            virtual bool parseMessage(const uint8_t &packetCode,const std::string &data);
            //have query with reply
            virtual bool parseQuery(const uint8_t &packetCode,const uint8_t &queryNumber,const std::string &data);
            //send reply
            virtual bool parseReplyData(const uint8_t &packetCode,const uint8_t &queryNumber,const std::string &data);

            //servers list
            LogicialGroup * addLogicalGroup(const std::string &path, const std::string &xml, const std::string &language);
            ServerFromPoolForDisplay *addLogicalServer(const ServerFromPoolForDisplayTemp &server, const std::string &language);

            //error
            void parseError(const std::string &userMessage, const std::string &errorString);

            //general data
            virtual void defineMaxPlayers(const uint16_t &maxPlayers) = 0;

            //stored local player info
            uint16_t max_players;
            uint16_t max_players_real;
            uint32_t number_of_map;

            //to send trame
            uint8_t queryNumber();
            static std::unordered_set<std::string> extensionAllowed;
            std::map<uint8_t,uint64_t> querySendTime;

            //inventory
            std::vector<uint16_t> lastObjectUsed;

            //datapack
            std::string mDatapackBase;
            std::string mDatapackMain;
            std::string mDatapackSub;

            //teleport list query id
            struct TeleportQueryInformation
            {
                uint8_t queryId;
                Direction direction;//for the internal set of last_direction
            };

            std::vector<TeleportQueryInformation> teleportList;

            //trade
            std::vector<uint8_t> tradeRequestId;
            bool isInTrade;
            //battle
            std::vector<uint8_t> battleRequestId;
            bool isInBattle;
            std::string token;
            std::string passHash;
            std::string loginHash;

            //server list
            int32_t selectedServerIndex;
            std::vector<ServerFromPoolForDisplay> serverOrdenedList;
            std::vector<LogicialGroup *> logicialGroupIndexList;
            std::vector<std::vector<CharacterEntry> > characterListForSelection;
            std::string tokenForGameServer;

        public:
            void send_player_direction(const CatchChallenger::Direction &the_direction);
            void send_player_move(const uint8_t &moved_unit,const CatchChallenger::Direction &direction);

            void sendChatText(const CatchChallenger::Chat_type &chatType,const std::string &text);
            void sendPM(const std::string &text,const std::string &pseudo);
            bool teleportDone();

            //character
            bool addCharacter(const uint8_t &charactersGroupIndex,const uint8_t &profileIndex, const std::string &pseudo, const uint8_t &monsterGroupId, const uint8_t &skinId);
            bool removeCharacter(const uint8_t &charactersGroupIndex,const uint32_t &characterId);
            bool selectCharacter(const uint8_t &charactersGroupIndex, const uint32_t &serverUniqueKey, const uint32_t &characterId);
            bool selectCharacter(const uint8_t &charactersGroupIndex, const uint32_t &serverUniqueKey, const uint32_t &characterId,const uint32_t &serverIndex);
            LogicialGroup getLogicialGroup() const;

            //plant, can do action only if the previous is finish
            void useSeed(const uint8_t &plant_id);
            void collectMaturePlant();
            //crafting
            void useRecipe(const uint16_t &recipeId);
            void addRecipe(const uint16_t &recipeId);

            //trade
            void tradeRefused();
            void tradeAccepted();
            void tradeCanceled();
            void tradeFinish();
            void addTradeCash(const uint64_t &cash);
            void addObject(const uint16_t &item,const uint32_t &quantity);
            void addMonsterByPosition(const uint8_t &monsterPosition);

            //battle
            void battleRefused();
            void battleAccepted();

            //inventory
            void destroyObject(const uint16_t &object,const uint32_t &quantity=1);
            bool useObject(const uint16_t &object);
            bool useObjectOnMonsterByPosition(const uint16_t &object, const uint8_t &monsterPosition);
            void wareHouseStore(const int64_t &cash, const std::vector<std::pair<uint16_t, int32_t> > &items, const std::vector<uint32_t> &withdrawMonsters, const std::vector<uint32_t> &depositeMonsters);
            void takeAnObjectOnMap();

            //shop
            void getShopList(const uint16_t &shopId);/// \see CommonMap, std::unordered_map<std::pair<uint8_t,uint8_t>,std::vector<uint16_t>, pairhash> shops;
            void buyObject(const uint16_t &shopId,const uint16_t &objectId,const uint32_t &quantity,const uint32_t &price);/// \see CommonMap, std::unordered_map<std::pair<uint8_t,uint8_t>,std::vector<uint16_t>, pairhash> shops;
            void sellObject(const uint16_t &shopId, const uint16_t &objectId, const uint32_t &quantity, const uint32_t &price);/// \see CommonMap, std::unordered_map<std::pair<uint8_t,uint8_t>,std::vector<uint16_t>, pairhash> shops;

            //factory
            void getFactoryList(const uint16_t &factoryId);
            void buyFactoryProduct(const uint16_t &factoryId,const uint16_t &objectId,const uint32_t &quantity,const uint32_t &price);
            void sellFactoryResource(const uint16_t &factoryId, const uint16_t &objectId, const uint32_t &quantity, const uint32_t &price);

            //fight
            void tryEscape();
            void useSkill(const uint16_t &skill);
            void heal();
            void requestFight(const uint16_t &fightId);
            void changeOfMonsterInFightByPosition(const uint8_t &monsterPosition);

            //monster
            void learnSkillByPosition(const uint8_t &monsterPosition, const uint16_t &skill);
            void monsterMoveDown(const uint8_t &number);
            void monsterMoveUp(const uint8_t &number);
            void confirmEvolutionByPosition(const uint8_t &monterPosition);

            //quest
            void startQuest(const uint16_t &questId);
            void finishQuest(const uint16_t &questId);
            void cancelQuest(const uint16_t &questId);
            void nextQuestStep(const uint16_t &questId);

            //clan
            void createClan(const std::string &name);
            void leaveClan();
            void dissolveClan();
            void inviteClan(const std::string &pseudo);
            void ejectClan(const std::string &pseudo);
            void inviteAccept(const bool &accept);
            void waitingForCityCapture(const bool &cancel);

            //market
            void getMarketList();
            void buyMarketObject(const uint32_t &marketObjectId,const uint32_t &quantity=1);
            void buyMarketMonster(const uint32_t &monsterMarketId);
            void putMarketObject(const uint16_t &objectId, const uint32_t &quantity, const uint64_t &price);
            void putMarketMonsterByPosition(const uint8_t &monsterPosition,const uint64_t &price);
            void recoverMarketCash();
            void withdrawMarketObject(const uint16_t &objectPosition, const uint32_t &quantity=1);
            void withdrawMarketMonster(const uint32_t &monsterMarketId);



        public:
            void newError(const std::string &error,const std::string &detailedError);
            void message(const std::string &message);
            void lastReplyTime(const uint32_t &time);

            //protocol/connection info
            void disconnected(const std::string &reason);
            void notLogged(const std::string &reason);
            void logged(const std::vector<std::vector<CharacterEntry> > &characterEntryList);
            void protocol_is_good();
            void connectedOnLoginServer();
            void connectingOnGameServer();
            void connectedOnGameServer();
            void haveDatapackMainSubCode();
            void gatewayCacheUpdate(const uint8_t gateway,const uint8_t progression);

            //general info
            void number_of_player(const uint16_t &number,const uint16_t &max_players);
            void random_seeds(const std::string &data);

            //character
            void newCharacterId(const uint8_t &returnCode,const uint32_t &characterId);
            void haveCharacter();

            //events
            void setEvents(const std::vector<std::pair<uint8_t,uint8_t> > &events);
            void newEvent(const uint8_t &event,const uint8_t &event_value);

            //map move
            void insert_player(const CatchChallenger::Player_public_informations &player,const uint32_t &mapId,const uint8_t &x,const uint8_t &y,const CatchChallenger::Direction &direction);
            void move_player(const uint16_t &id,const std::vector<std::pair<uint8_t,CatchChallenger::Direction> > &movement);
            void remove_player(const uint16_t &id);
            void reinsert_player(const uint16_t &id,const uint8_t &x,const uint8_t &y,const CatchChallenger::Direction &direction);
            void full_reinsert_player(const uint16_t &id,const uint32_t &mapId,const uint8_t &x,const uint8_t y,const CatchChallenger::Direction &direction);
            void dropAllPlayerOnTheMap();
            void teleportTo(const uint32_t &mapId,const uint8_t &x,const uint8_t &y,const CatchChallenger::Direction &direction);

            //plant
            void insert_plant(const uint32_t &mapId,const uint8_t &x,const uint8_t &y,const uint8_t &plant_id,const uint16_t &seconds_to_mature);
            void remove_plant(const uint32_t &mapId,const uint8_t &x,const uint8_t &y);
            void seed_planted(const bool &ok);
            void plant_collected(const CatchChallenger::Plant_collect &stat);

            //crafting
            void recipeUsed(const RecipeUsage &recipeUsage);

            //inventory
            void objectUsed(const ObjectUsage &objectUsage);
            void monsterCatch(const bool &success);

            //chat
            void new_chat_text(const CatchChallenger::Chat_type &chat_type,const std::string &text,const std::string &pseudo,const CatchChallenger::Player_type &player_type);
            void new_system_text(const CatchChallenger::Chat_type &chat_type,const std::string &text);

            //player info
            void have_current_player_info(const CatchChallenger::Player_private_and_public_informations &informations);
            void have_inventory(const std::unordered_map<uint16_t,uint32_t> &items,const std::unordered_map<uint16_t,uint32_t> &warehouse_items);
            void add_to_inventory(const std::unordered_map<uint16_t,uint32_t> &items);
            void remove_to_inventory(const std::unordered_map<uint16_t,uint32_t> &items);

            //datapack
            void haveTheDatapack();
            void haveTheDatapackMainSub();

            //base
            void newFileBase(const std::string &fileName,const std::string &data);
            void newHttpFileBase(const std::string &url,const std::string &fileName);
            void removeFileBase(const std::string &fileName);
            void datapackSizeBase(const uint32_t &datapckFileNumber,const uint32_t &datapckFileSize);

            //main
            void newFileMain(const std::string &fileName,const std::string &data);
            void newHttpFileMain(const std::string &url,const std::string &fileName);
            void removeFileMain(const std::string &fileName);
            void datapackSizeMain(const uint32_t &datapckFileNumber,const uint32_t &datapckFileSize);

            //sub
            void newFileSub(const std::string &fileName,const std::string &data);
            void newHttpFileSub(const std::string &url,const std::string &fileName);
            void removeFileSub(const std::string &fileName);
            void datapackSizeSub(const uint32_t &datapckFileNumber,const uint32_t &datapckFileSize);

            //shop
            void haveShopList(const std::vector<ItemToSellOrBuy> &items);
            void haveBuyObject(const BuyStat &stat,const uint32_t &newPrice);
            void haveSellObject(const SoldStat &stat,const uint32_t &newPrice);

            //factory
            void haveFactoryList(const uint32_t &remainingProductionTime,const std::vector<ItemToSellOrBuy> &resources,const std::vector<ItemToSellOrBuy> &products);
            void haveBuyFactoryObject(const BuyStat &stat,const uint32_t &newPrice);
            void haveSellFactoryObject(const SoldStat &stat,const uint32_t &newPrice);

            //trade
            void tradeRequested(const std::string &pseudo,const uint8_t &skinInt);
            void tradeAcceptedByOther(const std::string &pseudo,const uint8_t &skinInt);
            void tradeCanceledByOther();
            void tradeFinishedByOther();
            void tradeValidatedByTheServer();
            void tradeAddTradeCash(const uint64_t &cash);
            void tradeAddTradeObject(const uint32_t &item,const uint32_t &quantity);
            void tradeAddTradeMonster(const CatchChallenger::PlayerMonster &monster);

            //battle
            void battleRequested(const std::string &pseudo,const uint8_t &skinInt);
            void battleAcceptedByOther(const std::string &pseudo,const uint8_t &skinId,const std::vector<uint8_t> &stat,const uint8_t &monsterPlace,const PublicPlayerMonster &publicPlayerMonster);
            void battleCanceledByOther();
            void sendBattleReturn(const std::vector<Skill::AttackReturn> &attackReturn);

            //clan
            void clanActionSuccess(const uint32_t &clanId);
            void clanActionFailed();
            void clanDissolved();
            void clanInformations(const std::string &name);
            void clanInvite(const uint32_t &clanId,const std::string &name);
            void cityCapture(const uint32_t &remainingTime,const uint8_t &type);

            //city
            void captureCityYourAreNotLeader();
            void captureCityYourLeaderHaveStartInOtherCity(const std::string &zone);
            void captureCityPreviousNotFinished();
            void captureCityStartBattle(const uint16_t &player_count,const uint16_t &clan_count);
            void captureCityStartBotFight(const uint16_t &player_count,const uint16_t &clan_count,const uint32_t &fightId);
            void captureCityDelayedStart(const uint16_t &player_count,const uint16_t &clan_count);
            void captureCityWin();

            //market
            void marketList(const uint64_t &price,const std::vector<MarketObject> &marketObjectList,const std::vector<MarketMonster> &marketMonsterList,const std::vector<MarketObject> &marketOwnObjectList,const std::vector<MarketMonster> &marketOwnMonsterList);
            void marketBuy(const bool &success);
            void marketBuyMonster(const PlayerMonster &playerMonster);
            void marketPut(const bool &success);
            void marketGetCash(const uint64_t &cash);
            void marketWithdrawCanceled();
            void marketWithdrawObject(const uint32_t &objectId,const uint32_t &quantity);
            void marketWithdrawMonster(const PlayerMonster &playerMonster);

        signals:
            void QtnewError(const std::string &error,const std::string &detailedError);
            void Qtmessage(const std::string &message);
            void QtlastReplyTime(const uint32_t &time);

            //protocol/connection info
            void Qtdisconnected(const std::string &reason);
            void QtnotLogged(const std::string &reason);
            void Qtlogged(const std::vector<std::vector<CharacterEntry> > &characterEntryList);
            void Qtprotocol_is_good();
            void QtconnectedOnLoginServer();
            void QtconnectingOnGameServer();
            void QtconnectedOnGameServer();
            void QthaveDatapackMainSubCode();
            void QtgatewayCacheUpdate(const uint8_t gateway,const uint8_t progression);

            //general info
            void Qtnumber_of_player(const uint16_t &number,const uint16_t &max_players);
            void Qtrandom_seeds(const std::string &data);

            //character
            void QtnewCharacterId(const uint8_t &returnCode,const uint32_t &characterId);
            void QthaveCharacter();

            //events
            void QtsetEvents(const std::vector<std::pair<uint8_t,uint8_t> > &events);
            void QtnewEvent(const uint8_t &event,const uint8_t &event_value);

            //map move
            void Qtinsert_player(const CatchChallenger::Player_public_informations &player,const uint32_t &mapId,const uint8_t &x,const uint8_t &y,const CatchChallenger::Direction &direction);
            void Qtmove_player(const uint16_t &id,const std::vector<std::pair<uint8_t,CatchChallenger::Direction> > &movement);
            void Qtremove_player(const uint16_t &id);
            void Qtreinsert_player(const uint16_t &id,const uint8_t &x,const uint8_t &y,const CatchChallenger::Direction &direction);
            void Qtfull_reinsert_player(const uint16_t &id,const uint32_t &mapId,const uint8_t &x,const uint8_t y,const CatchChallenger::Direction &direction);
            void QtdropAllPlayerOnTheMap();
            void QtteleportTo(const uint32_t &mapId,const uint8_t &x,const uint8_t &y,const CatchChallenger::Direction &direction);

            //plant
            void Qtinsert_plant(const uint32_t &mapId,const uint8_t &x,const uint8_t &y,const uint8_t &plant_id,const uint16_t &seconds_to_mature);
            void Qtremove_plant(const uint32_t &mapId,const uint8_t &x,const uint8_t &y);
            void Qtseed_planted(const bool &ok);
            void Qtplant_collected(const CatchChallenger::Plant_collect &stat);

            //crafting
            void QtrecipeUsed(const RecipeUsage &recipeUsage);

            //inventory
            void QtobjectUsed(const ObjectUsage &objectUsage);
            void QtmonsterCatch(const bool &success);

            //chat
            void Qtnew_chat_text(const CatchChallenger::Chat_type &chat_type,const std::string &text,const std::string &pseudo,const CatchChallenger::Player_type &player_type);
            void Qtnew_system_text(const CatchChallenger::Chat_type &chat_type,const std::string &text);

            //player info
            void Qthave_current_player_info(const CatchChallenger::Player_private_and_public_informations &informations);
            void Qthave_inventory(const std::unordered_map<uint16_t,uint32_t> &items,const std::unordered_map<uint16_t,uint32_t> &warehouse_items);
            void Qtadd_to_inventory(const std::unordered_map<uint16_t,uint32_t> &items);
            void Qtremove_to_inventory(const std::unordered_map<uint16_t,uint32_t> &items);

            //datapack
            void QthaveTheDatapack();
            void QthaveTheDatapackMainSub();

            //base
            void QtnewFileBase(const std::string &fileName,const std::string &data);
            void QtnewHttpFileBase(const std::string &url,const std::string &fileName);
            void QtremoveFileBase(const std::string &fileName);
            void QtdatapackSizeBase(const uint32_t &datapckFileNumber,const uint32_t &datapckFileSize);

            //main
            void QtnewFileMain(const std::string &fileName,const std::string &data);
            void QtnewHttpFileMain(const std::string &url,const std::string &fileName);
            void QtremoveFileMain(const std::string &fileName);
            void QtdatapackSizeMain(const uint32_t &datapckFileNumber,const uint32_t &datapckFileSize);
            //sub
            void QtnewFileSub(const std::string &fileName,const std::string &data);
            void QtnewHttpFileSub(const std::string &url,const std::string &fileName);
            void QtremoveFileSub(const std::string &fileName);
            void QtdatapackSizeSub(const uint32_t &datapckFileNumber,const uint32_t &datapckFileSize);

            //shop
            void QthaveShopList(const std::vector<ItemToSellOrBuy> &items);
            void QthaveBuyObject(const BuyStat &stat,const uint32_t &newPrice);
            void QthaveSellObject(const SoldStat &stat,const uint32_t &newPrice);

            //factory
            void QthaveFactoryList(const uint32_t &remainingProductionTime,const std::vector<ItemToSellOrBuy> &resources,const std::vector<ItemToSellOrBuy> &products);
            void QthaveBuyFactoryObject(const BuyStat &stat,const uint32_t &newPrice);
            void QthaveSellFactoryObject(const SoldStat &stat,const uint32_t &newPrice);

            //trade
            void QttradeRequested(const std::string &pseudo,const uint8_t &skinInt);
            void QttradeAcceptedByOther(const std::string &pseudo,const uint8_t &skinInt);
            void QttradeCanceledByOther();
            void QttradeFinishedByOther();
            void QttradeValidatedByTheServer();
            void QttradeAddTradeCash(const uint64_t &cash);
            void QttradeAddTradeObject(const uint32_t &item,const uint32_t &quantity);
            void QttradeAddTradeMonster(const CatchChallenger::PlayerMonster &monster);

            //battle
            void QtbattleRequested(const std::string &pseudo,const uint8_t &skinInt);
            void QtbattleAcceptedByOther(const std::string &pseudo,const uint8_t &skinId,const std::vector<uint8_t> &stat,const uint8_t &monsterPlace,const PublicPlayerMonster &publicPlayerMonster);
            void QtbattleCanceledByOther();
            void QtsendBattleReturn(const std::vector<Skill::AttackReturn> &attackReturn);

            //clan
            void QtclanActionSuccess(const uint32_t &clanId);
            void QtclanActionFailed();
            void QtclanDissolved();
            void QtclanInformations(const std::string &name);
            void QtclanInvite(const uint32_t &clanId,const std::string &name);
            void QtcityCapture(const uint32_t &remainingTime,const uint8_t &type);

            //city
            void QtcaptureCityYourAreNotLeader();
            void QtcaptureCityYourLeaderHaveStartInOtherCity(const std::string &zone);
            void QtcaptureCityPreviousNotFinished();
            void QtcaptureCityStartBattle(const uint16_t &player_count,const uint16_t &clan_count);
            void QtcaptureCityStartBotFight(const uint16_t &player_count,const uint16_t &clan_count,const uint32_t &fightId);
            void QtcaptureCityDelayedStart(const uint16_t &player_count,const uint16_t &clan_count);
            void QtcaptureCityWin();

            //market
            void QtmarketList(const uint64_t &price,const std::vector<MarketObject> &marketObjectList,const std::vector<MarketMonster> &marketMonsterList,const std::vector<MarketObject> &marketOwnObjectList,const std::vector<MarketMonster> &marketOwnMonsterList);
            void QtmarketBuy(const bool &success);
            void QtmarketBuyMonster(const PlayerMonster &playerMonster);
            void QtmarketPut(const bool &success);
            void QtmarketGetCash(const uint64_t &cash);
            void QtmarketWithdrawCanceled();
            void QtmarketWithdrawObject(const uint32_t &objectId,const uint32_t &quantity);
            void QtmarketWithdrawMonster(const PlayerMonster &playerMonster);
    };
}

#endif // API_PROTOCOL_QT_2

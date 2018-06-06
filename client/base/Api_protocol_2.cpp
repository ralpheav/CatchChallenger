#include "Api_protocol_2.h"

using namespace CatchChallenger;

Api_protocol_2::Api_protocol_2()
{
    inProgress = false;
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

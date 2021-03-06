#include "DatapackGeneralLoader.h"
#include "GeneralVariable.h"
#include "CommonDatapack.h"
#include "CommonSettingsServer.h"
#include "FacilityLib.h"
#include "FacilityLibGeneral.h"
#include "cpp11addition.h"

#include <vector>
#include <iostream>

using namespace CatchChallenger;

std::vector<Reputation> DatapackGeneralLoader::loadReputation(const std::string &file)
{
    std::regex excludeFilterRegex("[\"']");
    std::regex typeRegex("^[a-z]{1,32}$");
    std::vector<Reputation> reputation;
    tinyxml2::XMLDocument *domDocument;
    #ifndef EPOLLCATCHCHALLENGERSERVER
    if(CommonDatapack::commonDatapack.xmlLoadedFile.find(file)!=CommonDatapack::commonDatapack.xmlLoadedFile.cend())
        domDocument=&CommonDatapack::commonDatapack.xmlLoadedFile[file];
    else
    {
        domDocument=&CommonDatapack::commonDatapack.xmlLoadedFile[file];
        #else
        domDocument=new tinyxml2::XMLDocument();
        #endif
        const auto loadOkay = domDocument->LoadFile(file.c_str());
        if(loadOkay!=0)
        {
            std::cerr << file+", "+tinyxml2errordoc(domDocument) << std::endl;
            return reputation;
        }
        #ifndef EPOLLCATCHCHALLENGERSERVER
    }
    #endif
    const tinyxml2::XMLElement * root = domDocument->RootElement();
    if(root==NULL)
    {
        std::cerr << "Unable to open the file: " << file << ", no root balise found for the xml file" << std::endl;
        return reputation;
    }
    if(strcmp(root->Name(),"reputations")!=0)
    {
        std::cerr << "Unable to open the file: " << file << ", \"reputations\" root balise not found for reputation of the xml file" << std::endl;
        return reputation;
    }

    //load the content
    bool ok;
    const tinyxml2::XMLElement * item = root->FirstChildElement("reputation");
    while(item!=NULL)
    {
        if(item->Attribute("type")!=NULL)
        {
            std::vector<int32_t> point_list_positive,point_list_negative;
            std::vector<std::string> text_positive,text_negative;
            const tinyxml2::XMLElement * level = item->FirstChildElement("level");
            ok=true;
            while(level!=NULL && ok)
            {
                if(level->Attribute("point")!=NULL)
                {
                    const int32_t &point=stringtoint32(level->Attribute("point"),&ok);
                    std::string text_val;
                    if(ok)
                    {
                        ok=true;
                        bool found=false;
                        unsigned int index=0;
                        if(point>=0)
                        {
                            while(index<point_list_positive.size())
                            {
                                if(point_list_positive.at(index)==point)
                                {
                                    std::cerr << "Unable to open the file: " << file << ", reputation level with same number of point found!: child->Name(): "
                                              << item->Name() << std::endl;
                                    found=true;
                                    ok=false;
                                    break;
                                }
                                if(point_list_positive.at(index)>point)
                                {
                                    point_list_positive.insert(point_list_positive.begin()+index,point);
                                    text_positive.insert(text_positive.begin()+index,text_val);
                                    found=true;
                                    break;
                                }
                                index++;
                            }
                            if(!found)
                            {
                                point_list_positive.push_back(point);
                                text_positive.push_back(text_val);
                            }
                        }
                        else
                        {
                            while(index<point_list_negative.size())
                            {
                                if(point_list_negative.at(index)==point)
                                {
                                    std::cerr << "Unable to open the file: " << file << ", reputation level with same number of point found!: child->Name(): "
                                              << item->Name() << std::endl;
                                    found=true;
                                    ok=false;
                                    break;
                                }
                                if(point_list_negative.at(index)<point)
                                {
                                    point_list_negative.insert(point_list_negative.begin()+index,point);
                                    text_negative.insert(text_negative.begin()+index,text_val);
                                    found=true;
                                    break;
                                }
                                index++;
                            }
                            if(!found)
                            {
                                point_list_negative.push_back(point);
                                text_negative.push_back(text_val);
                            }
                        }
                    }
                    else
                        std::cerr << "Unable to open the file: " << file << ", point is not number: child->Name(): " << item->Name() << std::endl;
                }
                level = level->NextSiblingElement("level");
            }
            std::sort(point_list_positive.begin(),point_list_positive.end());
            std::sort(point_list_negative.begin(),point_list_negative.end());
            std::reverse(point_list_negative.begin(),point_list_negative.end());
            if(ok)
                if(point_list_positive.size()<2)
                {
                    std::cerr << "Unable to open the file: " << file << ", reputation have to few level: child->Name(): " << item->Name() << std::endl;
                    ok=false;
                }
            if(ok)
                if(!vectorcontainsAtLeastOne(point_list_positive,0))
                {
                    std::cerr << "Unable to open the file: " << file << ", no starting level for the positive: child->Name(): " << item->Name() << std::endl;
                    ok=false;
                }
            if(ok)
                if(!point_list_negative.empty() && !vectorcontainsAtLeastOne(point_list_negative,-1))
                {
                    //std::cerr << "Unable to open the file: " << file << ", no starting level for the negative, first level need start with -1, fix by change range: child->Name(): " << item->Name() << std::endl;
                    std::vector<int32_t> point_list_negative_new;
                    int lastValue=-1;
                    unsigned int index=0;
                    while(index<point_list_negative.size())
                    {
                        point_list_negative_new.push_back(lastValue);
                        lastValue=point_list_negative.at(index);//(1 less to negative value)
                        index++;
                    }
                    point_list_negative=point_list_negative_new;
                }
            if(ok)
            {
                const std::string &type=item->Attribute("type");
                if(!std::regex_match(type,typeRegex))
                {
                    std::cerr << "Unable to open the file: " << file << ", the type " << item->Attribute("type")
                              << " don't match wiuth the regex: ^[a-z]{1,32}$: child->Name(): " << item->Name() << std::endl;
                    ok=false;
                }
                else
                {
                    if(!std::regex_match(type,excludeFilterRegex))
                    {
                        Reputation reputationTemp;
                        reputationTemp.name=type;
                        reputationTemp.reputation_positive=point_list_positive;
                        reputationTemp.reputation_negative=point_list_negative;
                        if(reputation.size()>=255)
                        {
                            std::cerr << "You can't have mopre than 255 reputation " << file << ": " << item->Name() << std::endl;
                            abort();
                        }
                        reputation.push_back(reputationTemp);
                    }
                }
            }
        }
        else
            std::cerr << "Unable to open the file: " << file << ", have not the item id: child->Name(): " << item->Name() << std::endl;
        item = item->NextSiblingElement("reputation");
    }

    #ifdef EPOLLCATCHCHALLENGERSERVER
    delete domDocument;
    #endif
    return reputation;
}

#ifndef CATCHCHALLENGER_CLASS_MASTER
std::unordered_map<uint16_t, Quest> DatapackGeneralLoader::loadQuests(const std::string &folder)
{
    bool ok;
    std::unordered_map<uint16_t, Quest> quests;
    //open and quick check the file
    const std::vector<FacilityLibGeneral::InodeDescriptor> &fileList=CatchChallenger::FacilityLibGeneral::listFolderNotRecursive(folder,CatchChallenger::FacilityLibGeneral::ListFolder::Dirs);
    unsigned int index=0;
    while(index<fileList.size())
    {
        if(!CatchChallenger::FacilityLibGeneral::isFile(fileList.at(index).absoluteFilePath+"/definition.xml"))
        {
            index++;
            continue;
        }
        const uint16_t &questId=stringtouint16(fileList.at(index).name,&ok);
        if(ok)
        {
            //add it, all seam ok
            std::pair<bool,Quest> returnedQuest=loadSingleQuest(fileList.at(index).absoluteFilePath+"/definition.xml");
            if(returnedQuest.first==true)
            {
                returnedQuest.second.id=questId;
                if(quests.find(returnedQuest.second.id)!=quests.cend())
                    std::cerr << "The quest with id: " << returnedQuest.second.id << " is already found, disable: " << fileList.at(index).absoluteFilePath << "/definition.xml" << std::endl;
                else
                    quests[returnedQuest.second.id]=returnedQuest.second;
            }
        }
        else
            std::cerr << "Unable to open the folder: " << fileList.at(index).absoluteFilePath << ", because is folder name is not a number" << std::endl;
        index++;
    }
    return quests;
}

std::pair<bool,Quest> DatapackGeneralLoader::loadSingleQuest(const std::string &file)
{
    std::unordered_map<std::string,uint8_t> reputationNameToId;
    {
        uint8_t index=0;
        while(index<CommonDatapack::commonDatapack.reputation.size())
        {
            const Reputation &reputation=CommonDatapack::commonDatapack.reputation.at(index);
            reputationNameToId[reputation.name]=index;
            index++;
        }
    }
    CatchChallenger::Quest quest;
    quest.id=0;
    tinyxml2::XMLDocument *domDocument;
    #ifndef EPOLLCATCHCHALLENGERSERVER
    if(CommonDatapack::commonDatapack.xmlLoadedFile.find(file)!=CommonDatapack::commonDatapack.xmlLoadedFile.cend())
        domDocument=&CommonDatapack::commonDatapack.xmlLoadedFile[file];
    else
    {
        domDocument=&CommonDatapack::commonDatapack.xmlLoadedFile[file];
        #else
        domDocument=new tinyxml2::XMLDocument();
        #endif
        const auto loadOkay = domDocument->LoadFile(file.c_str());
        if(loadOkay!=0)
        {
            std::cerr << file+", "+tinyxml2errordoc(domDocument) << std::endl;
            return std::pair<bool,Quest>(false,quest);
        }
        #ifndef EPOLLCATCHCHALLENGERSERVER
    }
    #endif
    const tinyxml2::XMLElement * root = domDocument->RootElement();
    if(root==NULL)
    {
        std::cerr << "Unable to open the file: " << file << ", no root balise found for the xml file" << std::endl;
        return std::pair<bool,Quest>(false,quest);
    }
    if(strcmp(root->Name(),"quest")!=0)
    {
        std::cerr << "Unable to open the file: " << file << ", \"quest\" root balise not found for reputation of the xml file" << std::endl;
        return std::pair<bool,Quest>(false,quest);
    }

    //load the content
    bool ok;
    std::vector<uint16_t> defaultBots;
    quest.id=0;
    quest.repeatable=false;
    if(root->Attribute("repeatable")!=NULL)
        if(strcmp(root->Attribute("repeatable"),"yes")==0 ||
           strcmp(root->Attribute("repeatable"),"true")==0)
            quest.repeatable=true;
    if(root->Attribute("bot")!=NULL)
    {
        const std::vector<std::string> &tempStringList=stringsplit(root->Attribute("bot"),';');
        unsigned int index=0;
        while(index<tempStringList.size())
        {
            uint16_t tempInt=stringtouint16(tempStringList.at(index),&ok);
            if(ok)
                defaultBots.push_back(tempInt);
            index++;
        }
    }

    //load requirements
    const tinyxml2::XMLElement * requirements = root->FirstChildElement("requirements");
    while(requirements!=NULL)
    {
        //load requirements reputation
        {
            const tinyxml2::XMLElement * requirementsItem = requirements->FirstChildElement("reputation");
            while(requirementsItem!=NULL)
            {
                if(requirementsItem->Attribute("type")!=NULL && requirementsItem->Attribute("level")!=NULL)
                {
                    if(reputationNameToId.find(requirementsItem->Attribute("type"))!=reputationNameToId.cend())
                    {
                        std::string stringLevel=requirementsItem->Attribute("level");
                        bool positif=!stringStartWith(stringLevel,"-");
                        if(!positif)
                            stringLevel.erase(0,1);
                        uint8_t level=stringtouint8(stringLevel,&ok);
                        if(ok)
                        {
                            CatchChallenger::ReputationRequirements reputation;
                            reputation.level=level;
                            reputation.positif=positif;
                            reputation.reputationId=reputationNameToId.at(requirementsItem->Attribute("type"));
                            quest.requirements.reputation.push_back(reputation);
                        }
                        else
                            std::cerr << "Unable to open the file: " << file << ", reputation is not a number " << stringLevel << ": child->Name(): " << requirementsItem->Name() << std::endl;
                    }
                    else
                        std::cerr << "Has not the attribute: " << requirementsItem->Attribute("type")
                                  << ", reputation not found: child->Name(): " << requirementsItem->Name() << std::endl;
                }
                else
                    std::cerr << "Has not the attribute: type level, have not attribute type or level: child->Name(): " << requirementsItem->Name() << std::endl;
                requirementsItem = requirementsItem->NextSiblingElement("reputation");
            }
        }
        //load requirements quest
        {
            const tinyxml2::XMLElement * requirementsItem = requirements->FirstChildElement("quest");
            while(requirementsItem!=NULL)
            {
                if(requirementsItem->Attribute("id")!=NULL)
                {
                    const uint16_t &questId=stringtouint16(requirementsItem->Attribute("id"),&ok);
                    if(ok)
                    {
                        QuestRequirements questNewEntry;
                        questNewEntry.quest=questId;
                        questNewEntry.inverse=false;
                        if(requirementsItem->Attribute("inverse")!=NULL)
                            if(strcmp(requirementsItem->Attribute("inverse"),"true")==0)
                                questNewEntry.inverse=true;
                        quest.requirements.quests.push_back(questNewEntry);
                    }
                    else
                        std::cerr << "Unable to open the file: " << file << ", requirement quest item id is not a number " <<
                                     requirementsItem->Attribute("id") << ": child->Name(): " << requirementsItem->Name() << std::endl;
                }
                else
                    std::cerr << "Has attribute: %1, requirement quest item have not id attribute: child->Name(): " << requirementsItem->Name() << std::endl;
                requirementsItem = requirementsItem->NextSiblingElement("quest");
            }
        }
        requirements = requirements->NextSiblingElement("requirements");
    }

    //load rewards
    const tinyxml2::XMLElement * rewards = root->FirstChildElement("rewards");
    while(rewards!=NULL)
    {
        //load rewards reputation
        {
            const tinyxml2::XMLElement * reputationItem = rewards->FirstChildElement("reputation");
            while(reputationItem!=NULL)
            {
                if(reputationItem->Attribute("type")!=NULL && reputationItem->Attribute("point")!=NULL)
                {
                    if(reputationNameToId.find(reputationItem->Attribute("type"))!=reputationNameToId.cend())
                    {
                        const int32_t &point=stringtoint32(reputationItem->Attribute("point"),&ok);
                        if(ok)
                        {
                            CatchChallenger::ReputationRewards reputation;
                            reputation.reputationId=reputationNameToId.at(reputationItem->Attribute("type"));
                            reputation.point=point;
                            quest.rewards.reputation.push_back(reputation);
                        }
                        else
                            std::cerr << "Unable to open the file: " << file << ", quest rewards point is not a number: child->Name(): " << reputationItem->Name() << std::endl;
                    }
                    else
                        std::cerr << "Unable to open the file: " << file << ", quest rewards point is not a number: child->Name(): " << reputationItem->Name() << std::endl;
                }
                else
                    std::cerr << "Has not the attribute: type/point, quest rewards point have not type or point attribute: child->Name(): " << reputationItem->Name() << std::endl;
                reputationItem = reputationItem->NextSiblingElement("reputation");
            }
        }
        //load rewards item
        {
            const tinyxml2::XMLElement * rewardsItem = rewards->FirstChildElement("item");
            while(rewardsItem!=NULL)
            {
                if(rewardsItem->Attribute("id")!=NULL)
                {
                    CatchChallenger::Quest::Item item;
                    item.item=stringtouint16(rewardsItem->Attribute("id"),&ok);
                    item.quantity=1;
                    if(ok)
                    {
                        if(CommonDatapack::commonDatapack.items.item.find(item.item)==CommonDatapack::commonDatapack.items.item.cend())
                        {
                            std::cerr << "Unable to open the file: " << file << ", rewards item id is not into the item list: "
                                      << rewardsItem->Attribute("id") << ": child->Name(): " << rewardsItem->Name() << std::endl;
                            return std::pair<bool,Quest>(false,quest);
                        }
                        if(rewardsItem->Attribute("quantity")!=NULL)
                        {
                            item.quantity=stringtouint32(rewardsItem->Attribute("quantity"),&ok);
                            if(!ok)
                                item.quantity=1;
                        }
                        quest.rewards.items.push_back(item);
                    }
                    else
                        std::cerr << "Unable to open the file: " << file << ", rewards item id is not a number: "
                                  << rewardsItem->Attribute("id") << ": child->Name(): " << rewardsItem->Name() << std::endl;
                }
                else
                    std::cerr << "Has not the attribute: id, rewards item have not attribute id: child->Name(): " << rewardsItem->Name() << std::endl;
                rewardsItem = rewardsItem->NextSiblingElement("item");
            }
        }
        //load rewards allow
        {
            const tinyxml2::XMLElement * allowItem = rewards->FirstChildElement("allow");
            while(allowItem!=NULL)
            {
                if(allowItem->Attribute("type")!=NULL)
                {
                    if(strcmp(allowItem->Attribute("type"),"clan")==0)
                        quest.rewards.allow.push_back(CatchChallenger::ActionAllow_Clan);
                    else
                        std::cerr << "Unable to open the file: " << file << ", allow type not understand " <<
                                     allowItem->Attribute("id") << ": child->Name(): " << allowItem->Name() << std::endl;
                }
                else
                    std::cerr << "Has attribute: %1, rewards item have not attribute id: child->Name(): " << allowItem->Name() << std::endl;
                allowItem = allowItem->NextSiblingElement("allow");
            }
        }
        rewards = rewards->NextSiblingElement("rewards");
    }

    std::unordered_map<uint8_t,CatchChallenger::Quest::Step> steps;
    //load step
    const tinyxml2::XMLElement * step = root->FirstChildElement("step");
    while(step!=NULL)
    {
        if(step->Attribute("id")!=NULL)
        {
            const uint8_t &id=stringtouint8(step->Attribute("id"),&ok);
            if(ok)
            {
                CatchChallenger::Quest::Step stepObject;
                if(step->Attribute("bot")!=NULL)
                {
                    const std::vector<std::string> &tempStringList=stringsplit(step->Attribute("bot"),';');
                    unsigned int index=0;
                    while(index<tempStringList.size())
                    {
                        const uint16_t &tempInt=stringtouint16(tempStringList.at(index),&ok);
                        if(ok)
                            stepObject.bots.push_back(tempInt);
                        index++;
                    }
                }
                else
                    stepObject.bots=defaultBots;
                //do the item
                {
                    const tinyxml2::XMLElement * stepItem = step->FirstChildElement("item");
                    while(stepItem!=NULL)
                    {
                        if(stepItem->Attribute("id")!=NULL)
                        {
                            CatchChallenger::Quest::Item item;
                            item.item=stringtouint16(stepItem->Attribute("id"),&ok);
                            item.quantity=1;
                            if(ok)
                            {
                                if(CommonDatapack::commonDatapack.items.item.find(item.item)==CommonDatapack::commonDatapack.items.item.cend())
                                {
                                    std::cerr << "Unable to open the file: " << file << ", rewards item id is not into the item list: "
                                              << stepItem->Attribute("id") << ": child->Name(): " << stepItem->Name() << std::endl;
                                    return std::pair<bool,Quest>(false,quest);
                                }
                                if(stepItem->Attribute("quantity")!=NULL)
                                {
                                    item.quantity=stringtouint32(stepItem->Attribute("quantity"),&ok);
                                    if(!ok)
                                        item.quantity=1;
                                }
                                stepObject.requirements.items.push_back(item);
                                if(stepItem->Attribute("monster")!=NULL && stepItem->Attribute("rate")!=NULL)
                                {
                                    CatchChallenger::Quest::ItemMonster itemMonster;
                                    itemMonster.item=item.item;

                                    const std::vector<std::string> &tempStringList=stringsplit(stepItem->Attribute("monster"),';');
                                    unsigned int index=0;
                                    while(index<tempStringList.size())
                                    {
                                        const uint16_t &tempInt=stringtouint16(tempStringList.at(index),&ok);
                                        if(ok)
                                            itemMonster.monsters.push_back(tempInt);
                                        index++;
                                    }

                                    std::string rateString=stepItem->Attribute("rate");
                                    stringreplaceOne(rateString,"%","");
                                    itemMonster.rate=stringtouint8(rateString,&ok);
                                    if(ok)
                                        stepObject.itemsMonster.push_back(itemMonster);
                                }
                            }
                            else
                                std::cerr << "Unable to open the file: " << file << ", step id is not a number " << stepItem->Attribute("id") << ": child->Name(): " << step->Name() << std::endl;
                        }
                        else
                            std::cerr << "Has not the attribute: id, step have not id attribute: child->Name(): " << step->Name() << std::endl;
                        stepItem = stepItem->NextSiblingElement("item");
                    }
                }
                //do the fight
                {
                    const tinyxml2::XMLElement * fightItem = step->FirstChildElement("fight");
                    while(fightItem!=NULL)
                    {
                        if(fightItem->Attribute("id")!=NULL)
                        {
                            const uint16_t &fightId=stringtouint16(fightItem->Attribute("id"),&ok);
                            if(ok)
                                stepObject.requirements.fightId.push_back(fightId);
                            else
                                std::cerr << "Unable to open the file: " << file << ", step id is not a number "
                                          << fightItem->Attribute("id") << ": child->Name(): " << fightItem->Name() << std::endl;
                        }
                        else
                            std::cerr << "Has attribute: %1, step have not id attribute: child->Name(): " << fightItem->Name() << std::endl;
                        fightItem = fightItem->NextSiblingElement("fight");
                    }
                }
                steps[id]=stepObject;
            }
            else
                std::cerr << "Unable to open the file: " << file << ", step id is not a number: child->Name(): " << step->Name() << std::endl;
        }
        else
            std::cerr << "Has attribute: %1, step have not id attribute: child->Name(): " << step->Name() << std::endl;
        step = step->NextSiblingElement("step");
    }

    #ifdef EPOLLCATCHCHALLENGERSERVER
    delete domDocument;
    #endif

    //sort the step
    uint8_t indexLoop=1;
    while(indexLoop<(steps.size()+1))
    {
        if(steps.find(indexLoop)==steps.cend())
            break;
        quest.steps.push_back(steps.at(indexLoop));
        indexLoop++;
    }
    if(indexLoop<(steps.size()+1))
        return std::pair<bool,Quest>(false,quest);
    return std::pair<bool,Quest>(true,quest);
}

std::unordered_map<uint8_t, Plant> DatapackGeneralLoader::loadPlants(const std::string &file)
{
    std::unordered_map<std::string,uint8_t> reputationNameToId;
    {
        uint8_t index=0;
        while(index<CommonDatapack::commonDatapack.reputation.size())
        {
            reputationNameToId[CommonDatapack::commonDatapack.reputation.at(index).name]=index;
            index++;
        }
    }
    std::unordered_map<uint8_t, Plant> plants;
    tinyxml2::XMLDocument *domDocument;
    //open and quick check the file
    #ifndef EPOLLCATCHCHALLENGERSERVER
    if(CommonDatapack::commonDatapack.xmlLoadedFile.find(file)!=CommonDatapack::commonDatapack.xmlLoadedFile.cend())
        domDocument=&CommonDatapack::commonDatapack.xmlLoadedFile[file];
    else
    {
        domDocument=&CommonDatapack::commonDatapack.xmlLoadedFile[file];
        #else
        domDocument=new tinyxml2::XMLDocument();
        #endif
        const auto loadOkay = domDocument->LoadFile(file.c_str());
        if(loadOkay!=0)
        {
            std::cerr << file+", "+tinyxml2errordoc(domDocument) << std::endl;
            return plants;
        }
        #ifndef EPOLLCATCHCHALLENGERSERVER
    }
    #endif
    const tinyxml2::XMLElement * root = domDocument->RootElement();
    if(root==NULL)
    {
        std::cerr << "Unable to open the file: " << file << ", no root balise found for the xml file" << std::endl;
        return plants;
    }
    if(strcmp(root->Name(),"plants")!=0)
    {
        std::cerr << "Unable to open the file: " << file << ", \"plants\" root balise not found for reputation of the xml file" << std::endl;
        return plants;
    }

    //load the content
    bool ok,ok2;
    const tinyxml2::XMLElement * plantItem = root->FirstChildElement("plant");
    while(plantItem!=NULL)
    {
        if(plantItem->Attribute("id")!=NULL && plantItem->Attribute("itemUsed")!=NULL)
        {
            const uint8_t &id=stringtouint8(plantItem->Attribute("id"),&ok);
            const uint16_t &itemUsed=stringtouint16(plantItem->Attribute("itemUsed"),&ok2);
            if(ok && ok2)
            {
                if(plants.find(id)==plants.cend())
                {
                    Plant plant;
                    plant.fruits_seconds=0;
                    plant.sprouted_seconds=0;
                    plant.taller_seconds=0;
                    plant.flowering_seconds=0;
                    plant.itemUsed=itemUsed;
                    {
                        const tinyxml2::XMLElement * requirementsItem = plantItem->FirstChildElement("requirements");
                        if(requirementsItem!=NULL)
                        {
                            const tinyxml2::XMLElement * reputationItem = requirementsItem->FirstChildElement("reputation");
                            while(reputationItem!=NULL)
                            {
                                if(reputationItem->Attribute("type")!=NULL && reputationItem->Attribute("level")!=NULL)
                                {
                                    if(reputationNameToId.find(reputationItem->Attribute("type"))!=reputationNameToId.cend())
                                    {
                                        ReputationRequirements reputationRequirements;
                                        std::string stringLevel=reputationItem->Attribute("level");
                                        reputationRequirements.positif=!stringStartWith(stringLevel,"-");
                                        if(!reputationRequirements.positif)
                                            stringLevel.erase(0,1);
                                        reputationRequirements.level=stringtouint8(stringLevel,&ok);
                                        if(ok)
                                        {
                                            reputationRequirements.reputationId=reputationNameToId.at(reputationItem->Attribute("type"));
                                            plant.requirements.reputation.push_back(reputationRequirements);
                                        }
                                    }
                                    else
                                        std::cerr << "Reputation type not found: " << reputationItem->Attribute("type") << ", have not the id, child->Name(): " << reputationItem->Name() << std::endl;
                                }
                                else
                                    std::cerr << "Unable to open the industries link file: " << file << ", have not the id, child->Name(): " << reputationItem->Name() << std::endl;
                                reputationItem = reputationItem->NextSiblingElement("reputation");
                            }
                        }
                    }
                    {
                        const tinyxml2::XMLElement * rewardsItem = plantItem->FirstChildElement("rewards");
                        if(rewardsItem!=NULL)
                        {
                            const tinyxml2::XMLElement * reputationItem = rewardsItem->FirstChildElement("reputation");
                            while(reputationItem!=NULL)
                            {
                                if(reputationItem->Attribute("type")!=NULL && reputationItem->Attribute("point")!=NULL)
                                {
                                    if(reputationNameToId.find(reputationItem->Attribute("type"))!=reputationNameToId.cend())
                                    {
                                        ReputationRewards reputationRewards;
                                        reputationRewards.point=stringtoint32(reputationItem->Attribute("point"),&ok);
                                        if(ok)
                                        {
                                            reputationRewards.reputationId=reputationNameToId.at(reputationItem->Attribute("type"));
                                            plant.rewards.reputation.push_back(reputationRewards);
                                        }
                                    }
                                }
                                else
                                    std::cerr << "Unable to open the industries link file: " << file << ", have not the id, child->Name(): " << reputationItem->Name() << std::endl;
                                reputationItem = reputationItem->NextSiblingElement("reputation");
                            }
                        }
                    }
                    ok=false;
                    const tinyxml2::XMLElement * quantity = plantItem->FirstChildElement("quantity");
                    if(quantity!=NULL)
                    {
                        if(quantity->GetText()!=NULL)
                        {
                            const float &float_quantity=stringtofloat(quantity->GetText(),&ok2);
                            const int &integer_part=float_quantity;
                            float random_part=float_quantity-integer_part;
                            random_part*=RANDOM_FLOAT_PART_DIVIDER;
                            plant.fix_quantity=static_cast<uint16_t>(integer_part);
                            plant.random_quantity=random_part;
                            ok=ok2;
                        }
                    }
                    int intermediateTimeCount=0;
                    const tinyxml2::XMLElement * grow = plantItem->FirstChildElement("grow");
                    if(grow!=NULL)
                    {
                        const tinyxml2::XMLElement * fruits = grow->FirstChildElement("fruits");
                        if(fruits!=NULL)
                        {
                            if(fruits->GetText()!=NULL)
                            {
                                plant.fruits_seconds=stringtouint32(fruits->GetText(),&ok2)*60;
                                plant.sprouted_seconds=static_cast<uint16_t>(plant.fruits_seconds);
                                plant.taller_seconds=static_cast<uint16_t>(plant.fruits_seconds);
                                plant.flowering_seconds=static_cast<uint16_t>(plant.fruits_seconds);
                                if(!ok2)
                                {
                                    std::cerr << "Unable to parse the plants file: " << file << ", sprouted is not a number: " << fruits->GetText() << " child->Name(): " << fruits->Name() << std::endl;
                                    ok=false;
                                }
                            }
                            else
                            {
                                ok=false;
                                std::cerr << "Unable to parse the plants file: " << file << ", fruits is not an element: child->Name(): " << fruits->Name() << std::endl;
                            }
                        }
                        else
                        {
                            ok=false;
                            std::cerr << "Unable to parse the plants file: " << file << ", fruits is null: child->Name(): " << grow->Name() << std::endl;
                        }
                        const tinyxml2::XMLElement * sprouted = grow->FirstChildElement("sprouted");
                        if(sprouted!=NULL)
                        {
                            if(sprouted->GetText()!=NULL)
                            {
                                plant.sprouted_seconds=stringtouint16(sprouted->GetText(),&ok2)*60;
                                if(!ok2)
                                {
                                    std::cerr << "Unable to parse the plants file: " << file << ", sprouted is not a number: " << sprouted->GetText() << " child->Name(): " << sprouted->Name() << std::endl;
                                    ok=false;
                                }
                                else
                                    intermediateTimeCount++;
                            }
                            else
                                std::cerr << "Unable to parse the plants file: " << file << ", sprouted is not an element: child->Name(): " << sprouted->Name() << std::endl;
                        }
                        const tinyxml2::XMLElement * taller = grow->FirstChildElement("taller");
                        if(taller!=NULL)
                        {
                            if(taller->GetText()!=NULL)
                            {
                                plant.taller_seconds=stringtouint16(taller->GetText(),&ok2)*60;
                                if(!ok2)
                                {
                                    std::cerr << "Unable to parse the plants file: " << file << ", sprouted is not a number: " << taller->GetText() << " child->Name(): " << taller->Name() << std::endl;
                                    ok=false;
                                }
                                else
                                    intermediateTimeCount++;
                            }
                            else
                                std::cerr << "Unable to parse the plants file: " << file << ", taller is not an element: child->Name(): " << taller->Name() << std::endl;
                        }
                        const tinyxml2::XMLElement * flowering = grow->FirstChildElement("flowering");
                        if(flowering!=NULL)
                        {
                            if(flowering->GetText()!=NULL)
                            {
                                plant.flowering_seconds=stringtouint16(flowering->GetText(),&ok2)*60;
                                if(!ok2)
                                {
                                    ok=false;
                                    std::cerr << "Unable to parse the plants file: " << file << ", sprouted is not a number: " << flowering->GetText() << " child->Name(): " << flowering->Name() << std::endl;
                                }
                                else
                                    intermediateTimeCount++;
                            }
                            else
                                std::cerr << "Unable to parse the plants file: " << file << ", flowering is not an element: child->Name(): " << flowering->Name() << std::endl;
                        }
                    }
                    else
                        std::cerr << "Unable to parse the plants file: " << file << ", grow is null: child->Name(): child->Name(): " << plantItem->Name() << std::endl;
                    if(ok)
                    {
                        bool needIntermediateTimeFix=false;
                        if(plant.flowering_seconds>=plant.fruits_seconds)
                        {
                            needIntermediateTimeFix=true;
                            if(intermediateTimeCount>=3)
                                std::cerr << "Warning when parse the plants file: " << file << ", flowering_seconds>=fruits_seconds: child->Name(): child->Name(): " << grow->Name() << std::endl;
                        }
                        if(plant.taller_seconds>=plant.flowering_seconds)
                        {
                            needIntermediateTimeFix=true;
                            if(intermediateTimeCount>=3)
                                std::cerr << "Warning when parse the plants file: " << file << ", taller_seconds>=flowering_seconds: child->Name(): " << grow->Name() << std::endl;
                        }
                        if(plant.sprouted_seconds>=plant.taller_seconds)
                        {
                            needIntermediateTimeFix=true;
                            if(intermediateTimeCount>=3)
                                std::cerr << "Warning when parse the plants file: " << file << ", sprouted_seconds>=taller_seconds: child->Name(): " << grow->Name() << std::endl;
                        }
                        if(plant.sprouted_seconds<=0)
                        {
                            needIntermediateTimeFix=true;
                            if(intermediateTimeCount>=3)
                                std::cerr << "Warning when parse the plants file: " << file << ", sprouted_seconds<=0: child->Name(): " << grow->Name() << std::endl;
                        }
                        if(needIntermediateTimeFix)
                        {
                            plant.flowering_seconds=static_cast<uint16_t>(plant.fruits_seconds*3/4);
                            plant.taller_seconds=static_cast<uint16_t>(plant.fruits_seconds*2/4);
                            plant.sprouted_seconds=static_cast<uint16_t>(plant.fruits_seconds*1/4);
                        }
                        plants[id]=plant;
                    }
                }
                else
                    std::cerr << "Unable to open the plants file: " << file << ", id number already set: child->Name(): " << plantItem->Name() << std::endl;
            }
            else
                std::cerr << "Unable to open the plants file: " << file << ", id is not a number: child->Name(): " << plantItem->Name() << std::endl;
        }
        else
            std::cerr << "Unable to open the plants file: " << file << ", have not the plant id: child->Name(): " << plantItem->Name() << std::endl;
        plantItem = plantItem->NextSiblingElement("plant");
    }
    #ifdef EPOLLCATCHCHALLENGERSERVER
    delete domDocument;
    #endif
    return plants;
}

std::pair<std::unordered_map<uint16_t,CrafingRecipe>,std::unordered_map<uint16_t,uint16_t> > DatapackGeneralLoader::loadCraftingRecipes(const std::string &file,const std::unordered_map<uint16_t, Item> &items,uint16_t &crafingRecipesMaxId)
{
    std::unordered_map<std::string,uint8_t> reputationNameToId;
    {
        uint8_t index=0;
        while(index<CommonDatapack::commonDatapack.reputation.size())
        {
            reputationNameToId[CommonDatapack::commonDatapack.reputation.at(index).name]=index;
            index++;
        }
    }
    std::unordered_map<uint16_t,CrafingRecipe> crafingRecipes;
    std::unordered_map<uint16_t,uint16_t> itemToCrafingRecipes;
    tinyxml2::XMLDocument *domDocument;
    #ifndef EPOLLCATCHCHALLENGERSERVER
    //open and quick check the file
    if(CommonDatapack::commonDatapack.xmlLoadedFile.find(file)!=CommonDatapack::commonDatapack.xmlLoadedFile.cend())
        domDocument=&CommonDatapack::commonDatapack.xmlLoadedFile[file];
    else
    {
        domDocument=&CommonDatapack::commonDatapack.xmlLoadedFile[file];
        #else
        domDocument=new tinyxml2::XMLDocument();
        #endif
        const auto loadOkay = domDocument->LoadFile(file.c_str());
        if(loadOkay!=0)
        {
            std::cerr << file+", "+tinyxml2errordoc(domDocument) << std::endl;
            return std::pair<std::unordered_map<uint16_t,CrafingRecipe>,std::unordered_map<uint16_t,uint16_t> >(crafingRecipes,itemToCrafingRecipes);
        }
        #ifndef EPOLLCATCHCHALLENGERSERVER
    }
    #endif
    const tinyxml2::XMLElement * root = domDocument->RootElement();
    if(root==NULL)
    {
        std::cerr << "Unable to open the file: " << file << ", no root balise found for the xml file" << std::endl;
        return std::pair<std::unordered_map<uint16_t,CrafingRecipe>,std::unordered_map<uint16_t,uint16_t> >(crafingRecipes,itemToCrafingRecipes);
    }
    if(strcmp(root->Name(),"recipes")!=0)
    {
        std::cerr << "Unable to open the file: " << file << ", \"recipes\" root balise not found for reputation of the xml file" << std::endl;
        return std::pair<std::unordered_map<uint16_t,CrafingRecipe>,std::unordered_map<uint16_t,uint16_t> >(crafingRecipes,itemToCrafingRecipes);
    }

    //load the content
    bool ok,ok2,ok3;
    const tinyxml2::XMLElement * recipeItem = root->FirstChildElement("recipe");
    while(recipeItem!=NULL)
    {
        if(recipeItem->Attribute("id")!=NULL && recipeItem->Attribute("itemToLearn")!=NULL && recipeItem->Attribute("doItemId")!=NULL)
        {
            uint8_t success=100;
            if(recipeItem->Attribute("success")!=NULL)
            {
                const uint8_t &tempShort=stringtouint8(recipeItem->Attribute("success"),&ok);
                if(ok)
                {
                    if(tempShort>100)
                        std::cerr << "preload_crafting_recipes() success can't be greater than 100 for crafting recipe file: " << file << ", child->Name(): " << recipeItem->Name() << std::endl;
                    else
                        success=tempShort;
                }
                else
                    std::cerr << "preload_crafting_recipes() success in not an number for crafting recipe file: " << file << ", child->Name(): " << recipeItem->Name() << std::endl;
            }
            uint16_t quantity=1;
            if(recipeItem->Attribute("quantity")!=NULL)
            {
                const uint32_t &tempShort=stringtouint32(recipeItem->Attribute("quantity"),&ok);
                if(ok)
                {
                    if(tempShort>65535)
                        std::cerr << "preload_crafting_recipes() quantity can't be greater than 65535 for crafting recipe file: " << file << ", child->Name(): " << recipeItem->Name() << std::endl;
                    else
                        quantity=static_cast<uint16_t>(tempShort);
                }
                else
                    std::cerr << "preload_crafting_recipes() quantity in not an number for crafting recipe file: " << file << ", child->Name(): " << recipeItem->Name() << std::endl;
            }

            const uint16_t &id=stringtouint16(recipeItem->Attribute("id"),&ok);
            const uint16_t &itemToLearn=stringtouint16(recipeItem->Attribute("itemToLearn"),&ok2);
            const uint16_t &doItemId=stringtouint16(recipeItem->Attribute("doItemId"),&ok3);
            if(ok && ok2 && ok3)
            {
                if(crafingRecipes.find(id)==crafingRecipes.cend())
                {
                    ok=true;
                    CatchChallenger::CrafingRecipe recipe;
                    recipe.doItemId=doItemId;
                    recipe.itemToLearn=itemToLearn;
                    recipe.quantity=quantity;
                    recipe.success=success;
                    {
                        const tinyxml2::XMLElement * requirementsItem = recipeItem->FirstChildElement("requirements");
                        if(requirementsItem!=NULL)
                        {
                            const tinyxml2::XMLElement * reputationItem = requirementsItem->FirstChildElement("reputation");
                            while(reputationItem!=NULL)
                            {
                                if(reputationItem->Attribute("type")!=NULL && reputationItem->Attribute("level")!=NULL)
                                {
                                    if(reputationNameToId.find(reputationItem->Attribute("type"))!=reputationNameToId.cend())
                                    {
                                        ReputationRequirements reputationRequirements;
                                        std::string stringLevel=reputationItem->Attribute("level");
                                        reputationRequirements.positif=!stringStartWith(stringLevel,"-");
                                        if(!reputationRequirements.positif)
                                            stringLevel.erase(0,1);
                                        reputationRequirements.level=stringtouint8(stringLevel,&ok);
                                        if(ok)
                                        {
                                            reputationRequirements.reputationId=reputationNameToId.at(reputationItem->Attribute("type"));
                                            recipe.requirements.reputation.push_back(reputationRequirements);
                                        }
                                    }
                                    else
                                        std::cerr << "Reputation type not found: " << reputationItem->Attribute("type") << ", have not the id, child->Name(): " << reputationItem->Name() << std::endl;
                                }
                                else
                                    std::cerr << "Unable to open the industries link file: " << file << ", have not the id, child->Name(): " << reputationItem->Name() << std::endl;
                                reputationItem = reputationItem->NextSiblingElement("reputation");
                            }
                        }
                    }
                    {
                        const tinyxml2::XMLElement * rewardsItem = recipeItem->FirstChildElement("rewards");
                        if(rewardsItem!=NULL)
                        {
                            const tinyxml2::XMLElement * reputationItem = rewardsItem->FirstChildElement("reputation");
                            while(reputationItem!=NULL)
                            {
                                if(reputationItem->Attribute("type")!=NULL && reputationItem->Attribute("point")!=NULL)
                                {
                                    if(reputationNameToId.find(reputationItem->Attribute("type"))!=reputationNameToId.cend())
                                    {
                                        ReputationRewards reputationRewards;
                                        reputationRewards.point=stringtoint32(reputationItem->Attribute("point"),&ok);
                                        if(ok)
                                        {
                                            reputationRewards.reputationId=reputationNameToId.at(reputationItem->Attribute("type"));
                                            recipe.rewards.reputation.push_back(reputationRewards);
                                        }
                                    }
                                }
                                else
                                    std::cerr << "Unable to open the industries link file: " << file << ", have not the id, child->Name(): " << reputationItem->Name() << std::endl;
                                reputationItem = reputationItem->NextSiblingElement("reputation");
                            }
                        }
                    }
                    const tinyxml2::XMLElement * material = recipeItem->FirstChildElement("material");
                    while(material!=NULL && ok)
                    {
                        if(material->Attribute("itemId")!=NULL)
                        {
                            const uint16_t &itemId=stringtouint16(material->Attribute("itemId"),&ok2);
                            if(!ok2)
                            {
                                ok=false;
                                std::cerr << "preload_crafting_recipes() material attribute itemId is not a number for crafting recipe file: " << file << ": child->Name(): " << material->Name() << std::endl;
                                break;
                            }
                            uint16_t quantity=1;
                            if(material->Attribute("quantity")!=NULL)
                            {
                                const uint32_t &tempShort=stringtouint32(material->Attribute("quantity"),&ok2);
                                if(ok2)
                                {
                                    if(tempShort>65535)
                                    {
                                        ok=false;
                                        std::cerr << "preload_crafting_recipes() material quantity can't be greater than 65535 for crafting recipe file: " << file << ": child->Name(): " << material->Name() << std::endl;
                                        break;
                                    }
                                    else
                                        quantity=static_cast<uint16_t>(tempShort);
                                }
                                else
                                {
                                    ok=false;
                                    std::cerr << "preload_crafting_recipes() material quantity in not an number for crafting recipe file: " << file << ": child->Name(): " << material->Name() << std::endl;
                                    break;
                                }
                            }
                            if(items.find(itemId)==items.cend())
                            {
                                ok=false;
                                std::cerr << "preload_crafting_recipes() material itemId in not into items list for crafting recipe file: " << file << ": child->Name(): " << material->Name() << std::endl;
                                break;
                            }
                            CatchChallenger::CrafingRecipe::Material newMaterial;
                            newMaterial.item=itemId;
                            newMaterial.quantity=quantity;
                            unsigned int index=0;
                            while(index<recipe.materials.size())
                            {
                                if(recipe.materials.at(index).item==newMaterial.item)
                                    break;
                                index++;
                            }
                            if(index<recipe.materials.size())
                            {
                                ok=false;
                                std::cerr << "id of item already into resource or product list: %1: child->Name(): " << material->Name() << std::endl;
                            }
                            else
                            {
                                if(recipe.doItemId==newMaterial.item)
                                {
                                    std::cerr << "id of item already into resource or product list: %1: child->Name(): " << material->Name() << std::endl;
                                    ok=false;
                                }
                                else
                                    recipe.materials.push_back(newMaterial);
                            }
                        }
                        else
                            std::cerr << "preload_crafting_recipes() material have not attribute itemId for crafting recipe file: " << file << ": child->Name(): " << material->Name() << std::endl;
                        material = material->NextSiblingElement("material");
                    }
                    if(ok)
                    {
                        if(items.find(recipe.itemToLearn)==items.cend())
                        {
                            ok=false;
                            std::cerr << "preload_crafting_recipes() itemToLearn is not into items list for crafting recipe file: " << file << ": child->Name(): " << recipeItem->Name() << std::endl;
                        }
                    }
                    if(ok)
                    {
                        if(items.find(recipe.doItemId)==items.cend())
                        {
                            ok=false;
                            std::cerr << "preload_crafting_recipes() doItemId is not into items list for crafting recipe file: " << file << ": child->Name(): " << recipeItem->Name() << std::endl;
                        }
                    }
                    if(ok)
                    {
                        if(itemToCrafingRecipes.find(recipe.itemToLearn)!=itemToCrafingRecipes.cend())
                        {
                            ok=false;
                            std::cerr << "preload_crafting_recipes() itemToLearn already used to learn another recipe: " << itemToCrafingRecipes.at(recipe.doItemId) << ": file: " << file << " child->Name(): " << recipeItem->Name() << std::endl;
                        }
                    }
                    if(ok)
                    {
                        if(recipe.itemToLearn==recipe.doItemId)
                        {
                            ok=false;
                            std::cerr << "preload_crafting_recipes() the product of the recipe can't be them self: " << id << ": file: " << file << ": child->Name(): " << recipeItem->Name() << std::endl;
                        }
                    }
                    if(ok)
                    {
                        if(itemToCrafingRecipes.find(recipe.doItemId)!=itemToCrafingRecipes.cend())
                        {
                            ok=false;
                            std::cerr << "preload_crafting_recipes() the product of the recipe can't be a recipe: " << itemToCrafingRecipes.at(recipe.doItemId) << ": file: " << file << ": child->Name(): " << recipeItem->Name() << std::endl;
                        }
                    }
                    if(ok)
                    {
                        if(crafingRecipesMaxId<id)
                            crafingRecipesMaxId=id;
                        crafingRecipes[id]=recipe;
                        itemToCrafingRecipes[recipe.itemToLearn]=id;
                    }
                }
                else
                    std::cerr << "Unable to open the crafting recipe file: " << file << ", id number already set: child->Name(): " << recipeItem->Name() << std::endl;
            }
            else
                std::cerr << "Unable to open the crafting recipe file: " << file << ", id is not a number: child->Name(): " << recipeItem->Name() << std::endl;
        }
        else
            std::cerr << "Unable to open the crafting recipe file: " << file << ", have not the crafting recipe id: child->Name(): " << recipeItem->Name() << std::endl;
        recipeItem = recipeItem->NextSiblingElement("recipe");
    }
    #ifdef EPOLLCATCHCHALLENGERSERVER
    delete domDocument;
    #endif
    return std::pair<std::unordered_map<uint16_t,CrafingRecipe>,std::unordered_map<uint16_t,uint16_t> >(crafingRecipes,itemToCrafingRecipes);
}

std::unordered_map<uint16_t,Industry> DatapackGeneralLoader::loadIndustries(const std::string &folder,const std::unordered_map<uint16_t, Item> &items)
{
    std::unordered_map<uint16_t,Industry> industries;
    const std::vector<FacilityLibGeneral::InodeDescriptor> &fileList=CatchChallenger::FacilityLibGeneral::listFolderNotRecursive(folder,CatchChallenger::FacilityLibGeneral::ListFolder::Files);
    unsigned int file_index=0;
    while(file_index<fileList.size())
    {
        const std::string &file=fileList.at(file_index).absoluteFilePath;
        tinyxml2::XMLDocument *domDocument;
        //open and quick check the file
        #ifndef EPOLLCATCHCHALLENGERSERVER
        if(CommonDatapack::commonDatapack.xmlLoadedFile.find(file)!=CommonDatapack::commonDatapack.xmlLoadedFile.cend())
            domDocument=&CommonDatapack::commonDatapack.xmlLoadedFile[file];
        else
        {
            domDocument=&CommonDatapack::commonDatapack.xmlLoadedFile[file];
            #else
            domDocument=new tinyxml2::XMLDocument();
            #endif
            const auto loadOkay = domDocument->LoadFile(file.c_str());
            if(loadOkay!=0)
            {
                std::cerr << file+", "+tinyxml2errordoc(domDocument) << std::endl;
                file_index++;
                continue;
            }
            #ifndef EPOLLCATCHCHALLENGERSERVER
        }
        #endif
        const tinyxml2::XMLElement * root = domDocument->RootElement();
        if(root==NULL)
        {
            std::cerr << "Unable to open the file: " << file << ", no root balise found for the xml file" << std::endl;
            file_index++;
            continue;
        }
        if(strcmp(root->Name(),"industries")!=0)
        {
            std::cerr << "Unable to open the file: " << file << ", \"industries\" root balise not found for reputation of the xml file" << std::endl;
            file_index++;
            continue;
        }

        //load the content
        bool ok,ok2,ok3;
        const tinyxml2::XMLElement * industryItem = root->FirstChildElement("industrialrecipe");
        while(industryItem!=NULL)
        {
            if(industryItem->Attribute("id")!=NULL && industryItem->Attribute("time")!=NULL && industryItem->Attribute("cycletobefull")!=NULL)
            {
                Industry industry;
                const uint16_t &id=stringtouint16(industryItem->Attribute("id"),&ok);
                industry.time=stringtouint32(industryItem->Attribute("time"),&ok2);
                industry.cycletobefull=stringtouint32(industryItem->Attribute("cycletobefull"),&ok3);
                if(ok && ok2 && ok3)
                {
                    if(industries.find(id)==industries.cend())
                    {
                        if(industry.time<60*5)
                        {
                            std::cerr << "the time need be greater than 5*60 seconds to not slow down the server: " << industry.time << ", " << file << ": child->Name(): " << industryItem->Name() << std::endl;
                            industry.time=60*5;
                        }
                        if(industry.cycletobefull<1)
                        {
                            std::cerr << "cycletobefull need be greater than 0: child->Name(): " << industryItem->Name() << std::endl;
                            industry.cycletobefull=1;
                        }
                        else if(industry.cycletobefull>65535)
                        {
                            std::cerr << "cycletobefull need be lower to 10 to not slow down the server, use the quantity: child->Name(): " << industryItem->Name() << std::endl;
                            industry.cycletobefull=10;
                        }
                        //resource
                        {
                            const tinyxml2::XMLElement * resourceItem = industryItem->FirstChildElement("resource");
                            ok=true;
                            while(resourceItem!=NULL && ok)
                            {
                                Industry::Resource resource;
                                resource.quantity=1;
                                if(resourceItem->Attribute("quantity")!=NULL)
                                {
                                    resource.quantity=stringtouint32(resourceItem->Attribute("quantity"),&ok);
                                    if(!ok)
                                        std::cerr << "quantity is not a number: child->Name(): " << industryItem->Name() << std::endl;
                                }
                                if(ok)
                                {
                                    if(resourceItem->Attribute("id")!=NULL)
                                    {
                                        resource.item=stringtouint16(resourceItem->Attribute("id"),&ok);
                                        if(!ok)
                                            std::cerr << "id is not a number: child->Name(): " << industryItem->Name() << std::endl;
                                        else if(items.find(resource.item)==items.cend())
                                        {
                                            ok=false;
                                            std::cerr << "id is not into the item list: child->Name(): " << industryItem->Name() << std::endl;
                                        }
                                        else
                                        {
                                            unsigned int index=0;
                                            while(index<industry.resources.size())
                                            {
                                                if(industry.resources.at(index).item==resource.item)
                                                    break;
                                                index++;
                                            }
                                            if(index<industry.resources.size())
                                            {
                                                ok=false;
                                                std::cerr << "id of item already into resource or product list: child->Name(): " << industryItem->Name() << std::endl;
                                            }
                                            else
                                            {
                                                index=0;
                                                while(index<industry.products.size())
                                                {
                                                    if(industry.products.at(index).item==resource.item)
                                                        break;
                                                    index++;
                                                }
                                                if(index<industry.products.size())
                                                {
                                                    ok=false;
                                                    std::cerr << "id of item already into resource or product list: child->Name(): " << industryItem->Name() << std::endl;
                                                }
                                                else
                                                    industry.resources.push_back(resource);
                                            }
                                        }
                                    }
                                    else
                                    {
                                        ok=false;
                                        std::cerr << "have not the id attribute: child->Name(): " << industryItem->Name() << std::endl;
                                    }
                                }
                                resourceItem = resourceItem->NextSiblingElement("resource");
                            }
                        }

                        //product
                        if(ok)
                        {
                            const tinyxml2::XMLElement * productItem = industryItem->FirstChildElement("product");
                            ok=true;
                            while(productItem!=NULL && ok)
                            {
                                Industry::Product product;
                                product.quantity=1;
                                if(productItem->Attribute("quantity")!=NULL)
                                {
                                    product.quantity=stringtouint32(productItem->Attribute("quantity"),&ok);
                                    if(!ok)
                                        std::cerr << "quantity is not a number: child->Name(): " << industryItem->Name() << std::endl;
                                }
                                if(ok)
                                {
                                    if(productItem->Attribute("id")!=NULL)
                                    {
                                        product.item=stringtouint16(productItem->Attribute("id"),&ok);
                                        if(!ok)
                                            std::cerr << "id is not a number: child->Name(): " << industryItem->Name() << std::endl;
                                        else if(items.find(product.item)==items.cend())
                                        {
                                            ok=false;
                                            std::cerr << "id is not into the item list: child->Name(): " << industryItem->Name() << std::endl;
                                        }
                                        else
                                        {
                                            unsigned int index=0;
                                            while(index<industry.resources.size())
                                            {
                                                if(industry.resources.at(index).item==product.item)
                                                    break;
                                                index++;
                                            }
                                            if(index<industry.resources.size())
                                            {
                                                ok=false;
                                                std::cerr << "id of item already into resource or product list: child->Name(): " << industryItem->Name() << std::endl;
                                            }
                                            else
                                            {
                                                index=0;
                                                while(index<industry.products.size())
                                                {
                                                    if(industry.products.at(index).item==product.item)
                                                        break;
                                                    index++;
                                                }
                                                if(index<industry.products.size())
                                                {
                                                    ok=false;
                                                    std::cerr << "id of item already into resource or product list: child->Name(): " << industryItem->Name() << std::endl;
                                                }
                                                else
                                                    industry.products.push_back(product);
                                            }
                                        }
                                    }
                                    else
                                    {
                                        ok=false;
                                        std::cerr << "have not the id attribute: child->Name(): " << industryItem->Name() << std::endl;
                                    }
                                }
                                productItem = productItem->NextSiblingElement("product");
                            }
                        }

                        //add
                        if(ok)
                        {
                            if(industry.products.empty() || industry.resources.empty())
                                std::cerr << "product or resources is empty: child->Name(): " << industryItem->Name() << std::endl;
                            else
                                industries[id]=industry;
                        }
                    }
                    else
                        std::cerr << "Unable to open the industries id number already set: file: " << file << ", child->Name(): " << industryItem->Name() << std::endl;
                }
                else
                    std::cerr << "Unable to open the industries id is not a number: file: " << file << ", child->Name(): " << industryItem->Name() << std::endl;
            }
            else
                std::cerr << "Unable to open the industries have not the id: file: " << file << ", child->Name(): " << industryItem->Name() << std::endl;
            industryItem = industryItem->NextSiblingElement("industrialrecipe");
        }
        #ifdef EPOLLCATCHCHALLENGERSERVER
        delete domDocument;
        #endif
        file_index++;
    }
    return industries;
}

std::unordered_map<uint16_t,IndustryLink> DatapackGeneralLoader::loadIndustriesLink(const std::string &file,const std::unordered_map<uint16_t,Industry> &industries)
{
    std::unordered_map<std::string,uint8_t> reputationNameToId;
    {
        uint8_t index=0;
        while(index<CommonDatapack::commonDatapack.reputation.size())
        {
            reputationNameToId[CommonDatapack::commonDatapack.reputation.at(index).name]=index;
            index++;
        }
    }
    std::unordered_map<uint16_t,IndustryLink> industriesLink;
    tinyxml2::XMLDocument *domDocument;
    #ifndef EPOLLCATCHCHALLENGERSERVER
    //open and quick check the file
    if(CommonDatapack::commonDatapack.xmlLoadedFile.find(file)!=CommonDatapack::commonDatapack.xmlLoadedFile.cend())
        domDocument=&CommonDatapack::commonDatapack.xmlLoadedFile[file];
    else
    {
        domDocument=&CommonDatapack::commonDatapack.xmlLoadedFile[file];
        #else
        domDocument=new tinyxml2::XMLDocument();
        #endif
        const auto loadOkay = domDocument->LoadFile(file.c_str());
        if(loadOkay!=0)
        {
            std::cerr << file+", "+tinyxml2errordoc(domDocument) << std::endl;
            return industriesLink;
        }
        #ifndef EPOLLCATCHCHALLENGERSERVER
    }
    #endif
    const tinyxml2::XMLElement * root = domDocument->RootElement();
    if(root==NULL)
    {
        std::cerr << "Unable to open the file: " << file << ", no root balise found for the xml file" << std::endl;
        return industriesLink;
    }
    if(strcmp(root->Name(),"industries")!=0)
    {
        std::cerr << "Unable to open the file: " << file << ", \"industries\" root balise not found for reputation of the xml file" << std::endl;
        return industriesLink;
    }

    //load the content
    bool ok,ok2;
    const tinyxml2::XMLElement * linkItem = root->FirstChildElement("link");
    while(linkItem!=NULL)
    {
        if(linkItem->Attribute("industrialrecipe")!=NULL && linkItem->Attribute("industry")!=NULL)
        {
            const uint16_t &industry_id=stringtouint16(linkItem->Attribute("industrialrecipe"),&ok);
            const uint16_t &factory_id=stringtouint16(linkItem->Attribute("industry"),&ok2);
            if(ok && ok2)
            {
                if(industriesLink.find(factory_id)==industriesLink.cend())
                {
                    if(industries.find(industry_id)!=industries.cend())
                    {
                        industriesLink[factory_id].industry=industry_id;
                        IndustryLink *industryLink=&industriesLink[factory_id];
                        {
                            {
                                const tinyxml2::XMLElement * requirementsItem = linkItem->FirstChildElement("requirements");
                                if(requirementsItem!=NULL)
                                {
                                    const tinyxml2::XMLElement * reputationItem = requirementsItem->FirstChildElement("reputation");
                                    while(reputationItem!=NULL)
                                    {
                                        if(reputationItem->Attribute("type")!=NULL && reputationItem->Attribute("level")!=NULL)
                                        {
                                            if(reputationNameToId.find(reputationItem->Attribute("type"))!=reputationNameToId.cend())
                                            {
                                                ReputationRequirements reputationRequirements;
                                                std::string stringLevel=reputationItem->Attribute("level");
                                                reputationRequirements.positif=!stringStartWith(stringLevel,"-");
                                                if(!reputationRequirements.positif)
                                                    stringLevel.erase(0,1);
                                                reputationRequirements.level=stringtouint8(stringLevel,&ok);
                                                if(ok)
                                                {
                                                    reputationRequirements.reputationId=reputationNameToId.at(reputationItem->Attribute("type"));
                                                    industryLink->requirements.reputation.push_back(reputationRequirements);
                                                }
                                            }
                                            else
                                                std::cerr << "Reputation type not found: have not the id, child->Name(): " << reputationItem->Name() << std::endl;
                                        }
                                        else
                                            std::cerr << "Unable to open the industries link have not the id, file: " << file << ", child->Name(): " << reputationItem->Name() << std::endl;
                                        reputationItem = reputationItem->NextSiblingElement("reputation");
                                    }
                                }
                            }
                            {
                                const tinyxml2::XMLElement * rewardsItem = linkItem->FirstChildElement("rewards");
                                if(rewardsItem!=NULL)
                                {
                                    const tinyxml2::XMLElement * reputationItem = rewardsItem->FirstChildElement("reputation");
                                    while(reputationItem!=NULL)
                                    {
                                        if(reputationItem->Attribute("type")!=NULL && reputationItem->Attribute("point")!=NULL)
                                        {
                                            if(reputationNameToId.find(reputationItem->Attribute("type"))!=reputationNameToId.cend())
                                            {
                                                ReputationRewards reputationRewards;
                                                reputationRewards.point=stringtoint32(reputationItem->Attribute("point"),&ok);
                                                if(ok)
                                                {
                                                    reputationRewards.reputationId=reputationNameToId.at(reputationItem->Attribute("type"));
                                                    industryLink->rewards.reputation.push_back(reputationRewards);
                                                }
                                            }
                                        }
                                        else
                                            std::cerr << "Unable to open the industries link have not the id, file: " << file << ", child->Name(): " << reputationItem->Name() << std::endl;
                                        reputationItem = reputationItem->NextSiblingElement("reputation");
                                    }
                                }
                            }
                        }
                    }
                    else
                        std::cerr << "Industry id for factory is not found: " << industry_id << ", file: " << file << ", child->Name(): " << linkItem->Name() << std::endl;
                }
                else
                    std::cerr << "Factory already found: " << factory_id << ", file: " << file << ", child->Name(): " << linkItem->Name() << std::endl;
            }
            else
                std::cerr << "Unable to open the industries link the attribute is not a number, file: " << file << ", child->Name(): " << linkItem->Name() << std::endl;
        }
        else
            std::cerr << "Unable to open the industries link have not the id, file: " << file << ", child->Name(): " << linkItem->Name() << std::endl;
        linkItem = linkItem->NextSiblingElement("link");
    }
    #ifdef EPOLLCATCHCHALLENGERSERVER
    delete domDocument;
    #endif
    return industriesLink;
}

ItemFull DatapackGeneralLoader::loadItems(const std::string &folder,const std::unordered_map<uint8_t,Buff> &monsterBuffs)
{
    #ifdef EPOLLCATCHCHALLENGERSERVERNOGAMESERVER
    (void)monsterBuffs;
    #endif
    ItemFull items;
    items.itemMaxId=0;
    const std::vector<std::string> &fileList=FacilityLibGeneral::listFolder(folder+'/');
    unsigned int file_index=0;
    while(file_index<fileList.size())
    {
        if(!stringEndsWith(fileList.at(file_index),".xml"))
        {
            file_index++;
            continue;
        }
        const std::string &file=folder+fileList.at(file_index);
        tinyxml2::XMLDocument *domDocument;
        //open and quick check the file
        if(!stringEndsWith(file,".xml"))
        {
            file_index++;
            continue;
        }
        #ifndef EPOLLCATCHCHALLENGERSERVER
        if(CommonDatapack::commonDatapack.xmlLoadedFile.find(file)!=CommonDatapack::commonDatapack.xmlLoadedFile.cend())
            domDocument=&CommonDatapack::commonDatapack.xmlLoadedFile[file];
        else
        {
            domDocument=&CommonDatapack::commonDatapack.xmlLoadedFile[file];
            #else
            domDocument=new tinyxml2::XMLDocument();
            #endif
            const auto loadOkay = domDocument->LoadFile(file.c_str());
            if(loadOkay!=0)
            {
                std::cerr << file+", "+tinyxml2errordoc(domDocument) << std::endl;
                file_index++;
                continue;
            }
            #ifndef EPOLLCATCHCHALLENGERSERVER
        }
        #endif
        const tinyxml2::XMLElement * root = domDocument->RootElement();
        if(root==NULL)
        {
            file_index++;
            continue;
        }
        if(strcmp(root->Name(),"items")!=0)
        {
            file_index++;
            continue;
        }

        //load the content
        bool ok;
        const tinyxml2::XMLElement * item = root->FirstChildElement("item");
        while(item!=NULL)
        {
            if(item->Attribute("id")!=NULL)
            {
                const uint16_t &id=stringtouint16(item->Attribute("id"),&ok);
                if(ok)
                {
                    if(items.item.find(id)==items.item.cend())
                    {
                        if(items.itemMaxId<id)
                            items.itemMaxId=id;
                        //load the price
                        {
                            if(item->Attribute("price")!=NULL)
                            {
                                bool ok;
                                items.item[id].price=stringtouint32(item->Attribute("price"),&ok);
                                if(!ok)
                                {
                                    std::cerr << "price is not a number, file: " << file << ", child->Name(): " << item->Name() << std::endl;
                                    items.item[id].price=0;
                                }
                            }
                            else
                                items.item[id].price=0;
                        }
                        //load the consumeAtUse
                        {
                            if(item->Attribute("consumeAtUse")!=NULL)
                            {
                                if(strcmp(item->Attribute("consumeAtUse"),"false")==0)
                                    items.item[id].consumeAtUse=false;
                                else
                                    items.item[id].consumeAtUse=true;
                            }
                            else
                                items.item[id].consumeAtUse=true;
                        }
                        bool haveAnEffect=false;
                        //load the trap
                        if(!haveAnEffect)
                        {
                            const tinyxml2::XMLElement * trapItem = item->FirstChildElement("trap");
                            if(trapItem!=NULL)
                            {
                                Trap trap;
                                trap.bonus_rate=1.0;
                                if(trapItem->Attribute("bonus_rate")!=NULL)
                                {
                                    float bonus_rate=stringtofloat(trapItem->Attribute("bonus_rate"),&ok);
                                    if(ok)
                                        trap.bonus_rate=bonus_rate;
                                    else
                                        std::cerr << "Unable to open the file: bonus_rate is not a number, file: " << file << ", child->Name(): " << trapItem->Name() << std::endl;
                                }
                                else
                                    std::cerr << "Unable to open the file: trap have not the attribute bonus_rate, file: " << file << ", child->Name(): " << trapItem->Name() << std::endl;
                                items.trap[id]=trap;
                                haveAnEffect=true;
                            }
                        }
                        //load the repel
                        if(!haveAnEffect)
                        {
                            const tinyxml2::XMLElement * repelItem = item->FirstChildElement("repel");
                            if(repelItem!=NULL)
                            {
                                if(repelItem->Attribute("step")!=NULL)
                                {
                                    const uint32_t &step=stringtouint32(repelItem->Attribute("step"),&ok);
                                    if(ok)
                                    {
                                        if(step>0)
                                        {
                                            items.repel[id]=step;
                                            haveAnEffect=true;
                                        }
                                        else
                                            std::cerr << "Unable to open the file: step is not greater than 0, file: " << file << ", child->Name(): " << repelItem->Name() << std::endl;
                                    }
                                    else
                                        std::cerr << "Unable to open the file: step is not a number, file: " << file << ", child->Name(): " << repelItem->Name() << std::endl;
                                }
                                else
                                    std::cerr << "Unable to open the file: repel have not the attribute step, file: " << file << ", child->Name(): " << repelItem->Name() << std::endl;
                            }
                        }
                        //load the monster effect
                        if(!haveAnEffect)
                        {
                            {
                                const tinyxml2::XMLElement * hpItem = item->FirstChildElement("hp");
                                while(hpItem!=NULL)
                                {
                                    if(hpItem->Attribute("add")!=NULL)
                                    {
                                        if(strcmp(hpItem->Attribute("add"),"all")==0)
                                        {
                                            MonsterItemEffect monsterItemEffect;
                                            monsterItemEffect.type=MonsterItemEffectType_AddHp;
                                            monsterItemEffect.data.hp=-1;
                                            items.monsterItemEffect[id].push_back(monsterItemEffect);
                                        }
                                        else
                                        {
                                            std::string addString=hpItem->Attribute("add");
                                            if(addString.find("%")==std::string::npos)//todo this part
                                            {
                                                const int32_t &add=stringtouint32(hpItem->Attribute("add"),&ok);
                                                if(ok)
                                                {
                                                    if(add>0)
                                                    {
                                                        MonsterItemEffect monsterItemEffect;
                                                        monsterItemEffect.type=MonsterItemEffectType_AddHp;
                                                        monsterItemEffect.data.hp=add;
                                                        items.monsterItemEffect[id].push_back(monsterItemEffect);
                                                    }
                                                    else
                                                        std::cerr << "Unable to open the file, add is not greater than 0, file: " << file << ", child->Name(): " << hpItem->Name() << std::endl;
                                                }
                                                else
                                                    std::cerr << "Unable to open the file, add is not a number, file: " << file << ", child->Name(): " << hpItem->Name() << std::endl;
                                            }
                                        }
                                    }
                                    else
                                        std::cerr << "Unable to open the file, hp have not the attribute add, file: " << file << ", child->Name(): " << hpItem->Name() << std::endl;
                                    hpItem = hpItem->NextSiblingElement("hp");
                                }
                            }
                            #ifndef EPOLLCATCHCHALLENGERSERVERNOGAMESERVER
                            {
                                const tinyxml2::XMLElement * buffItem = item->FirstChildElement("buff");
                                while(buffItem!=NULL)
                                {
                                    if(buffItem->Attribute("remove")!=NULL)
                                    {
                                        if(strcmp(buffItem->Attribute("remove"),"all")==0)
                                        {
                                            MonsterItemEffect monsterItemEffect;
                                            monsterItemEffect.type=MonsterItemEffectType_RemoveBuff;
                                            monsterItemEffect.data.buff=-1;
                                            items.monsterItemEffect[id].push_back(monsterItemEffect);
                                        }
                                        else
                                        {
                                            const int8_t &removebuffid=stringtouint8(buffItem->Attribute("remove"),&ok);
                                            if(ok)
                                            {
                                                if(removebuffid>0)
                                                {
                                                    if(monsterBuffs.find(removebuffid)!=monsterBuffs.cend())
                                                    {
                                                        MonsterItemEffect monsterItemEffect;
                                                        monsterItemEffect.type=MonsterItemEffectType_RemoveBuff;
                                                        monsterItemEffect.data.buff=removebuffid;
                                                        items.monsterItemEffect[id].push_back(monsterItemEffect);
                                                    }
                                                    else
                                                        std::cerr << "Unable to open the file, buff item to remove is not found, file: " << file << ", child->Name(): " << buffItem->Name() << std::endl;
                                                }
                                                else
                                                    std::cerr << "Unable to open the file, step is not greater than 0, file: " << file << ", child->Name(): " << buffItem->Name() << std::endl;
                                            }
                                            else
                                                std::cerr << "Unable to open the file, step is not a number, file: " << file << ", child->Name(): " << buffItem->Name() << std::endl;
                                        }
                                    }
                                    /// \todo
                                     /* else
                                        std::cerr << "Unable to open the file: " << file << ", buff have not the attribute know attribute like remove: child->Name(): %2 (at line: %3)").arg(file).arg(buffItem->Name()).arg(CATCHCHALLENGER_XMLELENTATLINE(buffItem));*/
                                    buffItem = buffItem->NextSiblingElement("buff");
                                }
                            }
                            #endif
                            if(items.monsterItemEffect.find(id)!=items.monsterItemEffect.cend())
                                haveAnEffect=true;
                        }
                        //load the monster offline effect
                        if(!haveAnEffect)
                        {
                            const tinyxml2::XMLElement * levelItem = item->FirstChildElement("level");
                            while(levelItem!=NULL)
                            {
                                if(levelItem->Attribute("up")!=NULL)
                                {
                                    const uint8_t &levelUp=stringtouint8(levelItem->Attribute("up"),&ok);
                                    if(!ok)
                                        std::cerr << "Unable to open the file, level up is not possitive number, file: " << file << ", child->Name(): " << levelItem->Name() << std::endl;
                                    else if(levelUp<=0)
                                        std::cerr << "Unable to open the file, level up is greater than 0, file: " << file << ", child->Name(): " << levelItem->Name() << std::endl;
                                    else
                                    {
                                        MonsterItemEffectOutOfFight monsterItemEffectOutOfFight;
                                        monsterItemEffectOutOfFight.type=MonsterItemEffectTypeOutOfFight_AddLevel;
                                        monsterItemEffectOutOfFight.data.level=levelUp;
                                        items.monsterItemEffectOutOfFight[id].push_back(monsterItemEffectOutOfFight);
                                    }
                                }
                                else
                                    std::cerr << "Unable to open the file, level have not the attribute up, file: " << file << ", child->Name(): " << levelItem->Name() << std::endl;
                                levelItem = levelItem->NextSiblingElement("level");
                            }
                        }
                    }
                    else
                        std::cerr << "Unable to open the file, id number already set, file: " << file << ", child->Name(): " << item->Name() << std::endl;
                }
                else
                    std::cerr << "Unable to open the file, id is not a number, file: " << file << ", child->Name(): " << item->Name() << std::endl;
            }
            else
                std::cerr << "Unable to open the file, have not the item id, file: " << file << ", child->Name(): " << item->Name() << std::endl;
            item = item->NextSiblingElement("item");
        }
        #ifdef EPOLLCATCHCHALLENGERSERVER
        delete domDocument;
        #endif
        file_index++;
    }
    return items;
}
#endif

std::vector<std::string> DatapackGeneralLoader::loadSkins(const std::string &folder)
{
    return FacilityLibGeneral::skinIdList(folder);
}

std::pair<std::vector<const tinyxml2::XMLElement *>, std::vector<Profile> > DatapackGeneralLoader::loadProfileList(const std::string &datapackPath, const std::string &file,
                                                                                  #ifndef CATCHCHALLENGER_CLASS_MASTER
                                                                                  const std::unordered_map<uint16_t, Item> &items,
                                                                                  #endif // CATCHCHALLENGER_CLASS_MASTER
                                                                                  const std::unordered_map<uint16_t,Monster> &monsters,const std::vector<Reputation> &reputations)
{
    std::unordered_set<std::string> idDuplicate;
    std::unordered_map<std::string,uint8_t> reputationNameToId;
    {
        uint8_t index=0;
        while(index<CommonDatapack::commonDatapack.reputation.size())
        {
            reputationNameToId[CommonDatapack::commonDatapack.reputation.at(index).name]=index;
            index++;
        }
    }
    std::vector<std::string> defaultforcedskinList;
    std::unordered_map<std::string,uint8_t> skinNameToId;
    {
        uint8_t index=0;
        while(index<CommonDatapack::commonDatapack.skins.size())
        {
            skinNameToId[CommonDatapack::commonDatapack.skins.at(index)]=index;
            defaultforcedskinList.push_back(CommonDatapack::commonDatapack.skins.at(index));
            index++;
        }
    }
    std::pair<std::vector<const tinyxml2::XMLElement *>, std::vector<Profile> > returnVar;
    tinyxml2::XMLDocument *domDocument;
    #ifndef EPOLLCATCHCHALLENGERSERVER
    //open and quick check the file
    if(CommonDatapack::commonDatapack.xmlLoadedFile.find(file)!=CommonDatapack::commonDatapack.xmlLoadedFile.cend())
        domDocument=&CommonDatapack::commonDatapack.xmlLoadedFile[file];
    else
    {
        domDocument=&CommonDatapack::commonDatapack.xmlLoadedFile[file];
        #else
        domDocument=new tinyxml2::XMLDocument();
        #endif
        const auto loadOkay = domDocument->LoadFile(file.c_str());
        if(loadOkay!=0)
        {
            std::cerr << file+", "+tinyxml2errordoc(domDocument) << std::endl;
            return returnVar;
        }
        #ifndef EPOLLCATCHCHALLENGERSERVER
    }
    #endif
    const tinyxml2::XMLElement * root = domDocument->RootElement();
    if(root==NULL)
    {
        std::cerr << "Unable to open the file: " << file << ", no root balise found for the xml file" << std::endl;
        return returnVar;
    }
    if(strcmp(root->Name(),"profile")!=0)
    {
        std::cerr << "Unable to open the file: " << file << ", \"profile\" root balise not found for reputation of the xml file" << std::endl;
        return returnVar;
    }

    //load the content
    bool ok;
    const tinyxml2::XMLElement * startItem = root->FirstChildElement("start");
    while(startItem!=NULL)
    {
        Profile profile;
        profile.cash=0;

        if(startItem->Attribute("id")!=NULL)
            profile.databaseId=startItem->Attribute("id");

        if(idDuplicate.find(profile.databaseId)!=idDuplicate.cend())
        {
            std::cerr << "Unable to open the xml file: " << file << ", child->Name(): " << startItem->Name() << std::endl;
            startItem = startItem->NextSiblingElement("start");
            continue;
        }

        if(!profile.databaseId.empty() && idDuplicate.find(profile.databaseId)==idDuplicate.cend())
        {
            const tinyxml2::XMLElement * forcedskin = startItem->FirstChildElement("forcedskin");

            std::vector<std::string> forcedskinList;
            if(forcedskin!=NULL && forcedskin->Attribute("value")!=NULL)
                forcedskinList=stringsplit(forcedskin->Attribute("value"),';');
            else
                forcedskinList=defaultforcedskinList;
            {
                unsigned int index=0;
                while(index<forcedskinList.size())
                {
                    if(skinNameToId.find(forcedskinList.at(index))!=skinNameToId.cend())
                        profile.forcedskin.push_back(skinNameToId.at(forcedskinList.at(index)));
                    else
                        std::cerr << "Unable to open the xml file: " << file << ", skin " << forcedskinList.at(index) << " don't exists: child->Name(): " << startItem->Name() << std::endl;
                    index++;
                }
            }
            unsigned int index=0;
            while(index<profile.forcedskin.size())
            {
                if(!CatchChallenger::FacilityLibGeneral::isDir(datapackPath+DATAPACK_BASE_PATH_SKIN+CommonDatapack::commonDatapack.skins.at(profile.forcedskin.at(index))))
                {
                    std::cerr << "Unable to open the xml file: " << file << ", skin value: " << forcedskinList.at(index) << " don't exists into: into " << datapackPath << DATAPACK_BASE_PATH_SKIN << CommonDatapack::commonDatapack.skins.at(profile.forcedskin.at(index)) << ": child->Name(): " << startItem->Name() << std::endl;
                    profile.forcedskin.erase(profile.forcedskin.begin()+index);
                }
                else
                    index++;
            }

            profile.cash=0;
            const tinyxml2::XMLElement * cash = startItem->FirstChildElement("cash");
            if(cash!=NULL && cash->Attribute("value")!=NULL)
            {
                profile.cash=stringtodouble(cash->Attribute("value"),&ok);
                if(!ok)
                {
                    std::cerr << "Unable to open the xml file: " << file << ", cash is not a number: child->Name(): " << startItem->Name() << std::endl;
                    profile.cash=0;
                }
            }
            const tinyxml2::XMLElement * monstersElementGroup = startItem->FirstChildElement("monstergroup");
            while(monstersElementGroup!=NULL)
            {
                std::vector<Profile::Monster> monsters_list;
                const tinyxml2::XMLElement * monstersElement = monstersElementGroup->FirstChildElement("monster");
                while(monstersElement!=NULL)
                {
                    Profile::Monster monster;
                    if(monstersElement->Attribute("id")!=NULL && monstersElement->Attribute("level")!=NULL &&
                            monstersElement->Attribute("captured_with")!=NULL)
                    {
                        monster.id=stringtouint16(monstersElement->Attribute("id"),&ok);
                        if(!ok)
                            std::cerr << "Unable to open the xml file: " << file << ", monster id is not a number: child->Name(): " << startItem->Name() << std::endl;
                        if(ok)
                        {
                            monster.level=stringtouint8(monstersElement->Attribute("level"),&ok);
                            if(!ok)
                                std::cerr << "Unable to open the xml file: " << file << ", monster level is not a number: child->Name(): " << startItem->Name() << std::endl;
                        }
                        if(ok)
                        {
                            if(monster.level==0 || monster.level>CATCHCHALLENGER_MONSTER_LEVEL_MAX)
                                std::cerr << "Unable to open the xml file: " << file << ", monster level is not into the range: child->Name(): " << startItem->Name() << std::endl;
                        }
                        if(ok)
                        {
                            monster.captured_with=stringtouint16(monstersElement->Attribute("captured_with"),&ok);
                            if(!ok)
                                std::cerr << "Unable to open the xml file: " << file << ", captured_with is not a number: child->Name(): " << startItem->Name() << std::endl;
                        }
                        if(ok)
                        {
                            if(monsters.find(monster.id)==monsters.cend())
                            {
                                std::cerr << "Unable to open the xml file: " << file << ", starter don't found the monster " << monster.id << ": child->Name(): " << startItem->Name() << std::endl;
                                ok=false;
                            }
                        }
                        #ifndef CATCHCHALLENGER_CLASS_MASTER
                        if(ok)
                        {
                            if(items.find(monster.captured_with)==items.cend())
                                std::cerr << "Unable to open the xml file: " << file << ", starter don't found the monster capture item " << monster.id << ": child->Name(): " << startItem->Name() << std::endl;
                        }
                        #endif // CATCHCHALLENGER_CLASS_MASTER
                        if(ok)
                            monsters_list.push_back(monster);
                    }
                    else
                        std::cerr << "Unable to open the xml file: " << file << ", no monster attribute to load: child->Name(): " << monstersElement->Name() << std::endl;
                    monstersElement = monstersElement->NextSiblingElement("monster");
                }
                if(monsters_list.empty())
                {
                    std::cerr << "Unable to open the xml file: " << file << ", no monster to load: child->Name(): " << monstersElement->Name() << std::endl;
                    startItem = startItem->NextSiblingElement("start");
                    continue;
                }
                profile.monstergroup.push_back(monsters_list);
                monstersElementGroup = monstersElementGroup->NextSiblingElement("monstergroup");
            }
            if(profile.monstergroup.empty())
            {
                std::cerr << "Unable to open the xml file: " << file << ", no monstergroup to load: child->Name(): " << startItem->Name() << std::endl;
                startItem = startItem->NextSiblingElement("start");
                continue;
            }
            const tinyxml2::XMLElement * reputationElement = startItem->FirstChildElement("reputation");
            while(reputationElement!=NULL)
            {
                Profile::Reputation reputationTemp;
                reputationTemp.internalIndex=255;
                reputationTemp.level=-1;
                reputationTemp.point=0xFFFFFFFF;
                if(reputationElement->Attribute("type")!=NULL && reputationElement->Attribute("level")!=NULL)
                {
                    reputationTemp.level=stringtoint8(reputationElement->Attribute("level"),&ok);
                    if(!ok)
                        std::cerr << "Unable to open the xml file: " << file << ", reputation level is not a number: child->Name(): " << startItem->Name() << std::endl;
                    if(ok)
                    {
                        if(reputationNameToId.find(reputationElement->Attribute("type"))==reputationNameToId.cend())
                        {
                            std::cerr << "Unable to open the xml file: " << file << ", reputation type not found " << reputationElement->Attribute("type") << ": child->Name(): " << startItem->Name() << std::endl;
                            ok=false;
                        }
                        if(ok)
                        {
                            reputationTemp.internalIndex=reputationNameToId.at(reputationElement->Attribute("type"));
                            if(reputationTemp.level==0)
                            {
                                std::cerr << "Unable to open the xml file: " << file << ", reputation level is useless if level 0: child->Name(): " << startItem->Name() << std::endl;
                                ok=false;
                            }
                            else if(reputationTemp.level<0)
                            {
                                if((-reputationTemp.level)>(int32_t)reputations.at(reputationTemp.internalIndex).reputation_negative.size())
                                {
                                    std::cerr << "Unable to open the xml file: " << file << ", reputation level is lower than minimal level for "
                                              << reputationElement->Attribute("type") << ": child->Name(): " << startItem->Name() << std::endl;
                                    ok=false;
                                }
                            }
                            else// if(reputationTemp.level>0)
                            {
                                if((reputationTemp.level)>=(int32_t)reputations.at(reputationTemp.internalIndex).reputation_positive.size())
                                {
                                    std::cerr << "Unable to open the xml file: " << file << ", reputation level is higther than maximal level for "
                                              << reputationElement->Attribute("type") << ": child->Name(): " << startItem->Name() << std::endl;
                                    ok=false;
                                }
                            }
                        }
                        if(ok)
                        {
                            reputationTemp.point=0;
                            if(reputationElement->Attribute("point")!=NULL)
                            {
                                reputationTemp.point=stringtoint32(reputationElement->Attribute("point"),&ok);
                                std::cerr << "Unable to open the xml file: " << file << ", reputation point is not a number: child->Name(): " << startItem->Name() << std::endl;
                                if(ok)
                                {
                                    if((reputationTemp.point>0 && reputationTemp.level<0) || (reputationTemp.point<0 && reputationTemp.level>=0))
                                        std::cerr << "Unable to open the xml file: " << file << ", reputation point is not negative/positive like the level: child->Name(): " << startItem->Name() << std::endl;
                                }
                            }
                        }
                    }
                    if(ok)
                        profile.reputations.push_back(reputationTemp);
                }
                reputationElement = reputationElement->NextSiblingElement("reputation");
            }
            const tinyxml2::XMLElement * itemElement = startItem->FirstChildElement("item");
            while(itemElement!=NULL)
            {
                Profile::Item itemTemp;
                if(itemElement->Attribute("id")!=NULL)
                {
                    itemTemp.id=stringtouint16(itemElement->Attribute("id"),&ok);
                    if(!ok)
                        std::cerr << "Unable to open the xml file: " << file << ", item id is not a number: child->Name(): " << startItem->Name() << std::endl;
                    if(ok)
                    {
                        itemTemp.quantity=0;
                        if(itemElement->Attribute("quantity")!=NULL)
                        {
                            itemTemp.quantity=stringtouint32(itemElement->Attribute("quantity"),&ok);
                            if(!ok)
                                std::cerr << "Unable to open the xml file: " << file << ", item quantity is not a number: child->Name(): " << startItem->Name() << std::endl;
                            if(ok)
                            {
                                if(itemTemp.quantity==0)
                                {
                                    std::cerr << "Unable to open the xml file: " << file << ", item quantity is null: child->Name(): " << startItem->Name() << std::endl;
                                    ok=false;
                                }
                            }
                            #ifndef CATCHCHALLENGER_CLASS_MASTER
                            if(ok)
                            {
                                if(items.find(itemTemp.id)==items.cend())
                                {
                                    std::cerr << "Unable to open the xml file: " << file << ", item not found as know item " << itemTemp.id << ": child->Name(): " << startItem->Name() << std::endl;
                                    ok=false;
                                }
                            }
                            #endif // CATCHCHALLENGER_CLASS_MASTER
                        }
                    }
                    if(ok)
                        profile.items.push_back(itemTemp);
                }
                itemElement = itemElement->NextSiblingElement("item");
            }
            idDuplicate.insert(profile.databaseId);
            returnVar.second.push_back(profile);
            returnVar.first.push_back(startItem);
        }
        startItem = startItem->NextSiblingElement("start");
    }
    #ifdef EPOLLCATCHCHALLENGERSERVER
    delete domDocument;
    #endif
    return returnVar;
}

#ifndef CATCHCHALLENGER_CLASS_MASTER
std::vector<MonstersCollision> DatapackGeneralLoader::loadMonstersCollision(const std::string &file, const std::unordered_map<uint16_t, Item> &items,const std::vector<Event> &events)
{
    std::unordered_map<std::string,uint8_t> eventStringToId;
    std::unordered_map<std::string,std::unordered_map<std::string,uint8_t> > eventListingToId;
    {
        uint8_t index=0;
        uint8_t sub_index;
        while(index<events.size())
        {
            const Event &event=events.at(index);
            eventStringToId[event.name]=index;
            sub_index=0;
            while(sub_index<event.values.size())
            {
                eventListingToId[event.name][event.values.at(sub_index)]=sub_index;
                sub_index++;
            }
            index++;
        }
    }
    std::vector<MonstersCollision> returnVar;
    tinyxml2::XMLDocument *domDocument;
    #ifndef EPOLLCATCHCHALLENGERSERVER
    //open and quick check the file
    if(CommonDatapack::commonDatapack.xmlLoadedFile.find(file)!=CommonDatapack::commonDatapack.xmlLoadedFile.cend())
        domDocument=&CommonDatapack::commonDatapack.xmlLoadedFile[file];
    else
    {
        domDocument=&CommonDatapack::commonDatapack.xmlLoadedFile[file];
        #else
        domDocument=new tinyxml2::XMLDocument();
        #endif
        const auto loadOkay = domDocument->LoadFile(file.c_str());
        if(loadOkay!=0)
        {
            std::cerr << file+", "+tinyxml2errordoc(domDocument) << std::endl;
            return returnVar;
        }
        #ifndef EPOLLCATCHCHALLENGERSERVER
    }
    #endif
    const tinyxml2::XMLElement * root = domDocument->RootElement();
    if(root==NULL)
    {
        std::cerr << "Unable to open the file: " << file << ", no root balise found for the xml file" << std::endl;
        return returnVar;
    }
    if(strcmp(root->Name(),"layers")!=0)
    {
        std::cerr << "Unable to open the file: " << file << ", \"layers\" root balise not found for reputation of the xml file" << std::endl;
        return returnVar;
    }

    //load the content
    bool ok;
    const tinyxml2::XMLElement * monstersCollisionItem = root->FirstChildElement("monstersCollision");
    while(monstersCollisionItem!=NULL)
    {
        if(monstersCollisionItem->Attribute("type")==NULL)
            std::cerr << "Have not the attribute type, into file: " << file << std::endl;
        else if(monstersCollisionItem->Attribute("monsterType")==NULL)
            std::cerr << "Have not the attribute monsterType, into: file: " << file << std::endl;
        else
        {
            ok=true;
            MonstersCollision monstersCollision;
            if(strcmp(monstersCollisionItem->Attribute("type"),"walkOn")==0)
                monstersCollision.type=MonstersCollisionType_WalkOn;
            else if(strcmp(monstersCollisionItem->Attribute("type"),"actionOn")==0)
                monstersCollision.type=MonstersCollisionType_ActionOn;
            else
            {
                ok=false;
                std::cerr << "type is not walkOn or actionOn, into file: " << file << std::endl;
            }
            if(ok)
            {
                if(monstersCollisionItem->Attribute("layer")!=NULL)
                    monstersCollision.layer=monstersCollisionItem->Attribute("layer");
            }
            if(ok)
            {
                if(monstersCollision.layer.empty() && monstersCollision.type!=MonstersCollisionType_WalkOn)//need specific layer name to do that's
                {
                    ok=false;
                    std::cerr << "To have blocking layer by item, have specific layer name, into file: " << file << std::endl;
                }
                else
                {
                    monstersCollision.item=0;
                    if(monstersCollisionItem->Attribute("item")!=NULL)
                    {
                        monstersCollision.item=stringtouint16(monstersCollisionItem->Attribute("item"),&ok);
                        if(!ok)
                            std::cerr << "item attribute is not a number, into file: " << file << std::endl;
                        else if(items.find(monstersCollision.item)==items.cend())
                        {
                            ok=false;
                            std::cerr << "item is not into item list, into file: " << file << std::endl;
                        }
                    }
                }
            }
            if(ok)
            {
                if(monstersCollisionItem->Attribute("tile")!=NULL)
                    monstersCollision.tile=monstersCollisionItem->Attribute("tile");
            }
            if(ok)
            {
                if(monstersCollisionItem->Attribute("background")!=NULL)
                    monstersCollision.background=monstersCollisionItem->Attribute("background");
            }
            if(ok)
            {
                if(monstersCollisionItem->Attribute("monsterType")!=NULL)
                {
                    monstersCollision.defautMonsterTypeList=stringsplit(monstersCollisionItem->Attribute("monsterType"),';');
                    vectorRemoveEmpty(monstersCollision.defautMonsterTypeList);
                    vectorRemoveDuplicatesForSmallList(monstersCollision.defautMonsterTypeList);
                    monstersCollision.monsterTypeList=monstersCollision.defautMonsterTypeList;
                    //load the condition
                    const tinyxml2::XMLElement * eventItem = monstersCollisionItem->FirstChildElement("event");
                    while(eventItem!=NULL)
                    {
                        if(eventItem->Attribute("id")!=NULL && eventItem->Attribute("value")!=NULL && eventItem->Attribute("monsterType")!=NULL)
                        {
                            if(eventStringToId.find(eventItem->Attribute("id"))!=eventStringToId.cend())
                            {
                                const auto & list=eventListingToId.at(eventItem->Attribute("id"));
                                if(list.find(eventItem->Attribute("value"))!=list.cend())
                                {
                                    MonstersCollision::MonstersCollisionEvent event;
                                    event.event=eventStringToId.at(eventItem->Attribute("id"));
                                    event.event_value=eventListingToId.at(eventItem->Attribute("id")).at(eventItem->Attribute("value"));
                                    event.monsterTypeList=stringsplit(eventItem->Attribute("monsterType"),';');
                                    if(!event.monsterTypeList.empty())
                                    {
                                        monstersCollision.events.push_back(event);
                                        unsigned int index=0;
                                        while(index<event.monsterTypeList.size())
                                        {
                                            if(!vectorcontainsAtLeastOne(monstersCollision.monsterTypeList,event.monsterTypeList.at(index)))
                                                monstersCollision.monsterTypeList.push_back(event.monsterTypeList.at(index));
                                            index++;
                                        }
                                    }
                                    else
                                        std::cerr << "monsterType is empty, into file: " << file << std::endl;
                                }
                                else
                                    std::cerr << "event value not found, into file: " << file << std::endl;
                            }
                            else
                                std::cerr << "event not found, into file: " << file << std::endl;
                        }
                        else
                            std::cerr << "event have missing attribute, into file: " << file << std::endl;
                        eventItem = eventItem->NextSiblingElement("event");
                    }
                }
            }
            if(ok)
            {
                unsigned int index=0;
                while(index<returnVar.size())
                {
                    if(returnVar.at(index).type==monstersCollision.type && returnVar.at(index).layer==monstersCollision.layer && returnVar.at(index).item==monstersCollision.item)
                    {
                        std::cerr << "similar monstersCollision previously found, into file: " << file << std::endl;
                        ok=false;
                        break;
                    }
                    if(monstersCollision.type==MonstersCollisionType_WalkOn && returnVar.at(index).layer==monstersCollision.layer)
                    {
                        std::cerr << "You can't have different item for same layer in walkOn mode, into file: " << file << std::endl;
                        ok=false;
                        break;
                    }
                    index++;
                }
            }
            if(ok && !monstersCollision.monsterTypeList.empty())
            {
                if(monstersCollision.type==MonstersCollisionType_WalkOn && monstersCollision.layer.empty() && monstersCollision.item==0)
                {
                    if(returnVar.empty())
                        returnVar.push_back(monstersCollision);
                    else
                    {
                        returnVar.push_back(returnVar.back());
                        returnVar.front()=monstersCollision;
                    }
                }
                else
                    returnVar.push_back(monstersCollision);
            }
        }
        monstersCollisionItem = monstersCollisionItem->NextSiblingElement("monstersCollision");
    }
    #ifdef EPOLLCATCHCHALLENGERSERVER
    delete domDocument;
    #endif
    return returnVar;
}

LayersOptions DatapackGeneralLoader::loadLayersOptions(const std::string &file)
{
    LayersOptions returnVar;
    returnVar.zoom=2;
    tinyxml2::XMLDocument *domDocument;
    #ifndef EPOLLCATCHCHALLENGERSERVER
    //open and quick check the file
    if(CommonDatapack::commonDatapack.xmlLoadedFile.find(file)!=CommonDatapack::commonDatapack.xmlLoadedFile.cend())
        domDocument=&CommonDatapack::commonDatapack.xmlLoadedFile[file];
    else
    {
        domDocument=&CommonDatapack::commonDatapack.xmlLoadedFile[file];
        #else
        domDocument=new tinyxml2::XMLDocument();
        #endif
        const auto loadOkay = domDocument->LoadFile(file.c_str());
        if(loadOkay!=0)
        {
            std::cerr << file+", "+tinyxml2errordoc(domDocument) << std::endl;
            return returnVar;
        }
        #ifndef EPOLLCATCHCHALLENGERSERVER
    }
    #endif
    const tinyxml2::XMLElement * root = domDocument->RootElement();
    if(root==NULL)
    {
        std::cerr << "Unable to open the file: " << file << ", no root balise found for the xml file" << std::endl;
        return returnVar;
    }
    if(strcmp(root->Name(),"layers")!=0)
    {
        std::cerr << "Unable to open the file: " << file << ", \"layers\" root balise not found for reputation of the xml file" << std::endl;
        std::cerr << "Unable to open the xml file: " << file << ", \"list\" root balise not found for the xml file" << std::endl;
        return returnVar;
    }
    if(root->Attribute("zoom")!=NULL)
    {
        bool ok;
        returnVar.zoom=stringtouint8(root->Attribute("zoom"),&ok);
        if(!ok)
        {
            returnVar.zoom=2;
            std::cerr << "Unable to open the xml file: " << file << ", zoom is not a number" << std::endl;
        }
        else
        {
            if(returnVar.zoom<1 || returnVar.zoom>4)
            {
                returnVar.zoom=2;
                std::cerr << "Unable to open the xml file: " << file << ", zoom out of range 1-4" << std::endl;
            }
        }
    }
    #ifdef EPOLLCATCHCHALLENGERSERVER
    delete domDocument;
    #endif
    return returnVar;
}

std::vector<Event> DatapackGeneralLoader::loadEvents(const std::string &file)
{
    std::vector<Event> returnVar;

    tinyxml2::XMLDocument *domDocument;
    #ifndef EPOLLCATCHCHALLENGERSERVER
    //open and quick check the file
    if(CommonDatapack::commonDatapack.xmlLoadedFile.find(file)!=CommonDatapack::commonDatapack.xmlLoadedFile.cend())
        domDocument=&CommonDatapack::commonDatapack.xmlLoadedFile[file];
    else
    {
        domDocument=&CommonDatapack::commonDatapack.xmlLoadedFile[file];
        #else
        domDocument=new tinyxml2::XMLDocument();
        #endif
        const auto loadOkay = domDocument->LoadFile(file.c_str());
        if(loadOkay!=0)
        {
            std::cerr << file+", "+tinyxml2errordoc(domDocument) << std::endl;
            return returnVar;
        }
        #ifndef EPOLLCATCHCHALLENGERSERVER
    }
    #endif
    const tinyxml2::XMLElement * root = domDocument->RootElement();
    if(root==NULL)
    {
        std::cerr << "Unable to open the file: " << file << ", no root balise found for the xml file" << std::endl;
        return returnVar;
    }
    if(strcmp(root->Name(),"events")!=0)
    {
        std::cerr << "Unable to open the file: " << file << ", \"events\" root balise not found for reputation of the xml file" << std::endl;
        return returnVar;
    }

    //load the content
    const tinyxml2::XMLElement * eventItem = root->FirstChildElement("event");
    while(eventItem!=NULL)
    {
        if(eventItem->Attribute("id")==NULL)
            std::cerr << "Have not the attribute id, into file: " << file << std::endl;
        else
        {
            std::string idString=eventItem->Attribute("id");
            if(idString.empty())
                std::cerr << "Have id empty, into file: " << file << std::endl;
            else
            {
                Event event;
                event.name=idString;
                const tinyxml2::XMLElement * valueItem = eventItem->FirstChildElement("value");
                while(valueItem!=NULL)
                {
                    if(valueItem->GetText()!=NULL)
                        event.values.push_back(valueItem->GetText());
                    valueItem = valueItem->NextSiblingElement("value");
                }
                if(!event.values.empty())
                    returnVar.push_back(event);
            }
        }
        eventItem = eventItem->NextSiblingElement("event");
    }
    #ifdef EPOLLCATCHCHALLENGERSERVER
    delete domDocument;
    #endif
    return returnVar;
}

/// \see CommonMap, std::unordered_map<std::pair<uint8_t,uint8_t>,std::vector<uint16_t>, pairhash> shops;
std::unordered_map<uint16_t, Shop> DatapackGeneralLoader::preload_shop(const std::string &file, const std::unordered_map<uint16_t, Item> &items)
{
    std::unordered_map<uint16_t,Shop> shops;

    tinyxml2::XMLDocument *domDocument;
    #ifndef EPOLLCATCHCHALLENGERSERVER
    //open and quick check the file
    if(CommonDatapack::commonDatapack.xmlLoadedFile.find(file)!=CommonDatapack::commonDatapack.xmlLoadedFile.cend())
        domDocument=&CommonDatapack::commonDatapack.xmlLoadedFile[file];
    else
    {
        domDocument=&CommonDatapack::commonDatapack.xmlLoadedFile[file];
        #else
        domDocument=new tinyxml2::XMLDocument();
        #endif
        const auto loadOkay = domDocument->LoadFile(file.c_str());
        if(loadOkay!=0)
        {
            std::cerr << file+", "+tinyxml2errordoc(domDocument) << std::endl;
            return shops;
        }
        #ifndef EPOLLCATCHCHALLENGERSERVER
    }
    #endif
    const tinyxml2::XMLElement * root = domDocument->RootElement();
    if(root==NULL)
    {
        std::cerr << "Unable to open the file: " << file << ", no root balise found for the xml file" << std::endl;
        return shops;
    }
    if(strcmp(root->Name(),"shops")!=0)
    {
        std::cerr << "Unable to open the file: " << file << ", \"shops\" root balise not found for reputation of the xml file" << std::endl;
        return shops;
    }

    //load the content
    bool ok;
    const tinyxml2::XMLElement * shopItem = root->FirstChildElement("shop");
    while(shopItem!=NULL)
    {
        if(shopItem->Attribute("id")!=NULL)
        {
            const uint16_t id=stringtouint16(shopItem->Attribute("id"),&ok);
            if(ok)
            {
                if(shops.find(id)==shops.cend())
                {
                    Shop shop;
                    const tinyxml2::XMLElement * product = shopItem->FirstChildElement("product");
                    while(product!=NULL)
                    {
                        if(product->Attribute("itemId")!=NULL)
                        {
                            uint16_t itemId=stringtouint16(product->Attribute("itemId"),&ok);
                            if(!ok)
                                std::cerr << "preload_shop() product attribute itemId is not a number for shops file: " << file << ", child->Name(): " << shopItem->Name() << std::endl;
                            else
                            {
                                if(items.find(itemId)==items.cend())
                                    std::cerr << "preload_shop() product itemId in not into items list for shops file: " << file << ", child->Name(): " << shopItem->Name() << std::endl;
                                else
                                {
                                    uint32_t price=items.at(itemId).price;
                                    if(product->Attribute("overridePrice")!=NULL)
                                    {
                                        price=stringtouint32(product->Attribute("overridePrice"),&ok);
                                        if(!ok)
                                            price=items.at(itemId).price;
                                    }
                                    if(price==0)
                                        std::cerr << "preload_shop() item can't be into the shop with price == 0' for shops file: " << file << ", child->Name(): " << shopItem->Name() << std::endl;
                                    else
                                    {
                                        shop.prices.push_back(price);
                                        shop.items.push_back(itemId);
                                    }
                                }
                            }
                        }
                        else
                            std::cerr << "preload_shop() material have not attribute itemId for shops file: " << file << ", child->Name(): " << shopItem->Name() << std::endl;
                        product = product->NextSiblingElement("product");
                    }
                    shops[id]=shop;
                }
                else
                    std::cerr << "Unable to open the shops file: " << file << ", child->Name(): " << shopItem->Name() << std::endl;
            }
            else
                std::cerr << "Unable to open the shops file: " << file << ", id is not a number: child->Name(): " << shopItem->Name() << std::endl;
        }
        else
            std::cerr << "Unable to open the shops file: " << file << ", have not the shops id: child->Name(): " << shopItem->Name() << std::endl;
        shopItem = shopItem->NextSiblingElement("shop");
    }
    #ifdef EPOLLCATCHCHALLENGERSERVER
    delete domDocument;
    #endif
    return shops;
}
#endif

std::vector<ServerSpecProfile> DatapackGeneralLoader::loadServerProfileList(const std::string &datapackPath, const std::string &mainDatapackCode, const std::string &file,const std::vector<Profile> &profileCommon)
{
    std::vector<ServerSpecProfile> serverProfile=loadServerProfileListInternal(datapackPath,mainDatapackCode,file);
    //index of base profile
    std::unordered_set<std::string> profileId,serverProfileId;
    {
        unsigned int index=0;
        while(index<profileCommon.size())
        {
            const Profile &profile=profileCommon.at(index);
            //already deduplicated at loading
            profileId.insert(profile.databaseId);
            index++;
        }
    }
    //drop serverProfileList
    {
        unsigned int index=0;
        while(index<serverProfile.size())
        {
            const ServerSpecProfile &serverSpecProfile=serverProfile.at(index);
            if(profileId.find(serverSpecProfile.databaseId)!=profileId.cend())
            {
                serverProfileId.insert(serverSpecProfile.databaseId);
                index++;
            }
            else
            {
                std::cerr << "Profile xml file: " << file << ", found id \"" << serverSpecProfile.databaseId << "\" but not found in common, drop it" << std::endl;
                serverProfile.erase(serverProfile.begin()+index);
            }
        }
    }
    //add serverProfileList
    {
        unsigned int index=0;
        while(index<profileCommon.size())
        {
            const Profile &profile=profileCommon.at(index);
            if(serverProfileId.find(profile.databaseId)==serverProfileId.cend())
            {
                std::cerr << "Profile xml file: " << file << ", found common id \"" << profile.databaseId << "\" but not found in server, add it" << std::endl;
                std::cerr << "Mostly due datapack/player/start.xml entry not found into datapack/internal/map/main/official/start.xml" << std::endl;
                /*ServerSpecProfile serverProfileTemp;
                serverProfileTemp.databaseId=profile.databaseId;
                serverProfileTemp.orientation=Orientation_bottom;
                serverProfileTemp.x=0;
                serverProfileTemp.y=0;
                serverProfile.push_back(serverProfileTemp);*/
            }
            index++;
        }
    }

    return serverProfile;
}

std::vector<ServerSpecProfile> DatapackGeneralLoader::loadServerProfileListInternal(const std::string &datapackPath, const std::string &mainDatapackCode, const std::string &file)
{
    std::unordered_set<std::string> idDuplicate;
    std::vector<ServerSpecProfile> serverProfileList;

    tinyxml2::XMLDocument *domDocument;
    #ifndef EPOLLCATCHCHALLENGERSERVER
    //open and quick check the file
    if(CommonDatapack::commonDatapack.xmlLoadedFile.find(file)!=CommonDatapack::commonDatapack.xmlLoadedFile.cend())
        domDocument=&CommonDatapack::commonDatapack.xmlLoadedFile[file];
    else
    {
        domDocument=&CommonDatapack::commonDatapack.xmlLoadedFile[file];
        #else
        domDocument=new tinyxml2::XMLDocument();
        #endif
        const auto loadOkay = domDocument->LoadFile(file.c_str());
        if(loadOkay!=0)
        {
            std::cerr << file+", "+tinyxml2errordoc(domDocument) << std::endl;
            return serverProfileList;
        }
        #ifndef EPOLLCATCHCHALLENGERSERVER
    }
    #endif
    const tinyxml2::XMLElement * root = domDocument->RootElement();
    if(root==NULL)
    {
        std::cerr << "Unable to open the file: " << file << ", no root balise found for the xml file" << std::endl;
        return serverProfileList;
    }
    if(strcmp(root->Name(),"profile")!=0)
    {
        std::cerr << "Unable to open the file: " << file << ", \"profile\" root balise not found for reputation of the xml file" << std::endl;
        return serverProfileList;
    }

    //load the content
    bool ok;
    const tinyxml2::XMLElement * startItem = root->FirstChildElement("start");
    while(startItem!=NULL)
    {
        ServerSpecProfile serverProfile;
        serverProfile.orientation=Orientation_bottom;

        const tinyxml2::XMLElement * map = startItem->FirstChildElement("map");
        if(map!=NULL && map->Attribute("file")!=NULL && map->Attribute("x")!=NULL && map->Attribute("y")!=NULL)
        {
            serverProfile.mapString=map->Attribute("file");
            if(!stringEndsWith(serverProfile.mapString,".tmx"))
                serverProfile.mapString+=".tmx";
            if(!CatchChallenger::FacilityLibGeneral::isFile(datapackPath+DATAPACK_BASE_PATH_MAPMAIN+mainDatapackCode+'/'+serverProfile.mapString))
            {
                std::cerr << "Unable to open the xml file: " << file << ", map don't exists " << serverProfile.mapString << ": child->Name(): " << startItem->Name() << std::endl;
                {
                    std::cerr << "Into the starter the map \"" << serverProfile.mapString << "\" is not found, fix it (abort)" << std::endl;
                    abort();
                }
                startItem = startItem->NextSiblingElement("start");
                continue;
            }
            serverProfile.x=stringtouint8(map->Attribute("x"),&ok);
            if(!ok)
            {
                std::cerr << "Unable to open the xml file: " << file << ", map x is not a number: child->Name(): " << startItem->Name() << std::endl;
                startItem = startItem->NextSiblingElement("start");
                continue;
            }
            serverProfile.y=stringtouint8(map->Attribute("y"),&ok);
            if(!ok)
            {
                std::cerr << "Unable to open the xml file: " << file << ", map y is not a number: child->Name(): " << startItem->Name() << std::endl;
                startItem = startItem->NextSiblingElement("start");
                continue;
            }
        }
        else
        {
            std::cerr << "Unable to open the xml file: " << file << ", no correct map configuration: child->Name(): " << startItem->Name() << std::endl;
            startItem = startItem->NextSiblingElement("start");
            continue;
        }

        if(startItem->Attribute("id")!=NULL)
            serverProfile.databaseId=startItem->Attribute("id");

        if(idDuplicate.find(serverProfile.databaseId)!=idDuplicate.cend())
        {
            std::cerr << "Unable to open the xml file: " << file << ", id duplicate: child->Name(): " << startItem->Name() << std::endl;
            startItem = startItem->NextSiblingElement("start");
            continue;
        }

        if(!serverProfile.databaseId.empty() && idDuplicate.find(serverProfile.databaseId)==idDuplicate.cend())
        {
            idDuplicate.insert(serverProfile.databaseId);
            serverProfileList.push_back(serverProfile);
        }
        startItem = startItem->NextSiblingElement("start");
    }
    #ifdef EPOLLCATCHCHALLENGERSERVER
    delete domDocument;
    #endif
    return serverProfileList;
}


#ifndef CATCHCHALLENGER_CLASS_MASTER
//global to drop useless communication as 100% item luck or to have more informations into client and knowleg on bot
std::unordered_map<uint16_t,std::vector<MonsterDrops> > DatapackGeneralLoader::loadMonsterDrop(const std::string &folder,
                                                                                               const std::unordered_map<uint16_t, Item> &items,
                                                                                               const std::unordered_map<uint16_t,Monster> &monsters)
{
    std::unordered_map<uint16_t,std::vector<MonsterDrops> > monsterDrops;
    const std::vector<FacilityLibGeneral::InodeDescriptor> &fileList=CatchChallenger::FacilityLibGeneral::listFolderNotRecursive(folder,CatchChallenger::FacilityLibGeneral::ListFolder::Files);
    unsigned int file_index=0;
    while(file_index<(uint32_t)fileList.size())
    {
        const std::string &file=fileList.at(file_index).absoluteFilePath;
        if(!stringEndsWith(file,".xml"))
        {
            file_index++;
            continue;
        }

        tinyxml2::XMLDocument *domDocument;
        #ifndef EPOLLCATCHCHALLENGERSERVER
        //open and quick check the file
        if(CommonDatapack::commonDatapack.xmlLoadedFile.find(file)!=CommonDatapack::commonDatapack.xmlLoadedFile.cend())
            domDocument=&CommonDatapack::commonDatapack.xmlLoadedFile[file];
        else
        {
            domDocument=&CommonDatapack::commonDatapack.xmlLoadedFile[file];
            #else
            domDocument=new tinyxml2::XMLDocument();
            #endif
            const auto loadOkay = domDocument->LoadFile(file.c_str());
            if(loadOkay!=0)
            {
                std::cerr << file+", "+tinyxml2errordoc(domDocument) << std::endl;
                file_index++;
                continue;
            }
            #ifndef EPOLLCATCHCHALLENGERSERVER
        }
        #endif
        const tinyxml2::XMLElement * root = domDocument->RootElement();
        if(root==NULL)
        {
            file_index++;
            continue;
        }
        if(strcmp(root->Name(),"monsters")!=0)
        {
            file_index++;
            continue;
        }

        //load the content
        bool ok;
        const tinyxml2::XMLElement * item = root->FirstChildElement("monster");
        while(item!=NULL)
        {
            if(item->Attribute("id")!=NULL)
            {
                const uint16_t &id=stringtouint16(item->Attribute("id"),&ok);
                if(!ok)
                    std::cerr << "Unable to open the xml file: " << file << ", id not a number: child.tagName(): " << item->Name() << std::endl;
                else if(monsters.find(id)==monsters.cend())
                    std::cerr << "Unable to open the xml file: " << file << ", id into the monster list, skip: child.tagName(): " << item->Name() << std::endl;
                else
                {
                    const tinyxml2::XMLElement * drops = item->FirstChildElement("drops");
                    if(drops!=NULL)
                    {
                        const tinyxml2::XMLElement * drop = drops->FirstChildElement("drop");
                        while(drop!=NULL)
                        {
                            if(drop->Attribute("item")!=NULL)
                            {
                                MonsterDrops dropVar;
                                dropVar.item=0;
                                if(drop->Attribute("quantity_min")!=NULL)
                                {
                                    dropVar.quantity_min=stringtouint32(drop->Attribute("quantity_min"),&ok);
                                    if(!ok)
                                        std::cerr << "Unable to open the xml file: " << file << ", quantity_min is not a number: child.tagName(): " << drop->Name() << std::endl;
                                }
                                else
                                    dropVar.quantity_min=1;
                                if(ok)
                                {
                                    if(drop->Attribute("quantity_max")!=NULL)
                                    {
                                        dropVar.quantity_max=stringtouint32(drop->Attribute("quantity_max"),&ok);
                                        if(!ok)
                                            std::cerr << "Unable to open the xml file: " << file << ", quantity_max is not a number: child.tagName(): " << drop->Name() << std::endl;
                                    }
                                    else
                                        dropVar.quantity_max=1;
                                }
                                if(ok)
                                {
                                    if(dropVar.quantity_min<=0)
                                    {
                                        ok=false;
                                        std::cerr << "Unable to open the xml file: " << file << ", dropVar.quantity_min is 0: child.tagName(): " << drop->Name() << std::endl;
                                    }
                                }
                                if(ok)
                                {
                                    if(dropVar.quantity_max<=0)
                                    {
                                        ok=false;
                                        std::cerr << "Unable to open the xml file: " << file << ", dropVar.quantity_max is 0: child.tagName(): " << drop->Name() << std::endl;
                                    }
                                }
                                if(ok)
                                {
                                    if(dropVar.quantity_max<dropVar.quantity_min)
                                    {
                                        ok=false;
                                        std::cerr << "Unable to open the xml file: " << file << ", dropVar.quantity_max<dropVar.quantity_min: child.tagName(): " << drop->Name() << std::endl;
                                    }
                                }
                                if(ok)
                                {
                                    if(drop->Attribute("luck")!=NULL)
                                    {
                                        std::string luck=drop->Attribute("luck");
                                        if(!luck.empty())
                                            if(luck.back()=='%')
                                                luck.resize(luck.size()-1);
                                        dropVar.luck=stringtouint8(luck,&ok);
                                        if(!ok)
                                            std::cerr << "Unable to open the xml file: " << file << ", luck is not a number: child.tagName(): " << drop->Name() << std::endl;
                                        else if(dropVar.luck==0)
                                        {
                                            std::cerr << "Unable to open the xml file: " << file << ", luck can't be 0: child.tagName(): " << drop->Name() << std::endl;
                                            ok=false;
                                        }
                                    }
                                    else
                                        dropVar.luck=100;
                                }
                                if(ok)
                                {
                                    if(dropVar.luck<=0)
                                    {
                                        ok=false;
                                        std::cerr << "Unable to open the xml file: " << file << ", luck is 0!: child.tagName(): " << drop->Name() << std::endl;
                                    }
                                    if(dropVar.luck>100)
                                    {
                                        ok=false;
                                        std::cerr << "Unable to open the xml file: " << file << ", luck is greater than 100: child.tagName(): " << drop->Name() << std::endl;
                                    }
                                }
                                if(ok)
                                {
                                    if(drop->Attribute("item")!=NULL)
                                    {
                                        dropVar.item=stringtouint16(drop->Attribute("item"),&ok);
                                        if(!ok)
                                            std::cerr << "Unable to open the xml file: " << file << ", item is not a number: child.tagName(): " << drop->Name() << std::endl;
                                    }
                                    else
                                        dropVar.luck=100;
                                }
                                if(ok)
                                {
                                    if(items.find(dropVar.item)==items.cend())
                                    {
                                        ok=false;
                                        std::cerr << "Unable to open the xml file: " << file << ", the item " << dropVar.item << " is not into the item list: child.tagName(): %2" << std::endl;
                                    }
                                }
                                if(ok)
                                {
                                    if(static_cast<double>(CommonSettingsServer::commonSettingsServer.rates_drop)!=1.0)
                                    {
                                        if(CommonSettingsServer::commonSettingsServer.rates_drop==0)
                                        {
                                            std::cerr << "CommonSettingsServer::commonSettingsServer.rates_drop==0 durring loading the drop, reset to 1" << std::endl;
                                            CommonSettingsServer::commonSettingsServer.rates_drop=1;
                                        }
                                        dropVar.luck=dropVar.luck*CommonSettingsServer::commonSettingsServer.rates_drop;
                                        double targetAverage=static_cast<double>(((double)dropVar.quantity_min+(double)dropVar.quantity_max)/2.0);
                                        targetAverage=static_cast<double>((targetAverage*dropVar.luck)/100.0);
                                        while(dropVar.luck>100)
                                        {
                                            dropVar.quantity_max++;
                                            double currentAverage=static_cast<double>(((double)dropVar.quantity_min+(double)dropVar.quantity_max)/2.0);
                                            dropVar.luck=static_cast<double>((100.0*targetAverage)/currentAverage);
                                        }
                                    }
                                    monsterDrops[id].push_back(dropVar);
                                }
                            }
                            else
                                std::cerr << "Unable to open the xml file: " << file << ", as not item attribute: child.tagName(): " << drop->Name() << std::endl;
                            drop = drop->NextSiblingElement("drop");
                        }
                    }
                }
            }
            else
                std::cerr << "Unable to open the xml file: " << file << ", have not the monster id: child.tagName(): " << item->Name() << std::endl;
            item = item->NextSiblingElement("monster");
        }
        #ifdef EPOLLCATCHCHALLENGERSERVER
        delete domDocument;
        #endif
        file_index++;
    }
    return monsterDrops;
}
#endif

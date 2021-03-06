#include "FightLoader.h"
#include "../base/GeneralVariable.h"
#include "../base/CommonSettingsCommon.h"
#include "../base/CommonSettingsServer.h"
#include "../base/FacilityLibGeneral.h"
#include "../base/CommonDatapack.h"
#include "../base/tinyXML2/tinyxml2.h"
#include "../base/CachedString.h"

#include <vector>
#include <iostream>
#include <math.h>
#include <cmath>

using namespace CatchChallenger;

bool CatchChallenger::operator<(const Monster::AttackToLearn &entry1, const Monster::AttackToLearn &entry2)
{
    if(entry1.learnAtLevel!=entry2.learnAtLevel)
        return entry1.learnAtLevel < entry2.learnAtLevel;
    else
        return entry1.learnSkill < entry2.learnSkill;
}

#ifndef CATCHCHALLENGER_CLASS_MASTER
std::vector<Type> FightLoader::loadTypes(const std::string &file)
{
    std::unordered_map<std::string,uint8_t> nameToId;
    std::vector<Type> types;
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
            std::cerr << file+", "+domDocument->ErrorName() << std::endl;
            return types;
        }
        #ifndef EPOLLCATCHCHALLENGERSERVER
    }
    #endif
    const tinyxml2::XMLElement * root = domDocument->RootElement();
    if(root==NULL)
    {
        std::cerr << "Unable to open the file: " << file << ", no root balise found for the xml file" << std::endl;
        return types;
    }
    if(strcmp(root->Name(),"types")!=0)
    {
        std::cerr << "Unable to open the file: " << file << ", \"types\" root balise not found for the xml file" << std::endl;
        return types;
    }

    //load the content
    bool ok;
    {
        std::unordered_set<std::string> duplicate;
        const tinyxml2::XMLElement * typeItem = root->FirstChildElement(XMLCACHEDSTRING_type);
        while(typeItem!=NULL)
        {
            if(typeItem->Attribute(XMLCACHEDSTRING_name)!=NULL)
            {
                std::string name=std::string(typeItem->Attribute(XMLCACHEDSTRING_name));
                if(duplicate.find(name)==duplicate.cend())
                {
                    duplicate.insert(name);
                    Type type;
                    type.name=name;
                    nameToId[type.name]=static_cast<uint8_t>(types.size());
                    types.push_back(type);
                }
                else
                    std::cerr << "Unable to open the file: " << file << ", name is already set for type: child->Name(): " << typeItem->Name() << std::endl;
            }
            else
                std::cerr << "Unable to open the file: " << file << ", have not the item id: child->Name(): " << typeItem->Name() << std::endl;
            typeItem = typeItem->NextSiblingElement(XMLCACHEDSTRING_type);
        }
    }
    {
        std::unordered_set<std::string> duplicate;
        const tinyxml2::XMLElement * typeItem = root->FirstChildElement(XMLCACHEDSTRING_type);
        while(typeItem!=NULL)
        {
            if(typeItem->Attribute(XMLCACHEDSTRING_name)!=NULL)
            {
                std::string name=std::string(typeItem->Attribute(XMLCACHEDSTRING_name));
                if(duplicate.find(name)==duplicate.cend())
                {
                    duplicate.insert(name);
                    const tinyxml2::XMLElement * multiplicator = typeItem->FirstChildElement(XMLCACHEDSTRING_multiplicator);
                    while(multiplicator!=NULL)
                    {
                        if(multiplicator->Attribute(XMLCACHEDSTRING_number)!=NULL && multiplicator->Attribute(XMLCACHEDSTRING_to)!=NULL)
                        {
                            float number=stringtofloat(multiplicator->Attribute(XMLCACHEDSTRING_number),&ok);
                            std::vector<std::string> to=stringsplit(multiplicator->Attribute(XMLCACHEDSTRING_to),';');
                            if(ok && (static_cast<double>(number)==2.0 || static_cast<double>(number)==0.5 || static_cast<double>(number)==0.0))
                            {
                                unsigned int index=0;
                                while(index<to.size())
                                {
                                    if(nameToId.find(to.at(index))!=nameToId.cend())
                                    {
                                        const std::string &typeName=to.at(index);
                                        if(number==0)
                                            types[nameToId.at(name)].multiplicator[nameToId.at(typeName)]=0;
                                        else if(number>1)
                                            types[nameToId.at(name)].multiplicator[nameToId.at(typeName)]=number;
                                        else
                                            types[nameToId.at(name)].multiplicator[nameToId.at(typeName)]=static_cast<float>(-(1.0/static_cast<double>(number)));
                                    }
                                    else
                                        std::cerr << "Unable to open the file: " << file << ", name is not into list: " << to.at(index) << " is not found: child->Name(): " << multiplicator->Name() << std::endl;
                                    index++;
                                }
                            }
                            else
                                std::cerr << "Unable to open the file: " << file << ", name is already set for type: child->Name(): " << multiplicator->Name() << std::endl;
                        }
                        else
                            std::cerr << "Unable to open the file: " << file << ", have not the item id: child->Name(): " << multiplicator->Name() << std::endl;
                        multiplicator = multiplicator->NextSiblingElement(XMLCACHEDSTRING_multiplicator);
                    }
                }
                else
                    std::cerr << "Unable to open the file: " << file << ", name is already set for type: child->Name(): " << typeItem->Name() << std::endl;
            }
            else
                std::cerr << "Unable to open the file: " << file << ", have not the item id: child->Name(): " << typeItem->Name() << std::endl;
            typeItem = typeItem->NextSiblingElement(XMLCACHEDSTRING_type);
        }
    }
    #ifdef EPOLLCATCHCHALLENGERSERVER
    delete domDocument;
    #endif
    return types;
}
#endif

std::unordered_map<uint16_t,Monster> FightLoader::loadMonster(const std::string &folder, const std::unordered_map<uint16_t, Skill> &monsterSkills
                                                #ifndef CATCHCHALLENGER_CLASS_MASTER
                                                , const std::vector<Type> &types, const std::unordered_map<uint16_t, Item> &items,
                                                uint16_t &monstersMaxId
                                                #endif
                                                )
{
    #ifndef CATCHCHALLENGER_CLASS_MASTER
    #ifdef CATCHCHALLENGER_EXTRA_CHECK
    if(types.size()>255)
    {
        std::cerr << "FightLoader::loadMonster() types.size()>255" << std::endl;
        abort();
    }
    #endif
    #endif
    #ifndef CATCHCHALLENGER_CLASS_MASTER
    if(CommonSettingsServer::commonSettingsServer.rates_xp_pow==0)
    {
        std::cerr << "CommonSettingsServer::commonSettingsServer.rates_xp_pow==0 abort" << std::endl;
        abort();
    }
    #endif
    std::unordered_map<uint16_t,Monster> monsters;
    const std::vector<FacilityLibGeneral::InodeDescriptor> &fileList=
            CatchChallenger::FacilityLibGeneral::listFolderNotRecursive(folder,CatchChallenger::FacilityLibGeneral::ListFolder::Files);
    unsigned int file_index=0;
    while(file_index<(uint32_t)fileList.size())
    {
        const std::string &file=fileList.at(file_index).absoluteFilePath;
        if(!stringEndsWith(file,CACHEDSTRING_dotxml))
        {
            file_index++;
            continue;
        }
        #ifndef CATCHCHALLENGER_CLASS_MASTER
        std::unordered_map<std::string,uint8_t> typeNameToId;
        {
            uint8_t index=0;
            while(index<types.size())
            {
                typeNameToId[types.at(index).name]=index;
                index++;
            }
        }
        #endif
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
                std::cerr << file+", "+domDocument->ErrorName() << std::endl;
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
        if(strcmp(root->Name(),XMLCACHEDSTRING_monsters)!=0)
        {
            file_index++;
            continue;
        }

        //load the content
        bool ok,ok2;
        tinyxml2::XMLElement * item = const_cast<tinyxml2::XMLElement *>(root->FirstChildElement(XMLCACHEDSTRING_monster));
        while(item!=NULL)
        {
            bool attributeIsOk=true;
            if(item->Attribute(XMLCACHEDSTRING_id)==NULL)
            {
                std::cerr << "Unable to open the xml file: " << file << ", have not the monster attribute \"id\": child->Name(): " << item->Name() << std::endl;
                attributeIsOk=false;
            }
            #ifndef CATCHCHALLENGER_CLASS_MASTER
            if(item->Attribute(XMLCACHEDSTRING_egg_step)==NULL)
            {
                std::cerr << "Unable to open the xml file: " << file << ", have not the monster attribute \"egg_step\": child->Name(): " << item->Name() << std::endl;
                attributeIsOk=false;
            }
            if(item->Attribute(XMLCACHEDSTRING_xp_for_max_level)==NULL && item->Attribute(XMLCACHEDSTRING_xp_max)==NULL)
            {
                std::cerr << "Unable to open the xml file: " << file << ", have not the monster attribute \"xp_for_max_level\": child->Name(): " << item->Name() << std::endl;
                attributeIsOk=false;
            }
            else
            {
                if(item->Attribute(XMLCACHEDSTRING_xp_for_max_level)==NULL)
                    item->SetAttribute(XMLCACHEDSTRING_xp_for_max_level,item->Attribute(XMLCACHEDSTRING_xp_max));
            }
            if(item->Attribute(XMLCACHEDSTRING_hp)==NULL)
            {
                std::cerr << "Unable to open the xml file: " << file << ", have not the monster attribute \"hp\": child->Name(): " << item->Name() << std::endl;
                attributeIsOk=false;
            }
            if(item->Attribute(XMLCACHEDSTRING_attack)==NULL)
            {
                std::cerr << "Unable to open the xml file: " << file << ", have not the monster attribute \"attack\": child->Name(): " << item->Name() << std::endl;
                attributeIsOk=false;
            }
            if(item->Attribute(XMLCACHEDSTRING_defense)==NULL)
            {
                std::cerr << "Unable to open the xml file: " << file << ", have not the monster attribute \"defense\": child->Name(): " << item->Name() << std::endl;
                attributeIsOk=false;
            }
            if(item->Attribute(XMLCACHEDSTRING_special_attack)==NULL)
            {
                std::cerr << "Unable to open the xml file: " << file << ", have not the monster attribute \"special_attack\": child->Name(): " << item->Name() << std::endl;
                attributeIsOk=false;
            }
            if(item->Attribute(XMLCACHEDSTRING_special_defense)==NULL)
            {
                std::cerr << "Unable to open the xml file: " << file << ", have not the monster attribute \"special_defense\": child->Name(): " << item->Name() << std::endl;
                attributeIsOk=false;
            }
            if(item->Attribute(XMLCACHEDSTRING_speed)==NULL)
            {
                std::cerr << "Unable to open the xml file: " << file << ", have not the monster attribute \"speed\": child->Name(): " << item->Name() << std::endl;
                attributeIsOk=false;
            }
            if(item->Attribute(XMLCACHEDSTRING_give_sp)==NULL)
            {
                std::cerr << "Unable to open the xml file: " << file << ", have not the monster attribute \"give_sp\": child->Name(): " << item->Name() << std::endl;
                attributeIsOk=false;
            }
            if(item->Attribute(XMLCACHEDSTRING_give_xp)==NULL)
            {
                std::cerr << "Unable to open the xml file: " << file << ", have not the monster attribute \"give_xp\": child->Name(): " << item->Name() << std::endl;
                attributeIsOk=false;
            }
            #endif // CATCHCHALLENGER_CLASS_MASTER
            if(attributeIsOk)
            {
                Monster monster;
                #ifndef CATCHCHALLENGER_CLASS_MASTER
                monster.catch_rate=100;
                #endif
                uint16_t id=stringtouint16(item->Attribute(XMLCACHEDSTRING_id),&ok);
                if(!ok)
                    std::cerr << "Unable to open the xml file: " << file << ", id not a number: child->Name(): " << item->Name() << std::endl;
                else if(monsters.find(id)!=monsters.cend())
                    std::cerr << "Unable to open the xml file: " << file << ", id already found: child->Name(): " << item->Name() << std::endl;
                else
                {
                    #ifndef CATCHCHALLENGER_CLASS_MASTER
                    if(item->Attribute(XMLCACHEDSTRING_catch_rate)!=NULL)
                    {
                        bool ok2;
                        uint8_t catch_rate=stringtouint8(item->Attribute(XMLCACHEDSTRING_catch_rate),&ok2);
                        if(ok2)
                        {
                            //if(catch_rate<=255)
                                monster.catch_rate=catch_rate;
                            /*else
                                std::cerr << "Unable to open the xml file: " << file << ", catch_rate is not a number: " << CATCHCHALLENGER_XMLATTRIBUTETOSTRING(item->Attribute(XMLCACHEDSTRING_catch_rate)) << " child->Name(): " << item->Name() << std::endl;*/
                        }
                        else
                            std::cerr << "Unable to open the xml file: " << file << ", catch_rate is not a number: " << item->Attribute(XMLCACHEDSTRING_catch_rate) << " child->Name(): " << item->Name() << std::endl;
                    }
                    if(item->Attribute(XMLCACHEDSTRING_type)!=NULL)
                    {
                        const std::vector<std::string> &typeList=stringsplit(item->Attribute(XMLCACHEDSTRING_type),';');
                        unsigned int index=0;
                        while(index<typeList.size())
                        {
                            if(typeNameToId.find(typeList.at(index))!=typeNameToId.cend())
                                monster.type.push_back(typeNameToId.at(typeList.at(index)));
                            else
                                std::cerr << "Unable to open the xml file: " << file << ", type not found into the list: " << item->Attribute(XMLCACHEDSTRING_type) << " child->Name(): " << item->Name() << std::endl;
                            index++;
                        }
                    }
                    if(item->Attribute(XMLCACHEDSTRING_type2)!=NULL)
                    {
                        const std::vector<std::string> &typeList=stringsplit(item->Attribute(XMLCACHEDSTRING_type2),';');
                        unsigned int index=0;
                        while(index<typeList.size())
                        {
                            if(typeNameToId.find(typeList.at(index))!=typeNameToId.cend())
                                monster.type.push_back(typeNameToId.at(typeList.at(index)));
                            else
                                std::cerr << "Unable to open the xml file: " << file << ", type not found into the list: " << item->Attribute(XMLCACHEDSTRING_type2) << " child->Name(): " << item->Name() << std::endl;
                            index++;
                        }
                    }
                    #ifdef CATCHCHALLENGER_CLIENT
                    double &powerVar=monster.powerVar;
                    #else
                    double powerVar=1.0;
                    #endif
                    powerVar=1.0;
                    if(ok)
                    {
                        if(item->Attribute(XMLCACHEDSTRING_pow)!=NULL)
                        {
                            powerVar=stringtodouble(item->Attribute(XMLCACHEDSTRING_pow),&ok);
                            if(!ok)
                            {
                                powerVar=1.0;
                                std::cerr << "Unable to open the xml file: " << file << ", pow is not a double: child->Name(): " << item->Name() << std::endl;
                                ok=true;
                            }
                            if(powerVar<=1.0)
                            {
                                powerVar=1.0;
                                std::cerr << "Unable to open the xml file: " << file << ", pow is too low: child->Name(): " << item->Name() << std::endl;
                            }
                            if(powerVar>=10.0)
                            {
                                powerVar=1.0;
                                std::cerr << "Unable to open the xml file: " << file << ", pow is too hight: child->Name(): " << item->Name() << std::endl;
                            }
                        }
                    }
                    #ifndef CATCHCHALLENGER_CLASS_MASTER
                    #ifndef EPOLLCATCHCHALLENGERSERVERNOGAMESERVER
                    powerVar*=static_cast<double>(CommonSettingsServer::commonSettingsServer.rates_xp_pow);
                    #endif
                    #endif
                    if(ok)
                    {
                        monster.egg_step=stringtouint32(item->Attribute(XMLCACHEDSTRING_egg_step),&ok);
                        if(!ok)
                            std::cerr << "Unable to open the xml file: " << file << ", egg_step is not number: child->Name(): " << item->Name() << std::endl;
                    }
                    if(ok)
                    {
                        monster.xp_for_max_level=stringtouint32(item->Attribute(XMLCACHEDSTRING_xp_for_max_level),&ok);
                        if(!ok)
                            std::cerr << "Unable to open the xml file: " << file << ", xp_for_max_level is not number: child->Name(): " << item->Name() << std::endl;
                    }
                    #endif
                    if(ok)
                    {
                        monster.stat.hp=stringtouint32(item->Attribute(XMLCACHEDSTRING_hp),&ok);
                        if(!ok)
                            std::cerr << "Unable to open the xml file: " << file << ", hp is not number: child->Name(): " << item->Name() << std::endl;
                    }
                    #ifndef CATCHCHALLENGER_CLASS_MASTER
                    if(ok)
                    {
                        monster.stat.attack=stringtouint32(item->Attribute(XMLCACHEDSTRING_attack),&ok);
                        if(!ok)
                            std::cerr << "Unable to open the xml file: " << file << ", attack is not number: child->Name(): " << item->Name() << std::endl;
                    }
                    if(ok)
                    {
                        monster.stat.defense=stringtouint32(item->Attribute(XMLCACHEDSTRING_defense),&ok);
                        if(!ok)
                            std::cerr << "Unable to open the xml file: " << file << ", defense is not number: child->Name(): " << item->Name() << std::endl;
                    }
                    if(ok)
                    {
                        monster.stat.special_attack=stringtouint32(item->Attribute(XMLCACHEDSTRING_special_attack),&ok);
                        if(!ok)
                            std::cerr << "Unable to open the xml file: " << file << ", special_attack is not number: child->Name(): " << item->Name() << std::endl;
                    }
                    if(ok)
                    {
                        monster.stat.special_defense=stringtouint32(item->Attribute(XMLCACHEDSTRING_special_defense),&ok);
                        if(!ok)
                            std::cerr << "Unable to open the xml file: " << file << ", special_defense is not number: child->Name(): " << item->Name() << std::endl;
                    }
                    if(ok)
                    {
                        monster.stat.speed=stringtouint32(item->Attribute(XMLCACHEDSTRING_speed),&ok);
                        if(!ok)
                            std::cerr << "Unable to open the xml file: " << file << ", speed is not number: child->Name(): " << item->Name() << std::endl;
                    }
                    #ifndef EPOLLCATCHCHALLENGERSERVERNOGAMESERVER
                    if(CommonSettingsServer::commonSettingsServer.rates_xp<=0)
                    {
                        std::cerr << "CommonSettingsServer::commonSettingsServer.rates_xp can't be null" << std::endl;
                        abort();
                    }
                    if(ok)
                    {
                        monster.give_xp=stringtouint32(item->Attribute(XMLCACHEDSTRING_give_xp),&ok)*CommonSettingsServer::commonSettingsServer.rates_xp;
                        if(!ok)
                            std::cerr << "Unable to open the xml file: " << file << ", give_xp is not number: child->Name(): " << item->Name() << std::endl;
                    }
                    if(ok)
                    {
                        monster.give_sp=stringtouint32(item->Attribute(XMLCACHEDSTRING_give_sp),&ok)*CommonSettingsServer::commonSettingsServer.rates_xp;
                        if(!ok)
                            std::cerr << "Unable to open the xml file: " << file << ", give_sp is not number: child->Name(): " << item->Name() << std::endl;
                    }
                    #else
                    monster.give_xp=0;
                    monster.give_sp=0;
                    #endif
                    #endif
                    if(ok)
                    {
                        if(item->Attribute(XMLCACHEDSTRING_ratio_gender)!=NULL)
                        {
                            std::string ratio_gender=std::string(item->Attribute(XMLCACHEDSTRING_ratio_gender));
                            stringreplaceOne(ratio_gender,CACHEDSTRING_percent,"");
                            monster.ratio_gender=stringtoint8(ratio_gender,&ok2);
                            if(!ok2)
                            {
                                std::cerr << "Unable to open the xml file: " << file << ", ratio_gender is not number: child->Name(): " << item->Name() << std::endl;
                                monster.ratio_gender=50;
                            }
                            if(monster.ratio_gender<-1 || monster.ratio_gender>100)
                            {
                                std::cerr << "Unable to open the xml file: " << file << ", ratio_gender is not in range of -1, 100: child->Name(): " << item->Name() << std::endl;
                                monster.ratio_gender=50;
                            }
                        }
                        monster.ratio_gender=50;
                    }
                    if(ok)
                    {
                        {
                            const tinyxml2::XMLElement * attack_list = item->FirstChildElement(XMLCACHEDSTRING_attack_list);
                            if(attack_list!=NULL)
                            {
                                tinyxml2::XMLElement * attack = const_cast<tinyxml2::XMLElement *>(attack_list->FirstChildElement(XMLCACHEDSTRING_attack));
                                while(attack!=NULL)
                                {
                                    if(attack->Attribute(XMLCACHEDSTRING_skill)!=NULL || attack->Attribute(XMLCACHEDSTRING_id)!=NULL)
                                    {
                                        ok=true;
                                        if(attack->Attribute(XMLCACHEDSTRING_skill)==NULL)
                                            attack->SetAttribute(XMLCACHEDSTRING_skill,attack->Attribute(XMLCACHEDSTRING_id));
                                        Monster::AttackToLearn attackVar;
                                        if(attack->Attribute(XMLCACHEDSTRING_skill_level)!=NULL || attack->Attribute(XMLCACHEDSTRING_attack_level)!=NULL)
                                        {
                                            if(attack->Attribute(XMLCACHEDSTRING_skill_level)==NULL)
                                                attack->SetAttribute(XMLCACHEDSTRING_skill_level,attack->Attribute(XMLCACHEDSTRING_attack_level));
                                            attackVar.learnSkillLevel=stringtouint8(attack->Attribute(XMLCACHEDSTRING_skill_level),&ok);
                                            if(!ok)
                                                std::cerr << "Unable to open the xml file: " << file << ", skill_level is not a number: child->Name(): " << attack->Name() << std::endl;
                                        }
                                        else
                                            attackVar.learnSkillLevel=1;
                                        if(ok)
                                        {
                                            attackVar.learnSkill=stringtouint16(attack->Attribute(XMLCACHEDSTRING_skill),&ok);
                                            if(!ok)
                                                std::cerr << "Unable to open the xml file: " << file << ", skill is not a number: child->Name(): " << attack->Name() << std::endl;
                                        }
                                        if(ok)
                                        {
                                            if(monsterSkills.find(attackVar.learnSkill)==monsterSkills.cend())
                                            {
                                                std::cerr << "Unable to open the xml file: " << file << ", attack is not into attack loaded: child->Name(): " << attack->Name() << std::endl;
                                                ok=false;
                                            }
                                        }
                                        if(ok)
                                        {
                                            if(attackVar.learnSkillLevel<=0 || attackVar.learnSkillLevel>(uint32_t)monsterSkills.at(attackVar.learnSkill).level.size())
                                            {
                                                std::cerr << "Unable to open the xml file: " << file << ", attack level is not in range 1-" << monsterSkills.at(attackVar.learnSkill).level.size() << ": child->Name(): " << attack->Name() << std::endl;
                                                ok=false;
                                            }
                                        }
                                        if(attack->Attribute(XMLCACHEDSTRING_level)!=NULL)
                                        {
                                            if(ok)
                                            {
                                                attackVar.learnAtLevel=stringtouint8(attack->Attribute(XMLCACHEDSTRING_level) ,&ok);
                                                if(ok)
                                                {
                                                    unsigned int index=0;
                                                    //if it's the first lean don't need previous learn
                                                    if(attackVar.learnSkillLevel>1)
                                                    {
                                                        index=0;
                                                        while(index<monster.learn.size())
                                                        {
                                                            if(monster.learn.at(index).learnSkillLevel==(attackVar.learnSkillLevel-1) && monster.learn.at(index).learnSkill==attackVar.learnSkill)
                                                                break;
                                                            index++;
                                                        }
                                                        if(index==monster.learn.size())
                                                        {
                                                            std::cerr << "Unable to open the xml file: " << file << ", attack " << attackVar.learnSkill << " with level " << attackVar.learnSkillLevel << " can't be added because not same attack with previous level: child->Name(): " << attack->Name() << std::endl;
                                                            ok=false;
                                                        }
                                                    }

                                                    //check if can learn
                                                    if(ok)
                                                    {
                                                        index=0;
                                                        while(index<monster.learn.size())
                                                        {
                                                            if(attackVar.learnSkillLevel>1 && (monster.learn.at(index).learnSkillLevel-1)==attackVar.learnSkillLevel)
                                                                ok=true;
                                                            if(monster.learn.at(index).learnSkillLevel==attackVar.learnSkillLevel && monster.learn.at(index).learnSkill==attackVar.learnSkill)
                                                            {
                                                                std::cerr << "Unable to open the xml file: " << file << ", attack already do for this level for skill " << attackVar.learnSkill << " at level " << attackVar.learnSkillLevel << " for monster " << id << ": child->Name(): " << attack->Name() << std::endl;
                                                                ok=false;
                                                                break;
                                                            }
                                                            if(monster.learn.at(index).learnSkill==attackVar.learnSkill && monster.learn.at(index).learnSkillLevel==attackVar.learnSkillLevel)
                                                            {
                                                                std::cerr << "Unable to open the xml file: " << file << ", this attack level is already found " << attackVar.learnSkill << ", level: " << attackVar.learnSkillLevel << " for attack: " << index << ": child->Name(): " << attack->Name() << std::endl;
                                                                ok=false;
                                                                break;
                                                            }
                                                            index++;
                                                        }
                                                        if(ok)
                                                            monster.learn.push_back(attackVar);
                                                    }
                                                    else
                                                        std::cerr << "Unable to open the xml file: " << file << ", no way to learn " << attackVar.learnSkill << ", level: " << attackVar.learnSkillLevel << " for attack: ?: child->Name(): " << attack->Name() << std::endl;
                                                }
                                                else
                                                    std::cerr << "Unable to open the xml file: " << file << ", level is not a number: child->Name(): " << attack->Name() << std::endl;
                                            }
                                        }
                                        else
                                        {
                                            #ifndef CATCHCHALLENGER_CLASS_MASTER
                                            if(attack->Attribute(XMLCACHEDSTRING_byitem)!=NULL)
                                            {
                                                uint16_t itemId=0;
                                                if(ok)
                                                {
                                                    itemId=stringtouint16(attack->Attribute(XMLCACHEDSTRING_byitem),&ok);
                                                    if(!ok)
                                                        std::cerr << "Unable to open the xml file: " << file << ", item to learn is not a number " << attack->Attribute(XMLCACHEDSTRING_byitem) << ": child->Name(): " << attack->Name() << std::endl;
                                                }
                                                if(ok)
                                                {
                                                    if(items.find(itemId)==items.cend())
                                                    {
                                                        std::cerr << "Unable to open the xml file: " << file << ", item to learn not found " << itemId << ": child->Name(): " << attack->Name() << std::endl;
                                                        ok=false;
                                                    }
                                                }
                                                if(ok)
                                                {
                                                    if(monster.learnByItem.find(itemId)!=monster.learnByItem.cend())
                                                    {
                                                        std::cerr << "Unable to open the xml file: " << file << ", item to learn is already used " << itemId << ": child->Name(): " << attack->Name() << std::endl;
                                                        ok=false;
                                                    }
                                                }
                                                if(ok)
                                                {
                                                    Monster::AttackToLearnByItem tempEntry;
                                                    tempEntry.learnSkill=attackVar.learnSkill;
                                                    tempEntry.learnSkillLevel=attackVar.learnSkillLevel;
                                                    monster.learnByItem[itemId]=tempEntry;
                                                }
                                            }
                                            else
                                            {
                                                std::cerr << "Unable to open the xml file: " << file << ", level and byitem is not found: child->Name(): " << attack->Name() << std::endl;
                                                ok=false;
                                            }
                                            #endif // CATCHCHALLENGER_CLASS_MASTER
                                        }
                                    }
                                    else
                                        std::cerr << "Unable to open the xml file: " << file << ", missing arguements (level or skill): child->Name(): " << attack->Name() << std::endl;
                                    attack = attack->NextSiblingElement(XMLCACHEDSTRING_attack);
                                }
                                std::sort(monster.learn.begin(),monster.learn.end());
                            }
                            else
                                std::cerr << "Unable to open the xml file: " << file << ", have not attack_list: child->Name(): " << item->Name() << std::endl;
                        }
                        #ifndef CATCHCHALLENGER_CLASS_MASTER
                        {
                            const tinyxml2::XMLElement * evolutionsItem = item->FirstChildElement(XMLCACHEDSTRING_evolutions);
                            if(evolutionsItem!=NULL)
                            {
                                const tinyxml2::XMLElement * evolutionItem = evolutionsItem->FirstChildElement(XMLCACHEDSTRING_evolution);
                                while(evolutionItem!=NULL)
                                {
                                    if(evolutionItem->Attribute(XMLCACHEDSTRING_type)!=NULL && (
                                           evolutionItem->Attribute(XMLCACHEDSTRING_level)!=NULL ||
                                           (strcmp(evolutionItem->Attribute(XMLCACHEDSTRING_type),XMLCACHEDSTRING_trade)==0 && evolutionItem->Attribute(XMLCACHEDSTRING_evolveTo)!=NULL) ||
                                           (strcmp(evolutionItem->Attribute(XMLCACHEDSTRING_type),XMLCACHEDSTRING_item)==0 && evolutionItem->Attribute(XMLCACHEDSTRING_item)!=NULL)
                                           )
                                       )
                                    {
                                        ok=true;
                                        Monster::Evolution evolutionVar;
                                        const std::string &typeText=std::string(evolutionItem->Attribute(XMLCACHEDSTRING_type));
                                        if(typeText!=CACHEDSTRING_trade)
                                        {
                                            if(typeText==CACHEDSTRING_item)
                                                evolutionVar.data.item=stringtouint16(evolutionItem->Attribute(XMLCACHEDSTRING_item),&ok);
                                            else//level
                                                evolutionVar.data.level=stringtouint8(evolutionItem->Attribute(XMLCACHEDSTRING_level),&ok);
                                            if(!ok)
                                                std::cerr << "Unable to open the xml file: " << file << ", level is not a number: child->Name(): " << evolutionItem->Name() << std::endl;
                                        }
                                        else
                                        {
                                            evolutionVar.data.level=0;
                                            evolutionVar.data.item=0;
                                        }
                                        if(ok)
                                        {
                                            evolutionVar.evolveTo=stringtouint16(evolutionItem->Attribute(XMLCACHEDSTRING_evolveTo),&ok);
                                            if(!ok)
                                                std::cerr << "Unable to open the xml file: " << file << ", evolveTo is not a number: child->Name(): " << evolutionItem->Name() << std::endl;
                                        }
                                        if(ok)
                                        {
                                            if(typeText==CACHEDSTRING_level)
                                                evolutionVar.type=Monster::EvolutionType_Level;
                                            else if(typeText==CACHEDSTRING_item)
                                                evolutionVar.type=Monster::EvolutionType_Item;
                                            else if(typeText==CACHEDSTRING_trade)
                                                evolutionVar.type=Monster::EvolutionType_Trade;
                                            else
                                            {
                                                ok=false;
                                                std::cerr << "Unable to open the xml file: " << file << ", unknown evolution type: " << typeText << " child->Name(): " << evolutionItem->Name() << std::endl;
                                            }
                                        }
                                        if(ok)
                                        {
                                            if(typeText==CACHEDSTRING_level && (evolutionVar.data.level<0 || evolutionVar.data.level>CATCHCHALLENGER_MONSTER_LEVEL_MAX))
                                            {
                                                ok=false;
                                                std::cerr << "Unable to open the xml file: " << file << ", level out of range: " << evolutionVar.data.level << " child->Name(): " << evolutionItem->Name() << std::endl;
                                            }
                                        }
                                        if(ok)
                                        {
                                            if(typeText==CACHEDSTRING_item)
                                            {
                                                if(items.find(evolutionVar.data.item)==items.cend())
                                                {
                                                    ok=false;
                                                    std::cerr << "Unable to open the xml file: " << file << ", unknown evolution item: " << evolutionVar.data.item << " child->Name(): " << evolutionItem->Name() << std::endl;
                                                }
                                            }
                                        }
                                        if(ok)
                                            monster.evolutions.push_back(evolutionVar);
                                    }
                                    else
                                        std::cerr << "Unable to open the xml file: " << file << ", missing arguements (level or skill): child->Name(): " << evolutionItem->Name() << std::endl;
                                    evolutionItem = evolutionItem->NextSiblingElement(XMLCACHEDSTRING_evolution);
                                }
                            }
                        }
                        #ifdef CATCHCHALLENGER_EXTRA_CHECK
                        if(powerVar==0)
                        {
                            std::cerr << "powerVar==0" << std::endl;
                            abort();
                        }
                        #endif
                        int index=0;
                        while(index<CATCHCHALLENGER_MONSTER_LEVEL_MAX)
                        {
                            uint64_t xp_for_this_level=std::pow(index+1,powerVar);
                            uint64_t xp_for_max_level=monster.xp_for_max_level;
                            uint64_t max_xp=std::pow(CATCHCHALLENGER_MONSTER_LEVEL_MAX,powerVar);
                            uint64_t tempXp=xp_for_this_level*xp_for_max_level/max_xp;
                            if(tempXp<1)
                                tempXp=1;
                            /*#ifdef CATCHCHALLENGER_EXTRA_CHECK
                            if(id==7 && monster.level_to_xp.size()==20)
                                std::cout << "Example level to xp: " << tempXp << std::endl;
                            #endif*/
                            monster.level_to_xp.push_back(static_cast<uint32_t>(tempXp));
                            index++;
                        }
                        #ifdef DEBUG_MESSAGE_MONSTER_XP_LOAD
                        index=0;
                        while(index<CATCHCHALLENGER_MONSTER_LEVEL_MAX)
                        {
                            int give_xp=(monster.give_xp*(index+1))/CATCHCHALLENGER_MONSTER_LEVEL_MAX;
                            std::cerr << "monster: " << id << ", xp " << index+1 << " for the level: " << monster.level_to_xp.at(index) << ", monster for this level: " << monster.level_to_xp.at(index)/give_xp << std::endl;
                            index++;
                        }
                        std::cerr << "monster.level_to_xp.size(): " << monster.level_to_xp.size() << std::endl;
                        #endif

                        #ifndef EPOLLCATCHCHALLENGERSERVERNOGAMESERVER
                        #ifdef CATCHCHALLENGER_EXTRA_CHECK
                        if(monster.give_xp!=0)
                            if((monster.xp_for_max_level*CommonSettingsServer::commonSettingsServer.rates_xp/monster.give_xp)>150)
                                std::cerr << "Warning: you need more than " << monster.xp_for_max_level/monster.give_xp << " monster(s) to pass the last level, prefer do that's with the rate for the monster id: " << id << std::endl;
                        #endif
                        #endif

                        if(monstersMaxId<id)
                            monstersMaxId=id;
                        #endif // CATCHCHALLENGER_CLASS_MASTER
                        monsters[id]=monster;
                    }
                    else
                        std::cerr << "Unable to open the xml file: " << file << ", one of the attribute is wrong or is not a number: child->Name(): " << item->Name() << std::endl;
                }
            }
            else
                std::cerr << "Unable to open the xml file: " << file << ", have not the monster id: child->Name(): " << item->Name() << std::endl;
            item = item->NextSiblingElement(XMLCACHEDSTRING_monster);
        }
        #ifdef EPOLLCATCHCHALLENGERSERVER
        delete domDocument;
        #endif
        file_index++;
    }

    #ifndef CATCHCHALLENGER_CLASS_MASTER
    //check the evolveTo
    auto i = monsters.begin();
    while(i!=monsters.cend())
    {
        unsigned int index=0;
        bool evolutionByLevel=false,evolutionByTrade=false;
        while(index<i->second.evolutions.size())
        {
            std::unordered_set<uint32_t> itemUse;
            if(i->second.evolutions.at(index).type==Monster::EvolutionType_Level)
            {
                if(evolutionByLevel)
                {
                    std::cerr << "The monster " << i->first << " have already evolution by level" << std::endl;
                    i->second.evolutions.erase(i->second.evolutions.cbegin()+index);
                    continue;
                }
                evolutionByLevel=true;
            }
            if(i->second.evolutions.at(index).type==Monster::EvolutionType_Trade)
            {
                if(evolutionByTrade)
                {
                    std::cerr << "The monster " << i->first << " have already evolution by trade" << std::endl;
                    i->second.evolutions.erase(i->second.evolutions.cbegin()+index);
                    continue;
                }
                evolutionByTrade=true;
            }
            if(i->second.evolutions.at(index).type==Monster::EvolutionType_Item)
            {
                if(itemUse.find(i->second.evolutions.at(index).data.item)!=itemUse.cend())
                {
                    std::cerr << "The monster " << i->first << " have already evolution with this item" << std::endl;
                    i->second.evolutions.erase(i->second.evolutions.cbegin()+index);
                    continue;
                }
                itemUse.insert(i->second.evolutions.at(index).data.item);
            }
            if(i->second.evolutions.at(index).evolveTo==i->first)
            {
                std::cerr << "The monster " << i->first << " can't evolve into them self" << std::endl;
                i->second.evolutions.erase(i->second.evolutions.cbegin()+index);
                continue;
            }
            else if(monsters.find(i->second.evolutions.at(index).evolveTo)==monsters.cend())
            {
                std::cerr << "The monster " << i->second.evolutions.at(index).evolveTo << " for the evolution of " << i->first << " can't be found" << std::endl;
                i->second.evolutions.erase(i->second.evolutions.cbegin()+index);
                continue;
            }
            index++;
        }
        ++i;
    }
    #endif

    return monsters;
}

#ifndef EPOLLCATCHCHALLENGERSERVERNOGAMESERVER
std::unordered_map<uint16_t/*item*/, std::unordered_map<uint16_t/*monster*/,uint16_t/*evolveTo*/> > FightLoader::loadMonsterEvolutionItems(const std::unordered_map<uint16_t,Monster> &monsters)
{
    std::unordered_map<uint16_t, std::unordered_map<uint16_t,uint16_t> > evolutionItem;
    auto i=monsters.begin();
    while(i!=monsters.cend())
    {
        unsigned int index=0;
        while(index<i->second.evolutions.size())
        {
            if(i->second.evolutions.at(index).type==Monster::EvolutionType_Item)
                evolutionItem[i->second.evolutions.at(index).data.item][i->first]=i->second.evolutions.at(index).evolveTo;
            index++;
        }
        ++i;
    }
    return evolutionItem;
}
#endif

#ifndef EPOLLCATCHCHALLENGERSERVERNOGAMESERVER
std::unordered_map<uint16_t/*item*/, std::unordered_set<uint16_t/*monster*/> > FightLoader::loadMonsterItemToLearn(const std::unordered_map<uint16_t,Monster> &monsters,const std::unordered_map<uint16_t/*item*/, std::unordered_map<uint16_t/*monster*/,uint16_t/*evolveTo*/> > &evolutionItem)
{
    std::unordered_map<uint16_t/*item*/, std::unordered_set<uint16_t/*monster*/> > learnItem;
    auto i=monsters.cbegin();
    while(i!=monsters.cend())
    {
        auto j=i->second.learnByItem.cbegin();
        while(j!=i->second.learnByItem.cend())
        {
            if(evolutionItem.find(j->first)==evolutionItem.cend())
                learnItem[j->first].insert(i->first);
            else
                std::cerr << "The item " << j->first << " can't be used to learn because already used to evolv" << std::endl;
            ++j;
        }
        ++i;
    }
    return learnItem;
}
#endif

std::vector<PlayerMonster::PlayerSkill> FightLoader::loadDefaultAttack(const uint16_t &monsterId,const uint8_t &level, const std::unordered_map<uint16_t,Monster> &monsters, const std::unordered_map<uint16_t, Skill> &monsterSkills)
{
    std::vector<CatchChallenger::PlayerMonster::PlayerSkill> skills;
    std::vector<CatchChallenger::Monster::AttackToLearn> attack=monsters.at(monsterId).learn;
    unsigned int index=0;
    while(index<attack.size())
    {
        if(attack.at(index).learnAtLevel<=level)
        {
            CatchChallenger::PlayerMonster::PlayerSkill temp;
            temp.level=attack.at(index).learnSkillLevel;
            temp.skill=attack.at(index).learnSkill;
            temp.endurance=0;
            if(monsterSkills.find(temp.skill)!=monsterSkills.cend())
                if(temp.level<=monsterSkills.at(temp.skill).level.size() && temp.level>0)
                    temp.endurance=monsterSkills.at(temp.skill).level.at(temp.level-1).endurance;
            skills.push_back(temp);
        }
        index++;
    }
    while(skills.size()>4)
        skills.erase(skills.begin());
    return skills;
}

#ifndef EPOLLCATCHCHALLENGERSERVERNOGAMESERVER
std::unordered_map<uint16_t,BotFight> FightLoader::loadFight(const std::string &folder, const std::unordered_map<uint16_t,Monster> &monsters, const std::unordered_map<uint16_t, Skill> &monsterSkills, const std::unordered_map<uint16_t, Item> &items,uint16_t &botFightsMaxId)
{
    std::unordered_map<uint16_t,BotFight> botFightList;
    const std::vector<FacilityLibGeneral::InodeDescriptor> &fileList=CatchChallenger::FacilityLibGeneral::listFolderNotRecursive(folder,CatchChallenger::FacilityLibGeneral::ListFolder::Files);
    unsigned int index_file=0;
    while(index_file<fileList.size())
    {
        const std::string &file=fileList.at(index_file).absoluteFilePath;
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
                std::cerr << file+", "+domDocument->ErrorName() << std::endl;
                index_file++;
                continue;
            }
            #ifndef EPOLLCATCHCHALLENGERSERVER
        }
        #endif
        const tinyxml2::XMLElement * root = domDocument->RootElement();
        if(root==NULL)
        {
            index_file++;
            continue;
        }
        if(strcmp(root->Name(),"fights")!=0)
        {
            std::cerr << "Unable to open the xml file: " << file << ", \"fights\" root balise not found for the xml file" << std::endl;
            index_file++;
            continue;
        }

        //load the content
        bool ok;
        const tinyxml2::XMLElement * item = root->FirstChildElement(XMLCACHEDSTRING_fight);
        while(item!=NULL)
        {
            if(item->Attribute(XMLCACHEDSTRING_id)!=NULL)
            {
                const uint16_t &id=stringtouint16(item->Attribute(XMLCACHEDSTRING_id),&ok);
                if(ok)
                {
                    bool entryValid=true;
                    CatchChallenger::BotFight botFight;
                    botFight.cash=0;
                    {
                        const tinyxml2::XMLElement * monster = item->FirstChildElement(XMLCACHEDSTRING_monster);
                        while(entryValid && monster!=NULL)
                        {
                            if(monster->Attribute(XMLCACHEDSTRING_id)==NULL)
                                std::cerr << "Has not attribute \"id\": Name(): " << monster->Name() << std::endl;
                            else
                            {
                                CatchChallenger::BotFight::BotFightMonster botFightMonster;
                                botFightMonster.level=1;
                                botFightMonster.id=stringtouint16(monster->Attribute(XMLCACHEDSTRING_id),&ok);
                                if(ok)
                                {
                                    if(monsters.find(botFightMonster.id)==monsters.cend())
                                    {
                                        entryValid=false;
                                        std::cerr << "Monster not found into the monster list: " << botFightMonster.id << " into the Name(): " << monster->Name() << "), file: " << file << std::endl;
                                        break;
                                    }
                                    if(monster->Attribute(XMLCACHEDSTRING_level)!=NULL)
                                    {
                                        botFightMonster.level=stringtouint8(monster->Attribute(XMLCACHEDSTRING_level),&ok);
                                        if(!ok)
                                        {
                                            std::cerr << "The level is not a number: type: " << monster->Attribute(XMLCACHEDSTRING_type) << " Name(): " << monster->Name() << "), file: " << file << std::endl;
                                            botFightMonster.level=1;
                                        }
                                        if(botFightMonster.level<1)
                                        {
                                            std::cerr << "Can't be 0 or negative: type: " << monster->Attribute(XMLCACHEDSTRING_type) << " Name(): " << monster->Name() << "), file: " << file << std::endl;
                                            botFightMonster.level=1;
                                        }
                                    }
                                    const tinyxml2::XMLElement * attack = monster->FirstChildElement(XMLCACHEDSTRING_attack);
                                    while(entryValid && attack!=NULL)
                                    {
                                        uint8_t attackLevel=1;
                                        if(attack->Attribute(XMLCACHEDSTRING_id)==NULL)
                                            std::cerr << "Has not attribute \"type\": Name(): " << attack->Name() << ", file: " << file << std::endl;
                                        else
                                        {
                                            const uint16_t attackId=stringtouint16(attack->Attribute(XMLCACHEDSTRING_id),&ok);
                                            if(ok)
                                            {
                                                if(monsterSkills.find(attackId)==monsterSkills.cend())
                                                {
                                                    entryValid=false;
                                                    std::cerr << "Monster attack not found: %1 into the Name(): " << attack->Name() << ", file: " << file << std::endl;
                                                    break;
                                                }
                                                if(attack->Attribute(XMLCACHEDSTRING_level)!=NULL)
                                                {
                                                    attackLevel=stringtouint8(attack->Attribute(XMLCACHEDSTRING_level),&ok);
                                                    if(!ok)
                                                    {
                                                        std::cerr << "The level is not a number: Name(): " << attack->Name() << ", file: " << file << std::endl;
                                                        entryValid=false;
                                                        break;
                                                    }
                                                    if(attackLevel<1)
                                                    {
                                                        std::cerr << "Can't be 0 or negative: Name(): " << attack->Name() << ", file: " << file << std::endl;
                                                        entryValid=false;
                                                        break;
                                                    }
                                                }
                                                if(attackLevel>monsterSkills.at(attackId).level.size())
                                                {
                                                    std::cerr << "Level out of range: Name(): " << attack->Name() << ", file: " << file << std::endl;
                                                    entryValid=false;
                                                    break;
                                                }
                                                CatchChallenger::PlayerMonster::PlayerSkill botFightAttack;
                                                botFightAttack.skill=attackId;
                                                botFightAttack.level=attackLevel;
                                                botFightMonster.attacks.push_back(botFightAttack);
                                            }
                                        }
                                        attack = attack->NextSiblingElement(XMLCACHEDSTRING_attack);
                                    }
                                    if(botFightMonster.attacks.empty())
                                        botFightMonster.attacks=loadDefaultAttack(botFightMonster.id,botFightMonster.level,monsters,monsterSkills);
                                    if(botFightMonster.attacks.empty())
                                    {
                                        std::cerr << "Empty attack list: Name(): " << monster->Name() << "), file: " << file << std::endl;
                                        entryValid=false;
                                        break;
                                    }
                                    botFight.monsters.push_back(botFightMonster);
                                }
                            }
                            monster = monster->NextSiblingElement(XMLCACHEDSTRING_monster);
                        }
                    }
                    {
                        const tinyxml2::XMLElement * gain = item->FirstChildElement(XMLCACHEDSTRING_gain);
                        while(entryValid && gain!=NULL)
                        {
                            if(gain->Attribute(XMLCACHEDSTRING_cash)!=NULL)
                            {
                                const uint32_t &cash=stringtouint32(gain->Attribute(XMLCACHEDSTRING_cash),&ok)*CommonSettingsServer::commonSettingsServer.rates_gold;
                                if(ok)
                                    botFight.cash+=cash;
                                else
                                    std::cerr << "Unable to open the xml file: " << file << ", unknow cash text: child->Name(): " << item->Name() << std::endl;
                            }
                            else if(gain->Attribute(XMLCACHEDSTRING_item)!=NULL)
                            {
                                BotFight::Item itemVar;
                                itemVar.quantity=1;
                                itemVar.id=stringtouint16(gain->Attribute(XMLCACHEDSTRING_item),&ok);
                                if(ok)
                                {
                                    if(items.find(itemVar.id)!=items.cend())
                                    {
                                        if(gain->Attribute(XMLCACHEDSTRING_quantity)!=NULL)
                                        {
                                            itemVar.quantity=stringtouint32(gain->Attribute(XMLCACHEDSTRING_quantity),&ok);
                                            if(!ok || itemVar.quantity<1)
                                            {
                                                itemVar.quantity=1;
                                                std::cerr << "Unable to open the xml file: " << file << ", quantity value is wrong: child->Name(): " << item->Name() << std::endl;
                                            }
                                        }
                                        botFight.items.push_back(itemVar);
                                    }
                                    else
                                        std::cerr << "Unable to open the xml file: " << file << ", item not found: child->Name(): " << item->Name() << std::endl;
                                }
                                else
                                    std::cerr << "Unable to open the xml file: " << file << ", unknow item id text: child->Name(): " << item->Name() << std::endl;
                            }
                            else
                                std::cerr << "unknown fight gain: file: " << file << " child->Name(): " << gain->Name() << std::endl;
                            gain = gain->NextSiblingElement(XMLCACHEDSTRING_gain);
                        }
                    }
                    if(entryValid)
                    {
                        if(botFightList.find(id)==botFightList.cend())
                        {
                            if(!botFight.monsters.empty())
                            {
                                if(botFightsMaxId<id)
                                    botFightsMaxId=id;
                                botFightList[id]=botFight;
                            }
                            else
                                std::cerr << "Monster list is empty to open the xml file: " << file << ", id already found: child->Name(): " << item->Name() << std::endl;
                        }
                        else
                            std::cerr << "Unable to open the xml file: " << file << ", id already found: child->Name(): " << item->Name() << std::endl;
                    }
                }
                else
                    std::cerr << "Unable to open the xml file: " << file << ", id is not a number: child->Name(): " << item->Name() << std::endl;
            }
            item = item->NextSiblingElement(XMLCACHEDSTRING_fight);
        }
        #ifdef EPOLLCATCHCHALLENGERSERVER
        delete domDocument;
        #endif
        index_file++;
    }
    return botFightList;
}
#endif

std::unordered_map<uint16_t,Skill> FightLoader::loadMonsterSkill(const std::string &folder
                                                   #ifndef CATCHCHALLENGER_CLASS_MASTER
                                                   , const std::unordered_map<uint8_t, Buff> &monsterBuffs
                                                   , const std::vector<Type> &types
                                                   #endif
                                                   )
{
    std::unordered_map<uint16_t,Skill> monsterSkills;
    const std::vector<FacilityLibGeneral::InodeDescriptor> &fileList=CatchChallenger::FacilityLibGeneral::listFolderNotRecursive(folder,CatchChallenger::FacilityLibGeneral::ListFolder::Files);
    unsigned int file_index=0;
    while(file_index<fileList.size())
    {
        const std::string &file=fileList.at(file_index).absoluteFilePath;
        if(!stringEndsWith(file,CACHEDSTRING_dotxml))
        {
            file_index++;
            continue;
        }
        #ifndef CATCHCHALLENGER_CLASS_MASTER
        std::unordered_map<std::string,uint8_t> typeNameToId;
        {
            uint8_t index=0;
            while(index<types.size())
            {
                typeNameToId[types.at(index).name]=index;
                index++;
            }
        }
        #endif // CATCHCHALLENGER_CLASS_MASTER
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
                std::cerr << file+", "+domDocument->ErrorName() << std::endl;
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
        if(strcmp(root->Name(),"skills")!=0)
        {
            std::cerr << "Unable to open the xml file: " << file << ", \"list\" root balise not found for the xml file" << std::endl;
            file_index++;
            continue;
        }

        //load the content
        bool ok;
        #ifndef CATCHCHALLENGER_CLASS_MASTER
        bool ok2;
        #endif
        const tinyxml2::XMLElement * item = root->FirstChildElement(XMLCACHEDSTRING_skill);
        while(item!=NULL)
        {
            if(item->Attribute(XMLCACHEDSTRING_id)!=NULL)
            {
                uint16_t id=stringtouint16(item->Attribute(XMLCACHEDSTRING_id),&ok);
                if(ok && monsterSkills.find(id)!=monsterSkills.cend())
                    std::cerr << "Unable to open the xml file: " << file << ", id already found: child->Name(): " << item->Name() << std::endl;
                else if(ok)
                {
                    std::unordered_map<uint8_t,Skill::SkillList> levelDef;
                    const tinyxml2::XMLElement * effect = item->FirstChildElement(XMLCACHEDSTRING_effect);
                    if(effect!=NULL)
                    {
                        const tinyxml2::XMLElement * level = effect->FirstChildElement(XMLCACHEDSTRING_level);
                        while(level!=NULL)
                        {
                            if(level->Attribute(XMLCACHEDSTRING_number)!=NULL)
                            {
                                uint32_t sp=0;
                                if(level->Attribute(XMLCACHEDSTRING_sp)!=NULL)
                                {
                                    sp=stringtouint32(level->Attribute(XMLCACHEDSTRING_sp),&ok);
                                    if(!ok)
                                    {
                                        std::cerr << "Unable to open the xml file: " << file << ", sp is not number: child->Name(): " << level->Name() << std::endl;
                                        sp=0;
                                    }
                                }
                                uint8_t endurance=40;
                                if(level->Attribute(XMLCACHEDSTRING_endurance)!=NULL)
                                {
                                    endurance=stringtouint8(level->Attribute(XMLCACHEDSTRING_endurance),&ok);
                                    if(!ok)
                                    {
                                        std::cerr << "Unable to open the xml file: " << file << ", endurance is not number: child->Name(): " << level->Name() << std::endl;
                                        endurance=40;
                                    }
                                    if(endurance<1)
                                    {
                                        std::cerr << "Unable to open the xml file: " << file << ", endurance lower than 1: child->Name(): " << level->Name() << std::endl;
                                        endurance=40;
                                    }
                                }
                                uint8_t number=0;
                                if(ok)
                                    number=stringtouint8(level->Attribute(XMLCACHEDSTRING_number),&ok);
                                if(ok)
                                {
                                    levelDef[number].sp_to_learn=sp;
                                    levelDef[number].endurance=endurance;
                                    if(number>0)
                                    {
                                        #ifndef CATCHCHALLENGER_CLASS_MASTER
                                        {
                                            const tinyxml2::XMLElement * life = level->FirstChildElement(XMLCACHEDSTRING_life);
                                            while(life!=NULL)
                                            {
                                                Skill::Life effect;
                                                if(life->Attribute(XMLCACHEDSTRING_applyOn)!=NULL)
                                                {
                                                    const std::string &applyOn=life->Attribute(XMLCACHEDSTRING_applyOn);
                                                    if(applyOn==CACHEDSTRING_aloneEnemy)
                                                        effect.effect.on=ApplyOn_AloneEnemy;
                                                    else if(applyOn==CACHEDSTRING_themself)
                                                        effect.effect.on=ApplyOn_Themself;
                                                    else if(applyOn==CACHEDSTRING_allEnemy)
                                                        effect.effect.on=ApplyOn_AllEnemy;
                                                    else if(applyOn==CACHEDSTRING_allAlly)
                                                        effect.effect.on=ApplyOn_AllAlly;
                                                    else if(applyOn==CACHEDSTRING_nobody)
                                                        effect.effect.on=ApplyOn_Nobody;
                                                    else
                                                    {
                                                        std::cerr << "Unable to open the xml file: " << file << ", applyOn tag wrong " << life->Attribute(XMLCACHEDSTRING_applyOn) << ": child->Name(): " << item->Name() << std::endl;
                                                        effect.effect.on=ApplyOn_AloneEnemy;
                                                    }
                                                }
                                                else
                                                    effect.effect.on=ApplyOn_AloneEnemy;
                                                std::string text;
                                                if(life->Attribute(XMLCACHEDSTRING_quantity)!=NULL)
                                                    text=life->Attribute(XMLCACHEDSTRING_quantity);
                                                if(stringEndsWith(text,"%"))
                                                    effect.effect.type=QuantityType_Percent;
                                                else
                                                    effect.effect.type=QuantityType_Quantity;
                                                stringreplaceOne(text,"%","");
                                                stringreplaceOne(text,"+","");
                                                effect.effect.quantity=stringtoint32(text,&ok);
                                                effect.success=100;
                                                if(life->Attribute(XMLCACHEDSTRING_success)!=NULL)
                                                {
                                                    std::string success=life->Attribute(XMLCACHEDSTRING_success);
                                                    stringreplaceOne(success,"%","");
                                                    effect.success=stringtouint8(success,&ok2);
                                                    if(!ok2)
                                                    {
                                                        std::cerr << "Unable to open the xml file: " << file << ", success wrong corrected to 100%: child->Name(): " << item->Name() << std::endl;
                                                        effect.success=100;
                                                    }
                                                }
                                                if(ok)
                                                {
                                                    if(effect.effect.quantity!=0)
                                                        levelDef[number].life.push_back(effect);
                                                }
                                                else
                                                    std::cerr << "Unable to open the xml file: " << file << ", " << text << " is not a number: child->Name(): " << item->Name() << std::endl;
                                                life = life->NextSiblingElement(XMLCACHEDSTRING_life);
                                            }
                                        }
                                        #endif // CATCHCHALLENGER_CLASS_MASTER
                                        #ifndef CATCHCHALLENGER_CLASS_MASTER
                                        {
                                            const tinyxml2::XMLElement * buff = level->FirstChildElement(XMLCACHEDSTRING_buff);
                                            while(buff!=NULL)
                                            {
                                                if(buff->Attribute(XMLCACHEDSTRING_id)!=NULL)
                                                {
                                                    const uint8_t idBuff=stringtouint8(buff->Attribute(XMLCACHEDSTRING_id),&ok);
                                                    if(ok)
                                                    {
                                                        Skill::Buff effect;
                                                        if(buff->Attribute(XMLCACHEDSTRING_applyOn)!=NULL)
                                                        {
                                                            const std::string &applyOn=buff->Attribute(XMLCACHEDSTRING_applyOn);
                                                            if(applyOn==CACHEDSTRING_aloneEnemy)
                                                                effect.effect.on=ApplyOn_AloneEnemy;
                                                            else if(applyOn==CACHEDSTRING_themself)
                                                                effect.effect.on=ApplyOn_Themself;
                                                            else if(applyOn==CACHEDSTRING_allEnemy)
                                                                effect.effect.on=ApplyOn_AllEnemy;
                                                            else if(applyOn==CACHEDSTRING_allAlly)
                                                                effect.effect.on=ApplyOn_AllAlly;
                                                            else if(applyOn==CACHEDSTRING_nobody)
                                                                effect.effect.on=ApplyOn_Nobody;
                                                            else
                                                            {
                                                                std::cerr << "Unable to open the xml file: " << file << ", applyOn tag wrong "
                                                                          << buff->Attribute(XMLCACHEDSTRING_applyOn) << ": child->Name(): " << item->Name() << std::endl;
                                                                effect.effect.on=ApplyOn_AloneEnemy;
                                                            }
                                                        }
                                                        else
                                                            effect.effect.on=ApplyOn_AloneEnemy;
                                                        if(monsterBuffs.find(idBuff)==monsterBuffs.cend())
                                                            std::cerr << "Unable to open the xml file: " << file << ", this buff id is not found: " << idBuff << ": child->Name(): " << item->Name() << std::endl;
                                                        else
                                                        {
                                                            effect.effect.level=1;
                                                            ok2=true;
                                                            if(buff->Attribute(XMLCACHEDSTRING_level)!=NULL)
                                                            {
                                                                const std::string &level=buff->Attribute(XMLCACHEDSTRING_level);
                                                                effect.effect.level=stringtouint8(level,&ok2);
                                                                if(!ok2)
                                                                    std::cerr << "Unable to open the xml file: " << file << ", level wrong: " <<
                                                                                 buff->Attribute(XMLCACHEDSTRING_level) << " child->Name(): " << item->Name() << std::endl;
                                                                if(effect.effect.level<=0)
                                                                {
                                                                    ok2=false;
                                                                    std::cerr << "Unable to open the xml file: " << file << ", level need be egal or greater than 1: child->Name(): " << item->Name() << std::endl;
                                                                }
                                                            }
                                                            if(ok2)
                                                            {
                                                                if(monsterBuffs.at(idBuff).level.size()<effect.effect.level)
                                                                    std::cerr << "Unable to open the xml file: " << file << ", level needed: " << effect.effect.level << ", level max found: " << monsterBuffs.at(idBuff).level.size() << ": child->Name(): " << item->Name() << std::endl;
                                                                else
                                                                {
                                                                    effect.effect.buff=idBuff;
                                                                    effect.success=100;
                                                                    if(buff->Attribute(XMLCACHEDSTRING_success)!=NULL)
                                                                    {
                                                                        std::string success=buff->Attribute(XMLCACHEDSTRING_success);
                                                                        stringreplaceOne(success,"%","");
                                                                        effect.success=stringtouint8(success,&ok2);
                                                                        if(!ok2)
                                                                        {
                                                                            std::cerr << "Unable to open the xml file: " << file << ", success wrong corrected to 100%: child->Name(): " << item->Name() << std::endl;
                                                                            effect.success=100;
                                                                        }
                                                                    }
                                                                    levelDef[number].buff.push_back(effect);
                                                                }
                                                            }
                                                        }
                                                    }
                                                    else
                                                        std::cerr << "Unable to open the xml file: " << file << ", have not tag id: child->Name(): " << item->Name() << std::endl;
                                                }
                                                else
                                                    std::cerr << "Unable to open the xml file: " << file << ", have not tag id: child->Name(): " << item->Name() << std::endl;
                                                buff = buff->NextSiblingElement(XMLCACHEDSTRING_buff);
                                            }
                                        }
                                        #endif // CATCHCHALLENGER_CLASS_MASTER
                                    }
                                    else
                                        std::cerr << "Unable to open the xml file: " << file << ", level need be egal or greater than 1: child->Name(): " << item->Name() << std::endl;
                                }
                                else
                                    std::cerr << "Unable to open the xml file: " << file << ", number tag is not a number: child->Name(): " << item->Name() << std::endl;
                            }
                            level = level->NextSiblingElement(XMLCACHEDSTRING_level);
                        }
                        if(levelDef.size()==0)
                            std::cerr << "Unable to open the xml file: " << file << ", 0 level found: child->Name(): " << item->Name() << std::endl;
                        else
                        {
                            monsterSkills[id].type=255;
                            #ifndef CATCHCHALLENGER_CLASS_MASTER
                            if(item->Attribute(XMLCACHEDSTRING_type)!=NULL)
                            {
                                if(typeNameToId.find(item->Attribute(XMLCACHEDSTRING_type))!=typeNameToId.cend())
                                    monsterSkills[id].type=typeNameToId.at(item->Attribute(XMLCACHEDSTRING_type));
                                else
                                    std::cerr << "Unable to open the xml file: " << file << ", type not found: " << item->Attribute(XMLCACHEDSTRING_type) << ": child->Name(): " << item->Name() << std::endl;
                            }
                            #endif // CATCHCHALLENGER_CLASS_MASTER
                        }
                        //order by level to learn
                        uint8_t index=1;
                        while(levelDef.find(index)!=levelDef.cend())
                        {
                            /*if(levelDef.value(index).buff.empty() && levelDef.value(index).life.empty())
                                std::cerr << "Unable to open the xml file: " << file << ", no effect loaded for skill %4 at level %5, missing level to continue: child->Name(): %2 (at line: %3)").arg(file).arg(item->Name()).arg(CATCHCHALLENGER_XMLELENTATLINE(item)).arg(id).arg(index));*/
                            monsterSkills[id].level.push_back(levelDef.at(index));
                            levelDef.erase(index);
                            index++;
                        }
                        if(levelDef.size()>0)
                            std::cerr << "Unable to open the xml file: " << file << ", level up to " << index << " loaded, missing level to continue: child->Name(): " << item->Name() << std::endl;
                    }
                    /*else
                        std::cerr << "Unable to open the xml file: " << file << ", have not effect balise: child->Name(): " << item->Name() << std::endl;*/
                }
                else
                    std::cerr << "Unable to open the xml file: " << file << ", id is not a number: child->Name(): " << item->Name() << std::endl;
            }
            else
                std::cerr << "Unable to open the xml file: " << file << ", have not the skill id: child->Name(): " << item->Name() << std::endl;
            item = item->NextSiblingElement(XMLCACHEDSTRING_skill);
        }
        #ifdef EPOLLCATCHCHALLENGERSERVER
        delete domDocument;
        #endif
        file_index++;
    }

    //check the default attack
    if(monsterSkills.find(0)==monsterSkills.cend())
        std::cerr << "Warning: no default monster attack if no more attack" << std::endl;
    else if(monsterSkills.at(0).level.empty())
    {
        monsterSkills.erase(0);
        std::cerr << "Warning: no level for default monster attack if no more attack" << std::endl;
    }
    else
    {
        #ifndef CATCHCHALLENGER_CLASS_MASTER
        if(monsterSkills.at(0).level.front().life.empty())
        {
            monsterSkills.erase(0);
            std::cerr << "Warning: no life effect for the default attack" << std::endl;
        }
        else
        {
            const Skill &skill=monsterSkills.at(0);
            const Skill::SkillList &skillList=skill.level.front();
            const size_t &list_size=skillList.life.size();
            unsigned int index=0;
            while(index<list_size)
            {
                const Skill::Life &life=skillList.life.at(index);
                if(life.success==100 && life.effect.on==ApplyOn_AloneEnemy && life.effect.quantity<0)
                    break;
                index++;
            }
            if(index==list_size)
            {
                const Skill::Life &life=skillList.life.front();
                monsterSkills.erase(0);
                std::cerr << "Warning: no valid life effect for the default attack (id: 0): success=100%: " << life.success << ", on=ApplyOn_AloneEnemy: " << life.effect.on << ", quantity<0: " << life.effect.quantity << " for skill" << std::endl;
            }
        }
        #endif
    }

    return monsterSkills;
}

#ifndef EPOLLCATCHCHALLENGERSERVERNOGAMESERVER
std::unordered_map<uint8_t,Buff> FightLoader::loadMonsterBuff(const std::string &folder)
{
    std::unordered_map<uint8_t,Buff> monsterBuffs;
    const std::vector<FacilityLibGeneral::InodeDescriptor> &fileList=CatchChallenger::FacilityLibGeneral::listFolderNotRecursive(folder,CatchChallenger::FacilityLibGeneral::ListFolder::Files);
    unsigned int file_index=0;
    while(file_index<fileList.size())
    {
        const std::string &file=fileList.at(file_index).absoluteFilePath;
        if(!stringEndsWith(file,CACHEDSTRING_dotxml))
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
                std::cerr << file+", "+domDocument->ErrorName() << std::endl;
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
        if(strcmp(root->Name(),"buffs")!=0)
        {
            std::cerr << "Unable to open the xml file: " << file << ", \"list\" root balise not found for the xml file" << std::endl;
            file_index++;
            continue;
        }

        //load the content
        bool ok;
        const tinyxml2::XMLElement * item = root->FirstChildElement(XMLCACHEDSTRING_buff);
        while(item!=NULL)
        {
            if(item->Attribute(XMLCACHEDSTRING_id)!=NULL)
            {
                uint8_t id=stringtouint8(item->Attribute(XMLCACHEDSTRING_id),&ok);
                if(ok && monsterBuffs.find(id)!=monsterBuffs.cend())
                    std::cerr << "Unable to open the xml file: " << file << ", id already found: child->Name(): " << item->Name() << std::endl;
                else if(ok)
                {
                    Buff::Duration general_duration=Buff::Duration_ThisFight;
                    uint8_t general_durationNumberOfTurn=0;
                    float general_capture_bonus=1.0;
                    if(item->Attribute(XMLCACHEDSTRING_capture_bonus)!=NULL)
                    {
                       general_capture_bonus=stringtofloat(item->Attribute(XMLCACHEDSTRING_capture_bonus),&ok);
                        if(!ok)
                        {
                            std::cerr << "Unable to open the xml file: " << file << ", capture_bonus is not a number: child->Name(): " << item->Name() << std::endl;
                            general_capture_bonus=1.0;
                        }
                    }
                    if(item->Attribute(XMLCACHEDSTRING_duration)!=NULL)
                    {
                        if(strcmp(item->Attribute(XMLCACHEDSTRING_duration),XMLCACHEDSTRING_Always)==0)
                            general_duration=Buff::Duration_Always;
                        else if(strcmp(item->Attribute(XMLCACHEDSTRING_duration),XMLCACHEDSTRING_NumberOfTurn)==0)
                        {
                            if(item->Attribute(XMLCACHEDSTRING_durationNumberOfTurn)!=NULL)
                            {
                                general_durationNumberOfTurn=stringtouint8(item->Attribute(XMLCACHEDSTRING_durationNumberOfTurn),&ok);
                                if(!ok)
                                {
                                    std::cerr << "Unable to open the xml file: " << file << ", durationNumberOfTurn is not a number: child->Name(): " << item->Name() << std::endl;
                                    general_durationNumberOfTurn=3;
                                }
                                if(general_durationNumberOfTurn<=0)
                                {
                                    std::cerr << "Unable to open the xml file: " << file << ", durationNumberOfTurn is egal to 0: child->Name(): " << item->Name() << std::endl;
                                    general_durationNumberOfTurn=3;
                                }
                            }
                            else
                                general_durationNumberOfTurn=3;
                            general_duration=Buff::Duration_NumberOfTurn;
                        }
                        else if(strcmp(item->Attribute(XMLCACHEDSTRING_duration),XMLCACHEDSTRING_ThisFight)==0)
                            general_duration=Buff::Duration_ThisFight;
                        else
                        {
                            std::cerr << "Unable to open the xml file: " << file << ", attribute duration have wrong value \"" <<
                                         item->Attribute(XMLCACHEDSTRING_duration) << "\" is not a number: child->Name(): " << item->Name() << std::endl;
                            general_duration=Buff::Duration_ThisFight;
                        }
                    }
                    std::unordered_map<uint8_t,Buff::GeneralEffect> levelDef;
                    const tinyxml2::XMLElement * effect = item->FirstChildElement(XMLCACHEDSTRING_effect);
                    if(effect!=NULL)
                    {
                        const tinyxml2::XMLElement * level = effect->FirstChildElement(XMLCACHEDSTRING_level);
                        while(level!=NULL)
                        {
                            if(level->Attribute(XMLCACHEDSTRING_number)!=NULL)
                            {
                                uint8_t number=stringtouint8(level->Attribute(XMLCACHEDSTRING_number),&ok);
                                if(ok)
                                {
                                    if(number>0)
                                    {
                                        Buff::Duration duration=general_duration;
                                        uint8_t durationNumberOfTurn=general_durationNumberOfTurn;
                                        float capture_bonus=general_capture_bonus;
                                        if(item->Attribute(XMLCACHEDSTRING_capture_bonus)!=NULL)
                                        {
                                           capture_bonus=stringtofloat(item->Attribute(XMLCACHEDSTRING_capture_bonus),&ok);
                                            if(!ok)
                                            {
                                                std::cerr << "Unable to open the xml file: " << file << ", capture_bonus is not a number: child->Name(): " << item->Name() << std::endl;
                                                capture_bonus=general_capture_bonus;
                                            }
                                        }
                                        if(item->Attribute(XMLCACHEDSTRING_duration)!=NULL)
                                        {
                                            if(strcmp(item->Attribute(XMLCACHEDSTRING_duration),XMLCACHEDSTRING_Always)==0)
                                                duration=Buff::Duration_Always;
                                            else if(strcmp(item->Attribute(XMLCACHEDSTRING_duration),XMLCACHEDSTRING_NumberOfTurn)==0)
                                            {
                                                if(item->Attribute(XMLCACHEDSTRING_durationNumberOfTurn)!=NULL)
                                                {
                                                    durationNumberOfTurn=stringtouint8(item->Attribute(XMLCACHEDSTRING_durationNumberOfTurn),&ok);
                                                    if(!ok)
                                                    {
                                                        std::cerr << "Unable to open the xml file: " << file << ", durationNumberOfTurn is not a number: child->Name(): " << item->Name() << std::endl;
                                                        durationNumberOfTurn=general_durationNumberOfTurn;
                                                    }
                                                }
                                                else
                                                    durationNumberOfTurn=general_durationNumberOfTurn;
                                                duration=Buff::Duration_NumberOfTurn;
                                            }
                                            else if(strcmp(item->Attribute(XMLCACHEDSTRING_duration),XMLCACHEDSTRING_ThisFight)==0)
                                                duration=Buff::Duration_ThisFight;
                                            else
                                            {
                                                std::cerr << "Unable to open the xml file: " << file << ", attribute duration have wrong value \"" << item->Attribute(XMLCACHEDSTRING_duration) << "\" is not a number: child->Name(): " << item->Name() << std::endl;
                                                duration=general_duration;
                                            }
                                        }
                                        levelDef[number].duration=duration;
                                        levelDef[number].durationNumberOfTurn=durationNumberOfTurn;
                                        levelDef[number].capture_bonus=capture_bonus;



                                        const tinyxml2::XMLElement * inFight = level->FirstChildElement(XMLCACHEDSTRING_inFight);
                                        while(inFight!=NULL)
                                        {
                                            ok=true;
                                            Buff::Effect effect;
                                            std::string text;
                                            if(inFight->Attribute(XMLCACHEDSTRING_hp)!=NULL)
                                            {
                                                text=inFight->Attribute(XMLCACHEDSTRING_hp);
                                                effect.on=Buff::Effect::EffectOn_HP;
                                            }
                                            else if(inFight->Attribute(XMLCACHEDSTRING_defense)!=NULL)
                                            {
                                                text=inFight->Attribute(XMLCACHEDSTRING_defense);
                                                effect.on=Buff::Effect::EffectOn_Defense;
                                            }
                                            else if(inFight->Attribute(XMLCACHEDSTRING_attack)!=NULL)
                                            {
                                                text=inFight->Attribute(XMLCACHEDSTRING_attack);
                                                effect.on=Buff::Effect::EffectOn_Attack;
                                            }
                                            else
                                            {
                                                ok=false;
                                                std::cerr << "Unable to open the xml file: " << file << ", not know attribute balise: child->Name(): "
                                                          << inFight->Name() << std::endl;
                                            }
                                            if(ok)
                                            {
                                                if(stringEndsWith(text,"%"))
                                                    effect.type=QuantityType_Percent;
                                                else
                                                    effect.type=QuantityType_Quantity;
                                                stringreplaceOne(text,"%","");
                                                stringreplaceOne(text,"+","");
                                                effect.quantity=stringtoint32(text,&ok);
                                                if(ok)
                                                    levelDef[number].fight.push_back(effect);
                                                else
                                                    std::cerr << "Unable to open the xml file: " << file << ", \"" << text <<
                                                                 "\" something is wrong, or is not a number, or not into hp or defense balise: child->Name(): "
                                                              << inFight->Name() << std::endl;
                                            }
                                            inFight = inFight->NextSiblingElement(XMLCACHEDSTRING_inFight);
                                        }
                                        const tinyxml2::XMLElement * inWalk = level->FirstChildElement(XMLCACHEDSTRING_inWalk);
                                        while(inWalk!=NULL)
                                        {
                                            if(inWalk->Attribute(XMLCACHEDSTRING_steps)!=NULL)
                                            {
                                                uint32_t steps=stringtouint32(inWalk->Attribute(XMLCACHEDSTRING_steps),&ok);
                                                if(ok)
                                                {
                                                    Buff::EffectInWalk effect;
                                                    effect.steps=steps;
                                                    std::string text;
                                                    if(inWalk->Attribute(XMLCACHEDSTRING_hp)!=NULL)
                                                    {
                                                        text=inWalk->Attribute(XMLCACHEDSTRING_hp);
                                                        effect.effect.on=Buff::Effect::EffectOn_HP;
                                                    }
                                                    else if(inWalk->Attribute(XMLCACHEDSTRING_defense)!=NULL)
                                                    {
                                                        text=inWalk->Attribute(XMLCACHEDSTRING_defense);
                                                        effect.effect.on=Buff::Effect::EffectOn_Defense;
                                                    }
                                                    else
                                                        std::cerr << "Unable to open the xml file: " << file << ", not action found: child->Name(): " << item->Name() << std::endl;
                                                    if(stringEndsWith(text,"%"))
                                                        effect.effect.type=QuantityType_Percent;
                                                    else
                                                        effect.effect.type=QuantityType_Quantity;
                                                    stringreplaceOne(text,"%","");
                                                    stringreplaceOne(text,"+","");
                                                    effect.effect.quantity=stringtoint32(text,&ok);
                                                    if(ok)
                                                        levelDef[number].walk.push_back(effect);
                                                    else
                                                        std::cerr << "Unable to open the xml file: " << file << ", " << text << " is not a number: child->Name(): " << item->Name() << std::endl;
                                                }
                                                else
                                                    std::cerr << "Unable to open the xml file: " << file << ", have not tag steps: child->Name(): " << item->Name() << std::endl;
                                            }
                                            inWalk = inWalk->NextSiblingElement(XMLCACHEDSTRING_inWalk);
                                        }
                                    }
                                    else
                                        std::cerr << "Unable to open the xml file: " << file << ", level need be egal or greater than 1: child->Name(): " << item->Name() << std::endl;
                                }
                                else
                                    std::cerr << "Unable to open the xml file: " << file << ", number tag is not a number: child->Name(): " << item->Name() << std::endl;
                            }
                            level = level->NextSiblingElement(XMLCACHEDSTRING_level);
                        }
                        uint8_t index=1;
                        while(levelDef.find(index)!=levelDef.cend())
                        {
                            /*if(levelDef.value(index).fight.empty() && levelDef.value(index).walk.empty())
                                std::cerr << "Unable to open the xml file: " << file << ", no effect loaded for buff %4 at level %5, missing level to continue: child->Name(): %2 (at line: %3)").arg(file).arg(item->Name()).arg(CATCHCHALLENGER_XMLELENTATLINE(item)).arg(id).arg(index));*/
                            monsterBuffs[id].level.push_back(levelDef.at(index));
                            levelDef.erase(index);
                            index++;
                        }
                        if(levelDef.size()>0)
                            std::cerr << "Unable to open the xml file: " << file << ", level up to " << index << " loaded, missing level to continue: child->Name(): " << item->Name() << std::endl;
                    }
                    else
                        std::cerr << "Unable to open the xml file: " << file << ", have not effet balise: child->Name(): " << item->Name() << std::endl;
                }
                else
                    std::cerr << "Unable to open the xml file: " << file << ", id is not a number: child->Name(): " << item->Name() << std::endl;
            }
            else
                std::cerr << "Unable to open the xml file: " << file << ", have not the buff id: child->Name(): " << item->Name() << std::endl;
            item = item->NextSiblingElement(XMLCACHEDSTRING_buff);
        }
        #ifdef EPOLLCATCHCHALLENGERSERVER
        delete domDocument;
        #endif
        file_index++;
    }
    return monsterBuffs;
}
#endif

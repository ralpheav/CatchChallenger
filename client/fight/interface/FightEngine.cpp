#include "FightEngine.h"
#include "../../../general/base/MoveOnTheMap.h"
#include "../../../general/base/GeneralVariable.h"
#include "../../base/Api_client_real.h"

#include <QDebug>

using namespace Pokecraft;

FightEngine FightEngine::fightEngine;

FightEngine::FightEngine()
{
    resetAll();
}

FightEngine::~FightEngine()
{
}

//return is have random seed to do random step
bool FightEngine::canDoRandomFight(const Map &map,const quint8 &x,const quint8 &y)
{
    if(!wildMonsters.empty())
    {
        qDebug() << QString("map: %1 (%2,%3), is in fight").arg(map.map_file).arg(x).arg(y);
        return false;
    }
    if(Pokecraft::MoveOnTheMap::isGrass(map,x,y) && !map.grassMonster.empty())
        return m_randomSeeds.size()>=POKECRAFT_MIN_RANDOM_TO_FIGHT;
    if(Pokecraft::MoveOnTheMap::isWater(map,x,y) && !map.waterMonster.empty())
        return m_randomSeeds.size()>=POKECRAFT_MIN_RANDOM_TO_FIGHT;
    if(!map.caveMonster.empty())
        return m_randomSeeds.size()>=POKECRAFT_MIN_RANDOM_TO_FIGHT;

    /// no fight in this zone
    qDebug() << QString("map: %1 (%2,%3), no fight in this zone").arg(map.map_file).arg(x).arg(y);
    return true;
}

bool FightEngine::haveRandomFight(const Map &map,const quint8 &x,const quint8 &y)
{
    bool ok;
    if(!wildMonsters.empty())
    {
        qDebug() << QString("error: map: %1 (%2,%3), is in fight").arg(map.map_file).arg(x).arg(y);
        return false;
    }
    if(Pokecraft::MoveOnTheMap::isGrass(map,x,y) && !map.grassMonster.empty())
    {
        if(stepFight_Grass.empty())
        {
            if(m_randomSeeds.size()==0)
            {
                qDebug() << QString("error: no more random seed here, map: %1 (%2,%3), is in fight").arg(map.map_file).arg(x).arg(y);
                return false;
            }
            else
            {
                stepFight_Grass << (getOneSeed()%16);
                qDebug() << QString("next grass monster into: %1").arg(stepFight_Grass.last());
            }
        }
        else
            stepFight_Grass.first()--;
        if(stepFight_Grass.first()<=0)
        {
            stepFight_Grass.removeFirst();

            PlayerMonster monster=getRandomMonster(map.grassMonster,&ok);
            if(ok)
                wildMonsters << monster;
            return ok;
        }
        else
            return false;
    }
    if(Pokecraft::MoveOnTheMap::isWater(map,x,y) && !map.waterMonster.empty())
    {
        if(stepFight_Water.empty())
        {
            if(m_randomSeeds.size()==0)
            {
                qDebug() << QString("error: no more random seed here, map: %1 (%2,%3), is in fight").arg(map.map_file).arg(x).arg(y);
                return false;
            }
            else
            {
                stepFight_Water << (m_randomSeeds.at(0)%16);
                m_randomSeeds.remove(0,1);
                qDebug() << QString("next water monster into: %1").arg(stepFight_Water.last());
            }
        }
        else
            stepFight_Water.first()--;
        if(stepFight_Water.first()<=0)
        {
            stepFight_Water.removeFirst();

            PlayerMonster monster=getRandomMonster(map.waterMonster,&ok);
            if(ok)
                wildMonsters << monster;
            return ok;
        }
        else
            return false;
    }
    if(!map.caveMonster.empty())
    {
        if(stepFight_Cave.empty())
        {
            if(m_randomSeeds.size()==0)
            {
                qDebug() << QString("error: no more random seed here, map: %1 (%2,%3), is in fight").arg(map.map_file).arg(x).arg(y);
                return false;
            }
            else
            {
                stepFight_Cave << (getOneSeed()%16);
                qDebug() << QString("next cave monster into: %1").arg(stepFight_Cave.last());
            }
        }
        else
            stepFight_Cave.first()--;
        if(stepFight_Cave.first()<=0)
        {
            stepFight_Cave.removeFirst();

            PlayerMonster monster=getRandomMonster(map.caveMonster,&ok);
            if(ok)
                wildMonsters << monster;
            return ok;
        }
        else
            return false;
    }

    /// no fight in this zone
    return false;
}

quint8 FightEngine::getOneSeed()
{
    quint8 number=static_cast<quint8>(m_randomSeeds.at(0));
    m_randomSeeds.remove(0,1);
    return number;
}

PlayerMonster FightEngine::getRandomMonster(const QList<MapMonster> &monsterList,bool *ok)
{
    PlayerMonster playerMonster;
    playerMonster.captured_with=0;
    playerMonster.egg_step=0;
    playerMonster.remaining_xp=0;
    playerMonster.sp=0;
    quint8 randomMonsterInt=getOneSeed()%100;
    bool monsterFound=false;
    int index=0;
    while(index<monsterList.size())
    {
        int luck=monsterList.at(index).luck;
        if(randomMonsterInt<luck)
        {
            //it's this monster
            playerMonster.monster=monsterList.at(index).id;
            //select the level
            if(monsterList.at(index).maxLevel==monsterList.at(index).minLevel)
                playerMonster.level=monsterList.at(index).minLevel;
            else
            {
                playerMonster.level=getOneSeed()%(monsterList.at(index).maxLevel-monsterList.at(index).minLevel+1)+monsterList.at(index).minLevel;
            }
            monsterFound=true;
            break;
        }
        else
            randomMonsterInt-=luck;
        index++;
    }
    if(!monsterFound)
    {
        qDebug() << QString("error: no wild monster selected");
        *ok=false;
        playerMonster.monster=0;
        playerMonster.level=0;
        playerMonster.gender=PlayerMonster::Unknown;
        return playerMonster;
    }
    Monster monsterDef=monsters[playerMonster.monster];
    if(monsterDef.ratio_gender>0 && monsterDef.ratio_gender<100)
    {
        qint8 temp_ratio=getOneSeed()%101;
        if(temp_ratio<monsterDef.ratio_gender)
            playerMonster.gender=PlayerMonster::Male;
        else
            playerMonster.gender=PlayerMonster::Female;
    }
    else
    {
        switch(monsterDef.ratio_gender)
        {
            case 0:
                playerMonster.gender=PlayerMonster::Male;
            break;
            case 100:
                playerMonster.gender=PlayerMonster::Female;
            break;
            default:
                playerMonster.gender=PlayerMonster::Unknown;
            break;
        }
    }
    Monster::Stat monsterStat=getStat(monsterDef,playerMonster.level);
    playerMonster.hp=monsterStat.hp;
    index=monsterDef.attack.size()-1;
    while(index>=0 && playerMonster.skills.size()<POKECRAFT_MONSTER_WILD_SKILL_NUMBER)
    {
        if(monsterDef.attack.at(index).level<=playerMonster.level)
            playerMonster.skills << monsterDef.attack.at(index).skill;
        index--;
    }
    *ok=true;
    return playerMonster;
}

bool FightEngine::canDoFight()
{
    return m_canDoFight;
}

bool FightEngine::isInFight()
{
    if(!wildMonsters.empty())
        return true;
    else
        return false;
}

void FightEngine::setPlayerMonster(const QList<PlayerMonster> &playerMonster)
{
    this->playerMonsterList=playerMonster;
    updateCanDoFight();
}

void FightEngine::updateCanDoFight()
{
    m_canDoFight=false;
    int index=0;
    while(index<playerMonsterList.size())
    {
        const PlayerMonster &playerMonsterEntry=playerMonsterList.at(index);
        if(playerMonsterEntry.hp>0 && playerMonsterEntry.egg_step==0)
        {
            selectedMonster=index;
            m_canDoFight=true;
            return;
        }
        index++;
    }
}

QList<PlayerMonster> FightEngine::getPlayerMonster()
{
    return playerMonsterList;
}

PlayerMonster FightEngine::getFightMonster()
{
    return playerMonsterList.at(selectedMonster);
}

PlayerMonster FightEngine::getOtherMonster()
{
    return wildMonsters.first();
}

bool FightEngine::haveOtherMonster()
{
    return !wildMonsters.empty();
}

void FightEngine::resetAll()
{
    monsters.clear();
    monsterSkills.clear();
    monsterBuffs.clear();
    m_randomSeeds.clear();
    playerMonsterList.clear();
    m_canDoFight=false;

    monsterExtra.clear();
    monsterBuffs.clear();
    monsterSkills.clear();
    monsters.clear();
}

void FightEngine::appendRandomSeeds(const QByteArray &data)
{
    m_randomSeeds+=data;
}

/** \warning you need check before the input data */
Monster::Stat FightEngine::getStat(const Monster &monster, const quint8 &level)
{
    Monster::Stat stat=monster.stat;
    stat.attack=stat.attack*level/POKECRAFT_MONSTER_LEVEL_MAX;
    stat.defense=stat.defense*level/POKECRAFT_MONSTER_LEVEL_MAX;
    stat.hp=stat.hp*level/POKECRAFT_MONSTER_LEVEL_MAX;
    stat.special_attack=stat.special_attack*level/POKECRAFT_MONSTER_LEVEL_MAX;
    stat.special_defense=stat.special_defense*level/POKECRAFT_MONSTER_LEVEL_MAX;
    stat.speed=stat.speed*level/POKECRAFT_MONSTER_LEVEL_MAX;
    return stat;
}

const QByteArray FightEngine::randomSeeds()
{
    return m_randomSeeds;
}

bool FightEngine::tryEscape()
{
    Pokecraft::Api_client_real::client->tryEscape();
    quint8 value=getOneSeed()%101;
    if(wildMonsters.first().level<playerMonsterList.at(selectedMonster).level && value<75)
        return true;
    if(wildMonsters.first().level==playerMonsterList.at(selectedMonster).level && value<50)
        return true;
    if(wildMonsters.first().level>playerMonsterList.at(selectedMonster).level && value<25)
        return true;
    return false;
}

bool FightEngine::canDoFightAction()
{
    if(m_randomSeeds.size()>5)
        return true;
    else
        return false;
}
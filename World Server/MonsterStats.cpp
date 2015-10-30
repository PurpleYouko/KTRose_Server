/*
    Rose Online Server Emulator
    Copyright (C) 2006,2007 OSRose Team http://www.dev-osrose.com

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; either version 2
    of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

    depeloped with Main erose/hrose source server + some change from the original eich source
*/

#include "worldmonster.h"

// Get Mob Attack Power
unsigned int CMonster::GetAttackPower()
{
	unsigned int attack;
	attack = thisnpc->atkpower;

	if(IsSummon())//Tomiz : Check and Add The Owner AP To Base Summon Value(STB)
    {
        CPlayer* ownerclient = GetOwner( );
        if(ownerclient!=NULL)
        {
            UINT lvlp = thisnpc->level;
            attack += (long int)floor((double)(ownerclient->Stats->Attack_Power * lvlp) / 100);
            Log(MSG_INFO,"SUMMON: %i Got %i AP from is owner, lvlp: %i",thisnpc->id,(attack - thisnpc->atkpower),lvlp);
        }
    }

    if(thisnpc->weapon!=0)
    {
        attack += GServer->EquipList[8].Index[thisnpc->weapon]->attackpower;
    }
    if(Status->Attack_up!=0xff)
    {
        attack += MagicStatus[Status->Attack_up].Value;
    }
    if(Status->Attack_down!=0xff)
    {
        attack -= MagicStatus[Status->Attack_down].Value;
    }

    if(IsSummon())
    {
        Log(MSG_INFO,"SUMMON: %i Final ATK: %i",thisnpc->id,attack);
    }

	return attack;
}

// Get Monster Defense
unsigned int CMonster::GetDefense()
{
    unsigned int defense;
    defense =  thisnpc->defense;

    if(IsSummon())//Tomiz : Check and Add The Owner DEFF To Base Summon Value(STB)
    {
        CPlayer* ownerclient = GetOwner( );
        if(ownerclient!=NULL)
        {
            UINT lvlp = thisnpc->level;
            defense += (long int)floor((double)(ownerclient->Stats->Defense * lvlp) / 100);
            Log(MSG_INFO,"SUMMON: %i Got %i Deff from is owner, lvlp: %i",thisnpc->id,(defense - thisnpc->defense),lvlp);
        }
    }

    if(thisnpc->subweapon!=0)
    {
        defense += GServer->EquipList[SUBWEAPON].Index[thisnpc->subweapon]->defense;
    }
    if(Status->Defense_up!=0xff)
    {
        defense += MagicStatus[Status->Defense_up].Value;
    }
    if(Status->Defense_down!=0xff)
    {
        defense -= MagicStatus[Status->Defense_down].Value;
    }

    if(IsSummon())
    {
        Log(MSG_INFO,"SUMMON: %i Final DEFF: %i",thisnpc->id,defense);
    }

    return defense;
}

// Get Monster atk speed
unsigned int CMonster::GetAttackSpeed()
{
    unsigned int aspeed;
    aspeed = (unsigned int)thisnpc->atkspeed;

    if(aspeed == 0)// prevent a /0 when hunting dragon egg
    {
        aspeed = 100;
    }

    if(Status->Haste_up!=0xff)
    {
        aspeed += MagicStatus[Status->Haste_up].Value;
    }
    if(Status->Haste_down!=0xff)
    {
        aspeed -= MagicStatus[Status->Haste_down].Value;
    }

    /*if(IsSummon())
    {
        Log(MSG_INFO,"SUMMON: %i Final ASPEED : %i",thisnpc->id,aspeed);
    }*/

    return aspeed;
}

// Get Monster Movement speed
unsigned int CMonster::GetMoveSpeed()
{
    unsigned int mspeed = 0;

    if (thisnpc->stance==mRUNNING)
    {
       mspeed=thisnpc->rspeed;
    }
    else
    {
        mspeed=thisnpc->wspeed;
    }

    if(Status->Dash_up!=0xff)
    {
        mspeed += MagicStatus[Status->Dash_up].Value;
    }
    if(Status->Dash_down!=0xff)
    {
        mspeed -= MagicStatus[Status->Dash_down].Value;
    }

    return mspeed;
}

// return dodge
unsigned int CMonster::GetDodge()
{
    UINT dodge = 0;
    dodge = thisnpc->dodge;

    if(IsSummon())//Tomiz : Check and Add The Owner DODGE To Base Summon Value(STB)
    {
        CPlayer* ownerclient = GetOwner( );
        if(ownerclient!=NULL)
        {
            UINT lvlp = thisnpc->level;
            dodge += (long int)floor((double)(ownerclient->Stats->Dodge * lvlp) / 100);
            Log(MSG_INFO,"SUMMON: %i Got %i Dodge from is owner, lvlp: %i",thisnpc->id,(dodge - thisnpc->dodge),lvlp);
        }
    }

    if(dodge == 0)// prevent a /0 for hawk skill
    {
        dodge = thisnpc->defense;
    }
    if(Status->Dodge_up!=0xff)
    {
        dodge += MagicStatus[Status->Dodge_up].Value;
    }
    if(Status->Dodge_down!=0xff)
    {
        dodge -= MagicStatus[Status->Dodge_down].Value;
    }

    if(IsSummon())
    {
        Log(MSG_INFO,"SUMMON: %i Final DODGE: %i",thisnpc->id,dodge);
    }

    return dodge;
}

// return accury
unsigned int CMonster::GetAccury()
{
    UINT hitrate = 0;
    hitrate = thisnpc->hitrate;

    if(IsSummon())//Tomiz : Check and Add The Owner ACC To Base Summon Value(STB)
    {
        CPlayer* ownerclient = GetOwner( );
        if(ownerclient!=NULL)
        {
            UINT lvlp = thisnpc->level;
            hitrate += (long int)floor((double)(ownerclient->Stats->Accury * lvlp) / 100);
            Log(MSG_INFO,"SUMMON %i Got %i Acc from is owner, lvlp: %i",thisnpc->id,(hitrate - thisnpc->hitrate),lvlp);
        }
    }

    if(Status->Accury_up!=0xff)
    {
        hitrate += MagicStatus[Status->Accury_up].Value;
    }
    if(Status->Accury_down!=0xff)
    {
        hitrate -= MagicStatus[Status->Accury_down].Value;
    }

    if(IsSummon())
    {
        Log(MSG_INFO,"SUMMON: %i Final ACC: %i",thisnpc->id,hitrate);
    }

    return hitrate;
}

// return magic defense
unsigned int CMonster::GetMagicDefense()
{
    UINT mdef = 0;
    mdef = thisnpc->magicdefense;

    if(IsSummon())//Tomiz : Check and Add The Owner MDEFF To Base Summon Value(STB)
    {
        CPlayer* ownerclient = GetOwner( );
        if(ownerclient!=NULL)
        {
            UINT lvlp = thisnpc->level;
            mdef += (long int)floor((double)(ownerclient->Stats->Magic_Defense * lvlp) / 100);
            Log(MSG_INFO,"SUMMON %i Got %i Mdeff from is owner, lvlp: %i",thisnpc->id,(mdef - thisnpc->magicdefense),lvlp);
        }
    }

    if(Status->Magic_Defense_up!=0xff)
    {
        mdef += MagicStatus[Status->Magic_Defense_up].Value;
    }
    if(Status->Magic_Defense_down!=0xff)
    {
        mdef -= MagicStatus[Status->Magic_Defense_down].Value;
    }

    if(IsSummon())
    {
        Log(MSG_INFO,"SUMMON: %i Final MDEFF: %i",thisnpc->id,mdef);
    }

    return mdef;
}

// return max hp
// unsigned int CMonster::GetMaxHP( )
unsigned long long CMonster::GetMaxHP( )
{
    //UINT MaxHP = 0;
    unsigned long long MaxHP = 0;
    MaxHP = (unsigned long long ) thisnpc->hp;

    if(owner==0)
    {
        MaxHP = MaxHP * ((unsigned long long) thisnpc->level);
    }

    if(MaxHP==0)
    {
        MaxHP = (unsigned long long) thisnpc->shp;
    }

    return MaxHP;
}

//LMA: We for the maxHP and HP sometimes
bool CMonster::ForceMaxHP( )
{
    unsigned long long MaxHP = 0;
    MaxHP = (unsigned long long ) thisnpc->hp;
    MaxHP = MaxHP * ((unsigned long long) thisnpc->level);

    if(MaxHP==0)
    {
        MaxHP = (unsigned long long) thisnpc->shp;
    }

    if(MaxHP>MAXHPMOB)
    {
        MaxHP=(unsigned long long) MAXHPMOB;
    }

    Stats->MaxHP=MaxHP;
    Stats->HP=MaxHP;

    return true;
}

// Get Mob Critical
unsigned int CMonster::GetCritical()
{
    return 60;  //60 = 20% of 300 our critical probability
}

// Get Mob Attack Distance
float CMonster::GetAttackDistance()
{
    return thisnpc->atkdistance;
}

//Set Monster Stats Values
bool CMonster::SetStats(bool all)
{
    if(thisnpc==NULL)
    {
        return false;
    }

    if(!IsSummon())//Tomiz : Check Monster lvl if is not a Summon(the one from job, setup to fix normal atk formula because summon got lvl <= 100 in STB)
    {
        Stats->Level=thisnpc->level;
    }

    if (all||Stats->Attack_Power==0)
    {
    	Stats->Attack_Power = GetAttackPower();
    }

    if (all||Stats->Defense==0)
    {
    	Stats->Defense = GetDefense();
    }

    if (all||Stats->Dodge==0)
    {
    	Stats->Dodge = GetDodge();
    }

    if (all||Stats->Accury==0)
    {
    	Stats->Accury = GetAccury();
    }

    if (all||Stats->Magic_Defense==0)
    {
    	Stats->Magic_Defense = GetMagicDefense();
    }

    if (all||Stats->Critical==0)
    {
    	Stats->Critical = GetCritical();
    }

    if (all||Stats->Move_Speed==0)
    {
    	Stats->Move_Speed = GetMoveSpeed();
    }

    if (all||Stats->Attack_Speed==0)
    {
    	Stats->Attack_Speed = GetAttackSpeed();
    }

    if (all||Stats->Attack_Distance==0)
    {
    	Stats->Attack_Distance = GetAttackDistance();
    }

    Stats->MaxHP = GetMaxHP();
    Stats->MaxMP = GetMaxMP();

    /*for(int i=0;i<20;i++)
    {
        AIVar[i] = 0;
    }*/

    return true;
}

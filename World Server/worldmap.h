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
#ifndef __ROSE_MAP__
#define __ROSE_MAP__
#include "worldserver.h"
#include "player.h"
#include "character.h"
#include "worldmonster.h"
#include "datatypes.h"

//LMATEST
class CMonster;

class CMap
{
    public:
        CMap( );
        ~CMap( );

        bool AddPlayer( CPlayer* player );
        bool RemovePlayer( CPlayer* player, bool clearobject=true );

    //CMonster* AddMonster( UINT montype, fPoint position,  UINT owner=0, UINT spawnid=0, bool GetDropData=false );
    CMonster* AddMonster( UINT montype, fPoint position,  UINT owner=0, UINT spawnid=0, UINT aggro = 0, bool IsTD = false, UINT limit = 0, UINT AI = 0);
    bool DeleteMonster( CMonster*, bool clearobject=false, UINT i=0 );
    bool AddDrop( CDrop* drop );
    bool DeleteDrop( CDrop* drop );
    bool AddNPC( CNPC* npc );
    bool DeleteNPC( CNPC* npc );

    CMonster* ConverToMonster( CMonster* monster, UINT newmonttype, bool heal=true );
    CMonster* ConverToMonster( CNPC* npc, UINT newmonttype );

    bool TeleportPlayer( CPlayer* player, fPoint coordold, bool TelePassenger=true );
	bool pakTeleport( CPlayer* player, unsigned int Map, fPoint coord, bool TelePassenger );

    CRespawnPoint* GetNearRespawn( CPlayer* player );
    CRespawnPoint* GetFirstRespawn( );
    CRespawnPoint* GetStartRespawn( );

    CCharacter* GetCharInMap( unsigned int id );
    CPlayer* GetPlayerInMap( UINT id );
    CMonster* GetMonsterInMap( UINT id );
    CPlayer* GetCharIDInMap( DWORD id );
    CDrop* GetDropInMap( UINT id );
    CNPC* GetNPCInMap( UINT id );
    CNPC* GetNPCInMapQSD( UINT id );
    void UpdateTime( );
    void CleanDrops( );
    void RespawnMonster( );
    void UpdateSpawnCounter( UINT montype, UINT spawnid, UINT key );
    bool IsNight( );

    //LMA: nb_summons
    UINT nb_summons;
	//void LoadZon(strings zon);
    //void LoadData(strings zonpath, CStrStb* stbEvent);
    //void AddIfo(strings ifo, CStrStb* stbEvent);

    UINT id;           // id from map
    UINT dayperiod;    // duration of 1 day
    UINT morningtime;  // morning start time
    UINT daytime;      // day start time
    UINT eveningtime;  // evening start time
    UINT nighttime;    // nighttime start time
    UINT ZoneTime;     // how many 6 minute segments since start of the day
    UINT ghost;        // ghosttime?
    BYTE allowpvp;     // pvp allowed?
    bool dungeon;      // Map is a dungeon?
    bool allowpat;     // pat allowed?
    UINT STLID;         //LMA: Stl ID.
    unsigned long QSDzone;      //LMA: Zone transfer trigger
    unsigned long QSDkilling;   //LMA: Killing trigger
    unsigned long QSDDeath;        //LMA: Death trigger

    //LMA BEGIN
    //20070621-211100
    //For CF Mode 1
    UINT is_cf;        //is this a cf map (for jelly beans)...
    UINT id_temp_mon;        //id of the temporary monster (before the jelly bean or another one)
    UINT id_def_mon;        //id of the mon with great Xp
    UINT min_lvl;        //minimum lvl for cf
    UINT mon_lvl;        //lvl of the mob (used for exp calculation according to player's level)
    UINT mon_exp;        //exp of the mob
    UINT percent;        //% of luck summoning a monster
    //LMA END

    // Time values
    UINT MapTime;      // Past time in this map
    UINT Date;
    UINT hour;
    UINT minute;
    UINT ObjVar[20];   //for storing map variables used in AIP
    clock_t LastUpdate;// Last time update in the map
    UINT CurrentTime;  // 0 = morning | 1 = day  | 2 = evening | 3 = night | 4+ = invalid
    //TDMobList
    UINT TDMobList[500];        // List of TD monsters waiting to be spawned
    UINT TDNextSpawn;           // index pointer for next spawn in TDMobList
    UINT TDNextMon;             // index pointer for next addition to TDMobList
    clock_t lastTDSpawnTime;    // time of the last spawn
    UINT TDMobDelay;            // delay between spawns (ms)
    UINT crystalhealth;         // current health of the base crystal
    UINT portalhealth;          // current health of the spawn portal
    UINT mapXPRate;             // exp rate for this map
    UINT mapDropRate;			// Drop rate for map


    vector<CRespawnPoint*>	    RespawnList;	  // RespawnZones List
    vector<CSpawnArea*>	        MonsterSpawnList; // Monster spawn in this map
#ifdef USEIFO
    vector<CMobGroup*>          MobGroupList;     // Spawn "Zones"
#endif
    vector<CMonster*>           MonsterList;      // Monster List in this map
  	vector<CDrop*>			    DropsList;		  // Droped Items
    vector<CPlayer*>            PlayerList;       // Client list in this map;
    vector<CNPC*>               NPCList;          // NPC in this map
    vector<CTeleGate*>		    TeleGateList;	  // Telegates from this map
    vector<CCustomGate*>        CustomGateList;   //Custom telegate list
    vector<CCustomEvent*>       CustomEventList;  //Custom events list
	//vector<SZonPoint*> 			PointList;
    pthread_mutex_t MonsterMutex, DropMutex;
};

#endif

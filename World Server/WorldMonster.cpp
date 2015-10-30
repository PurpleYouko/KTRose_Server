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

// Monster Contructor
CMonster::CMonster( fPoint MonsterPosition, UINT MonsterType, UINT MonsterMap, UINT Owner, UINT spawnid )
{
    drop_dead=false;    //LMA: hasn't drop yet.
    montype = MonsterType;
    SpawnTime = clock( );
    lastSighCheck = clock( );
    MonsterDrop = NULL;    
    Position->source = MonsterPosition;
    Position->current = MonsterPosition;
    Position->destiny = MonsterPosition; 
    Position->Map = MonsterMap;
    Position->respawn = spawnid;
    Position->lastMoveTime = clock( );
	lastLifeUpdate = time(NULL);
    // Magic status (buffs)
    for(unsigned int i=0;i<30;i++)
    {
        MagicStatus[i].Status = 0;
        MagicStatus[i].Buff = 0;
        MagicStatus[i].Value = 0;        
        MagicStatus[i].Duration = 0;    
        MagicStatus[i].BuffTime = 0;                    
    }	      	    
    
    // Monster drop
    MonsterDrop = new MDrop;
    assert(MonsterDrop);
    MonsterDrop->drop = false; 
    MonsterDrop->firsthit = 0;
    MonsterDrop->firstlevel = 0;
    MonsterDrop->mobdrop = NULL;
    MonsterDrop->mapdrop = NULL;
    clientid = GServer->GetNewClientID();
    aip_npctype=0;
    aip_clientid=0;
    sp_aip=0;
    owner = Owner;
    owner_user_id=0;
    thisparty = NULL;
    if(owner==0) CharType = TMONSTER;
    else CharType = TSUMMON;

}

// Monster destructor
CMonster::~CMonster( )
{
    if(MonsterDrop!=NULL);
        delete MonsterDrop;       
    for(UINT i=0;i<PlayersDamage.size();i++)         
        delete PlayersDamage.at(i);
    PlayersDamage.clear();
}

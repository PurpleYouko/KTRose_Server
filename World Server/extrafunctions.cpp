/*
    Rose Online Server Emulator
    Copyright (C) 2006,2007 OSRose Team http://www.osrose.net

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
#include "worldserver.h"

// Build Item Head
unsigned CWorldServer::BuildItemHead( CItem thisitem )
{
	if (thisitem.count==0) return 0;
    return ( ( thisitem.itemnum & 0x7ffffff ) << 5 ) | ( thisitem.itemtype & 0x1f );
}

// Build Item Data
unsigned CWorldServer::BuildItemData( CItem thisitem )
{
	if (thisitem.count==0) return 0;
	if ( thisitem.itemtype >= 10 && thisitem.itemtype <= 13 )
    {
		return thisitem.count;
	}
    else
    {
		unsigned part1 = (thisitem.refine>>4) << 28;
		unsigned part2 = (thisitem.appraised?1:0) << 27;
		unsigned part3 = (thisitem.socketed?1:0) << 26;
		unsigned part4 = (thisitem.lifespan*10) << 16;
		unsigned part5 = thisitem.durability << 9;
		unsigned part6 = thisitem.stats;
		unsigned part7 = thisitem.gem;
		if (!part7==0)part6=0;
		return part1 | part2 | part3 | part4 | part5 | part6 | part7;
	}
}

// Get Item By Head and Data (Coded by Caali)
CItem CWorldServer::GetItemByHeadAndData( unsigned head, unsigned data )
{
    CItem thisitem;

    //Get info from ItemHead
    thisitem.itemnum = (head >> 5) & 0x7ffffff;
    head ^= (thisitem.itemtype & 0x7ffffff) << 5;
    thisitem.itemtype = head & 0x1f;

    //Get info from ItemData
    if( thisitem.itemtype >= 10 && thisitem.itemtype <= 13 ) //Stackable
    {
        thisitem.refine        = 0;
        thisitem.appraised    = 0;
        thisitem.socketed    = 0;
        thisitem.lifespan    = 100;
        thisitem.durability    = 40;
        thisitem.gem        = 0;
        thisitem.stats        = 0;
        thisitem.count        = (int)data;
    }
    else //Non-stackable
    {
        thisitem.refine = (data >> 28) << 4;
        data-=(thisitem.refine>>4) << 28;
        thisitem.appraised = ((data >> 27)==1)?true:false;
        data-=(thisitem.appraised?1:0) << 27;
        thisitem.socketed = ((data >> 26)==1)?true:false;
        data-=(thisitem.socketed?1:0) << 26;
        thisitem.lifespan = (data >> 16) / 10;
        data-=(thisitem.lifespan*10) << 16;
        thisitem.durability = data >> 9;
        data-=thisitem.durability << 9;
        thisitem.gem = data;
        data-=thisitem.gem;
        thisitem.count = 1;
    }
    return thisitem;
}

//----Build Item Show
unsigned CWorldServer::BuildItemShow(CItem thisitem)
{
	if (thisitem.gem == 0)
	{
		unsigned part1 = thisitem.refine*0x10000;
		unsigned part2 = thisitem.itemnum;
		return  part2|part1;
	}
	else
	{
		unsigned part1 = thisitem.refine*0x10000;
		unsigned part2 = thisitem.itemnum;
		unsigned part3 = (0xd0000)+(((thisitem.gem-320)*0x400));
		return  part3|part2|part1;
	}
}


//----Build Item refine
unsigned CWorldServer::BuildItemRefine(CItem thisitem)
{
	if (thisitem.gem == 0)
		return ((thisitem.refine)*256);
	else
	{
		return (0xd00+(thisitem.gem-320)*4)+((thisitem.refine)*256);
	}
}

// Send a packet too all connected clients
void CWorldServer::SendToAll( CPacket* pak )
{
    for(UINT i=0;i<ClientList.size();i++)
    {
        CPlayer* otherclient = (CPlayer*) ClientList.at( i )->player;
		if (otherclient->Session->inGame)
            otherclient->client->SendPacketCpy( pak );
	}
}

// Send a packet too all X who are visible
// -- CLIENT --
void CWorldServer::SendToVisible( CPacket* pak, CPlayer* thisclient, bool dothisclient )
{
	for(unsigned j=0; j<thisclient->VisiblePlayers.size(); j++)
    {
		 thisclient->VisiblePlayers.at( j )->client->SendPacketCpy( pak );
	}
	if(dothisclient)
        thisclient->client->SendPacketCpy( pak );
}

void CWorldServer::SendToVisible( CPacket* pak, CPlayer* thisclient, CPlayer* xotherclient )
{
	for(unsigned j=0; j<thisclient->VisiblePlayers.size(); j++)
    {
		CPlayer* otherclient = thisclient->VisiblePlayers.at( j );
		if(otherclient==xotherclient)
		  continue;
		otherclient->client->SendPacketCpy( pak );
	}
}

// -- MONSTER --
void CWorldServer::SendToVisible( CPacket* pak, CMonster* thismon, CDrop* thisdrop )
{
    CMap* map = MapList.Index[thismon->Position->Map];
    for(UINT i=0;i<map->PlayerList.size();i++)
    {
        CPlayer* otherclient = map->PlayerList.at(i);
		if( IsVisible(otherclient, thismon) )
			otherclient->client->SendPacketCpy( pak );
		if(thisdrop!=NULL)
		{
            otherclient->VisibleDrops.push_back( thisdrop );
        }
	}
}


// -- CHARACTER --
void CWorldServer::SendToVisible( CPacket* pak, CCharacter* character, CDrop* thisdrop )
{
    if(character->IsPlayer( ))
    {
        SendToVisible( pak, (CPlayer*)character );
    }
    else
    if(character->IsMonster( ))
    {
        SendToVisible( pak, (CMonster*)character, thisdrop );
    }
}

// -- DROP --
void CWorldServer::SendToVisible( CPacket* pak, CDrop* thisdrop )
{
    CMap* map = MapList.Index[thisdrop->posMap];
    for(UINT i=0;i<map->PlayerList.size();i++)
    {
        CPlayer* otherclient = map->PlayerList.at(i);
        if (otherclient->client == NULL) continue;
        if (!otherclient->client->isActive) continue;
		if( IsVisible(otherclient, thisdrop) )
			otherclient->client->SendPacketCpy( pak );
	}
}

// Send a packet too all clients on the specified map
void CWorldServer::SendToMap( CPacket* pak, int mapid )
{
    CMap* map = MapList.Index[mapid];
    for(UINT i=0;i<map->PlayerList.size();i++)
    {
        CPlayer* otherclient = map->PlayerList.at(i);
		if( otherclient->Session->inGame )
			otherclient->client->SendPacketCpy( pak );
	}
}

// -----------------------------------------------------------------------------------------
// Check if an object is visible to a client
// -----------------------------------------------------------------------------------------
// -- CLIENT --
bool CWorldServer::IsVisible( CPlayer* thisclient, CPlayer* otherclient )
{
	for(unsigned j=0; j<thisclient->VisiblePlayers.size(); j++)
    {
		if (otherclient==thisclient->VisiblePlayers.at(j))
            return true;
	}
	return false;
}

// -- MOB --
bool CWorldServer::IsVisible( CPlayer* thisclient, CMonster* thismon )
{
	for(unsigned j=0; j<thisclient->VisibleMonsters.size(); j++)
	{
		if (thismon->clientid == thisclient->VisibleMonsters.at(j)) return true;
	}
	return false;
}

// -- DROP --
bool CWorldServer::IsVisible( CPlayer* thisclient, CDrop* thisdrop )
{
	for(unsigned j=0; j<thisclient->VisibleDrops.size(); j++)
	{
		if (thisdrop == thisclient->VisibleDrops.at(j)) return true;
	}
	return false;
}

// -- NPC --
bool CWorldServer::IsVisible( CPlayer* thisclient, CNPC* thisnpc )
{
	for(unsigned j=0; j<thisclient->VisibleNPCs.size(); j++)
	{
		if (thisnpc==thisclient->VisibleNPCs.at(j)) return true;
	}
	return false;
}

// This function gets a new clientID for a npc, monster or mob
unsigned CWorldServer::GetNewClientID( )
{
	for (unsigned i=1; i<0xffff; i++)
    {
		if (ClientIDList[i]!=0 && time(NULL)-ClientIDList[i]>60)
        {//Log( MSG_INFO, "lock id: %i",i);
			ClientIDList[i] = 0;
			return i;
		}
	}
	return 0;
}

// This function will free our clientID
void CWorldServer::ClearClientID( unsigned int id )
{//Log( MSG_INFO, "free id: %i",id);
	ClientIDList[id] = time(NULL);
}

// Search a drop by ID
CDrop* CWorldServer::GetDropByID( UINT id, UINT map )
{
    if(map!=0)
        return MapList.Index[map]->GetDropInMap( id );
	for(unsigned j=0; j<MapList.Map.size(); j++)
    {
		CDrop* thisdrop = MapList.Map.at(j)->GetDropInMap( id );
		if(thisdrop != NULL)
		    return thisdrop;
	}
	return NULL;
}

// Search a Monster by ID
CMonster* CWorldServer::GetMonsterByID( UINT id, UINT map )
{
    if(map!=0)
        return MapList.Index[map]->GetMonsterInMap( id );
    for(UINT i=0;i<MapList.Map.size();i++)
    {
        CMonster* thismon = MapList.Map.at(i)->GetMonsterInMap( id );
        if(thismon!=NULL)
            return thismon;
    }
	return NULL;
}

// Search a Client by Username
CPlayer* CWorldServer::GetClientByUserName( char *username )
{
    for(UINT i=0;i<ClientList.size();i++)
    {
        CPlayer* thisclient = (CPlayer*) ClientList.at(i)->player;
		if (strcmp(thisclient->Session->username,username)==0)
            return thisclient;
	}
	return NULL;
}

// Get Client By ID
CPlayer* CWorldServer::GetClientByID( UINT id, UINT map )
{
    if(map!=0)
        return MapList.Index[map]->GetPlayerInMap( id );
    for(UINT i=0;i<ClientList.size();i++)
    {
        CPlayer* thisclient = (CPlayer*) ClientList.at(i)->player;
		if (thisclient->clientid==id)
            return thisclient;
	}
	return NULL;
}

// Search a Client by CharID
CPlayer* CWorldServer::GetClientByCID( UINT id, UINT map )
{
    if(map!=0)
        return MapList.Index[map]->GetCharIDInMap( id );
    for(UINT i=0;i<ClientList.size();i++)
    {
        CPlayer* thisclient = (CPlayer*) ClientList.at(i)->player;
		if (thisclient->CharInfo->charid==id)
            return thisclient;
	}
	return NULL;
}

// Seach a client by Charname
CPlayer* CWorldServer::GetClientByCharName( char* name  )
{
    for(UINT i=0;i<ClientList.size();i++)
    {
        CPlayer* thisclient = (CPlayer*) ClientList.at(i)->player;
        if (strncmp(thisclient->CharInfo->charname,name, 16)==0)
            return thisclient;
	}
	return NULL;
}

// Get Spawn Area by ID
CSpawnArea* CWorldServer::GetSpawnArea( UINT id, UINT map )
{
    if(map!=0)
    {
    	for(unsigned j=0; j<MapList.Index[map]->MonsterSpawnList.size(); j++)
        {
    		CSpawnArea* thisspawn = MapList.Index[map]->MonsterSpawnList.at(j);
    		if (thisspawn->id==id)
                return thisspawn;
    	}
    }
    else
    {
        for(map=0;map<MapList.Map.size();map++)
        {
        	for(unsigned j=0; j<MapList.Index[map]->MonsterSpawnList.size(); j++)
            {
        		CSpawnArea* thisspawn = MapList.Index[map]->MonsterSpawnList.at(j);
        		if (thisspawn->id==id)
                    return thisspawn;
        	}
        }
    }
	return NULL;
}

#ifdef USEIFO
CMobGroup* CWorldServer::GetMobGroup(UINT id, UINT map )
{
    if (map != 0)
    {
        for (unsigned j = 0; j < MapList.Index[map]->MobGroupList.size(); j++)
        {
            CMobGroup* thisgroup = MapList.Index[map]->MobGroupList.at(j);
            if (thisgroup->id == id)
                return thisgroup;
        }
    }
    else
    {
        for (map = 0; map < MapList.Map.size(); map++)
        {
            for (unsigned j = 0; j < MapList.Index[map]->MobGroupList.size(); j++)
            {
                CMobGroup* thisgroup = MapList.Index[map]->MobGroupList.at(j);
                if (thisgroup->id == id)
                    return thisgroup;
            }
        }
    }
    return NULL;
}
#endif

// delete a spawn
bool CWorldServer::DeleteSpawn( CSpawnArea* spawn )
{
    if(spawn==NULL)
        return false;
    for(UINT i=0;i<MapList.Index[spawn->map]->MonsterSpawnList.size();i++)
    {
        if(MapList.Index[spawn->map]->MonsterSpawnList.at(i)==spawn)
        {
            MapList.Index[spawn->map]->MonsterSpawnList.erase( MapList.Index[spawn->map]->MonsterSpawnList.begin()+i );
            delete spawn;
            return true;
        }
    }
    return false;
}

// Search NPC by ID
CNPC* CWorldServer::GetNPCByID( UINT id, UINT map )
{
    if(map!=0)
        return MapList.Index[map]->GetNPCInMap( id );
	for(unsigned j=0; j<MapList.Map.size(); j++)
    {
		CNPC* thisnpc = MapList.Map.at( j )->GetNPCInMap( id );
		if (thisnpc!=0)
            return thisnpc;
	}
	return NULL;
}

// Get Telegate by ID
CTeleGate* CWorldServer::GetTeleGateByID( unsigned int id )
{
	for(unsigned j=0; j<TeleGateList.size(); j++)
    {
		CTeleGate* thisgate = TeleGateList.at(j);
		if (thisgate->id == id) return thisgate;
	}
	// Hmm, shit, couldent find it
	return NULL;
}

// Get Triangle Area
float CWorldServer::AreaOfTriangle( fPoint p1, fPoint p2, fPoint p3 )
{
	return abs((int)((p2.x * p1.y - p1.x * p2.y) + (p3.x * p2.y - p2.x * p3.y) + (p1.x * p3.y - p2.x * p1.y))) / 2;
}

// Get a random point in triangle area
fPoint CWorldServer::RandInTriangle( fPoint p1, fPoint p2, fPoint p3 )
{
	fPoint thispoint;
	float a = 1.0;
	float b = 1.0;
	while (a+b>1) { a=(float)(rand()*1.0/RAND_MAX); b=(float)(rand()*1.0/RAND_MAX); }
	float c = 1 - a - b;
	thispoint.x = (p1.x*a)+(p2.x*b)+(p3.x*c);
	thispoint.y = (p1.y*a)+(p2.y*b)+(p3.y*c);
	return thispoint;
}

// Get random point in poly
fPoint CWorldServer::RandInPoly( fPoint p[], int pcount )
{
	int tnum = 0;
	float tval = 0;
	float totalarea = 0;
	float* areas = new float[pcount-1];

	for(int i=0; i<pcount-2; i++) {
		totalarea += AreaOfTriangle(p[0], p[i+1], p[i+2]);
		areas[i+1] = totalarea;
	}
	tval = rand() * totalarea / RAND_MAX;
	for (tnum=1; tnum<pcount-1; tnum++) { if(tval<=areas[tnum]&&tval>areas[tnum-1]) break; }

	return RandInTriangle(p[0], p[tnum], p[tnum+1]);
}

// Get Random point in circle
fPoint CWorldServer::RandInCircle( fPoint center, float radius )
{
	fPoint thispoint;
	float angle = (float)(rand() * ( pi * 2 ) / RAND_MAX);
	float dist = (float)sqrt( rand() * 1.0 / RAND_MAX ) * radius;
	thispoint.x = cos(angle) * dist + center.x;
	thispoint.y = sin(angle) * dist + center.y;
	return thispoint;
}

// Check if a Monster is in a Circle around a Client with a specific range
bool CWorldServer::IsMonInCircle( CPlayer* thisclient, CMonster* thismon, float radius )
{
    if ( thisclient->Position->Map!=thismon->Position->Map ) return false;

    float dx = ( thisclient->Position->current.x - thismon->Position->current.x );
    float dy = ( thisclient->Position->current.y - thismon->Position->current.y );

    if ( sqrt( (dx * dx) + (dy * dy) ) <= radius )
    {
        return true;
    }
    else
    {
        return false;
    }
}

// Check if a Monster is in a Circle around a point with a specific radius
bool CWorldServer::IsMonInCircle( fPoint center, fPoint position, float radius )
{
    float dx = ( center.x - position.x );
    float dy = ( center.y - position.y );

    if ( sqrt( (dx * dx) + (dy * dy) ) <= radius )
    {
        return true;
    }
    else
    {
        return false;
    }
}

// Check if a Player is in a Circle around a Client with a specific range
bool CWorldServer::IsPlayerInCircle( CPlayer* thisclient, CPlayer* otherclient, float radius )
{
    if ( thisclient->Position->Map!=otherclient->Position->Map ) return false;

    float dx = ( thisclient->Position->current.x - otherclient->Position->current.x );
    float dy = ( thisclient->Position->current.y - otherclient->Position->current.y );

    if ( sqrt( (dx * dx) + (dy * dy) ) <= radius )
    {
        return true;
    }
    else
    {
        return false;
    }
}

// Search Respawn by ID
CRespawnPoint* CWorldServer::GetRespawnByID( unsigned id )
{
    for(UINT i=0;i<MapList.Map.size();i++)
    {
        CMap* thismap = MapList.Map.at(i);
        for(UINT j=0;j<thismap->RespawnList.size();j++)
        {
            CRespawnPoint* thisrespawn = thismap->RespawnList.at(j);
            if(thisrespawn->id==id)
                return thisrespawn;
        }
    }
	return NULL;
}

// Search Respawn by Map
CRespawnPoint* CWorldServer::GetRespawnByMap( int map )
{
    for(UINT j=0;j<MapList.Index[map]->RespawnList.size();j++)
    {
        if(MapList.Index[map]->RespawnList.at(j)->destMap==map)
            return MapList.Index[map]->RespawnList.at(j);
    }
	return NULL;
}

// Search Skill By ID
CSkills* CWorldServer::GetSkillByID( unsigned int id )
{
    unsigned int A=0,B=0,C=0;
    for(A=0,B=SkillList.size()-1;A<=B;)
    {
        if(A==B)
        {
            CSkills* thisskill = (CSkills*) SkillList.at( A );
            if( id = thisskill->id )// lol this function aint working correct
            {
                //Log(MSG_INFO,"skill returned id = %i", id );
                return thisskill;
            }
            else
            {
                Log(MSG_WARNING,"SKILL NOT FOUNDED! %i", id );
                return NULL;
            }
        }
        C = (A+B)/2;
        CSkills* thisskill = (CSkills*) SkillList.at( C );
        if(thisskill->id == id)
        {
            //Log(MSG_INFO,"skill returned id = %i. C = %i", id,C );
            return thisskill;
        }
        if(thisskill->id > id)
        {
            B=C-1;
        }
        else
        {
            A=C+1;
        }
    }
    Log( MSG_WARNING,"SKILL NOT FOUNDED! %i", id );
    return NULL;
}

// Search Status By ID
CStatus* CWorldServer::GetStatusByID( unsigned int id )
{
    unsigned int A=0,B=0,C=0;
    for(A=0,B=StatusList.size()-1;A<=B;)
    {
        if(A==B)
        {
            CStatus* thisstatus = (CStatus*) StatusList.at( A );
            if( id = thisstatus->id )
                return thisstatus;
            else
            {
                Log(MSG_WARNING,"STATUS NOT FOUND! %i", id );
                return NULL;
            }
        }
        C = (A+B)/2;
        CStatus* thisstatus = (CStatus*) StatusList.at( C );
        if(thisstatus->id == id)
            return thisstatus;
        if(thisstatus->id > id)
            B=C-1;
        else
            A=C+1;
    }
    Log( MSG_WARNING,"STATUS NOT FOUND! %i", id );
    return NULL;
}

// Get Monster Drop By ID
CMDrops* CWorldServer::GetDropData( unsigned int id )
{
    return NULL;
    unsigned int A=0,B=0,C=0;
    for(A=0,B=MDropList.size()-1;A<=B;)
    {
        if(A==B)
        {
            CMDrops* thismdrop = (CMDrops*) MDropList.at( A );
            if( id == thismdrop->id ){return thismdrop;}
            else
            {
                Log(MSG_WARNING,"DROP NOT FOUNDED! %i", id );
                return 0;
            }
        }
        C = (A+B)/2;
        CMDrops* thismdrop = (CMDrops*) MDropList.at( C );
        if(thismdrop->id == id){return thismdrop;}
        if(thismdrop->id > id){B=C-1;}
        else{A=C+1;}
    }
    Log(MSG_WARNING,"DROP NOT FOUNDED! %i", id );
    return NULL;
}

// Get NPC Data by ID
CNPCData* CWorldServer::GetNPCDataByID( unsigned int id )
{
    unsigned int A=0,B=0,C=0;
    for(A=0,B=NPCData.size()-1;A<=B;)
    {
        if(A==B)
        {
            CNPCData* thisnpc = (CNPCData*) NPCData.at( A );
            if( id == thisnpc->id ){return thisnpc;}
            else
            {
                Log(MSG_WARNING,"NPC NOT FOUNDED! %i", id );
                return 0;
            }
        }
        C = (A+B)/2;
        CNPCData* thisnpc = (CNPCData*) NPCData.at( C );
        if(thisnpc->id == id){return thisnpc;}
        if(thisnpc->id > id){B=C-1;}
        else{A=C+1;}
    }
    Log(MSG_WARNING,"NPC NOT FOUNDED! %i", id );
    return NULL;
}

// Get Consumible Item Info
CUseInfo* CWorldServer::GetUseItemInfo(CPlayer* thisclient, unsigned int slot )
{
    if(thisclient->items[slot].count<1)
        return NULL;
    CUseInfo* useitem = new (nothrow) CUseInfo;
    if(useitem==NULL)
    {
        Log(MSG_WARNING, "Error allocing memory GetUseItemInfo");
        return useitem;
    }
    useitem->itemnum = 0;
    useitem->itemtype = 0;
    useitem->usescript = 0;
    useitem->usetype = 0;
    useitem->usevalue = 0;
    unsigned int type = 0;
    useitem->itemnum = thisclient->items[slot].itemnum;
    useitem->itemtype = thisclient->items[slot].itemtype;
    type = UseList.Index[useitem->itemnum]->type;
    switch(type)
    {
        case 311://Medicine
        case 312://Food
        {
            useitem->usescript = 1;
            useitem->usetype = UseList.Index[useitem->itemnum]->useeffect[0];
            useitem->usevalue = UseList.Index[useitem->itemnum]->useeffect[1];
        }
        break;
        case 313://Magic Item [scrolls/summons/fireworks/etc]
        {
            // scrolls [maxhp,attackpower up, ... ]
            if((useitem->itemnum >= 301 && useitem->itemnum <= 323)|| (useitem->itemnum >= 961 && useitem->itemnum <= 964) ||(useitem->itemnum >= 331 && useitem->itemnum <= 340))
            {
                useitem->usescript = 7;
                useitem->usetype = UseList.Index[useitem->itemnum]->useeffect[1];
                useitem->usevalue = 0;
            }
            else//Return Scrolls
            if(useitem->itemnum > 349 && useitem->itemnum < 367)
            {
                if( thisclient->Stats->MP < 33 ){delete useitem;}
                thisclient->Stats->MP -= 32;
                useitem->usescript = 2;
                switch( useitem->itemnum )
                {
                    //Adventure plain [credits to tomiz]
                    case 350:
                        useitem->usetype = 22;
                        useitem->usevalue = 51105310;
                    break;
                    //Zant
                    case 351:
                        useitem->usetype = 1;
                        useitem->usevalue = 52405192;
                    break;
                    //Junon
                    case 352:
                        useitem->usetype = 2;
                        useitem->usevalue = 55095283;
                    break;
                    //Eucar
                    case 353:
                        useitem->usetype = 51;
                        useitem->usevalue = 53665099;
                    break;
                    //Xita
                    case 354:
                        useitem->usetype = 61;
                        useitem->usevalue = 54344607;
                    break;
                    //Goblin cave
                    case 360:
                        useitem->usetype = 31;
                        useitem->usevalue = 55185444;
                    break;
                    //Desert of dead
                    case 361:
                        useitem->usetype = 29;
                        useitem->usevalue = 50825013;
                    break;
                    //El Verloon
                    case 362:
                        useitem->usetype = 24;
                        useitem->usevalue = 55275377;
                    break;
                    //George of Silence
                    case 363:
                        useitem->usetype = 28;
                        useitem->usevalue = 54674783;
                    break;
                    //Shady Jungle
                    case 364:
                        useitem->usetype = 62;
                        useitem->usevalue = 57515196;
                    break;
                    //Sunshine Beach
                    case 366:
                        useitem->usetype = 37;
                        useitem->usevalue = 50055382;
                    break;
                    default:
                    break;
                }
            }
            else //Charm Scrolls
            if( (useitem->itemnum>380 && useitem->itemnum<384) )
            {
                useitem->usescript = 3;
                useitem->usetype = UseList.Index[useitem->itemnum]->useeffect[1];
                thisclient->CharInfo->stamina -= 5000;
            }
            else //Dance Scroll
            if( (useitem->itemnum>370 && useitem->itemnum<377) ||
                (useitem->itemnum>389 && useitem->itemnum<398) )
            {
                useitem->usescript = 12; // should not be treated the same as firecrackers. this is a skill
                useitem->usetype = UseList.Index[useitem->itemnum]->useeffect[1];
            }
            else // Firecrackers
            if( useitem->itemnum == 913 || (useitem->itemnum >= 921 && useitem->itemnum <= 929) ||
                (useitem->itemnum >= 991 && useitem->itemnum <= 995) )
            {
                useitem->usescript = 13;
                useitem->usetype = UseList.Index[useitem->itemnum]->useeffect[1];
                //useitem->usevalue = UseList.Index[useitem->itemnum]->useeffect[1];
            }
            else // Emotions
            if( (useitem->itemnum>970 && useitem->itemnum<979) )
            {
                useitem->usescript = 4;
                useitem->usetype = UseList.Index[useitem->itemnum]->useeffect[0];
                useitem->usevalue = UseList.Index[useitem->itemnum]->useeffect[1];
            }
            else // Summons
            if(useitem->itemnum > 400 && useitem->itemnum < 440)
            {
                if( thisclient->CharInfo->stamina < 100 )
                    return NULL;
                thisclient->CharInfo->stamina -= 100;
                useitem->usescript = 5;
                useitem->usetype = UseList.Index[useitem->itemnum]->useeffect[1];
                CSkills* skill = GetSkillByID(useitem->usetype);
                if(skill == NULL)
                {
                    Log(MSG_INFO,"Useitem returnd a NULL skill");
                    return NULL;
                }
                useitem->usevalue = skill->svalue1;
            }
            else // Summons 2. Special summons such as goodie bags
            if(useitem->itemnum > 908 && useitem->itemnum < 918)
            {
                if( thisclient->CharInfo->stamina < 100 )
                    return NULL;
                thisclient->CharInfo->stamina -= 100;
                useitem->usescript = 5; // testing at 11 for a bit. Gives no owner to the summons.
                useitem->usetype = UseList.Index[useitem->itemnum]->useeffect[1];
                CSkills* skill = GetSkillByID(useitem->usetype);
                if(skill == NULL)
                {
                    Log(MSG_INFO,"Useitem returnd a NULL skill");
                    return NULL;
                }
                useitem->usevalue = skill->svalue1;
            }
            else // Snowball
            if( useitem->itemnum == 326 )
            {
                useitem->usescript = 6;
                useitem->usetype = UseList.Index[useitem->itemnum]->useeffect[0];
                useitem->usevalue = UseList.Index[useitem->itemnum]->useeffect[1];
            }
            else // fairy scroll
            if( useitem->itemnum == 4860 ) // random number. please adjust to the correct id for your scroll
            {
                useitem->usescript = 14; // new category for fairy scroll
                useitem->usetype = UseList.Index[useitem->itemnum]->useeffect[0];
                useitem->usevalue = UseList.Index[useitem->itemnum]->useeffect[1];
            }
            else
            {
                Log( MSG_WARNING, "Unknown Item %i - Type %i",useitem->itemnum,type);
                delete useitem;
                return NULL;
            }
        }
        break;
        case 314://Skill Book
            useitem->usescript = 10;
            useitem->usetype = UseList.Index[useitem->itemnum]->useeffect[0];
            useitem->usevalue = UseList.Index[useitem->itemnum]->useeffect[1];
            break;
        case 315://Repair Tool
            delete useitem;
            return NULL;
            break;
        case 316://Quest Scroll
            delete useitem;
            return NULL;
            break;
        case 317://Engine Fuel
            delete useitem;
            return NULL;
            break;
        case 320://Automatic Consumption
            delete useitem;
            return NULL;
            break;
        case 321://Time Coupon / shop coupon / storage expansion coupon
            useitem->usetype = UseList.Index[useitem->itemnum]->useeffect[0];
            useitem->usevalue = UseList.Index[useitem->itemnum]->useeffect[1];
            switch( useitem->usetype )
            {
                case 96: // shop coupons
                    delete useitem;
                    thisclient->Shop->ShopType = UseList.Index[useitem->itemnum]->quality;
                    break;
                case 94: // free storage coupon
                    break;
                case 95: // storage expansion coupon
                    break;
                case 133: // Exp medals
                    delete useitem;
                    thisclient->Stats->ItemXPRate = 2;
                    if (thisclient->Stats->ItemXPTime == 0) // don't reset timer if it already has time on it
                    {
                        thisclient->Stats->ItemXPTime = clock( ); // set start time for clock countdown
                    }
                    thisclient->Stats->ItemXPTimer += useitem->usevalue * 600000; //convert to seconds and add to time remaining
                    break;
                case 138: // Exp nulifier
                    delete useitem;
                    thisclient->Stats->ItemXPRate = -1; // set to -1 for XP nulification
                    thisclient->Stats->ItemXPTime = clock( ); // set start time for clock countdown. This overwrites any other XP timer
                    thisclient->Stats->ItemXPTimer += useitem->usevalue * 600000; //convert to seconds and add to time remaining
                    break;
                default:
                    break;
            }
            /*switch( useitem->itemnum )
            {
                case 954: //shop coupon A
                {
                    delete useitem;
                    thisclient->Shop->ShopType = 1;
                }
                case 955: //shop coupon B
                {
                    delete useitem;
                    thisclient->Shop->ShopType = 2;
                }
                case 956: //shop coupon C
                {
                    delete useitem;
                    thisclient->Shop->ShopType = 3;
                }
            }
            */
            return NULL;
            break;
        case 322: // gift boxes and stuff
             Log( MSG_WARNING, "Tried to use a gift box");
             break;
        default:
            Log( MSG_WARNING, "Unknown use item type: %i",type);
            return NULL;
            break;
    }
    useitem->itemtype -= 1;
    return useitem;
}

// Look if a skill target is friendly
bool CWorldServer::isSkillTargetFriendly( CSkills* thisskill )
{
	switch(thisskill->target)
    {   //Non-Hostile Target
        case tYourself://yourself
        case tPartyMember://party member
        case tClanMember://clan member
        case tAlly://ally
        case tAllMembers://all members
        case tAllCharacters: // All characters
        case tFaintedAlly://Fainted
            return true;
        break; //Hostile Target
        case tMonster: //monster
        case tHostileCharacter: //hostile character
        case tHostileTeamMember: //hostile team member
        case tHostileMonster: //hostile monster
            return false;
        break;
        default:
            Log(MSG_WARNING,"Unknown skill target %i. Skill id = %i", thisskill->target, thisskill->id);
        break;
    }
    return false;
}

// calc distance
float CWorldServer::distance( fPoint pos1, fPoint pos2 )
{
    float dist = 0;
	float dx = pos1.x - pos2.x;
	float dy = pos1.y - pos2.y;
	dist = sqrt( (dx*dx) + (dy*dy) );
    return dist;
}

// Search client by userid
CPlayer* CWorldServer::GetClientByUserID( UINT userid )
{
    for(UINT i=0;i<ClientList.size();i++)
	{
        CPlayer* thisclient = (CPlayer*)ClientList.at(i)->player;
        if(thisclient->Session->userid==userid)
            return thisclient;
	};
    return NULL;
}

// Check if a slot is valid (to prevent crash)
bool CWorldServer::CheckInventorySlot( CPlayer* thisclient, int slot )
{
    if(slot>=MAX_INVENTORY || slot<0)
    {
        Log(MSG_HACK, "Invalid Inventory slot %i from %s", slot, thisclient->Session->username );
        return false;
    }
    return true;
}

// Return a pseudo random number
UINT CWorldServer::RandNumber( UINT init, UINT range, UINT seed )
{
    if(seed!=0)
        srand(seed);
    if(range>RAND_MAX)
    {
        UINT max = (UINT)GServer->round(sqrt((double)range));
        UINT num1 = init+int(max*rand()/(RAND_MAX + 1.0));
        UINT num2 = init+int(max*rand()/(RAND_MAX + 1.0));
        UINT num3 = init+int(max*rand()/(RAND_MAX + 1.0));
        UINT res = (num1 * num2)+num3;
        return res>range?range:res;
    }
    else
    {
        UINT ranum = init+int(range*rand()/(RAND_MAX + 1.0));
        return ranum;
    }

}

// Check if is a valid item
bool CWorldServer::IsValidItem(UINT type, UINT id )
{
    if(type==0 || id==0 )
        return false;
    if(type<10)
    {
        if(id>4999)
            return false;
        if(EquipList[type].Index[id]->id==0)
            return false;
    }
    else
    {
        switch(type)
        {
            case 10:
                if(id>1999)
                    return false;
                if(UseList.Index[id]->id==0)
                    return false;
            break;
            case 11:
                if(id>3999)
                    return false;
                if(JemList.Index[id]->id==0)
                    return false;
            break;
            case 12:
                if(id>999)
                    return false;
                if(NaturalList.Index[id]->id==00)
                    return false;
            break;
            case 14:
                if(id>999)
                    return false;
                if(PatList.Index[id]->id==00)
                    return false;
            break;
            default:
                return false;
        }
    }
    return true;
}

int CWorldServer::GetIntValue( const char* s , void* var )
{
    char *tmp = strtok( (char*)var, (const char*)s );
    return atoi(tmp==NULL?0:tmp);
}

UINT CWorldServer::GetUIntValue( const char* s , void* var )
{
    char *tmp = strtok( (char*)var, (const char*)s );
    return atoi(tmp==NULL?0:tmp);
}

char* CWorldServer::GetStrValue( const char* s , void* var )
{
    char *tmp = strtok( (char*)var, (const char*)s );
    return tmp;
}

bool CWorldServer::AddParty( CParty* thisparty )
{
    PartyList.push_back( thisparty );
    return true;
}

bool CWorldServer::RemoveParty( CParty* thisparty )
{
    for(UINT i=0;i<PartyList.size( );i++)
    {
        if(PartyList.at(i)==thisparty)
        {
            PartyList.erase(PartyList.begin( )+i );
            return true;
        }
    }
    return false;
}

UINT CWorldServer::GetMatIDfromRecipe (UINT CraftLevel, UINT matType)
{
  int ThisCount = 0;
  for(int i=0;i<GServer->NaturalList.Data.size();i++)
  {
    if(GServer->NaturalList.Index[i]->type == matType)
    {
      ThisCount++;
      if(ThisCount == CraftLevel)
      {
        return GServer->NaturalList.Index[i]->id;
      }
    }
  }
  return 0;
}

// return fairy range for waiting time random
UINT CWorldServer::GetFairyRange( UINT part )
{
	UINT Range1[] = { 5, 5, 4, 3, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };
    UINT Range2[] = { 10, 7, 7, 6, 6, 6, 6, 5, 4, 3, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1 };
	if (part == 0) return Range1[GServer->FairyList.size()];
	if (part == 1) return Range2[GServer->FairyList.size()];
}

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
#include "worldserver.h"

// Build Item Head
unsigned CWorldServer::BuildItemHead( CItem thisitem )
{
	//if (thisitem.count==0) return 0;
	if (thisitem.count==0||thisitem.itemnum==0||thisitem.itemtype==0) return 0;
    return ( ( thisitem.itemnum & 0x7ffffff ) << 5 ) | ( thisitem.itemtype & 0x1f );
}

// Build Item Data
unsigned CWorldServer::BuildItemData( CItem thisitem )
{
	//if (thisitem.count==0) return 0;
	if (thisitem.count==0||thisitem.itemnum==0||thisitem.itemtype==0) return 0;
	if ( thisitem.itemtype >= 10 && thisitem.itemtype <= 13 )
    {
		return thisitem.count;
	}
    else
    {
		unsigned part1 = (thisitem.refine>>4) << 28;
		unsigned part2 = (thisitem.appraised?1:0) << 27;
		unsigned part3 = (thisitem.socketed?1:0) << 26;

		//LMA: special lifespan for PAT, trying to fix a weird bug at loading.
		unsigned part4 = 0;
		if(thisitem.itemtype==14)
		{
		    if (thisitem.sp_value==0&&thisitem.lifespan>0)
		    {
		        thisitem.sp_value=thisitem.lifespan*10;
		    }

		    if(thisitem.sp_value<0||thisitem.sp_value>1000)
		    {
		        thisitem.sp_value=thisitem.lifespan*10;
		    }

		    if(thisitem.sp_value%10==0&&thisitem.sp_value<1000)
		    {
		        part4 = (thisitem.sp_value+1) << 16;
		    }
		    else
		    {
		        part4 = (thisitem.sp_value) << 16;
		    }

		}
		else
		{
		    part4 = (thisitem.lifespan*10) << 16;
		}

		unsigned part5 = thisitem.durability << 9;
		unsigned part6 = thisitem.stats;
		unsigned part7 = thisitem.gem;

		//LMA: fix (we can't have gems and extra stats at the same time).
		//blue stuff can have stats but they are in their STB, NOT as extra stats!
		if(part7!=0)
		{
		    part6=0;
		}

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
        if (otherclient->client == NULL) continue;
        if (!otherclient->client->isActive) continue;

        if (otherclient->Session->inGame)
            otherclient->client->SendPacket( pak );
    }
}

void CWorldServer::SendToVisible( CPacket* pak, CPlayer* thisclient, bool dothisclient )
{
    for(UINT j=0;j<ClientList.size();j++)
    {
        CPlayer* otherclient = (CPlayer*) ClientList.at( j )->player;
        if(otherclient == NULL) continue;
        if(otherclient->client==NULL) continue;
        if(!otherclient->client->isActive) continue;
        if(!otherclient->Session->inGame) continue;

        if( IsVisible(thisclient, otherclient))
            otherclient->client->SendPacket( pak );
    }
    if(dothisclient && thisclient->client!=NULL && thisclient->client->isActive)
        thisclient->client->SendPacket( pak );
}

void CWorldServer::SendToVisible( CPacket* pak, CPlayer* thisclient, CPlayer* xotherclient )
{
    for(UINT j=0;j<ClientList.size();j++)
    {
		CPlayer* otherclient = (CPlayer*) ClientList.at( j )->player;
		if(otherclient == NULL) continue;
        if(otherclient==xotherclient) continue;
        if(otherclient->client==NULL) continue;
        if(!otherclient->client->isActive) continue;
        if(!otherclient->Session->inGame) continue;

        if( IsVisible(thisclient, otherclient))
            otherclient->client->SendPacket( pak );
    }
}

// -- MONSTER --
void CWorldServer::SendToVisible( CPacket* pak, CMonster* thismon, CDrop* thisdrop )
{
    CMap* map = MapList.Index[thismon->Position->Map];
    for(UINT i=0;i<map->PlayerList.size();i++)
    {
        CPlayer* otherclient = map->PlayerList.at(i);
        if(otherclient == NULL) continue;
        if(otherclient->client==NULL) continue;
        if(!otherclient->client->isActive) continue;
        if(!otherclient->Session->inGame) continue;

        if(IsVisible(otherclient,thismon))
            otherclient->client->SendPacket( pak );
        if(thisdrop!=NULL)
        {
            otherclient->VisibleDrops.push_back( thisdrop );
        }
    }
}

//LMA: NPC, special for AIP.
void CWorldServer::SendToVisibleAIP( CPacket* pak, CMonster* thismon, CDrop* thisdrop )
{
    CMap* map = MapList.Index[thismon->Position->Map];
    for(UINT i=0;i<map->PlayerList.size();i++)
    {
        CPlayer* otherclient = map->PlayerList.at(i);
        if(otherclient == NULL) continue;
        if(otherclient->client==NULL) continue;
        if(!otherclient->client->isActive) continue;
        if(!otherclient->Session->inGame) continue;

        //LMA: NPC or monster?
        if(thismon->aip_npctype!=0)
        {
            if(IsVisibleNPCType(otherclient,thismon->aip_npctype))
            {
                otherclient->client->SendPacket( pak );
            }

        }
        else
        {
            if(IsVisible(otherclient,thismon))
            {
                otherclient->client->SendPacket( pak );
            }

        }

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
    if(map == NULL) return;
    for(UINT i=0;i<map->PlayerList.size();i++)
    {
        CPlayer* otherclient = map->PlayerList.at(i);
        if (thisdrop == NULL || otherclient->client==NULL || !otherclient->client->isActive)
            continue;
        if( IsVisible(otherclient, thisdrop) )
            otherclient->client->SendPacket( pak );
    }
}

// Send a packet too all clients on the specified map
void CWorldServer::SendToMap( CPacket* pak, int mapid )
{
    CMap* map = MapList.Index[mapid];
    if(map==NULL) return;
    for(UINT i=0;i<map->PlayerList.size();i++)
    {
        CPlayer* otherclient = map->PlayerList.at(i);
        if (otherclient->client == NULL) continue;
        if (!otherclient->client->isActive) continue;
        if( otherclient->Session->inGame )
            otherclient->client->SendPacket( pak );
    }
}

//LMA: Returns if someone is your ally (union)
bool CWorldServer::IsUnionAlly( int union_player, int union_other_player)
{
    if(union_other_player==0||union_player==0)
    {
        return false;
    }

    if (union_other_player==union_player)
    {
        return true;
    }

    if ((union_player==1||union_player==4)&&((union_other_player==1||union_other_player==4)))
    {
        return true;
    }

    if ((union_player==3||union_player==5)&&((union_other_player==3||union_other_player==5)))
    {
        return true;
    }


    return false;
}

//LMA: Send a packet to all union allies on the specified map
void CWorldServer::SendToUnionInMap( CPacket* pak, int mapid, int union_player)
{
    CMap* map = MapList.Index[mapid];
    if(map==NULL) return;
    for(UINT i=0;i<map->PlayerList.size();i++)
    {
        CPlayer* otherclient = map->PlayerList.at(i);
        if (otherclient->client == NULL) continue;
        if (!otherclient->client->isActive) continue;

        //LMA: special for unions.
        if(!IsUnionAlly(union_player,otherclient->CharInfo->unionid))
        {
            continue;
        }

        if( otherclient->Session->inGame )
            otherclient->client->SendPacket( pak );
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
        //LMATEST
		//if (thismon==thisclient->VisibleMonsters.at(j)) return true;
		if (thismon->clientid==thisclient->VisibleMonsters.at(j)) return true;
	}
	return false;
}

// -- DROP --
bool CWorldServer::IsVisible( CPlayer* thisclient, CDrop* thisdrop )
{
	for(unsigned j=0; j<thisclient->VisibleDrops.size(); j++) {
		if (thisdrop==thisclient->VisibleDrops.at(j)) return true;
	}
	return false;
}

// -- NPC --
bool CWorldServer::IsVisible( CPlayer* thisclient, CNPC* thisnpc )
{
	for(unsigned j=0; j<thisclient->VisibleNPCs.size(); j++) {
		if (thisnpc==thisclient->VisibleNPCs.at(j)) return true;
	}
	return false;
}

//LMA: NPC Type (used for AIP)
bool CWorldServer::IsVisibleNPCType( CPlayer* thisclient, UINT npc_type )
{
	for(unsigned j=0; j<thisclient->VisibleNPCs.size(); j++) {
		if (npc_type==thisclient->VisibleNPCs.at(j)->npctype) return true;
	}
	return false;
}

#ifdef STATICID
// This function gets a new clientID for a npc, monster or mob
//LMA: serial version with a mutex as well...
unsigned CWorldServer::GetNewClientID( )
{
    //LMA: if the thread is already blocked, it should return EBUSY else 0
    UINT res_blockP=pthread_mutex_trylock(&GServer->PlayerMutex);
    UINT res_blockM=pthread_mutex_trylock(&GServer->MapMutex);

	for (unsigned i=last_cid; i<0xffff; i++)
    {
		if (ClientIDList[i]!=0 && time(NULL)-ClientIDList[i]>10)
        {
			ClientIDList[i] = 0;
			//Log(MSG_INFO,"XCID NEW %u",i);
			last_cid=i+1;
            if (res_blockP==0)
            {
                pthread_mutex_unlock( &GServer->PlayerMutex );
            }

            if(res_blockM==0)
            {
                pthread_mutex_unlock( &GServer->MapMutex );
            }

			return i;
		}

	}

	//Not found at the first pass, let's loop again.
	for (unsigned i=1; i<=last_cid; i++)
    {
		if (ClientIDList[i]!=0 && time(NULL)-ClientIDList[i]>10)
        {
			ClientIDList[i] = 0;
			//Log(MSG_INFO,"XCID (2) NEW %u",i);
			last_cid=i+1;
            if (res_blockP==0)
            {
                pthread_mutex_unlock( &GServer->PlayerMutex );
            }

            if(res_blockM==0)
            {
                pthread_mutex_unlock( &GServer->MapMutex );
            }

			return i;
		}

	}

    //TODO: cleaning function there?
    Log(MSG_INFO,"XCID ERR");

    if (res_blockP==0)
    {
        pthread_mutex_unlock( &GServer->PlayerMutex );
    }

    if(res_blockM==0)
    {
        pthread_mutex_unlock( &GServer->MapMutex );
    }


	return 0;
}
#else
// This function gets a new clientID for a npc, monster or mob
unsigned CWorldServer::GetNewClientID( )
{
	for (unsigned i=1; i<0xffff; i++)
    {
		if (ClientIDList[i]!=0 && time(NULL)-ClientIDList[i]>10)
        {
			ClientIDList[i] = 0;
			//Log(MSG_INFO,"XCID NEW %u",i);
			return i;
		}
	}

    Log(MSG_INFO,"XCID ERR");
	return 0;
}
#endif


//LMA: same for Party Id.
#ifdef STATICID
// This function gets a new clientID for a partyID
//LMA: serial version with a mutex as well...
unsigned CWorldServer::GetNewPartyID( )
{
    //LMA: if the thread is already blocked, it should return EBUSY else 0
    UINT res_blockP=pthread_mutex_trylock(&GServer->PlayerMutex);
    UINT res_blockM=pthread_mutex_trylock(&GServer->MapMutex);

	for (unsigned i=last_pid; i<0xffff; i++)
    {
		if (!PartyIDList[i])
        {
			PartyIDList[i] = true;
			last_pid=i+1;
            if (res_blockP==0)
            {
                pthread_mutex_unlock( &GServer->PlayerMutex );
            }

            if(res_blockM==0)
            {
                pthread_mutex_unlock( &GServer->MapMutex );
            }

			return i;
		}

	}

	//Not found at the first pass, let's loop again.
	for (unsigned i=1; i<=last_pid; i++)
    {
		if (!PartyIDList[i])
        {
			PartyIDList[i] = true;
			last_pid=i+1;
            if (res_blockP==0)
            {
                pthread_mutex_unlock( &GServer->PlayerMutex );
            }

            if(res_blockM==0)
            {
                pthread_mutex_unlock( &GServer->MapMutex );
            }

			return i;
		}

	}

    //TODO: cleaning function there?
    Log(MSG_INFO,"PCID ERR");

    if (res_blockP==0)
    {
        pthread_mutex_unlock( &GServer->PlayerMutex );
    }

    if(res_blockM==0)
    {
        pthread_mutex_unlock( &GServer->MapMutex );
    }


	return 0;
}
#else
//LMA: Getting a Party ID.
unsigned CWorldServer::GetNewPartyID( )
{
	for (unsigned i=1; i<0x1000; i++)
    {
		if (!PartyIDList[i])
        {
			PartyIDList[i] = true;
			return i;
		}
	}

    Log(MSG_INFO,"PCID ERR");
	return 0;
}
#endif

/*
//LMATESTCID: test function for CID's
void CWorldServer::CheckClientID(unsigned type, unsigned index)
{
    UINT nb_free=0;

    //0: nb free Cid's
    if (type==0)
    {
        for (unsigned i=1; i<0xffff; i++)
        {
            if (ClientIDList[i]!=0 && time(NULL)-ClientIDList[i]>10)
            {
                nb_free++;
            }
        }

        Log(MSG_WARNING,"CID: Free %u / %u",nb_free,0xFFFFFF-2);
        return;
    }

    //check if a CID's is free or not(simple check).
    if (type==1||type==2)
    {
        if(index<1||index>=0xFFFFFF)
            return;

        if (ClientIDList[index]!=0 && time(NULL)-ClientIDList[index]>10)
        {
            Log(MSG_WARNING,"CID 1/2 %u is available",index);
        }
        else
        {
            Log(MSG_WARNING,"CID 1/2 %u isn't available",index);
        }

    }


	return;
}
*/


// This function will free our clientID
void CWorldServer::ClearClientID( unsigned int id )
{
    if(id<0||id>0xffffff)
    {
        return;
    }

    #ifdef STATICID
    Log(MSG_INFO,"In clear CID %i",id);
    //LMA: if the thread is already blocked, it should return EBUSY
    //else 0
    UINT res_blockP=pthread_mutex_trylock(&GServer->PlayerMutex);
    UINT res_blockM=pthread_mutex_trylock(&GServer->MapMutex);
    #endif

    if(ClientIDList[id]>1)
    {
        Log(MSG_INFO,"CID %i has already been cleared?",id);
    }
    else
    {
        ClientIDList[id] = (unsigned)time(NULL);
    }

	//Log(MSG_INFO,"XCID, clearing CID %u",id);

    #ifdef STATICID
    if (res_blockP==0)
    {
        pthread_mutex_unlock( &GServer->PlayerMutex );
    }

    if(res_blockM==0)
    {
        pthread_mutex_unlock( &GServer->MapMutex );
    }

    Log(MSG_INFO,"Out clear CID %i",id);
    #endif


	return;
}

// Search a drop by ID
CDrop* CWorldServer::GetDropByID( UINT id, UINT map )
{
    if(map!=0)
        return MapList.Index[map]->GetDropInMap( id );
	for(unsigned j=0; j<MapList.Map.size(); j++)
    {
		CDrop* thisdrop = MapList.Map.at(j)->GetDropInMap( id );
		if(thisdrop!=NULL)
		  return thisdrop;
	}
	return NULL;
}

// Search a Chest by ChestID
CChest* CWorldServer::GetChestByID( UINT id )
{
    for(UINT i=0;i<ChestList.size();i++)
    {
        CChest* chest = (CChest*) ChestList.at(i);
    if (chest->chestid == id)
            return chest;
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

        //LMA: no case
		//if (strcmp(thisclient->Session->username,username)==0)
		if (stricmp(thisclient->Session->username,username)==0)
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
CPlayer* CWorldServer::GetClientByCID( DWORD id, UINT map )
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

// Search a client by Charname
CPlayer* CWorldServer::GetClientByCharName( char* name)
{
    for(UINT i=0;i<ClientList.size();i++)
    {
        CPlayer* thisclient = (CPlayer*) ClientList.at(i)->player;
        if (strncmp(thisclient->CharInfo->charname,name, 16)==0)
            return thisclient;
	}
	return NULL;
}


//LMA: Search a client by Charname (Case insensitive search)
CPlayer* CWorldServer::GetClientByCharNameCI( char* name  )
{
    for(UINT i=0;i<ClientList.size();i++)
    {
        CPlayer* thisclient = (CPlayer*) ClientList.at(i)->player;
        if (strnicmp(thisclient->CharInfo->charname,name, 16)==0)
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
CMobGroup* CWorldServer::GetMobGroup(UINT id, UINT map ) {
  if (map != 0) {
    for (unsigned j = 0; j < MapList.Index[map]->MobGroupList.size(); j++) {
      CMobGroup* thisgroup = MapList.Index[map]->MobGroupList.at(j);
      if (thisgroup->id == id)
        return thisgroup;
    }
  } else {
    for (map = 0; map < MapList.Map.size(); map++) {
      for (unsigned j = 0; j < MapList.Index[map]->MobGroupList.size(); j++) {
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
        if(MapList.Index[spawn->map]->MonsterSpawnList.at(i) == spawn)
        {
            MapList.Index[spawn->map]->MonsterSpawnList.erase( MapList.Index[spawn->map]->MonsterSpawnList.begin()+i );
            delete spawn;
            return true;
        }
    }
    return false;
}


// Search NPC by Type
CNPC* CWorldServer::GetNPCByType( UINT npctype )
{
	for(UINT i=0;i<MapList.Map.size();i++)
    {
        CMap* map = GServer->MapList.Map.at(i);
        for(UINT j=0;j<map->NPCList.size();j++)
        {
            CNPC* npc = map->NPCList.at(j);
            if(npc->npctype == npctype)
                return npc;
        }
	}
	return NULL;
}

// Search NPC by ID
CNPC* CWorldServer::GetNPCByID( UINT id, UINT map )
{
    if(map!=0)
    {
        if (map>=MapList.max)
        {
            Log(MSG_WARNING,"Incorrect map %i >= %u in GetNPCByID",map,MapList.max);
            return NULL;
        }

        return MapList.Index[map]->GetNPCInMap( id );
    }

	for(unsigned j=0; j<MapList.Map.size(); j++)
    {
		CNPC* thisnpc = MapList.Map.at( j )->GetNPCInMap( id );
		if (thisnpc!=0)
            return thisnpc;
	}
	return NULL;
}

//LMA: getting the npc name (MySQL).
//This one is npctype!
char* CWorldServer::GetNPCNameByType(UINT id)
{
    if(NpcNameList.find(id)==NpcNameList.end())
    {
        Log(MSG_INFO,"npc %i not found",id);
        return (char*) "";
    }

    //Log(MSG_INFO,"npc %i found %s",id,NpcNameList[id]);


    return NpcNameList[id];
}

//LMA: getting the npc name or monster name by STL.
//We get an ID here, NOT the STL value !
char* CWorldServer::GetSTLMonsterNameByID(UINT idorg)
{
    if (idorg>maxNPC)
    {
        Log(MSG_WARNING,"STLNPCMONSTER, incorrect id %u>%u",idorg,maxNPC);
        return STLNameList[1];
    }

    UINT id=NPCData[idorg]->STLId;

    if(id==0)
    {
        return STLNameList[0];
    }

    UINT idu=15*100000+id;

    if(STLNameList.find(idu)==STLNameList.end())
    {
        Log(MSG_INFO,"[STL] npc or monster STLID %i (Id %i) not found",id,idorg);
        return STLNameList[0];
    }

    //Log(MSG_INFO,"[STL] npc or monster %i found %s",id,STLNameList[idu]);


    return STLNameList[idu];
}

//LMA: getting the zone name by STL.
//We get an ID here, NOT the STL value !
char* CWorldServer::GetSTLZoneNameByID(UINT idorg)
{
    if (idorg>maxZone)
    {
        Log(MSG_WARNING,"STLZONE, incorrect id %u>%u",idorg,maxZone);
        return STLNameList[1];
    }

    UINT id=MapList.Index[idorg]->STLID;

    if(id==0)
    {
        return STLNameList[0];
    }

    UINT idu=19*100000+id;

    if(STLNameList.find(idu)==STLNameList.end())
    {
        Log(MSG_INFO,"[STL] Zone STLID %i (Id %i) not found",id,idorg);
        return STLNameList[0];
    }


    return STLNameList[idu];
}


//LMA: getting a skill name by STL :).
//We get an ID here, NOT the STL value !
char* CWorldServer::GetSTLSkillByID(UINT idorg)
{

    if (idorg>maxSkills)
    {
        Log(MSG_WARNING,"STLSkill, incorrect id %i",idorg);
        return STLNameList[1];
    }

    UINT id=SkillList[idorg]->STLId;
    if(id==0)
    {
        return STLNameList[0];
    }

    UINT idu=17*100000+id;

    if(STLNameList.find(idu)==STLNameList.end())
    {
        Log(MSG_INFO,"[STL] Skill %i not found",id);
        return STLNameList[0];
    }

    //Log(MSG_INFO,"[STL] Skill %i found %s",id,STLNameList[idu]);


    return STLNameList[idu];
}

//LMA: getting a quest name by STL :).
//We get an ID here, NOT the STL value !
char* CWorldServer::GetSTLQuestByID(UINT idorg)
{

    if (idorg>STB_QUEST.rowcount)
    {
        Log(MSG_WARNING,"STLQuest, incorrect id %u>%u",idorg,STB_QUEST.rowcount);
        return STLNameList[1];
    }

    UINT id=STB_QUEST.rows[idorg][4];
    if(id==0)
    {
        return STLNameList[0];
    }

    UINT idu=18*100000+id;

    if(STLNameList.find(idu)==STLNameList.end())
    {
        Log(MSG_INFO,"[STL] Quest %i not found",id);
        return STLNameList[0];
    }

    //Log(MSG_INFO,"[STL] Quest %i found %s",id,STLNameList[idu]);


    return STLNameList[idu];
}


//LMA: We get an item prefix (STL)
char* CWorldServer::GetSTLItemPrefix(int family,UINT idorg)
{
    UINT id=0;


    if(family<=0||family>=15)
    {
        return STLNameList[1];
    }

    //no prefix for them.
    if(family>=10||family==7)
    {
        return (char*) "";
    }

    if(family>=1&&family<=9)
    {
        if(idorg>=EquipList[family].max)
        {
            Log(MSG_WARNING,"STLObject, incorrect object %i:%i",family,idorg);
            return STLNameList[1];
        }

        id=EquipList[family].Index[idorg]->STLPrefix;
    }

    if(id==0)
    {
        return (char*) "";
    }

    UINT idu=16*100000+id;

    if(STLNameList.find(idu)==STLNameList.end())
    {
        Log(MSG_INFO,"[STL] itemprefix %i not found, object %i:%i",id,family,idorg);
        return STLNameList[0];
    }

    //Log(MSG_INFO,"[STL] itemprefix %i found %s",id,STLNameList[idu]);


    return STLNameList[idu];
}

//LMA: getting an object name by STL.
//We get an ID here, NOT the STL value !
char* CWorldServer::GetSTLObjNameByID(UINT family, UINT idorg,bool comment)
{
    UINT id=0;

    if(family<=0||family>=15)
    {
        return STLNameList[1];
    }

    if(family>=1&&family<=9)
    {
        if(idorg>=EquipList[family].max)
        {
            Log(MSG_WARNING,"STLObject, incorrect object %i:%i",family,idorg);
            return STLNameList[1];
        }

        id=EquipList[family].Index[idorg]->STLId;
    }

    if(family==10)
    {
        if(idorg>=UseList.max)
        {
            Log(MSG_WARNING,"STLObject, incorrect object %i:%i",family,idorg);
            return STLNameList[1];
        }

        id=UseList.Index[idorg]->STLId;
    }

    if(family==11)
    {
        if(idorg>=JemList.max)
        {
            Log(MSG_WARNING,"STLObject, incorrect object %i:%i",family,idorg);
            return STLNameList[1];
        }

        id=JemList.Index[idorg]->STLId;
    }

    if(family==12)
    {
        if(idorg>=NaturalList.max)
        {
            Log(MSG_WARNING,"STLObject, incorrect object %i:%i",family,idorg);
            return STLNameList[1];
        }

        id=NaturalList.Index[idorg]->STLId;
    }

    if(family==13)
    {
        if(idorg>=maxQuestItems)
        {
            Log(MSG_WARNING,"STLObject, incorrect object %i:%i",family,idorg);
            return STLNameList[1];
        }

        id=QuestItemData[idorg]->STLId;
    }

    if(family==14)
    {
        if(idorg>=PatList.max)
        {
            Log(MSG_WARNING,"STLObject, incorrect object %i:%i",family,idorg);
            return STLNameList[1];
        }

        id=PatList.Index[idorg]->STLId;
    }

    if(id==0)
    {
        return STLNameList[0];
    }

    UINT idu=family*100000+id;

    if(comment)
    {
        if(GServer->Config.massexport==0)
        {
            return STLNameList[0];
        }

        if(STLCommentList.find(idu)==STLCommentList.end())
        {
            Log(MSG_INFO,"[STL COMMENT] object %i:%i not found",family,id);
            return STLNameList[0];
        }

        return STLCommentList[idu];
    }
    else
    {
        if(STLNameList.find(idu)==STLNameList.end())
        {
            Log(MSG_INFO,"[STL] object %i:%i not found",family,id);
            return STLNameList[0];
        }

    }

    //Log(MSG_INFO,"[STL] object %i:%i found, %s",family,id,STLNameList[idu]);


    return STLNameList[idu];
}

// Get Telegate by ID
//LMA: Sometimes several telegates have the same Id, that's why we're using the offset.
CTeleGate* CWorldServer::GetTeleGateByID( UINT id, UINT offset)
{
	for(unsigned j=0; j<TeleGateList.size(); j++)
    {
		CTeleGate* thisgate = TeleGateList.at(j);
		//if (thisgate->id==id) return thisgate;
		if (thisgate->id==id&&thisgate->offset==offset) return thisgate;
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
	float distance = (float)sqrt( rand() * 1.0 / RAND_MAX ) * radius;
	thispoint.x = cos(angle) * distance + center.x;
	thispoint.y = sin(angle) * distance + center.y;
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
//LMA: new version since we load all skills from STB (even empty ones).
CSkills* CWorldServer::GetSkillByID( unsigned int id )
{
   if (id>=maxSkills)
    {
           Log( MSG_WARNING,"SKILL NOT FOUND! %i>=%i", id,maxSkills);
           return NULL;
    }

    CSkills* thisskill = SkillList[id];

    //stupid test, but you never know...
    if (id!=thisskill->id)
    {
        Log( MSG_WARNING,"SKILL index error! %i!=%i", id,thisskill->id);
        return NULL;
    }


    return thisskill;
}

// Search Status By ID
CStatus* CWorldServer::GetStatusByID( unsigned int id )
{
    if (id>=maxStatus)
    {
           Log( MSG_WARNING,"STATUS NOT FOUND! %i>=%i", id,maxStatus);
           return NULL;
    }

    //stupid test, but you never know...
    CStatus* thisStatus = StatusList[id];
    if (id!=thisStatus->id)
    {
        Log( MSG_WARNING,"STATUS index error! %i!=%i", id,thisStatus->id);
        return NULL;
    }


    return thisStatus;
}


// Get Monster Drop By ID
CMDrops* CWorldServer::GetDropData( unsigned int id )
{
    //LMA: problem is that we use PY's code and ->id is never used???
    return NULL;
}

// Get NPC Data by ID
//LMA: new version since we load all NPC from STB (even empty ones).
CNPCData* CWorldServer::GetNPCDataByID( unsigned int id )
{
    //LMA: Special for "VirtualNPC"
    if (id>10000)
    {
        //For now we only got one :)
        if (id!=WarpGate.virtualNpctypeID)
            return NULL;
        CNPCData* thisnpc = WarpGate.NPCData;
        return thisnpc;
    }

    if (id>=maxNPC)
    {
        Log( MSG_WARNING,"NPC NOT FOUND! %i>=%i", id,maxNPC);
        return NULL;
    }

    CNPCData* thisnpc = (CNPCData*) NPCData[id];

    //stupid test.
    if(id!=thisnpc->id)
    {
        Log( MSG_WARNING,"NPC index error! %i!=%i", id,thisnpc->id);
        return NULL;
    }


    return thisnpc;
}

// Get Consumible Item Info
CUseInfo* CWorldServer::GetUseItemInfo(CPlayer* thisclient, unsigned int slot )
{
    if(thisclient->items[slot].count<1)
    {
        return NULL;
    }

    CUseInfo* useitem = new (nothrow) CUseInfo;
    if(useitem==NULL)
    {
        delete useitem;
        Log(MSG_WARNING, "Error allocing memory GetUseItemInfo");
        return NULL;
    }

    useitem->itemnum = 0;
    useitem->itemtype = 0;
    useitem->usescript = 0;
    useitem->usetype = 0;
    useitem->usevalue = 0;
    useitem->use_buff=0;
    useitem->cooldown_type=0;   //LMA: cooldown handling for food.
    useitem->cooldown=0;   //LMA: cooldown handling for food.
    unsigned int type = 0;
    useitem->itemnum = thisclient->items[slot].itemnum;
    useitem->itemtype = thisclient->items[slot].itemtype;
    type = UseList.Index[useitem->itemnum]->type;

    //LMA TEST
    //forcing some values...
    /*
    useitem->itemnum = 894;
    useitem->itemtype = 10;
    type = 314;
    */
    //END LMA TEST

    Log(MSG_INFO,"%s:: Using item (%i:%i), type %i",thisclient->CharInfo->charname,useitem->itemtype,useitem->itemnum,type);

    switch(type)
    {
        case 311://Medicine
        case 312://Food
        {
            useitem->usescript = 1;
            useitem->usetype = UseList.Index[useitem->itemnum]->useeffect[0];
            useitem->usevalue = UseList.Index[useitem->itemnum]->useeffect[1];
            useitem->cooldown_type=UseList.Index[useitem->itemnum]->cooldown_type;
            useitem->cooldown=UseList.Index[useitem->itemnum]->cooldown;
        }
        break;
        case 313://Magic Item [scrolls/summons/fireworks/etc]
        {
            // scrolls [maxhp,attackpower up, ... ]
            //Arua Blessing added (944, thanks to PY)
            if( (useitem->itemnum>79 && useitem->itemnum<93) ||
                (useitem->itemnum>300 && useitem->itemnum<320) ||
                (useitem->itemnum>330 && useitem->itemnum<340) ||
                 useitem->itemnum == 944 ||
                (useitem->itemnum>960 && useitem->itemnum<967) ||
                (useitem->itemnum>1040 && useitem->itemnum<1047) ||
                 useitem->itemnum == 1305 )
            {
                useitem->usescript = 7;
                useitem->usetype = UseList.Index[useitem->itemnum]->useeffect[1];
                useitem->usevalue = 0;
            }

            else // cherry berrys
            if( (useitem->itemnum >= 981 && useitem->itemnum <= 988) ||
                (useitem->itemnum >= 1035 && useitem->itemnum <= 1039))
            {
                useitem->usescript = 0;
                useitem->usetype = UseList.Index[useitem->itemnum]->useeffect[0]; // don't really need this as it is zero
                useitem->usevalue = UseList.Index[useitem->itemnum]->useeffect[1];
            }

            else //Return Scrolls
            if( (useitem->itemnum>349 && useitem->itemnum<355) ||
                (useitem->itemnum>359 && useitem->itemnum<368) ||
                (useitem->itemnum==945 )  ||
                (useitem->itemnum>1029 && useitem->itemnum<1035) )
            {
                if( thisclient->Stats->MP < 33 )
                {
                    delete useitem;
                    return NULL;
                }
                thisclient->Stats->MP -= 32;
                useitem->usescript = 2;
                switch( useitem->itemnum )
                {
                    //Adventure's Plains - Drop only - [credits to tomiz]
                    case 350:
                        useitem->usetype = 22;
                        useitem->usevalue = 58005200; //Original AP
                        // useitem->usevalue = 51105310; // new AP
                    break;
                    //Zant
                    case 351:
                        useitem->usetype = 1;
                        useitem->usevalue = 52405192;
                    break;
                    //Junon Polis
                    case 352:
                        useitem->usetype = 2;
                        useitem->usevalue = 55095283;
                    break;
                    //Eucar
                    case 353:
                        useitem->usetype = 51;
                        useitem->usevalue = 53665099;
                    break;
                    //Xita Refuge
                    case 354:
                        useitem->usetype = 61;
                        useitem->usevalue = 54344607;
                    break;
                    //Goblin Cave - Mileage Scroll
                    case 360:
                    case 1030:
                        useitem->usetype = 22; //31 Value Before - rl2171
                        useitem->usevalue = 54105040; //55185444 Value Before - rl2171
                    break;
                    //Desert of the Dead - Mileage Scroll
                    case 361:
                    case 1031:
                        useitem->usetype = 29;
                        useitem->usevalue = 51405160; //50825013 Value Before - rl2171
                    break;
                    //El Verloon - Mileage Scroll
                    case 362:
                    case 1032:
                        useitem->usetype = 24;
                        useitem->usevalue = 55205370;
                    break;
                    //George of Silence - Mileage Scroll
                    case 363:
                    case 1033:
                        useitem->usetype = 28;
                        useitem->usevalue = 53005100; //54674783 Value before - rl2171
                    break;
                    //Shady Jungle - Mileage Scroll
                    case 364:
                    case 1034:
                        useitem->usetype = 62;
                        useitem->usevalue = 58405170; //57515196 Value before - rl2171
                    break;
                    //Sikuku Underground Prison - Mileage Scroll - added by rl2171
                    case 365:
                        useitem->usetype = 63;
                        useitem->usevalue = 54004440;
                    break;
                    //Marsh of Ghosts - Mileage Scroll - added by rl2171
                    case 366:
                        useitem->usetype = 64;
                        useitem->usevalue = 50405320;
                    break;
                    //Forgotten Temple B2 - Mileage Scroll - added by rl2171
                    case 367:
                        useitem->usetype = 57;
                        useitem->usevalue = 55405148;
                    break;
                    //Junon's Order Return Scroll - Quest Scroll - added by rl2171
                    case 945:
                        useitem->usetype = 2;
                        useitem->usevalue = 53225100;
                    break;
                }
            }
            else //Charm Scrolls - 381-383 Charm Scrolls
            if (useitem->itemnum>380 && useitem->itemnum<384)
            {
                useitem->usescript = 3;
                useitem->usetype = UseList.Index[useitem->itemnum]->useeffect[1];
            }
            else //Artisan 210-227 Bombs
            if (useitem->itemnum>209 && useitem->itemnum<228)
            {
                //useitem->usescript = 6; //not correct script ?
                useitem->usescript = 16; //LMA: testing AOE.
                useitem->usetype = UseList.Index[useitem->itemnum]->useeffect[0];
                useitem->usevalue = UseList.Index[useitem->itemnum]->useeffect[1];
            }

            else //Dance Scroll
            if( (useitem->itemnum>370 && useitem->itemnum<377) ||
                (useitem->itemnum>389 && useitem->itemnum<401) )
            {
                //LMA: PY's code, different script for dance scrolls
                /*useitem->usescript = 4;
                useitem->usetype = UseList.Index[useitem->itemnum]->useeffect[0];
                useitem->usevalue = UseList.Index[useitem->itemnum]->useeffect[1];*/
                useitem->usescript = 14; // should not be treated the same as firecrackers. this is a skill
                useitem->usetype = UseList.Index[useitem->itemnum]->useeffect[1];
            }
            else // Firecrackers
            if( useitem->itemnum==913 ||
                (useitem->itemnum>919 && useitem->itemnum<935) ||
                (useitem->itemnum>990 && useitem->itemnum<996) )
            {
                useitem->usescript = 4;
                useitem->usetype = UseList.Index[useitem->itemnum]->useeffect[0];
                useitem->usevalue = UseList.Index[useitem->itemnum]->useeffect[1];
            }
            else // Emotions
            if( (useitem->itemnum>970 && useitem->itemnum<979) ||
                (useitem->itemnum>917 && useitem->itemnum<920) ||
                (useitem->itemnum>599 && useitem->itemnum<609) )  //918 - Love Chocolate  919 - Kiss Scroll
            {
                useitem->usescript = 4;
                useitem->usetype = UseList.Index[useitem->itemnum]->useeffect[0];
                useitem->usevalue = UseList.Index[useitem->itemnum]->useeffect[1];
            }
            else // Summons
            if( (useitem->itemnum>400 && useitem->itemnum<440) ||
                (useitem->itemnum==496) || (useitem->itemnum==594) ||
                (useitem->itemnum>915 && useitem->itemnum<918) ||
                (useitem->itemnum>939 && useitem->itemnum<943) ||
                (useitem->itemnum==1055) )
            {
                if( thisclient->CharInfo->stamina<101 )
                    return NULL;
                thisclient->CharInfo->stamina -= 100;
                useitem->usescript = 5;
                useitem->usetype = UseList.Index[useitem->itemnum]->useeffect[1];
                if(useitem->itemnum==439){useitem->usevalue = 965;} // Metal Moldie
//                else if(useitem->itemnum==609){useitem->usevalue = 2047;} // Event Metal Moldie
                else if(useitem->itemnum==916){useitem->usevalue = 294;} //Firecracker Penguin
                else if(useitem->itemnum==917){useitem->usevalue = 297;} // Ice Spirit
                else if(useitem->itemnum==940){useitem->usevalue = 994;} // Easter Bunny
                else if(useitem->itemnum==941){useitem->usevalue = 995;} // Easter Egg
                else if(useitem->itemnum==942){useitem->usevalue = 1472;} // Soccer Ball Pet
                else if(useitem->itemnum==1055){useitem->usevalue = 251;} //Candle Ghost Pet
                else if(useitem->itemnum==594)
                {
                     // Lucky Ghost
                     useitem->usevalue = 941;
                     useitem->use_buff = UseList.Index[useitem->itemnum]->useeffect[0];
                }
                else if(useitem->itemnum==496) {useitem->usevalue = 992;} // Christmas Tree
//                else if(useitem->itemnum==943) {useitem->usevalue;} // Event Scroll
//                else if(useitem->itemnum==944){useitem->usevalue = 172;} // Arua's Blessing
                else{ useitem->usevalue = useitem->itemnum + 500; }
            }
            else // Snowball & Rock & Throwing Ball
            if( ( useitem->itemnum==326 ) || ( useitem->itemnum==327 ) || ( useitem->itemnum==937 ) )
            {
                useitem->usescript = 6;
                useitem->usetype = UseList.Index[useitem->itemnum]->useeffect[0];
                useitem->usevalue = UseList.Index[useitem->itemnum]->useeffect[1];

                //LMA: Special for Rocks.
                if(useitem->itemnum==327)
                {
                    useitem->usescript = 15;
                }

            }
            else if(useitem->itemnum==947)
            {
                //LMA: Net.
                useitem->usescript = 15;
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
        {
            useitem->usescript = 10;
            useitem->usetype = UseList.Index[useitem->itemnum]->useeffect[0];
            useitem->usevalue = UseList.Index[useitem->itemnum]->useeffect[1];

            if(useitem->itemnum==609)
            {
                 // Summon Metal Moldie  -  not working yet
                 useitem->usevalue = 2047;
                 useitem->usescript = 5;
                 useitem->usetype = UseList.Index[useitem->itemnum]->useeffect[1];
                 useitem->use_buff = UseList.Index[useitem->itemnum]->useeffect[1];
            }
            //LMA: Anti hack protection, those skills (plastic surgeon, reset skills) will be
            //deleted in the quest itself, it'll avoid packets injections ;) .
            //if (useitem->itemnum>=451&&useitem->itemnum<=453)
                //useitem->usescript = 11;
        }
        break;
        case 315://Repair Tool
            delete useitem;
            return NULL;
        break;
        case 316://Quest Scroll
           delete useitem;
           return NULL;
        break;
        case 317://Engine Fuel - items 293-295, LMA / DRose
        {
            useitem->usescript = 12;
            useitem->usetype = UseList.Index[useitem->itemnum]->useeffect[0];
            useitem->usevalue = UseList.Index[useitem->itemnum]->useeffect[1];
        }
        break;
        case 320://Automatic Consumption - HP, MP, Stamina, Clan Point
            if ((useitem->itemnum>150 && useitem->itemnum<192)||(useitem->itemnum>61 && useitem->itemnum<70))
            {
                useitem->usescript = 1;
                //clan points
                if(useitem->itemnum>180 && useitem->itemnum<192)
                     useitem->usescript = 13;
                useitem->usetype = UseList.Index[useitem->itemnum]->useeffect[0];
                useitem->usevalue = UseList.Index[useitem->itemnum]->useeffect[1];
                Log( MSG_INFO, "auto consumme (%i:%i), script: %i, %i,%i",type,useitem->itemnum,useitem->usescript,useitem->usetype,useitem->usevalue);
            }
            else
            {
                Log( MSG_WARNING, "Unknown Item %i - Type %i",useitem->itemnum,type);
                delete useitem;
                return NULL;
            }
        break;
        case 321://Time Coupon - New Way to check TC
        {
              //Log( MSG_INFO, "TIme Coupon Number %i",UseList.Index[useitem->itemnum]->useeffect[0]);
              switch (UseList.Index[useitem->itemnum]->useeffect[0])
              {
                    case 138: //LMA: Null exp.
                    {
                        Log(MSG_INFO,"Null Exp activated for %s",thisclient->CharInfo->charname);
                        thisclient->no_exp=true;
                        thisclient->timer_no_exp=time(NULL)+60*(UseList.Index[useitem->itemnum]->useeffect[1]*10);     //Effective Time Duration
                        BEGINPACKET( pak, 0x702 );
                        ADDSTRING( pak, "[Mileage] Null Exp activated." );
                        ADDBYTE( pak, 0 );
                        thisclient->client->SendPacket(&pak);
                        useitem->usescript = 18;
                        useitem->usetype =0;
                        useitem->usevalue =UseList.Index[useitem->itemnum]->quality/100;
                    }
                    break;
                    case 96://Special Shop Coupon, Shop A/B/C, Moldie, snowman, ....
                    {
                        Log(MSG_INFO,"Shop changed to %i",UseList.Index[useitem->itemnum]->quality);
                        thisclient->Shop->ShopType=UseList.Index[useitem->itemnum]->quality;
                        //thisclient->Shop->mil_shop_time=clock()+10*86400*1000; //10 days
                        //thisclient->Shop->mil_shop_time=time(NULL)+10*86400; //10 days
                        thisclient->Shop->mil_shop_time=time(NULL)+60*(UseList.Index[useitem->itemnum]->useeffect[1]*10);     //Effective Time Duration
                        BEGINPACKET( pak, 0x702 );
                        ADDSTRING( pak, "[Mileage] Your Shop look has changed." );
                        ADDBYTE( pak, 0 );
                        thisclient->client->SendPacket(&pak);
                        //useitem->usescript = 1;
                        useitem->usescript = 18;
                        useitem->usetype =0;
                        useitem->usevalue =UseList.Index[useitem->itemnum]->quality;
                    }
                    break;
                    //LMA / Dream Rose: Medal Exp
                    //if(((useitem->itemnum==948)||(useitem->itemnum>=201 && useitem->itemnum<=203))&&(thisclient->timerxp == 0))
                    case 133://Special Exp Time Coupon, Adventurer, Trainee, ...
                    {
                        if (thisclient->timerxp == 0)
                        {
                            //useitem->usescript = 1;
                            useitem->usescript = 18;
                            useitem->usetype =0;
                            useitem->usevalue =UseList.Index[useitem->itemnum]->quality/100;
                            thisclient->bonusxp=1;
                            thisclient->wait_validation=UseList.Index[useitem->itemnum]->quality/100;
                            Log(MSG_INFO,"Wait validation %i",thisclient->wait_validation);

                            //Good version?
                            if(useitem->itemnum==200||useitem->itemnum==948)
                            {
                                //valid until logout (limit to one hour)
                                thisclient->once=true;
                                //thisclient->timerxp=clock()+60*60*1000;  //1 hour
                                thisclient->timerxp=time(NULL)+60*60;  //1 hour
                                Log(MSG_INFO,"Bonus XP to %i",thisclient->bonusxp);
                                BEGINPACKET( pak, 0x702 );
                                ADDSTRING( pak, "The effect will hold until you log off or you play for one hour." );
                                ADDBYTE( pak, 0 );
                                thisclient->client->SendPacket(&pak);
                            }
                            else if (useitem->itemnum==199)
                            {
                                //30 minutes.
                                //valid until logout
                                thisclient->once=true;
                                thisclient->timerxp=time(NULL)+30*60;  //1 hour
                                //Log(MSG_INFO,"Bonus XP to %i",thisclient->bonusxp);
                                BEGINPACKET( pak, 0x702 );
                                ADDSTRING( pak, "The effect will hold until you log off or you play for 30 minutes." );
                                ADDBYTE( pak, 0 );
                                thisclient->client->SendPacket(&pak);
                            }
                            else if(useitem->itemnum==203)
                            {
                                //3 days, will "resist" to logout ;)
                                //thisclient->timerxp=clock()+86400*1000;
                                thisclient->timerxp=time(NULL)+86400*3;
                                thisclient->once=false;
                                //Log(MSG_INFO,"Bonus XP to %i",thisclient->bonusxp);
                                BEGINPACKET( pak, 0x702 );
                                ADDSTRING( pak, "The effect will hold 3 days." );
                                ADDBYTE( pak, 0 );
                                thisclient->client->SendPacket(&pak);
                            }
                            else
                            {
                                //1 day, will "resist" to logout ;)
                                //thisclient->timerxp=clock()+86400*1000;
                                thisclient->timerxp=time(NULL)+86400;
                                thisclient->once=false;
                                //Log(MSG_INFO,"Bonus XP to %i",thisclient->bonusxp);
                                BEGINPACKET( pak, 0x702 );
                                ADDSTRING( pak, "The effect will hold 24 hours." );
                                ADDBYTE( pak, 0 );
                                thisclient->client->SendPacket(&pak);
                            }
                        }
                        else
                        {
                            //LMA: Player already has a medal exp.
                            BEGINPACKET( pak, 0x702 );
                            ADDSTRING( pak, "You already have an exp bonus." );
                            ADDBYTE( pak, 0 );
                            thisclient->client->SendPacket(&pak);
                            return NULL;
                        }

                    }
                    break;
                    case 134://Medal of Fortune
                    {
                        if (thisclient->timerddrop == 0)
                        {
                            thisclient->bonusddrop=1;
                            thisclient->wait_validation_ddrop=UseList.Index[useitem->itemnum]->quality; //quality
                            Log(MSG_INFO,"Wait validation ddrop %i",thisclient->wait_validation_ddrop);
                            thisclient->timerddrop=time(NULL)+60*(UseList.Index[useitem->itemnum]->useeffect[1]*10);  //Effective Time Duration
                            Log(MSG_INFO,"Medal of Fortune drop * %i",UseList.Index[useitem->itemnum]->quality);
                             thisclient->once_ddrop=false;  //stay at logout
                        }
                        else
                        {
                            //LMA: Player already has a medal exp.
                            BEGINPACKET( pak, 0x702 );
                            ADDSTRING( pak, "You already have a drop bonus." );
                            ADDBYTE( pak, 0 );
                            thisclient->client->SendPacket(&pak);
                            return NULL;
                        }
                    }
                    break;
                    case 135://Medal of Excellence
                    {
                        if (thisclient->timerstatdrop == 0)
                        {
                            thisclient->bonusstatdrop=1;
                            thisclient->wait_validation_statdrop=UseList.Index[useitem->itemnum]->quality;
                            Log(MSG_INFO,"Wait validation statdrop %i",thisclient->wait_validation_statdrop);
                            thisclient->timerstatdrop=time(NULL)+60*(UseList.Index[useitem->itemnum]->useeffect[1]*10);  //Effective time duration
                            Log(MSG_INFO,"Medal of Excellence Drop Stat Increase by %i /100",UseList.Index[useitem->itemnum]->quality);
                            thisclient->once_statdrop=false;    //stay at logout

                        }
                        else
                        {
                            //LMA: Player already has a medal exp.
                            BEGINPACKET( pak, 0x702 );
                            ADDSTRING( pak, "You already have a drop stat bonus." );
                            ADDBYTE( pak, 0 );
                            thisclient->client->SendPacket(&pak);
                            return NULL;
                        }
                    }
                    break;
                    case 136://Medal of Retrieval
                    {
                        if (thisclient->timergraydrop == 0)
                        {
                            thisclient->bonusgraydrop=0;
                            thisclient->wait_validation_graydrop=UseList.Index[useitem->itemnum]->quality;
                            Log(MSG_INFO,"Wait validation graydrop %i",thisclient->wait_validation_graydrop);
                            thisclient->timergraydrop=time(NULL)+60*(UseList.Index[useitem->itemnum]->useeffect[1]*10);  //Effective time duration
                            Log(MSG_INFO,"Medal of Retrieval All Monster Drop to %i",UseList.Index[useitem->itemnum]->quality);
                            thisclient->once_graydrop=false;    //stay at logout
                        }
                        else
                        {
                            //LMA: Player already has a medal exp.
                            BEGINPACKET( pak, 0x702 );
                            ADDSTRING( pak, "You already have a retrieval bonus." );
                            ADDBYTE( pak, 0 );
                            thisclient->client->SendPacket(&pak);
                            return NULL;
                        }
                    }
                    break;
            }
        }
        break;
        case 323://Job Skill, Unique Kill, Mileage Skill and All Skill Reset Books
        {
            useitem->usescript = 17;
            useitem->usetype = 0;
            useitem->usevalue = 0;
        }
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
        case 0://yourself
        case 1://party member
        case 2://clan member
        case 3://ally
        case 8://all members
        case 9://Fainted
            return true;
        break; //Hostile Target
        case 4: //monster
        case 5: //hostile character
        case 6: //hostile team member
        case 10: //hostile monster
            return false;
        break;
        default:
            Log(MSG_WARNING,"Unknown skill target %i", thisskill->target);
    }
    return false;
}

// calc distance
float CWorldServer::distance( fPoint pos1, fPoint pos2 )
{
    float distance = 0;
	float dx = pos1.x - pos2.x;
	float dy = pos1.y - pos2.y;
	distance = sqrt( (dx*dx) + (dy*dy) );
    return distance;
}

// Search client by userid
CPlayer* CWorldServer::GetClientByUserID( UINT userid )
{
    for(UINT i=0;i<ClientList.size();i++)
	{
        CPlayer* thisclient = (CPlayer*)ClientList.at(i)->player;
        if(thisclient->Session->userid==userid && thisclient->Session->accesslevel != 0xffff)
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


//LMA: returns a "type" from an item in a slot.
UINT CWorldServer::ReturnItemType( CPlayer* thisclient, int slot )
{
    UINT type=0;
    int material=slot;

    if(slot>=MAX_INVENTORY || slot<0||thisclient->items[material].itemtype==0||thisclient->items[material].itemnum==0)
    {
        Log(MSG_WARNING,"Incorrect data in ReturnItemType slot %i, item %u::%u",thisclient->items[material].itemtype,thisclient->items[material].itemnum);
        return 0;
    }

    if(thisclient->items[material].itemtype>=1&&thisclient->items[material].itemtype<=9)
    {
        if(thisclient->items[material].itemnum>=EquipList[thisclient->items[material].itemtype].max)
        {
            Log(MSG_WARNING,"Incorrect itemnum in ReturnItemType slot %i, item %u::%u",thisclient->items[material].itemtype,thisclient->items[material].itemnum);
            return 0;
        }

        type=EquipList[thisclient->items[material].itemtype].Index[thisclient->items[material].itemnum]->type;
    }
    else if(thisclient->items[material].itemtype==10)
    {
        if(thisclient->items[material].itemnum>=UseList.max)
        {
            Log(MSG_WARNING,"Incorrect itemnum in ReturnItemType slot %i, item %u::%u",thisclient->items[material].itemtype,thisclient->items[material].itemnum);
            return 0;
        }

        type=UseList.Index[thisclient->items[material].itemnum]->type;
    }
    else if(thisclient->items[material].itemtype==11)
    {
        if(thisclient->items[material].itemnum>=JemList.max)
        {
            Log(MSG_WARNING,"Incorrect itemnum in ReturnItemType slot %i, item %u::%u",thisclient->items[material].itemtype,thisclient->items[material].itemnum);
            return 0;
        }

        type=JemList.Index[thisclient->items[material].itemnum]->type;
    }
    else if(thisclient->items[material].itemtype==12)
    {
        if(thisclient->items[material].itemnum>=NaturalList.max)
        {
            Log(MSG_WARNING,"Incorrect itemnum in ReturnItemType slot %i, item %u::%u",thisclient->items[material].itemtype,thisclient->items[material].itemnum);
            return 0;
        }

        type=NaturalList.Index[thisclient->items[material].itemnum]->type;
    }
    else if(thisclient->items[material].itemtype==14)
    {
        if(thisclient->items[material].itemnum>=PatList.max)
        {
            Log(MSG_WARNING,"Incorrect itemnum in ReturnItemType slot %i, item %u::%u",thisclient->items[material].itemtype,thisclient->items[material].itemnum);
            return 0;
        }

        type=PatList.Index[thisclient->items[material].itemnum]->type;
    }
    else
    {
        Log(MSG_WARNING,"ReturnItemType, not coded for itemtype %i",thisclient->items[material].itemtype);
    }


    return type;
}

// Return a pseudo random number
//LMA: So if you do RandNumber( 40, 70) it means it'll return a
//number from 40 to (40+70-1)!
UINT CWorldServer::RandNumber( UINT init, UINT range, UINT seed )
{
    if(seed!=0)
        srand(seed);
    if(range>RAND_MAX)
    {
        UINT max = (UINT)round(sqrt((double)range));
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
//LMA: using the new "MAX" defines.
bool CWorldServer::IsValidItem(UINT type, UINT id )
{
    if(type==0 || id==0 )
        return false;
    if(type<10)
    {
        if(id>=EquipList[type].max)
            return false;
        if(EquipList[type].Index[id]->id==0)
            return false;
    }
    else
    {
        switch(type)
        {
            case 10:
                if(id>=UseList.max)
                    return false;
                if(UseList.Index[id]->id==0)
                    return false;
            break;
            case 11:
                if(id>=JemList.max)
                    return false;
                if(JemList.Index[id]->id==0)
                    return false;
            break;
            case 12:
                if(id>=NaturalList.max)
                    return false;
                if(NaturalList.Index[id]->id==00)
                    return false;
            break;
            case 14:
                //if(id>999)
                if(id>=PatList.max)
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
    //LMA: adding ID.
    thisparty->PartyId=GetNewPartyID();
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
            //LMA: freeing the ID as well.
            PartyIDList[thisparty->PartyId]=false;
            return true;
        }
    }
    return false;
}

UINT CWorldServer::GetLevelGhost( UINT map, UINT level)
{
    UINT montype = 0;
    montype = (UINT)floor((double)(level - 19) / 4) + 711;
    if(map >=1 && map <=50) { if(level<20) return 711;if (montype> 731) return 731;}
    if(map >=51 && map <=60) { if(montype<732) return 732; if(montype>750) return 750;}
    if(map >=61 && map <= 65) { montype -= 64; if(montype<680) return 680; if(montype>692) return 692; }
    if(map > 65) { return 692;}
    return montype;
}
// return fairy range for waiting time random
UINT CWorldServer::GetFairyRange( UINT part )
{
	UINT Range1[] = { 5, 5, 4, 3, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };
    UINT Range2[] = { 10, 7, 7, 6, 6, 6, 6, 5, 4, 3, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1 };

    //LMA: possible problem if too many fairies...
    int offset=GServer->FairyList.size();
    if (offset>=20)
    {
        offset=19;
    }

	/*if (part == 0) return Range1[GServer->FairyList.size()];
	if (part == 1) return Range2[GServer->FairyList.size()];*/
    if (part == 0) return Range1[offset];
	if (part == 1) return Range2[offset];
}

//LMA Grid
//Calculates the grid number using player or monster position
UINT CWorldServer::GetGridNumber(int mapid, int posx, int posy)
{
   UINT res=0;
   int grid_id=0;


    if(mapid>=NB_MAPS)
    {
        Log(MSG_WARNING,"Wrong map in GetGridNumber, %i>=%u",mapid,NB_MAPS);
        return 0;
    }

   grid_id=allmaps[mapid].grid_id;
   if (grid_id==-1||allmaps[mapid].always_on==true)
   {
       return 0;
   }

   //we're on a 8*8 grid basis, we're translating to 10*10 grid basis.
   //In fact the 8*8 is into 10*10, a 1 border around 8*8, helps with calculations, explains the +1
   //Log(MSG_INFO,"map %i, O(%i,%i), A(%i,%i),W=%i,L=%i",mapid,gridmaps[grid_id].org_x,gridmaps[grid_id].org_y,posx,posy,gridmaps[grid_id].width,gridmaps[grid_id].length);
   /*res=(UINT) floor((posy-gridmaps[grid_id].org_y)*8/gridmaps[grid_id].width)+1;
   res=(UINT) (floor((posx-gridmaps[grid_id].org_x)*8/gridmaps[grid_id].length)+1)+res*10;*/
   //Log(MSG_INFO,"res: %u",res);

   //New way...
   res=(UINT) floor((double)(posy-gridmaps[grid_id].org_y)/MINVISUALRANGE)+1;
   res=(UINT) (floor((double)(posx-gridmaps[grid_id].org_x)/MINVISUALRANGE)+1)+res*(allmaps[mapid].nb_col+2);

   if(res>=gridmaps[grid_id].nb_cells)
   {
       Log(MSG_WARNING,"A Player is in a wrong grid! map %i (%i,%i) %u>=%u",mapid,posx,posy,res,gridmaps[grid_id].nb_cells);
       return 0;
   }


   return res;
}

//LMA Grid (for monster)
//Calculates the grid number using player or monster position
UINT CWorldServer::GetGridNumber(int mapid, int posx, int posy,CMonster* thismonster)
{
   UINT res=0;
   int grid_id=0;


    if(mapid>=NB_MAPS)
    {
        Log(MSG_WARNING,"Wrong map in GetGridNumber, %i>=%u",mapid,NB_MAPS);
        return 0;
    }

   grid_id=allmaps[mapid].grid_id;
   if (grid_id==-1||allmaps[mapid].always_on==true)
   {
       return 0;
   }

   //we're on a 8*8 grid basis, we're translating to 10*10 grid basis.
   //In fact the 8*8 is into 10*10, a 1 border around 8*8, helps with calculations, explains the +1
   //Log(MSG_INFO,"map %i, O(%i,%i), A(%i,%i),W=%i,L=%i",mapid,gridmaps[grid_id].org_x,gridmaps[grid_id].org_y,posx,posy,gridmaps[grid_id].width,gridmaps[grid_id].length);
   /*res=(UINT) floor((posy-gridmaps[grid_id].org_y)*8/gridmaps[grid_id].width)+1;
   res=(UINT) (floor((posx-gridmaps[grid_id].org_x)*8/gridmaps[grid_id].length)+1)+res*10;*/
   //Log(MSG_INFO,"res: %u",res);

   //New way...
   res=(UINT) floor((double)(posy-gridmaps[grid_id].org_y)/MINVISUALRANGE)+1;
   res=(UINT) (floor((double)(posx-gridmaps[grid_id].org_x)/MINVISUALRANGE)+1)+res*(allmaps[mapid].nb_col+2);

   if(res>=gridmaps[grid_id].nb_cells)
   {
       if(thismonster==NULL)
       {
           Log(MSG_WARNING,"A NULL monster is in a wrong grid! map %i (%i,%i) %u>=%u",mapid,posx,posy,res,gridmaps[grid_id].nb_cells);
           return 0;
       }

        Log(MSG_WARNING,"Monster CID %u (montype %u) is in a wrong grid, let's kill him! map %i (%i,%i) %u>=%u",thismonster->clientid,thismonster->montype,mapid,posx,posy,res,gridmaps[grid_id].nb_cells);
        thismonster->Stats->HP=-1;
        return 0;
   }


   return res;
}

//LMA: saving all storage
bool CWorldServer::SaveAllStorage( CPlayer* thisclient)
{

    if(!GServer->DB->QExecute("DELETE FROM storage WHERE owner=%i", thisclient->Session->userid)) return true;
    for(UINT i=0;i<160;i++)
    {
    	if (thisclient->storageitems[i].count > 0)
        {
    		GServer->DB->QExecute("INSERT INTO storage (owner,itemnum,itemtype,refine,durability,lifespan,slotnum,count,stats,socketed,appraised,gem) VALUES(%i,%i,%i,%i,%i,%i,%i,%i,%i,%i,%i,%i)",
    							thisclient->Session->userid, thisclient->storageitems[i].itemnum, thisclient->storageitems[i].itemtype,thisclient->storageitems[i].refine, thisclient->storageitems[i].durability,
    							thisclient->storageitems[i].lifespan, i, thisclient->storageitems[i].count, thisclient->storageitems[i].stats, (thisclient->storageitems[i].socketed?1:0),
                                (thisclient->storageitems[i].appraised?1:0), thisclient->storageitems[i].gem );//Gem Fix by Irulagain
    	}
    }

    return true;
}

//LMA: getting all storage.
bool CWorldServer::GetAllStorage( CPlayer* thisclient)
{
    //resetting array
    for(int k=0;k<MAX_STORAGE;k++)
    {
        thisclient->storageitems[k].itemnum = 0;
        thisclient->storageitems[k].itemtype = 0;
        thisclient->storageitems[k].refine = 0;
        thisclient->storageitems[k].durability = 0;
        thisclient->storageitems[k].lifespan = 0;
        thisclient->storageitems[k].count = 0;
        thisclient->storageitems[k].stats = 0;
        thisclient->storageitems[k].socketed = false;
        thisclient->storageitems[k].appraised = false;
        thisclient->storageitems[k].gem = 0;
    }

	MYSQL_ROW row;
	MYSQL_RES *result = GServer->DB->QStore("SELECT itemnum,itemtype,refine,durability,lifespan,slotnum,count,stats,socketed,appraised,gem FROM storage WHERE owner=%i", thisclient->Session->userid);
    if(result==NULL) return false;
    thisclient->nstorageitems = mysql_num_rows(result);
    while(row = mysql_fetch_row(result))
    {
        if(!GServer->IsValidItem( atoi(row[1]), atoi(row[0]) ) || atoi(row[6])==0)
        {
            Log(MSG_WARNING, "char %s have a invalid or empty item in storage: %i%i [%i], this item will be deleted", thisclient->CharInfo->charname, atoi(row[1]), atoi(row[0]), atoi(row[6]) );
            continue;
        }

        UINT itemnum = atoi(row[5]);

        if(itemnum<0||itemnum>=MAX_STORAGE)
        {
            Log(MSG_WARNING,"%s:: Storage, wrong slot (%i)",thisclient->CharInfo->charname,itemnum);
            continue;
        }

        thisclient->storageitems[itemnum].itemnum = atoi(row[0]);
        thisclient->storageitems[itemnum].itemtype = atoi(row[1]);
        thisclient->storageitems[itemnum].refine = atoi(row[2]);
        thisclient->storageitems[itemnum].durability = atoi(row[3]);
        thisclient->storageitems[itemnum].lifespan = atoi(row[4]);
        thisclient->storageitems[itemnum].count = atoi(row[6]);
        thisclient->storageitems[itemnum].stats = atoi(row[7]);
        thisclient->storageitems[itemnum].socketed = (atoi(row[8])==1)?true:false;
        thisclient->storageitems[itemnum].appraised = (atoi(row[9])==1)?true:false;
        thisclient->storageitems[itemnum].gem = atoi(row[10]);
    }
    GServer->DB->QFree( );


     return true;
}


//LMA: Loading Wishlist.
bool CWorldServer::GetWishlist( CPlayer* thisclient)
{
    for (int k=0;k<MAX_WISHLIST;k++)
    {
        thisclient->wishlistitems[k].count=0;
        thisclient->wishlistitems[k].data=0;
        thisclient->wishlistitems[k].head=0;
        thisclient->wishlistitems[k].price=0;
        thisclient->wishlistitems[k].slot=0;
    }

    MYSQL_ROW row;
	MYSQL_RES *result = DB->QStore("SELECT slot,itemhead,itemdata FROM wishlist WHERE itemowner=%i ORDER BY slot DESC",thisclient->CharInfo->charid);
    if(result==NULL)
    {
        return false;
    }

    while(row = mysql_fetch_row(result))
    {
        int myslot=0;
        myslot=atoi(row[0]);

        if(myslot<0||myslot>=MAX_WISHLIST)
        {
            Log(MSG_WARNING,"%s:: Wishlist, wrong slot (%i)",thisclient->CharInfo->charname,myslot);
            continue;
        }

        thisclient->wishlistitems[myslot].slot=myslot;
        thisclient->wishlistitems[myslot].head=atoi(row[1]);
        thisclient->wishlistitems[myslot].data=atoi(row[2]);
        thisclient->wishlistitems[myslot].price=0;
        thisclient->wishlistitems[myslot].count=0;
    }

    GServer->DB->QFree( );


    return true;
}

//LMA: Get Zuly from Storage (from MySQL)
bool CWorldServer::GetZulyStorage( CPlayer* thisclient)
{
	MYSQL_ROW row;
	MYSQL_RES *result = DB->QStore("SELECT zulystorage FROM accounts WHERE id=%i", thisclient->Session->userid);
    if(result==NULL) return false;
	row = mysql_fetch_row(result);
	thisclient->CharInfo->Storage_Zulies = atoi( row[0] );		//PY needs to be atoll really
	//thisclient->CharInfo->Storage_Zulies = atoll( row[0] );
	DB->QFree( );


     return true;
}


//LMA: Save Zuly to Storage (to MySQL)
bool CWorldServer::SaveZulyStorage( CPlayer* thisclient)
{
     //GServer->DB->QExecute("update accounts set zulystorage = %i where id = %i", thisclient->CharInfo->Storage_Zulies, thisclient->Session->userid);
     GServer->DB->QExecute("update accounts set zulystorage = %I64i where id = %i", thisclient->CharInfo->Storage_Zulies, thisclient->Session->userid);


     return true;
}

//LMA: Get a slot storage (Mysql)
bool CWorldServer::GetSlotStorage( CPlayer* thisclient,UINT slotnum)
{
	MYSQL_ROW row;
	MYSQL_RES *result = GServer->DB->QStore("SELECT itemnum,itemtype,refine,durability,lifespan,slotnum,count,stats,socketed,appraised,gem FROM storage WHERE owner=%i AND slotnum=%i", thisclient->Session->userid,slotnum);
     if(result==NULL)
     {
         Log(MSG_INFO,"in GetSlotStorage False");
         return false;
     }

    //row = mysql_fetch_row(result);
    while(row = mysql_fetch_row(result))
    {
        //we refresh only the item storage we need.
        thisclient->storageitems[slotnum].itemnum = atoi(row[0]);
        thisclient->storageitems[slotnum].itemtype = atoi(row[1]);
        thisclient->storageitems[slotnum].refine = atoi(row[2]);
        thisclient->storageitems[slotnum].durability = atoi(row[3]);
        thisclient->storageitems[slotnum].lifespan = atoi(row[4]);
        thisclient->storageitems[slotnum].count = atoi(row[6]);
        thisclient->storageitems[slotnum].stats = atoi(row[7]);
        thisclient->storageitems[slotnum].socketed = (atoi(row[8])==1)?true:false;
        thisclient->storageitems[slotnum].appraised = (atoi(row[9])==1)?true:false;
        thisclient->storageitems[slotnum].gem = atoi(row[10]);
    }
    GServer->DB->QFree( );


     return true;
}

//LMA: Saving one slot in storage
bool CWorldServer::SaveSlotStorage( CPlayer* thisclient,UINT slotnum)
{
    UINT i=0;
    i=slotnum;

    //New version (MySQL 4.1+)
    if (thisclient->storageitems[i].count > 0)
    {
        GServer->DB->QExecute("INSERT INTO storage (owner,slotnum,itemnum,itemtype,refine,durability,lifespan,count,stats,socketed,appraised,gem) VALUES(%i,%i,%i,%i,%i,%i,%i,%i,%i,%i,%i,%i) ON DUPLICATE KEY UPDATE owner=VALUES(owner),slotnum=VALUES(slotnum),itemnum=VALUES(itemnum),itemtype=VALUES(itemtype),refine=VALUES(refine),durability=VALUES(durability),lifespan=VALUES(lifespan),count=VALUES(count),stats=VALUES(stats),socketed=VALUES(socketed),appraised=VALUES(appraised),gem=VALUES(gem)",
    						thisclient->Session->userid, i, thisclient->storageitems[i].itemnum, thisclient->storageitems[i].itemtype,thisclient->storageitems[i].refine, thisclient->storageitems[i].durability,
    						thisclient->storageitems[i].lifespan, thisclient->storageitems[i].count, thisclient->storageitems[i].stats, (thisclient->storageitems[i].socketed?1:0),
                            (thisclient->storageitems[i].appraised?1:0), thisclient->storageitems[i].gem );
    }
    else
    {
        GServer->DB->QExecute("DELETE FROM storage WHERE owner=%i AND slotnum=%i", thisclient->Session->userid,i);
    }


    return true;
}

//LMA: Saving a slot in ItemMall.
bool CWorldServer::SaveSlotMall( CPlayer* thisclient,UINT slotnum)
{
    UINT i=0;
    i=slotnum;


    if (thisclient->itemmallitems[i].count > 0)
    {
        GServer->DB->QExecute("INSERT INTO mileage (owner,slotnum,itemnum,itemtype,refine,durability,lifespan,count,stats,socketed,appraised,gem) VALUES(%i,%i,%i,%i,%i,%i,%i,%i,%i,%i,%i,%i) ON DUPLICATE KEY UPDATE owner=VALUES(owner),slotnum=VALUES(slotnum),itemnum=VALUES(itemnum),itemtype=VALUES(itemtype),refine=VALUES(refine),durability=VALUES(durability),lifespan=VALUES(lifespan),count=VALUES(count),stats=VALUES(stats),socketed=VALUES(socketed),appraised=VALUES(appraised),gem=VALUES(gem)",
    						thisclient->Session->userid, i, thisclient->itemmallitems[i].itemnum, thisclient->itemmallitems[i].itemtype,thisclient->itemmallitems[i].refine, thisclient->itemmallitems[i].durability,
    						thisclient->itemmallitems[i].lifespan, thisclient->itemmallitems[i].count, thisclient->itemmallitems[i].stats, (thisclient->itemmallitems[i].socketed?1:0),
                            (thisclient->itemmallitems[i].appraised?1:0), thisclient->itemmallitems[i].gem );
    }
    else
    {
        GServer->DB->QExecute("DELETE FROM mileage WHERE owner=%i AND slotnum=%i", thisclient->Session->userid,i);
    }


    return true;
}

//LMA: Trying to get a monster from a location (for AOE_TARGET packet mainly).
CMonster* CWorldServer::LookAOEMonster(CPlayer* thisclient)
{
    CMap* map = MapList.Index[thisclient->Position->Map];
    for(UINT i=0;i<map->MonsterList.size();i++)
    {
        CMonster* monster = map->MonsterList.at(i);
        if(monster->clientid==thisclient->clientid) continue;
        if(thisclient->IsSummon( ) || thisclient->IsPlayer( ))
        {
            if(monster->IsSummon( ) && (map->allowpvp==0 || monster->owner==thisclient->clientid)) continue;
        }
        else
        {
            if(!monster->IsSummon( )) continue;
        }

        //Looking for a monsters in the aera (1 meter from position sent by client)
        if(GServer->IsMonInCircle( thisclient->Position->aoedestiny,monster->Position->current,(float) 1))
        {
            return monster;
        }

    }


    return NULL;
}

//LMA: Character version
CMonster* CWorldServer::LookAOEMonster(CCharacter* character)
{
    CMap* map = MapList.Index[character->Position->Map];
    for(UINT i=0;i<map->MonsterList.size();i++)
    {
        CMonster* monster = map->MonsterList.at(i);
        if(monster->clientid==character->clientid) continue;
        if(character->IsSummon( ) || character->IsPlayer( ))
        {
            if(monster->IsSummon( ) && (map->allowpvp==0 || monster->owner==character->clientid)) continue;
        }
        else
        {
            if(!monster->IsSummon( )) continue;
        }

        //Looking for a monsters in the aera (1 meter from position sent by client)
        if(GServer->IsMonInCircle( character->Position->aoedestiny,monster->Position->current,(float) 1))
        {
            return monster;
        }

    }


    return NULL;
}


//LMA: returns the Item Mall list (with packet)
void CWorldServer::ReturnItemMallList(CPlayer* thisclient)
{
     thisclient->CPlayer::RebuildItemMall();

     if (thisclient->nsitemmallitems==0)
        return;

    //First signature.
    BEGINPACKET( pak, 0x7d9 );
    ADDWORD    ( pak, 0x0008);
    ADDWORD    ( pak, 0x0000 );
    ADDBYTE    ( pak, 0x44 );
    ADDBYTE    ( pak, 0x04  );
    ADDBYTE    ( pak, 0x20  );
    ADDBYTE    ( pak, 0x8b  );
    ADDBYTE    ( pak, 0x11  );
    ADDBYTE    ( pak, 0x49  );
    ADDBYTE    ( pak, 0xc0  );
    ADDBYTE    ( pak, 0x3c  );
    ADDBYTE    ( pak, 0xf9  );
    ADDBYTE    ( pak, 0x48  );
    ADDBYTE    ( pak, 0x00  );
    ADDBYTE    ( pak, 0x00  );
    ADDBYTE    ( pak, 0x01  );
    thisclient->client->SendPacket( &pak );


     //list
     RESETPACKET(pak,0x7d9);
     ADDBYTE    ( pak, 0x09 );
     ADDBYTE    ( pak, thisclient->nsitemmallitems);

     //signature
     ADDWORD    ( pak, 0x0000 );
     ADDBYTE    ( pak, 0x2f );
     ADDBYTE    ( pak, 0x02 );
     ADDBYTE    ( pak, 0xc4 );
     ADDBYTE    ( pak, 0x28 );
     ADDBYTE    ( pak, 0x10 );
     ADDBYTE    ( pak, 0x49 );
     ADDBYTE    ( pak, 0x91 );
     ADDBYTE    ( pak, 0xbd );
     ADDBYTE    ( pak, 0x06 );
     ADDBYTE    ( pak, 0x49 );
     ADDBYTE    ( pak, 0x4f );
     ADDBYTE    ( pak, 0x2c );
     ADDBYTE    ( pak, 0x00);

     //there's no void between two objects ;)
     for(int i=0;i<thisclient->nsitemmallitems;i++)
     {
         ADDDWORD   ( pak, BuildItemHead( thisclient->itemmallitems[i] ) );
         ADDDWORD   ( pak, BuildItemData( thisclient->itemmallitems[i] ) );
         ADDWORD    ( pak, 0x0000 );
         ADDWORD    ( pak, 0x0000 );
         ADDWORD    ( pak, 0x0000 );
         ADDBYTE    ( pak, 0x00);

         Log(MSG_INFO,"We send item in slot %i/%i: %i * %i:%i",i,thisclient->nsitemmallitems,thisclient->itemmallitems[i].count,thisclient->itemmallitems[i].itemtype,thisclient->itemmallitems[i].itemnum);
     }

     thisclient->client->SendPacket( &pak );

    //Signature end
    RESETPACKET( pak, 0x7d9 );
    ADDWORD    ( pak, 0x000a);
    ADDWORD    ( pak, 0x0000 );
    ADDBYTE    ( pak, 0xca);
    ADDBYTE    ( pak, 0x01  );
    ADDBYTE    ( pak, 0x3c  );
    ADDBYTE    ( pak, 0x84  );
    ADDBYTE    ( pak, 0x08  );
    ADDBYTE    ( pak, 0x49  );
    ADDBYTE    ( pak, 0xbd  );
    ADDBYTE    ( pak, 0x8e  );
    ADDBYTE    ( pak, 0xda  );
    ADDBYTE    ( pak, 0x48  );
    ADDBYTE    ( pak, 0xcb  );
    ADDBYTE    ( pak, 0x03  );
    ADDBYTE    ( pak, 0x00  );
    thisclient->client->SendPacket( &pak );

     return;
}

//LMA: takes an item from item mall to player's inventory
void CWorldServer::TakeItemMallList(CPlayer* thisclient,int qty,int slot)
{
 	MYSQL_ROW row;
	MYSQL_RES *result = GServer->DB->QStore("SELECT itemnum,itemtype,refine,durability,lifespan,slotnum,count,stats,socketed,appraised,gem FROM mileage WHERE owner=%i AND slotnum=%i", thisclient->Session->userid,slot);
	if(result==NULL)
    {
       Log(MSG_HACK,"%s tried to get %i items from slot %i from mileage and there is nothing there !",thisclient->CharInfo->charname, qty, slot);
       return;
    }

    if(mysql_num_rows(result)!=1)
    {
      Log(MSG_HACK,"%s tried to get %i items from slot %i from mileage and there is nothing there !",thisclient->CharInfo->charname, qty, slot);
      return;
    }

    row = mysql_fetch_row(result);
    //little refresh :)
	thisclient->itemmallitems[slot].itemnum = atoi(row[0]);
	thisclient->itemmallitems[slot].itemtype = atoi(row[1]);
	thisclient->itemmallitems[slot].refine = atoi(row[2]);
	thisclient->itemmallitems[slot].durability = atoi(row[3]);
	thisclient->itemmallitems[slot].lifespan = atoi(row[4]);
	thisclient->itemmallitems[slot].count = atoi(row[6]);
	thisclient->itemmallitems[slot].stats = atoi(row[7]);
	thisclient->itemmallitems[slot].socketed = (atoi(row[8])==1)?true:false;
	thisclient->itemmallitems[slot].appraised = (atoi(row[9])==1)?true:false;
	thisclient->itemmallitems[slot].gem = atoi(row[10]);

    Log(MSG_INFO,"%s takes %i * %i:%i from item mall",thisclient->CharInfo->charname,qty,thisclient->itemmallitems[slot].itemtype,thisclient->itemmallitems[slot].itemnum);

    GServer->DB->QFree( );
    if(qty>thisclient->itemmallitems[slot].count)
    {
       Log(MSG_HACK,"%s tried to get %i items from slot %i from mileage, had only %i !",thisclient->CharInfo->charname, qty, slot,thisclient->itemmallitems[slot].count);
       return;
    }

    //Is there a free slot into inventory for this item?
    CItem newitem = thisclient->itemmallitems[slot];
    if(newitem.itemtype>9 && newitem.itemtype<14)
    {
        int count = qty;
        if( count>thisclient->itemmallitems[slot].count )
            count = thisclient->itemmallitems[slot].count;
        newitem.count = count;
        thisclient->itemmallitems[slot].count -= count;

        if(thisclient->itemmallitems[slot].count<=0)
            ClearItem(thisclient->itemmallitems[slot]);
    }
    else
    {
        ClearItem(thisclient->itemmallitems[slot]);
    }

    int newslot= thisclient->GetNewItemSlot ( newitem );
    //no place in player's inventory, so back to itemall.
    if(newslot==0xffff)
    {
        thisclient->itemmallitems[slot] = newitem;
        return ;
    }

    int amount = 0;
    if(thisclient->items[newslot].count>0)
    {
      int amount = thisclient->items[newslot].count;
	  newitem.count+=amount;
    }
    if( newitem.count > 999 )
    {
        amount = 999 - newitem.count;
        newitem.count = 999;
    }
    thisclient->items[newslot] = newitem;
    if( amount > 0 )
    {
        newitem.count = amount;
        unsigned int newslot2 = thisclient->GetNewItemSlot( newitem );
        if( newslot2 == 0xffff )
        {
            thisclient->itemmallitems[slot] = thisclient->items[newslot];
            thisclient->items[newslot].count = amount;
            return ;
        }
        thisclient->items[newslot2] = newitem;
        thisclient->UpdateInventory( newslot2 );
    }

    if (thisclient->items[newslot].itemnum>0)
       Log(MSG_INFO,"Slot in inventory: %i, exist already: %i * %i:%i",newslot,thisclient->items[newslot].count,thisclient->items[newslot].itemtype,thisclient->items[newslot].itemnum);
    else
        Log(MSG_INFO,"Slot in inventory: %i, does not exist already.",newslot);

    //Packet time :)
    BEGINPACKET( pak, 0x7d9 );
    ADDBYTE    ( pak, 0x04 );
    ADDBYTE    ( pak, slot );
    ADDBYTE    ( pak, newslot );
   	ADDDWORD   ( pak, BuildItemHead( thisclient->items[newslot] ) );
	ADDDWORD   ( pak, BuildItemData( thisclient->items[newslot] ) );
	//signature
    ADDBYTE    (pak,0xbb);
    ADDBYTE    (pak,0xfb);
    ADDBYTE    (pak,0xb0);
    ADDBYTE    (pak,0x01);
    ADDWORD    (pak,0x0000);
    //End of item signature.
    ADDBYTE    ( pak, slot );
    ADDDWORD   ( pak, BuildItemHead( thisclient->itemmallitems[slot] ) );
    ADDDWORD   ( pak, BuildItemData( thisclient->itemmallitems[slot] ) );
	//signature
    ADDBYTE    (pak,0x00);
    ADDBYTE    (pak,0x00);
    ADDBYTE    (pak,0x00);
    ADDBYTE    (pak,0x00);
    ADDWORD    (pak,0x0000);
    //End of item signature.
    //last signature
    ADDWORD    (pak,0xcccc );
    ADDBYTE    (pak,0x4f );
    ADDBYTE    (pak,0x0c );
    ADDBYTE    (pak,0x36 );
    ADDBYTE    (pak,0xf4 );
    ADDBYTE    (pak,0x00 );
    ADDWORD    (pak,0x0000 );
    ADDWORD    (pak,0x0000 );
    ADDWORD    (pak,0x0000 );
    ADDWORD    (pak,0x0000);
    //end of last signature...

    thisclient->client->SendPacket( &pak );
    //should be ok now :)
    if(thisclient->itemmallitems[slot].itemnum==0)
       thisclient->nsitemmallitems--;

    //Saving :)
    SaveSlotMall(thisclient,slot);
    thisclient->SaveSlot41(newslot);


    return;
}

//LMA: Send a packet too all connected clients in a map.
void CWorldServer::SendToAllInMap( CPacket* pak, int mapid)
{
    CMap* map = MapList.Index[mapid];

    for(UINT i=0;i<map->PlayerList.size();i++)
    {
        CPlayer* otherclient = map->PlayerList.at(i);
        if (otherclient->client == NULL) continue;
        if (!otherclient->client->isActive) continue;

        if (otherclient->Session->inGame)
            otherclient->client->SendPacket( pak );
    }

}

//LMA: return itemtype or itemnum
//0 returns itemtype.
//1 returns itemnum.
UINT CWorldServer::gi(UINT itemvalue, short type)
{
    UINT itemtype=0;
    UINT itemnum=0;


    itemtype= itemvalue/1000;
    if (itemtype>=1&&itemtype<=14)
    {
        itemnum=itemvalue % 1000;
        if(type==0)
        {
            return itemtype;
        }

        return itemnum;
    }

    if(itemtype==0)
    {
        itemtype=itemvalue/100;
        if (itemtype>=1&&itemtype<=14)
        {
            itemnum=itemvalue % 100;
            if(type==0)
            {
                return itemtype;
            }

            return itemnum;
        }

        if(itemtype==0)
        {
            itemtype=itemvalue/10;
            if (itemtype>=1&&itemtype<=14)
            {
                itemnum=itemvalue % 10;
                if(type==0)
                {
                    return itemtype;
                }

                return itemnum;
            }

        }

        if(itemtype==0)
        {
            Log(MSG_WARNING,"Impossible to get an item from %u",itemvalue);
            return 0;
        }

    }

    /*itemtype= itemvalue/10000;
    if (itemtype>=1&&itemtype<=14)
    {
        itemnum=itemvalue % 10000;
        if(type==0)
        {
            return itemtype;
        }

        return itemnum;
    }*/

    itemtype= itemvalue/100000;
    if (itemtype>=1&&itemtype<=14)
    {
        itemnum=itemvalue % 100000;
        if(type==0)
        {
            return itemtype;
        }

        return itemnum;
    }


    //LMA: who wants to be a milionnaire? ^_^
    itemtype= itemvalue/1000000;
    if (itemtype>=1&&itemtype<=14)
    {
        itemnum=itemvalue % 1000000;
        if(type==0)
        {
            return itemtype;
        }

        return itemnum;
    }

    Log(MSG_WARNING,"Impossible to get an item from %u",itemvalue);


    return 0;
}

//LMA: We get the exact Clan Points amount through mysql database
UINT CWorldServer::getClanPoints(int clanid)
{
    UINT nb_points=0;

    MYSQL_RES *result = GServer->DB->QStore("SELECT cp FROM list_clan where id=%i", clanid);
    if(result==NULL) return 0;
    if(mysql_num_rows(result)!=1)
    {
        return 0;
    }
    else
    {
        MYSQL_ROW row = mysql_fetch_row(result);
        nb_points= atoi(row[0]);
    }

    GServer->DB->QFree( );


    return nb_points;
}

//LMA: We get the exact Clan Grade through mysql database
UINT CWorldServer::getClanGrade(int clanid)
{
    int clangrade=0;

    MYSQL_RES *result = GServer->DB->QStore("SELECT grade FROM list_clan where id=%i", clanid);
    if(result==NULL) return 0;
    if(mysql_num_rows(result)!=1)
    {
        return 0;
    }
    else
    {
        MYSQL_ROW row = mysql_fetch_row(result);
        clangrade= atoi(row[0]);
    }

    GServer->DB->QFree( );


    return clangrade;
}


//LMA: formula for summons (capsules) HP.
UINT CWorldServer::SummonFormula(CPlayer* thisclient,CMonster* thismonster)
{
    if (thisclient==NULL)
    {
        return 0;
    }

    long long monster_hp=thismonster->thisnpc->hp;
    long long res=(long long)(0.0984*thisclient->Stats->Level*thisclient->Stats->Level+22.514*thisclient->Stats->Level+monster_hp);


    return res;
}


//LMA: Escaping time.
string CWorldServer::EscapeMe(char* texte)
{
    string s=texte;
    string t;
    t.reserve(300);
    int offset=0;

    for (int k=0;k<s.size();k++)
    {
        if(s[k]=='\'')
        {
            t[offset]='\\';
            offset++;
            t[offset]='\'';
            offset++;
        }
        else if(s[k]==10)
        {
            t[offset]=' ';
            offset++;
        }
        else
        {
            t[offset]=s[k];
            offset++;
        }

    }

    t[offset]=0;


    return t;
}


//LMA: getting a timer from an AIP script.
UINT CWorldServer::GetTimerFromAIP(dword script_id, bool is_npc)
{
    UINT my_timer=0;


    if(AipListTimer.find(script_id) != AipListTimer.end())
    {
        my_timer = AipListTimer[script_id];
        if(my_timer==0)
        {
            Log(MSG_WARNING,"Timer is 0 for script %i!",script_id);
        }

    }
    else
    {
        Log(MSG_WARNING,"No timer for script %i!",script_id);
    }

    if(my_timer==0)
    {
        //default value for monster (6 seconds, 60 for a NPC).
        my_timer=60000;
        if(!is_npc)
        {
            my_timer=6000;
        }

        //Log(MSG_INFO,"script %i has NOW a timer %u, NPC? %i",script_id,my_timer,is_npc);
    }
    else
    {
        //Just for test.
        //Log(MSG_INFO,"script %i has a timer %u, NPC? %i",script_id,my_timer,is_npc);
    }


    return my_timer;
}


//LMA: Sending the breakid from an itemnum and itemtype.
UINT CWorldServer::GetBreakID(UINT itemnum,UINT itemtype)
{
    UINT breakid=0;

    if(itemtype<=0||itemtype==7||itemtype>=10)
    {
        return 0;
    }

    if (itemnum>=EquipList[itemtype].max)
    {
        Log(MSG_WARNING,"Wrong itemnum %u > %u, (itemtype %u)",itemnum,EquipList[itemtype].max,itemtype);
        return 0;
    }

    breakid=EquipList[itemtype].Index[itemnum]->breakid;

    //some checks.
   if (breakid!=0)
    {
        if(breakid>=maxBreak)
        {
            Log(MSG_WARNING,"The breakid %u for (%i::%i) is > %u",breakid,itemtype,itemnum,maxBreak);
            return 0;
        }

        if(BreakList[breakid]->itemnum==0||BreakList[breakid]->itemtype==0)
        {
            Log(MSG_WARNING,"The breakid %u leads to an empty break?",breakid);
            return 0;
        }

        if(itemnum!=BreakList[breakid]->itemnum||itemtype!=BreakList[breakid]->itemtype)
        {
            Log(MSG_INFO,"The breakid %u for (%i::%i) is also used for another item?",breakid,itemtype,itemnum);
        }

    }


    return breakid;
}


//LMA: Count the number of userid (accounts) in the list.
int CWorldServer::GetNbUserID( UINT userid )
{
    int nb_accounts=0;
    for(UINT i=0;i<ClientList.size( );i++)
	{
	    CPlayer* thisclient = (CPlayer*)ClientList.at(i)->player;
        if(thisclient->Session->userid==userid)
        {
            nb_accounts++;
            Log(MSG_INFO,"Avatar logged for userid %i:: %s (nb=%i) ",userid,thisclient->CharInfo->charname,nb_accounts);
            /*if (nb_accounts>1)
            {
                //Hack, in all cases.
                return nb_accounts;
            }*/

        }

	}


    return nb_accounts;
}

//LMA: Disconnect char
bool CWorldServer::ForceDiscClient( unsigned int userid )
{
    CPlayer* otherclient = GetClientByUserID( userid );
    if(otherclient==NULL) return true;

    if(otherclient==NULL)
    {
        Log(MSG_WARNING, "userid '%s' not found online (ForceDiscClient)", userid );
        return true;
    }

    BEGINPACKET( pak, 0x707 );
    ADDWORD( pak, 0 );
    otherclient->client->SendPacket( &pak );
    otherclient->client->isActive = false;


    return true;
}


//LMA: The goal is to check a string is made of valid chars and extensions.
bool CWorldServer::CheckValidName(CPlayer* thisclient, const char* my_name)
{
    char mychar[50];
    int nb_car=0;

    nb_car=strlen(my_name);
    if (nb_car>=50||nb_car<=0)
    {
        Log(MSG_HACK,"%s tried to create a too long clan %s (%i>=50)",thisclient->CharInfo->charname,my_name,nb_car);
        return false;
    }

    sprintf (mychar,"%s",my_name);

    //uppercase time.
   for (int k=0;k<nb_car;k++)
    {
        int c=toupper(mychar[k]);
        mychar[k]=c;
    }

    const char* valid_chars="AZERTYUIOPQSDFGHJKLMWXCVBN0123456789";
    int i = strspn (mychar,valid_chars);

    if(i!=nb_car)
    {
        Log(MSG_HACK,"%s tried to create a not correct clan name %s",thisclient->CharInfo->charname,my_name);
        return false;
    }

    //Now we test some not valid sentences, like GM, ADMIN and so on...
    //the [6] here is the max size of a content, for example ADMIN is 5 so you need to add an extra 1 for NULL, so 6.
    char str[][6] = { "GM" , "ADMIN" , "DEV" };
    int nb_to_check=3;  //nb of extensions to check (see above).
    for (int n=0 ; n<nb_to_check ; n++)
    {
        if(strstr(mychar,str[n])!=NULL)
        {
            Log(MSG_HACK,"%s tried to create a clan with a reserved extension %s is in %s",thisclient->CharInfo->charname,str[n],my_name);
            return false;
        }

    }


    return true;
}

//LMA: escaping.
bool CWorldServer::EscapeMySQL(const char* data,string & mystring,int nb_car_max,bool check_same)
{
    //checking data length
    if(nb_car_max!=-1)
    {
        if (strlen(data)>nb_car_max)
        {
            Log(MSG_WARNING,"Escape:: Data too big (%s) %u>%i",data,strlen(data),nb_car_max);
            return false;
        }
    }

    char * lma_username = new char[strlen(data) + 1];
    strcpy(lma_username,data);
    char * new_username = new char[strlen(data)*3 +1];
    mysql_real_escape_string(DB->Mysql, new_username,lma_username,strlen(lma_username));
    mystring.assign(new_username);
    delete[] lma_username;
    delete[] new_username;

    //Is data escaped the same as the non escaped? Useful for login for example.
    if(strcmp(data,mystring.c_str()))
    {
        Log(MSG_WARNING,"Escape:: is different (%s != %s)",data,mystring.c_str());
        return false;
    }

    return true;
}


//LMA: escaping (this version only checks escaped and unescaped versions of a string are the same).
bool CWorldServer::CheckEscapeMySQL(const char* data,int nb_car_max,bool check_same)
{
    string mystring;

    //checking data length
    if(nb_car_max!=-1)
    {
        if (strlen(data)>nb_car_max)
        {
            Log(MSG_WARNING,"Escape:: Data too big (%s) %u>%i",data,strlen(data),nb_car_max);
            return false;
        }
    }

    char * lma_username = new char[strlen(data) + 1];
    strcpy(lma_username,data);
    char * new_username = new char[strlen(data)*3 +1];
    mysql_real_escape_string(DB->Mysql, new_username,lma_username,strlen(lma_username));
    mystring.assign(new_username);
    delete[] lma_username;
    delete[] new_username;

    //Is data escaped the same as the non escaped? Useful for login for example.
    if(strcmp(data,mystring.c_str())!=0)
    {
        Log(MSG_WARNING,"Escape:: is different (%s != %s)",data,mystring.c_str());
        return false;
    }


    return true;
}


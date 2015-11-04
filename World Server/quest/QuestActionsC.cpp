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

// Props to ExJam for this code :D Ported to OSpRose by Drakia

//LMA: This file if specifically for CCharacter QSD (NPC, monsters...)
#include "../worldserver.h"
dword GetRewardValueC(dword function, dword amount, CCharacter* client, word nDupCNT){
	switch(function){
		case 0:
		{
			dword tempVal = amount + 0x1E;
            //tempVal *= client->Attr->Cha + 0x0a; // We need to add a way to get all stats totals (Including buff, E, etc) - Drakia
            tempVal *= 1 + 0x0a; // We need to add a way to get all stats totals (Including buff, E, etc) - Drakia
			tempVal *= (100 & 0xFFFF);//World Rate
			tempVal *= 0x14;//Fame + 0x14
			tempVal = ((tempVal / (client->Stats->Level + 0x46)) / 0x7530) + amount;
			return tempVal;
		}
		break;
		case 1:
		{
			dword tempVal = (client->Stats->Level + 3) * amount;
			//tempVal *= (client->Attr->Cha >> 1) + client->Stats->Level + 0x28;
			tempVal *= 1 + client->Stats->Level + 0x28;
			tempVal *= (100 & 0xFFFF);//World Rate
			return tempVal / 0x2710;
		}
		break;
		case 2:
			return (amount * nDupCNT);
		break;
		case 3:
		case 5:
		{
			dword tempVal = amount + 0x14;
			//tempVal *= client->Attr->Cha + 0x0a;
			tempVal *= 1 + 0x0a;
			tempVal *= (100 & 0xFFFF);//World Rate
			tempVal *= 0x14;//Fame + 0x14
			tempVal = ((tempVal / (client->Stats->Level + 0x46)) / 0x7530) + amount;
			return tempVal;
		}
		break;
		case 4:
		{
			dword tempVal = amount + 0x2;
			//tempVal *= (client->Attr->Cha + client->Stats->Level + 0x28);
			tempVal *= (1 + client->Stats->Level + 0x28);
			tempVal *= 0x28;//Fame + 0x28
			tempVal *= (100 & 0xFFFF);//World Rate
			return tempVal / 0x222E0;
		}
		break;
		case 6:
		{
			dword tempVal = amount + 0x14;
			//tempVal *= client->Attr->Cha + client->Stats->Level;
			tempVal *= 1 + client->Stats->Level;
			tempVal *= 0x14;//Fame + 0x14
			tempVal *= (100 & 0xFFFF);//World Rate
			return (tempVal / 0x2DC6C0) + amount;
		}
		break;
	}
	return amount;
}

//Update Quest
QUESTREWDC(000)
{
	return QUEST_SUCCESS;
}

//Update Quest Items
QUESTREWDC(001)
{
    GETREWDDATA(001);
    return QUEST_SUCCESS;
}

//Set Quest Variable
QUESTREWDC(002)
{
	return QUEST_SUCCESS;
}

//Update Stats
QUESTREWDC(003)
{
	return QUEST_SUCCESS;
}

//Set Quest Variable
QUESTREWDC(004)
{
	return QUEST_SUCCESS;
}

//Give Reward
QUESTREWDC(005)
{
	return QUEST_SUCCESS;
}

//Restore HP/MP
QUESTREWDC(006)
{
	return QUEST_SUCCESS;
}

//Teleport
QUESTREWDC(007)
{
	return QUEST_SUCCESS;
}

//Spawn Monster
QUESTREWDC(008){
    GETREWDDATA(008);


    /*if(data->iMonsterSN>=3050||data->iMonsterSN<=3089)
    {
        Log(MSG_WARNING,"xmas gifts");
    }*/

    LogDebug("BEGIN QUESTREWDC(008)");
    fPoint position;
    dword mapId;
    if(data->iX == 0 || data->iY == 0 || data->iZoneSN == 0)
    {
        position.x = client->Position->current.x;
        position.y = client->Position->current.y;
        mapId = client->Position->Map;
    }
    else
    {
        position.x = data->iX / 100;
        position.y = data->iY / 100;
        mapId = data->iZoneSN;
    }

    for(dword i = 0; i < data->iHowMany; i++)
    {
        fPoint pos = GServer->RandInCircle( position, data->iRange );
        CMap* map = GServer->MapList.Index[mapId];

        //monster
        //Log(MSG_INFO, "Spawn mob[%i] @ map %i (%f,%f)", data->iMonsterSN, mapId,pos.x, pos.y);
        CMonster* mon = map->AddMonster( data->iMonsterSN, pos );
        // ghost rackies = non aggresive, same for Xmas trees.
        if((data->iMonsterSN > 750 && data->iMonsterSN < 755) || (data->iMonsterSN >= 1474 && data->iMonsterSN <= 1489))
        {
            mon->thisnpc->aggresive = 0;
        }
        else
        {
            mon->thisnpc->aggresive = 999; // Force the mob to be agressive.
        }

        mon->lastSighCheck = 0; // Force sight check instantly.

        //LMA: some monsters belong to a PVP team ;)
        if(data->iTeamNo!=0)
        {
            mon->team=data->iTeamNo;
            LogDebug("QSDCA8::Monster %i has a team %i",mon->montype,mon->team);
        }

    }

    LogDebug("END QUESTREWDC(008)");
    return QUEST_SUCCESS;
}

//Execute Quest Trigger
QUESTREWDC(009){
	GETREWDDATA(009);
	char* tempName = reinterpret_cast<char*>(&data->szNextTriggerSN) - 2;
	dword hash = MakeStrHash(tempName);

	int is_ok=false;

    CMonster* monster = reinterpret_cast<CMonster*>(client);
    if(monster == NULL)
    {
        LogDebug("QUESTREWDC(009) failed (monster null)");
        return QUEST_FAILURE;
    }

	UINT savelma = monster->thisnpc->refNPC;
	is_ok=client->ExecuteQuestTrigger(hash);
	monster->thisnpc->refNPC=savelma;

	if(is_ok==QUEST_SUCCESS)
	{
	    LogDebug("QAC009:: Execute trigger %s (%u), returns true",tempName,hash);
	    return QUEST_SUCCESS;
	}
	else
	{
	    LogDebug("QAC009:: Execute trigger %s (%u), returns false",tempName,hash);
	    return QUEST_FAILURE;
	}

    return QUEST_SUCCESS;
}

//Reset Stats
QUESTREWDC(010)
{
	return QUEST_SUCCESS;
}

//Update Object Var
QUESTREWDC(011)
{
	GETREWDDATA(011);
	if(data->btWho == 0)
	{
	    //Npc
        CMonster* monster = reinterpret_cast<CMonster*>(client);
        if(monster == NULL)
        {
            LogDebug("QUESTREWDC(011) failed (monster null)");
            return QUEST_FAILURE;
        }

        short tempval = 0;

        //LMA: WarpGate or standard NPC?
        bool is_gate=false;

        if(monster->thisnpc->refNPC>10000&&monster->thisnpc->refNPC==GServer->WarpGate.virtualNpctypeID)
        {
            LogDebug("QSDAC:: Warpgate");
            //WarpGate.
            if(data->nVarNo>19)
                return QUEST_FAILURE;
            tempval = GServer->WarpGate.IfoObjVar[data->nVarNo];
            is_gate=true;
        }
        else
        {
            if(monster->thisnpc->refNPC>=MAX_NPC)
            {
                Log(MSG_WARNING,"It seems NPC %i>=%u , change MAX_NPC value!",monster->thisnpc->refNPC,MAX_NPC);
                return QUEST_FAILURE;
            }

            tempval = GServer->ObjVar[monster->thisnpc->refNPC][data->nVarNo];
        }

        LogDebug("QSDAC Set variable NPC %i, data->btOp=%i, data->iValue=%i, data->nVarNo=%i",monster->thisnpc->refNPC,data->btOp,data->iValue,data->nVarNo);

        switch(data->btOp)
        {
        case 5:
            tempval = data->iValue;
            break;
        case 6:
            tempval += data->iValue;
            break;
        case 7:
            tempval -= data->iValue;
            break;
        case 9:
            tempval++;
            break;
        default:
            return AI_FAILURE;
            break;
        }

        if(tempval < 0)tempval = 0;

        //WarpGate?
        if(is_gate)
        {
            short previous_val=GServer->WarpGate.IfoObjVar[data->nVarNo];
            GServer->WarpGate.IfoObjVar[data->nVarNo]=tempval;
            if(data->nVarNo==0&&previous_val!=tempval)
            {
                if(tempval==0)
                {
                    GServer->WarpGate.hidden=true;
                }
                else
                {
                    GServer->WarpGate.hidden=false;
                }

                //LMA: 2 do, check if really needed....
                //Forcing refresh.
                //GServer->pakSpawnIfoObject(NULL,GServer->WarpGate.virtualNpctypeID,true);
                BEGINPACKET( pak, 0x790 );
                ADDWORD    ( pak, GServer->WarpGate.clientID);
                ADDWORD    ( pak, tempval);
                GServer->SendToAllInMap(&pak,GServer->WarpGate.mapid);
            }

        }
        else
        {
            GServer->ObjVar[monster->thisnpc->refNPC][data->nVarNo] = tempval;
        }

		if(data->nVarNo==0)
		{
		    //LMA: event, we have to update, not here because we need the clientID :(
		    //mainprocess will do it for us ;)
		    monster->thisnpc->eventid=tempval;
		}

	}
	else if	(data->btWho == 1)
	{
	    //Event
        //LMA: For event Object.

	    //Npc
        CMonster* monster = reinterpret_cast<CMonster*>(client);
        if(monster == NULL)
        {
            LogDebug("QUESTREWDC(011) failed (monster null)");
            return QUEST_FAILURE;
        }

        short tempval = 0;

        if(monster->thisnpc->refNPC>10000&&monster->thisnpc->refNPC==GServer->WarpGate.virtualNpctypeID)
        {
            //WarpGate.
            if(data->nVarNo>19)
                return QUEST_FAILURE;
            tempval = GServer->WarpGate.IfoObjVar[data->nVarNo];
        }
        else
        {
            LogDebug("QSDAC11 Setting Unknown Object %i",monster->thisnpc->refNPC);
            return QUEST_FAILURE;
        }

        LogDebug("QSD Set variable Object Event %i, data->btOp=%i, data->iValue=%i, data->nVarNo=%i",monster->thisnpc->refNPC,data->btOp,data->iValue,data->nVarNo);

        switch(data->btOp)
        {
        case 5:
            tempval = data->iValue;
            break;
        case 6:
            tempval += data->iValue;
            break;
        case 7:
            tempval -= data->iValue;
            break;
        case 9:
            tempval++;
            break;
        default:
            return AI_FAILURE;
            break;
        }

        if(tempval < 0)tempval = 0;

        short previous_val=GServer->WarpGate.IfoObjVar[data->nVarNo];
        GServer->WarpGate.IfoObjVar[data->nVarNo]=tempval;
        if(data->nVarNo==0&&previous_val!=tempval)
        {
            if(tempval==0)
            {
                GServer->WarpGate.hidden=true;
            }
            else
            {
                GServer->WarpGate.hidden=false;
            }

            //LMA: 2 do, check if really needed....
            //Forcing refresh.
            //GServer->pakSpawnIfoObject(NULL,GServer->WarpGate.virtualNpctypeID,true);
            BEGINPACKET( pak, 0x790 );
            ADDWORD    ( pak, GServer->WarpGate.clientID);
            ADDWORD    ( pak, tempval);
            GServer->SendToAllInMap(&pak,GServer->WarpGate.mapid);
        }

	}


	return QUEST_SUCCESS;
}

//NPC Speak
QUESTREWDC(012)
{
	GETREWDDATA(012);

    if(data->iStrID>=GServer->maxltbqsd)
    {
        //Log(MSG_INFO,"QUESTREWDC(012) QSD LTB index error %i>=%i",data->iStrID,GServer->maxltbqsd);
        return QUEST_SUCCESS;
    }

    //LMA: We use the selected NPC, if any, else the AIP "caller"...
    CMonster* thisMonster = reinterpret_cast<CMonster*>(client);
    int good_npc=thisMonster->aip_npctype;
    if(thisMonster->thisnpc->refNPC!=0)
        good_npc=thisMonster->thisnpc->refNPC;

    //LMA: sometimes another NPC/monster calls another NPC to shoot.
    //we have to find the right one.
    int good_map=thisMonster->Position->Map;
    if (thisMonster->thisnpc->refNPC!=0&&thisMonster->thisnpc->refNPC!=thisMonster->aip_npctype)
    {
        CNPC* findNPC=GServer->GetNPCByType(thisMonster->thisnpc->refNPC);
        if(findNPC!=NULL)
        {
            good_map=findNPC->posMap;
            Log(MSG_WARNING,"QSDAC012:: map changed from %i to %i",thisMonster->Position->Map,good_map);
        }

    }

	if(data->btMsgType == 1)
	{
	    //LogDebug("%s shouts Nb %i::%s",GServer->LtbstringQSD[data->iStrID]->NPCname,data->iStrID,GServer->LtbstringQSD[data->iStrID]->LTBstring);
        //GServer->NPCShout(thisMonster,GServer->LtbstringQSD[data->iStrID]->LTBstring,GServer->LtbstringQSD[data->iStrID]->NPCname);
        //LogDebug("%s (%i) shouts Nb %i::%s",GServer->GetNPCNameByType(thisMonster->aip_npctype),thisMonster->aip_npctype,data->iStrID,GServer->LtbstringQSD[data->iStrID]->LTBstring);
        //GServer->NPCShout(thisMonster,GServer->LtbstringQSD[data->iStrID]->LTBstring,GServer->GetNPCNameByType(thisMonster->aip_npctype));

        LogDebug("%s (%i) shouts Nb %i::%s",GServer->GetSTLMonsterNameByID(good_npc),thisMonster->aip_npctype,data->iStrID,GServer->LtbstringQSD[data->iStrID]->LTBstring);
        //GServer->NPCShout(thisMonster,GServer->LtbstringQSD[data->iStrID]->LTBstring,GServer->GetSTLMonsterNameByID(good_npc));
        GServer->NPCShout(thisMonster,GServer->LtbstringQSD[data->iStrID]->LTBstring,GServer->GetSTLMonsterNameByID(good_npc),good_map);
	}
	else if(data->btMsgType == 2)
	{
	    //LogDebug("%s (%i) announces Nb %i::%s",GServer->GetNPCNameByType(thisMonster->aip_npctype),thisMonster->aip_npctype,data->iStrID,GServer->LtbstringQSD[data->iStrID]->LTBstring);
	    //GServer->NPCAnnounce(GServer->LtbstringQSD[data->iStrID]->LTBstring,GServer->GetNPCNameByType(thisMonster->aip_npctype));

	    LogDebug("%s (%i) announces Nb %i::%s",GServer->GetSTLMonsterNameByID(good_npc),thisMonster->aip_npctype,data->iStrID,GServer->LtbstringQSD[data->iStrID]->LTBstring);
	    //GServer->NPCAnnounce(GServer->LtbstringQSD[data->iStrID]->LTBstring,GServer->GetSTLMonsterNameByID(good_npc),thisMonster->Position->Map);
	    GServer->NPCAnnounce(GServer->LtbstringQSD[data->iStrID]->LTBstring,GServer->GetSTLMonsterNameByID(good_npc),good_map);
	}


	return QUEST_SUCCESS;
}

//Unknown
QUESTREWDC(013)
{
	return QUEST_SUCCESS;
}

//Learn Skill
QUESTREWDC(014)
{
	return QUEST_SUCCESS;
}

//Set Quest Flag
QUESTREWDC(015)
{
	return QUEST_SUCCESS;
}

//Unknown
QUESTREWDC(016)
{
	return QUEST_SUCCESS;
}

//Reset All Quest Flags
QUESTREWDC(017)
{
	return QUEST_SUCCESS;
}

//Send Announcement
QUESTREWDC(018)
{
	return QUEST_SUCCESS;
}

//Execute Quest Trigger in Other Map (credits PY).
//LMA: It seems it's not really the same way in osRose after all...
QUESTREWDC(019)
{
	//this is going to be a pain
	//it runs a quest trigger that would appear to be sent to every player in the designated zone

	//word nZoneNo;           //pos 0x00
	//word nTeamNo;           //pos 0x02
	//word nTriggerLength;    //pos 0x04 This is definitely NOT the length of the following name. PY
	//string TriggerName;     //pos 0x06 + nTriggerlength more bytes. Name always appears to be 17 bytes long
	//dword m_HashTrigger;
    //word m_HashTrigger;     //pos 0x17 only has 2 bytes not 4. PY

    // let's check the values first
    GETREWDDATA(019);

    char* tempName = reinterpret_cast<char*>(&data->TriggerName) - 2;
    dword hash = MakeStrHash(tempName);
    CMap* map = GServer->MapList.Index[data->nZoneNo];
    LogDebug("Execute Quest trigger: %s for zone %i",tempName, data->nZoneNo);

	if(map == NULL) return QUEST_SUCCESS;
	if( map->PlayerList.size()<1 ) return QUEST_SUCCESS;

    //LMA: We have to beware of loops, sometimes we warp a player so playerlist's size changes...
    vector<CPlayer*> temp_player;
	for(UINT j=0;j<map->PlayerList.size();j++)
    {
        if(map->PlayerList.at(j)->pvp_id!=data->nTeamNo)
            continue;

        temp_player.push_back(map->PlayerList.at(j));
    }

	for(UINT j=0;j<temp_player.size();j++)
    {
        Log(MSG_WARNING,"QSDAC019:: quest trigger %u for %s",hash,temp_player.at(j)->CharInfo->charname);
        temp_player.at(j)->ExecuteQuestTrigger(hash,true);
    }


    return QUEST_SUCCESS;
}

//PvP Status
QUESTREWDC(020)
{
	return QUEST_SUCCESS;
}

//Set Respawn Position
QUESTREWDC(021)
{
	return QUEST_SUCCESS;
}

//Unknown
QUESTREWDC(022)
{
	return QUEST_SUCCESS;
}

//Raise Clan Grade - This is not the least bit efficient - Drakia
QUESTREWDC(023)
{
	return QUEST_SUCCESS;
}

//Clan Money
QUESTREWDC(024)
{
	return QUEST_SUCCESS;
}

//Clan Points
QUESTREWDC(025)
{
	return QUEST_SUCCESS;
}

//Clan Skill
QUESTREWDC(026)
{
	return QUEST_SUCCESS;
}

//Clan Contribution
QUESTREWDC(027)
{
	return QUEST_SUCCESS;
}

//Clan Teleportation - Again, not efficient. Maybe keep a list of pointers to clan members? - Drakia
QUESTREWDC(028)
{
 	return QUEST_SUCCESS;
}

//LMA: Execute LUA Script, client side.
QUESTREWDC(029)
{
 	return QUEST_SUCCESS;
}

//Unspawn a NPC
QUESTREWDC(034)
{
    return QUEST_SUCCESS;
}


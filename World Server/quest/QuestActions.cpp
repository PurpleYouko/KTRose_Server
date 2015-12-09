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
#include "../worldserver.h"
dword GetRewardValue(dword function, dword amount, CPlayer* client, word nDupCNT){
	switch(function){
		case 0:
		{
			dword tempVal = amount + 0x1E;
            tempVal *= client->Attr->Cha + 0x0a; // We need to add a way to get all stats totals (Including buff, E, etc) - Drakia
			tempVal *= (100 & 0xFFFF);//World Rate
			tempVal *= 0x14;//Fame + 0x14
			tempVal = ((tempVal / (client->Stats->Level + 0x46)) / 0x7530) + amount;
			return tempVal;
		}
		break;
		case 1:
		{
			dword tempVal = (client->Stats->Level + 3) * amount;
			tempVal *= (client->Attr->Cha >> 1) + client->Stats->Level + 0x28;
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
			tempVal *= client->Attr->Cha + 0x0a;
			tempVal *= (100 & 0xFFFF);//World Rate
			tempVal *= 0x14;//Fame + 0x14
			tempVal = ((tempVal / (client->Stats->Level + 0x46)) / 0x7530) + amount;
			return tempVal;
		}
		break;
		case 4:
		{
			dword tempVal = amount + 0x2;
			tempVal *= (client->Attr->Cha + client->Stats->Level + 0x28);
			tempVal *= 0x28;//Fame + 0x28
			tempVal *= (100 & 0xFFFF);//World Rate
			return tempVal / 0x222E0;
		}
		break;
		case 6:
		{
			dword tempVal = amount + 0x14;
			tempVal *= client->Attr->Cha + client->Stats->Level;
			tempVal *= 0x14;//Fame + 0x14
			tempVal *= (100 & 0xFFFF);//World Rate
			return (tempVal / 0x2DC6C0) + amount;
		}
		break;
	}
	return amount;
}

//Update Quest
QUESTREWD(000)
{
	GETREWDDATA(000);

	switch(data->btOp){//0 remove, 1 start, 2 replace quest keep items, 3 replace quest delete items, 4 select
		case 0:
		{
            if (client->ActiveQuest == data->iQuestSN) client->ActiveQuest = 0;
            for(dword i = 0; i < 10; i++)
            {
                if(client->quest.quests[i].QuestID != data->iQuestSN) continue;

                //LMA: Special for UW :(
                if(data->iQuestSN>=2851&&data->iQuestSN<=2858)
                {
                    //getting the proof of rival extermination.
                    UINT zaward=5000*client->uw_kills;
                    client->uw_kills=0;
                    if (zaward>0)
                    {
                        BEGINPACKET(pak, 0x7a7);
                        ADDWORD(pak, client->clientid);
                        ADDWORD(pak, 0);
                        ADDBYTE(pak, 0);
                        ADDDWORD(pak, 0xccccccdf);
                        ADDDWORD(pak, zaward);
                        ADDDWORD( pak, 0xcccccccc );
                        ADDWORD ( pak, 0xcccc );
                        client->client->SendPacket(&pak);
                        client->CharInfo->Zulies += zaward;
                        Log(MSG_WARNING,"Giving %I64i zuly to player %s",zaward,client->CharInfo->charname);
                    }

                }
                //End of patch.

                memset(&client->quest.quests[i], 0, sizeof(SQuest));
                LogDebug("We tried to delete quest %i",data->iQuestSN);
                break;
            }

        }
		break;
		case 1:
		{
            if( client->questdebug )
                server->SendPM(client, "Start Quest: %u", data->iQuestSN);
			for(dword i = 0; i < 10; i++){
			  if(client->quest.quests[i].QuestID == data->iQuestSN) return QUEST_SUCCESS;
				if(client->quest.quests[i].QuestID != 0) continue;
				memset(&client->quest.quests[i], 0, sizeof(SQuest));
				client->quest.quests[i].QuestID = data->iQuestSN;
				client->quest.quests[i].StartTime = time(NULL);
				LogDebug("We tried to start quest %i",data->iQuestSN);
				break;
			}
			client->ActiveQuest = data->iQuestSN;
		}
		break;
		case 2:
		{
              if( client->questdebug )
                server->SendPM(client, "Replace Quest, keep items: %u", data->iQuestSN);
			for(dword i = 0; i < 10; i++){
				if(client->quest.quests[i].QuestID != client->ActiveQuest) continue;
				client->quest.quests[i].QuestID = data->iQuestSN;
				client->quest.quests[i].StartTime = time(NULL);
				LogDebug("We tried to replace quest %i with items",data->iQuestSN);
				break;
            }
			client->ActiveQuest = data->iQuestSN;
		}
		break;
		case 3:
		{
          if( client->questdebug )
            server->SendPM(client, "Replace Quest, delete items: %u", data->iQuestSN);
			for(dword i = 0; i < 10; i++){
				if(client->quest.quests[i].QuestID != client->ActiveQuest) continue;
				memset(&client->quest.quests[i], 0, sizeof(SQuest));
				client->quest.quests[i].QuestID = data->iQuestSN;
				client->quest.quests[i].StartTime = time(NULL);
				LogDebug("We tried to replace quest %i without items",data->iQuestSN);
				break;
            }
			client->ActiveQuest = data->iQuestSN;
		}
		break;
		case 4:
		{
            if( client->questdebug )
                server->SendPM(client, "Select Quest: %u", data->iQuestSN);
            LogDebug("We tried to select quest %i",data->iQuestSN);
			client->ActiveQuest = data->iQuestSN;
		}
		break;
	}
	return QUEST_SUCCESS;
}

//Update Quest Items
QUESTREWD(001)
{
    GETREWDDATA(001);
  if( client->questdebug )
    server->SendPM(client, "Update Quest Item (uiItemSN: %u btOp: %u nDupCNT: %u)", data->uiItemSN, data->btOp, data->nDupCNT);
	CItem tmpItem;

	//LMA: Correct way...
	/*tmpItem.itemtype = data->uiItemSN / 1000;
	tmpItem.itemnum = data->uiItemSN % 1000;*/
	tmpItem.itemtype = GServer->gi(data->uiItemSN,0);
	tmpItem.itemnum = GServer->gi(data->uiItemSN,1);
	tmpItem.count = data->nDupCNT;

	//Log(MSG_INFO,"QSD ACT1:: (%s) stuff on item %u*(%u::%u), %u",client->CharInfo->charname,tmpItem.count,tmpItem.itemtype,tmpItem.itemnum,data->btOp);

	SQuest* curQuest = client->GetActiveQuest();
	if(curQuest == NULL)
	{
	  LogDebug( "Couldn't find active quest, wtf? Id %u", client->ActiveQuest);
	  return QUEST_FAILURE;
    }

    //LMA: sometimes it's not a quest item...
    if (tmpItem.itemtype!=13)
    {
        int slot=0;

        if(data->btOp==0)
        {
            tmpItem.count = 0;
            slot = client->GetNewItemSlot( tmpItem );
            if (slot==0xffff||client->items[slot].count<data->nDupCNT)
            {
                Log(MSG_HACK,"QSD001: %s: Not enough items to be deleted or bad slot (%u) for item %i*%u::%u (%u)!",client->CharInfo->charname,slot,tmpItem.count,tmpItem.itemtype,tmpItem.itemnum,data->uiItemSN);
                return QUEST_FAILURE;
            }

            client->items[slot].count-=data->nDupCNT;
            if (client->items[slot].count==0)
            {
                ClearItem(client->items[slot]);
                client->UpdateInventory(slot,0xffff,true);
            }

        }
        else
        {
            tmpItem.durability = 50;
            tmpItem.gem = 0;
            tmpItem.stats = 0;
            tmpItem.refine = 0;
            tmpItem.socketed = 0;
            tmpItem.lifespan = 100;
            tmpItem.appraised = 1;

            tmpItem.sp_value=0;
            tmpItem.last_sp_value=0;

            if(tmpItem.itemtype==14)
            {
                tmpItem.sp_value=tmpItem.lifespan*10;
                tmpItem.last_sp_value=tmpItem.sp_value;
            }

            slot=client->AddItem(tmpItem);
            if(slot==0xffff)
            {
                Log(MSG_WARNING,"QSDA1:: Impossible to add item in player's inventory");
                return QUEST_FAILURE;
            }

            //let's update player's inventory.
            client->UpdateInventory(slot,0xffff,true);
        }

    }
    else
    {
        UINT nb_items=curQuest->AddItem(&tmpItem, data->btOp);
        //LMA: Special for Union War.
        if (tmpItem.itemnum==496)
        {
            client->uw_kills=nb_items;
        }

    }


    return QUEST_SUCCESS;
}

//Set Quest Variable
QUESTREWD(002){
    GETREWDDATA(002);
	for(dword i = 0; i < data->iDataCnt; i++){
		dword address = i * sizeof(STR_QUEST_DATA);
		address += (dword)data;
		address += 4;
		STR_QUEST_DATA* curQst = (STR_QUEST_DATA*)address;
    if( client->questdebug )
      server->SendPM(client, "Set quest var[%#04x][%i] - %i (Op: %i)", curQst->m_wVarTYPE, curQst->m_wVarNO, curQst->nValue, curQst->btOp );

		word nValue = curQst->nValue;
		word tempValue = client->GetQuestVar(curQst->m_wVarTYPE, curQst->m_wVarNO);
		OperateValues<word>(curQst->btOp, &tempValue, curQst->nValue);
		client->SetQuestVar(curQst->m_wVarTYPE, curQst->m_wVarNO, tempValue);
    }

	return QUEST_SUCCESS;
}

//Update Stats
QUESTREWD(003)
{
	GETREWDDATA(003);
	for(dword i = 0; i < data->iDataCnt; i++){
		dword address = i * 0x0C;
		address += (dword)data;
		address += 4;
		STR_ABIL_DATA* curAbil = (STR_ABIL_DATA*)address;
    if( client->questdebug )
      server->SendPM(client, "Update stat %i - %i (Op: %i)", curAbil->iType, curAbil->iValue, curAbil->btOp);

		switch( curAbil->iType )
		{
        case sHair:
        {
            //LMA: changing face.
            if(!OperateValues<int>(curAbil->btOp, (int*)&client->CharInfo->Hair, curAbil->iValue))
				return QUEST_FAILURE;
            LogDebug("QSDA3 setting Hair to %i",client->CharInfo->Hair);

            //packet time.
            //CharInfo->Hair = (thisquest->value3*5);
            BEGINPACKET(pak, 0x721);
            ADDWORD(pak, 9);
            ADDWORD(pak, client->CharInfo->Hair);
            ADDWORD(pak, 0);
            client->client->SendPacket(&pak);
            RESETPACKET(pak, 0x0730);
            ADDWORD(pak, 5);
            ADDWORD(pak, 0xa24d);
            ADDWORD(pak, 0x40b3);
            client->client->SendPacket(&pak);
            GServer->DB->QExecute("UPDATE characters SET hairStyle=%i WHERE id=%i", client->CharInfo->Hair, client->CharInfo->charid);

        }
        break;
        case sFace:
        {
            //LMA: Changing Face.
            if(!OperateValues<int>(curAbil->btOp, (int*)&client->CharInfo->Face, curAbil->iValue))
				return QUEST_FAILURE;
            LogDebug("QSDA3 setting Face to %i",client->CharInfo->Face);

            BEGINPACKET(pak, 0x721);
            ADDWORD(pak, 8);
            ADDWORD(pak, client->CharInfo->Face);
            ADDWORD(pak, 0);
            client->client->SendPacket(&pak);
            RESETPACKET(pak, 0x0730);
            ADDWORD(pak, 5);
            ADDWORD(pak, 0xa24d);
            ADDWORD(pak, 0x40b3);
            client->client->SendPacket(&pak);
            GServer->DB->QExecute("UPDATE characters SET face=%i WHERE id=%i", client->CharInfo->Face, client->CharInfo->charid);
        }
        break;
        case sReputation:
        {
            //LMA: Union Fame
            if(!OperateValues<int>(curAbil->btOp, (int*)&client->CharInfo->unionfame, curAbil->iValue))
				return QUEST_FAILURE;
            LogDebug("QSDA3 setting Fame to %i (NOT COMPLETE)",client->CharInfo->unionfame);
        }
        break;
        case sSkillPoints:
        {
            //LMA: Skill Points.
            if(!OperateValues<int>(curAbil->btOp, (int*)&client->CharInfo->SkillPoints, curAbil->iValue))
				return QUEST_FAILURE;
            LogDebug("QSDA3 setting skillpoints to %i",client->CharInfo->SkillPoints);
            BEGINPACKET( pak, 0x720 );
            ADDWORD( pak, 37 );
            ADDWORD( pak, client->CharInfo->SkillPoints );
            ADDWORD( pak, 0 );
            client->client->SendPacket( &pak );
            RESETPACKET( pak, 0x0730 );
            ADDWORD( pak, 5 );
            ADDWORD( pak, 0xa24d );
            ADDWORD( pak, 0x40b3 );
            client->client->SendPacket( &pak );
        }
        break;
        case sPvp:
        {
            //LMA: PVP Case.
            if(!OperateValues<int>(curAbil->btOp, (int*)&client->pvp_id, curAbil->iValue))
				return QUEST_FAILURE;

			//LMA: We have to send a packet
            BEGINPACKET( pak, 0x721 );
            ADDWORD    ( pak, 0x0022 );
            ADDWORD    ( pak, client->pvp_id );
            ADDWORD    ( pak, 0x0000 );
            client->client->SendPacket( &pak );

            client->pvp_status=-1;

            LogDebug("QSDA3 setting pvp to %i",client->pvp_id);
        }
        break;
		case sGender:
		{
			if(!OperateValues<int>(curAbil->btOp, (int*)&client->CharInfo->Sex, curAbil->iValue))
				return QUEST_FAILURE;

            LogDebug("QSDA3 setting Gender to %i",client->CharInfo->Sex);

            //LMA: Adding packet.
            BEGINPACKET( pak, 0x720 );
            ADDWORD( pak, 2 );
            ADDWORD( pak, client->CharInfo->Sex );
            ADDWORD( pak, 0 );
            client->client->SendPacket( &pak );
            RESETPACKET( pak, 0x0730 );
            ADDWORD( pak, 5 );
            ADDWORD( pak, 0xa24d );
            ADDWORD( pak, 0x40b3 );
            client->client->SendPacket( &pak );
            GServer->DB->QExecute("UPDATE characters SET sex=%i WHERE id=%i", client->CharInfo->Sex, client->CharInfo->charid);
		}
        break;
		case sJob:
		{
			if(!OperateValues<int>(curAbil->btOp, (int*)&client->CharInfo->Job, curAbil->iValue))
				return QUEST_FAILURE;
			BEGINPACKET(pak, 0x721 );
			ADDWORD(pak, curAbil->iType );
			ADDDWORD(pak, curAbil->iValue );
			client->client->SendPacket( &pak );
		}
        break;
		case sUnion:
		{
		    //Log(MSG_INFO,"[Union] ? QUESTREWD(003) btOp %i, value: %i",curAbil->btOp,curAbil->iValue);
		    if(!OperateValues<int>(curAbil->btOp, (int*)&client->CharInfo->unionid, curAbil->iValue))
				return QUEST_FAILURE;
            //Log(MSG_INFO,"[Union] OK QUESTREWD(003) SET to %i",curAbil->iValue);
            BEGINPACKET( pak, 0x721 );
            ADDWORD( pak, 0x05 );
            ADDWORD( pak, curAbil->iValue );
            ADDWORD( pak, 0x0000 );
            client->client->SendPacket(&pak);
		}
			break;
        case 81:    //LMA: Union Points (no break, it's NOT a mistake)...
        {
            if (curAbil->iType==81)
            {
                client->CharInfo->union01+=curAbil->iValue;
                client->CharInfo->union01+=curAbil->iValue*client->pc_up/100;
            }

        }
        case 82:
        {
            if (curAbil->iType==82)
            {
                client->CharInfo->union02+=curAbil->iValue;
                client->CharInfo->union02+=curAbil->iValue*client->pc_up/100;
            }

        }
        case 83:
        {
            if (curAbil->iType==83)
            {
                client->CharInfo->union03+=curAbil->iValue;
                client->CharInfo->union03+=curAbil->iValue*client->pc_up/100;
            }

        }
        case 84:
        {
            if (curAbil->iType==84)
            {
                client->CharInfo->union04+=curAbil->iValue;
                client->CharInfo->union04+=curAbil->iValue*client->pc_up/100;
            }

        }
        case 85:
        {
            if (curAbil->iType==85)
            {
                client->CharInfo->union05+=curAbil->iValue;
                client->CharInfo->union05+=curAbil->iValue*client->pc_up/100;
            }

            Log(MSG_INFO,"[UnionPoints] ? QUESTREWD(003) btOp %i, value: %i",curAbil->btOp,curAbil->iValue);
            //BEGINPACKET( pak, 0x721 );
            BEGINPACKET( pak, 0x720 );
            ADDWORD( pak, curAbil->iType );
            ADDWORD( pak, curAbil->iValue );
            ADDWORD( pak, 0x0000 );
            client->client->SendPacket( &pak );
            Log(MSG_INFO,"[UnionPoints] OK QUESTREWD(003) SET union0%i adding %i (new value %i,%i,%i,%i,%i)",curAbil->iType,curAbil->iValue,client->CharInfo->union01,client->CharInfo->union02,client->CharInfo->union03,client->CharInfo->union04,client->CharInfo->union05);
        }
        break;
		case sStrength:
			if(!OperateValues<int>(curAbil->btOp, (int*)&client->Attr->Str, curAbil->iValue))
				return QUEST_FAILURE;
			break;

		case sDexterity:
			if(!OperateValues<int>(curAbil->btOp, (int*)&client->Attr->Dex, curAbil->iValue))
				return QUEST_FAILURE;
			break;

		case sIntelligence:
			if(!OperateValues<int>(curAbil->btOp, (int*)&client->Attr->Int, curAbil->iValue))
				return QUEST_FAILURE;
			break;

		case sConcentration:
			if(!OperateValues<int>(curAbil->btOp, (int*)&client->Attr->Con, curAbil->iValue))
				return QUEST_FAILURE;
			break;

		case sCharm:
			if(!OperateValues<int>(curAbil->btOp, (int*)&client->Attr->Cha, curAbil->iValue))
				return QUEST_FAILURE;
			break;

		case sSensibility:
			if(!OperateValues<int>(curAbil->btOp, (int*)&client->Attr->Sen, curAbil->iValue))
				return QUEST_FAILURE;
			break;

		case sLevel:
			if(!OperateValues<int>(curAbil->btOp, (int*)&client->Stats->Level, curAbil->iValue))
				return QUEST_FAILURE;
			break;

		case sStatPoints:
			if(!OperateValues<int>(curAbil->btOp, (int*)&client->CharInfo->StatPoints, curAbil->iValue))
				return QUEST_FAILURE;
			break;
        case sMoney:
        {
            if(!OperateValues<int>(curAbil->btOp, (int*)&client->CharInfo->Zulies, curAbil->iValue))
                return QUEST_FAILURE;

            //LMA: do we really have to send it?
            //we don't send it because most of the time the client will check again and if not enough, the check
            //will fail (for example, taking union).
            //it seems that from narose 228 we need it again...
			// Send zuly update packet
            BEGINPACKET( pak, 0x71d );
            ADDQWORD( pak, client->CharInfo->Zulies );
            client->client->SendPacket( &pak );
        }
            break;
		default:
		{
			Log(MSG_WARNING, "Type Unknown: '%i'", curAbil->iType);
		}
        break;

		}
	}
	return QUEST_SUCCESS;
}

//Set Quest Variable
QUESTREWD(004){
	return QUEST_REWD_002(server, client, raw);
}

//Give Reward
QUESTREWD(005){
	GETREWDDATA(005);
	switch(data->btTarget){
		case 0://EXP
		{
            client->CharInfo->Exp += GetRewardValue(data->btEquation, data->iValue, client, 0);

            //LMA: Only if not level up
            if(client->CharInfo->Exp<client->GetLevelEXP())
            {
                BEGINPACKET(pak, 0x79b);
                ADDDWORD(pak, client->CharInfo->Exp);
                ADDDWORD(pak, client->CharInfo->stamina);
                ADDWORD (pak, 0);
                client->client->SendPacket(&pak);
            }

        }
		break;
		case 1://Zuly
		{
			client->CharInfo->Zulies += GetRewardValue(data->btEquation, data->iValue, client, 1);//dunno nDupCount for this one!
			BEGINPACKET(pak, 0x71d);
			ADDQWORD(pak, client->CharInfo->Zulies);
			client->client->SendPacket(&pak);
		}
		break;

        case 2://Item
        {
            CItem nItem;
            //LMA: Correct way.
            /*nItem.itemtype = data->iItemSN / 1000;
            nItem.itemnum = data->iItemSN % 1000;*/
            nItem.itemtype = GServer->gi(data->iItemSN,0);
            nItem.itemnum = GServer->gi(data->iItemSN,1);

            if(nItem.IsStackable())
            {
                nItem.count = GetRewardValue(data->btEquation, data->iValue, client, 0);
            }
            else
            {
                nItem.count = 1;
            }

            //Maxxon: How is durability done in Evo?
            //nItem.durability = GServer->EquipList[nItem.itemtype].Index[nItem.itemnum]->d;
            nItem.durability = 50;
            nItem.gem = 0;
            nItem.stats = 0;
            nItem.refine = 0;
            nItem.socketed = 0;
            nItem.lifespan = 100;
            nItem.appraised = 1;

            nItem.sp_value=0;
            nItem.last_sp_value=0;

            if(nItem.itemtype==14)
            {
                nItem.sp_value=nItem.lifespan*10;
                nItem.last_sp_value=nItem.sp_value;
            }

            dword slot = client->AddItem(nItem);
            if (slot == 0xffff)
            { // Fail
                BEGINPACKET( pak, 0x7a7);
                ADDWORD(pak, 0x00);
                ADDBYTE(pak, 0x03);
                ADDBYTE(pak, 0x00);
                client->client->SendPacket(&pak);
            }
            else
            {
                client->UpdateInventory(slot,0xffff,true);
            }
        }
        break;

	}

	return QUEST_SUCCESS;
}

//Restore HP/MP
QUESTREWD(006)
{
	GETREWDDATA(006);
	//Log(MSG_WARNING,"We give %i HP and %i MP back to %s",data->iPercentOfHP,data->iPercentOfMP,client->CharInfo->charname);
	client->Stats->HP = (long int)((float)client->Stats->MaxHP / 100.0f) * data->iPercentOfHP;
	client->Stats->MP = (long int)((float)client->Stats->MaxHP / 100.0f) * data->iPercentOfMP;

	return QUEST_SUCCESS;
}

//Teleport
QUESTREWD(007)
{
	GETREWDDATA(007);
	fPoint thispoint;
	thispoint.x = floor(((float)data->iX)/100);
	thispoint.y = floor(((float)data->iY)/100);

	//LMA: Warping party members too.
	if(data->btPartyOpt!=0&&client->Party->party!=NULL)
	{
        for(UINT i=0;i<client->Party->party->Members.size( );i++)
        {
            CPlayer* member = client->Party->party->Members.at(i);

            //we'll warp our lad in last :)
            if(client->clientid!=member->clientid)
            {
                GServer->TeleportTo(member, data->iZoneSN, thispoint);
            }

        }

	}

	//LMA: Special case, if player isn't in game right now, it can happend for qsds called when warped in maps.
	//In this case, the warp should be done AFTER the pakdoid packet.
	if(!client->Session->inGame)
	{
	    client->map_warp_zone=data->iZoneSN;
	    client->Warp_Zone.x=thispoint.x;
	    client->Warp_Zone.y=thispoint.y;
	    //Log(MSG_WARNING,"QSDA007:: Player %s should be warped another time to %i (%u:%u)",client->CharInfo->charname,client->map_warp_zone,client->Warp_Zone.x,client->Warp_Zone.y);
	    return QUEST_SUCCESS;
	}

	GServer->TeleportTo(client, data->iZoneSN, thispoint);


	return QUEST_SUCCESS;
}

//Spawn Monster
QUESTREWD(008)
{
    GETREWDDATA(008);

    LogDebug("BEGIN QUESTREWD(008)");
    fPoint position;
  dword mapId;
  if(data->iX == 0 || data->iY == 0 || data->iZoneSN == 0){
    position.x = client->Position->current.x;
    position.y = client->Position->current.y;
    mapId = client->Position->Map;
  }else{
        position.x = data->iX / 100;
        position.y = data->iY / 100;
    mapId = data->iZoneSN;
  }
	for(dword i = 0; i < data->iHowMany; i++)
	{
		fPoint pos = GServer->RandInCircle( position, data->iRange );

        //LMA: coming from AIP.
        //if( client->questdebug&&!client->is_invisible)
        //server->SendPM(client, "Spawn mob[%i] @ %f, %f", data->iMonsterSN, pos.x, pos.y);
        //Log(MSG_INFO, "Spawn mob[%i] @ map %i (%f,%f)", data->iMonsterSN, mapId,pos.x, pos.y);
		CMap* map = GServer->MapList.Index[mapId];
		CMonster* mon = map->AddMonster( data->iMonsterSN, pos );
		if(data->iMonsterSN > 750 && data->iMonsterSN < 755) // ghost rackies = non aggresive
        {
            mon->thisnpc->aggresive = 0;
        } else {
            mon->thisnpc->aggresive = 999; // Force the mob to be agressive.
        }
		mon->lastSighCheck = 0; // Force sight check instantly.

        //LMA: some monsters belong to a PVP team ;)
        if(data->iTeamNo!=0)
        {
            mon->team=data->iTeamNo;
            if(i==0)
            {
                //We display the msg just once :)
                LogDebug("QSDA8::Monster %i has a team %i",mon->montype,mon->team);
            }

        }

    }

    LogDebug("END QUESTREWD(008)");
	return QUEST_SUCCESS;
}

//Execute Quest Trigger
QUESTREWD(009){
	GETREWDDATA(009);
	char* tempName = reinterpret_cast<char*>(&data->szNextTriggerSN) - 2;
	dword hash = MakeStrHash(tempName);
  if( client->questdebug )
  {
    server->SendPM(client, "Execute Quest Trigger %s[%d] [%08x]", tempName, data->shNameLen, hash);
  }

    int tempval=client->quest.RefNPC;
    int is_ok=client->ExecuteQuestTrigger(hash,true);
    client->quest.RefNPC=tempval;

    return is_ok;
}

//Reset Stats
QUESTREWD(010){
  if( client->questdebug )
    server->SendPM(client, "Reset Stats");
	client->CharInfo->StatPoints = 0;

	client->Attr->Str = 15;
	client->Attr->Dex = 15;
	client->Attr->Int = 15;
	client->Attr->Con = 15;
	client->Attr->Cha = 10;
	client->Attr->Sen = 10;

	/*for(int i = 2; i <= client->Stats->Level; i++) {
		client->CharInfo->StatPoints += 10;
		client->CharInfo->StatPoints += i / 2;
	}*/

	//LMA: new formula from Sousie
    for(int i = 2; i <= client->Stats->Level; i++)
    {
        client->CharInfo->StatPoints += 9;
        client->CharInfo->StatPoints += i - ((i - 1) / 5);
    }

	return QUEST_SUCCESS;
}

//Update Object Var
//LMA: like AIP.
QUESTREWD(011)
{
	GETREWDDATA(011);
	if(data->btWho == 0)
	{
        short tempval = 0;
        int refNPC = client->quest.RefNPC;
        if(refNPC==0)
        {
            Log(MSG_WARNING,"QSDA11::Np NPC selected");
            return QUEST_FAILURE;
        }

        //LMA: WarpGate or standard NPC?
        bool is_gate=false;

        if(refNPC>10000&&refNPC==GServer->WarpGate.virtualNpctypeID)
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
            if(refNPC>=MAX_NPC)
            {
                Log(MSG_WARNING,"It seems NPC %i>=%u , change MAX_NPC value!",refNPC,MAX_NPC);
                return QUEST_FAILURE;
            }

            tempval = GServer->ObjVar[refNPC][data->nVarNo];
        }

        LogDebug("QSDA11 Set variable NPC %i, data->btOp=%i, data->iValue=%i, data->nVarNo=%i",refNPC,data->btOp,data->iValue,data->nVarNo);

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
            GServer->ObjVar[refNPC][data->nVarNo] = tempval;
        }

		if(data->nVarNo==0)
		{
		    //LMA: Oh boy...
		    //TODO: check if we need to change eventId for ALL npc sharing this npctype?
            if(client->Position->Map==0||client->Position->Map>=GServer->MapList.max)
            {
                Log(MSG_WARNING,"QSDA11: Player %s is in an incorrect map %i",client->CharInfo->charname,client->Position->Map);
                return QUEST_FAILURE;
            }

            CMap* thisMap = GServer->MapList.Index[client->Position->Map];
            CNPC* thisNpc = thisMap->GetNPCInMapQSD(refNPC);
            if(thisNpc==NULL)
            {
                Log(MSG_WARNING,"QSDA11 Can't select NPC %i to change its eventID to %i",refNPC,tempval);
                LogDebug("QSDA11 Can't select NPC %i to change its eventID to %i",refNPC,tempval);
                //quest success for now...
                return QUEST_SUCCESS;
            }

            BEGINPACKET( pak, 0x790 );
            ADDWORD    ( pak, thisNpc->clientid);
            ADDWORD    ( pak, tempval);
            GServer->SendToAllInMap(&pak,GServer->WarpGate.mapid);
		    Log(MSG_WARNING,"QSDA11 NPC %i should have changed his eventID to %i",refNPC,tempval);
		    LogDebug("QSDA11 NPC %i should have changed his eventID to %i",refNPC,tempval);
		}

	}
	else if	(data->btWho == 1)
	{
        //LMA: For event Object.

	    //Npc
        int refNPC = client->quest.RefNPC;
        if(refNPC==0)
        {
            Log(MSG_WARNING,"QSDA11::Np NPC selected");
            return QUEST_FAILURE;
        }

        short tempval = 0;

        if(refNPC>10000&&refNPC==GServer->WarpGate.virtualNpctypeID)
        {
            //WarpGate.
            if(data->nVarNo>19)
                return QUEST_FAILURE;
            tempval = GServer->WarpGate.IfoObjVar[data->nVarNo];
        }
        else
        {
            LogDebug("QSDA11 Setting Unknown Object %i",refNPC);
            return QUEST_FAILURE;
        }

        LogDebug("QSDA11 Set variable Object Event %i, data->btOp=%i, data->iValue=%i, data->nVarNo=%i",refNPC,data->btOp,data->iValue,data->nVarNo);

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
QUESTREWD(012){
	/*if(entity->_EntityType != ENTITY_NPC) return QUEST_FAILURE;
	GETREWDDATA(012);

	CNpc* thisNpc = reinterpret_cast<CNpc*>(entity);
	if(thisNpc->SelectedNpc == NULL) return QUEST_FAILURE;
	std::map<dword, char*>::iterator triggerITR = server->NpcNames.find(thisNpc->SelectedNpc->NpcID);
	if(triggerITR == server->NpcNames.end()) return QUEST_FAILURE;

	char* npcName = triggerITR->second;
	char* sayStr = server->lngQst->GetLTBStr(data->iStrID);
	if(sayStr == NULL) return QUEST_FAILURE;
	if(data->btMsgType == 1){
		//Shout
		CPacket pakout(0x785);
		pakout.Add<string>(npcName);
		pakout.Add<string>(sayStr);
		server->SendPacketToZone(thisNpc, &pakout);
	}else if(data->btMsgType == 2){
		//Announce
		CPacket pakout(0x702);
		pakout.AddBytes((byte*)npcName, strlen(npcName));
		pakout.Add<byte>('>');
		pakout.Add<string>(sayStr);
		server->SendPacketToZone(thisNpc, &pakout);
	}
	delete [] sayStr;

	return QUEST_SUCCESS;*/
	//Log(MSG_WARNING,"A NPC TRYES TO SHOUT!!!");
	return QUEST_FAILURE; // This is cool and all, but we lack the stuff to do it - Drakia
}

/*
//Unknown
QUESTREWD(013){
	return QUEST_SUCCESS;
}
*/

//PY: Execute Quest Trigger
QUESTREWD(013)
{
	//	byte btWho               pos 0x00
	//	int iWho                 pos 0x00
	//dword iSec                 pos 0x02
	//word nNameLength           pos 0x06
	//string szTriggerName       pos 0x08
	//dword m_HashNextTrigger    pos 0x06 + nNameLength?
	//looks like a quest trigger to me
	GETREWDDATA(013);
	char* tempName = reinterpret_cast<char*>(&data->szTriggerName) - 2;
	dword hash = MakeStrHash(tempName);
    if( client->questdebug )
    {
        server->SendPM(client, " RWD 013 pretty sure this -- Execute Quest Trigger %s[%d] [%08x]", tempName, data->nNameLength, hash);
    }

    int tempval=client->quest.RefNPC;
    int is_ok=client->ExecuteQuestTrigger(hash,true);
    client->quest.RefNPC=tempval;


    return is_ok;
}

//Learn Skill
QUESTREWD(014){
    GETREWDDATA(014);
  if( client->questdebug )
  {
    server->SendPM(client, "Learn skill: %i", data->iSkillNo);
  }

    if(!GServer->LearnSkill(client, data->iSkillNo, false))
    {
        Log(MSG_WARNING,"QSDA014:: Player %s failed to learn skill %i",client->CharInfo->charname,data->iSkillNo);
    }

    //	GServer->LearnSkill(client, data->iSkillNo);


	return QUEST_SUCCESS;
}

//Set Quest Flag
QUESTREWD(015){
	GETREWDDATA(015);
  if( client->questdebug )
    server->SendPM(client, "Set QFlag[%i]=%i", data->nSN, data->btOp);
	client->quest.SetFlag(data->nSN, (data->btOp == 1)?true:false);
	return QUEST_SUCCESS;
}

//Unknown
QUESTREWD(016){
	return QUEST_SUCCESS;
}

//Reset All Quest Flags
QUESTREWD(017){
  if( client->questdebug )
    server->SendPM(client, "Reset all quest flags");
	memset(&client->quest.flags, 0, 64);
	return QUEST_SUCCESS;
}

//Send Announcement
QUESTREWD(018){
    GETREWDDATA(018);
	return QUEST_SUCCESS;
}

//Execute Quest Trigger in Other Map
QUESTREWD(019){
    LogDebug("QSDA19 Not coded for players");
	return QUEST_SUCCESS;
}

//PvP Status
QUESTREWD(020)
{
    //LMA: We use it now :)
    GETREWDDATA(020);
    client->pvp_status=data->btNoType;
    client->pvp_id=-1;


	return QUEST_SUCCESS;
}

//Set Respawn Position (Credits PY)
QUESTREWD(021)
{
    GETREWDDATA(021);
    //LMA: We change it a bit :)
    client->Position->saved = client->Position->Map;

    //LMA: We change it a bit :)
    float X=(float)(data->iX)/((float)100);
    float Y=(float)(data->iY)/((float)100);
    client->UWPosition->Map=client->Position->Map;
    client->UWPosition->source.x=X;
    client->UWPosition->source.y=Y;
    client->UWPosition->source.z=0;


	return QUEST_SUCCESS;
}

//Unknown
QUESTREWD(022){
	return QUEST_SUCCESS;
}

//Raise Clan Grade - This is not the least bit efficient - Drakia
QUESTREWD(023){
    GETREWDDATA(023);
  if( client->questdebug )
    server->SendPM(client, "Raise clan grade");
    for(UINT i=0;i<GServer->MapList.Map.size();i++)
    {
        CMap* map = GServer->MapList.Map.at(i);
        if( map->PlayerList.size()<1 )
            continue;
        for(UINT j=0;j<map->PlayerList.size();j++)
        {
        CPlayer* player = map->PlayerList.at(j);
        if( player->Clan->clanid != client->Clan->clanid ) continue;

        //LMA: Getting real grade
        player->Clan->grade=GServer->getClanGrade(player->Clan->clanid);
        player->Clan->grade++;

        //load clan info in char server
	    BEGINPACKET( pak, 0x7e0 );
	    ADDBYTE    ( pak, 0xfb ); //action to update clan information (charserver)
	    //ADDWORD    ( pak, client->CharInfo->charid );   //LMA: adding client ID.
	    ADDDWORD    ( pak, client->CharInfo->charid );   //LMA: adding client ID.
	    //ADDWORD    ( pak, player->Clan->clanid ); //LMA: should be useless...
	    ADDWORD    ( pak, player->Clan->grade );
        //GServer->SendISCPacket( &pak );
		//Log (MSG_NOTICE, "implement CharServer communication in __FILE__ line __LINE__");
        cryptPacket( (char*)&pak, GServer->cct );
        send( GServer->csock, (char*)&pak, pak.Size, 0 );

        //Send to other players
        RESETPACKET( pak, 0x7e0 );
        ADDBYTE    ( pak, 0x35 );
        ADDWORD    ( pak, player->clientid );
        ADDWORD    ( pak, player->Clan->clanid);
        ADDWORD    ( pak, 0x0000 );//???
        ADDWORD    ( pak, player->Clan->back );
        ADDWORD    ( pak, player->Clan->logo );
        ADDBYTE    ( pak, player->Clan->grade );//clan grade
        ADDBYTE    ( pak, 0x06 );//clan rank
        ADDSTRING  ( pak, player->Clan->clanname );
        ADDBYTE    ( pak, 0x00 );
        GServer->SendToVisible( &pak, player );
        }
    }
    GServer->DB->QExecute("UPDATE list_clan SET grade=%i WHERE id=%i", client->Clan->grade, client->Clan->clanid);
	return QUEST_SUCCESS;
}

//Clan Money
QUESTREWD(024)
{
    //LMA: never used.
	return QUEST_SUCCESS;
}

//Clan Points
QUESTREWD(025)
{
    //LMA: let's add or delete clan points ;)
    GETREWDDATA(025);
    int points=data->nPOINT;

    //Loading "real" CP points.
    client->Clan->CP=GServer->getClanPoints(client->Clan->clanid);

    //we "delete" points.
    if(data->btOP==7)
    {
        points=-1*points;
    }

    if (client->Clan->CP+points<0)
    {
        client->Clan->CP=0;
    }
    else
    {
        client->Clan->CP+=points;
    }

    //sending packet.
    BEGINPACKET( pak, 0x7e0 );
    ADDBYTE    ( pak, 0xfe );
    ADDDWORD    ( pak, client->CharInfo->charid);  //charid
    ADDDWORD    ( pak, points);  //Clan points (to be added)
    cryptPacket( (char*)&pak, GServer->cct );
    send( GServer->csock, (char*)&pak, pak.Size, 0 );

    Log(MSG_WARNING,"We added %i CP",points);

    RESETPACKET( pak, 0x7e0 );
    ADDBYTE    ( pak, 0x35 );
    ADDWORD    ( pak, client->clientid );
    ADDWORD    ( pak, client->Clan->clanid );
    ADDWORD    ( pak, 0x0000 );
    ADDWORD    ( pak, client->Clan->back );
    ADDWORD    ( pak, client->Clan->logo );
    ADDBYTE    ( pak, client->Clan->grade );
    ADDBYTE    ( pak, client->Clan->clanrank);
    ADDSTRING  ( pak, client->Clan->clanname );
    ADDBYTE    ( pak, 0x00 );
    client->client->SendPacket(&pak);


	return QUEST_SUCCESS;
}

//Clan Skill
QUESTREWD(026){
	return QUEST_SUCCESS;
}

//Clan Contribution
QUESTREWD(027){
	return QUEST_SUCCESS;
}

//Clan Teleportation - Again, not efficient. Maybe keep a list of pointers to clan members? - Drakia
QUESTREWD(028){
    GETREWDDATA(028);
    fPoint telepos;
    fPoint newPos;
    telepos.x = data->iX/100;
    telepos.y = data->iY/100;
  if( client->questdebug )
    server->SendPM(client, "Clan teleport to [%i][%f][%f]", data->nZoneNo, telepos.x, telepos.y);
    for(UINT i=0;i<GServer->MapList.Map.size();i++)
    {
        CMap* map = GServer->MapList.Map.at(i);
        if( map->PlayerList.size()<1 )
            continue;
        for(UINT j=0;j<map->PlayerList.size();j++)
        {
            CPlayer* player = map->PlayerList.at(j);
            if( player->Clan->clanid != client->Clan->clanid ) continue;
            newPos = GServer->RandInCircle(telepos, data->iRange);
            GServer->TeleportTo(player, data->nZoneNo, newPos );
        }
    }
	return QUEST_SUCCESS;
}

//LMA: Execute LUA Script, client side.
QUESTREWD(029)
{
 	return QUEST_SUCCESS;
}

//Unspawn a NPC
//LMA: JUST for a specific player ! More like a hiding !
//LMA: We changed the way the NPC was used.
QUESTREWD(034)
{
    GETREWDDATA(034);

    if (client->questdebug)
    {
        server->SendPM(client, "removing selected NPC");
    }

    //if (client->quest.selectedNpc == NULL)
    if (client->quest.RefNPC == 0)
    {
        // WTF?
        Log(MSG_WARNING,"QSDA34::No NPC selected");
        return QUEST_FAILURE;
    }

    for (int i = 0; i < client->VisibleNPCs.size(); i++)
    {
        CNPC* curNpc = client->VisibleNPCs.at(i);
        //if (curNpc == client->quest.selectedNpc)

        if (curNpc->npctype == client->quest.RefNPC)
        {
            // found!
            /*
            //client->VisibleNPCs.erase(client->VisibleNPCs.begin()+i);
            client->ClearObject(client->quest.selectedNpc->clientid);
            client->quest.selectedNpc = NULL;
            */
            client->ClearObject(curNpc->clientid);
            client->quest.RefNPC = 0;
            return QUEST_SUCCESS;
        }

    }

    return QUEST_FAILURE;
}

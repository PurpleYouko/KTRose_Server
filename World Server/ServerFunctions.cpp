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

// from Paul_T
bool CWorldServer::SendPM( CPlayer* thisclient, const char* Format, ... )
{
    char buf[512];
    va_list ap;
    va_start( ap, Format );
    vsprintf( buf, Format, ap );
    BEGINPACKET( pak, 0x0784 );
    ADDSTRING( pak, "Server" );
    ADDBYTE( pak, 0 );
    ADDSTRING( pak, buf );
    ADDBYTE( pak, 0 );
    thisclient->client->SendPacket(&pak);
    va_end( ap );
    return true;
}

// from Paul_T
bool CWorldServer::SendGlobalMSG( CPlayer* thisclient, char msg[200] )
{
    BEGINPACKET( pak, 0x0784 );
    ADDSTRING( pak, thisclient->CharInfo->charname );
    ADDBYTE( pak, 0 );
    ADDSTRING( pak, msg );
    ADDBYTE( pak, 0 );
    SendToAll(&pak);
    return true;
}

// NPC Announce to the server
bool CWorldServer::NPCAnnounce( char* msg, char* npc, int mapid)
{
    //LMA: We only announce in the NPC's map
    //Log(MSG_INFO,"%s announces: %s",npc,msg);
    BEGINPACKET( pak, 0x702 );
    ADDSTRING( pak, npc );
	ADDSTRING( pak, "> " );
	ADDSTRING( pak, msg);
	ADDBYTE( pak, 0x00);

	if(mapid!=0)
	{
	    SendToMap  ( &pak, mapid );
	}
	else
	{
	    SendToAll( &pak );
	}


	return true;
}

// NPC Whisper to a character
bool CWorldServer::NPCWhisper( CPlayer* thisclient, CMonster* thismon, char msg[200], char npc[50] )
{
    BEGINPACKET( pak, 0x0784 );
    ADDSTRING( pak, npc );
    ADDBYTE( pak, 0 );
    ADDSTRING( pak, msg );
    ADDBYTE( pak, 0 );
    thisclient->client->SendPacket(&pak);
    return true;
}

// NPC Shout to the current map
bool CWorldServer::NPCShout( CMonster* thismon, char* msg, char* npc,int mapid)
{
    BEGINPACKET(pak, 0x0785);
	ADDSTRING  ( pak, npc );
	ADDBYTE    ( pak, 0 );
	ADDSTRING  ( pak, msg );
	ADDBYTE    ( pak, 0 );

	if(mapid!=0)
        SendToMap  ( &pak, mapid );
    else
        SendToMap  ( &pak, thismon->Position->Map );


	return true;
}


bool CWorldServer::NPCShout2( CMonster* thismon, char* msg, char* npc )
{
    BEGINPACKET(pak, 0x0785);
	ADDSTRING  ( pak, npc );
	ADDBYTE    ( pak, 0 );
	ADDSTRING  ( pak, msg );
	ADDBYTE    ( pak, 0 );
	SendToMap  ( &pak, thismon->Position->Map );
	return true;
}



// Send a PM from a specific NPC using the blue text of the shout system. Used in custom quests
bool CWorldServer::NPCMessage( CPlayer* thisclient, char msg[200], char npc[50])
{
    if (npc == "")
        strcpy(npc, "Event NPC");
    BEGINPACKET(pak, 0x0785);
    ADDSTRING  ( pak, npc );
    ADDBYTE    ( pak, 0 );
    ADDSTRING  ( pak, msg );
    ADDBYTE    ( pak, 0 );
    thisclient->client->SendPacket(&pak);
    return true;
}

// Search Quest by ID
CQuest* CWorldServer::GetQuestByID( unsigned long int id )
{
	for(unsigned j=0; j<QuestList.size(); j++)
    {
		CQuest* thisquest = (CQuest*)QuestList.at( j );
		if ( thisquest->id == id )
            return thisquest;
	}
	return 0;
}

// Search Quest by QuestID
CQuest* CWorldServer::GetQuestByQuestID( unsigned long int questid )
{
	for(unsigned j=0; j<QuestList.size(); j++)
    {
		CQuest* thisquest = (CQuest*)QuestList.at( j );
		if ( thisquest->questid == questid )
            return thisquest;
	}
	return 0;
}

// Search Quest by FinalID
CQuest* CWorldServer::GetQuestByFinalID(  unsigned long int questid )
{
	for(unsigned j=0; j<QuestList.size(); j++)
    {
		CQuest* thisquest = (CQuest*)QuestList.at( j );
		if ( thisquest->finalid == questid )
            return thisquest;
	}
	return 0;
}

bool CWorldServer::DoSkillScript( CCharacter* character, CSkills* thisskill )
{
    if(thisskill==0)
        return false;
    if(thisskill->script==0)
        return false;
    switch(thisskill->script)
    {
        case 1:
        {
            if(thisskill->svalue1==0)
                return false;

            //fPoint position = RandInCircle( character->Position->current, 5 );
            fPoint position = RandInCircle( character->Position->current, 0 );

            //LMA: We must first check that the player can spawn this monster (summon gauge).
            if(character->IsPlayer())
            {
                CPlayer* tempplayer=(CPlayer*) character;
                if (tempplayer->cur_jauge >= tempplayer->summon_jauge)
                {
                    //Log(MSG_WARNING,"Player %s tries to summon %i but his summon gauge is full (%i>=%i)",tempplayer->CharInfo->charname,thisskill->svalue1,tempplayer->cur_jauge,tempplayer->summon_jauge);
                    return false;
                }

                if((GServer->NPCData[thisskill->svalue1]->tab1+tempplayer->cur_jauge)>tempplayer->summon_jauge)
                {
                    //Log(MSG_WARNING,"Player %s tries to summon %i but won't have enough summon gauge (%i+%u>%i)",tempplayer->CharInfo->charname,thisskill->svalue1,tempplayer->cur_jauge,GServer->NPCData[thisskill->svalue1]->tab1,tempplayer->summon_jauge);
                    return false;
                }

            }

            CMap* map = MapList.Index[character->Position->Map];
            CMonster* thismonster=map->AddMonster( thisskill->svalue1, position, character->clientid );
            if (thismonster!=NULL)
            {
                //LMA: updating summon gauge.
                if(character->IsPlayer())
                {
                    CPlayer* tempplayer=(CPlayer*) character;
                    tempplayer->cur_jauge+=GServer->NPCData[thisskill->svalue1]->tab1;
                    thismonster->owner_user_id=tempplayer->Session->userid;
                    //Log(MSG_INFO,"new gauge value %i/%i",tempplayer->cur_jauge,tempplayer->summon_jauge);
                }

               thismonster->skillid=thisskill->id;
               if (!thismonster->IsBonfire())
                  break;

               //LMA: Special for bonfire (and others) cases.
               switch (thisskill->value1[0])
               {
                      case 3:
                           {
                             thismonster->bonushp=1;
                             thismonster->bonusmp=1;
                           }
                           break;
                      case 2:
                           {
                             thismonster->bonushp=1;
                           }
                           break;
                      case 1:
                           {
                             thismonster->bonusmp=1;
                           }
                           break;
                      default:
                           {
                             //NA
                           }
                           break;
               }

                if (thismonster->bonushp==0&&thismonster->bonusmp==0)
                   break;
                thismonster->minvalue=thisskill->value1[1];
                thismonster->maxvalue=thisskill->value1[2];
                thismonster->range=thisskill->range;
            }

        }
        break;
    }
    return true;
}



// Build Drop. New method.
CDrop* CWorldServer::GetNewDrop( CMonster* thismon )
{
    //Log(MSG_DEBUG,"Monster %i killed. FInding a drop",thismon->montype);
    CPlayer* thisclient = GServer->GetClientByCID(thismon->MonsterDrop->firsthit);
    if(thisclient == NULL)return NULL;
    if( thisclient->Session->codedebug )
            GServer->SendPM(thisclient, "Drops function called after killing monster type %i",thismon->montype);

    int randv;
    int i;
    CDrop* newdrop = new (nothrow) CDrop;

    if(newdrop == NULL)
    {
        Log(MSG_WARNING, "newDrop is NULL [getdrop]" );
        return NULL;
    }
    CMDrops* thisdrop = NULL;
    CMDrops* testdrop = NULL;

    newdrop->clientid = GetNewClientID( );
    newdrop->posMap = thismon->Position->Map;
    newdrop->pos = RandInCircle( thismon->Position->current, 3 );
    //newdrop->droptime = time(NULL);
    newdrop->owner = thismon->MonsterDrop->firsthit;
    newdrop->thisparty = thismon->thisparty;
    ClearItem(newdrop->item);

    // Each monster has its own rates for zuly and item drops defined in the stb
    // first check for Zuly drops
    randv = RandNumber( 1, 100);
    if( thisclient->Session->codedebug )
    {
       SendPM(thisclient, "randv = %i.  money = %i. itemrate = %i. ", randv,thismon->thisnpc->money );
    }
    if(randv <= thismon->thisnpc->money) // zuly drop
    {
        newdrop->type = 1; //zuly
        newdrop->amount = thismon->thisnpc->level * 5 * (Config.ZULY_RATE) + RandNumber( 1, 10 );
        if( thisclient->Session->codedebug )
        {
           SendPM(thisclient, "Zuly drop selected " );
        }
        return  newdrop;
    }
    //now check for item drops
	if( thisclient->Session->codedebug )
    {
       SendPM(thisclient, "randv = %i. Item = %i. Itemrate = %i", randv,thismon->thisnpc->item );
    }
	randv = RandNumber( 1, 100);
    if(randv <= thismon->thisnpc->item) // Item drop
    {
        //OK got an item. Now we find which one
        //Map drop or mob drop? map = 50% mob = 50%. Maybe should be configurable or modified by monster data?
        int mapChance = 0;
		switch(thismon->thisnpc->boss)
		{
			case 1:	//king
			case 4:	//World Boss
				mapChance = 0;
				break;
			case 2: //Giant
			case 3:	//Guardian
				mapChance = 10;
				break;
			case 5:	//Leader
				mapChance = 20;
				break;
			default:
				mapChance = 50;
				break;
		}
		int randv2 = RandNumber( 1, 100);
        if(randv2 <= mapChance) //map drop
        {
            for(i=0;i<GServer->MDropList.size();i++)
            {
                testdrop =  GServer->MDropList.at(i);
                if(testdrop == NULL) return NULL;
                if(testdrop->Dropid == thismon->Position->Map)
                {
                    thisdrop = testdrop;
                }
            }

            if( thisclient->Session->codedebug )
            {
               SendPM(thisclient, "Map drop selected for map %i. ", thismon->Position->Map );
            }
        }
        else // mob drop
        {
            for(i=0;i<GServer->MDropList.size();i++)
            {
                testdrop =  GServer->MDropList.at(i);
                if(testdrop == NULL) return NULL;
                if(testdrop->Dropid == thismon->thisnpc->dropid)
                {
                    thisdrop = testdrop;
                }
            }
            if( thisclient->Session->codedebug )
            {
               SendPM(thisclient, "Mob drop selected for dropid %i. ", thismon->thisnpc->dropid );
            }
        }
        if(thisdrop == NULL)
        {
            Log(MSG_WARNING, "Thisdrop is NULL [getdrop]" );
            return NULL;
        }
        newdrop->type = 2; //Item
        dword randitem = RandNumber( 1, thisdrop->TotalChance );
        UINT itemindex = 0;
        //if( thisclient->Session->codedebug )
        //{
        //   SendPM(thisclient, "random chance %i", randitem);
        //}

        i = 0;
        do
        {
            //if( thisclient->Session->codedebug )
            //{
            //   SendPM(thisclient, "Index: %i Item id: %i chance: %i", i, thisdrop->item[i], thisdrop->RunningTotal[i]);
            //}
            i++;
        }
        while (randitem > thisdrop->RunningTotal[i]);
        itemindex = i;

        if(thisdrop->item[itemindex] == 0)
        {
            Log(MSG_DEBUG, "item at itemindex %i does not exist",itemindex );
            return NULL; //no item listed for this dropid
        }
        //ok we found an item. Now we process it.
        //if( thisclient->Session->codedebug )
        //{
        //   SendPM(thisclient, "Found an item to drop itemindex: %i id: %i. ", itemindex, thisdrop->item[itemindex] );
        //}
        if(thisdrop->item[itemindex] < 100000)
        {
            newdrop->item.itemtype = thisdrop->item[itemindex] / 1000;
            newdrop->item.itemnum = thisdrop->item[itemindex] % 1000;
        }
        else
        {
            newdrop->item.itemtype = thisdrop->item[itemindex] / 1000000;
            newdrop->item.itemnum = thisdrop->item[itemindex] % 1000000;
        }
        //if( thisclient->Session->codedebug )
        //{
        //   SendPM(thisclient, "Item type = %i. item number = %i", newdrop->item.itemtype,newdrop->item.itemnum );
        //}
        newdrop->item.lifespan = 10 + rand()%80;
        switch( newdrop->item.itemtype )
        {
            case 1: case 2: case 3: case 4: case 5: case 6: case 7: case 8: case 9:
                newdrop->item.type = GServer->EquipList[newdrop->item.itemtype].Index[newdrop->item.itemnum]->type;
                newdrop->item.raretype = GServer->EquipList[newdrop->item.itemtype].Index[newdrop->item.itemnum]->raretype;
                break;
            case 10:
                newdrop->item.type = GServer->UseList.Index[newdrop->item.itemnum]->type;
                newdrop->item.raretype = 0;
                break;
            case 11:
                newdrop->item.type = GServer->JemList.Index[newdrop->item.itemnum]->type;
                newdrop->item.raretype = 0;
                break;
            case 12:
                newdrop->item.type = GServer->NaturalList.Index[newdrop->item.itemnum]->type;
                newdrop->item.raretype = 0;
                break;
            case 14:
                newdrop->item.type = GServer->PatList.Index[newdrop->item.itemnum]->type;
                newdrop->item.raretype = 0;
                break;
            default:
                Log(MSG_DEBUG, "itemtype %i does not exist",newdrop->item.itemtype );
                return NULL;
                break;
        }
        float dmod = 0; //random number from 0 to 100 made up of 5 sub numbers to keep
        //the average value near to 50
        for(i=0; i<5; i++)
        {
            float r1 = rand()%20;
            dmod += r1;
        }
        newdrop->item.durability = 10 + (int)dmod;
        newdrop->item.appraised = true;
        newdrop->item.stats = 0;
        newdrop->item.count = 1;

        if( newdrop->item.itemtype == 8 || newdrop->item.itemtype == 9 )
        {
            //This probability is now configurable
            int psocked = rand()%101; //Probability of finding a socketed item
            if( psocked < Config.SlotChance) //default should be around 5% needs to be rare
            {
                newdrop->item.socketed = true;
            }
            else
            {
                 newdrop->item.socketed = false;
            }
        }
        else
        {
            newdrop->item.socketed = false;
        }

		float TmpMod = thisclient->Attr->Cha / 30;
        short chamod = (short)floor(TmpMod);
        if(chamod < 0) chamod = 0;
        int basedrop = thisclient->Stats->itemdropcountrate; //Base number of items to be dropped.
        int maxdrop = basedrop + Config.DropMultiple;
        if(newdrop->item.itemtype == 11) //gems only get 1
            newdrop->item.count = 1;
        if( newdrop->item.itemtype == 10 || newdrop->item.itemtype == 12 )
        {
            newdrop->item.count = RandNumber(basedrop, maxdrop);
            if(thismon->thisnpc->level == 1)
                newdrop->item.count = 1; //limit the drop rate of items from level 1 event mobs
            if(newdrop->item.count == 0)
                newdrop->item.count = 1;
            if(newdrop->item.type == 314) //skillbooks
                newdrop->item.count = 1;
            if(newdrop->item.itemtype == 12)
            {
                newdrop->item.count = RandNumber(1, 6) + RandNumber(0, thisclient->Stats->itemdropcountrate); //basic type 12 items
                //some item types have their own rules
                if(newdrop->item.type == 428) //special skill items such as mana jams and scrolls
                    newdrop->item.count = RandNumber(1, 3);
                if(newdrop->item.itemnum > 300 && newdrop->item.itemnum < 360) //bullets get a lot higher count.
                {
                    newdrop->item.count *= 10;
                    newdrop->item.count += 10;
                }
            }
        }
        else if( newdrop->item.itemtype > 1 && newdrop->item.itemtype !=7 && newdrop->item.itemtype < 10)		//Why not refine jewelry? should be possible
        {
            // check to see if the item will be refined
            int prefine = rand()%100; //Probability of finding a refined item
            int refinechance = Config.RefineChance;
            if(prefine < refinechance) // default = 5%
            {
                int refinelevel = rand()%101;  //which level of refine do we actually get
                if( refinelevel < 5)        //default 5%
                    newdrop->item.refine = 4 * 16;
                else if( refinelevel < 15 )   //10%
                    newdrop->item.refine = 3 * 16;
                else if(refinelevel < 35 )   // 20%
                    newdrop->item.refine = 2 * 16;
                else                          // 65%
                    newdrop->item.refine = 16;
            }
            else //95%
                newdrop->item.refine = 0;

            int StatChance = Config.StatChance;
			int Stat2Chance = 0;
            if(newdrop->item.raretype > 0) // it's a blue item
            {
                int pstats = RandNumber(1, 100);
				newdrop->item.durability += 10;		//make blues a little more durable
				if( pstats < StatChance + 10)       // default 15%. More chance of extra stats on a blue
				{
					newdrop->item.appraised = false;   //statted items need to be appraised
					newdrop->item.stats = RandNumber(0, 299); 
				}
				//now unique stats. About 75% of dropped blues should have unique stats. reset statchance
				StatChance = 75;
				Stat2Chance = 35;
            }
            else // not a blue item
            {
                int pstats = RandNumber(1, 100);
                if( pstats < StatChance)        // default 5%
				{
                    newdrop->item.appraised = false;   //statted items need to be appraised
					newdrop->item.stats = RandNumber(0, 299); 
				}
				//now unique stats. About 50% of dropped blues should have unique stats. reset statchance
				StatChance = 50;
				Stat2Chance = 20;
            }
			//Unique stats
			int pstats = RandNumber(1, 100);
			if( pstats < StatChance)       
			{
				newdrop->item.UStat[0] = GetUStat( 0 ); 
				newdrop->item.UValue[0] = GetUValue( 0 , newdrop->item.UStat[0] );
				if( pstats < Stat2Chance )
				{
					newdrop->item.UStat[1] = GetUStat( 0 );
					newdrop->item.UValue[1] = GetUValue( 0 , newdrop->item.UStat[1] );
				}
			}
        }
        newdrop->item.gem = 0;
        if(newdrop->item.count == 0) 
            newdrop->item.count = 1;  //just in case this is causing bugged items.
        if( thisclient->Session->codedebug )
        {
           SendPM(thisclient, "Dropped item type: %i ID: %i count %i", newdrop->item.itemtype, newdrop->item.itemnum,newdrop->item.count);
        }
        return newdrop;

    }
    return NULL;
    // did not qualify to drop anything this time
}

//Adding new stats to make items truly unique
UINT CWorldServer::GetUStat(short modifier)
{
	short UStat = 0;
	//now to assign the actual stat
	UStat = RandNumber( 3,132 );		//recode more inteligently later

	return UStat;
}

UINT CWorldServer::GetUValue( short modifier, short stat )
{
	short UValue = 0;
	UValue = RandNumber( 1, 10 );		//recode more inteligently later
	return UValue;
}

 // PY extra stats lookup
 UINT CWorldServer::GetExtraStats( UINT modifier )
 {
    UINT stat = rand()%300 + modifier;
    if(stat > 300)stat = 300;
    return StatLookup[stat].statnumber;
 }

 // this function makes sure that values returned from the char are positive. negative numbers mess up characters.
UINT CWorldServer::GetSafeAtoi( char *itmp )
{
	DWORD tmpVal = atoi(itmp);
	UINT retVal = 0;
	if( tmpVal < 0 )
		retVal = 0;
	else
		retVal = (UINT)tmpVal;
	return retVal;
}


UINT CWorldServer::GetColorExp( UINT playerlevel,UINT moblevel, UINT exp )
{
    int dif = moblevel - playerlevel;

    if(dif>15)//purple
        exp = (UINT)floor((double)exp*1.5);
    else if(dif>10)
        exp = (UINT)floor((double)exp*1.2);
    else if(dif>4)
        exp = (UINT)floor((double)exp*1.1);
    else if(dif>-4)
        exp = (UINT)floor((double)exp*1);
    else if(dif>-9)
        exp = (UINT)floor((double)exp*0.7);
    else if(dif>-15)
        exp = (UINT)floor((double)exp*0.1);
    else if(dif>-25)
        exp = (UINT)floor((double)exp*0.005);
    else
        exp = 0;
    return exp;
}
bool CWorldServer::SendSysMsg( CPlayer* thisclient, string message )
{
	BEGINPACKET( pak, 0x7df );
	ADDBYTE    ( pak, 0xf1 );
	ADDSTRING  ( pak, message.c_str( ));
	ADDBYTE    ( pak, 0x00 );
	thisclient->client->SendPacket( &pak );
    return true;
}


// teleport to specificated map and position
bool CWorldServer::TeleportTo ( CPlayer* thisclient, int map, fPoint position )
{
    MapList.Index[map]->TeleportPlayer( thisclient, position, false );
    return true;
}

//LMA: checking compatible class (for skills)
bool CWorldServer::CheckCompatibleClass(UINT rclass, UINT player_job)
{
    if(rclass == player_job)
        return true;

    if(player_job >= 121 && player_job <= 122)
    {
        if(rclass == 111)
        {
            return true;
        }
        else
        {
            return false;
        }

    }

    if(player_job >= 221 && player_job <= 222)
    {
        if(rclass == 211)
        {
            return true;
        }
        else
        {
            return false;
        }

    }

    if(player_job >= 321 && player_job <= 322)
    {
        if(rclass == 311)
        {
            return true;
        }
        else
        {
            return false;
        }

    }

    if(player_job >= 421 && player_job <= 422)
    {
        if(rclass == 411)
        {
            return true;
        }
        else
        {
            return false;
        }

    }

    Log(MSG_INFO,"Class %i/%i not found?",rclass,player_job);


    return false;
}

// Learn Skill
//bool CWorldServer::LearnSkill( CPlayer* thisclient, UINT skill )
bool CWorldServer::LearnSkill( CPlayer* thisclient, UINT skill, bool takeSP)
{
    /*
    0 - already learned
    1 - he learned
    2 - not job
    3 - another skill is absent
    4 - not status points the (lvl)
    5 - it is not possible to raise of lvl the skill
    6 - incorrect number of skill
    7 - you do not have sufficient sp
    */

    int b = 1;
    CSkills* thisskill = GetSkillByID( skill );
    if( thisskill == NULL )
        return false;

    //LMA: Is it an empty skill?
    if(GServer->SkillList[skill]->skill_tab==0)
    {
        Log(MSG_WARNING,"%s:: Incorrect or empty skill %i",thisclient->CharInfo->charname,skill);
        b = 6;
    }

    if(takeSP&&thisclient->CharInfo->SkillPoints<thisskill->sp )
    {
        Log(MSG_WARNING,"%s:: not enough skill points (%u<%u) for skill %u",thisclient->CharInfo->charname,thisclient->CharInfo->SkillPoints,thisskill->sp,skill);
        b = 7;
    }
    else if( thisskill->clevel>thisclient->Stats->Level )
    {
        Log(MSG_WARNING,"%s:: incorrect level for skill %u (%u<%u)",thisclient->CharInfo->charname,skill,thisskill->clevel,thisclient->Stats->Level);
        b = 4;
    }

    if(b == 1)
    {
        UINT rclass = 0;
        if (thisskill->c_class == 41)
        {
            rclass = 111;
        }
        else if (thisskill->c_class == 42)
        {
            rclass = 211;
        }
        else if (thisskill->c_class == 43)
        {
            rclass = 311;
        }
        else if (thisskill->c_class == 44)
        {
            rclass = 411;
        }
        else if (thisskill->c_class == 61)
        {
            rclass = 121;
        }
        else if (thisskill->c_class == 62)
        {
            rclass = 122;
        }
        else if (thisskill->c_class == 63)
        {
            rclass = 221;
        }
        else if (thisskill->c_class == 64)
        {
            rclass = 222;
        }
        else if (thisskill->c_class == 65)
        {
            rclass = 321;
        }
        else if (thisskill->c_class == 66)
        {
            rclass = 322;
        }
        else if (thisskill->c_class == 67)
        {
            rclass = 421;
        }
        else if (thisskill->c_class == 68)
        {
            rclass = 422;
        }

        //LMA: new way, so "old" job skill can be learned...
        //if(rclass == thisclient->CharInfo->Job)
        if(CheckCompatibleClass(rclass,thisclient->CharInfo->Job))
        {
            b = 1;
        }
        else
        {
            Log(MSG_WARNING,"%s:: Incompatible class for skill %u (%u)",thisclient->CharInfo->charname,skill,rclass);
            b = 2;
        }
    }
    if(b == 1)
    {
        for(UINT i=0;i<3; i++)
        {
            if(thisskill->rskill[i] != 0)
            {
                UINT rskill = thisclient->GetPlayerSkill(thisskill->rskill[i]);
                if(rskill == 0xffff)
                {
                    Log(MSG_WARNING,"%s:: Necessary skill %u not found to learn %u",thisclient->CharInfo->charname,thisskill->rskill[i],skill);
                    b=3;
                }
                else
                {
                    if(thisskill->lskill[i] > thisclient->cskills[rskill].level)
                    {
                        Log(MSG_WARNING,"%s:: Necessary skill (%u) level incorrect (%u>%u) to learn %u",thisclient->CharInfo->charname,thisskill->rskill[i],thisskill->lskill[i],thisclient->cskills[rskill].level,skill);
                        b=5;
                    }

                }

            }

        }

    }

    if(b == 1)
    {
        //LMA: Looking for good place to save it now...
        int family = thisclient->GoodSkill(skill);
        if(family == -1)
        {
            Log(MSG_WARNING,"%s:: Can't find family for skill %i",thisclient->CharInfo->charname,skill);
            b=6;
        }
        else
        {
            //LMA: Checking if the skill isn't already learned.
            if (thisclient->FindSkill(family,skill))
            {
                Log(MSG_WARNING,"%s:: already learned skill %i family %i",thisclient->CharInfo->charname,skill,family);
                b=0;
                BEGINPACKET( pak, 0x7b0 );
                ADDBYTE    ( pak, b);
                ADDWORD    ( pak, thisclient->p_skills-1);
                ADDWORD    ( pak, skill);
                ADDWORD    ( pak, thisclient->CharInfo->SkillPoints);
                thisclient->client->SendPacket( &pak);
                return false;
            }

            int index=thisclient->FindSkillOffset(family);
            if(index==-1)
            {
                Log(MSG_WARNING,"%s:: Can't find index in family %i for skill %i",thisclient->CharInfo->charname,family,skill);
                b=6;
            }
            else
            {
                thisclient->cskills[index].id=skill;
                thisclient->cskills[index].level=1;
                thisclient->cskills[index].thisskill = thisskill;
                thisclient->cskills[index].cooldown_skill = 0;

                if (takeSP)
                {
                    thisclient->CharInfo->SkillPoints -= thisskill->sp;
                }
                //in the good family now.
                //thisclient->SaveSkillInfo(family,thisclient->cur_max_skills[family],skill,1);
                thisclient->saveskills();

                BEGINPACKET( pak, 0x7b0 );
                ADDBYTE    ( pak, b);
                ADDWORD    ( pak, index);
                ADDWORD    ( pak, skill);
                ADDWORD    ( pak, thisclient->CharInfo->SkillPoints);
                thisclient->client->SendPacket( &pak);
                thisclient->SetStats( );


                return true;
                //LMA: new way end.
            }

        }

    }

    BEGINPACKET( pak, 0x7b0 );
    ADDBYTE    ( pak, b);
    ADDWORD    ( pak, thisclient->p_skills-1);
    ADDWORD    ( pak, skill);
    ADDWORD    ( pak, thisclient->CharInfo->SkillPoints);
    thisclient->client->SendPacket( &pak);

    Log(MSG_WARNING,"%s:: skill %i not learned (error %i)",thisclient->CharInfo->charname,skill,b);


    return false;
}

//add / remove a Fairy
void CWorldServer::DoFairyStuff(CPlayer* targetclient, int action)
{
    BEGINPACKET( pak, 0x7dc );
	ADDBYTE( pak, action );
	ADDWORD( pak, targetclient->clientid);
	SendToVisible( &pak, targetclient );
}

void CWorldServer::DoFairyFree( int fairy )
{
    GServer->FairyList.at(fairy)->assigned = false;
    GServer->FairyList.at(fairy)->ListIndex = 0;

    if(GServer->Config.FairyTestMode == 0)
    {
        GServer->FairyList.at(fairy)->WaitTime = GServer->Config.FairyWait * (rand()% GServer->GetFairyRange(1)+ GServer->GetFairyRange(0));
    }

    GServer->FairyList.at(fairy)->LastTime = clock();


    return;
}

//LMA: Old code.
/*
void CWorldServer::RefreshFairy( )
{
        //fairy: The wait timer should be possibibly bigger when there is few people online.
        if (Config.FairyMode == 1 && ClientList.size() > 1)
        {   //if fairy mode on and someone online
           for (int i=0; i<Config.FairyMax; i++)     // check all fairies
           {
                if ( FairyList.at(i)->LastTime <= ( clock() - (FairyList.at(i)->WaitTime*60*CLOCKS_PER_SEC) ) && !FairyList.at(i)->assigned )  // if fairy hour is now
	            {
                    int value = rand()%(int)(ClientList.size()-1) + 1;  //choose random value in clientlist
		            CPlayer* targetclient = (CPlayer*) ClientList.at(value)->player;

		            //LMA: Patch if fairy was attributed as the player is not actually yet IG...
		            //The Fairy is not an easy lady ^_^
		            if ((clock()-targetclient->firstlogin)>(60*CLOCKS_PER_SEC))
		            {
                        FairyList.at(i)->ListIndex = value;
    			        FairyList.at(i)->LastTime = clock();
    			        FairyList.at(i)->assigned = true;
    			        targetclient->Fairy = true;
    			        targetclient->FairyListIndex = i;                 // FairyList index number of our actual fairy
    			        DoFairyStuff(targetclient, 1);           // spawn fairy to target
    			        targetclient->SetStats();
                    }

                }

			    if ( (FairyList.at(i)->LastTime + (Config.FairyStay*60*CLOCKS_PER_SEC)) <= clock() && FairyList.at(i)->assigned)  // id time for our fairy to go away
    			{
                    CPlayer* oldclient  = (CPlayer*) ClientList.at(FairyList.at(i)->ListIndex)->player;
                    DoFairyFree(i);
                    oldclient->Fairy = false;
                    oldclient->FairyListIndex = 0;
                    DoFairyStuff(oldclient, 0);  // unspawn fairy
                    oldclient->SetStats();
                }

           }

        }

        // this close fairies after their time if GM de activate  fairy mode when some fairies are assigned.
        if (Config.FairyMode == 0 && ClientList.size() > 1)
        {    // if serevr mode off and someone online
             for (int i=0; i<Config.FairyMax; i++)
             {
                 if ( (FairyList.at(i)->LastTime + (Config.FairyStay*60*CLOCKS_PER_SEC)) <= clock() && FairyList.at(i)->assigned)
    			{
                    CPlayer* oldclient  = (CPlayer*) ClientList.at(FairyList.at(i)->ListIndex)->player;
                    DoFairyFree(i);
                    oldclient->Fairy = 0;
                    oldclient->FairyListIndex = 0;
                    DoFairyStuff(oldclient, 0);
                    oldclient->SetStats();
                }

             }

         }

}
*/

//LMA: New version using charid now...
void CWorldServer::RefreshFairy( )
{
        //fairy: The wait timer should be possibibly bigger when there is few people online.
        if (Config.FairyMode == 1 && ClientList.size() > 1)
        {
            //if fairy mode on and someone online

            // check all fairies
           //for (int i=0; i<Config.FairyMax; i++)
           for (int i=0; i<FairyList.size(); i++)
           {

               //LMA: Available fairies, depending on Config.FairyMax (depending on number of players).
               if(i<Config.FairyMax)
               {
                    if (!FairyList.at(i)->assigned && FairyList.at(i)->LastTime <= ( clock() - (FairyList.at(i)->WaitTime*60*CLOCKS_PER_SEC) ))  // if fairy hour is now
                    {
                        int value = rand()%(int)(ClientList.size()-1) + 1;  //choose random value in clientlist
                        CPlayer* targetclient = (CPlayer*) ClientList.at(value)->player;
                        if (targetclient==NULL)
                        {
                            Log(MSG_WARNING,"RefreshFairyNew:: Player is NULL");
                            return;
                        }

                        //LMA: Patch if fairy was attributed as the player is not actually yet IG...
                        //The Fairy is not an easy lady ^_^
                        //A player already faired can't have one...
                        if (!targetclient->Fairy&&(clock()-targetclient->firstlogin)>(60*CLOCKS_PER_SEC))
                        {
                            FairyList.at(i)->ListIndex = targetclient->CharInfo->charid;
                            FairyList.at(i)->LastTime = clock();
                            FairyList.at(i)->assigned = true;
                            targetclient->Fairy = true;
                            targetclient->FairyListIndex = i;        // FairyList index number of our actual fairy
                            DoFairyStuff(targetclient, 1);           // spawn fairy to target
                            targetclient->SetStats();
                        }

                    }

               }

                //it's time for our fairy to go away
                //LMA: We need to check all the vector, else if some players disconnected, some players could be "trapped" with
                //their fairy since Config.FairyMax<Fairylist.size()
			    if (FairyList.at(i)->assigned&&(FairyList.at(i)->LastTime + (Config.FairyStay*60*CLOCKS_PER_SEC)) <= clock())
    			{
    			    CPlayer* oldclient  = GetClientByCID(FairyList.at(i)->ListIndex);
    			    DoFairyFree(i);
    			    if(oldclient!=NULL)
    			    {
                        oldclient->Fairy = false;
                        oldclient->FairyListIndex = 0;
                        DoFairyStuff(oldclient, 0);  // unspawn fairy
                        oldclient->SetStats();
    			    }

                }

           }

        }

        // this close fairies after their time if GM de activate fairy mode when some fairies are assigned.
        if (Config.FairyMode == 0 && ClientList.size() > 1)
        {
            // if server mode off and someone online
             //for (int i=0; i<Config.FairyMax; i++)
             //LMA: For all fairies...
             for (int i=0; i<FairyList.size(); i++)
             {
                 if (FairyList.at(i)->assigned && (FairyList.at(i)->LastTime + (Config.FairyStay*60*CLOCKS_PER_SEC)) <= clock())
    			{
                    CPlayer* oldclient  = GetClientByCID(FairyList.at(i)->ListIndex);
    			    DoFairyFree(i);
    			    if(oldclient!=NULL)
    			    {
                        oldclient->Fairy = false;
                        oldclient->FairyListIndex = 0;
                        DoFairyStuff(oldclient, 0);  // unspawn fairy
                        oldclient->SetStats();
    			    }

                }

             }

         }


    return;
}

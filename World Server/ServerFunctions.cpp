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

            //LMA: We must first check that the player can spawn this monster (summon jauge).
            if(character->IsPlayer())
            {
                CPlayer* tempplayer=(CPlayer*) character;
                if (tempplayer->cur_jauge>=tempplayer->summon_jauge)
                {
                    Log(MSG_WARNING,"Player %s tries to summon %i but his jauge is full (%i>=%i)",tempplayer->CharInfo->charname,thisskill->svalue1,tempplayer->cur_jauge,tempplayer->summon_jauge);
                    return false;
                }

                if((GServer->NPCData[thisskill->svalue1]->tab1+tempplayer->cur_jauge)>tempplayer->summon_jauge)
                {
                    Log(MSG_WARNING,"Player %s tries to summon %i but won't have enough jauge (%i+%u>%i)",tempplayer->CharInfo->charname,thisskill->svalue1,tempplayer->cur_jauge,GServer->NPCData[thisskill->svalue1]->tab1,tempplayer->summon_jauge);
                    return false;
                }

            }

            CMap* map = MapList.Index[character->Position->Map];
            CMonster* thismonster=map->AddMonster( thisskill->svalue1, position, character->clientid );
            if (thismonster!=NULL)
            {
                //LMA: updating summon jauge.
                if(character->IsPlayer())
                {
                    CPlayer* tempplayer=(CPlayer*) character;
                    tempplayer->cur_jauge+=GServer->NPCData[thisskill->svalue1]->tab1;
                    thismonster->owner_user_id=tempplayer->Session->userid;
                    Log(MSG_INFO,"new jauge value %i/%i",tempplayer->cur_jauge,tempplayer->summon_jauge);
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

// Build Drop (LMA: shouldn't be used anymore).
/*
CDrop* CWorldServer::GetDrop( CMonster* thismon )
{
    CDrop* newdrop = new (nothrow) CDrop;
    if(newdrop==NULL)
    {
        Log(MSG_WARNING, "Error allocing memory [getdrop]" );
        return NULL;
    }
    newdrop->clientid = GetNewClientID( );
    newdrop->posMap = thismon->Position->Map;
    newdrop->pos = RandInCircle( thismon->Position->current, 3 );
    newdrop->droptime = time(NULL);
    newdrop->owner = thismon->MonsterDrop->firsthit;
    newdrop->thisparty = thismon->thisparty;
    ClearItem(newdrop->item);
    int randv = RandNumber( 1, 100);
    if(thismon->MonsterDrop->mapdrop->level_max<thismon->MonsterDrop->firstlevel) randv = 100;
    if(randv<=30)//30% zuly [zulies will count as mapdrop]
    {
        if(thismon->MonsterDrop->mapdrop->level_max>=thismon->MonsterDrop->firstlevel)
        {
            newdrop->type = 1; //Drop Zuly
            newdrop->amount = thismon->thisnpc->level*5*Config.ZULY_RATE + RandNumber( 1, 10 );
            return  newdrop;
        }
        delete newdrop;
        return NULL;
    }
    CMDrops* thisdrops;
    newdrop->type = 2; //drop item drop
    switch( Config.DROP_TYPE )
    {
        case 0://only map
            thisdrops = thismon->MonsterDrop->mobdrop;
            if(thisdrops->level_max<thismon->MonsterDrop->firstlevel)
            {
                delete newdrop;
                return NULL;
            }
        break;
        case 1://mob only
            thisdrops = thismon->MonsterDrop->mapdrop;
            if(thismon->thisnpc->level-thismon->MonsterDrop->firstlevel<-14)
            {
                delete newdrop;
                return NULL;
            }
        break;
        default://both
            randv = RandNumber(1,100);
            if(thismon->MonsterDrop->mapdrop!=NULL)
                if(thismon->MonsterDrop->mapdrop->level_max<thismon->MonsterDrop->firstlevel)
                    randv = 100;
            if(randv>60)//select wich drop will use (map or mob) //40 - 60%
            {
                thisdrops = thismon->MonsterDrop->mobdrop;
                if((int)(thismon->thisnpc->level-thismon->MonsterDrop->firstlevel) < -14)
                {
                    delete newdrop;
                    return NULL;
                }
            }
            else
            {
                thisdrops = thismon->MonsterDrop->mapdrop;
                if(thisdrops->level_max<thismon->MonsterDrop->firstlevel)
                {
                    delete newdrop;
                    return NULL;
                }
            }
        break;
    }
    if(thisdrops==NULL)
    {
        thisdrops = thismon->MonsterDrop->mobdrop;
        if(thisdrops==NULL)
        {
            thisdrops = thismon->MonsterDrop->mapdrop;
            if(thisdrops==NULL)
            {
                newdrop->type = 1; //Drop Zuly
                newdrop->amount = thismon->thisnpc->level*5*Config.ZULY_RATE - RandNumber( 1, 20 );
                return  newdrop;
            }
        }
    }
    randv = 0;
    randv = RandNumber( 1, thisdrops->probmax );
    DWORD prob = 1;
    for(UINT i=0;i<thisdrops->Drops.size();i++)
    {
        CDropInfo* dropinfo = thisdrops->Drops.at( i );
        prob += dropinfo->prob;
        if(randv<=prob)
        {
            newdrop->item.itemtype = dropinfo->type;
            newdrop->item.itemnum = dropinfo->item;
            break;
        }
    }
    if(newdrop->item.itemtype==0)
    {
        Log(MSG_WARNING, "Drop Probability Highter: %i", randv );
        delete newdrop;
        return NULL;
    }
    if(newdrop->item.itemtype>1 && newdrop->item.itemtype<10 && newdrop->item.itemtype!=JEWEL)
    {
        // Refine
        randv = RandNumber( 1, 100 );
        if( randv < 10 )        //10%
            newdrop->item.refine = 48;
        else if( randv < 25 )   //15%
            newdrop->item.refine = 32;
        else if( randv < 50 )   // 25%
            newdrop->item.refine = 16;
        else //50%
            newdrop->item.refine = 0;
        newdrop->item.lifespan = RandNumber( 30, 70 );
        newdrop->item.durability = RandNumber( 35, 65 );
        if( newdrop->item.itemtype==WEAPON || newdrop->item.itemtype==SUBWEAPON )
        {
            //socketed
            randv = RandNumber( 1, 100 );
            if( randv < 30 ) newdrop->item.socketed = true; // 30%
            else newdrop->item.socketed = false;            // 70%
        }
        else
        {
            newdrop->item.socketed = false;
        }
        randv = RandNumber( 1, 100 );
        if( randv < 30 )        // 30%
            newdrop->item.stats = rand()%300;
        newdrop->item.appraised = newdrop->item.stats==0?true:false;
    }
    else
    {
         newdrop->item.lifespan = 100;
         newdrop->item.durability = 40;
         newdrop->item.socketed = false;
         newdrop->item.stats = 0;
    }
    newdrop->item.count = 1;
    if( newdrop->item.itemtype == 10 || newdrop->item.itemtype == 12 )
    {
        newdrop->item.count = RandNumber( 1, 3 );
    }
    newdrop->item.gem = 0;
    return newdrop;
}
*/

//hidden
// Build Drop the PY way
CDrop* CWorldServer::GetPYDrop( CMonster* thismon, UINT droptype )
{   //if droptype = 1 then it is a normal drop. if it is 2 then it is a potential side drop.
    //Log(MSG_INFO,"GetPYDrop, monster %i, droptype %i",thismon->montype,droptype);
	// PY OLD and outdated. To be removed later
    if(droptype == 2) // monster is still alive
    {
        // kicks it straight back if the monster is not dead
        if(thismon->thisnpc->side != 0) //perhaps we get a side drop??
        {
            if(GServer->RandNumber(0,100) < thismon->thisnpc->sidechance)
            {
                droptype = thismon->thisnpc->side;
            }
            else
            {
                return NULL;  //No drop this time
            }
        }
        else
        {
            return NULL;  //No drop this time
        }
    }

    CDrop* newdrop = new (nothrow) CDrop;
    if(newdrop==NULL)
    {
        Log(MSG_WARNING, "Error allocing memory [getdrop]" );
        return NULL;
    }
	/*
    newdrop->clientid = GetNewClientID( );
    newdrop->posMap = thismon->Position->Map;
    newdrop->pos = RandInCircle( thismon->Position->current, 3 );
    newdrop->droptime = time(NULL);
    newdrop->owner = thismon->MonsterDrop->firsthit;
    newdrop->thisparty = thismon->thisparty;
    ClearItem(newdrop->item);

    CPlayer* thisclient = GServer->GetClientByCID(thismon->MonsterDrop->firsthit);
    if(thisclient == NULL)
    {
        ClearClientID(newdrop->clientid);
        delete(newdrop);
        Log(MSG_WARNING,"GetPYDrop:: Failed to create player");
        return NULL;
    }

    // code to modify drop chance for different levels
    //float charm = 0;
    float charm = (float)thisclient->Attr->Cha / 5;

    //LMA: Gray drops
    float leveldif = (float)thismon->thisnpc->level - (float)thisclient->Stats->Level;
    if (thisclient->bonusgraydrop!=0)
    {
        Log(MSG_INFO,"Gray medal detected");
        leveldif=20.0;
    }

    float droprate = (float)GServer->Config.DROP_RATE + charm;  //basic server rate + extra for player charm
    float dropchance = (droprate + (droprate * 0.01 * leveldif));
    //Log(MSG_INFO,"charm %.2f, leveldif %.2f, droprate %.2f, dropchance %.2f",charm,leveldif,droprate,dropchance);
    if(dropchance < 10) dropchance = 10; //always a small chance of a drop even when the mob is more than 20 levels beneath your own
    //Log(MSG_INFO,"dropchance %.2f",dropchance);
    if(thismon->thisnpc->level == 1)
    {
        dropchance = 80;
    }

    //Log(MSG_INFO,"dropchance %.2f",dropchance);
    UINT lma_save_rand=0;
    lma_save_rand=GServer->RandNumber(0, 100);
    if (lma_save_rand>dropchance)
    {
        //Log(MSG_INFO,"no drop, %i > %.2f",lma_save_rand,dropchance);
        ClearClientID(newdrop->clientid);
        delete(newdrop);
        return NULL; // no drop here. not this time anyway.
    }

    //Log(MSG_INFO,"drop possible, %i <= %.2f",lma_save_rand,dropchance);

    CItemType prob[MDropList.size()];
    bool isdrop = false;
    int n = 0;
    int test = 0;
    long int probmax = 0;
    int itemnumber[MDropList.size()];
    int itemtype[MDropList.size()];
    int probability[MDropList.size()];
    int alternate[MDropList.size()][8];

    if( thismon->IsGhost())
    {
        // Stuff to do if the mob is a ghost of any type
        int selection = 1 + rand()%10;
        if( selection <= 3 ) //MP water
        {
            newdrop->type = 2;
            itemnumber[n] = 399;
            itemtype[n] = 12;
            probability[n] = 10;
            probmax =10;
            n++;
        }
        else if( selection <=6 ) //HP water
        {
            newdrop->type = 2;
            itemnumber[n] = 400;
            itemtype[n] = 12;
            probability[n] = 10;
            probmax =10;
            n++;
        }
        else  //skillbooks
        {
            for(int i=0; i<SkillbookList.size( ); i++)
            {
                newdrop->type = 2;
                CMDrops* thisdrop = GServer->SkillbookList.at(i);
                if(thisdrop->level_min <= thismon->thisnpc->level &&  thisdrop->level_max >= thismon->thisnpc->level)
                {
                    itemnumber[n] = thisdrop->itemnum;
                    itemtype[n] = thisdrop->itemtype;
                    probability[n] = thisdrop->prob;
                    probmax += thisdrop->prob;
                    n++;
                }
            }
        }
    }
    else
    {
        int randv = RandNumber( 1, 100);
        if(randv <= 30)//30% zuly drop instead of item drop
        {
            newdrop->type = 1; //Drop Zuly
            newdrop->amount = thismon->thisnpc->level * 5 * Config.ZULY_RATE + RandNumber( 1, 10 );
            //Log(MSG_INFO,"zuly drop %i",newdrop->amount);
            return  newdrop;
        }
        // Stuff to do if the mob isn't a ghost

        int randomdrop = GServer->RandNumber(1, 100);
        //enable the next line for debug purposes if you want to confirm a drop is working.
        //Log(MSG_INFO, "Mob type %i. Map = %i. Level = %i", thismon->montype, thismon->Position->Map,thismon->thisnpc->level);

        for(int i=0; i<MDropList.size( ); i++)
        {
            isdrop=false;
            CMDrops* thisdrop = GServer->MDropList.at(i);
            if(thisdrop->mob == thismon->montype)
            {
                //Mob drop possible.
                test = GServer->RandNumber(1, 1000);
                if(test < thisdrop->prob)
                {
                    isdrop = true;
                    //item will be added to the short list
                }
            }
            if(thisdrop->map == thismon->Position->Map)
            {
                //Map drop possible.
                test = GServer->RandNumber(1, 1000);
                if(thismon->thisnpc->level == 1)
                   test = GServer->RandNumber(1, 10000); // make it less likely to get map drops from event mobs
                if(test < thisdrop->prob)
                {
                    isdrop = true;
                    //item will be added to the short list
                }
            }
            if(thismon->thisnpc->level >= thisdrop->level_min && thismon->thisnpc->level <= thisdrop->level_max)
            {
                //Level drop possible
                test = GServer->RandNumber(1, 1000);
                if(test < thisdrop->prob)
                {
                    isdrop = true;
                    //item will be added to the short list
                }
            }
            if(isdrop == true) //Add item to the short list
            {
                if(droptype != 1) //side drops only. Skip if the item is not a match for side type
                {
                    if(itemtype[n] != droptype)continue;
                }
                //droptype 1 is a regular drop
                itemnumber[n] = thisdrop->itemnum;
                itemtype[n] = thisdrop->itemtype;
                //probability[n] = thisdrop->prob;
                alternate[n][0] = 0;
                for(int i=1;i<8;i++)
                {
                    alternate[n][i] = thisdrop->alt[i];
                }
                n++;
            }
        }
    }

    int newn = n;

    if(n == 0)
    {
        ClearClientID(newdrop->clientid);
        delete(newdrop);
        return NULL;
    }

    int maxitems = n;
    //maxitems is the number of items in the shortlist

    // randomize the item from the shortlist. items get equal chance
    n = GServer->RandNumber(0, maxitems);

    newdrop->item.itemnum = itemnumber[n];
    newdrop->item.itemtype = itemtype[n];
    newdrop->type = 2;

    newdrop->item.lifespan = 10 + rand()%80;
    float dmod = 0; //random number from 0 to 100 made up of 4 sub numbers to keep
    //the average value near to 50
    for(int i=0; i<4; i++)
    {
        float r1 = rand()%20;
        dmod += r1;
    }
    newdrop->item.durability = 10 + (int)dmod;
    if( newdrop->item.itemtype == 8 || newdrop->item.itemtype == 9 )
    {
        //This probability is now configurable from WorldServer.conf
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
    newdrop->item.appraised = false;
    newdrop->item.stats = 0;
    newdrop->item.count = 1;

    //chamod = a modifier based on the character's CHA stat. Increases the number of drops
    int chamod = (int)floor(thisclient->Attr->Cha / 20);
    if(chamod <0) chamod = 0;
    int basedrop = 6 + chamod; //Base number of items to be dropped. add CHA to increase this.
    if( newdrop->item.itemtype == 10 || newdrop->item.itemtype == 12 )
    {
        newdrop->item.count = RandNumber(0, basedrop);
        if(thismon->thisnpc->level == 1 && newdrop->item.count > 6) newdrop->item.count = 6; //limit the drop rate of items from level 1 event mobs
        if(newdrop->item.count==0)
            newdrop->item.count = 1;
        // Skillbooks & Chests
        if(newdrop->item.itemtype == 10)
        {
            if((newdrop->item.itemnum >=441 && newdrop->item.itemnum <= 888) ||
               (newdrop->item.itemnum >=247 && newdrop->item.itemnum <= 249) ||
               (newdrop->item.itemnum >=270 && newdrop->item.itemnum <= 275) ||
               (newdrop->item.itemnum >=1001 && newdrop->item.itemnum <= 1028) ||
               (newdrop->item.itemnum >=1110 && newdrop->item.itemnum <= 1178) ||
               (newdrop->item.itemnum >=1080 && newdrop->item.itemnum <= 1090) )
                newdrop->item.count = 1;   // just one skill book or chest per drop
        
        //441-888    Skills
        //247-249    Christmas Presents
        //270-275    Dirty Stones
        //1001-1028  Prison Chests
        //1110-1178  Dispensers
        //1080-1090  Event Boxes
        //1200-1201  Christmas Gift - Present Box - Mileage
        //1202-1203  Boy and Girl Snow Suit - Mileage
        
        }
        // Gem Drops
        if(newdrop->item.itemtype == 11)
            newdrop->item.count = 1;   // just one gem per drop

        if(newdrop->item.itemtype == 12)
        {
            if(newdrop->item.itemnum > 300 && newdrop->item.itemnum < 360) //bullets get a lot higher count.
            {
                newdrop->item.count *= 10;
                newdrop->item.count += 10;
            }
        }
    }
    else if( newdrop->item.itemtype >1 && newdrop->item.itemtype !=7 && newdrop->item.itemtype < 10)
    {
        // check to see if the item will be refined
        int prefine = rand()%100; //Probability of finding a refined item
        if(prefine < Config.RefineChance) // default = 5%
        {
            int refinelevel = rand()%101;  //which level of refine do we actually get
            if( refinelevel < 5)        //default 5%
                newdrop->item.refine = 3 * 16;
            else if( refinelevel < 11 )   //10%
                newdrop->item.refine = 2 * 16;
            else                          // 90%
                newdrop->item.refine = 16;
        }
        else //99%
            newdrop->item.refine = 0;

        // will the item be a blue?
        int blue = 0;
        int bluechance1 = RandNumber( 1, 100);
        int bluechance2 = Config.BlueChance + chamod;


        // will the items get stats? All blue items will.
        int pstats = rand()%101; //Probability of the item having stats. default = 5%

        //This probability is now configurable from WorldServer.conf. CHA also has an effect
        if(bluechance1 < bluechance2) // some percentage of drops will be specials or blues whenever one is available.
        {
            //Log( MSG_INFO, "Selected a blue item");
            int p = 1;
            while(alternate[n][p] != 0 && p < 8)
            {
                p++;
            }
            if(p > 1) // blues available for this item
            {
                p--;
                int bluenum = RandNumber( 1, p);
                newdrop->item.itemnum = alternate[n][bluenum];
                pstats = 1; //make sure we get stats for this item
            }
            //else
            //{
            //    //Sorry blue item not available for this item
            //}
        }
        //Uniques count as blues.
        if(newdrop->item.itemnum > 900)pstats = 1; //make sure we get stats for this unique item
        if( pstats < Config.StatChance)
        {   // default 5%
            //PY stats

            //LMA: Better stats if the player has a bonus medal.
            if (thisclient->bonusstatdrop!=1)
            {
                Log(MSG_INFO,"Better stats should come in this drop");
                newdrop->item.stats = GetExtraStats( 100 );
            }
            else
            {
                newdrop->item.stats = GetExtraStats( 0 );
            }

            //newdrop->item.stats = rand()%300;
        }

    }
    newdrop->item.gem = 0;

    //Log(MSG_INFO,"drop %i* (%i:%i)",newdrop->amount,newdrop->type,newdrop->item);
	*/
    return newdrop;
}

//LMA: Build Drop the PY way, with the AND system, with some changes as well.
CDrop* CWorldServer::GetPYDropAnd( CMonster* thismon, UINT droptype )
{   //if droptype = 1 then it is a normal drop. if it is 2 then it is a potential side drop.
    //Log(MSG_INFO,"GetPYDrop, monster %i, droptype %i",thismon->montype,droptype);
	// PY OLD and disabled. To be deleted later
	
    if(droptype == 2) // monster is still alive
    {
        // kicks it straight back if the monster is not dead
        if(thismon->thisnpc->side != 0) //perhaps we get a side drop??
        {
            if(GServer->RandNumber(0,100) < thismon->thisnpc->sidechance)
            {
                droptype = thismon->thisnpc->side;
            }
            else
            {
                return NULL;  //No drop this time
            }
        }
        else
        {
            return NULL;  //No drop this time
        }
    }

    CDrop* newdrop = new (nothrow) CDrop;
    if(newdrop==NULL)
    {
        Log(MSG_WARNING, "GetPYDropAnd:: Error allocing memory [getdrop]" );
        return NULL;
    }
	/*
    newdrop->clientid = GetNewClientID( );
    newdrop->posMap = thismon->Position->Map;
    newdrop->pos = RandInCircle( thismon->Position->current, 3 );
    newdrop->droptime = time(NULL);
    newdrop->owner = thismon->MonsterDrop->firsthit;
    newdrop->thisparty = thismon->thisparty;
    ClearItem(newdrop->item);

    CPlayer* thisclient = GServer->GetClientByCID(thismon->MonsterDrop->firsthit);
    if(thisclient == NULL)
    {
        ClearClientID(newdrop->clientid);
        delete(newdrop);
        Log(MSG_WARNING,"GetPYDropAnd:: Failed to find player");
        return NULL;
    }

    // code to modify drop chance for different levels
    //float charm = 0;
    float charm = (float)thisclient->Attr->Cha / 5;

    float leveldif = (float)thismon->thisnpc->level - (float)thisclient->Stats->Level;
    Log(MSG_INFO,"leveldif %.2f",leveldif);

    //LMA: Gray drops
    if (thisclient->bonusgraydrop!=0)
    {
        Log(MSG_INFO,"GetPYDropAnd:: Gray medal detected");
        leveldif=20.0;
    }

    //basic server rate + extra for player charm
    float droprate = (float)GServer->Config.DROP_RATE + charm;
    float dropchance = (droprate + (droprate * 0.01 * leveldif));
    Log(MSG_INFO,"charm %.2f, leveldif %.2f, droprate %.2f, dropchance %.2f",charm,leveldif,droprate,dropchance);

    if(dropchance < 0)
    {
        //always a small chance of a drop even when the mob is more than 20 levels beneath your own
        dropchance = 2;
    }

    Log(MSG_INFO,"dropchance %.2f",dropchance);
    if(thismon->thisnpc->level == 1)
    {
        dropchance = 80;
    }

    Log(MSG_INFO,"dropchance %.2f",dropchance);
    UINT lma_save_rand=0;
    lma_save_rand=GServer->RandNumber(0, 100);
    if (lma_save_rand>dropchance)
    {
        Log(MSG_INFO,"GetPYDropAnd:: no drop, %i > %.2f",lma_save_rand,dropchance);
        ClearClientID(newdrop->clientid);
        delete(newdrop);
        return NULL; // no drop here. not this time anyway.
    }

    Log(MSG_INFO,"drop possible, %i <= %.2f",lma_save_rand,dropchance);

    //CItemType prob[MDropList.size()];
    bool isdrop = false;
    int n = 0;
    int test = 0;
    long int probmax = 0;
    //int itemnumber[MDropList.size()];
    //int itemtype[MDropList.size()];
    //int probability[MDropList.size()];
    //int alternate[MDropList.size()][8];
    vector <CDropInfoAnd> MyMonsterDrops;

    //LMA: Priorities.
    bool prio_lvl=false;
    bool prio_monster=false;

    if( thismon->IsGhost())
    {
        // Stuff to do if the mob is a ghost of any type
        int selection = 1 + rand()%10;
        if( selection <= 3 ) //MP water
        {
            newdrop->type = 2;

            CDropInfoAnd ThisTempDrop;
            ThisTempDrop.item=399;
            ThisTempDrop.type=12;
            ThisTempDrop.prob=10;
            ThisTempDrop.nb_alternate=0;
            MyMonsterDrops.push_back(ThisTempDrop);

            probmax =10;
        }
        else if( selection <=6 ) //HP water
        {
            newdrop->type = 2;

            CDropInfoAnd ThisTempDrop;
            ThisTempDrop.item=400;
            ThisTempDrop.type=12;
            ThisTempDrop.prob=10;
            ThisTempDrop.nb_alternate=0;
            MyMonsterDrops.push_back(ThisTempDrop);

            probmax =10;
        }
        else  //skillbooks
        {
            for(int i=0; i<SkillbookList.size( ); i++)
            {
                newdrop->type = 2;
                CMDrops* thisdrop = GServer->SkillbookList.at(i);
                if(thisdrop->level_min <= thismon->thisnpc->level &&  thisdrop->level_max >= thismon->thisnpc->level)
                {
                    CDropInfoAnd ThisTempDrop;
                    ThisTempDrop.item=thisdrop->itemnum;
                    ThisTempDrop.type=thisdrop->itemtype;
                    ThisTempDrop.prob=thisdrop->prob;
                    ThisTempDrop.nb_alternate=0;
                    MyMonsterDrops.push_back(ThisTempDrop);

                    probmax += thisdrop->prob;
                }
            }
        }
    }
    else
    {
        int randv = RandNumber( 1, 100);
        if(randv <= GServer->Config.pc_drop_zuly)//x% zuly drop instead of item drop
        {
            newdrop->type = 1; //Drop Zuly
            newdrop->amount = thismon->thisnpc->level * 5 * Config.ZULY_RATE + RandNumber( 1, 10 );
            Log(MSG_INFO,"GetPYDropAnd:: zuly drop %u",newdrop->amount);
            return  newdrop;
        }
        // Stuff to do if the mob isn't a ghost

        //int randomdrop = GServer->RandNumber(1, 100);
        //enable the next line for debug purposes if you want to confirm a drop is working.
        //Log(MSG_INFO, "Mob type %i. Map = %i. Level = %i", thismon->montype, thismon->Position->Map,thismon->thisnpc->level);

        //LMA: AND code from PY, adapted :)

        //LMA: We check first the monster's map.
        if (DropsAnd.find(thismon->Position->Map)!=DropsAnd.end())
        {
            int nb_lines=DropsAnd[thismon->Position->Map].size( );

            for(int i=0; i<nb_lines; i++)
            {
                isdrop = true; // start out true then eliminate drops later

                CMDrops* thisdrop = GServer->DropsAnd[thismon->Position->Map].at(i);
                if(thisdrop->mob != 0 && thisdrop->mob != thismon->montype)
                {
                    isdrop = false;
                }

                if(thisdrop->level_max > thisdrop->level_min && (thismon->thisnpc->level < thisdrop->level_min || thismon->thisnpc->level > thisdrop->level_max))
                {
                    isdrop = false;
                }

                //testing area.
                if (isdrop&&thisdrop->a_x!=0)
                {
                    if (thismon->Position->current.x<isdrop&&thisdrop->a_x||thismon->Position->current.x>isdrop&&thisdrop->b_x)
                    {
                        isdrop=false;
                    }

                    if (thismon->Position->current.y>isdrop&&thisdrop->a_y||thismon->Position->current.x<isdrop&&thisdrop->b_y)
                    {
                        isdrop=false;
                    }

                }

                //LMA: 10000 now.
                test = GServer->RandNumber(1, 10000);
                if(thisdrop->prob < test)
                {
                    isdrop = false;
                }

                if(isdrop)
                {
                    //refreshing priorities.
                    if(thisdrop->level_max>0)
                    {
                        prio_lvl=true;
                    }

                    if(thisdrop->mob>0)
                    {
                        prio_monster=true;
                    }

                    CDropInfoAnd ThisTempDrop;
                    ThisTempDrop.item=thisdrop->itemnum;
                    ThisTempDrop.type=thisdrop->itemtype;
                    ThisTempDrop.prob=thisdrop->prob;

                    //We only use 7 alt drops.
                    //for(int k=1;k<8;k++)
                    //ThisTempDrop.nb_alternate=8;
                    ThisTempDrop.nb_alternate=7;
                    ThisTempDrop.alternate[7]=0;
                    for(int k=0;k<7;k++)
                    {
                        ThisTempDrop.alternate[k]=thisdrop->alt[k];
                    }

                    Log(MSG_INFO,"GetPYDropAnd:: drop added to possible drop list, item %i::%i, prob: %u, prio: (%i/%i)",ThisTempDrop.type,ThisTempDrop.item,ThisTempDrop.prob, prio_lvl,prio_monster);
                    MyMonsterDrops.push_back(ThisTempDrop);
                }

            }

        }

        //Do we need to check the single condition map?
        //monster only or level check only.
        if(!prio_monster||!prio_lvl)
        {
            Log(MSG_INFO,"GetPYDropAnd:: no priorities detected, going for normal drops.");
            int nb_lines=DropsAnd[0].size( );

            for(int i=0; i<nb_lines; i++)
            {
                isdrop = true; // start out true then eliminate drops later

                CMDrops* thisdrop = GServer->DropsAnd[0].at(i);

                //monster drop.
                if(prio_monster||(thisdrop->mob != 0 && thisdrop->mob != thismon->montype))
                {
                    isdrop = false;
                }

                //map drop (map drops changed to multiple drop check).
                
                //if(thisdrop->map != 0 && thisdrop->map != thismon->Position->Map)
                //{
                //    isdrop = false;
                //}
                

                //level drop.
                if(prio_lvl||(thisdrop->level_max > thisdrop->level_min && (thismon->thisnpc->level < thisdrop->level_min || thismon->thisnpc->level > thisdrop->level_max)))
                {
                    isdrop = false;
                }

                //LMA: 10000 now/
                test = GServer->RandNumber(1, 10000);
                if(thisdrop->prob < test)
                    isdrop = false;

                if(isdrop)
                {
                    CDropInfoAnd ThisTempDrop;
                    ThisTempDrop.item=thisdrop->itemnum;
                    ThisTempDrop.type=thisdrop->itemtype;
                    ThisTempDrop.prob=thisdrop->prob;

                    //LMA: Only 7 alternates.
                    //ThisTempDrop.nb_alternate=8;
                    //for(int k=1;k<8;k++)
                    ThisTempDrop.nb_alternate=7;
                    ThisTempDrop.alternate[7]=0;
                    for(int k=0;k<7;k++)
                    {
                        ThisTempDrop.alternate[k]=thisdrop->alt[k];
                    }

                    Log(MSG_INFO,"GetPYDropAnd:: drop added (1) to possible drop list, item %i::%i, prob: %u, prio: (%i/%i)",ThisTempDrop.type,ThisTempDrop.item,ThisTempDrop.prob, prio_lvl,prio_monster);
                    MyMonsterDrops.push_back(ThisTempDrop);
                }

            }

        }

    }

    n=MyMonsterDrops.size();
    Log(MSG_INFO,"GetPYDropAnd:: %i possible drops",n);

    //LMA: no drops...
    if(n == 0)
    {
        ClearClientID(newdrop->clientid);
        //LMA: Forcing the vector to free the memory.
        //MyMonsterDrops.clear();
        vector<CDropInfoAnd>().swap(MyMonsterDrops);
        delete(newdrop);
        return NULL;
    }

    int maxitems = n;
    //maxitems is the number of items in the shortlist

    // randomize the item from the shortlist. items get equal chance
    n = GServer->RandNumber(0, maxitems);
    if(n>=maxitems)
    {
        Log(MSG_WARNING,"GetPYDropAnd:: overflow detected");
        n=0;
    }

    newdrop->item.itemnum = MyMonsterDrops.at(n).item;
    newdrop->item.itemtype = MyMonsterDrops.at(n).type;
    newdrop->type = 2;

    Log(MSG_INFO,"GetPYDropAnd:: chosen item is offset %i, %i::%i",n,newdrop->item.itemtype,newdrop->item.itemnum);

    newdrop->item.lifespan = 10 + rand()%80;
    float dmod = 0; //random number from 0 to 100 made up of 4 sub numbers to keep
    //the average value near to 50
    for(int i=0; i<4; i++)
    {
        float r1 = rand()%20;
        dmod += r1;
    }
    newdrop->item.durability = 10 + (int)dmod;
    if( newdrop->item.itemtype == 8 || newdrop->item.itemtype == 9 )
    {
        //This probability is now configurable from WorldServer.conf
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
    newdrop->item.appraised = false;
    newdrop->item.stats = 0;
    newdrop->item.count = 1;

    //chamod = a modifier based on the character's CHA stat. Increases the number of drops
    int chamod = (int)floor(thisclient->Attr->Cha / 20);
    if(chamod <0) chamod = 0;
    int basedrop = 6 + chamod; //Base number of items to be dropped. add CHA to increase this.
    if( newdrop->item.itemtype == 10 || newdrop->item.itemtype == 12 )
    {
        newdrop->item.count = RandNumber(0, basedrop);
        if(thismon->thisnpc->level == 1 && newdrop->item.count > 6) newdrop->item.count = 6; //limit the drop rate of items from level 1 event mobs
        if(newdrop->item.count==0)
            newdrop->item.count = 1;
        // Skillbooks & Chests
        if(newdrop->item.itemtype == 10)
        {
             if (
                   //(newdrop->item.itemnum >=70 && newdrop->item.itemnum <= 72) ||     //not in break list
                   (newdrop->item.itemnum >=75 && newdrop->item.itemnum <= 78) ||
                   (newdrop->item.itemnum >=247 && newdrop->item.itemnum <= 249) ||
                   (newdrop->item.itemnum >=256 && newdrop->item.itemnum <= 258) ||
                   (newdrop->item.itemnum >=270 && newdrop->item.itemnum <= 276) ||
                   (newdrop->item.itemnum >=341 && newdrop->item.itemnum <= 347) ||
                   (newdrop->item.itemnum >=441 && newdrop->item.itemnum <= 889) ||
                   //(newdrop->item.itemnum >=905 && newdrop->item.itemnum <= 907) ||   //not in break list
                   (newdrop->item.itemnum >=946) || (newdrop->item.itemnum >=996 && newdrop->item.itemnum <= 1000) ||
                   (newdrop->item.itemnum >=1001 && newdrop->item.itemnum <= 1028) ||
                   (newdrop->item.itemnum >=1080 && newdrop->item.itemnum <= 1082) ||
                   (newdrop->item.itemnum >=1110 && newdrop->item.itemnum <= 1178) ||
                   (newdrop->item.itemnum >=1200 && newdrop->item.itemnum <= 1220)
                )
            {
                newdrop->item.count = 1;   // just one skill book or chest per drop
            }

            
            //70-72      Gift Box
            //75-78      Full moon Gift, Gift Box, Package of biscuits
            //441-888    Skills
            //247-249    Christmas Presents
            //256-258    Hallowwen Garb Bag, Christmas box
            //270-276    Dirty Stones
            //341-347    Eldeon Box
            //905-907    Gift Box
            //946        Christmas Gift Box
            //996-1000   Rose Treasure Chest (Mileage)
            //1001-1028  Prison Chests
            //1080-1082  Event Boxes
            //1110-1178  Dispensers
            //1200-1220  Christmas Gift - Present Box - Mileage
            //1202-1203  Boy and Girl Snow Suit - Mileage
            
        }

        // Gem Drops
        if(newdrop->item.itemtype == 11)
        {
            newdrop->item.count = 1;   // just one gem per drop
        }

        if(newdrop->item.itemtype == 12)
        {
            if(newdrop->item.itemnum > 300 && newdrop->item.itemnum < 360) //bullets get a lot higher count.
            {
                newdrop->item.count *= 10;
                newdrop->item.count += 10;
            }

        }

    }
    else if( newdrop->item.itemtype >1 && newdrop->item.itemtype !=7 && newdrop->item.itemtype < 10)
    {
        // check to see if the item will be refined
        int prefine = rand()%100; //Probability of finding a refined item
        if(prefine < Config.RefineChance) // default = 5%
        {
            int refinelevel = rand()%101;  //which level of refine do we actually get
            if( refinelevel < 5)        //default 5%
                newdrop->item.refine = 3 * 16;
            else if( refinelevel < 11 )   //10%
                newdrop->item.refine = 2 * 16;
            else                          // 90%
                newdrop->item.refine = 16;
        }
        else //99%
            newdrop->item.refine = 0;

        // will the item be a blue?
        int blue = 0;
        int bluechance1 = RandNumber( 1, 100);
        int bluechance2 = Config.BlueChance + chamod;


        // will the items get stats? All blue items will.
        int pstats = rand()%101; //Probability of the item having stats. default = 5%

        //This probability is now configurable from WorldServer.conf. CHA also has an effect
        if(bluechance1 < bluechance2) // some percentage of drops will be specials or blues whenever one is available.
        {
            //Log( MSG_INFO, "Selected a blue item");
            //LMA: 7 alt
            int p = 1;
            //while(MyMonsterDrops.at(n).alternate[p] != 0 && p < 8)
            while(p < 8 && MyMonsterDrops.at(n).alternate[p-1] != 0)
            {
                p++;
            }

            if(p > 1) // blues available for this item
            {
                p--;
                //int bluenum = RandNumber( 1, p);
                int bluenum = RandNumber( 0, p);
                if(bluenum>=p)
                {
                    bluenum=0;
                    Log(MSG_WARNING,"GetPYDropAnd:: overflow trapped in blue");
                }

                newdrop->item.itemnum = MyMonsterDrops.at(n).alternate[bluenum];
                Log(MSG_INFO,"GetPYDropAnd:: Alt drop offset %i, %i::%i",bluenum,newdrop->item.itemtype,newdrop->item.itemnum);
                pstats = 1; //make sure we get stats for this item
            }

        }

        //Uniques count as blues.
        if(newdrop->item.itemnum > 900)pstats = 1; //make sure we get stats for this unique item
        if( pstats < Config.StatChance)
        {   // default 5%
            //PY stats

            //LMA: Extra stats (medal for example).
            if (thisclient->bonusstatdrop!=1)
            {
                Log(MSG_INFO,"GetPYDropAnd:: Better stats (medal) should come in this drop");
                newdrop->item.stats = GetExtraStats( 100 );
            }
            else
            {
                newdrop->item.stats = GetExtraStats( 0 );
            }

            //newdrop->item.stats = rand()%300;
        }

    }

    newdrop->item.gem = 0;
    //LMA: Forcing the vector freeing the memory.
    //MyMonsterDrops.clear();
    vector<CDropInfoAnd>().swap(MyMonsterDrops);


    //Log(MSG_INFO,"drop %i* (%i:%i)",newdrop->amount,newdrop->type,newdrop->item);
	*/
    return newdrop;
}

// Build Drop. New experimental method.
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



    int droprate = 80; // base drop percentage for monsters your own level
    int DropFloor = Config.BASE_DROP_CHANCE;
    int leveldif = (int)((float)thismon->thisnpc->level - (float)thisclient->Stats->Level);
    float mod = droprate * 0.01 * leveldif;
    int dropchance = int(droprate + mod);
    if(dropchance < DropFloor) dropchance = DropFloor; //always a small chance of a drop even when the mob is more than 20 levels beneath your own
    if(thismon->thisnpc->level == 1)
        dropchance = 80;
    //if( thisclient->Session->codedebug )
    //    SendPM(thisclient, "Drop chance (based on level diference) = %i", dropchance );
    int droprand = GServer->RandNumber(0,100);
    if (droprand > dropchance)
    {
        if( thisclient->Session->codedebug )
            SendPM(thisclient, "Drop rate check failed with %i. No drop this time",droprand);
        return NULL; // no drop here. not this time anyway.
    }


    // Each monster has its own rates for zuly and item drops defined in the stb
    // first check for Zuly drops
    randv = RandNumber( 1, 100);
    //if( thisclient->Session->codedebug )
    //{
    //   SendPM(thisclient, "randv = %i.  money = %i. zulyrate = %i. ", randv,thismon->thisnpc->money,thisclient->Stats->zulydroprate );
    //}
    if(randv <= (thismon->thisnpc->money + thisclient->Stats->zulydroprate)) // zuly drop
    {
        newdrop->type = 1; //zuly
        newdrop->amount = thismon->thisnpc->level * 5 * (Config.ZULY_RATE + thisclient->Stats->zulydroprate) + RandNumber( 1, 10 );
        if( thisclient->Session->codedebug )
        {
           SendPM(thisclient, "Zuly drop selected " );
        }
        return  newdrop;
    }
    //now check for item drops
    //if( thisclient->Session->codedebug )
    //{
    //   SendPM(thisclient, "randv = %i. Item = %i. Itemrate = %i", randv,thismon->thisnpc->item,thisclient->Stats->itemdroprate );
    //}
    if(randv <= (thismon->thisnpc->item + thisclient->Stats->itemdroprate + thismon->thisnpc->money)) // Item drop
    {
        //OK got an item. Now we find which one
        //Map drop or mob drop? map = 50% mob = 50%. Maybe should be configurable or modified by monster data?
        int randv2 = RandNumber( 1, 100);
        if(randv2 <= 50) //map drop
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

        i=0;
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
        int chamod = (int)floor(TmpMod);
        if(chamod < 0) chamod = 0;
        int basedrop = thisclient->Stats->itemdroprate; //Base number of items to be dropped.
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
        else if( newdrop->item.itemtype >1 && newdrop->item.itemtype !=7 && newdrop->item.itemtype < 10)
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
            if(newdrop->item.raretype > 0) // it's a blue item
            {
                newdrop->item.durability += 10; //make blues a little more durable
                newdrop->item.appraised = false; //blues need to be appraised
                newdrop->item.stats = RandNumber(0, 29) * 10 + 1; // stats can only be 1,11,21,,,,,291
            }
            else // not a blue item
            {
                int pstats = RandNumber(1, 100);
                if( pstats < StatChance)        // default 5%
                    newdrop->item.stats = RandNumber(0, 29) * 10 + 1; // stats can only be 1,11,21,,,,,291
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

 // PY extra stats lookup
 UINT CWorldServer::GetExtraStats( UINT modifier )
 {
    UINT stat = rand()%300 + modifier;
    if(stat > 300)stat = 300;
    return StatLookup[stat].statnumber;
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
    if(rclass==player_job)
        return true;

    if(player_job>=121&&player_job<=122)
    {
        if(rclass==111)
        {
            return true;
        }
        else
        {
            return false;
        }

    }

    if(player_job>=221&&player_job<=222)
    {
        if(rclass==211)
        {
            return true;
        }
        else
        {
            return false;
        }

    }

    if(player_job>=321&&player_job<=322)
    {
        if(rclass==311)
        {
            return true;
        }
        else
        {
            return false;
        }

    }

    if(player_job>=421&&player_job<=422)
    {
        if(rclass==411)
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

    int b=1;
    CSkills* thisskill = GetSkillByID( skill );
    if( thisskill==NULL )
        return false;

    //LMA: Is it an empty skill?
    if(GServer->SkillList[skill]->skill_tab==0)
    {
        Log(MSG_WARNING,"%s:: Incorrect or empty skill %i",thisclient->CharInfo->charname,skill);
        b=6;
    }

    if(takeSP&&thisclient->CharInfo->SkillPoints<thisskill->sp )
    {
        Log(MSG_WARNING,"%s:: not enough skill points (%u<%u) for skill %u",thisclient->CharInfo->charname,thisclient->CharInfo->SkillPoints,thisskill->sp,skill);
        b=7;
    }
    else if( thisskill->clevel>thisclient->Stats->Level )
    {
        Log(MSG_WARNING,"%s:: incorrect level for skill %u (%u<%u)",thisclient->CharInfo->charname,skill,thisskill->clevel,thisclient->Stats->Level);
        b=4;
    }

    if(b==1)
    {
        UINT rclass = 0;
        for(UINT i=0;i<4; i++)
        {
            if (thisskill->c_class[i] == 0)
            {
                continue;
            }

            if (thisskill->c_class[i] == 41)
            {
                rclass = 111;
            }
             else if (thisskill->c_class[i] == 42)
            {
                rclass = 211;
            }
            else if (thisskill->c_class[i] == 43)
            {
                rclass = 311;
            }
            else if (thisskill->c_class[i] == 44)
            {
                rclass = 411;
            }
            else if (thisskill->c_class[i] == 61)
            {
                rclass = 121;
            }
            else if (thisskill->c_class[i] == 62)
            {
                rclass = 122;
            }
            else if (thisskill->c_class[i] == 63)
            {
                rclass = 221;
            }
            else if (thisskill->c_class[i] == 64)
            {
                rclass = 222;
            }
            else if (thisskill->c_class[i] == 65)
            {
                rclass = 321;
            }
            else if (thisskill->c_class[i] == 66)
            {
                rclass = 322;
            }
            else if (thisskill->c_class[i] == 67)
            {
                rclass = 421;
            }
            else if (thisskill->c_class[i] == 68)
            {
                rclass = 422;
            }

            //LMA: new way, so "old" job skill can be learned...
            //if(rclass == thisclient->CharInfo->Job)
            if(CheckCompatibleClass(rclass,thisclient->CharInfo->Job))
            {
                b=1;
                break;
            }
            else
            {
                Log(MSG_WARNING,"%s:: Incompatible class for skill %u (%u)",thisclient->CharInfo->charname,skill,rclass);
                b=2;
            }
        }
    }
    if(b==1)
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

    if(b==1)
    {
        //LMA: Looking for good place to save it now...
        int family=thisclient->GoodSkill(skill);
        if(family==-1)
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

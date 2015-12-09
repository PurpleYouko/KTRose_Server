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

// Give Exp
bool CWorldServer::GiveExp( CMonster* thismon, UINT special_lvl, UINT special_exp )
{
    if( thismon->owner != 0) // Summon
	{
        MapList.Index[thismon->Position->Map]->DeleteMonster( thismon );
	    return true;
    }

    //LMA TEST
    bool lma_debug=false;
   if(thismon->Position->respawn==4589)
   {
       lma_debug=true;
       Log(MSG_INFO,"GiveExp Spawn %u CID %u",thismon->Position->respawn,thismon->clientid);
   }

    // Give Experience Drops and Quest Items
    vector<CPartyExp*> PartyExp;
    for(UINT i=0;i<thismon->PlayersDamage.size();i++)
    {
        MonsterDamage* thisplayer = thismon->PlayersDamage.at(i);
		CPlayer* thisclient = GetClientByCID( thisplayer->charid, thismon->Position->Map );

        //LMA: Player mustn't be dead.
		if( thisplayer->damage>0 && thisclient!=NULL && !thisclient->IsDead())
        {
    		if( thisclient->Battle->target == thismon->clientid )
            {
                ClearBattle( thisclient->Battle )
                thisclient->Position->destiny = thisclient->Position->current; //GIVE EXP FUNCTION
            }
            if( thismon->MonsterDrop->firsthit == thisclient->CharInfo->charid )
            {
                for( int q=0;q<10;q++)
                {
                    // Give Quest Item
                    if( thisclient->quest.quests[q].QuestID!=0 )
                    {
                        BEGINPACKET( pak, 0x731 )
                        ADDWORD    ( pak, thismon->montype );
                        thisclient->client->SendPacket( &pak );
                        break;
                    }
                }

            }

            //LMA BEGIN
            //20070621-211100
            //mods for CF...
            //unsigned int exp = (unsigned int)ceil((thismon->thisnpc->exp * thisplayer->damage) / (thismon->thisnpc->hp*thismon->thisnpc->level)+special_exp);
            unsigned long long exp = (unsigned long long)ceil((double)(thismon->thisnpc->exp * thisplayer->damage) / (thismon->thisnpc->hp*thismon->thisnpc->level)+special_exp);
            //LMA END

            /*unsigned int cp = (unsigned int)ceil((thismon->thisnpc->level/6 * thisplayer->damage) / thismon->thisnpc->hp);
            //ADD Clan points under any condition
            if ((thismon->Position->Map>=11 && thismon->Position->Map<= 13) || thismon->Position->Map== 59) //So far only junon maps
            {
               thisclient->AddClanPoints(GetColorExp( cp));
            }*/

            exp = exp * (unsigned long long) (Config.EXP_RATE * 6);
            if( thisclient->Party->party!=NULL )
            {
                bool pflag = false;

                for(int i=0;i<PartyExp.size();i++)
                {
                    CPartyExp* thisparty = PartyExp.at( i );
                    if( thisparty->thisparty == thisclient->Party->party )
                    {
                        //LMA: We get exp only if we deserve it.
                        //thisparty->exp += exp;
                        thisparty->exp += GetColorExp( thisclient->Stats->Level, thismon->thisnpc->level + special_lvl, exp );
                        //LMA: bug...
                        //thisparty->exp += exp * (unsigned long long) ((thisclient->Party->party->PartyLevel*2) / 100);
                        thisparty->exp += (unsigned long long) ((exp*thisclient->Party->party->PartyLevel*2) / 100);
                        pflag = true;
                    }
                }

                if( !pflag )
                {
                    CPartyExp* thisparty = new CPartyExp;
                    thisparty->thisparty = thisclient->Party->party;
                    //LMA: We get exp only if we deserve it.
                    thisparty->exp = exp;
                    thisparty->exp = GetColorExp( thisclient->Stats->Level, thismon->thisnpc->level + special_lvl, exp );
                    thisparty->flag = false;
                    //LMA: bug
                    //thisparty->exp += exp * (unsigned long long) ((thisclient->Party->party->PartyLevel*2) / 100);
                    thisparty->exp += (unsigned long long) ((exp*thisclient->Party->party->PartyLevel*2) / 100);
                    thisparty->num = 1;
                    thisparty->partymember[0] = thisclient->CharInfo->charid;
                    thisparty->cheat_max_lvl=thisclient->Stats->Level;
                    thisparty->cheat_min_lvl=thisclient->Stats->Level;
                    thisparty->maxlevel = thisclient->Stats->Level;
                    for(unsigned int p=0;p<ClientList.size();p++)
                    {
                        CPlayer* otherclient = (CPlayer*) ClientList.at( p )->player;
                        if(otherclient->client==NULL) continue;
                        if(otherclient->Party->party==NULL) continue;
                        if(!otherclient->client->isActive) continue;
                        if(!otherclient->Session->inGame) continue;

                        //LMA: checking the gap between the max and min level of people in the party.
                        if (thisclient->Party->party == otherclient->Party->party)
                        {
                            if(otherclient->Stats->Level>thisparty->cheat_max_lvl)
                            {
                                thisparty->cheat_max_lvl=otherclient->Stats->Level;
                            }

                            if(otherclient->Stats->Level<thisparty->cheat_min_lvl)
                            {
                                thisparty->cheat_min_lvl=otherclient->Stats->Level;
                            }

                        }

                        if( IsVisible(thisclient, otherclient))
                        {
                            if(thisclient->Party->party == otherclient->Party->party )
                            {
                                //LMA: Little stupid check.
                                if (thisparty->num==7)
                                {
                                    Log(MSG_WARNING,"Too many people in this party, already at max, skipping");
                                    break;
                                }

                                thisparty->partymember[thisparty->num] = otherclient->CharInfo->charid;
                                thisparty->num++;
                                thisparty->maxlevel += otherclient->Stats->Level;
                            }
                        }
                    }
                    PartyExp.push_back( thisparty );
                }
                continue;
            }

             //LMA BEGIN
             //20070621-211100
            //mod for CF
            //Adding bonusxp (mileage)
            //UINT prev_xp=thisclient->CharInfo->Exp;

            //LMA: Xp nullifier.
            if(!thisclient->no_exp)
            {
                int tempexp = thisclient->bonusxp*GetColorExp( thisclient->Stats->Level, thismon->thisnpc->level + special_lvl, exp );
				if (tempexp <= 0) 
					tempexp = thismon->thisnpc->exp;
				thisclient->CharInfo->Exp +=  tempexp;
            }

            //Log(MSG_INFO,"Bonus XP %i, previous XP, %i, new: %i",thisclient->bonusxp,prev_xp,thisclient->CharInfo->Exp);
            //LMA END

            //LMA: We don't send exp packet if there is a level up coming up next.
            if(thisclient->CharInfo->Exp < thisclient->GetLevelEXP())
            {
                //LMA: TEST
                //Log(MSG_INFO,"new exp in giveexp %I64i",thisclient->CharInfo->Exp);
                BEGINPACKET( pak, 0x79b );
                ADDDWORD   ( pak, thisclient->CharInfo->Exp );
                ADDWORD    ( pak, thisclient->CharInfo->stamina );
                //ADDWORD    ( pak, 0 );
                ADDWORD    ( pak, thismon->clientid );
                thisclient->client->SendPacket( &pak );
            }

        }

    }


    for(int p=0;p<PartyExp.size();p++)
    {
        CPartyExp* thisparty = PartyExp.at( p );
        //LMA: no exp if a player has a too low level... This is another version in this case, NONE of the guys got exp...
        //HIGHRATEPARTYEXPNONE
        /*if(abs(thisparty->cheat_min_lvl-thisparty->cheat_max_lvl)>(Config.Partygap+1))
        {
            Log(MSG_HACK,"A Party tryes to get exp which has min_lvl %i and max_lvl %i that don't fit max gap %i",thisparty->cheat_min_lvl,thisparty->cheat_max_lvl,Config.Partygap+1);
            continue;
        }*/
        //LMA: End of HIGHRATEPARTYEXPNONE.

        for(int i=0;i<thisparty->num;i++)
        {
            CPlayer* partyclient = GetClientByCID( thisparty->partymember[i], thismon->Position->Map );
            if( partyclient==NULL )
                continue;
            if(partyclient->Party->party==NULL)
                continue;

            //LMA: no exp if a player has a too low level...
            //Uncomment this part of code below if you want to use it... (HIGHRATEPARTYEXP)
            /*if(abs(partyclient->Stats->Level-thisparty->cheat_max_lvl)>(Config.Partygap+1))
            {
                Log(MSG_HACK,"Player %s is lvl %i and tryes to get exp in a party which has max_lvl %i and max gap %i",partyclient->CharInfo->charname,partyclient->Stats->Level,thisparty->cheat_max_lvl,Config.Partygap+1);
                continue;
            }*/
            //Uncomment should end here for HIGHRATEPARTYEXP.

            if(!thisparty->flag)
            {
                //partyclient->Party->party->Exp += (thisparty->exp / 6) / 2;
                partyclient->Party->party->Exp += (thisparty->exp / 6) / 6; //Tomiz: thisparty->exp decreased
                if( partyclient->Party->party->PartyLevel == 50)
                {
                    partyclient->Party->party->Exp = 0;
                }

                //LMA: old code
                /*if( partyclient->Party->party->Exp > GetMaxPartyExp( partyclient->Party->party->PartyLevel ) )
                {
                    partyclient->Party->party->PartyLevel++;
                    partyclient->Party->party->Exp -= GetMaxPartyExp( partyclient->Party->party->PartyLevel-1 );
                }
                */

                //New code.
                bool new_level=false;
                while(partyclient->Party->party->PartyLevel<50&&partyclient->Party->party->Exp > GetMaxPartyExp( partyclient->Party->party->PartyLevel ) )
                {
                    partyclient->Party->party->PartyLevel++;
                    partyclient->Party->party->Exp -= GetMaxPartyExp( partyclient->Party->party->PartyLevel-1 );
                    Log(MSG_INFO,"Going from party level %i to %i, exp left %I64i",partyclient->Party->party->PartyLevel-1,partyclient->Party->party->PartyLevel,partyclient->Party->party->Exp);
                    new_level=true;
                }

                if( partyclient->Party->party->PartyLevel == 50)
                {
                    partyclient->Party->party->Exp = 0;
                }

                //Strange thing when a new level:
                unsigned long long send_exp=partyclient->Party->party->Exp;
                if(new_level)
                {
                    send_exp+=0x80000000;
                }

                Log(MSG_INFO,"Exp sent: %I64i",send_exp);
                //End of code.

                BEGINPACKET( pak, 0x7d4 );
                ADDBYTE    ( pak, partyclient->Party->party->PartyLevel );

                //LMA: little change.
                //ADDDWORD   ( pak, partyclient->Party->party->Exp );
                ADDDWORD   ( pak, send_exp );

                partyclient->Party->party->SendToMembers( &pak );
                thisparty->flag = true;
            }

            //LMA: no exp for dead people.
            if(partyclient->IsDead())
            {
                Log(MSG_INFO,"No exp for player %s, he's dead.",partyclient->CharInfo->charname);
                continue;
            }

            //LMA Begin
            //20070621-211100
            //mods for CF
            unsigned int expoption = partyclient->Party->party->Option%0x80;
            //LMA: Exp nullfier.
            if(!partyclient->no_exp)
            {
                if( expoption==0 )
                {

                    partyclient->CharInfo->Exp +=  GetColorExp( partyclient->Stats->Level, thismon->Stats->Level + special_lvl, (UINT)round(thisparty->exp / thisparty->num) );
                }
                else
                {
                    partyclient->CharInfo->Exp +=  GetColorExp( partyclient->Stats->Level, thismon->Stats->Level + special_lvl, (UINT)round(partyclient->Stats->Level * thisparty->exp / thisparty->maxlevel) );
                }

            }
            //LMA END

            if(partyclient->CharInfo->Exp<partyclient->GetLevelEXP())
            {
                //LMA: TEST
                //Log(MSG_INFO,"new exp in giveexp %I64i",partyclient->CharInfo->Exp);
                BEGINPACKET( pak, 0x79b );
                ADDDWORD   ( pak, partyclient->CharInfo->Exp );
                ADDWORD    ( pak, partyclient->CharInfo->stamina );
                //ADDWORD    ( pak, 0 );
                ADDWORD    ( pak, thismon->clientid );
                partyclient->client->SendPacket( &pak );
            }

        }
    }

    //LMA: cleaning time.
    for(int p=0;p<PartyExp.size();p++)
    {
        //It seems it's ok, the class party destructor isn't beeing called...
        delete PartyExp.at(p);
    }

    if (lma_debug)
    {
        Log(MSG_INFO,"END GiveExp Spawn %u CID %u",thismon->Position->respawn,thismon->clientid);
    }

    //PY: This bloody line of code has been driving me mad by deleting monsters out of time and place. Good ridance to it. More lmame stuff i suspect
	//MapList.Index[thismon->Position->Map]->DeleteMonster( thismon );


    return true;
}

// Give Exp
bool CWorldServer::GiveExp( CMonster* thismon )
{
    //Log(MSG_DEBUG,"Awarding EXP");
    int tmpMult = 1;
    if( thismon->owner != 0) // Summon
	{
        MapList.Index[thismon->Position->Map]->DeleteMonster( thismon );
	    return true;
    }
    // Give Experience Drops and Quest Items
    vector<CPartyExp*> PartyExp;
    vector<CParty*> PartyList;
	CMap* map = GServer->MapList.Index[thismon->Position->Map];
    for(UINT i=0;i<thismon->PlayersDamage.size();i++)
    {
        MonsterDamage* thisplayer = thismon->PlayersDamage.at(i);
		CPlayer* thisclient = GetClientByCID( thisplayer->charid, thismon->Position->Map );
		if( thisplayer->damage > 0 && thisclient != NULL ) //player did some damage
        {
    		Log(MSG_DEBUG,"Player did %i damage. max = %i",thisplayer->damage,thismon->Stats->MaxHP);
    		float MyPercent = (float)thisplayer->damage / thismon->Stats->MaxHP;
			if(MyPercent > thisclient->CharInfo->HighestOverkill)
			{
				thisclient->CharInfo->HighestOverkill = MyPercent;
				SendPM(thisclient, "Congratulations!! You have exceeded your highest ever Overkill rate. New Best: %f",thisclient->CharInfo->HighestOverkill);
			}
    		if(MyPercent > GServer->Config.MaxOverkill)MyPercent = GServer->Config.MaxOverkill;   //set overkill ceiling
			Log(MSG_DEBUG,"Percentage multiplier %f",MyPercent);
            if( thisclient->Battle->target == thismon->clientid )
            {
                ClearBattle( thisclient->Battle )
                thisclient->Position->destiny = thisclient->Position->current;
                //Log(MSG_DEBUG,"(GiveExp) Destiny set to current position X: %f Y: %f.",thisclient->Position->current.x,thisclient->Position->current.y);
            }
            if( thismon->MonsterDrop->firsthit == thisclient->CharInfo->charid )
            {
                for( int q=0;q<10;q++)
                {
                    // Give Quest Item
                    if( thisclient->quest.quests[q].QuestID!=0 )
                    {
                        Log(MSG_DEBUG,"Giving quest reward item for quest %i",thisclient->quest.quests[q].QuestID);
                        BEGINPACKET( pak, 0x731 )
                        ADDWORD    ( pak, thismon->montype );
                        thisclient->client->SendPacket( &pak );
                        break;
                    }
                }
            }
            //assign my own exp for monsters that I personally damaged
            unsigned int exp = (unsigned int)floor(thismon->thisnpc->exp * MyPercent);
            //unsigned int exp = (unsigned int)ceil((double)((thismon->thisnpc->exp * thisplayer->damage) / (thismon->thisnpc->hp*thismon->thisnpc->level)));
            exp = exp * Config.EXP_RATE * map->mapXPRate *   MyPercent;      //calculate base exp for this client. No medals or stuff accounted for yet
			
            Log(MSG_DEBUG,"MonXP: %i config rate: %i Map rate : %i My percent: %f Total XP: %i", thismon->thisnpc->exp, Config.EXP_RATE, map->mapXPRate, MyPercent, exp);
            thisclient->CharInfo->Pending_Exp += (exp * thisclient->Stats->xprate);    //store exp into thisclient's pending_exp using personal xprate adjustments
			Log(MSG_DEBUG,"My personal XPrate: %i ", thisclient->Stats->xprate);
            if( thisclient->Party->party!=NULL )
            {
                //Log(MSG_DEBUG,"Player is in a party");
                //player is in a party so scan the party members
                CParty* party = thisclient->Party->party; //assign a party
                if(party == NULL)
                    return true;
                //Log(MSG_DEBUG,"party found. Counted = %i", party->counted);
                for(UINT p=0;p<party->Members.size();p++) //loop through all the members in the party
                {
                    //Log(MSG_DEBUG,"member %i being parsed", p);
                    if(!party->counted)
                    {
                        //Log(MSG_DEBUG,"party added to list. level = %i", party->PartyLevel);
                        party->counted = true; //tag the party so we don't add it to the list twice
                        PartyList.push_back( party ); //we will need this list later to convert pending exp to real exp
                    }
                    CPlayer* thismember = party->Members.at(p); //find a party member
                    if(thismember == NULL)
                        return true;
					float RawTmpExp = (exp * (party->PartyLevel + 25) / 50);
                    unsigned int tempxp =(unsigned int)(floor)(RawTmpExp);
                    if (tempxp < 1)tempxp = 1;
                    //Log(MSG_DEBUG,"member %i pending exp = %i tempxp: %i", p, thismember->CharInfo->Pending_Exp, tempxp);
                    thismember->CharInfo->Pending_Exp += tempxp; // add a percentage of thisclient's non-adjusted exp to all party members including himself

                    //Log(MSG_DEBUG,"member %i pending exp (after tempxp) = %i", p, thismember->CharInfo->Pending_Exp);
                }
                if(party->PartyLevel < 50) //only give party exp if party level is under 50
                {

                    party->Pending_Exp += exp; //add thisclient's non-adjusted xp to the pending exp of the party
                }

               
            }
            else //not in a party so deal with all the exp now
            {
                //SendPM(thisclient, "You receive %i EXP",thisclient->CharInfo->Pending_Exp);
                //Log(MSG_DEBUG,"Player awarded %i experience points",thisclient->CharInfo->Pending_Exp);
				thisclient->CharInfo->Exp += thisclient->CharInfo->Pending_Exp;
                thisclient->CharInfo->Pending_Exp = 0;
                //if(!thisclient->CheckPlayerLevelUP())
				if(thisclient->CharInfo->Exp < thisclient->GetLevelEXP())
                {
					BEGINPACKET( pak, 0x79b );
                    ADDDWORD   ( pak, thisclient->CharInfo->Exp );
                    ADDWORD    ( pak, thisclient->CharInfo->stamina );
					ADDWORD    ( pak, thismon->clientid );
                    thisclient->client->SendPacket( &pak );
                }
            }
        }
    }
    for(int p=0;p<PartyList.size();p++) //loop through our party list to assign final exp to all party members. We already did non-party members up there^
    {

        CParty* thisparty = PartyList.at( p );

        if(thisparty == NULL)
        {
            //Log(MSG_DEBUG,"Party not valid");
            continue;
        }
        //Log(MSG_DEBUG,"Party %i exp: %i pending exp: %i", p, thisparty->Exp, thisparty->Pending_Exp );
        thisparty->counted = false;         //reset the boolean for next time
        for(UINT i=0;i<thisparty->Members.size();i++) //loop through all the members in the party
        {
            CPlayer* thismember = thisparty->Members.at(i); //find a party member
            if(thismember == NULL)
                return true;
            thismember->CharInfo->Exp += thismember->CharInfo->Pending_Exp;
            //Log(MSG_DEBUG,"Added pending exp %i to regular exp %i for member %i", thismember->CharInfo->Pending_Exp, thismember->CharInfo->Exp, i);
            thismember->CharInfo->Pending_Exp = 0;
            if(!thismember->CheckPlayerLevelUP( ))
            {
                BEGINPACKET( pak, 0x79b );
                ADDDWORD   ( pak, thismember->CharInfo->Exp );
                ADDWORD    ( pak, thismember->CharInfo->stamina );
                //ADDWORD    ( pak, 0 );		//PY: not needed
				ADDWORD    ( pak, thismon->clientid );
                thismember->client->SendPacket( &pak );
            }

        }
        thisparty->Exp += thisparty->Pending_Exp;
        thisparty->Pending_Exp = 0;
		unsigned int m_bitlevelup = 0;
        if( thisparty->Exp > GetMaxPartyExp(thisparty->PartyLevel)) //level up the party
        {
            thisparty->PartyLevel++;
            thisparty->Exp -= GetMaxPartyExp(thisparty->PartyLevel-1);
			m_bitlevelup = 1;	//set levelup bit. See client structure below
        }
		//PY: structure of 0x7d4
		/*BYTE				m_btLEVEL;
		struct 
		{
			unsigned int	m_iEXP		 : 31;
			unsigned int	m_bitLevelUP : 1;
		} ;
		*/
        BEGINPACKET	( pak, 0x7d4 );
        ADDBYTE		( pak, thisparty->PartyLevel );
        ADDWORD		( pak, thisparty->Exp );
		ADDWORD		( pak, m_bitlevelup );
        thisparty->SendToMembers( &pak );
    }
    return true;
}




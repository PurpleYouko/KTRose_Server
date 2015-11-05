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
                thisclient->CharInfo->Exp +=  thisclient->bonusxp*GetColorExp( thisclient->Stats->Level, thismon->thisnpc->level + special_lvl, exp );
            }

            //Log(MSG_INFO,"Bonus XP %i, previous XP, %i, new: %i",thisclient->bonusxp,prev_xp,thisclient->CharInfo->Exp);
            //LMA END

            //LMA: We don't send exp packet if there is a level up coming up next.
            if(thisclient->CharInfo->Exp<thisclient->GetLevelEXP())
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

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
    for(UINT i=0;i<thismon->PlayersDamage.size();i++)
    {
        MonsterDamage* thisplayer = thismon->PlayersDamage.at(i);
		CPlayer* thisclient = GetClientByCID( thisplayer->charid, thismon->Position->Map );
		if( thisplayer->damage > 0 && thisclient != NULL ) //player di some damage
        {
    		//Log(MSG_DEBUG,"Player did %i damage max = %i",thisplayer->damage,thismon->Stats->MaxHP);
    		float MyPercent = (float)thisplayer->damage / thismon->Stats->MaxHP;
    		if(MyPercent > 1.5)MyPercent = 1.5;   //set overkill ceiling
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
            exp = exp * Config.EXP_RATE * tmpMult;      //calculate base exp for this client. No medals or stuff accounted for yet
            //Log(MSG_DEBUG,"MaxXP: %i config rate: %i percent: %f Total XP: %i", thismon->thisnpc->exp, Config.EXP_RATE, MyPercent, exp);
            thisclient->CharInfo->Pending_Exp += (exp * thisclient->Stats->xprate);    //store exp into thisclient's pending_exp using personal xprate adjustments

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

                /*
                bool pflag = false;

                for(int j=0;j<PartyExp.size();j++)
                {
                    CPartyExp* thisparty = PartyExp.at( j );
                    if( thisparty->thisparty == thisclient->Party->party )
                    {
                        thisparty->exp += exp;
                        thisparty->exp += exp * ((thisclient->Party->party->PartyLevel*2) / 100);
                        pflag = true;
                    }
                }
                if( !pflag )
                {
                    CPartyExp* thisparty = new CPartyExp;
                    thisparty->thisparty = thisclient->Party->party;
                    thisparty->exp = exp;
                    thisparty->flag = false;
                    thisparty->exp += exp * ((thisclient->Party->party->PartyLevel*2) / 100);
                    thisparty->num = 1;
                    thisparty->partymember[0] = thisclient->CharInfo->charid;
                    thisparty->maxlevel = thisclient->Stats->Level;
                    for(int p=0;p<thisclient->VisiblePlayers.size();p++)
                    {
                        CPlayer* otherclient = thisclient->VisiblePlayers.at( p );
                        if(otherclient->Party->party==NULL) continue;
                        if( thisclient->Party->party == otherclient->Party->party );
                        {
                            thisparty->partymember[thisparty->num] = otherclient->CharInfo->charid;
                            thisparty->num++;
                            thisparty->maxlevel += otherclient->Stats->Level;
                        }
                    }
                    PartyExp.push_back( thisparty );
                }
                continue;
                */
            }
            else //not in a party so deal with all the exp now
            {
                //SendPM(thisclient, "You receive %i EXP",thisclient->CharInfo->Pending_Exp);
                thisclient->CharInfo->Exp += thisclient->CharInfo->Pending_Exp;
                thisclient->CharInfo->Pending_Exp = 0;
                if(!thisclient->CheckPlayerLevelUP())
                {
                    BEGINPACKET( pak, 0x79b );
                    ADDDWORD   ( pak, thisclient->CharInfo->Exp );
                    ADDWORD    ( pak, thisclient->CharInfo->stamina );
                    ADDWORD    ( pak, 0 );
                    thisclient->client->SendPacket( &pak );
                }
            }
            //unsigned int thisexp = GetColorExp( thisclient->Stats->Level, thismon->thisnpc->level, exp ) * thisclient->Stats->xprate;
            //Log(MSG_DEBUG,"Player rewarded %i experience after all mods",thisexp);
            //thisclient->CharInfo->Exp +=  GetColorExp( thisclient->Stats->Level, thismon->thisnpc->level, exp ) * thisclient->Stats->xprate;
        }
    }

    //new way PY
    //Log(MSG_DEBUG,"Parsing party list now");
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
                ADDWORD    ( pak, 0 );
                thismember->client->SendPacket( &pak );
            }

        }
        thisparty->Exp += thisparty->Pending_Exp;
        thisparty->Pending_Exp = 0;
        if( thisparty->Exp > GetMaxPartyExp(thisparty->PartyLevel)) //level up the party
        {
            thisparty->PartyLevel++;
            thisparty->Exp -= GetMaxPartyExp(thisparty->PartyLevel-1);
        }
        BEGINPACKET( pak, 0x7d4 );
        ADDBYTE    ( pak, thisparty->PartyLevel );
        ADDDWORD   ( pak, thisparty->Exp );
        thisparty->SendToMembers( &pak );

    }

    //old way
    /*
    for(int p=0;p<PartyExp.size();p++)
    {
        CPartyExp* thisparty = PartyExp.at( p );
        for(int i=0;i<thisparty->num;i++)
        {
            CPlayer* partyclient = GetClientByCID( thisparty->partymember[i], thismon->Position->Map );
            if( partyclient == NULL )
                continue;
            if(partyclient->Party->party==NULL)
                continue;
            if(!thisparty->flag)
            {
                partyclient->Party->party->Exp += (thisparty->exp / 6) / 2;
                if( partyclient->Party->party->PartyLevel == 60)
                {
                    partyclient->Party->party->Exp = 0;
                }
                if( partyclient->Party->party->Exp > GetMaxPartyExp( partyclient->Party->party->PartyLevel ) )
                {
                    partyclient->Party->party->PartyLevel++;
                    partyclient->Party->party->Exp -= GetMaxPartyExp( partyclient->Party->party->PartyLevel-1 );
                }
                BEGINPACKET( pak, 0x7d4 );
                ADDBYTE    ( pak, partyclient->Party->party->PartyLevel );
                ADDDWORD   ( pak, partyclient->Party->party->Exp );
                partyclient->Party->party->SendToMembers( &pak );
                thisparty->flag = true;
            }
            unsigned int expoption = partyclient->Party->party->Option%0x80;
            if( expoption==0 )
            {

                partyclient->CharInfo->Exp +=  GetColorExp( partyclient->Stats->Level, thismon->Stats->Level, (UINT)round(thisparty->exp / thisparty->num) ) * partyclient->Stats->xprate;
            }
            else
            {
                partyclient->CharInfo->Exp +=  GetColorExp( partyclient->Stats->Level, thismon->Stats->Level, (UINT)round(partyclient->Stats->Level * thisparty->exp / thisparty->maxlevel) ) * partyclient->Stats->xprate;
            }
            if(!partyclient->CheckPlayerLevelUP( )){
    		BEGINPACKET( pak, 0x79b );
    		ADDDWORD   ( pak, partyclient->CharInfo->Exp );
    		ADDWORD    ( pak, partyclient->CharInfo->stamina );
    		ADDWORD    ( pak, 0 );
    		partyclient->client->SendPacket( &pak );}
        }
    }
    */
    //MapList.Index[thismon->Position->Map]->DeleteMonster( thismon );
    return true;
}

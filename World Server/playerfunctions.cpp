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
#include "player.h"
#include "worldserver.h"

// Returns the amount of exp that is needed for the next level
UINT CPlayer::GetLevelEXP( )
{
	if (Stats->Level <= 15)       return (unsigned int)( ( Stats->Level + 10 )  * ( Stats->Level + 5 )  * ( Stats->Level + 3 ) * 0.7 );
	else if (Stats->Level <= 50)  return (unsigned int)( ( Stats->Level - 5 )   * ( Stats->Level + 2 )  * ( Stats->Level + 2 ) * 2.2 );
	else if (Stats->Level <= 100) return (unsigned int)( ( Stats->Level - 38 )  * ( Stats->Level - 5 )  * ( Stats->Level + 2 ) * 9 );
	else if (Stats->Level <= 139) return (unsigned int)( ( Stats->Level + 220 ) * ( Stats->Level + 34 ) * ( Stats->Level + 27 ) );
	else                     return (unsigned int)( ( Stats->Level - 126 ) * ( Stats->Level - 15 ) * ( Stats->Level + 7 ) * 41 );
}

// check if player can level up
//LMA: Old version
/*
bool CPlayer::CheckPlayerLevelUP( )
{
	if (CharInfo->Exp >= GetLevelEXP())
    {

        //LMA: TEST:
        Log(MSG_INFO,"%s should level up (%I64i >= %u), level: %u",CharInfo->charname,CharInfo->Exp,GetLevelEXP(),Stats->Level);

	    CharInfo->Exp -= GetLevelEXP();
	    Stats->Level++;
	    Stats->HP = GetMaxHP( );
	    Stats->MP = GetMaxHP( );
	    CharInfo->StatPoints += 10 + (Stats->Level/2);
	    if(Stats->Level>=10)
	    {
	        CharInfo->SkillPoints += 1;
	    }

        //LMA: TEST
	    Log(MSG_INFO,"new exp %I64i, new level: %u",CharInfo->Exp,Stats->Level);

		BEGINPACKET( pak, 0x79e );
		ADDWORD( pak, clientid );
		ADDWORD( pak, Stats->Level );
		ADDDWORD( pak, CharInfo->Exp );
		ADDWORD( pak, CharInfo->StatPoints );
		ADDWORD( pak, CharInfo->SkillPoints );
		client->SendPacket( &pak );

		RESETPACKET( pak, 0x79e );
		ADDWORD( pak, clientid );
		GServer->SendToVisible( &pak, this );
		SetStats( );
            //SendLevelUPtoChar(this);
	}
	return true;
}
*/

//LMA: New version (check if player can level up):
bool CPlayer::CheckPlayerLevelUP( )
{
    //No lvl up needed.
    if (CharInfo->Exp<GetLevelEXP())
    {
        return true;
    }

	while (CharInfo->Exp >= GetLevelEXP())
    {
        //LMA: TEST:
        //Log(MSG_INFO,"%s should level up (%I64i >= %u), level: %u",CharInfo->charname,CharInfo->Exp,GetLevelEXP(),Stats->Level);

	    CharInfo->Exp -= GetLevelEXP();
	    Stats->Level++;
	    Stats->HP = GetMaxHP( );
	    Stats->MP = GetMaxHP( );

	    //LMA: We change the formula due to Rescudo (adapted):
	    //CharInfo->StatPoints += 10 + (Stats->Level/2);
        CharInfo->StatPoints +=9;
        CharInfo->StatPoints += Stats->Level - ((Stats->Level - 1) / 5);

	    if(Stats->Level>=10)
	    {
	        CharInfo->SkillPoints += 1;
	    }

        //LMA: TEST
	    //Log(MSG_INFO,"new exp %I64i, new level: %u",CharInfo->Exp,Stats->Level);
	}

	//sending the packet now :)
    BEGINPACKET( pak, 0x79e );
    ADDWORD( pak, clientid );
    ADDWORD( pak, Stats->Level );
    ADDDWORD( pak, CharInfo->Exp );
    ADDWORD( pak, CharInfo->StatPoints );
    ADDWORD( pak, CharInfo->SkillPoints );
    client->SendPacket( &pak );

    RESETPACKET( pak, 0x79e );
    ADDWORD( pak, clientid );
    GServer->SendToVisible( &pak, this );
    SetStats( );
    //SendLevelUPtoChar(this);
    //Log(MSG_INFO,"Def exp %I64i, def level: %u",CharInfo->Exp,Stats->Level);


	return true;
}


// Send a PM to client with user information
bool CPlayer::GetPlayerInfo( )
{
    char text[50];
    sprintf(text,"Attack: %i | Critical: %i",Stats->Attack_Power, Stats->Critical );
	BEGINPACKET( pak, 0x0784 );
	ADDSTRING( pak, "[GM]PlayerInfo" );
	ADDBYTE( pak, 0 );
	ADDSTRING( pak, text );
	ADDBYTE( pak, 0 );
	client->SendPacket(&pak);
    sprintf(text,"Defense: %i | Magic Defense: %i",Stats->Defense, Stats->Magic_Defense);
	RESETPACKET( pak, 0x0784 );
	ADDSTRING( pak, "[GM]PlayerInfo" );
	ADDBYTE( pak, 0 );
	ADDSTRING( pak, text );
	ADDBYTE( pak, 0 );
	client->SendPacket(&pak);
    sprintf(text,"Accury: %i | Dodge: %i",Stats->Accury,Stats->Dodge );
	RESETPACKET( pak, 0x0784 );
	ADDSTRING( pak, "[GM]PlayerInfo" );
	ADDBYTE( pak, 0 );
	ADDSTRING( pak, text );
	ADDBYTE( pak, 0 );
	client->SendPacket(&pak);
    sprintf(text,"aspeed: %i | mspeed: %i",Stats->Attack_Speed,Stats->Move_Speed );
	RESETPACKET( pak, 0x0784 );
	ADDSTRING( pak, "[GM]PlayerInfo" );
	ADDBYTE( pak, 0 );
	ADDSTRING( pak, text );
	ADDBYTE( pak, 0 );
	client->SendPacket(&pak);
    sprintf(text,"HP: %I64i/%I64i | MP: %I64i/%I64i",Stats->HP,Stats->MaxHP,Stats->MP,Stats->MaxMP);
	RESETPACKET( pak, 0x0784 );
	ADDSTRING( pak, "[GM]PlayerInfo" );
	ADDBYTE( pak, 0 );
	ADDSTRING( pak, text );
	ADDBYTE( pak, 0 );
	client->SendPacket(&pak);
    sprintf(text,"Position[%i]: (%.0f,%.0f)",Position->Map,Position->current.x,Position->current.y);
	RESETPACKET( pak, 0x0784 );
	ADDSTRING( pak, "[GM]PlayerInfo" );
	ADDBYTE( pak, 0 );
	ADDSTRING( pak, text );
	ADDBYTE( pak, 0 );
	client->SendPacket(&pak);
    sprintf(text,"ClientID: %u | CharID: %u", clientid, CharInfo->charid );
	RESETPACKET( pak, 0x0784 );
	ADDSTRING( pak, "[GM]PlayerInfo" );
	ADDBYTE( pak, 0 );
	ADDSTRING( pak, text );
	ADDBYTE( pak, 0 );
	client->SendPacket(&pak);
    sprintf(text,"inGame: %i | Logged: %i", Session->inGame, Session->isLoggedIn );
	RESETPACKET( pak, 0x0784 );
	ADDSTRING( pak, "[GM]PlayerInfo" );
	ADDBYTE( pak, 0 );
	ADDSTRING( pak, text );
	ADDBYTE( pak, 0 );
	client->SendPacket(&pak);
    sprintf(text,"ClanName[%u]: %s | ClanGrade: %i | ClanRank: %i", Clan->clanid, Clan->clanname, Clan->grade, Clan->clanrank );
	RESETPACKET( pak, 0x0784 );
	ADDSTRING( pak, "[GM]PlayerInfo" );
	ADDBYTE( pak, 0 );
	ADDSTRING( pak, text );
	ADDBYTE( pak, 0 );
	client->SendPacket(&pak);
	sprintf(text,"Extra DMG: %i", Stats->ExtraDamage_add );
	RESETPACKET( pak, 0x0784 );
	ADDSTRING( pak, "[GM]PlayerInfo" );
	ADDBYTE( pak, 0 );
	ADDSTRING( pak, text );
	ADDBYTE( pak, 0 );
	client->SendPacket(&pak);
    return true;
}

// clearn player lists
bool CPlayer::CleanPlayerVector( )
{
    CMap* map = GServer->MapList.Index[Position->Map];
    VisiblePlayers.clear();
	VisibleMonsters.clear();
	VisibleDrops.clear();
	VisibleNPCs.clear();
	return true;
}

//LMA BEGIN
//20070621-211100/outdated?
//we clear and "regenerate" a monster...
bool CPlayer::ForceRefreshMonster(bool refresh_all, UINT monster_id)
{
     CMap* map = GServer->MapList.Index[Position->Map];

    // Monsters
    for(UINT i=0;i<map->MonsterList.size();i++)
    {
        CMonster* thismon = map->MonsterList.at( i );
          if (thismon==NULL)
          {
             return false;
          }

          if (!refresh_all && (thismon->clientid!=monster_id))
          {
              continue;
          }

        //here we are only interested to respawn guys who kick our ass being invisible :)
		float distance = GServer->distance ( this->Position->current, thismon->Position->current );
		if ( GServer->IsVisible( this, thismon ) )
        {

			if (distance < MAXVISUALRANGE )
			{
                //we clear it
                ClearObject( thismon->clientid );
                //and respawn it
                thismon->SpawnMonster(this, thismon );
                //the visibility list will be updated in time automatically.
                //if the monster kicks you, it's because it's already too near ;)

              if (!refresh_all)
              {
                  return true;
              }

            }

		}

	}


	return true;
}
//LMA END

// update visibility list
bool CPlayer::VisiblityList( )
{
	std::vector<CPlayer*>	newVisiblePlayers;
	std::vector<CDrop*>			newVisibleDrops;

	//LMATEST
	//std::vector<CMonster*>		newVisibleMonsters;
	std::vector<unsigned int>        newVisibleMonsters;



	std::vector<CNPC*>			newVisibleNPCs;
	// Clients
	CMap* map = GServer->MapList.Index[Position->Map];
    for(UINT i=0;i<map->PlayerList.size();i++)
    {
        CPlayer* otherclient = map->PlayerList.at(i);
		if ( this==otherclient || !otherclient->Session->inGame)
		{
            continue;
        }
		float distance = GServer->distance( this->Position->current, otherclient->Position->current );
		if ( GServer->IsVisible( this, otherclient ) )
        {
            //LMA: now invisible players are just usual users.
			//if ( distance < MAXVISUALRANGE && !otherclient->isInvisibleMode )
			if ( distance < MAXVISUALRANGE)
			{
				newVisiblePlayers.push_back( otherclient );
            }
			else
			{
				ClearObject( otherclient->clientid );
            }
		}
        else
        {
            //LMA: now invisible players are just usual users.
			//if ( distance < MINVISUALRANGE && !otherclient->isInvisibleMode )
			if ( distance < MINVISUALRANGE)
            {
			    newVisiblePlayers.push_back( otherclient );
				otherclient->SpawnToPlayer(this, otherclient);
			}
        }
	}
    // Monsters
    bool monster_seen=false;
    int bon_sp_mp=0;
    int bon_sp_hp=0;
    int bon_nb_mp=0;
    int bon_nb_hp=0;

    for(UINT i=0;i<map->MonsterList.size();i++)
    {
        CMonster* thismon = map->MonsterList.at( i );
		float distance = GServer->distance ( this->Position->current, thismon->Position->current );
		monster_seen=false;
		if ( GServer->IsVisible( this, thismon ) )
        {
			if (distance < MAXVISUALRANGE )
			{
                //LMATEST
                //newVisibleMonsters.push_back( thismon );
                newVisibleMonsters.push_back( thismon->clientid );
                monster_seen=true;
            }
			else
			{
				ClearObject( thismon->clientid );
            }
		}
        else
        {
			if ( distance< MINVISUALRANGE )
            {
                 //LMATEST
				//newVisibleMonsters.push_back( thismon );
				newVisibleMonsters.push_back( thismon->clientid );
				thismon->SpawnMonster(this, thismon );
				monster_seen=true;
            }
		}

		//LMA: bonfire, salamender handle (all done by AIP now)
		/*
		if((monster_seen)&&(thismon->IsBonfire())&&(distance<=thismon->range))
		{
            if(thismon->bonusmp>0)
                bon_sp_mp+=GServer->RandNumber(thismon->minvalue,thismon->maxvalue);
            if(thismon->bonushp>0)
                bon_sp_hp+=GServer->RandNumber(thismon->minvalue,thismon->maxvalue);
            bon_nb_mp+=thismon->bonusmp;
            bon_nb_hp+=thismon->bonushp;
        }
        */

	}

	//LMA: bonus from bonfires / salamender.  (all done by AIP now)
	//One update so we divide if several bonuses
	//regen will be updated faster, that's all.
    nb_mp=bon_nb_mp;
    nb_hp=bon_nb_hp;
	sp_hp=bon_sp_hp;
    sp_mp=bon_sp_mp;
    //Log(MSG_INFO,"SIP %i,%i,%i,%i",nb_mp,nb_hp,sp_hp,sp_mp);


	// Drops
	for(unsigned i=0; i<map->DropsList.size(); i++)
    {
		CDrop* thisdrop = map->DropsList.at(i);
		float distance = GServer->distance( this->Position->current, thisdrop->pos );
		if ( GServer->IsVisible( this, thisdrop ) )
        {
			if ( distance< MAXVISUALRANGE )
			{
				newVisibleDrops.push_back( thisdrop );
            }
			else
			{
				this->ClearObject( thisdrop->clientid );
            }
		}
        else
        {
			if ( distance < MINVISUALRANGE )
            {
				newVisibleDrops.push_back( thisdrop );
				GServer->pakSpawnDrop( this, thisdrop );
			}
		}
	}
	// Npcs
	for(unsigned i=0; i<map->NPCList.size(); i++)
    {
		CNPC* thisnpc = map->NPCList.at(i);
		float distance = GServer->distance( this->Position->current, thisnpc->pos );
		if ( GServer->IsVisible( this, thisnpc ) )
        {
			if ( distance < MAXVISUALRANGE )
            {
				newVisibleNPCs.push_back( thisnpc );
            }
			else
			{
        		this->ClearObject( thisnpc->clientid );
            }
		}
        else
        {
			if ( distance < MINVISUALRANGE )
            {
				newVisibleNPCs.push_back( thisnpc );

				//LMA: WarpGate.
				if(thisnpc->npctype>10000&&(thisnpc->npctype==GServer->WarpGate.virtualNpctypeID))
				{
				    GServer->pakSpawnIfoObject( this, GServer->WarpGate.virtualNpctypeID );
				}
				else
				{
				    GServer->pakSpawnNPC( this, thisnpc );
				}

			}
		}
	}
	VisiblePlayers.clear();
	VisibleDrops.clear();
	VisibleMonsters.clear();
	VisibleNPCs.clear();
	VisiblePlayers = newVisiblePlayers;
	VisibleDrops = newVisibleDrops;
	VisibleMonsters = newVisibleMonsters;
	VisibleNPCs = newVisibleNPCs;
    return true;

}

// Returns a free slot in the inventory (0xffff if is full)
UINT CPlayer::GetNewItemSlot( CItem thisitem )
{
    UINT tabsize = 30;
    UINT itemtab = 0;
    switch(thisitem.itemtype)
    {
        case 1:case 2:case 3:case 4:case 5:case 6:case 7:case 8:case 9://equip
            itemtab=0;
        break;
        case 10://consumibles
            itemtab=1;
        break;
        case 11:case 12://etc
            itemtab=2;
        break;
        case 14://pat
            itemtab=3;
        break;
        default:
            Log(MSG_WARNING,"unknown itemtype %i", thisitem.itemtype);
            return 0xffff;
         break;
    }
    //useitems and natural
    //first we try to find a slot that already contains some of this item
    //this way the items will always stack if possible.
    if(itemtab == 1 || itemtab == 2)
    {
        for(int i=0;i<30;i++)
        {
            UINT slot=12;
            slot += (tabsize*itemtab)+i;
            if(items[slot].itemnum == thisitem.itemnum && items[slot].itemtype == thisitem.itemtype)
            {
                int totcount=thisitem.count + items[slot].count;
                if(totcount<999)
                    return slot;
            }
        }
    }
    //now scan again to find a fully empty slot for mats, useitems and equipable stuff
    //doesn't matter what tab type
    for(int i=0;i<30;i++)
    {
        UINT slot=12;
        slot += (tabsize*itemtab)+i;
		//LMA: Bug?
        //if(items[slot].itemnum==0 &items[slot].count<1)
		if(items[slot].itemnum==0&&items[slot].count<1)
            return slot;
    }
    return 0xffff;
}

// Returns a free slot in the storage (0xffff if is full)
UINT CPlayer::GetNewStorageItemSlot( CItem thisitem )
{
    //first check for a slot that already contains this item if it is stackable
    if(thisitem.itemtype == 10 || thisitem.itemtype == 12)
        {
        for(UINT i=0;i<160;i++)
        {
            if(storageitems[i].itemtype == thisitem.itemtype && storageitems[i].itemnum == thisitem.itemnum)
            {
                //now we make sure that the total of the new items plus the ones already there do not exceed 999
                int totalcount = storageitems[i].count + thisitem.count;
                if(totalcount < 999)
                    return i;
            }
        }
    }
    //so we couldn't find a matching entry to stack onto
    //or the item is unstackable
    //Let's look for an empty slot instead then
    for(UINT i=0;i<160;i++)
    {
        if(storageitems[i].itemtype == 0)
            return i;
    }
    return 0xffff;
}


// Required skill check by insider
UINT CPlayer::GetPlayerSkill( unsigned int id )
{
    //for(UINT i=0;i<MAX_SKILL;i++)
    for(UINT i=0;i<MAX_ALL_SKILL;i++)
    {
        if (cskills[i].id == id)
            return i;
    }

    return 0xffff;
}

// Erase a object from the user
bool CPlayer::ClearObject( unsigned int otherclientid )
{
	BEGINPACKET( pak, 0x794 );
	ADDWORD( pak, otherclientid );
	client->SendPacket( &pak );
	return true;
}

// Clean the player values
void CPlayer::RestartPlayerVal( )
{
    ClearBattle( Battle );
    Shop->open = false;
    Trade->trade_target = 0;
    Trade->trade_status = 0;
}

//LMA
//Force refresh of actual HP / MP Hp/Mp jumping bug.
bool CPlayer::RefreshHPMP()
{
    clock_t etime = clock() - lastShowTime;
    if ( etime < CLOCKS_PER_SEC )
        return true;

    BEGINPACKET( pak, 0x7ec );
    ADDWORD    ( pak, Stats->HP );
    ADDWORD    ( pak, Stats->MP );
    client->SendPacket ( &pak );

    if (Party->party != NULL)
    {
        BEGINPACKET( pak, 0x7d5 );
        ADDDWORD   ( pak, CharInfo->charid );
        ADDWORD    ( pak, clientid );
        ADDWORD    ( pak, GetMaxHP( ) );
        ADDWORD    ( pak, Stats->HP );
        //ADDDWORD   ( pak, 0x01000000 );//Tomiz: Was not commented before
        ADDDWORD   ( pak, GServer->BuildBuffs( this ));//Tomiz: Buff Data
        //ADDDWORD   ( pak, 0x0000000f );//Tomiz: Was not commented before
        ADDDWORD   ( pak, 0x1f40008c );//Tomiz
        ADDWORD    ( pak, 0x1388 );
        Party->party->SendToMembers( &pak);
    }

    lastShowTime = clock();
    return true;
}


// HP/MP Regeneration Function
/*bool CPlayer::Regeneration()
{
    //LMA: New version, takes bonfire, salamender, sit, fairy in account.
    if (Stats->MaxHP==Stats->HP)
       lastRegenTime_hp=0;

    if (Stats->MaxMP==Stats->MP)
       lastRegenTime_mp=0;

    if ((Stats->MaxHP==Stats->HP)&&(Stats->MaxMP==Stats->MP))
       return true;

    //LMA: HP
    float nb_sec_stance=0;  //LMA: on your feet soldier.
    UINT bonus_sitted=0;
    int bonus_hp=0;
    int bonus_mp=0;

    if (Stats->HP<Stats->MaxHP)
    {
        nb_sec_stance=(float) 8;  //LMA: on your feet soldier.
        bonus_sitted=1;

        //LMA: added Planetary_Myth check, should be a bit faster and a bit cleaner :)
        if(nb_hp>0 || Status->Stance==1 || Fairy)
        {
            nb_sec_stance=(float) 3;
        }

    	clock_t etime = clock() - lastRegenTime_hp;
        if( etime >= nb_sec_stance * CLOCKS_PER_SEC && Stats->HP > 0 )
        {
            if (Status->Stance==1)
            {
               bonus_hp++;
               //bonus_sitted=GServer->RandNumber(150,250);
               bonus_sitted=GServer->RandNumber(150,100);
            }

            if(Fairy)
            {
               bonus_hp++;
               //bonus_sitted+=GServer->RandNumber(150,250);
               bonus_sitted+=GServer->RandNumber(150,100);
            }

            bonus_hp+=nb_hp;
            if(bonus_hp!=0)
               nb_sec_stance=nb_sec_stance/((float)bonus_hp);    //LMA: salamender-bonfire?
            //Log(MSG_INFO,"HP info: sp_hp=%i, nb_hp=%i, bonus=%i, nb_bonus=%i, nb_sec=%.2f",sp_hp,nb_hp,bonus_sitted,bonus_hp,nb_sec_stance);

            //Log(MSG_INFO,"SIP2 %i,%i,%i,%i",nb_mp,nb_hp,sp_hp,sp_mp);

            unsigned int hpamount = GetHPRegenAmount( );

            if (bonus_hp!=0)
            {
               Stats->HP += (long int) (hpamount*(sp_hp+bonus_sitted)/(100*bonus_hp));           //bonfire and so on...
               //Log(MSG_INFO,"REGEN HP %i(%i*(%i+%i)/(100*%i)",(long int) (hpamount*(sp_hp+bonus_sitted)/(100*bonus_hp)),hpamount,sp_hp,bonus_sitted,bonus_hp);
            }
            else
                Stats->HP += hpamount;

            if( Stats->HP > Stats->MaxHP)
                Stats->HP = Stats->MaxHP;

            if (Stats->MaxHP==Stats->HP)
               lastRegenTime_hp=0;
            else
                lastRegenTime_hp = clock();
        }

    }

    //LMA: MP
    if(Stats->MP<Stats->MaxMP)
    {
        nb_sec_stance=(float) 8;  //LMA: on your feet soldier.
        bonus_sitted=1;
        bonus_mp=0;

        if(nb_mp>0)
           nb_sec_stance=(float) 3;

        if (Status->Stance==1)
        {
           bonus_mp++;
           //bonus_sitted=GServer->RandNumber(150,250);
           bonus_sitted=GServer->RandNumber(150,100);
           nb_sec_stance=(float) 3;    //LMA: be sitted.
        }
        if(Fairy)
        {
           bonus_mp++;
           nb_sec_stance=(float) 3;
           //bonus_sitted+=GServer->RandNumber(150,250);
           bonus_sitted+=GServer->RandNumber(150,100);
        }
        bonus_mp+=nb_mp;
        if(bonus_mp!=0)
           nb_sec_stance=nb_sec_stance/((float)bonus_mp);    //LMA: salamender-bonfire?
        //Log(MSG_INFO,"MP info: sp_mp=%i, nb_mp=%i, bonus=%i, nb_bonus=%i, nb_sec=%.2f",sp_mp,nb_mp,bonus_sitted,bonus_mp,nb_sec_stance);
    	clock_t etime = clock() - lastRegenTime_mp;
        if( etime >= nb_sec_stance * CLOCKS_PER_SEC && Stats->HP > 0 )
        {
            unsigned int mpamount = GetMPRegenAmount( );

            if (bonus_mp!=0)
            {
               Stats->MP += (long int) (mpamount*(sp_mp+bonus_sitted)/(100*bonus_mp));           //bonfire and so on...
               //Log(MSG_INFO,"REGEN MP %i (%i*(%i+%i)/(100*%i)",(long int) (mpamount*(sp_mp+bonus_sitted)/(100*bonus_mp)),mpamount,sp_mp,bonus_sitted,bonus_mp);
            }
            else
                Stats->MP += mpamount;

            if( Stats->MP > Stats->MaxMP )
                Stats->MP = Stats->MaxMP;

            if (Stats->MaxMP== Stats->MP)
               lastRegenTime_mp=0;
            else
                lastRegenTime_mp = clock();
        }

    }


    return true;
}
*/

//LMA: new version from Planetary_Myth
// HP/MP Regeneration Function
bool CPlayer::Regeneration()
{
    if (Stats->MaxHP==Stats->HP)
       lastRegenTime_hp = clock();

    if (Stats->MaxMP==Stats->MP)
      lastRegenTime_mp = clock();

    if ((Stats->MaxHP==Stats->HP)&&(Stats->MaxMP==Stats->MP))
       return true;

     UINT bonus_sitted=1;
     UINT bonus_fairy=1;
     int bonus_hp=0;
     int bonus_mp=0;
     int nb_sec_stance=8;   //Original setting was 5

     if(Fairy)
     {
         nb_sec_stance= 8;      //Original setting was 3
         bonus_mp++;
         bonus_hp++;
         bonus_fairy=3;
     }
     if (Status->Stance==1)
     {
         nb_sec_stance= 5;      //Original setting was 3
         bonus_mp++;
         bonus_hp++;
         bonus_sitted=3;
     }

     int TimeDiff=nb_sec_stance*CLOCKS_PER_SEC;

    //LMA: HP
    if (Stats->HP<Stats->MaxHP)
    {
        clock_t etimeHP = clock() - lastRegenTime_hp;

        if( etimeHP >= TimeDiff && Stats->HP > 0 )
        {
            unsigned int hpamount = GetHPRegenAmount( );

            if (bonus_hp!=0)
            {
               Stats->HP += (long int) (hpamount*bonus_sitted)*bonus_fairy;
               //Log(MSG_INFO,"REGEN HP %i(%i*%i)*%i",(long int) (hpamount*bonus_sitted)*bonus_fairy,hpamount,bonus_sitted,bonus_fairy);
            }
            else
            {
                Stats->HP += hpamount;
            }
            if( Stats->HP > Stats->MaxHP)
                Stats->HP = Stats->MaxHP;

            if (Stats->HP < Stats->MaxHP)
                lastRegenTime_hp = clock();
        }
    }

    //LMA: MP
    if(Stats->MP<Stats->MaxMP)
    {
        clock_t etimeMP = clock() - lastRegenTime_mp;

        if( etimeMP >= TimeDiff && Stats->HP > 0 )
        {
            unsigned int mpamount = GetMPRegenAmount( );

            if (bonus_mp!=0)
            {
               Stats->MP += (long int) (mpamount * bonus_sitted)* bonus_fairy;
               //Log(MSG_INFO,"RegenMP %i(%i*%i)*%i",(long int) (mpamount*bonus_sitted)*bonus_fairy,mpamount,bonus_sitted,bonus_fairy);
            }
            else
            {
                Stats->MP += mpamount;
            }
            if( Stats->MP > Stats->MaxMP )
                Stats->MP = Stats->MaxMP;

            if (Stats->HP < Stats->MaxHP)
                lastRegenTime_mp = clock();
        }
    }
    return true;
}


// Heal Player when use Food/Pots
bool CPlayer::PlayerHeal()
{
    clock_t transtime = clock() - UsedItem->lastRegTime;
	if( UsedItem->usevalue!=0 && transtime >= 0.3*CLOCKS_PER_SEC )
    {
        if( UsedItem->used < UsedItem->usevalue && Stats->HP > 0 )
        {
            int value = UsedItem->userate;
            if((UsedItem->usevalue - UsedItem->used) < value)
            {
                value = UsedItem->usevalue - UsedItem->used;
            }
            switch( UsedItem->usetype )
            {
                case 16: // HP
                    Stats->HP += value;
                    if(Stats->HP > GetMaxHP())
                        Stats->HP = GetMaxHP();
                break;
                case 17: // MP
                    Stats->MP += value;
                    if(Stats->MP > GetMaxMP())
                        Stats->MP = GetMaxMP();
                break;
            }
            UsedItem->used += value;
            UsedItem->lastRegTime = clock();
        }
        else
        {
            BEGINPACKET( pak,0x7b7 );
            ADDWORD    ( pak, clientid );
            ADDDWORD   ( pak, GServer->BuildBuffs( this ) );
            switch( UsedItem->usetype )
            {
                case 16: // HP
                    ADDWORD( pak, Stats->HP );
                break;
                case 17: // MP
                    ADDWORD( pak, Stats->MP );
                break;
            }
            GServer->SendToVisible( &pak, this );
            UsedItem->used = 0;
            UsedItem->usevalue = 0;
            UsedItem->userate = 0;
            UsedItem->usetype = 0;
        }
    }
    return true;
}

//Reduce the number of arrows, bullets...
void CPlayer::ReduceABC( int amount, bool do_packet )
{
    unsigned int weapontype = 0;
    weapontype = GServer->EquipList[WEAPON].Index[items[7].itemnum]->type;
    int slot=0;
    int reload=0;
    //printf("Reducing ABC item count /n");

    switch(weapontype)
    {
        case BOW:
            if (items[132].count==0)
                return;
            items[132].count--;
            slot=132;
            //Log(MSG_INFO,"%u Arrows remain.",items[132].count);
            if(items[132].count<=0)
            {
                reload=SearchABC(slot);
                //ClearBattle( Battle );
                ClearItem( items[132] );
            }
        break;
        case GUN:
            if (items[133].count==0)
                return;
            items[133].count--;
            slot=133;
            //Log(MSG_INFO,"%u Bullets remain.",items[133].count);
            if(items[133].count<=0)
            {
                reload=SearchABC(slot);
                //ClearBattle( Battle );
                ClearItem( items[133] );
            }
        break;
        case LAUNCHER:
            if (items[134].count==0)
                return;
            items[134].count--;
            slot=134;
            //Log(MSG_INFO,"%u Cannons remain.",items[134].count);
            if(items[134].count<=0)
            {
                reload=SearchABC(slot);
                //ClearBattle( Battle );
                ClearItem( items[134] );
            }
        break;
        case CROSSBOW:
            if (items[132].count==0)
                return;
            items[132].count--;
            slot=132;
            //Log(MSG_INFO,"%u Arrows remain.",items[132].count);
            if(items[132].count<=0)
            {
                reload=SearchABC(slot);
                //ClearBattle( Battle );
                ClearItem( items[132] );
            }
        break;
    }

    if(slot>0)
    {
    	 if(do_packet||items[slot].count==0)
    	 	{
	        BEGINPACKET( pak, 0x718 );
	        ADDBYTE( pak, 1 );
	        ADDBYTE    ( pak, slot);
	        ADDDWORD   ( pak, GServer->BuildItemHead( items[slot] ) );
	        ADDDWORD   ( pak, GServer->BuildItemData( items[slot] ) );
	        ADDDWORD( pak, 0x00000000 );
	        ADDWORD ( pak, 0x0000 );
	        client->SendPacket( &pak );

    	 		if(items[slot].count==0)
  	 			{
                    BEGINPACKET( pak, 0x7ab );
                    ADDWORD( pak, clientid );
                    ADDWORD ( pak, 0x0000 );
                    client->SendPacket( &pak );

                    if(reload!=0)
                    {
                       //let's try to reload.
                       items[slot].itemnum=items[reload].itemnum;
                       items[slot].itemtype=items[reload].itemtype;
                       items[slot].count=items[reload].count;
                       items[slot].appraised=items[reload].appraised;
                       items[slot].durability=items[reload].durability;
                       items[slot].gem=items[reload].gem;
                       items[slot].last_sp_value=items[reload].last_sp_value;
                       items[slot].lifespan=items[reload].lifespan;
                       items[slot].refine=items[reload].refine;
                       items[slot].socketed=items[reload].socketed;
                       items[slot].sp_value=items[reload].sp_value;
                       items[slot].stats=items[reload].stats;
                       ClearItem(items[reload]);

                       UpdateInventory(slot,reload);

                        BEGINPACKET( pak, 0x7ab );
                        ADDWORD    ( pak, clientid );
                        unsigned int effect = items[slot].itemnum * 32 ;
                        ADDWORD    ( pak, effect );
                        GServer->SendToVisible( &pak, this );
                    }
                    else
                    {
                        //Stop attack.
                       RESETPACKET( pak, 0x796 );
                       ADDWORD    ( pak, clientid );
                       ADDFLOAT   ( pak, Position->current.x*100 );
                       ADDFLOAT   ( pak, Position->current.y*100 );
                       ADDWORD    ( pak, 0xDC0C );
                       GServer->SendToVisible(&pak,this);
                    }
  	 			}
	      }
    }
    return;
}

//LMA: Searching for replacement arrows, bullets...
int CPlayer::SearchABC ( int slot )
{
    if (slot<132||slot>134)
    {
        return 0;
    }

    for (int k=12;k<132;k++)
    {
        if (items[k].count<=0)
            continue;

        if (items[k].itemnum==items[slot].itemnum&&items[k].itemtype==items[slot].itemtype)
        {
            return k;
        }
    }
    return 0;
}

// return party pointer
CParty* CPlayer::GetParty( )
{
    return Party->party;
}

//returns clan,
CLAN* CPlayer::GetClan( )
{
  return Clan;
}

// return intelligence
unsigned int CPlayer::GetInt( )
{
    //return Attr->Int;
    return (Attr->Int + Attr->Eint); //Tomiz : We need to take int from cloth,gem,ect into consideration(ExtraInt/Eint)
}

// return Sensibility
unsigned int CPlayer::GetSen( )
{
    return (Attr->Sen + Attr->Esen); //Tomiz : We need to take sen from cloth,gem,ect into consideration(ExtraSen/Esen)
}

// return Concentration
unsigned int CPlayer::GetCon( )
{
    return (Attr->Con + Attr->Econ); //Tomiz : We need to take con from cloth,gem,ect into consideration(ExtraCon/Econ)
}

// add item [return item slot [0xffff if couldn't add it]]
unsigned int CPlayer::AddItem( CItem item )
{
    unsigned int newslot=0;
    newslot = GetNewItemSlot( item );
    if(newslot!=0xffff)
    {
        if(items[newslot].count>0)
        {
            unsigned int ntcount = item.count;
            unsigned int utcount = items[newslot].count;
            if(ntcount+utcount>999)
            {
                item.count = ntcount+utcount - 999;
                items[newslot].count = 999;
                unsigned int otherslot = GetNewItemSlot( item );
                if(otherslot!=0xffff)
                {
                    if(items[otherslot].count!=0) items[otherslot].count += item.count;
                    else items[otherslot] = item;
                    return newslot*1000+otherslot;
                }
                else
                {
                    items[newslot].count = utcount;
                    return 0xffff; //not inventory space
                }
            }
            else items[newslot].count = ntcount+utcount;
        }
        else items[newslot] = item;
    }
    return newslot;
}

//LMA: Saving slot with a MySQL 4.1+ function for Inventory
void CPlayer::SaveSlot41( unsigned int slot)
{
   //Update or add a slot (kinky way).
    //Log(MSG_INFO,"[Slot41] Trying to alter slot %i for player %i",slot,CharInfo->charid);
    CalculateSignature(slot);
    int res_mysql=0;
    int sp_item_value=0;


	if (items[slot].itemtype > 0)
    {
        //insert/update
        if (items[slot].itemtype ==14)
           sp_item_value=items[slot].sp_value;
        res_mysql=GServer->DB->QExecuteUpdate("INSERT INTO items (owner,slotnum,itemnum,itemtype,refine,durability,lifespan,count,stats,socketed,appraised,gem,sp_value) VALUES(%i,%i,%i,%i,%i,%i,%i,%i,%i,%i,%i,%i,%i) ON DUPLICATE KEY UPDATE owner=VALUES(owner),itemnum=VALUES(itemnum),itemtype=VALUES(itemtype),refine=VALUES(refine),durability=VALUES(durability),lifespan=VALUES(lifespan),slotnum=VALUES(slotnum),count=VALUES(count),stats=VALUES(stats),socketed=VALUES(socketed),appraised=VALUES(appraised),gem=VALUES(gem),sp_value=VALUES(sp_value)",
    								CharInfo->charid, slot, items[slot].itemnum, items[slot].itemtype,items[slot].refine, items[slot].durability,
    								items[slot].lifespan, items[slot].count, items[slot].stats, (items[slot].socketed?1:0),
    								(items[slot].appraised?1:0),items[slot].gem,sp_item_value );
        //Log(MSG_INFO,"[Slot41] Save slot %i for player %i (res=%i)",slot,CharInfo->charid,res_mysql);
		return;
	}

    //delete the slot.
    GServer->DB->QExecute("DELETE FROM items WHERE owner=%i AND slotnum=%i", CharInfo->charid,slot);
    //Log(MSG_INFO,"[Slot41] Delete slot %i for player %i",slot,CharInfo->charid);
    items[slot].sig_data=-1;
    items[slot].sig_head=-1;
    items[slot].sig_gem=-1;
    items[slot].sp_value=-1;
    items[slot].last_sp_value=-1;


   return;
}

//LMA: saving slot in database (outdated)
void CPlayer::SaveSlot( unsigned int slot)
{
    //Update or add a slot (kinky way).
    Log(MSG_INFO,"Tryng to alter slot %i for player %i",slot,CharInfo->charid);
    CalculateSignature(slot);
    int res_mysql=0;

	if (items[slot].itemtype > 0)
    {
        //insert if update failed.
        res_mysql=GServer->DB->QExecuteUpdate("UPDATE items  SET itemnum=%i, itemtype=%i, refine=%i, durability=%i, lifespan=%i, count=%i, stats=%i, socketed=%i, appraised=%i ,gem=%i WHERE owner=%i AND slotnum=%i",
							items[slot].itemnum, items[slot].itemtype,items[slot].refine, items[slot].durability,
							items[slot].lifespan, items[slot].count, items[slot].stats, (items[slot].socketed?1:0),
							(items[slot].appraised?1:0),items[slot].gem,CharInfo->charid,slot);

		Log(MSG_INFO,"After Update attempt");
        if(res_mysql==0)
        {
           //Slot not in database or nothing has changed.
           Log(MSG_INFO,"Select");
        	MYSQL_RES *result = GServer->DB->QStore("SELECT owner FROM items WHERE owner=%i AND slotnum=%i",CharInfo->charid,slot);
        	if(result==NULL)
            {
               Log(MSG_WARNING,"MySQL Error in SaveSlot");
               return;
            }
           Log(MSG_INFO,"After Select");

        	if(mysql_num_rows(result)==0)
        	{
                    Log(MSG_INFO,"insert");
               GServer->DB->QFree( );
               GServer->DB->QExecute("INSERT INTO items (owner,itemnum,itemtype,refine,durability,lifespan,slotnum,count,stats,socketed,appraised,gem) VALUES(%i,%i,%i,%i,%i,%i,%i,%i,%i,%i,%i,%i)",
    							CharInfo->charid, items[slot].itemnum, items[slot].itemtype,items[slot].refine, items[slot].durability,
    							items[slot].lifespan, slot, items[slot].count, items[slot].stats, (items[slot].socketed?1:0),
    							(items[slot].appraised?1:0),items[slot].gem );
               Log(MSG_INFO,"Insert slot %i for player %i",slot,CharInfo->charid);
               return;
            }

            GServer->DB->QFree( );
            Log(MSG_INFO,"Same Update slot %i for player %i...",slot,CharInfo->charid);
       }
       else
       {
           Log(MSG_INFO,"Update slot %i for player %i, nb rows=%i",slot,CharInfo->charid,res_mysql);
       }


		return;
	}

    //delete the slot.
    GServer->DB->QExecute("DELETE FROM items WHERE owner=%i AND slotnum=%i", CharInfo->charid,slot);
    Log(MSG_INFO,"Delete slot %i for player %i",slot,CharInfo->charid);


   return;
}

//LMA: Gives Clan Points
void CPlayer::GiveCP(unsigned int points)
{
     if(points==0)
         return;
     if (Clan->clanid==0)
     {
        Log(MSG_WARNING,"GiveCP:: User %s does not have a clan",CharInfo->charname);
        return;
     }

     //adding points if needed
     //Asking CharServer to refresh the player's informations.
    char buffer[200];
    sprintf( buffer, "You received %i Clan Points !!", points);
    BEGINPACKET ( pak, 0x702 );
    ADDSTRING( pak, buffer );
    ADDBYTE( pak, 0 );
    client->SendPacket( &pak );

    //LMA: We DON'T get the real value here since it's used for CP consumption
    //that would make too many requests...
    //Clan->CP=GServer->getClanPoints(Clan->clanid);
	if(Clan->CP+points<0)
	{
        Clan->CP=0;
	}
	else
	{
	    Clan->CP+=points;
	}

    RESETPACKET( pak, 0x7e0 );
 	ADDBYTE    ( pak, 0xfe );
	ADDDWORD    ( pak, CharInfo->charid);  //charid
	ADDDWORD    ( pak, points);  //Clan points (to be added)
	cryptPacket( (char*)&pak, GServer->cct );
	send( GServer->csock, (char*)&pak, pak.Size, 0 );

    //Add the last packet needed.
    RESETPACKET( pak, 0x7e0 );
    ADDBYTE    ( pak, 0x35 );
    ADDWORD    ( pak, clientid );
    ADDWORD    ( pak, Clan->clanid );
    ADDWORD    ( pak, 0x0000 );
    ADDWORD    ( pak, Clan->back );
    ADDWORD    ( pak, Clan->logo );
    ADDBYTE    ( pak, Clan->grade );
    ADDBYTE    ( pak, Clan->clanrank);
    ADDSTRING  ( pak, Clan->clanname );
    ADDBYTE    ( pak, 0x00 );
    client->SendPacket(&pak);


     return;
}


//LMA: takes player fuel (or refuels). Handles CG and Cart.
void CPlayer::TakeFuel(int add_fuel)
{
    float conso_fuel=0;
    int save_fuel=0;

    //refuels.
    if(add_fuel>0)
    {
       if(items[135].itemnum==0||items[136].itemnum==0)
          return;

       //Log(MSG_INFO,"Refueling by %i %%, lifespan was %i",add_fuel,items[136].lifespan);
       items[136].lifespan+=add_fuel;
       if(items[136].lifespan>100)
         items[136].lifespan=100;
       items[136].sp_value=items[136].lifespan*10;
       //Log(MSG_INFO,"New fuel lifespan %i",items[136].lifespan);
       BEGINPACKET( pak,0x7ce );
       ADDWORD    ( pak, 0x88 );
       ADDWORD    ( pak, items[136].sp_value ); //%*10
       client->SendPacket( &pak );
       return;
    }

    if(items[135].itemnum==0||items[136].itemnum==0||last_fuel==0)
      return;

    save_fuel=items[136].sp_value;
    if(save_fuel<=0)
       save_fuel=items[136].lifespan*10;

    if(save_fuel==0)
    {
       items[136].sp_value=0;
       BEGINPACKET( pak,0x7ce );
       ADDWORD    ( pak, 0x88 ); //Slot
       ADDWORD    ( pak, 0 ); //lifespan %*10 (fuel here)
       client->SendPacket( &pak );
       return;
    }

    //LMA: new way (STB)
    float total_fuel=(float) GServer->PatList.Index[items[135].itemnum]->jauge+ (float) GServer->PatList.Index[items[136].itemnum]->jauge;   //maximum fuel.

    for (int k=135;k<140;k++)
    {
        if(items[k].itemnum==0)
            continue;

        conso_fuel+=(float) GServer->PatList.Index[items[k].itemnum]->fuelcons;
    }

    //LMA: We save it for later (normal attacks...)
    attack_fuel=conso_fuel;

    //How much taken in the meantime?
    float current_fuel=((float) save_fuel)*total_fuel/(10*100);
    conso_fuel*=250*(clock()-last_fuel)/(60*1000*100);
    //Log(MSG_INFO,"Total fuel %.2f/%.2f, conso fuel %.2f, multiplicator %i, frame %i:%i, engine %i:%i",current_fuel,total_fuel,conso_fuel,GServer->PatList.Index[items[136].itemnum]->fuelcons,GServer->PatList.Index[items[135].itemnum]->fuelcons,GServer->PatList.Index[items[135].itemnum]->maxfuel,GServer->PatList.Index[items[136].itemnum]->fuelcons,GServer->PatList.Index[items[136].itemnum]->maxfuel);

    //How much left?
    if(current_fuel==0)
    {
       items[136].sp_value=0;
       BEGINPACKET( pak,0x7ce );
       ADDWORD    ( pak, 0x88 ); //Slot
       ADDWORD    ( pak, 0 ); //lifespan %*10 (fuel here)
       client->SendPacket( &pak );
       return;
    }

    current_fuel=100*(current_fuel-conso_fuel)/total_fuel;

    if (current_fuel<0)
       current_fuel=0;
    if (current_fuel>100)
       current_fuel=100;
    save_fuel=(int) (current_fuel*10);
    items[136].lifespan=(int) current_fuel;
    items[136].sp_value=save_fuel;
    //Log(MSG_INFO,"New lifespan %i, saved value %i",items[136].lifespan,save_fuel);

    //Sending Lifespan Packet for PAT
    BEGINPACKET( pak,0x7ce );
    ADDWORD    ( pak, 0x88 ); //Slot
    ADDWORD    ( pak, items[136].sp_value ); //lifespan %*10 (fuel here)
    client->SendPacket( &pak );


    return;
}


void CPlayer::UpdateInventory( unsigned int slot1, unsigned int slot2, bool save)
{
     //Log(MSG_INFO,"In Update Inventory");
    if(slot1==0xffff && slot2==0xffff) return;
    BEGINPACKET( pak, 0x718 );
    //if(slot2!=0xffff && slot2!=0xffff) {ADDBYTE( pak, 2 );}
    if(slot1!=0xffff && slot2!=0xffff) {ADDBYTE( pak, 2 );}
    else {ADDBYTE( pak, 1 );}
    if(slot1!=0xffff)
    {
        ADDBYTE    ( pak, slot1);
        ADDDWORD   ( pak, GServer->BuildItemHead( items[slot1] ) );
        ADDDWORD   ( pak, GServer->BuildItemData( items[slot1] ) );
        ADDDWORD( pak, 0x00000000 );
        ADDWORD ( pak, 0x0000 );
    }
    if(slot2!=0xffff)
    {
        ADDBYTE    ( pak, slot2 );
        ADDDWORD   ( pak, GServer->BuildItemHead( items[slot2] ) );
        ADDDWORD   ( pak, GServer->BuildItemData( items[slot2] ) );
        ADDDWORD( pak, 0x00000000 );
        ADDWORD ( pak, 0x0000 );
    }
    client->SendPacket( &pak );

    //LMA: debug.
    //Log(MSG_INFO,"Slot %i, H%i, D%i, Slot %i, H%i, D%i",slot1,GServer->BuildItemHead( items[slot1] ),GServer->BuildItemData( items[slot1] ),slot2,GServer->BuildItemHead( items[slot2] ),GServer->BuildItemData( items[slot2] ));

    //LMA: MySQL Save slot
    if(!save)
        return;

    if(slot1!=0xffff)
         SaveSlot41(slot1);
    if(slot2!=0xffff)
         SaveSlot41(slot2);


    return;
}

//LMA: We don't save this one in database.
void CPlayer::UpdateInventoryNoSave( unsigned int slot1, unsigned int slot2 )
{
     //Log(MSG_INFO,"In Update Inventory");
    if(slot1==0xffff && slot2==0xffff) return;
    BEGINPACKET( pak, 0x718 );
    //if(slot2!=0xffff && slot2!=0xffff) {ADDBYTE( pak, 2 );}
    if(slot1!=0xffff && slot2!=0xffff) {ADDBYTE( pak, 2 );}
    else {ADDBYTE( pak, 1 );}
    if(slot1!=0xffff)
    {
        ADDBYTE    ( pak, slot1);
        ADDDWORD   ( pak, GServer->BuildItemHead( items[slot1] ) );
        ADDDWORD   ( pak, GServer->BuildItemData( items[slot1] ) );
        ADDDWORD( pak, 0x00000000 );
        ADDWORD ( pak, 0x0000 );
    }
    if(slot2!=0xffff)
    {
        ADDBYTE    ( pak, slot2 );
        ADDDWORD   ( pak, GServer->BuildItemHead( items[slot2] ) );
        ADDDWORD   ( pak, GServer->BuildItemData( items[slot2] ) );
        ADDDWORD( pak, 0x00000000 );
        ADDWORD ( pak, 0x0000 );
    }
    client->SendPacket( &pak );
}

void CPlayer::reduceItemsLifeSpan( bool attacked)
{
        hits=0;
        for (int i=1; i< 9; i++) //Find check all equip slots for equiped items
        {
            if ( (i!=7 && attacked) || ( i==7 && (!attacked) ) )
            {
             items[i].durabLeft = (items[i].durabLeft<=0 ? items[i].durability*5:items[i].durabLeft-1);
             //Log(MSG_INFO, "DurabLeft: %i", items[i].durabLeft);
             if (items[i].durabLeft < 1)
             if (items[i].itemnum != 0 && items[i].appraised )
             {
                //Log( MSG_INFO, "Reducing item's life span!\nItemId: %i type: %i", items[i].itemnum, items[i].itemtype);
                items[i].lifespan--;
                if (items[i].lifespan < 1)
                {
                   ClearItem( items[i] );
                   BEGINPACKET( pak, 0x7a5);
                   ADDWORD( pak, clientid );
	               ADDWORD( pak, i);
	               ADDWORD( pak, 0);
	               ADDWORD( pak, 0);
	               ADDWORD( pak, Stats->Move_Speed );
                   GServer->SendToVisible( &pak,this );
                }
                   //UpdateInventory(i);
                   UpdateInventoryNoSave(i);         //LMA: will be saved in database at auto save.
             }
            }
        }
}

unsigned CPlayer::getWeaponType()
{
         return GServer->EquipList[WEAPON].Index[items[7].itemnum]->type;
}

//removes bugged or restricted items - By PurpleYouko
//LMA: allowed now?
bool CPlayer::CheckItems()
{
    /*
     CPlayer* thisclient = GServer->GetClientByCharName( CharInfo->charname );
     if(thisclient->items[7].itemtype == 8 && thisclient->items[7].itemnum == 40)
     {
         thisclient->items[7].count = 0;
         ClearItem( thisclient->items[7] );
         GServer->SendPM(thisclient, "Sorry. The Morningstar is a banned item. It has been confiscated");
         thisclient->SetStats( );
     }*/


    return true;
 }

//Check for negative zuly by PurpleYouko
bool CPlayer::CheckZulies()
{
    //save character info so that we can look at it later in the database
    //CPlayer* thisclient = GServer->GetClientByCharName( CharInfo->charname );
    if((CharInfo->Zulies < 0) || (CharInfo->Storage_Zulies < 0))
    {
        Log(MSG_HACK,"HACK:: %s has been kicked because of negative Zuly.",CharInfo->charname);

        //thisclient->savedata();
        savedata();

        //now kick the player out
        BEGINPACKET( pak, 0x702 );
        ADDSTRING( pak, "You have been disconnected from the server for Zuly hacking!" );
        ADDBYTE( pak, 0 );
        //thisclient->client->SendPacket( &pak );
        client->SendPacket( &pak );

        RESETPACKET( pak, 0x707 );
        ADDWORD( pak, 0 );
        //thisclient->client->SendPacket( &pak );
        client->SendPacket( &pak );

        //thisclient->client->isActive = false;
        client->isActive = false;

    }


  return true;
}

/*
bool CPlayer::AddClanPoints(unsigned int count)
{
     if ( Clan== NULL ) return true;
     //load clan info in char server
	BEGINPACKET( pak, 0x7e0 );
	ADDBYTE    ( pak, 0xfb ); //action to update clan informacion (charserver)
	ADDWORD    ( pak, Clan->clanid );
	//ADDWORD    ( pak, CharInfo->charid );
	ADDDWORD    ( pak, CharInfo->charid );
	ADDWORD    ( pak, clientid );
	ADDWORD    ( pak, count );
	cryptPacket( (char*)&pak, GServer->cct );
	send( GServer->csock, (char*)&pak, pak.Size, 0 );

	printf("added %i clan points\n", count);
}
*/

CClientSocket* CPlayer::getClient()
{
   return client;
}

//LMA: reload itemmall... In fact there must be no void in itemmall :(
//so we load, check and rebuild if necessary...
void CPlayer::RebuildItemMall()
{
    for(int i=0;i<MAX_ITEMMALL;i++)
    {
  		itemmallitems[i].itemnum = 0;
		itemmallitems[i].itemtype = 0;
		itemmallitems[i].refine = 0;
		itemmallitems[i].durability = 0;
		itemmallitems[i].lifespan = 0;
		itemmallitems[i].count = 0;
		itemmallitems[i].stats = 0;
		itemmallitems[i].socketed = 0;
		itemmallitems[i].appraised = 0;
		itemmallitems[i].gem = 0;
    }

	MYSQL_ROW row;
	MYSQL_RES *result = GServer->DB->QStore("SELECT itemnum,itemtype,refine,durability,lifespan,slotnum,count,stats,socketed,appraised,gem FROM mileage WHERE owner=%i", Session->userid);
	if(result==NULL)
    {
       nsitemmallitems=0;
       return;
    }

	nsitemmallitems = mysql_num_rows(result);
	int i=0;
	bool do_again=false;
	while(row = mysql_fetch_row(result))
    {
        if(!GServer->IsValidItem( atoi(row[1]), atoi(row[0]) ) || atoi(row[6])==0)
        {
            Log(MSG_WARNING, "char %s have a invalid or empty item in mileage: %i%i [%i], this item will be deleted", CharInfo->charname, atoi(row[1]), atoi(row[0]), atoi(row[6]) );
            continue;
        }

		UINT itemnum = atoi(row[5]);
		if(itemnum!=i)
		   do_again=true;
		itemmallitems[i].itemnum = atoi(row[0]);
		itemmallitems[i].itemtype = atoi(row[1]);
		itemmallitems[i].refine = atoi(row[2]);
		itemmallitems[i].durability = atoi(row[3]);
		itemmallitems[i].lifespan = atoi(row[4]);
		itemmallitems[i].count = atoi(row[6]);
		itemmallitems[i].stats = atoi(row[7]);
		itemmallitems[i].socketed = (atoi(row[8])==1)?true:false;
		itemmallitems[i].appraised = (atoi(row[9])==1)?true:false;
		itemmallitems[i].gem = atoi(row[10]);
		//Log(MSG_INFO,"We have item %i * %i:%i",itemmallitems[i].count,itemmallitems[i].itemtype,itemmallitems[i].itemnum);
		i++;
	}

    //Log(MSG_INFO,"We have %i items in item mall",nsitemmallitems);
    GServer->DB->QFree( );
    if (!do_again)
       return;

    //We have to do the table again...
    GServer->DB->QExecute("DELETE FROM mileage WHERE owner=%i", Session->userid);
    for(int i=0;i<MAX_ITEMMALL;i++)
    {
      if(itemmallitems[i].itemnum==0)
        continue;
      if(!GServer->DB->QExecute("INSERT INTO mileage (owner,slotnum,itemnum,itemtype,refine,durability,lifespan,count,stats,socketed,appraised,gem) VALUES(%i,%i,%i,%i,%i,%i,%i,%i,%i,%i,%i,%i) ON DUPLICATE KEY UPDATE owner=VALUES(owner),slotnum=VALUES(slotnum),itemnum=VALUES(itemnum),itemtype=VALUES(itemtype),refine=VALUES(refine),durability=VALUES(durability),lifespan=VALUES(lifespan),count=VALUES(count),stats=VALUES(stats),socketed=VALUES(socketed),appraised=VALUES(appraised),gem=VALUES(gem)",
    						Session->userid, i, itemmallitems[i].itemnum, itemmallitems[i].itemtype,itemmallitems[i].refine, itemmallitems[i].durability,
    						itemmallitems[i].lifespan, itemmallitems[i].count, itemmallitems[i].stats, (itemmallitems[i].socketed?1:0),
                            (itemmallitems[i].appraised?1:0), itemmallitems[i].gem ))
      {
          Log(MSG_INFO,"Error in Mysql...");
      }

    }


     return;
}

//LMA: Core's code to check a shield when double weapon equiped.
bool CPlayer::CheckDoubleEquip()
{
    if(items[8].itemnum==0)
    {
        return true;
    }

    UINT weapontype = GServer->EquipList[WEAPON].Index[items[7].itemnum]->type;
    switch(weapontype)
    {
        case TWO_HAND_SWORD:
        case SPEAR:
        case TWO_HAND_AXE:
        case BOW:
        case GUN:
        case LAUNCHER:
        case STAFF:
        case KATAR:
        case DOUBLE_SWORD:
        case DUAL_GUN:
        {
            // we should unequip the shield
            UINT newslot = GetNewItemSlot( items[8] );
            if(newslot==0xffff)
            {
                return true;
            }

            items[newslot] = items[8];
            ClearItem( items[8] );
            UpdateInventory( newslot, 8 );

            BEGINPACKET( pak, 0x7a5 );
            ADDWORD    ( pak, clientid );
            ADDWORD    ( pak, 8 );
            ADDWORD    ( pak, items[8].itemnum );
            ADDWORD    ( pak, GServer->BuildItemRefine( items[8] ) );
            ADDWORD    ( pak, Stats->Move_Speed );
            GServer->SendToVisible( &pak, this);
            SetStats( );

            //resetting quickbar
            Log(MSG_HACK,"%s tryed to use a shield with a double hand weapon",CharInfo->charname);
            GServer->SendPM(this,"<GM> Don't hack the game !!! You're reported !!!");
            /*
            for(int i=0;i<MAX_QUICKBAR;i++)
                quickbar[i] = 0;
            */
        }
        break;
    }


    return true;
}

#ifdef PYCUSTOM
bool CPlayer::CheckPortal()
{
    //CPlayer* thisclient = GServer->GetClientByCharName( CharInfo->charname );
    clock_t etime = clock() - lastportalchecktime;

    if ( etime >= 2 * CLOCKS_PER_SEC )
    {
        lastportalchecktime = clock();
        for(int i=0;i<GServer->CustomGateList.size();i++)
        {
            CCustomGate* thisgate = GServer->CustomGateList.at(i);
            if(!thisgate->active)
                continue;
            if(thisgate->sourcemap == Position->Map && thisgate->active == true)
            {
                float xmin = thisgate->source.x - thisgate->radius;
                float xmax = thisgate->source.x + thisgate->radius;
                float ymin = thisgate->source.y - thisgate->radius;
                float ymax = thisgate->source.y + thisgate->radius;
                if(Position->current.x >= xmin && Position->current.x <= xmax && Position->current.y >= ymin && Position->current.y <=ymax)
                {
                    fPoint coord;
                    coord.x = thisgate->dest.x;
                    coord.y = thisgate->dest.y;
                    GServer->MapList.Index[thisgate->destmap]->TeleportPlayer( this, coord, false );
                    return true;
                }
            }
        }
    }
    return true;
}
bool CPlayer::CheckEvents()
{
   //CPlayer* thisclient = GServer->GetClientByCharName( CharInfo->charname );

   clock_t etime = clock() - lastEventTime;
   clock_t rtime = clock() - RefreshEventTimer;
   if ( etime >= 2 * CLOCKS_PER_SEC )
   {
       lastEventTime = clock( );
       for(int i=0;i<GServer->CustomEventList.size();i++)
       {
           CCustomEvent* thisevent = GServer->CustomEventList.at(i);
           if(thisevent->map == Position->Map && thisevent->active == 1)
           {
               float xmin = thisevent->location.x - thisevent->radius;
               float xmax = thisevent->location.x + thisevent->radius;
               float ymin = thisevent->location.y - thisevent->radius;
               float ymax = thisevent->location.y + thisevent->radius;
               if(Position->current.x >= xmin && Position->current.x <= xmax && Position->current.y >= ymin && Position->current.y <= ymax)
               {
                   //first interrogate the player's inventory to see if they have any of the necessary items.
                   //Log( MSG_INFO, "Found the right event details. Event = %i",CharInfo->ActiveEvent );
                   if(CharInfo->ActiveEvent == true)
                       continue;
                   CharInfo->ActiveEvent = true;
                   //Log( MSG_INFO, "event = %i",CharInfo->ActiveEvent );
                   bool hasitem = false;
                   char buffer2[200];
                   int itemcount = 0;
                   for(int slot=71;slot<103;slot++)
                   {
                       if(items[slot].itemtype == thisevent->collecttype && items[slot].itemnum == thisevent->collectid && items[slot].count >0)
                       {
                           //player has some of the items needed for this event
                           hasitem = true;
                           itemcount +=  items[slot].count;
                       }
                   }
                   // fetch type event. Collect items and exchange for rewards
                   // Also used for pure information that requires no items
                   if (thisevent->eventtype == 0)
                   {
                       if (hasitem == true) //call this script if player has some of the items
                       {
                           if(itemcount < thisevent->prizecost[1])
                           {
                               sprintf ( buffer2, "I'm sorry but you haven't collected enough %s to earn a reward yet. Please collect more", thisevent->itemname );
                               GServer->NPCMessage(this, buffer2, thisevent->npcname);
                           }
                           else
                           {
                               sprintf ( buffer2, "Oh you have collected enough %s to earn a reward.",thisevent->itemname );
                               GServer->NPCMessage(this, buffer2, thisevent->npcname);
                               sprintf ( buffer2, "%s", thisevent->script2 ); //invitation to a prize script
                               GServer->NPCMessage(this, buffer2, thisevent->npcname);
                               for(int j=0;j<10;j++)
                               {
                                   if(itemcount >= thisevent->prizecost[j] && thisevent->prizecost[j] != 0)
                                   {
                                       sprintf ( buffer2, "Item %i: %s. %i %s and its yours", j,thisevent->prizename[j].prizename,thisevent->prizecost[j],thisevent->itemname );
                                       GServer->NPCMessage(this, buffer2, thisevent->npcname);
                                   }
                               }
                           }
                       }
                       else // call this if the player doesn't have any
                       {
                           sprintf ( buffer2, "%s", thisevent->script1 ); //event introduction script
                           GServer->NPCMessage(this, buffer2, thisevent->npcname);
                           Log( MSG_INFO, "Custom event script1 sent for type 0 event" );
                       }
                   }
                   // quest type event
                   // used if a specific item unlocks a different behaviour from the NPC
                   if (thisevent->eventtype == 1)
                   {
                       if(hasitem == false)
                       {
                           sprintf ( buffer2, "%s", thisevent->script1 ); //send this script if the player has no prerequisite item for the next step of the quest
                           GServer->NPCMessage(this, buffer2, thisevent->npcname);
                           Log( MSG_INFO, "Custom event script1 sent for type1 event" );
                       }
                   }
               }
               else
               {
                   CharInfo->ActiveEvent = false;
               }
           }
       }
   }
   return true;
}


bool CPlayer::PrizeExchange(CPlayer* thisclient, UINT prizeid)
{
    //first find which, if any, event area the player is currently in.
    Log( MSG_INFO, "Prize function called" );
    char buffer2[200];
    for(int i=0;i<GServer->CustomEventList.size();i++)
    {
        CCustomEvent* thisevent = GServer->CustomEventList.at(i);
        if(thisevent->map == Position->Map && thisevent->active == 1)
        {
            float xmin = thisevent->location.x - thisevent->radius;
            float xmax = thisevent->location.x + thisevent->radius;
            float ymin = thisevent->location.y - thisevent->radius;
            float ymax = thisevent->location.y + thisevent->radius;
            if(Position->current.x >= xmin && Position->current.x <= xmax && Position->current.y >= ymin && Position->current.y <= ymax)
            {
                //well we found the correct event
                int cost = thisevent->prizecost[prizeid];
                int type = thisevent->prizetype[prizeid];
                int id = thisevent->prizeid[prizeid];
                if (type == 0)return true; //just in case somebody puts in an invalid number.
                //now find if the player has enough items to purchase the prize
                UINT itemcount = 0;
                for(int slot=42;slot<101;slot++)
                {
                    if(items[slot].itemtype == thisevent->collecttype && items[slot].itemnum == thisevent->collectid && items[slot].count > 0)
                    {
                        //player has some of the items needed for this event
                        itemcount += items[slot].count;
                        items[slot].count = 0;
                        ClearItem(items[slot]);
                        BEGINPACKET( pak, 0x718 );
                        {ADDBYTE( pak, 1 );}
                        ADDBYTE    ( pak, slot);
                        ADDDWORD   ( pak, GServer->BuildItemHead( items[slot] ) );
                        ADDDWORD   ( pak, GServer->BuildItemData( items[slot] ) );
                        client->SendPacket( &pak );
                    }
                }
                if(itemcount >= cost)
                {
                    Log( MSG_INFO, "Itemcount = %i. Cost = %i",itemcount,cost );
                    //OK the player meets the criteria. Now give him the prize and take away the items
                    sprintf ( buffer2, "Your prize has been placed into your inventory.");
                    GServer->NPCMessage(this, buffer2, thisevent->npcname);
                    bool freeslot = false;
                    int slot;
                    for(slot=12;slot<41;slot++)
                    {
                        if(items[slot].count == 0)
                        {
                            freeslot = true;
                            break;
                        }
                    }
                    if(freeslot) // put the unused items back into inventory
                    {
                        itemcount -= cost;
                        items[slot].itemnum = thisevent->prizeid[prizeid];
                        items[slot].itemtype = thisevent->prizetype[prizeid];
                        items[slot].count = 1;
                        items[slot].lifespan = 100;
                        items[slot].durability = GServer->RandNumber(25, 50);
                        items[slot].stats = GServer->RandNumber(1, 299);
                        BEGINPACKET( pak, 0x718 );
                        ADDBYTE( pak, 1 );
                        ADDBYTE    ( pak, slot);
                        ADDDWORD   ( pak, GServer->BuildItemHead( items[slot] ) );
                        ADDDWORD   ( pak, GServer->BuildItemData( items[slot] ) );
                        client->SendPacket( &pak );
                        if (thisevent->collecttype == 12)
                        {
                          for(int slt=72;slt<101;slt++)
                          {
                            if(items[slt].count == 0 && itemcount > 0)
                            {
                                if(itemcount <= 999)
                                {
                                    items[slt].count = itemcount;
                                    items[slt].itemtype = thisevent->collecttype;
                                    items[slt].itemnum = thisevent->collectid;
                                    itemcount = 0;
                                    RESETPACKET( pak, 0x718 );
                                    ADDBYTE( pak, 1 );
                                    ADDBYTE    ( pak, slt);
                                    ADDDWORD   ( pak, GServer->BuildItemHead( items[slt] ) );
                                    ADDDWORD   ( pak, GServer->BuildItemData( items[slt] ) );
                                    client->SendPacket( &pak );
                                    Log( MSG_INFO, "Placed items into slot %i",slt );
                                }
                                else
                                {
                                    items[slt].count = 999;
                                    items[slt].itemtype = thisevent->collecttype;
                                    items[slt].itemnum = thisevent->collectid;
                                    itemcount -= 999;
                                    RESETPACKET( pak, 0x718 );
                                    ADDBYTE( pak, 1 );
                                    ADDBYTE    ( pak, slt);
                                    ADDDWORD   ( pak, GServer->BuildItemHead( items[slt] ) );
                                    ADDDWORD   ( pak, GServer->BuildItemData( items[slt] ) );
                                    client->SendPacket( &pak );
                                    Log( MSG_INFO, "Placed items into slot %i",slt );
                                }
                            }
                          }
                        }
                        if (thisevent->collecttype == 10)
                        {
                          for(int slt=42;slt<71;slt++)
                          {
                            if(items[slt].count == 0 && itemcount > 0)
                            {
                                if(itemcount <= 999)
                                {
                                    items[slt].count = itemcount;
                                    items[slt].itemtype = thisevent->collecttype;
                                    items[slt].itemnum = thisevent->collectid;
                                    itemcount = 0;
                                    RESETPACKET( pak, 0x718 );
                                    ADDBYTE( pak, 1 );
                                    ADDBYTE    ( pak, slt);
                                    ADDDWORD   ( pak, GServer->BuildItemHead( items[slt] ) );
                                    ADDDWORD   ( pak, GServer->BuildItemData( items[slt] ) );
                                    client->SendPacket( &pak );
                                    Log( MSG_INFO, "Placed items into slot %i",slt );
                                }
                                else
                                {
                                    items[slt].count = 999;
                                    items[slt].itemtype = thisevent->collecttype;
                                    items[slt].itemnum = thisevent->collectid;
                                    itemcount -= 999;
                                    RESETPACKET( pak, 0x718 );
                                    ADDBYTE( pak, 1 );
                                    ADDBYTE    ( pak, slt);
                                    ADDDWORD   ( pak, GServer->BuildItemHead( items[slt] ) );
                                    ADDDWORD   ( pak, GServer->BuildItemData( items[slt] ) );
                                    client->SendPacket( &pak );
                                    Log( MSG_INFO, "Placed items into slot %i",slt );
                                }
                            }
                          }
                        }

                    }
                    else //didn't have a free slot so put all the items back
                    {
                       Log( MSG_INFO, "Sorry we don't have a free slot" );
                       sprintf ( buffer2, "Sorry You don't have a free slot to put the item in.");
                       GServer->NPCMessage(this, buffer2, thisevent->npcname);
                       if (thisevent->collecttype == 12)
                       {
                         for(int slt=72;slt<101;slt++)
                         {
                            if(items[slt].count == 0 && itemcount > 0)
                            {
                                if(itemcount < 999)
                                {
                                    items[slt].count = itemcount;
                                    items[slt].itemtype = thisevent->collecttype;
                                    items[slt].itemnum = thisevent->collectid;
                                    itemcount = 0;
                                    BEGINPACKET( pak, 0x718 );
                                    ADDBYTE( pak, 1 );
                                    ADDBYTE    ( pak, slt);
                                    ADDDWORD   ( pak, GServer->BuildItemHead( items[slt] ) );
                                    ADDDWORD   ( pak, GServer->BuildItemData( items[slt] ) );
                                    client->SendPacket( &pak );
                                }
                                else
                                {
                                    items[slt].count = 999;
                                    items[slt].itemtype = thisevent->collecttype;
                                    items[slt].itemnum = thisevent->collectid;
                                    itemcount -= 999;
                                    BEGINPACKET( pak, 0x718 );
                                    ADDBYTE( pak, 1 );
                                    ADDBYTE    ( pak, slt);
                                    ADDDWORD   ( pak, GServer->BuildItemHead( items[slt] ) );
                                    ADDDWORD   ( pak, GServer->BuildItemData( items[slt] ) );
                                    client->SendPacket( &pak );
                                }
                            }
                          }
                        }
                        if (thisevent->collecttype == 10)
                       {
                         for(int slt=42;slt<71;slt++)
                         {
                            if(items[slt].count == 0 && itemcount > 0)
                            {
                                if(itemcount < 999)
                                {
                                    items[slt].count = itemcount;
                                    items[slt].itemtype = thisevent->collecttype;
                                    items[slt].itemnum = thisevent->collectid;
                                    itemcount = 0;
                                    BEGINPACKET( pak, 0x718 );
                                    ADDBYTE( pak, 1 );
                                    ADDBYTE    ( pak, slt);
                                    ADDDWORD   ( pak, GServer->BuildItemHead( items[slt] ) );
                                    ADDDWORD   ( pak, GServer->BuildItemData( items[slt] ) );
                                    client->SendPacket( &pak );
                                }
                                else
                                {
                                    items[slt].count = 999;
                                    items[slt].itemtype = thisevent->collecttype;
                                    items[slt].itemnum = thisevent->collectid;
                                    itemcount -= 999;
                                    BEGINPACKET( pak, 0x718 );
                                    ADDBYTE( pak, 1 );
                                    ADDBYTE    ( pak, slt);
                                    ADDDWORD   ( pak, GServer->BuildItemHead( items[slt] ) );
                                    ADDDWORD   ( pak, GServer->BuildItemData( items[slt] ) );
                                    client->SendPacket( &pak );
                                }
                            }
                          }
                        }
                    }
                }
                else
                {
                    for(int slt=72;slt<101;slt++) //put the stuff back into inventory
                    {
                         if(items[slt].count == 0 && itemcount > 0)
                         {
                             if(itemcount < 999)
                             {
                                 items[slt].count = itemcount;
                                 items[slt].itemtype = thisevent->collecttype;
                                 items[slt].itemnum = thisevent->collectid;
                                 itemcount = 0;
                                 BEGINPACKET( pak, 0x718 );
                                 ADDBYTE( pak, 1 );
                                 ADDBYTE    ( pak, slt);
                                 ADDDWORD   ( pak, GServer->BuildItemHead( items[slt] ) );
                                 ADDDWORD   ( pak, GServer->BuildItemData( items[slt] ) );
                                 client->SendPacket( &pak );
                             }
                             else
                             {
                                 items[slt].count = 999;
                                 items[slt].itemtype = thisevent->collecttype;
                                 items[slt].itemnum = thisevent->collectid;
                                 itemcount -= 999;
                                 BEGINPACKET( pak, 0x718 );
                                 ADDBYTE( pak, 1 );
                                 ADDBYTE    ( pak, slt);
                                 ADDDWORD   ( pak, GServer->BuildItemHead( items[slt] ) );
                                 ADDDWORD   ( pak, GServer->BuildItemData( items[slt] ) );
                                 client->SendPacket( &pak );
                                 itemcount -= 999;
                             }
                         }

                    }

                    sprintf ( buffer2, "Sorry You don't have enough for that.");
                    GServer->NPCMessage(this, buffer2, thisevent->npcname);
                }
            }
        }
    }
    return true;
}
#endif


//QSD Quests
int CPlayer::ExecuteQuestTrigger(dword hash,bool send_packet, UINT index)
{
    Log(MSG_WARNING,"EXTP BEGIN %u for %s, index %u",hash,CharInfo->charname,index);
    CQuestTrigger* trigger = NULL;
    CQuestTrigger* nexttrigger = NULL;
    CheckQuest = -1;

    //LMA: To counter some very weird cases where the hashes exist several times...
    UINT my_index=0;
    //for(unsigned j=0; j < GServer->TriggerList.size(); j++)
    for(unsigned j=index; j < GServer->TriggerList.size(); j++)
    {
      if (GServer->TriggerList.at(j)->TriggerHash == hash)
      {
        trigger = GServer->TriggerList.at(j);

        //LMA: bug on next trigger.
        if(j+1<GServer->TriggerList.size())
        {
            my_index=j+1;
            nexttrigger = GServer->TriggerList.at(j + 1);
            Log(MSG_WARNING,"EXTP %u for %s:: next trigger %u, %s FORCED",hash,CharInfo->charname,nexttrigger->TriggerHash,nexttrigger->TriggerName);
        }
        else
        {
            Log(MSG_WARNING,"EXTP %u for %s:: next trigger is NULL!",hash,CharInfo->charname);
        }

        break;
      }

    }

    if (trigger == NULL)
    {
        Log(MSG_WARNING,"EXTP, questid %u not found",hash);
        return QUEST_FAILURE;
    }

    int success = QUEST_SUCCESS;
    LogDebug( "EXTP::Trigger Executed: %s[%i]", trigger->TriggerName, trigger->CheckNext);
    Log(MSG_WARNING,"EXTP::Trigger Executed: %s[next? %i]", trigger->TriggerName, trigger->CheckNext);

    for (dword i = 0; i < trigger->ConditionCount; i++)
    {
      int command = trigger->Conditions[i]->opcode;
      if (command > 30 || command < 0) continue;
      success = (*GServer->qstCondFunc[command])(GServer, this, trigger->Conditions[i]->data);
      LogDebug( "EXTP::Condition %03u returned %d", command, success);
      Log(MSG_WARNING,"EXTP::Condition %03u returned %d", command, success);

      if (success == QUEST_FAILURE)
      {
        if (!trigger->CheckNext)
        {
            LogDebug("EXTP::No checknext (FAILURE)");
            Log(MSG_WARNING,"EXTP::No checknext (FAILURE)");
            return success;
        }
        else
        {
            LogDebug("EXTP::checknext because FAILURE");
            Log(MSG_WARNING,"EXTP::checknext because FAILURE, will do %u",nexttrigger->TriggerHash);

            if(nexttrigger==NULL)
            {
                Log(MSG_WARNING,"CPLAYER::ExecuteQuestTrigger, Next trigger but NULL! %u",hash);
                return QUEST_SUCCESS;
            }

            return ExecuteQuestTrigger(nexttrigger->TriggerHash,send_packet,my_index);
        }

      }

      LogDebug("EXTP::Quest cdt success");
      Log(MSG_WARNING,"EXTP::Quest cdt %i / %i success",i+1,trigger->ConditionCount);
    }

    Log(MSG_WARNING,"EXTP:: OUT of CDT");

    for (dword i = 0; i < trigger->ActionCount; i++)
    {
      int command = trigger->Actions[i]->opcode;

      //LMA: command 29 is ok.
      //if ((command > 28 || command < 0) && command != 34)
      if ((command > 29 || command < 0) && command != 34)
      {
          LogDebug( "unknown Action command %i", command);
          Log(MSG_WARNING,"unknown Action command %i", command);
          continue;
      }

      //LogDebug("EXTP::QSD ACT %03u BEGIN",command);

      //Don't delete that LOG!!!
      //(*GServer->qstRewdFunc[command])(GServer, this, trigger->Actions[i]->data);
      LogDebug( "EXTP::Reward %03u returned %d, %i / %i", command, (*GServer->qstRewdFunc[command])(GServer, this, trigger->Actions[i]->data),i+1,trigger->ActionCount);
    }

    //LMA: In some case we have to send a quest packet.
    if(send_packet&&success==QUEST_SUCCESS)
    {
        //Log(MSG_WARNING,"P::Sending quest ok for questid %u",hash);
        BEGINPACKET ( pak, 0x730);
        ADDBYTE ( pak, success);
        ADDBYTE ( pak, 0);
        ADDDWORD( pak, hash);
        client->SendPacket(&pak);
    }

    Log(MSG_WARNING,"EXTP END %u for %s",hash,CharInfo->charname);

    return success;
}

SQuest* CPlayer::GetActiveQuest( )
{
    for(dword i = 0; i < 10; i++){
      if(quest.quests[i].QuestID != ActiveQuest) continue;
      return &quest.quests[i];
    }
    return NULL;
}

void CPlayer::SetQuestVar(short nVarType, short nVarNO, short nValue){
  switch(nVarType){
    case 0:
    {
      SQuest* activeQuest = GetActiveQuest();
      if(activeQuest == NULL) return;
      activeQuest->Variables[nVarNO] = nValue;
    }
    return;
    case 0x100:
    {
      SQuest* activeQuest = GetActiveQuest();
      if(activeQuest == NULL) return;
      activeQuest->SetSwitchBit(nVarNO, nValue);
    }
    return;
    case 0x300:
      if(nVarNO >= 5) return;
      quest.EpisodeVar[nVarNO] = nValue;
      return;
    case 0x400:
      if(nVarNO >= 3) return;
      quest.JobVar[nVarNO] = nValue;
      return;
    case 0x500:
      if(nVarNO >= 7) return;
      quest.PlanetVar[nVarNO] = nValue;
      return;
    case 0x600:
      if(nVarNO >= 10) return;
      quest.UnionVar[nVarNO] = nValue;
      return;
    case 0x700:
    {
        //LMA: ClanVar
        if(nVarNO >= 5) return;
        quest.ClanVar[nVarNO] = nValue;
        return;
    }
    break;

  }
}

int CPlayer::GetQuestVar(short nVarType, short nVarNO){
  switch(nVarType){
    case 0:
    {
      SQuest* activeQuest = GetActiveQuest();
      if(activeQuest == NULL) return -1;
      return activeQuest->Variables[nVarNO];
    }
    case 0x100:
    {
      SQuest* activeQuest = GetActiveQuest();
      if(activeQuest == NULL) return -1;
      return activeQuest->GetSwitchBit(nVarNO);
    }
    case 0x200://Remaining time
      Log(MSG_WARNING,"GetQuestVar got remaining quest time, but not coded yet!");
      return 1;
    case 0x300:
      if(nVarNO >= 5) return -1;
      return quest.EpisodeVar[nVarNO];
    case 0x400:
      if(nVarNO >= 3) return -1;
      return quest.JobVar[nVarNO];
    case 0x500:
      if(nVarNO >= 7) return -1;
      return quest.PlanetVar[nVarNO];
    case 0x600:
      if(nVarNO >= 10) return -1;
      return quest.UnionVar[nVarNO];
    case 0x700:
    {
        //LMA: ClanVar
      if(nVarNO >= 5) return -1;
      return quest.ClanVar[nVarNO];
    }
    break;

  }
  return -1;
}


//LMA: Checking if a stat is ok to equip an item.
bool CPlayer::CheckStats(int slot, int dest_slot)
{
    if(slot==0||items[slot].itemtype==0||items[slot].itemnum==0)
    {
        return true;
    }

    //Checking slots:
    int good_slot=0;
    switch(items[slot].itemtype)
    {
        case 1:
        {
            good_slot=1;
        }
        break;
        case 2:
        {
            good_slot=2;
        }
        break;
        case 3:
        {
            good_slot=3;
        }
        break;
        case 4:
        {
            good_slot=5;
        }
        break;
        case 5:
        {
            good_slot=6;
        }
        break;
        case 6:
        {
            good_slot=4;
        }
        break;
        case 7:
        {
            //jewels 9-11
            if (dest_slot!=9&&dest_slot!=10&&dest_slot!=11)
            {
                Log(MSG_HACK,"Player %s tried to equip item %u::%u slot %i (dest slot %i) but can't be equiped (should be a jewel).",CharInfo->charname,items[slot].itemtype,items[slot].itemnum,slot,dest_slot);
                return false;
            }

        }
        break;
        case 8:
        {
            good_slot=7;
        }
        break;
        case 9:
        {
            good_slot=8;
        }
        break;
        case 14:
        {
            //135-139
            if (dest_slot!=135&&dest_slot!=136&&dest_slot!=137&&dest_slot!=138&&dest_slot!=139)
            {
                Log(MSG_HACK,"Player %s tried to equip item %u::%u slot %i (dest slot %i) but can't be equiped (should be a PAT).",CharInfo->charname,items[slot].itemtype,items[slot].itemnum,slot,dest_slot);
                return false;
            }

        }
        break;
        default:
        {
            //We can't equip 10, 11, 12, 13.
            Log(MSG_HACK,"Player %s tried to equip item %u::%u slot %i but can't be equiped.",CharInfo->charname,items[slot].itemtype,items[slot].itemnum,slot);
            return false;
        }
        break;
    }

    if(good_slot!=0&&(dest_slot!=good_slot))
    {
        Log(MSG_HACK,"Player %s tried to equip item %u::%u slot %i but can't be equiped in slot %i (asked slot %i).",CharInfo->charname,items[slot].itemtype,items[slot].itemnum,slot,good_slot,dest_slot);
        return false;
    }

    //Checking stats (TODO, add them all :( ):
    for(int k=0;k<2;k++)
    {
        int cdt=0;
        int value=0;
        int player_value=0;

        if(k==0)
        {
            cdt=GServer->EquipList[items[slot].itemtype].Index[items[slot].itemnum]->condition1[0];
            //int value=GServer->EquipList[items[slot].itemtype].Index[items[slot].itemnum]->stat1[k];
            value=GServer->EquipList[items[slot].itemtype].Index[items[slot].itemnum]->condition1[1];
        }
        else
        {
            cdt=GServer->EquipList[items[slot].itemtype].Index[items[slot].itemnum]->condition2[0];
            //int value=GServer->EquipList[items[slot].itemtype].Index[items[slot].itemnum]->stat1[k];
            value=GServer->EquipList[items[slot].itemtype].Index[items[slot].itemnum]->condition2[1];
        }

        if(cdt==0)
        {
            continue;
        }

        switch(cdt)
        {
            case sLevel:
            {
                player_value=Stats->Level;
            }
            break;
            case sStrength:
            {
                player_value=Attr->Str;
            }
            break;
            case sDexterity:
            {
                player_value=Attr->Dex;
            }
            break;
            case sIntelligence:
            {
                player_value=Attr->Int;
            }
            break;
            case sConcentration:
            {
                player_value=Attr->Con;
            }
            break;
            case sCharm:
            {
                player_value=Attr->Cha;
            }
            break;
            case sSensibility:
            {
                player_value=Attr->Sen;
            }
            break;
            default:
            {
                Log(MSG_WARNING,"CheckStats:: unknown stat %i value %i for item %u::%u, index %i",cdt,value,items[slot].itemtype,items[slot].itemnum,k);
                value=0;
            }
            break;
        }

        if(value>player_value)
        {
            Log(MSG_HACK,"Player %s tried to equip item %u::%u slot %i but stat %i is wrong (%i>%i)",CharInfo->charname,items[slot].itemtype,items[slot].itemnum,slot,cdt,value,player_value);
            return false;
        }

    }

    //LMA: Checking jobs now.
    bool is_ok=false;
    int nb_0=0;
    int job_id=0;
    for (int k=0;k<3;k++)
    {
        job_id=GServer->EquipList[items[slot].itemtype].Index[items[slot].itemnum]->occupation[k];
        if (job_id==0)
        {
            nb_0++;
            continue;
        }

        if (!CheckJobs(job_id))
        {
            continue;
        }

        is_ok=true;
        break;
    }

    //sometimes there are no requirements (or visitor so usueless...)
    if(nb_0==3)
    {
        is_ok=true;
    }

    if(!is_ok)
    {
        Log(MSG_HACK,"Player %s tried to equip item %u::%u slot %i but job check failed (idx %i, %i, %i (see list_class.stb), job %i)",CharInfo->charname,items[slot].itemtype,items[slot].itemnum,slot,
            GServer->EquipList[items[slot].itemtype].Index[items[slot].itemnum]->occupation[0],
            GServer->EquipList[items[slot].itemtype].Index[items[slot].itemnum]->occupation[1],
            GServer->EquipList[items[slot].itemtype].Index[items[slot].itemnum]->occupation[2],
            CharInfo->Job);
        return false;
    }


    return true;
}


//LMA: Checking job requirements.
bool CPlayer::CheckJobs(int item_job)
{
    if (item_job<0)
    {
        Log(MSG_WARNING,"CheckJobs:: wrong item_job (negative) %i",item_job);
        return false;
    }

    //Special case, union.
    if(item_job>0&&item_job<=5)
    {
        if(CharInfo->unionid==item_job)
        {
            return true;
        }
        else
        {
            return false;
        }

    }

    //jobs.
    if (item_job>=GServer->maxClass)
    {
        Log(MSG_WARNING,"CheckJobs:: wrong item_job %i>=%u",item_job,GServer->maxClass);
        return false;
    }

    bool is_ok=false;
    int nb_slots=GServer->ClassList[item_job]->size();
    if (nb_slots==0)
    {
        //nothing to check, we still return true.
        Log(MSG_WARNING,"CheckJobs:: nothing to check for item_job %i?",item_job);
        return true;
    }

    //it works like "OK".
    for(int k=0;k<nb_slots;k++)
    {
        if(GServer->ClassList[item_job]->at(k)!=CharInfo->Job)
        {
            continue;
        }

        is_ok=true;
        break;
    }


    return is_ok;
}



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
#include "worldmonster.h"

// check is a player is in range
bool CMonster::PlayerInRange()
{
    CMap* map = GServer->MapList.Index[Position->Map];
    for(int i=0; i<map->PlayerList.size(); i++)
    {
        CPlayer* thisclient = map->PlayerList.at(i);
        if(GServer->IsVisible(thisclient, this)) return true;
    }
    return false;
}

//LMA: maps, is there a player in a nearby grid (alternative to PlayerInRange)
bool CMonster::PlayerInGrid()
{
    int grid_id=0;
    UINT coords=0;


    //Log(MSG_INFO,"position->Map %i",Position->Map);
    CMap* map = GServer->MapList.Index[Position->Map];
    grid_id=GServer->allmaps[map->id].grid_id;
    //we don't handle this map (player shouldn't be here, monster neither, non existing map).
    //Or no players in map.
    if (map->PlayerList.size()==0||grid_id==-1)
    {
        return false;
    }

    //getting coordinates.
    coords=GServer->GetGridNumber((int) map->id,(int) Position->current.x,(int) Position->current.y,this);

    //LMA: Special case where a monster is outside the grid, we update it to kill it.
    if(coords==0&&Stats->HP<=0)
    {
        return true;
    }

    //Is there anyone in the grids nearby?
    int nb_players=0;
    int col_offset=0;
    col_offset=GServer->allmaps[map->id].nb_col+2;
    nb_players=GServer->gridmaps[grid_id].coords[coords]+GServer->gridmaps[grid_id].coords[coords+1]+GServer->gridmaps[grid_id].coords[coords-1];
    nb_players+=GServer->gridmaps[grid_id].coords[coords+col_offset]+GServer->gridmaps[grid_id].coords[coords+col_offset+1]+GServer->gridmaps[grid_id].coords[coords+col_offset-1];
    nb_players+=GServer->gridmaps[grid_id].coords[coords-col_offset]+GServer->gridmaps[grid_id].coords[coords-col_offset+1]+GServer->gridmaps[grid_id].coords[coords-col_offset-1];
    if (nb_players>0)
    {
        //Log(MSG_INFO,"[GRID-%i] Mob %i X(%.2f,%.2f)",coords,clientid,Position->current.x,Position->current.y);
       return true;
    }

    //Still here? Special case for very little maps or special maps.
    if (map->PlayerList.size()>0&&GServer->allmaps[map->id].always_on==true)
    {
        //Log(MSG_INFO,"monster in cell %i (AUTO)",coords);
        return true;
    }


    return false;
}

// get the near player
CPlayer* CMonster::GetNearPlayer( UINT mdist )
{
    CPlayer* thisplayer = NULL;
    float distance = 0xffff;
    for(UINT i=0;i<GServer->MapList.Index[Position->Map]->PlayerList.size();i++)
    {
        CPlayer* thisclient = GServer->MapList.Index[Position->Map]->PlayerList.at(i);
        float tempdist = GServer->distance( Position->current, thisclient->Position->current );
        if(tempdist<distance)
        {
            thisplayer = thisclient;
            distance = tempdist;
        }
    }

    if(distance>mdist)
        return NULL; // near player is too far


    return thisplayer;
}


// return true if is a ghost
bool CMonster::IsGhost( )
{
    // candle host, elec ghost, yigore ghost
    if((montype>710 && montype<751) || (montype>679 && montype<694) || (montype>697 && montype<700))
        return true;
    return false;
}

// return true if is a ghost seed
bool CMonster::IsGhostSeed( )
{
    if((montype>700 && montype<711) && montype!=703)
        return true;
    return false;
}

// Return True if bonfire/salamander flame/mana flame.. by Terr0risT
bool CMonster::IsBonfire( )
{
    if(montype>770 && montype<811)
        return true;
    return false;
}

// return true if current monster can move
bool CMonster::CanMove( )
{
    if (Status->Stun!=0xff)
       return false;
    //LMA: bonfire case handled elsewhere...
    //if((montype<41 || montype>45) && (montype<325 || montype>329) && montype!=659 && montype!=992 && (montype<1474 || montype>1489))
    if (thisnpc->wspeed!=0 && thisnpc->rspeed!=0)
        return true;
    return false;
}

// return true if this monster is ready to move
bool CMonster::ReadyToMove( )
{
    return true;
}

// return a player pointers
CPlayer* CMonster::GetOwner( )
{
    if(!IsSummon( )) return NULL;
    CPlayer* ownerclient = GServer->GetClientByID( owner, Position->Map );
    return ownerclient;
}

bool CMonster::IsMonster( )
{
    return true;
}

//LMA: moving to long long...
//void CMonster::AddDamage( CCharacter* enemy, long long hitpower)
void CMonster::AddDamage( CCharacter* enemy, long long hitpower)
{
    //Log(MSG_INFO,"In CMonster::AddDamage");

    CPlayer* player = NULL;
    if(enemy->IsMonster( ))
    {
        CMonster* monster = (CMonster*) enemy;
        if(!enemy->IsSummon( )) return;
        player = monster->GetOwner( );
        if(player==NULL) return;
    }
    else
    {
        player = (CPlayer*) enemy;
    }
    if(MonsterDrop->firsthit==0 && hitpower > 0)
    {
        MonsterDrop->firsthit = player->CharInfo->charid;
        MonsterDrop->firstlevel = player->Stats->Level;
        if( player->Party->party!=NULL )
            thisparty = player->Party->party;
        else
            thisparty = NULL;
    }
    for(UINT i=0;i<PlayersDamage.size();i++)
    {
        MonsterDamage* thisplayer = PlayersDamage.at(i);
        if(thisplayer->charid == player->CharInfo->charid)
        {
            if( hitpower > Stats->HP )
            {
                //hitpower += (long int)Stats->HP + (long int)( Stats->HP * 0.10 );
                hitpower += Stats->HP + (long long) ( Stats->HP * 0.10 );
            }

            thisplayer->damage += hitpower;
            return;
        }
    }
    MonsterDamage* newplayer = new MonsterDamage;
    newplayer->charid = player->CharInfo->charid;
    if( hitpower > Stats->HP )
    {
        //hitpower += (long int)Stats->HP + (long int)( Stats->HP  * 0.10 );
        hitpower += Stats->HP + (long long)( Stats->HP  * 0.10 );
    }
    newplayer->damage = hitpower;
    PlayersDamage.push_back( newplayer );
}

//hidden
CDrop* CMonster::GetDrop( )
{
    //added a new functionality to PYGetDrop.
    //now requires an input value for droptype. A droptype of 1 is a normal drop while a droptype of 2 can be sent to generate a drop while the monster is still alive
    //Code for these 'side drops' is still under development and will follow soon
    //GServer->PYGetDrop( this, 1 );

    if (IsDead())
    {
        drop_dead=true;
    }

    //LMA: And system
    if(!GServer->Config.drop_rev)
    {
        return GServer->GetPYDrop( this, 1 );
    }
    else
    {
        return GServer->GetPYDropAnd( this, 1 );
    }

}

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

#include "party.h"

CParty::CParty( )
{
    PartyId=0;
    PartyLevel = 1;
    Exp = 0;
    Option =0;
    LastItem = 0;
    LastETC = 0;
    LastZulies = 0;
    Capacity = 5;
    Members.clear();
    //PartyExp = new CPartyExp;
}

CParty::~CParty( )
{
    //delete PartyExp;
}

//LMA: We set the max number of Party members
void CParty::RefreshMax()
{
    /*Party level : 1~4 -----> Up to 4 players
    Party Level : 5~9 -----> Up to 5 players
    Party Level : 10~14 -----> Up to 6 players
    Party LeveL : Over 15 -----> Up to 7 players*/
    int new_one=0;

    if(PartyLevel>=1&&PartyLevel<=4)
    {
        new_one=4;
    }

    if(PartyLevel>=5&&PartyLevel<=9)
    {
        new_one=5;
    }

    if(PartyLevel>=10&&PartyLevel<=14)
    {
        new_one=6;
    }

    if(PartyLevel>=15)
    {
        new_one=7;
    }

    if (new_one!=Capacity)
    {
        Capacity=new_one;
    }


    return;
}


CPlayer* CParty::GetMember( UINT id )
{
    for(UINT i=0;i<Members.size( );i++)
    {
        CPlayer* member = Members.at(i);
        if(member->clientid==id)
            return member;
    }
    return NULL;
}

CPlayer* CParty::GetMemberByCharID( DWORD id )
{
    for(UINT i=0;i<Members.size( );i++)
    {
        CPlayer* member = Members.at(i);
        if(member->CharInfo->charid==id)
            return member;
    }
    return NULL;
}

void CParty::SendToMembers( CPacket* pak, CPlayer* explayer )
{
    for(UINT i=0;i<Members.size( );i++)
    {
        CPlayer* member = Members.at(i);
        if(member==explayer)
            continue;
        //Maxxon: Party crash when exit fix.
        if (member == NULL) {
            continue;
        }
        member->client->SendPacket( pak );
    }
}

void CParty::SendToVisible( CPacket* pak, CPlayer* thisclient )
{
    for(UINT i=0;i<Members.size( );i++)
    {
        CPlayer* member = Members.at(i);
        if(member==thisclient)
            continue;
        if(GServer->IsVisible( thisclient, member ))
            member->client->SendPacket( pak );
    }
}

bool CParty::AddPlayer( CPlayer* player )
{
    player->Party->party = this;
    Members.push_back( player );
    return true;
}

bool CParty::RemovePlayer( CPlayer* player )
{
    for(UINT i=0;i<Members.size( );i++)
    {
        if(Members.at(i)==player)
        {
            Members.erase( Members.begin( )+i );
            player->Party->IsMaster = true;
            player->Party->party = NULL;
            return true;
        }
    }
    return false;
}






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

/*bool CWorldServer::DoQuestScript( CPlayer* thisclient, CQuest* thisquest )
{
    BEGINPACKET( pak, 0 );
    switch( thisquest->script )
    {
        case 1: //Teleport
        {
            if(thisquest->value3>0)
            {
                thisclient->CharInfo->Zulies -= thisquest->value3;
               	RESETPACKET( pak, 0x71e );
                ADDQWORD   ( pak, thisclient->CharInfo->Zulies );
                ADDBYTE    ( pak, 0x00 );
                thisclient->client->SendPacket( &pak );             
            }
            fPoint thispoint;
            thispoint.x = floor( thisquest->value2/10000 );
            thispoint.y = floor( thisquest->value2%10000 );
            TeleportTo ( thisclient, thisquest->value1, thispoint );       
            thisclient->EndQuest( thisquest->questid );
        }
        break;    
        case 2: // Change Job
        {
            if( thisclient->CharInfo->Job == 0 )
            {
                thisclient->CharInfo->Job = thisquest->value1;
                RESETPACKET(pak, 0x721 );
                ADDWORD    (pak, 0x0004 );
                ADDWORD    (pak, thisclient->CharInfo->Job );
                ADDWORD    (pak, 0x0000 );
                thisclient->client->SendPacket(&pak);                                  
            }          
        }
        break;  
        case 20: // 2nd Change Job
        {   //Credits to tomiz
            if( thisclient->CharInfo->Job > 0 )
            {
                thisclient->CharInfo->Job = thisquest->value1;
                RESETPACKET(pak, 0x721 );
                ADDWORD    (pak, 0x0004 );
                ADDWORD    (pak, thisclient->CharInfo->Job );
                ADDWORD    (pak, 0x0000 );
                thisclient->client->SendPacket(&pak);
            }
            if(thisquest->value3>0) // zuly need for second job
            {
                thisclient->CharInfo->Zulies-= thisquest->value3;
                RESETPACKET( pak, 0x71e );
                ADDQWORD   ( pak, thisclient->CharInfo->Zulies );
                ADDBYTE    ( pak, 0x00 );
                thisclient->client->SendPacket( &pak );
            }
        }
        break;          
        case 3: //Give Stats/Skill points
        {
            switch( thisquest->value1 )
            {
                case 1: // Give Stats points
                    thisclient->CharInfo->StatPoints += thisquest->value2;                
                    RESETPACKET( pak, 0x720 );
                    ADDWORD    ( pak, 0x0020 );
                    ADDDWORD   ( pak, thisquest->value2 );
                    thisclient->client->SendPacket( &pak );                
                break;
                case 2: // Give Skill points
                    thisclient->CharInfo->SkillPoints += thisquest->value2;
                break;
            }
        }
        break;
        case 4: //Restart Stats/Skills points
            switch( thisquest->value1 )
            {
                case 1: // Stats points [fixed by Geobot]
                    thisclient->CharInfo->StatPoints = 0;
                    for(UINT i=2;i<=thisclient->Stats->Level;i++)
                        thisclient->CharInfo->StatPoints += 10 + (i/2);
                    thisclient->Attr->Str = 15;
                    thisclient->Attr->Dex = 15;
                    thisclient->Attr->Int = 15;
                    thisclient->Attr->Con = 15;
                    thisclient->Attr->Cha = 10;
                    thisclient->Attr->Sen = 10;                    
                break;
                case 2: // Skill points

                break;
            }         
        break;
        case 10: //Delete a quest
            if( thisquest->value1==0 )
                return true;    
            CQuest* athisquest = GetQuestByID( thisquest->value1 );
            QUESTS* myquest = thisclient->GetQuestByQuestID( athisquest->questid ); 
            if( myquest!=0 )
                myquest->active = false;               
        break;
    }    
    return true;
}*/

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

// Clan Manager
bool CWorldServer::pakClanManager ( CPlayer* thisclient, CPacket* P )
{
    int action = GETBYTE((*P),0);
    switch(action)
    {
        case 0xf0:
        {
            //LMA: is this one really used at all?
            MYSQL_ROW row;
            /*int charid = GETWORD((*P),1);
            int clanid = GETWORD((*P),3);*/
            DWORD charid = GETDWORD((*P),1);
            int clanid = GETWORD((*P),5);

            CPlayer* otherclient = GetClientByCID ( charid );
            if(otherclient==NULL)
                return true;
    	    MYSQL_RES *result = DB->QStore("SELECT logo,back,name,grade FROM list_clan where id=%i", clanid);
    	    if(result==NULL) return true;
        	if(mysql_num_rows(result)!=1)
        	{
                Log(MSG_WARNING, "Invalid clan %i", clanid );
                DB->QFree( );
          	    return true;
            }
            row = mysql_fetch_row(result);
        	otherclient->Clan->logo = atoi(row[0]);
    	    otherclient->Clan->back = atoi(row[1]);
    	    strcpy(otherclient->Clan->clanname,row[2]);
    	    otherclient->Clan->grade = atoi(row[3]);
        	DB->QFree( );
            BEGINPACKET( pak, 0x7e0 );
            ADDBYTE    ( pak, 0x35 );//funcion
            ADDWORD    ( pak, otherclient->clientid );//cleint id
            ADDWORD    ( pak, clanid );//?
            ADDWORD    ( pak, 0x0000 );//?
            ADDWORD    ( pak, otherclient->Clan->back );//?
            ADDWORD    ( pak, otherclient->Clan->logo );//?
            ADDBYTE    ( pak, otherclient->Clan->grade );
            ADDBYTE    ( pak, otherclient->Clan->clanrank);
            ADDSTRING  ( pak, otherclient->Clan->clanname );
            ADDBYTE    ( pak, 0x00 );
            SendToVisible( &pak, otherclient );
            Log(MSG_INFO,"[WS] pakClanManager 0x7e1, case 0xf0 (new member too?) %s",otherclient->CharInfo->charname);
        }
        case 0xfa://new member added
        {
        	MYSQL_ROW row;
            /*int charid = GETWORD((*P),1);
            int clanid = GETWORD((*P),3);*/
            DWORD charid = GETDWORD((*P),1);
            int clanid = GETWORD((*P),5);

            CPlayer* otherclient = GetClientByCID ( charid );
            if(otherclient==NULL)
                return true;
    	    MYSQL_RES *result = DB->QStore("SELECT logo,back,name,grade FROM list_clan where id=%i", clanid);
    	    if(result==NULL) return true;
        	if(mysql_num_rows(result)!=1)
        	{
                Log(MSG_WARNING, "Invalid clan %i", clanid );
                DB->QFree( );
          	    return true;
            }

            row = mysql_fetch_row(result);
        	otherclient->Clan->logo = atoi(row[0]);
    	    otherclient->Clan->back = atoi(row[1]);
    	    strcpy(otherclient->Clan->clanname,row[2]);
    	    otherclient->Clan->grade = atoi(row[3]);
        	DB->QFree( );
            otherclient->Clan->clanid=clanid;
            otherclient->Clan->clanrank=1;
            BEGINPACKET( pak, 0x7e0 );
            ADDBYTE    ( pak, 0x35 );//funcion
            ADDWORD    ( pak, otherclient->clientid );//cleint id
            ADDWORD    ( pak, clanid );//?
            ADDWORD    ( pak, 0x0000 );//?
            ADDWORD    ( pak, otherclient->Clan->back );//?
            ADDWORD    ( pak, otherclient->Clan->logo );//?
            ADDBYTE    ( pak, otherclient->Clan->grade );
            ADDBYTE    ( pak, otherclient->Clan->clanrank );
            ADDSTRING  ( pak, otherclient->Clan->clanname );
            ADDBYTE    ( pak, 0x00 );
            SendToVisible( &pak, otherclient );
            Log(MSG_INFO,"[WS] pakClanManager 0x7e1, new member %s",otherclient->CharInfo->charname);
        }
        break;
        case 0xfb://Member Kicked
        {
           char nick[30];
           memcpy( nick, &P->Buffer[1], P->Size );
           CPlayer* otherclient = GetClientByCharName( nick );
           if(otherclient!=NULL)
           {
                otherclient->Clan->clanid=0;
                otherclient->Clan->clanrank=1;
	            otherclient->Clan->back=0;
	            otherclient->Clan->logo=0;
	            otherclient->Clan->grade=0;
	            strcpy(otherclient->Clan->clanname,"");
                BEGINPACKET( pak, 0x7e0 );
                ADDBYTE    ( pak, 0x35 );
                ADDWORD    ( pak, otherclient->clientid );
                ADDQWORD   ( pak, 0 );
                ADDWORD    ( pak, 0x0001 );
                SendToVisible( &pak, otherclient );
                Log(MSG_INFO,"[WS] pakClanManager 0x7e1, member kicked, %s",nick);
           }
        }
        break;
        case 0xfc://member change rank
        {
           char nick[30];
           int newrank = GETBYTE((*P),1);
           memcpy( nick, &P->Buffer[2], P->Size );
           CPlayer* otherclient = GetClientByCharName( nick );
           if(otherclient!=NULL)
           {
                otherclient->Clan->clanrank = newrank;
                Log(MSG_INFO,"[WS] pakClanManager 0x7e1, change rank for %s to %i",nick,newrank);
           }
        }
        break;
        case 0xfd://disorg
        {
            unsigned int clanid = GETWORD((*P),1);
            //unsigned int charid = GETWORD((*P),3);
            DWORD charid = GETDWORD((*P),3);
            CPlayer* tclient = GetClientByCID( charid );
            if(tclient==NULL)
                return true;
            tclient->Clan->clanid = 0;
            tclient->Clan->clanrank = 1;
            tclient->Clan->grade = 0;
            tclient->Clan->back = 0;
            tclient->Clan->logo = 0;
            memset( &tclient->Clan->clanname, '\0', 17 );
            BEGINPACKET( pak, 0x7e0 );
            ADDBYTE    ( pak, 0x35 );
            ADDWORD    ( pak, tclient->clientid );
            ADDQWORD   ( pak, 0 );
            ADDWORD    ( pak, 0x0001 );
            SendToVisible( &pak, tclient );
            Log(MSG_INFO,"[WS] pakClanManager 0x7e1, disorg");
        }
        break;
        case 0xfe://Member Leave
        {
           char nick[17];
           memcpy( nick, &P->Buffer[1], P->Size );
           CPlayer* otherclient = GetClientByCharName(nick);
           if(otherclient!=NULL)
           {
                otherclient->Clan->clanid=0;
                otherclient->Clan->clanrank=0;
	            otherclient->Clan->back=0;
	            otherclient->Clan->logo=0;
	            otherclient->Clan->grade=0;
	            strcpy(otherclient->Clan->clanname,"");
                BEGINPACKET( pak, 0x7e0 );
                ADDBYTE    ( pak, 0x35 );
                ADDWORD    ( pak, otherclient->clientid );
                ADDQWORD   ( pak, 0 );
                ADDWORD    ( pak, 0x0001 );
                SendToVisible( &pak, otherclient );
                Log(MSG_INFO,"[WS] pakClanManager 0x7e1, member left");
           }
        }
        break;
        case 0xff: // update clan mark
        {
            unsigned int clanid = GETWORD((*P),1);
            unsigned int clanlogo = GETDWORD((*P), 3 );
            for(unsigned int i=0;i<ClientList.size();i++)
            {
                if(ClientList.at(i)->player==NULL) continue;
                CPlayer* player = (CPlayer*)ClientList.at(i)->player;
                if(player->Clan->clanid==clanid)
                {
                    player->Clan->back = 0;
                    player->Clan->logo = clanlogo;
                    BEGINPACKET( pak, 0x7e0 );
                    ADDBYTE    ( pak, 0x35 );//funcion
                    ADDWORD    ( pak, player->clientid );//cleint id
                    ADDWORD    ( pak, clanid );//?
                    ADDWORD    ( pak, 0x0000 );//?
                    ADDWORD    ( pak, player->Clan->back );//?
                    ADDWORD    ( pak, player->Clan->logo );//?
                    ADDBYTE    ( pak, player->Clan->grade );
                    ADDBYTE    ( pak, player->Clan->clanrank );
                    ADDSTRING  ( pak, player->Clan->clanname );
                    ADDBYTE    ( pak, 0x00 );
                    SendToVisible( &pak, player );
                    Log(MSG_INFO,"[WS] pakClanManager 0x7e1, update clan mark");
                }
            }
        }
        break;
        default:
            Log( MSG_INFO, "[WS] Clan manager 0x7e1 unknown action %i", action );
    }
    return true;
}

// Create a new clan
bool CWorldServer::pakCreateClan ( CPlayer* thisclient, CPacket* P )
{
    if(thisclient->CharInfo->Zulies<1000000)
        return true;
    thisclient->CharInfo->Zulies -= 1000000;
	//MYSQL_ROW row;
    int background = GETWORD((*P),1);
    int icon = GETWORD((*P),3);
    //char *name = "";
    //char *slogan = "";
    char *name=(char*)&P->Buffer[5];
    char *slogan=(char*)&P->Buffer[strlen(name)+6];

    //LMA: Check if name already exists and is short enough.
    if(!CheckValidName(thisclient,name))
    {
        return true;
    }

    //LMA: Checking if it needs to be escaped.
    if(!CheckEscapeMySQL(name,50,true))
    {
        return true;
    }

    //LMA: checking if the name is ok.
	MYSQL_RES *result = DB->QStore("SELECT name FROM list_clan WHERE name='%s'", name);
    if(result==NULL) return true;
	if ( mysql_num_rows( result ) > 0 )
    {
        BEGINPACKET( pak, 0x07e0 );
	    ADDWORD    ( pak, 0x42 );
		thisclient->client->SendPacket( &pak );
		DB->QFree( );
		return true;
	}
    DB->QFree( );

    //Check if user can create a clan
    if( thisclient->Clan->clanid != 0 )
    {
        BEGINPACKET( pak, 0x07e0 );
	    ADDWORD    ( pak, 0x44 );
		thisclient->client->SendPacket( &pak );
		return true;
	}

    //Add clan to sql table
    //LMA: inserting slogan now
    char * lma_slogan = new char[strlen(slogan) + 1];
    //char lma_slogan[200]="";
	strcpy(lma_slogan,slogan);
	char * new_slogan = new char[strlen(lma_slogan)*3 +1];
	mysql_real_escape_string(DB->Mysql, new_slogan,lma_slogan,strlen(lma_slogan));
	//mysql_real_escape_string(DB->Mysql, new_slogan,"abcdefghijk",11);
    Log(MSG_INFO,"[WS] New clan %s created, slogan detected=%s, cleaned=%s ",name,slogan,new_slogan);

    //LMA: checking slogan's size.
    if(strlen(new_slogan)>=100)
    {
        Log(MSG_HACK,"%s tried to create clan %s with too long slogan %s (%i>=100)",thisclient->CharInfo->charname,name,new_slogan,strlen(new_slogan));
        return true;
    }

    DB->QExecute( "INSERT into list_clan (logo,back,name,cp,grade,slogan,news) values (%i,%i,'%s',0,1,'%s','')",icon,background,name,new_slogan);
	thisclient->Clan->clanid = mysql_insert_id(DB->Mysql);
	thisclient->Clan->clanrank = 6;
    thisclient->Clan->logo = icon;
    thisclient->Clan->back = background;
    strncpy(thisclient->Clan->clanname,name,16);
    thisclient->Clan->grade = 1;

    //Update user clan information
    DB->QExecute( "UPDATE characters set clanid=%i,clan_rank=6 where id=%i",
                                    thisclient->Clan->clanid,thisclient->CharInfo->charid);
    //load clan info in char server
	BEGINPACKET( pak, 0x7e0 );
	ADDBYTE    ( pak, 0xfa ); //action to update clan informacion (charserver)
	ADDWORD    ( pak, thisclient->Clan->clanid );
	//ADDWORD    ( pak, thisclient->CharInfo->charid );
	ADDDWORD    ( pak, thisclient->CharInfo->charid );
	ADDWORD    ( pak, thisclient->clientid );
	cryptPacket( (char*)&pak, cct );
	send( csock, (char*)&pak, pak.Size, 0 );

    //Send to other players
    RESETPACKET( pak, 0x7e0 );
    ADDBYTE    ( pak, 0x35 );
    ADDWORD    ( pak, thisclient->clientid );
    ADDWORD    ( pak, 0x081b );//???
    ADDWORD    ( pak, 0x0000 );//???
    ADDWORD    ( pak, background );
    ADDWORD    ( pak, icon );
    ADDBYTE    ( pak, 0x01 );//clan grade
    ADDBYTE    ( pak, 0x06 );//clan rank
    ADDSTRING  ( pak, name );
    ADDBYTE    ( pak, 0x00 );
    SendToVisible( &pak, thisclient );

    delete[] lma_slogan;
    delete[] new_slogan;


    return true;
}

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
#include "loginserver.h"

// Send Server encryption
bool CLoginServer::pakEncryptionRequest( CLoginClient* thisclient, CPacket* P )
{
	BEGINPACKET( pak, 0x7ff );
	ADDWORD    ( pak, 0xaf02 );
	ADDWORD    ( pak, 0xbd46 );
	ADDWORD    ( pak, 0x0009 );
	ADDWORD    ( pak, 0x0012 );
	ADDBYTE    ( pak, 0x0000 );
	ADDDWORD   ( pak, 0xcdcdcdcd );
	ADDDWORD   ( pak, 0xcdcdcdcd );
	ADDDWORD   ( pak, 0xcdcdcdcd );
	ADDDWORD   ( pak, 0xcdcdcdcd );
	ADDWORD    ( pak, 0xcdcd );
	ADDBYTE    ( pak, 0xd3 );
	thisclient->SendPacket( &pak );
	Log(MSG_DEBUG, "Sent a 0x7ff packet to client");
    return true;
}

// Packet when user login (chck user and pass and active)
bool CLoginServer::pakUserLogin( CLoginClient* thisclient, CPacket* P )
{
    if ( thisclient->isLoggedIn ) return false;
    MYSQL_ROW row;

    //LMA: new way.
    //thisclient->username.assign( (const char*)&P->Buffer, 32, (P->Size-6-32)>16?16:P->Size-6-32 );
    string temp_string;
    string temp_password;
    temp_string.reserve(17);
    temp_password.reserve(33);
    temp_string.assign( (const char*)&P->Buffer, 32, (P->Size-6-32)>16?16:P->Size-6-32 );
    //LMA: end.

    //thisclient->password.assign( (const char*)&P->Buffer, 0, 32 );
    temp_password.assign((const char*)&P->Buffer, 0, 32 );
    EscapeMySQL(temp_password.c_str(),thisclient->password,-1,true);    //client sends already in MD5 form (or should...)

    if ( !thisclient->hasGameGuard && Config.checkGameGuard )
    {
        //Log(MSG_HACK, "Warning, user [ %s ] tried logging in without gameguard", thisclient->username.c_str());
        Log(MSG_HACK, "Warning, user [ %s ] tried logging in without gameguard", temp_string.c_str());
        BEGINPACKET( pak, 0x708 );
        ADDBYTE( pak, 10 );
        ADDDWORD( pak, 0);
        thisclient->SendPacket( &pak );
        return false;
    }

    BEGINPACKET( pak, 0x708 );

    //LMA: Escaping.
    /*char * lma_username = new char[temp_string.size() + 1];
    strcpy(lma_username,temp_string.c_str());
    char * new_username = new char[temp_string.size()*3 +1];
    mysql_real_escape_string(DB->Mysql, new_username,lma_username,strlen(lma_username));
    thisclient->username.assign(new_username);*/
    //LMA: end escaping

    //LMA: new way.
    if(!EscapeMySQL(temp_string.c_str(),thisclient->username,16,true))
    {
        return false;
    }

    MYSQL_RES *result = DB->QStore( "SELECT id,password,accesslevel,online,active FROM accounts WHERE username='%s'", thisclient->username.c_str() );
    if(result==NULL)
    {
        /*delete[] lma_username;
        delete[] new_username;*/
        return false;
    }

    /*delete[] lma_username;
    delete[] new_username;*/

    if( mysql_num_rows( result ) == 1 )
    {
        row = mysql_fetch_row(result);
        int res = 0;
        #ifdef _WIN32
            res = _stricmp( row[1], thisclient->password.c_str() );
        #else
            res = strcasecmp( row[1], thisclient->password.c_str() );
        #endif
        if ( res == 0 )
        {
            //Account is Active
           if(atoi(row[4])==1)
            {
                // Activation Fix By Rifke
                Log( MSG_INFO, "Success login '%s' : Account verfified.", thisclient->username.c_str() );

                // characters is already logged
                if(atoi(row[3])==1)
                {
                    Log(MSG_WARNING, "Account %s try re-login", thisclient->username.c_str() );
                    ADDBYTE( pak, 4 );
                    ADDDWORD( pak, 0 );
                    thisclient->SendPacket( &pak );

                    //relogin crash fixed by zrose.
                    BEGINPACKET( pak2, 0x502 );
                    ADDBYTE    ( pak2, 1 );
                    ADDDWORD   ( pak2, atoi(row[0]) );
                    //cryptPacket( (char*)&pak2, NULL );

                    DB->QFree( );
                    for(UINT i=0;i<ServerList.size();i++)
                        send( ServerList.at(i)->sock , (char*)&pak2, pak2.Size, 0 );

                    DB->QExecute( "update accounts set online=0 WHERE username='%s'", thisclient->username.c_str());
                    return false;
                }

                thisclient->accesslevel = atoi(row[2]);

                //LMA: banned handled after this check.
                if( thisclient->accesslevel>0&&thisclient->accesslevel < Config.MinimumAccessLevel )
                { //The server are under inspection
                    ADDBYTE( pak, 0 );
                    ADDDWORD( pak, 0 );
                    thisclient->SendPacket( &pak );
                    DB->QFree( );
                    return true;
                }

                if ( thisclient->accesslevel > 0 )
                {
                    thisclient->userid = atoi(row[0]);
                    thisclient->isLoggedIn = true;
                    DB->QFree( );
                    //OK!

                    //LMA: Xadet "patch" for GM tricks (Alt+5, Alt+K)
                    //Old code.
                    /*ADDDWORD( pak, 0x0c000000 );
                    ADDBYTE( pak, 0 );*/

                    //new code.
                    ADDBYTE( pak, 0x00 );

                    if(thisclient->accesslevel==300)
                    {
                        //GM level 1
                        ADDWORD( pak, 256 );
                    }
                    else if(thisclient->accesslevel==400)
                    {
                        //GM level 2
                        ADDWORD( pak, 512 );
                    }
                    else if(thisclient->accesslevel==500)
                    {
                        //admin.
                        ADDWORD( pak, 768 );
                    }
                    else
                    {
                        //standard user.
                        ADDWORD( pak, 100 );
                    }

                    ADDWORD( pak, 0x00 );

                    result = DB->QStore( "SELECT id,name FROM channels WHERE type=1" );
                    if(result==NULL) return false;
                    while( row = mysql_fetch_row(result) )
                    {
                        ADDBYTE( pak, 48 + atoi( row[0] ) );
                        ADDSTRING( pak, row[1] );
                        ADDBYTE( pak, 0 );
                        ADDBYTE(pak, atoi( row[0] ) );
                        ADDBYTE( pak, 0 );
                        ADDWORD( pak, 0 );
                    }
                    DB->QFree( );
                }else{
                    //BANNED
                    ADDBYTE( pak, 5 );
                    ADDDWORD( pak, 0 );
                    DB->QFree( );
                }
            }else{
                // Not activated
                Log( MSG_INFO, "Failed login attempt '%s' : Account not verified ", thisclient->username.c_str() );
                ADDBYTE( pak, 9 );
                ADDDWORD( pak, 0 );
                DB->QFree( );
            }
        }else{
            //BAD PASSWORD
            ADDBYTE( pak, 3 );
            ADDDWORD( pak, 0 );
            DB->QFree( );
        }
    }
    else
    {
        DB->QFree( );
        if(Config.CreateLoginAccount)
        {
            if(!DB->QExecute("INSERT into accounts (username,password,accesslevel) values ('%s','%s',100)",thisclient->username.c_str(),thisclient->password.c_str()))
               return true;
            Log( MSG_INFO, "New Account created '%s'", thisclient->username.c_str() );
        }
        //BAD USERNAME
        ADDBYTE( pak, 2 );
        ADDDWORD( pak, 0 );
    }

    /*
    1 - general error   | 4 - your account is already logged
    6 - topup account   | 7 - cannot connect to server please try again
    8 - server exceeded | 9 - account have not been verified
    10 - login failed   | 11 - ip capacity is full
    */

    thisclient->SendPacket ( &pak );
    return true;
}

// Send server list
bool CLoginServer::pakGetServers( CLoginClient* thisclient, CPacket* P )
{
	if( !thisclient->isLoggedIn ) return false;
	MYSQL_ROW row;
	DWORD servernum = GETDWORD( (*P), 0 );
	MYSQL_RES *result = DB->QStore( "SELECT id,name,connected,maxconnections FROM channels WHERE owner=%i and type=2", servernum );
	if(result==NULL) return false;
	BEGINPACKET( pak, 0x704 );
	ADDDWORD   ( pak, servernum );
	ADDBYTE    ( pak, (BYTE)mysql_num_rows( result ) );
	while(row = mysql_fetch_row(result))
    {
    	UINT connected = atoi(row[2]);
    	UINT maxconnections = atoi(row[3]);
    	BYTE id = atoi( row[0] );
    	string name = row[1];
    	//BYTE status = (((100 * connected) / maxconnections==0?1:maxconnections) / 5) & 0xff;
    	unsigned int status = (connected * 100) / maxconnections;
		ADDWORD( pak, id );
		ADDBYTE( pak, 0 );
		ADDWORD( pak, status );
		ADDSTRING( pak, name.c_str() );
		ADDBYTE( pak, 0 );
	}
	DB->QFree( );
	thisclient->SendPacket ( &pak );
    return true;
}

// Send server IP
bool CLoginServer::pakGetIP( CLoginClient* thisclient, CPacket* P )
{
	if (!thisclient->isLoggedIn) return false;
	;
	MYSQL_ROW row;
	DWORD servernum = GETDWORD( (*P), 0 );
	BYTE channelnum = GETBYTE( (*P), 4 );

	BEGINPACKET( pak, 0x70a );

	if(!DB->QExecute( "UPDATE accounts SET lastsvr=%i,lastip='%s',lasttime=UNIX_TIMESTAMP( NOW() ) WHERE id=%i", channelnum, inet_ntoa( thisclient->clientinfo.sin_addr ), thisclient->userid))
	   return false;
	MYSQL_RES *result = DB->QStore( "SELECT host,port,lanip,lansubmask,connected,maxconnections FROM channels WHERE id=%i and type=1", servernum );
    if(result==NULL) return false;
	if(mysql_num_rows(result)!=1)
	{
        Log(MSG_WARNING, "Player selected a invalid channel or channel offline" );
        DB->QFree( );
        return true;
    }
	row = mysql_fetch_row(result);
	UINT connected = atoi(row[4]);
	UINT maxconnections = atoi(row[5]);
	ADDBYTE( pak, 0 );
	ADDDWORD( pak, thisclient->userid );
	ADDDWORD( pak, 0x87654321 );
	if(strcmp(thisclient->ClientSubNet,row[3])==0)//from lan
	{
        ADDSTRING( pak, row[2] );
       	Log(MSG_INFO, "Lan: %s choice channel #%i", thisclient->username.c_str(), channelnum);
    }
    else if(strcmp( thisclient->ClientSubNet ,"127.0.0")==0)//same computer
    {
        ADDSTRING( pak, "127.0.0.1" );
       	Log(MSG_INFO, "Server: %s choice channel #%i", thisclient->username.c_str(), channelnum);
    }
    else
    {
        ADDSTRING( pak, row[0] );
       	Log(MSG_INFO, "Inet: %s choice channel #%i", thisclient->username.c_str(), channelnum);
    }
	ADDBYTE( pak, 0 );
	ADDWORD( pak, atoi(row[1]) );
	DB->QFree( );
	thisclient->SendPacket ( &pak );
    return true;
}

// Connect user to charserver
bool CLoginServer::pakConnectToChar( CLoginClient* thisclient, CPacket *P )
{
	if(thisclient->isLoggedIn) return true;
	if(GETDWORD((*P),0) != Config.LoginPass)
    {
        Log( MSG_HACK, "Charserver Tried to connect loginserver with wrong password ");
        return true;
    }
    thisclient->userid = GETDWORD((*P), 4); //our serverid
    thisclient->accesslevel = 0xffff; //this will identify thisclient as server

    // add new server
    CServers* newserver = new (nothrow) CServers( );
    if(newserver==NULL)
        return false;
    newserver->id = GETDWORD((*P), 4);
    newserver->ip = inet_ntoa( thisclient->clientinfo.sin_addr );
    newserver->port = GETWORD((*P),8 );
    Log( MSG_INFO, "Server #%i connected from ip %s", newserver->id, newserver->ip );

    // Connect to charserver
	newserver->sock = socket( AF_INET, SOCK_STREAM, 0 );
	if (newserver->sock == INVALID_SOCKET)
    {
		Log( MSG_WARNING, "Could not access char server IP: %s , Port: %i", newserver->ip, newserver->port );
		delete newserver;
		return false;
	}
	struct sockaddr_in ain;
	ain.sin_family		= AF_INET;
   	ain.sin_addr.s_addr	= thisclient->clientinfo.sin_addr.s_addr;
	ain.sin_port		= htons( newserver->port );
	if ( connect( newserver->sock , (SOCKADDR*) &ain, sizeof(ain) ) == SOCKET_ERROR)
    {
		Log( MSG_WARNING, "Could not access char server IP: %s , Port: %i", newserver->ip , newserver->port );
		delete newserver;
		return false;
	}
    ServerList.push_back( newserver );
    // Identify packet
    thisclient->isLoggedIn = true;
    BEGINPACKET( pak, 0x501 );
    ADDDWORD   ( pak, Config.CharPass );
    //cryptPacket( (char*)&pak, NULL );
    send( newserver->sock, (char*)&pak, pak.Size, 0 );
    return true;
}

bool CLoginServer::pakGameGuard( CLoginClient* thisclient, CPacket *P )
{
    thisclient->hasGameGuard = true;
    return true;
}

//LMA: escaping.
bool CLoginServer::EscapeMySQL(const char* data,string & mystring,int nb_car_max,bool check_same)
{
    //checking data length
    if(nb_car_max!=-1)
    {
        if (strlen(data)>nb_car_max)
        {
            Log(MSG_WARNING,"Escape:: Data too big (%s) %u>%i",data,strlen(data),nb_car_max);
            return false;
        }
    }

    char * lma_username = new char[strlen(data) + 1];
    strcpy(lma_username,data);
    char * new_username = new char[strlen(data)*3 +1];
    mysql_real_escape_string(DB->Mysql, new_username,lma_username,strlen(lma_username));
    mystring.assign(new_username);
    delete[] lma_username;
    delete[] new_username;

    //Is data escaped the same as the non escaped? Useful for login for example.
    if(strcmp(data,mystring.c_str())!=0)
    {
        Log(MSG_WARNING,"Escape:: is different (%s != %s)",data,mystring.c_str());
        return false;
    }

    return true;
}

//LMA: escaping (this version only checks escaped and unescaped versions of a string are the same).
bool CLoginServer::CheckEscapeMySQL(const char* data,int nb_car_max,bool check_same)
{
    string mystring;

    //checking data length
    if(nb_car_max!=-1)
    {
        if (strlen(data)>nb_car_max)
        {
            Log(MSG_WARNING,"Escape:: Data too big (%s) %u>%i",data,strlen(data),nb_car_max);
            return false;
        }
    }

    char * lma_username = new char[strlen(data) + 1];
    strcpy(lma_username,data);
    char * new_username = new char[strlen(data)*3 +1];
    mysql_real_escape_string(DB->Mysql, new_username,lma_username,strlen(lma_username));
    mystring.assign(new_username);
    delete[] lma_username;
    delete[] new_username;

    //Is data escaped the same as the non escaped? Useful for login for example.
    if(strcmp(data,mystring.c_str())!=0)
    {
        Log(MSG_WARNING,"Escape:: is different (%s != %s)",data,mystring.c_str());
        return false;
    }


    return true;
}


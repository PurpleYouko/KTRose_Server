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
#ifndef __ROSE_SERVER__
#define __ROSE_SERVER__

#include "../common/sockets.h"
#include "datatype.h"

// Player class
class CLoginClient : public CClientSocket
{
    public:
    	CLoginClient ( );      // Constructor
    	~CLoginClient ( );     // destructor
        // Variables
    	bool isLoggedIn;
    	UINT userid;
    	string username;
    	string password;
    	WORD accesslevel;
    	bool hasGameGuard;
};

// Server class
class CLoginServer : public CServerSocket
{
    public:
    	CLoginServer ( string );  // Constructor
    	~CLoginServer( );                         // Destructor

    	//Functions
        CLoginClient* CreateClientSocket( );      // Create a client
    	void DeleteClientSocket( CClientSocket* thisclient ); // Delete a client
    	bool OnReceivePacket( CClientSocket* thisclient, CPacket* P );
        bool OnServerReady(void);
        void LoadConfigurations( char* );
        bool isBanned( sockaddr_in* ClientInfo );

    	//LMA BEGIN
    	//20070623, 224500
    	bool Ping();             //MySQL Ping
    	//LMA END

	   // Packets
    	bool pakEncryptionRequest( CLoginClient* thisclient, CPacket* P );
        bool pakConnectToChar( CLoginClient* thisclient, CPacket *P );
    	bool pakUserLogin( CLoginClient* thisclient, CPacket* P );
    	bool pakGetServers( CLoginClient* thisclient, CPacket* P );
    	bool pakGetIP( CLoginClient* thisclient, CPacket* P );
    	bool pakGameGuard( CLoginClient* thisclient, CPacket* P );
        bool EscapeMySQL(const char* data,string & mystring,int nb_car_max=-1,bool check_same=false);   //LMA: used to escape MySQL.
        bool CheckEscapeMySQL(const char* data,int nb_car_max,bool check_same);   //LMA: checking escaping.

        // Variables
        string   filename;
        vector<CServers*>          ServerList;            // list of channels;

        CDatabase *DB;
};

void StartSignal( );
void StopSignal( );
void HandleSignal( int num );
extern class CLoginServer* GServer;

#endif




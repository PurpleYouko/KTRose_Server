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
#define REVISION 32
#include "worldserver.h"

#ifdef STATICID
//LMA: Static for CID and PID.
unsigned CWorldServer::last_cid=1;
unsigned CWorldServer::last_pid=1;
#endif

CWorldServer* GServer;

bool PRINT_LOG = true;
unsigned char LOG_THISSERVER;

//LMA: TEST Values
UINT lma_loop=0;
UINT fmmonstertype=0;
UINT ftypeskill=0;
UINT fskill=0;

// Main server function
int main(int argc, char *argv[])
{
	//LMA: Opening log.
	LogHandleSp(1);
	LogDebugPriority(0,true);   //LMA: LogDebug won't appear in Logs or screen.

    srand( (unsigned)time(NULL) );
    srand( rand()*time(NULL) );

    //LMA: TEST DEACTIVATING ERROR TRAPPING
    //Uncomment following line to use "console" mode (control+C).
    //StartSignal( );

	LOG_THISSERVER = LOG_WORLD_SERVER;
    InitWinSocket( );
    string fileconf = "worldserver.conf";
    if(argc>1)
    {
        fileconf = argv[1];
    }
	CWorldServer *server = new (nothrow) CWorldServer( fileconf );
    if(server==NULL)
        return -1;
    pthread_attr_init(&server->at);
    pthread_attr_setdetachstate(&server->at, PTHREAD_CREATE_JOINABLE);
    // Connect mysql
    MYSQL mysql;
	server->DB = new CDatabase( server->Config.SQLServer.pcServer,
	                    server->Config.SQLServer.pcUserName,
	                    server->Config.SQLServer.pcPassword,
	                    server->Config.SQLServer.pcDatabase,
	                    server->Config.SQLServer.pcPort, &mysql );
    server->port = server->Config.CharPort;
	if(server->Config.usethreads)
	   Log( MSG_INFO, "Using Threads" );
	// Start server
	server->port = server->Config.WorldPort;

	if(server->DB->Connect( )==0) Log( MSG_INFO, "Connected to MySQL server" );
	else
	{
        delete server;
        server = NULL;
        #ifdef _WIN32
		system("pause");
		#endif
		return -1;
	}

	server->StartServer();
	// Close server
    pthread_attr_destroy(&server->at);
    server->DB->Disconnect( );
	CloseWinSocket( );
	StopSignal( );
	Log(MSG_INFO, "Cleaning memory, please wait..." );
	delete server;
	server = NULL;

	//LMA: closing log.
	LogHandleSp(0);

	return EXIT_SUCCESS;
}

// -----------------------------------------------------------------------------------------

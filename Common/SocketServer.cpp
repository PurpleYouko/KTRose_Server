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
#include "sockets.h"
#include <math.h>

// Constructor
CServerSocket::CServerSocket( )
{
    ConnectedClients = 0;
}

// Destructor
CServerSocket::~CServerSocket( )
{

}

// Start up our server
bool CServerSocket::StartServer( )
{
	struct sockaddr_in ain;
	sock = socket( AF_INET, SOCK_STREAM, 0 );
	if (sock == INVALID_SOCKET)
    {
		//Log( MSG_FATALERROR, "Could not create a socket" );
		return false;
	}
    int optval = 1;
    if(setsockopt(sock, SOL_SOCKET,SO_KEEPALIVE,(const char*)&optval,sizeof(optval))==-1)
    {
        //Log(MSG_ERROR, "setsockopt:SO_KEEPALIVE" );
    }
    setsockopt(sock, IPPROTO_TCP,TCP_NODELAY,(const char*)&optval,sizeof(optval));
	ain.sin_family		= AF_INET;
	ain.sin_addr.s_addr	= INADDR_ANY;
	ain.sin_port		= htons( port );
	memset(&(ain.sin_zero), '\0', 8);
	if ( bind( sock, (const sockaddr*)&ain, sizeof( struct sockaddr ) ) )
    {
		//Log( MSG_FATALERROR, "Could not bind socket" );
		closesocket( sock );
		sock = INVALID_SOCKET;
		return false;
	}

	if ( listen( sock, SOMAXCONN ) == -1 )
    {
		//Log( MSG_FATALERROR, "Could not listen on socket" );
		closesocket( sock );
		sock = INVALID_SOCKET;
		return false;
	}

	isActive = true;

	if ( !this->OnServerReady( ) )
    {
		//Log( MSG_FATALERROR, "Server could not start" );
		closesocket( sock );
		sock = INVALID_SOCKET;
		isActive = false;
		return false;
	}
	//Log( MSG_INFO, "Server started on port %i and is listening.", port );
	ServerLoop( );
	// Nothing past here is ever really called
	OnServerDie( );
	closesocket( sock );

	return true;
}

// Server is started, lets run our loop :D
void CServerSocket::ServerLoop( )
{
	fd_set		fds;
	int			activity;
    maxfd = 0;
	sockaddr_in	ClientInfo;
	SOCKET		NewSocket;
	timeval		timeout;
	maxfd = sock;
	OnServerStep();

	//LMA BEGIN
	//MySQL Ping (every hour)
	//20070623, 221000
	UINT time_last_ping=clock();
	UINT time_last_client_ping=clock();
	UINT delay_ping=3600000;
	UINT delay_client_ping=5000;    //We do this each 5 seconds.
	//LMA END

	do
	{
        //LMA BEGIN
        //MySQL Ping
        //20070623, 221000
        UINT etime = (UINT)(clock( ) - time_last_ping);
        if(etime>=delay_ping)
        {
            time_last_ping=clock();
            Ping();
        }
        //LMA END

        //LMA: We have to send a ping packets to client...
        etime = (UINT)(clock( ) - time_last_client_ping);
        if(etime>=delay_client_ping)
        {
            PingClients();
            time_last_client_ping=clock();
        }

        timeout.tv_sec = 0;
        timeout.tv_usec = 1000;
        NewSocket = INVALID_SOCKET;
        FD_ZERO( &fds );
		if(!Config.usethreads)
    	   FillFDS( &fds );
		FD_SET( sock, &fds );
		activity = select( maxfd+1, &fds, NULL, NULL, &timeout );
		if ( activity == 0 ) continue;
		if ( activity < 0 && errno != EINTR )
        {
			#ifdef _WIN32
			//Log( MSG_ERROR, "Select command failed. Error #%i", WSAGetLastError() );
			#else
			//Log( MSG_ERROR, "Select command failed. Error #%i", errno );
			#endif
			isActive = false;
		}

		if ( FD_ISSET( sock, &fds ) && ConnectedClients < 1024 )
        {
			int clientinfolen = sizeof( sockaddr_in );
            #ifdef _WIN32
       		NewSocket = accept( sock, (sockaddr*)&ClientInfo, (int*)&clientinfolen );
       		#else
    		NewSocket = accept( sock, (sockaddr*)&ClientInfo, (socklen_t*)&clientinfolen );
            #endif
			// TODO: check if server is full
			if (NewSocket != INVALID_SOCKET)
			{
				if(!isBanned(&ClientInfo))
					AddUser( NewSocket, &ClientInfo );
				else
				{
					//Log( MSG_WARNING, "Banned client tried to connect: %s", inet_ntoa( ClientInfo.sin_addr ) );
					close( NewSocket );
				}
			}
			else
			{
			    #ifdef _WIN32
			    //Log( MSG_ERROR, "Error accepting socket: %i", WSAGetLastError() );
			    #else
			    //Log( MSG_ERROR, "Error accepting socket: %i", errno );
			    #endif
            }
		}
		if(!Config.usethreads)
    		HandleClients( &fds );
	} while( isActive );
}

//LMA BEGIN
//MySQL Ping
//20070623, 224500
bool CServerSocket::Ping()
{
     return true;
}
//LMA END

// Fills out an FDS for the server
void CServerSocket::FillFDS( fd_set* fds )
{
    for(UINT i=0;i<ClientList.size( );i++)
	{
        CClientSocket* client = ClientList.at( i );
        if(client->isActive)
        {
		      FD_SET( (unsigned)client->sock, fds );
		      if(client->sock>maxfd)
		          maxfd=client->sock;
        }
        else
        {
            //Log(MSG_INFO,"CServerSocket::FillFDS sid %i",client->sock);
            DisconnectClient( client );
        }
	}
}

// Handle all our clients
void CServerSocket::HandleClients( fd_set* fds )
{
    for(UINT i=0;i<ClientList.size( );i++)
	{
        CClientSocket* client = ClientList.at( i );
        if(!client->isActive)
        {
            continue;
        }

		if(FD_ISSET( client->sock, fds ))
		{
			if(!client->ReceiveData( ) )
			{
			    //LMA: Log.
			    //Log(MSG_INFO,"Client sid %i is inactive",client->sock);
			    //End of log.

                client->isActive = false;
                DisconnectClient( client );
			}
		}
	}
}

// Handle all our clients
void CServerSocket::PingClients()
{
    for(UINT i=0;i<ClientList.size( );i++)
	{
        CClientSocket* client = ClientList.at( i );
        if(!client->isActive)
        {
            continue;
        }

        //LMA: Sending ping each 30 seconds.
        UINT etime = (UINT)(clock( ) - client->CxTime);
        if(etime>30000)
        {
            BEGINPACKET( pak, 0x700 );
            ADDWORD   ( pak, 0x0000);
            client->SendPacket( &pak );

            client->CxTime=clock();
            //Log(MSG_INFO,"Sending ping to cid %i",client->sock);
        }

	}


	return;
}

// Add a new user to our server
void CServerSocket::AddUser( SOCKET sock, sockaddr_in* ClientInfo )
{
    ConnectedClients++;
	CClientSocket* thisclient = this->CreateClientSocket( );
	if (thisclient==NULL)
    {
		closesocket( thisclient->sock );
		if (thisclient!=0) delete thisclient;
		thisclient=0;
		return;
	}
	thisclient->GS = this;
	thisclient->sock = sock;
	thisclient->isActive = true;
	thisclient->CxTime=clock();

	if (!OnClientConnect( thisclient )) {
		closesocket( thisclient->sock );
		if (thisclient!=0) delete thisclient;
		thisclient=0;
		return;
	}
	//Log( MSG_INFO, "[%i]User connected from %s", thisclient->sock, inet_ntoa( ClientInfo->sin_addr ) );
	thisclient->ClientIP = "";
    thisclient->ClientIP = inet_ntoa( ClientInfo->sin_addr );
    char *tmp;
    memset(&thisclient->ClientSubNet, '\0', 12 );
    sprintf(thisclient->ClientSubNet, "%i.%i.%i",
           (ClientInfo->sin_addr.s_addr )&0xFF, (ClientInfo->sin_addr.s_addr>>8 )&0xFF,
           (ClientInfo->sin_addr.s_addr>>16)&0xFF);
	ClientList.push_back( thisclient );
	if(Config.usethreads)
	{
        pthread_create( &threads[sock], NULL, ClientMainThread, (PVOID)thisclient);
    }
    memcpy( &thisclient->clientinfo, ClientInfo, sizeof(struct sockaddr_in));
}

// Disconnect our user
void CServerSocket::DisconnectClient( CClientSocket* thisclient )
{
    ConnectedClients--;
	OnClientDisconnect( thisclient );
	closesocket( thisclient->sock );
	thisclient->isActive = false;
  	thisclient->sock = INVALID_SOCKET;
    for(UINT i=0;i<ClientList.size( );i++)
    {
        CClientSocket* client = ClientList.at( i );
        if( client == thisclient )
        {
            ClientList.erase( ClientList.begin( ) + i );
            break;
        }
    }

	DeleteClientSocket( thisclient );
}

// This function creates an appropriate client socket
CClientSocket* CServerSocket::CreateClientSocket ( )
{
	CClientSocket* thisclient;
    thisclient = new (nothrow) CClientSocket( );
	return thisclient;
}

// This function deletes an old client socket
void CServerSocket::DeleteClientSocket( CClientSocket* thisclient )
{
	//Log( MSG_INFO, "User disconnected CServerSocket::DeleteClientSocket" );
    //    Log( MSG_INFO, " %s : disconnected" , thisclient->CharInfo->charname);

	delete thisclient;
}

// This function is called just before proccessing clients
void CServerSocket::OnServerStep( )
{
}

// This function is called just before the server starts
bool CServerSocket::OnServerReady( )
{
	return true;
}

// This function is called just before the server dies
void CServerSocket::OnServerDie( )
{
	// DOESNT WORK - DAMN CONSOLE APPS
}

// This function is called, if a client receives data
bool CServerSocket::OnReceivePacket( CClientSocket* thisclient, CPacket *P )
{
	return true;
}

// This function is called, if a client connects
bool CServerSocket::OnClientConnect( CClientSocket* thisclient )
{
	return true;
}

// This function is called, if a client disconnects
void CServerSocket::OnClientDisconnect( CClientSocket* thisclient )
{
}

// return if ip is banned
bool CServerSocket::isBanned( sockaddr_in* ClientInfo )
{
	return false;
}

// Returns the current time/date
unsigned long int CServerSocket::GetServerTime( )
{
    //LMA: This is wrong since the year goes from 1900 and there is a month offset too...
    /*
	// Get time/date and write it to the beginning of the row
	time_t rawtime;							// For time
	struct tm* timeinfo;					//    "
	time	  ( &rawtime );
	timeinfo  = localtime( &rawtime );
	unsigned long int uCurTime = 0;
	uCurTime += (timeinfo->tm_sec         * 1         );
	uCurTime += (timeinfo->tm_min         * 60        );
	uCurTime += (timeinfo->tm_hour        * 3600      );
	uCurTime += (timeinfo->tm_yday        * 86400     );
	uCurTime += ((timeinfo->tm_year-2000) * 86400*366 );
	*/

	unsigned long int uCurTime = time(NULL);

	return uCurTime;
}

void CServerSocket::startConsole( )
{
    pthread_create( &consoleThread, NULL, Console, (PVOID)this );
}

/*void* CServerSocket::Console( PVOID cserver )
{
    //When the console is open, all the servers messages will not be printed (but still will be saved to files)
    Log( MSG_INFO, "Console started." );
    CServerSocket* server = static_cast<CServerSocket*>(cserver);
    bool running = true;
    PRINT_LOG = false;
    while(running)
    {
        char command[100];
        memset( &command,'\0', 100 );
        std::cout << "# ";
        std::cin.getline( command, 100);
        if(strcasecmp( command, "exit" )==0)
        {
            running = false;
        }
        else
        {
            if(!server->handleCommand( command ))
            {
                running = false;
            }

        }

    }
    PRINT_LOG = true;
    Log( MSG_INFO, "Console closed." );
}*/


//LMA: New version of the Console... Handling double Control+C bug and some others.
void* CServerSocket::Console( PVOID cserver )
{
    //When the console is open, all the servers messages will not be printed (but still will be saved to files)
    CServerSocket* server = static_cast<CServerSocket*>(cserver);
    bool running = true;
    PRINT_LOG = false;
    //Log( MSG_INFO, "Console started." );

    while(running)
    {
        char command[100];
        memset( &command,'\0', 100 );
        std::cout << "# " << std::flush;

        std::cin.clear();
        std::cin.sync();
        std::cin.ignore(std::cin.rdbuf()->in_avail(),'\n');
        std::cin.getline( command, 100);
        if(strcmp( command, "exit" )==0)
        {
            running = false;
        }
        else
        {
            if(strlen(command)>0)
            {
                if(!server->handleCommand( command ))
                {
                    running = false;
                }

            }
            else
            {
                running=false;
            }

        }

    }

    std::cin.clear();
    std::cin.sync();
    std::cin.ignore(std::cin.rdbuf()->in_avail(),'\n');
    server->console_started=false;
    PRINT_LOG = true;
    //Log( MSG_INFO, "\nConsole closed." );
	return 0;
}


bool CServerSocket::handleCommand( char* cmd )
{
    if(strcmp(cmd, "close server")==0)
    {
        //Log( MSG_CONSOLE, "Closing server..." );
        isActive = false;
        return false;
    }
    //Log( MSG_CONSOLE, "Unhandled command: %s", cmd );
    return true;
}

#include "WorldServer.h"

bool CWorldServer::handleCommand( char* buffer )
{
	char* command = strtok( buffer , " ");
	if (command==NULL) return true;
	if(strcasecmp( command, "closeserver" )==0)
	{
        Log( MSG_CONSOLE, "Closing server..." );
        isActive = false;
        return false;
    }
    if(strcasecmp( command, "ann" )==0)
    {
        char* from = strtok( NULL , " ");
        if(from==NULL) 
        {
            Log (MSG_CONSOLE, "'ann' command: ann <name> <message>" );        
            return true;
        }
        unsigned msgoffset = strlen(from)+5;
        return pakConsoleAnn( from, (char*)&buffer[msgoffset] );
    }
    else
    if(strcasecmp( command, "help" )==0)
    {
            Log (MSG_CONSOLE, "Console Commands Available:" );        
            Log (MSG_CONSOLE, "'ann' command: ann <name> <message>" );        
            Log (MSG_CONSOLE, "'help' command: returns this list" );        
            return true;
    }
    else
    
        Log( MSG_CONSOLE, "Command not handled" );
    return true;
}

// CONSOLE: Announcment
bool CWorldServer::pakConsoleAnn( char* from, char* message )
{
	BEGINPACKET( pak, 0x702 );
	ADDSTRING( pak, from );
	ADDSTRING( pak, "> " );
	ADDSTRING( pak, message );
	ADDBYTE( pak, 0x00);
	SendToAll( &pak );
	Log( MSG_CONSOLE, "Announcment sent" );
	return true;
}

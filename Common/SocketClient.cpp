/*
    Rose Online Server Emulator
    Copyright (C) 2006,2007 OSRose Team http://osroseon.to.md

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

    developed with Main erose/hrose source server + some change from the original eich source
*/
#include "sockets.h"
#include "seh.hpp"
#include "config.h"

// Constructor
CClientSocket::CClientSocket( )
{
	PacketSize		= 6;		// Starting size
	PacketOffset	= 0;		// No bytes read, yet.
}

// Destructor
CClientSocket::~CClientSocket( )
{

}

// Receive this client's socket
bool CClientSocket::ReceiveData( )
{
	int   ReceivedBytes;
	short BytesToRead;

	// Calculate bytes to read to get the full packet
	BytesToRead = PacketSize - PacketOffset;
	// This should never happen, but it is integrated:
	if ( BytesToRead > 0x400 - PacketOffset ) return false;
	if ( BytesToRead == 0 ) return false;

	// Receive data from client
	ReceivedBytes = recv( sock, (char*)&Buffer[ PacketOffset ], BytesToRead, 0 );
	if ( ReceivedBytes <= 0 ) return false;

	// Update pointer
	PacketOffset += ReceivedBytes;

	// If the packet is not complete, leave the function
	if ( ReceivedBytes != BytesToRead ) return true;

	if ( PacketSize == 6 )
	{
		// We received the headerblock
#ifndef USE124
		PacketSize = DecryptBufferHeader( &CryptStatus, CryptTable, Buffer );
#else
		CPacket* header = (CPacket*)&Buffer;
		PacketSize = header->Size;
#endif

		// Did we receive an incorrect buffer?
		if ( PacketSize < 6 )
		{
			Log(MSG_ERROR, "(SID:%i) Client sent incorrect blockheader.", sock );
			return false;
		}

		// Is the packet larger than just the header, then continue receiving
		if ( PacketSize > 6 ) return true;
	}

#ifndef USE124
	// We received the whole packet - Now we try to decrypt it
	if ( !DecryptBufferData( CryptTable, Buffer ) )
	{
		Log(MSG_ERROR, "(SID:%i) Client sent illegal block.", sock );
		return false;
	}
#else
  cryptPacket( (char*)Buffer, this->ct);
#endif
   CPacket* pak = (CPacket*)&Buffer;

	//comment to prevent packet logging
	FILE *fh = fopen(  "log/inoutpackets.log", "a+" );
	if ( fh != NULL ) {
		fprintf( fh, "(SID:%08u) IN  %04x: ", sock, pak->Command );
		for ( int i=0; i<pak->Size-6; ++i ) fprintf( fh, "%02x ", (unsigned char)pak->Buffer[i] );
		fprintf( fh, "\n" );
		fclose( fh );
	}

	// Handle actions for this packet
	if ( !GS->OnReceivePacket( this, pak ) )
	{
         //Log(MSG_ERROR, "onrecieve packet returned false");
         return false;
    }


	// Reset values for the next packet
	PacketSize   = 6;
	PacketOffset = 0;
	//Log(MSG_DEBUG, "onrecieve packet returned true");
	return true;
}

// Send a packet to this client
void CClientSocket::SendPacket( CPacket *P )
{
	//first log it so we don't have to worry about encryption. Comment to prevent logging
	bool LogFile = true;
	FILE *fh = fopen(  "log/inoutpackets.log", "a+" );


	if ( fh != NULL && LogFile) 
	{
		fprintf( fh, "(SID:%08u) OUT %04x: ", sock, P->Command );
		for ( int i=0; i<P->Size-6; ++i ) fprintf( fh, "%02x ", (unsigned char)P->Buffer[i] );
		fprintf( fh, "\n" );
		fclose( fh );
	}


	// :WARNING: IF WE ADD A CRYPT FUNCTION HERE WE MUST COPY THE
	//             PACKET AND NOT USE THE ORIGINAL, IT WILL FUCK UP
	//             THE SENDTOALL FUNCTIONS

    unsigned char* Buffer2 = (unsigned char*)P;
	unsigned Size = P->Size;
#ifndef USE124
    if(this == NULL)
    {
        Log(MSG_WARNING,"tried to send a packet to a socket that doesn't exist any more");
        return;
    }
    //__seh_try
    //{
    //    EncryptBuffer( CryptTable, Buffer );
    //}
    //__seh_except(info, context)
    //{
    //    Log(MSG_WARNING,"Caught a bad packet %s. Send cancelled", Buffer);
    //    return;
    //}
    //__seh_end
	EncryptBuffer( CryptTable, Buffer2 );
#endif
	send( sock, (char*)Buffer2, Size, 0 );

	//record teh packet that we just sent
	//CPacket* pak = (CPacket*)&Buffer;


}

//-------------------------------------------------------------------------
// Send a packet to a client without encrypting the source packet
//-------------------------------------------------------------------------
void CClientSocket::SendPacketCpy( CPacket *P )
{
	CPacket NewPacket;

	memcpy( &NewPacket, P, sizeof( CPacket ) );
	SendPacket( &NewPacket );
}

// Handle client socket (threads)
PVOID ClientMainThread( PVOID ClientSocket )
{
    CClientSocket* thisplayer = (CClientSocket*) ClientSocket;
	fd_set fds;
    while(thisplayer->isActive)
    {
        FD_ZERO(&fds);
        FD_SET (thisplayer->sock, &fds);
        int Select = select( thisplayer->sock+1, &fds, NULL, NULL, NULL );
        if(Select == SOCKET_ERROR)
        {
            Log( MSG_ERROR,NULL,"Error in Select");
            thisplayer->isActive = false;
        }
        else
        {
            if(FD_ISSET( thisplayer->sock, &fds ))
            {
                if (thisplayer->isserver == true)
                {
                   //Log( MSG_INFO,"ISC PACKET in ClientMainThread");
                   thisplayer->ISCThread();
                }
                else
                if(!thisplayer->ReceiveData( ))
                {
                    thisplayer->isActive = false;
                }
            }
        }

    }
    thisplayer->GS->DisconnectClient( thisplayer );
    pthread_exit(NULL);
	return 0 ;
}

// -----------------------------------------------------------------------------------------

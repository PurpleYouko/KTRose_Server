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

#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <ctype.h>

// Find the entry
bool ConfigGetEntry( const char *pcFile, const char *pcNameMust, char* &pcResult )
{
	char pcBuffer[ 512 ];
	char *pcName, *pcPar, *pcPoint;
	unsigned len;

	pcResult=0;

	// Open file
	FILE *fh;
	fh = fopen(pcFile, "r" );
	if ( fh==NULL ) return false;

	do
	{
		fgets( pcBuffer, 510, fh );
		len = (unsigned)strlen( pcBuffer );
		if ( pcBuffer[ len-1 ] == '\n' ) --len;
		pcBuffer[ len ] = 0;

		pcName = pcBuffer;
		while ( *pcName==' ' || *pcName=='\t' ) ++pcName;
		if ( *pcName==0 || *pcName=='#' ) continue;
		pcPoint = pcName;
		while ( *pcPoint!=' ' && *pcPoint!='\t' && *pcPoint!='#' && *pcPoint!='=' && *pcPoint!=0 ) ++pcPoint;
		if ( *pcPoint==0 || *pcPoint=='#' ) continue;
		pcPar = pcPoint;
		while ( *pcPar==' ' || *pcPar=='\t' || *pcPar=='=' ) ++pcPar;
		*pcPoint=0;

		#ifdef _WIN32
		if ( !_strcmpi( pcName, pcNameMust ) )
		{
			pcResult = _strdup( pcPar );
			pcPoint  = pcResult + strlen( pcPar );
			if ( *pcPoint==0 ) return true;
			--pcPoint;
			while( *pcPoint==' ' || *pcPoint=='\t' ) *pcPoint--=0;
			return true;
		}

		#else
		if ( strcasecmp( pcName, pcNameMust )==0 )
		{
			pcResult = strdup( pcPar );
			pcPoint  = pcResult + strlen( pcPar );
			if ( *pcPoint==0 ) return true;
			--pcPoint;
			while( *pcPoint==' ' || *pcPoint=='\t' ) *pcPoint--=0;
			return true;
		}
		#endif

	} while ( !feof( fh ) );

	// Close file
	fclose( fh );

	return true;
}

// -----------------------------------------------------------------------------------------
// Returns a text
// -----------------------------------------------------------------------------------------
char *ConfigGetString( const char *pcFile, const char *pcName, const char *pcDefault )
{
	char *pcRet, *pcRetReal, *pcRetOld;
	if ( !ConfigGetEntry( pcFile, pcName, pcRet ) ) return strdup( pcDefault );
	if (pcRet==0) return strdup( pcDefault );
	pcRetOld = pcRet;
	if ( *pcRet=='"' )
	{
		++pcRet;
		pcRet[ strlen(pcRet) - 1 ] = 0;
	}

	pcRetReal = strdup( pcRet );
	free( pcRetOld );

	return pcRetReal;
}

// Returns a number
unsigned ConfigGetInt( const char *pcFile, const char *pcName, unsigned uDefault )
{
	char *pcRet;
	unsigned uRet;
	if ( !ConfigGetEntry( pcFile, pcName, pcRet ) ) return uDefault;
	if (pcRet==0) return uDefault;

	uRet = atoi( pcRet );
	free( pcRet );

	return uRet;
}

// -----------------------------------------------------------------------------------------

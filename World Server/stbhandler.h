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

// Written by Brett19
#include "worldserver.h"

struct CSTBData {
	unsigned rowcount;
	unsigned fieldcount;
	int** rows;
};

struct CSTBDataChar {
	unsigned rowcount;
	unsigned fieldcount;
	unsigned long** rows;
};

int STBStoreData( const char* filename, CSTBData* data );
void STBFreeData( CSTBData* data );
int STBStoreDataChar( const char* filename, CSTBDataChar* data ); //LMA: returning unsigned long
void STBFreeDataChar( CSTBDataChar* data );

//LMA: test for quest hack (stackable).
#ifdef QHACK
int STBStoreDataNPC( const char* filename, CSTBDataChar* data ); //LMA: returning unsigned long
#endif
//LMA: end

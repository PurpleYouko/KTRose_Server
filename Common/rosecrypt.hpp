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
#ifndef __ROSE_ENCRYPTION__
#define __ROSE_ENCRYPTION__

#ifdef _WIN32
    #include <windows.h>
#endif
#define allocChecksum() new char[0x200]
#define allocCrypttable() new char[256+8]

// Functions
#ifdef _WIN32
//int buildChecksum( char* csum, char* path = 0 );
void buildCryptTable( char* crypttable, char* checksum, int checksumlen );
#endif
void cryptPacket( char *packet, char *crypttable );


#endif






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
#include "rosecrypt.hpp"

#ifdef _WIN32
// we will not include this in linux
// i haven't ported because we don't need this for the new encryption

// Build trose.exe checks
/*
int buildChecksum(char* csum, char* path)
{
	char buf[0x200];
	int bytesread=0;
	HANDLE fh;

	if (path==0) path = "TRose.exe";
	fh = CreateFileA( path, GENERIC_READ, 0, NULL, OPEN_EXISTING, 80, NULL );
	if ( fh == INVALID_HANDLE_VALUE ) return -1;

	SetFilePointer(fh, -512, NULL, FILE_END);
	ReadFile(fh, buf, 0x200, (LPDWORD)&bytesread, NULL);
	CloseHandle(fh);

	if (bytesread!=0x200) return -2;

	short keylen = buf[0x0f];
	char* key = new char[keylen+1];
	unsigned short key_ct[] = {0x94, 0x20, 0x5F, 0x53, 0x14, 0xF2, 0x61, 0x11, 0x54, 0xFF, 0x72, 0x99, 0x44, 0xF4, 0xA1, 0xAA, 0xBA, 0xC2, 0x54, 0x52, 0x6F, 0x73, 0x65, 0x2E, 0x65, 0x78, 0x65};

	for(int i=0; i<keylen; i++)
		key[i] = key_ct[i] ^ buf[0x18A + i];

	int csumlen=0;
	int multiplier=1;
	for(int i=keylen; i>0; i--) {
		int tmp = (key[i-1]-0x30)*multiplier;
		multiplier *= 0x0a;
		csumlen += tmp;
	}

	delete [] key;

	unsigned short csumtable[] = {0x34, 0x5C, 0x3D, 0x52, 0xCE, 0xF3, 0x12, 0xD4, 0x05, 0x91, 0xEE, 0xFF, 0x49, 0xC2, 0xD2};

	int tmp=0;
	for(int i=0; i<csumlen; i++) {
		csum[i] = csumtable[tmp] ^ buf[0x87 + i];
		if (tmp==0x0e) tmp=0; else tmp++;
	}

	return csumlen;
}
*/

// build crypt table
void buildCryptTable(char* crypttable, char* checksum, int checksumlen)
{
	for (int i=0; i<0x008; i++) crypttable[i]=0;
	for (int i=0; i<0x100; i++) crypttable[i+8]=i;

	int csumpos=0;
	int modifier=0;

	for (int i=0; i<0x40*4; i++) {
			int curpos=crypttable[8+i];

			modifier = (checksum[csumpos] + curpos + modifier) & 0xff;
			crypttable[8+i] = crypttable[modifier+8];
			crypttable[modifier+8] = curpos;

			csumpos=++csumpos%checksumlen;
	}
}

// en/decrypt packet
void cryptPacket(char *packet, char* crypttable )
{
	return; // bypass crypt functions completely
	unsigned short paksize=(*((unsigned short*)&packet[0])) - 2;
	for(int i=2; i<paksize; i++)
    {
		packet[i] = 0x61 ^ packet[i];
	}
}


#endif

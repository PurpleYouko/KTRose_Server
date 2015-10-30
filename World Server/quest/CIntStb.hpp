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

// Props to ExJam for this code :D

class CIntStb
{
public:
	CIntStb() : rows(0) {}
	CIntStb(char* filename) : rows(0){ LoadSTB(filename); }
	~CIntStb(){ CleanUp(); }

	int LoadSTB( char* filename ){
		FILE* fh;
		char tmpfield[512];
		unsigned dataoffset = 0;
		unsigned short fieldlen = 0;

		if ( (fh = fopen( filename, "rb" )) != 0 ) {
			Log( MSG_ERROR, "Could not load STB '%s'", filename );
			return 1;
		}else{
			fseek( fh, 4, SEEK_SET );
			fread( &dataoffset, 4, 1, fh );
			fread( &rowcount, 4, 1, fh );
			fread( &fieldcount, 4, 1, fh );

			rowcount--;
			fieldcount--;

			fseek( fh, dataoffset, SEEK_SET );
			int* tmp = new int[rowcount*fieldcount];
			rows = new int*[rowcount];
			for(dword i = 0; i < rowcount; i++ ) rows[i] = &tmp[i*fieldcount];
			for(dword j = 0; j < rowcount*fieldcount; j++) {
				fread( &fieldlen, 2, 1, fh );
				fread( tmpfield, 1, fieldlen, fh );
				tmpfield[fieldlen]=0;
				tmp[j] = atoi( tmpfield );
			}
			fclose( fh );
		}

		return 0;
	}

	dword rowcount;
	dword fieldcount;
	int** rows;

private:
	void CleanUp(){
		if(rows != NULL){
			delete [] rows[0];
			delete [] rows;
		}
	}
};

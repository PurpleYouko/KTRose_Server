/*
    Rose Online Server Emulator
    Copyright (C) 2006,2007,2008 OSRose Team http://www.dev-osrose.com

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

// -----------------------------------------------------------------------------------------
// STB reading shit
// -----------------------------------------------------------------------------------------
int STBStoreData( const char* filename, CSTBData* data )
{
	char tmpfield[512];
	unsigned dataoffset = 0;
	unsigned short fieldlen = 0;

    FILE* fh = fopen( filename, "rb" );
	if (  fh== 0 ) {
		Log( MSG_ERROR, "Could not load STB '%s'", filename );
		return 1;
	}else{
		// Read the header
		fseek( fh, 4, SEEK_SET );
		fread( &dataoffset, 4, 1, fh );
		fread( &data->rowcount, 4, 1, fh );
		fread( &data->fieldcount, 4, 1, fh );
		data->rowcount--;
		data->fieldcount--;
		// Read the data
		fseek( fh, dataoffset, SEEK_SET );
		int* tmp = new int[data->rowcount*data->fieldcount];
		data->rows = new int*[data->rowcount];
		for( unsigned i = 0; i < data->rowcount; i++ ) data->rows[i] = &tmp[i*data->fieldcount];
		for( unsigned j = 0; j < data->rowcount*data->fieldcount; j++ )
		{
			fread( &fieldlen, 2, 1, fh );
			fread( tmpfield, 1, fieldlen, fh );
			tmpfield[fieldlen]=0;

			//LMA: to get STL Id's as well.
			if (fieldlen>3&&tmpfield[0]==76)
			{

                for(int w=0;w<fieldlen;w++)
                {
                    if(tmpfield[w]>=48&&tmpfield[w]<=57)
                        break;
                    tmpfield[w]=48;
                }

			}
			//LMA: end.

			tmp[j] = atoi( tmpfield );
		}
		fclose( fh );
	}

	return 0;
}

void STBFreeData( CSTBData* data )
{
    //LMA: test to avoid stupid crash id STB not found.
    if(data->rowcount==0)
        return;
	delete [] data->rows[0];
	delete [] data->rows;
}


//LMA: Special for Zone (unsigned long).
int STBStoreDataChar( const char* filename, CSTBDataChar* data )
{
	char tmpfield[512];
	unsigned dataoffset = 0;
	unsigned short fieldlen = 0;
	int current_row=0;
	int current_col=0;

    FILE* fh = fopen( filename, "rb" );
	if (  fh== 0 ) {
		Log( MSG_ERROR, "Could not load STB '%s'", filename );
		return 1;
	}else{
		// Read the header
		fseek( fh, 4, SEEK_SET );
		fread( &dataoffset, 4, 1, fh );
		fread( &data->rowcount, 4, 1, fh );
		fread( &data->fieldcount, 4, 1, fh );
		data->rowcount--;
		data->fieldcount--;
		// Read the data
		fseek( fh, dataoffset, SEEK_SET );
		unsigned long* tmp = new unsigned long[data->rowcount*data->fieldcount];
		data->rows = new unsigned long*[data->rowcount];
		for( unsigned i = 0; i < data->rowcount; i++ ) data->rows[i] = &tmp[i*data->fieldcount];

		for( unsigned j = 0; j < data->rowcount*data->fieldcount; j++ )
		{
			fread( &fieldlen, 2, 1, fh );
			fread( tmpfield, 1, fieldlen, fh );
			tmpfield[fieldlen]=0;

			current_row=(int)(j/data->fieldcount);
			current_col=j-(current_row*data->fieldcount);

            if(current_col==26)
            {
                //STL.
                if (fieldlen>3&&tmpfield[0]==76)
                {

                    for(int w=0;w<fieldlen;w++)
                    {
                        if(tmpfield[w]>=48&&tmpfield[w]<=57)
                            break;
                        tmpfield[w]=48;
                    }

                }
                //LMA: end.
            }
            else if(current_col>=22&&current_col<=24)
            {
                //qsd triggers.
                if(fieldlen==0)
                {
                    tmp[j]=0;
                    continue;
                }

                tmp[j]=MakeStrHash(tmpfield);
                continue;
            }

			tmp[j] = atol( tmpfield );
		}

		fclose( fh );
	}

	return 0;

}

//LMA: For unisnged long
void STBFreeDataChar( CSTBDataChar* data )
{
    //LMA: test to avoid stupid crash id STB not found.
    if(data->rowcount==0)
        return;
	delete [] data->rows[0];
	delete [] data->rows;
}


//LMA: test for quest hack (stackable).
#ifdef QHACK
//LMA: Special for NPCs (unsigned long).
int STBStoreDataNPC( const char* filename, CSTBDataChar* data )
{
	char tmpfield[512];
	unsigned dataoffset = 0;
	unsigned short fieldlen = 0;
	int current_row=0;
	int current_col=0;

    FILE* fh = fopen( filename, "rb" );
	if (  fh== 0 ) {
		Log( MSG_ERROR, "Could not load STB '%s'", filename );
		return 1;
	}else{
		// Read the header
		fseek( fh, 4, SEEK_SET );
		fread( &dataoffset, 4, 1, fh );
		fread( &data->rowcount, 4, 1, fh );
		fread( &data->fieldcount, 4, 1, fh );
		data->rowcount--;
		data->fieldcount--;
		// Read the data
		fseek( fh, dataoffset, SEEK_SET );
		unsigned long* tmp = new unsigned long[data->rowcount*data->fieldcount];
		data->rows = new unsigned long*[data->rowcount];
		for( unsigned i = 0; i < data->rowcount; i++ ) data->rows[i] = &tmp[i*data->fieldcount];

		for( unsigned j = 0; j < data->rowcount*data->fieldcount; j++ )
		{
			fread( &fieldlen, 2, 1, fh );
			fread( tmpfield, 1, fieldlen, fh );
			tmpfield[fieldlen]=0;

			current_row=(int)(j/data->fieldcount);
			current_col=j-(current_row*data->fieldcount);

            if(current_col==40)
            {
                //STL.
                if (fieldlen>3&&tmpfield[0]==76)
                {

                    for(int w=0;w<fieldlen;w++)
                    {
                        if(tmpfield[w]>=48&&tmpfield[w]<=57)
                            break;
                        tmpfield[w]=48;
                    }

                }
                //LMA: end.
            }
            else if(current_col==41)
            {
                //qsd trigger.
                if(fieldlen==0)
                {
                    tmp[j]=0;
                    continue;
                }

                tmp[j]=MakeStrHash(tmpfield);
                continue;
            }

			tmp[j] = atol( tmpfield );
		}

		fclose( fh );
	}

	return 0;

}
#endif

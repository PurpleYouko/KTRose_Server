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

//LMA: Code taken from IFONPCDUMPER from Drakia and adpated.

#include "worldserver.h"


//Reading and storing a STL in a single map :)
int STLStoreData(const char* filename, int family)
{
    entry** index;
    map<UINT, entry> table;
	char tempfield[512];
    UINT entryCount;
    UINT langCount;
    int temp=0;


    FILE* fh = fopen( filename, "rb" );
	if (fh==0)
	{
		Log( MSG_ERROR, "Could not load STL '%s'", filename );
		return 0;
	}

    fread(&temp,1,1,fh);
    fread(tempfield,1,temp,fh);
    fread(&entryCount,4,1,fh);

    index = new entry*[entryCount];
    for (UINT i = 0; i < entryCount; i++)
    {
        entry item;
        temp=0;
        fread(&temp,1,1,fh);
        fread(tempfield,1,temp,fh);
        tempfield[temp]=0;

        for(int k=0;k<temp;k++)
        {
            if(tempfield[k]>=48&&tempfield[k]<=57)
                break;
            tempfield[k]=48;
        }

        item.stringId=atoi(tempfield);
        fread(&item.id,4,1,fh);
        table[item.stringId] = item;
        index[i] = &table[item.stringId];
    }

    vector<UINT> langOffset;
    fread(&langCount,4,1,fh);

    for (UINT i = 0; i < langCount; i++)
    {
        temp=0;
        fread(&temp,4,1,fh);
        langOffset.push_back(temp);
    }


    UINT i=STL_ENGLISH;
    fseek( fh, langOffset.at(i), SEEK_SET );

    for (UINT j = 0; j < entryCount; j++)
    {
      //entry* Entry = index[j];
      language lang;
        temp=0;
        fread(&temp,4,1,fh);
        lang.offset=temp;
      //Entry->lang.push_back(lang);
      index[j]->lang.push_back(lang);
    }

    for (UINT j = 0; j < entryCount; j++)
    {
          entry* Entry = index[j];
          //language *item = &Entry->lang.at(i);
          language *item = &Entry->lang.at(0);
          fseek( fh, item->offset, SEEK_SET );
          temp=0;
          fread(&temp,1,1,fh);

          //no text?
          if(temp==0)
          {
              continue;
          }

          fread(item->text,1,temp,fh);
          item->text[temp]=0;

          //LMA: let's do the map ^_^
          UINT mapid=family*100000+index[j]->stringId;
          if(GServer->STLNameList.find(mapid)!=GServer->STLNameList.end())
          {
              Log(MSG_WARNING,"STL ID %u (line %i) already in map %s",mapid,j,GServer->STLNameList[mapid]);
              continue;
          }

          char* new_text=new char[temp+1];
          memcpy(new_text,item->text,temp);
          new_text[temp]=0;
          GServer->STLNameList[mapid]=new_text;

          //LMA: Trying to get the comment now.
          if(GServer->Config.massexport!=0)
          {
            temp=0;
            fread(&temp,1,1,fh);

            if(temp==0)
            {
                continue;
            }

            char* new_comment=new char[temp+1];
            fread(new_comment,1,temp,fh);
            new_comment[temp]=0;
            GServer->STLCommentList[mapid]=new_comment;
          }

    }

    fclose( fh );
    delete[] index;


    return 1;
}




/*
    Rose Online Server Emulator
    Copyright (C) 2006,2007 OSRose Team http://www.osrose.net

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
#include "player.h"
#include "worldserver.h"

unsigned char GetCharVal( char mychar )
{
	switch( mychar )
	{
		case '0':	return 0;
		case '1':	return 1;
		case '2':	return 2;
		case '3':	return 3;
		case '4':	return 4;
		case '5':	return 5;
		case '6':	return 6;
		case '7':	return 7;
		case '8':	return 8;
		case '9':	return 9;
		case 'a':	return 10;
		case 'b':	return 11;
		case 'c':	return 12;
		case 'd':	return 13;
		case 'e':	return 14;
		case 'f':	return 15;
		case 'A':	return 10;
		case 'B':	return 11;
		case 'C':	return 12;
		case 'D':	return 13;
		case 'E':	return 14;
		case 'F':	return 15;
		default:
            break;
	}
	return 0;
}

void EncodeBinary( char* encoded, unsigned char* data, unsigned datalen )
{
	for( unsigned short i = 0, j = 0; i < datalen; i++ )
	{
		encoded[ j++ ] = "0123456789ABCDEF"[ data[ i ] >> 4 ];
		encoded[ j++ ] = "0123456789ABCDEF"[ data[ i ] & 0xf ];
	}
	encoded[ datalen * 2 ] = 0;
}

void DecodeBinary( char* encoded, unsigned char* data )
{
	unsigned curbit = 0;
	for( unsigned i = 0; i < (unsigned)strlen(encoded); i+=2 )
	{
		data[curbit] = GetCharVal( encoded[i] ) << 4;
		data[curbit] += GetCharVal( encoded[i+1] );
		curbit++;
	}
}

// Load this characters userdata from the database
bool CPlayer::loaddata( )
{
	//Log(MSG_INFO,"Loading character data");
    MYSQL_ROW row;
	MYSQL_RES *result = GServer->DB->QStore("SELECT level,face,hairStyle,sex,classid,zuly,str,dex,_int,con,cha,sen,curHp,curMp,id,statp,skillp,exp,stamina,quickbar,basic_skills,askill,askilllvl,pskill,pskilllvl,respawnid,clanid,clan_rank,townid,isgm,isdev FROM characters WHERE char_name='%s'", CharInfo->charname);
	if(result==NULL) return false;
	if(mysql_num_rows(result)!=1)
	{
        Log( MSG_WARNING, "Number of user with charname '%s' is %i", CharInfo->charname,mysql_num_rows(result));
        return false;
    }
	row = mysql_fetch_row(result);
	Stats->Level = atoi(row[0]);
	CharInfo->Face = atoi(row[1]);
	CharInfo->Hair = atoi(row[2]);
	CharInfo->Sex = atoi(row[3]);
	CharInfo->Job = atoi(row[4]);
	CharInfo->Zulies = atol(row[5]);
	Attr->Str = atoi(row[6]);
	Attr->Dex = atoi(row[7]);
	Attr->Int = atoi(row[8]);
	Attr->Con = atoi(row[9]);
	Attr->Cha = atoi(row[10]);
	Attr->Sen = atoi(row[11]);
	Stats->HP = atoi(row[12]);
	Stats->MP = atoi(row[13]);
	CharInfo->charid = atoi(row[14]);
	CharInfo->StatPoints = atoi(row[15]);
	CharInfo->SkillPoints = atoi(row[16]);
	CharInfo->Exp = atoi(row[17]);
	CharInfo->stamina = atoi(row[18]);
    Position->respawn = atoi(row[25]);
    Clan->clanid = atoi(row[26]);
    Clan->clanrank = atoi(row[27]);
    Position->saved = atoi(row[28]);
    Stats->xprate = GetXPRate();
    CharInfo->IsGM = atoi(row[29]);
    CharInfo->IsDev = atoi(row[30]);
    Log(MSG_DEBUG,"Selected Character %s Class %i GM status: %i DEV status %i", CharInfo->charname, CharInfo->Job, CharInfo->IsGM, CharInfo->IsDev);
	p_skills = 0;
   	for(char i=0;i<32;i++)
       {
             char* tmp=strtok((i==0?row[19]:NULL), ",");
             if (tmp!=NULL) quickbar[i]=atoi(tmp);
              else
            quickbar[i]=0;
       }

	for(char i=0;i<30;i++)
    {
             char* tmp=strtok((i==0?row[20]:NULL), ",");
             if (tmp!=NULL) bskills[i]=atoi(tmp);
              else
            bskills[i]=0;

    }
	for(char i=0;i<30;i++)
    {
              char* tmp=strtok((i==0?row[21]:NULL), ",");
              if (tmp!=NULL) askill[i]=atoi(tmp);
               else
            askill[i]=0;
    }
	for(char i=0;i<30;i++)
    {
             char* tmp=strtok((i==0?row[22]:NULL), ",");
             if (tmp!=NULL) askilllvl[i]=atoi(tmp);
              else
            askilllvl[i]=0;
    }
	for(char i=0;i<30;i++)
    {
             char* tmp=strtok((i==0?row[23]:NULL), ",");
             if (tmp!=NULL) pskill[i]=atoi(tmp);
              else
            pskill[i]=0;
    }
	for(char i=0;i<30;i++)
    {
             char* tmp=strtok((i==0?row[24]:NULL), ",");
             if (tmp!=NULL) pskilllvl[i]=atoi(tmp);
              else
            pskilllvl[i]=0;
    }

    GServer->DB->QFree( );
    CharInfo->Pending_Exp = 0;
    Log(MSG_INFO,"Loading union data");
	result = GServer->DB->QStore("SELECT unionid,union1points,union2points,union3points,union4points,union5points,union6points,union7points,union8points,union9points,union10points FROM characters WHERE char_name='%s'", CharInfo->charname);
	if(result == NULL) return false;
	if(mysql_num_rows(result)!=1)
	{
        Log( MSG_WARNING, "Number of user with charname '%s' is %i", CharInfo->charname,mysql_num_rows(result));
        return false;
    }
	row = mysql_fetch_row(result);
	for (char points=0; points<11;points++)
	{
        if(points == 9)
        {
            Union_s->unionvar[points] = Session->KTPoints;
        }
        else
        {
            Union_s->unionvar[points] = atoi(row[points]);
        }
    }
	GServer->DB->QFree( );
	// code to reset players back to the correct faction after using the KTRose shop
	if(Union_s->unionvar[0] == 9)
	{
        //Log(MSG_DEBUG, "UnionVar 7 used to reset player Union from %i to %i",Union_s->unionvar[0],Union_s->unionvar[7] );
        Union_s->unionvar[0] = Union_s->unionvar[7];
        Union_s->unionvar[7] = 0;
    }
	// PY experimental loadup code
	//Log(MSG_INFO,"Loading inventory data");
    result = GServer->DB->QStore("SELECT itemid,itemtype,refine,durability,lifespan,count,stats,socketed,appraised,trans_action FROM charitems WHERE owner=%i", CharInfo->charid);
    if(result == NULL)
    {
        Log(MSG_WARNING, "Something a bit wrong with this query");
        return false;
    }
    const char *tmpid;
    char *itmp;
    int itemid;
    while( row = mysql_fetch_row(result) )
    {
        tmpid = strtok( row[0] , "|");
        for(unsigned int i=1;i<=MAX_INVENTORY; i++)
        {
            if((itmp = strtok( NULL , "|"))!= NULL)
                items[i].itemnum = atoi(itmp);
        }
	    tmpid = strtok( row[1] , "|");
        for(unsigned int i=1;i<=MAX_INVENTORY; i++)
        {
            if((itmp = strtok( NULL , "|"))!= NULL)
                items[i].itemtype = atoi(itmp);
        }
        tmpid = strtok( row[2] , "|");
        for(unsigned int i=1;i<=MAX_INVENTORY; i++)
        {
            if((itmp = strtok( NULL , "|"))!= NULL)
                items[i].refine = atoi(itmp);
        }
        tmpid = strtok( row[3] , "|");
        for(unsigned int i=1;i<=MAX_INVENTORY; i++)
        {
            if((itmp = strtok( NULL , "|"))!= NULL)
                items[i].durability = atoi(itmp);
        }
        tmpid = strtok( row[4] , "|");
        for(unsigned int i=1;i<=MAX_INVENTORY; i++)
        {
            if((itmp = strtok( NULL , "|"))!= NULL)
                items[i].lifespan = atoi(itmp);
        }
        tmpid = strtok( row[5] , "|");
        for(unsigned int i=1;i<=MAX_INVENTORY; i++)
        {
            if((itmp = strtok( NULL , "|"))!= NULL)
            {
                items[i].count = atoi(itmp);
                //Log(MSG_INFO,"slot: %i itemid: %i type: %i count: %i",i, items[i].itemnum,items[i].itemtype,items[i].count); //PAT items seem to load ok here
            }
        }
        tmpid = strtok( row[6] , "|");
        for(unsigned int i=1;i<=MAX_INVENTORY; i++)
        {
            if((itmp = strtok( NULL , "|"))!= NULL)
            {
                items[i].stats = atoi(itmp);
                if(items[i].stats >300)
                {
                    items[i].gem = items[i].stats;
                }
                else
                    items[i].gem = 0;
            }
        }
        tmpid = strtok( row[7] , "|");
        for(unsigned int i=1;i<=MAX_INVENTORY; i++)
        {
            if((itmp = strtok( NULL , "|"))!= NULL)
                items[i].socketed = (atoi(itmp)==1)?true:false;
            //just in case there is a discrepency in the database this will fix sockets.
            if(items[i].gem != 0)
                items[i].socketed = true;
        }
        tmpid = strtok( row[8] , "|");
        for(unsigned int i=1;i<=MAX_INVENTORY; i++)
        {
            if((itmp = strtok( NULL , "|"))!= NULL)
                items[i].appraised = (atoi(itmp)==1)?true:false;
        }
        for(unsigned int i=1;i<=MAX_INVENTORY; i++)
        {
            // first delete bad items
            if(items[i].itemnum > GServer->EquipList[items[i].itemtype].STBMax)
            {
                Log(MSG_INFO,"deleting item in slot: %i itemid: %i type: %i count: %i",i, items[i].itemnum,items[i].itemtype,items[i].count);
                //Item number too high
                items[i].itemnum = 0;
                items[i].itemtype = 0;
                items[i].count = 0;
                items[i].durability = 0;

            }
                // find item type for crafting
            if(items[i].itemtype > 0 && items[i].itemtype < 10)
                items[i].type = GServer->EquipList[items[i].itemtype].Index[items[i].itemnum]->type;
            if(items[i].itemtype == 10)
                items[i].type = GServer->UseList.Index[items[i].itemnum]->type;
            if(items[i].itemtype == 11)
                items[i].type = GServer->JemList.Index[items[i].itemnum]->type;
            if(items[i].itemtype == 12)
                items[i].type = GServer->NaturalList.Index[items[i].itemnum]->type;
            if(items[i].itemtype == 14)
                items[i].type = GServer->PatList.Index[items[i].itemnum]->type;
            //Log(MSG_INFO,"slot: %i itemid: %i type: %i count: %i",i, items[i].itemnum,items[i].itemtype,items[i].count);
        }

    }
    //PY end
    /*
    result = GServer->DB->QStore("SELECT itemnum,itemtype,refine,durability,lifespan,slotnum,count,stats,socketed,appraised,gem FROM items WHERE owner=%i", CharInfo->charid);
    if(result==NULL) return false;
	while(row = mysql_fetch_row(result))
    {
        if(!GServer->IsValidItem( atoi(row[1]), atoi(row[0]) ) || atoi(row[6])==0)
        {
            Log(MSG_WARNING, "char %s have a invalid or empty item in inventory: %i-%i [%i], this item will be deleted", CharInfo->charname, atoi(row[1]), atoi(row[0]), atoi(row[6]) );
            continue;
        }
		UINT itemnum = atoi(row[5]);
		items[itemnum].itemnum = atoi(row[0]);
		items[itemnum].itemtype = atoi(row[1]);
		items[itemnum].refine = atoi(row[2]);
		items[itemnum].durability = atoi(row[3]);
		items[itemnum].lifespan = atoi(row[4]);
		items[itemnum].count = atoi(row[6]);
		items[itemnum].stats = atoi(row[7]);
		items[itemnum].socketed = (atoi(row[8])==1)?true:false;
		items[itemnum].appraised = (atoi(row[9])==1)?true:false;
		items[itemnum].gem = atoi(row[10])>3999?0:atoi(row[10]);
		//extra code to fix bugged gem items
        if(items[itemnum].gem > 0)
        {
            items[itemnum].stats = items[itemnum].gem;
        }
        // find item type for crafting
        if(items[itemnum].itemtype < 10)
            items[itemnum].type = GServer->EquipList[items[itemnum].itemtype].Index[items[itemnum].itemnum]->type;
        if(items[itemnum].itemtype == 10)
            items[itemnum].type = GServer->UseList.Index[items[itemnum].itemnum]->type;
        if(items[itemnum].itemtype == 11)
            items[itemnum].type = GServer->JemList.Index[items[itemnum].itemnum]->type;
        if(items[itemnum].itemtype == 12)
            items[itemnum].type = GServer->NaturalList.Index[items[itemnum].itemnum]->type;
        if(items[itemnum].itemtype == 14)
            items[itemnum].type = GServer->PatList.Index[items[itemnum].itemnum]->type;

	}*/
	//Log( MSG_DEBUG, "loaded inventory info");
	GServer->DB->QFree( );

	// PY experimental loadup code for storage
	//Log(MSG_INFO,"Loading storage data");
    result = GServer->DB->QStore("SELECT itemid,itemtype,refine,durability,lifespan,count,stats,socketed,appraised,trans_action FROM charstorage WHERE owner=%i", Session->userid);
    if(result == NULL)
    {
        Log(MSG_WARNING, "Something a bit wrong with this query");
        return false;
    }
    tmpid = "";
    itmp = 0;
    while( row = mysql_fetch_row(result) )
    {
        tmpid = strtok( row[0] , "|");
        for(unsigned int i=0;i<=MAX_STORAGE; i++)
        {
            if((itmp = strtok( NULL , "|"))!= NULL)
                storageitems[i].itemnum = atoi(itmp);
            //if (Session->userid == 8) //test code delete later
            //{
            //    if(i < 4)
            //        Log(MSG_DEBUG,"Storage item %i loaded. Value = %i",i,atoi(itmp));
            //}
        }
	    tmpid = strtok( row[1] , "|");
        for(unsigned int i=0;i<=MAX_STORAGE; i++)
        {
            if((itmp = strtok( NULL , "|"))!= NULL)
                storageitems[i].itemtype = atoi(itmp);
        }
        tmpid = strtok( row[2] , "|");
        for(unsigned int i=0;i<=MAX_STORAGE; i++)
        {
            if((itmp = strtok( NULL , "|"))!= NULL)
                storageitems[i].refine = atoi(itmp);
        }
        tmpid = strtok( row[3] , "|");
        for(unsigned int i=0;i<=MAX_STORAGE; i++)
        {
            if((itmp = strtok( NULL , "|"))!= NULL)
                storageitems[i].durability = atoi(itmp);
        }
        tmpid = strtok( row[4] , "|");
        for(unsigned int i=0;i<=MAX_STORAGE; i++)
        {
            if((itmp = strtok( NULL , "|"))!= NULL)
                storageitems[i].lifespan = atoi(itmp);
        }
        tmpid = strtok( row[5] , "|");
        for(unsigned int i=0;i<=MAX_STORAGE; i++)
        {
            if((itmp = strtok( NULL , "|"))!= NULL)
                storageitems[i].count = atoi(itmp);
        }
        tmpid = strtok( row[6] , "|");
        for(unsigned int i=0;i<=MAX_STORAGE; i++)
        {
            if((itmp = strtok( NULL , "|"))!= NULL)
            {
                storageitems[i].stats = atoi(itmp);
                if(storageitems[i].stats > 300)
                {
                    storageitems[i].gem = items[i].stats;
                }
                else
                    storageitems[i].gem = 0;
            }
        }
        tmpid = strtok( row[7] , "|");
        for(unsigned int i=0;i<=MAX_STORAGE; i++)
        {
            if((itmp = strtok( NULL , "|"))!= NULL)
                storageitems[i].socketed = (atoi(itmp)==1)?true:false;
            //just in case there is a discrepency in the database this will fix sockets.
            if(storageitems[i].gem != 0)
                storageitems[i].socketed = true;
        }
        tmpid = strtok( row[8] , "|");
        for(unsigned int i=0;i<=MAX_STORAGE; i++)
        {
            if((itmp = strtok( NULL , "|"))!= NULL)
                storageitems[i].appraised = (atoi(itmp)==1)?true:false;
        }
        for(unsigned int i=0;i<=MAX_STORAGE; i++)
        {
                // find item type for crafting
            if(storageitems[i].itemtype > 0 && storageitems[i].itemtype < 10)
                storageitems[i].type = GServer->EquipList[storageitems[i].itemtype].Index[storageitems[i].itemnum]->type;
            if(storageitems[i].itemtype == 10)
                storageitems[i].type = GServer->UseList.Index[storageitems[i].itemnum]->type;
            if(storageitems[i].itemtype == 11)
                storageitems[i].type = GServer->JemList.Index[storageitems[i].itemnum]->type;
            if(storageitems[i].itemtype == 12)
                storageitems[i].type = GServer->NaturalList.Index[storageitems[i].itemnum]->type;
            if(storageitems[i].itemtype == 14)
                storageitems[i].type = GServer->PatList.Index[storageitems[i].itemnum]->type;
        }

    }
    //PY end

	/*
	//original storage load code
	GServer->DB->QFree( );
	Log(MSG_INFO,"Loading storage data");
	result = GServer->DB->QStore("SELECT itemnum,itemtype,refine,durability,lifespan,slotnum,count,stats,socketed,appraised,gem FROM storage WHERE owner=%i", Session->userid);
	if(result == NULL) return false;
	nstorageitems = mysql_num_rows(result);
	while(row = mysql_fetch_row(result))
    {
        if(!GServer->IsValidItem( atoi(row[1]), atoi(row[0]) ) || atoi(row[6])==0)
        {
            Log(MSG_WARNING, "char %s have a invalid or empty item in storage: %i%i [%i], this item will be deleted", CharInfo->charname, atoi(row[1]), atoi(row[0]), atoi(row[6]) );
            continue;
        }
		UINT itemnum = atoi(row[5]);
		storageitems[itemnum].itemnum = atoi(row[0]);
		storageitems[itemnum].itemtype = atoi(row[1]);
		storageitems[itemnum].refine = atoi(row[2]);
		storageitems[itemnum].durability = atoi(row[3]);
		storageitems[itemnum].lifespan = atoi(row[4]);
		storageitems[itemnum].count = atoi(row[6]);
		storageitems[itemnum].stats = atoi(row[7]);
		storageitems[itemnum].socketed = (atoi(row[8])==1)?true:false;
		storageitems[itemnum].appraised = (atoi(row[9])==1)?true:false;
		storageitems[itemnum].gem = atoi(row[10]);
		//extra code to fix bugged gem items
        if(storageitems[itemnum].gem > 0)
        {
            storageitems[itemnum].stats = storageitems[itemnum].gem;
        }
        // find item type for crafting
        if(storageitems[itemnum].itemtype < 10)
            storageitems[itemnum].type = GServer->EquipList[items[itemnum].itemtype].Index[items[itemnum].itemnum]->type;
        if(storageitems[itemnum].itemtype == 10)
            storageitems[itemnum].type = GServer->UseList.Index[items[itemnum].itemnum]->type;
        if(storageitems[itemnum].itemtype == 11)
            storageitems[itemnum].type = GServer->JemList.Index[items[itemnum].itemnum]->type;
        if(storageitems[itemnum].itemtype == 12)
            storageitems[itemnum].type = GServer->NaturalList.Index[items[itemnum].itemnum]->type;
        if(storageitems[itemnum].itemtype == 14)
            storageitems[itemnum].type = GServer->PatList.Index[items[itemnum].itemnum]->type;

	}
	*/
	//End of original storage load code.


	GServer->DB->QFree( );
    //Log(MSG_INFO,"saving backup storage data.");
	SaveBackupStorage(1); // makes a copy of the account's storage at the moment of login
	//Log( MSG_DEBUG, "Saved backup storage data");
    if(Clan->clanid!=0)
    {
    	result = GServer->DB->QStore("SELECT logo,back,name,cp,grade FROM list_clan where id=%i", Clan->clanid);
        if(result==NULL) return false;
    	if(mysql_num_rows(result)!=1)
    	{
            Log(MSG_WARNING, "There are %i clan(s) with id %i", mysql_num_rows(result), Clan->clanid );
    	    Clan->clanid=0;
        }
    	else
    	{
    	    row = mysql_fetch_row(result);
    	    Clan->logo = atoi(row[0]);
    	    Clan->back = atoi(row[1]);
    	    strcpy(Clan->clanname,row[2]);
    	    Clan->CP = atoi(row[3]);
    	    Clan->grade = atoi(row[4]);
        }
    	GServer->DB->QFree( );
    }
    memset(&quest, 0, sizeof(SQuestData));
    //Log( MSG_DEBUG, "loaded clan info");
	UINT QuestToggle = 0;
	UINT rowcount = 0;
	/*
    result = GServer->DB->QStore("SELECT quests from list_quest where owner=%i",CharInfo->charid );
    if(result!=NULL)
    {
    	while(row = mysql_fetch_row(result))
        {
    	    if( row[0] != NULL )
    	    {
                memcpy(&quest, row[0], sizeof(SQuestData));
                rowcount++;
            }
        }
        if(rowcount == 0) //no data detected
        {
            //Log( MSG_DEBUG, "No quest data detected with Exjam method. using PY method instead");
            QuestToggle = 1; // set toggle to use PY method
            GServer->DB->QFree( );
        }
        if(QuestToggle == 0)
        {
            //delete the old quest entry after loading it.
           	//GServer->DB->QExecute( "DELETE FROM list_quest WHERE owner=%i",thisclient->CharInfo->charid );
           	//now convert quest flags to new system

            for(int flagid=0;flagid<512;flagid++)
            {
                dword byteid = flagid / 8;
    		    dword bitid = flagid % 8;
    		    if (((quest.flags[byteid] >> bitid) & 1) == 1)
        		    quest.Qflags[byteid][bitid] = 1;
    		    else
    		        quest.Qflags[byteid][bitid] = 0;
            }
            GServer->DB->QFree( );
            Log( MSG_DEBUG, "loaded quests via Exjam method");
            savequests(this);
            Log( MSG_DEBUG, "Converted flags and re-saved quests info");
        }
    }
    else
    {
        GServer->DB->QFree( );
    }
    if(QuestToggle = 1)
    */
   	{  // PY quest loading. Only performed if no old quests are found. Won't be any since there are no old characters
   	    Log( MSG_DEBUG, "starting PY quest load");
       	result = GServer->DB->QStore("SELECT questid,starttime,vbl,switches,items,count,episodevar,jobvar,planetvar,unionvar from player_quests where owner=%i",CharInfo->charid );
       	//Log( MSG_DEBUG, "ran the query");
        tmpid = "";
        itmp = "";
        if(result != NULL)
        {
            UINT Index = 0;
            while(row = mysql_fetch_row(result))
            {
                quest.quests[Index].QuestID = atoi(row[0]);
                quest.quests[Index].StartTime = atoi(row[1]);

                tmpid = strtok( row[2] , "|");
                for(unsigned int j=0;j<=10;j++)                       //quest variables
                {
                    if(tmpid != NULL)
                        quest.quests[Index].Variables[j] = atoi(tmpid);
                    else
                        quest.quests[Index].Variables[j] = 0;
                    tmpid = strtok(NULL, ",|");
                    //Log( MSG_DEBUG, "Quest Var[%i][%i] = %i",Index ,j ,quest.quests[Index].Variables[j]);
                }
                tmpid = strtok( row[3] , "|");                        //quest switches
                for(unsigned int j=0;j<=4;j++)
                {
                    if(tmpid != NULL)
                        quest.quests[Index].Switches[j] = atoi(tmpid);
                    else
                        quest.quests[Index].Switches[j] = 0;
                    tmpid = strtok(NULL, ",|");
                    //Log( MSG_DEBUG, "Quest Switch[%i][%i] = %i",Index ,j ,quest.quests[Index].Switches[j]);
                }
                tmpid = strtok( row[4] , "|");                        //quest items
                for(unsigned int j=0;j<=5;j++)
                {
                    if(tmpid != NULL)
                    {
                        itemid = atoi(tmpid);
                        quest.quests[Index].Items[j].itemnum = itemid % 1000;
                        quest.quests[Index].Items[j].itemtype = itemid / 1000;
                    }
                    else
                    {
                        quest.quests[Index].Items[j].itemnum = 0;
                        quest.quests[Index].Items[j].itemtype = 0;
                    }
                    tmpid = strtok(NULL, ",|");
                    //Log( MSG_DEBUG, "Quest Item[%i][%i] = %i",Index , j ,quest.quests[Index].Items[j].itemnum);
                }
                tmpid = strtok( row[5] , "|");                        //quest item count
                for(unsigned int j=0;j<=5;j++)
                {
                    if(tmpid != NULL)
                        quest.quests[Index].Items[j].count = atoi(tmpid);
                    else
                        quest.quests[Index].Items[j].count = 0;
                    tmpid = strtok(NULL, ",|");
                    //Log( MSG_DEBUG, "Quest Item Count[%i][%i] = %i",Index ,j ,quest.quests[Index].Items[j].count);
                }
                tmpid = strtok( row[6] , "|");
                for(unsigned int j=0;j<=5;j++)                      //episode variables
                {
                    if(tmpid != NULL)
                        quest.EpisodeVar[j] = atoi(tmpid);
                    else
                        quest.EpisodeVar[j] = 0;
                    tmpid = strtok(NULL, ",|");
                    //Log( MSG_DEBUG, "Quest EpisodeVar[%i][%i] = %i",Index ,j ,quest.EpisodeVar[j]);
                }
                tmpid = strtok( row[7] , "|");                        //job variables
                for(unsigned int j=0;j<=3;j++)
                {
                    if(tmpid != NULL)
                        quest.JobVar[j] = atoi(tmpid);
                    else
                        quest.JobVar[j] = 0;
                    tmpid = strtok(NULL, ",|");
                    //Log( MSG_DEBUG, "Quest JobVar[%i][%i] = %i",Index ,j ,quest.JobVar[j]);
                }
                tmpid = strtok( row[8] , "|");                        //Planet variables
                for(unsigned int j=0;j<=7;j++)
                {
                    if(tmpid != NULL)
                        quest.PlanetVar[j] = atoi(tmpid);
                    else
                        quest.PlanetVar[j] = 0;
                    tmpid = strtok(NULL, ",|");
                }
                tmpid = strtok( row[9] , "|");                        //Planet variables
                for(unsigned int j=0;j<=10;j++)
                {
                    if(tmpid != NULL)
                        quest.UnionVar[j] = atoi(tmpid);
                    else
                        quest.UnionVar[j] = 0;
                    tmpid = strtok(NULL, ",|");
                }
                Index ++;
            }
        }
        //Log( MSG_DEBUG, "loaded the regular quest variables");
        GServer->DB->QFree( );
        // PY method load quest flags
        //Log( MSG_DEBUG, "starting PY flag load");
        result = GServer->DB->QStore("SELECT flags from questflags where owner=%i",CharInfo->charid );
        //Log( MSG_DEBUG, "ran the query");
        if(result != NULL)
        {
            tmpid = "";
            itmp = 0;
            UINT Index = 0;
            UINT flagCount = 0;
            //first set all quest flags to zero
            for(int i=0;i<64;i++)
            {
               for(int j=0;j<8;j++)
                   quest.Qflags[i][j] = 0;
            }
            while(row = mysql_fetch_row(result))
            {
                int flagid = atoi(row[0]);
                dword byteid = flagid / 8;
    		    dword bitid = flagid % 8;
    		    quest.Qflags[byteid][bitid] = 1;
    		    flagCount++;
            }
            //Log( MSG_DEBUG, "Flag count = %i",flagCount);
        }
        GServer->DB->QFree( );
        //Log( MSG_DEBUG, "loaded quests info via PY method");
    }
	Session->isLoggedIn = true;




	// Timestamp for last login
    time_t rtime;
    time(&rtime);
    char *timestamp = ctime(&rtime);
    timestamp[ strlen(timestamp)-1 ] = ' ';
    //Log(MSG_DEBUG,"setting time stamp fo login: %s for userid: %u",timestamp,Session->userid );
    GServer->DB->QExecute("UPDATE accounts SET online=1, lastlogin='%s' where id=%u", timestamp,Session->userid );
    Session->LogTime = clock();
    //Log(MSG_INFO,"set datestamp");

    // Calculate how many fairies are available online, according to amount of players
	int oldFairyMax = GServer->Config.FairyMax;
    GServer->Config.FairyMax = (int)ceil((float)GServer->ClientList.size() / 50.0) + GServer->Config.ScrollFairy; //1 fairy for every 50 player + the number of fairies summoned by scrolls
	if( oldFairyMax < GServer->Config.FairyMax ){
        CFairy* thisfairy = new (nothrow) CFairy;
        thisfairy->LastTime = clock();
        thisfairy->assigned = 0;
        thisfairy->ListIndex = 0;
        thisfairy->WaitTime = GServer->Config.FairyWait;
        GServer->FairyList.push_back( thisfairy );
    }
	return true;
}

// Trying a new save method PY (nothing changed yet. just made a copy of savedata for safety
void CPlayer::savedata( )
{
    lastSaveTime = clock();
    if(Session->userid!=0)
    {
        CMap* map = GServer->MapList.Index[Position->Map];
    	CRespawnPoint* thisrespawn = map->GetNearRespawn( this );
    	CPlayer* thisclient = GServer->GetClientByID( clientid );
    	if(thisrespawn == NULL)
    	   Position->respawn = 4;
    	else
    	   Position->respawn = thisrespawn->id;
	    char quick[1024];
	    char basic[1024];
	    //char sclass[1024];
 	    //char slevel[1024];
        char active[1024];
        char activelvl[1024];
        char pasive[1024];
        char pasivelvl[1024];
    	for(UINT i=0;i<30;i++)
    	{
            if(i==0)
            	sprintf(&basic[i], "%i",bskills[i]);
            else
                sprintf(&basic[strlen(basic)], ",%i",bskills[i]);
        }
        for(UINT i=0;i<30;i++)
    	{
            if(i==0)
            	sprintf(&active[i], "%i",askill[i]);
            else
                sprintf(&active[strlen(active)], ",%i",askill[i]);
        }
        for(UINT i=0;i<30;i++)
        {
            if(i==0)
            	sprintf(&activelvl[i], "%i",askilllvl[i]);
            else
                sprintf(&activelvl[strlen(activelvl)], ",%i",askilllvl[i]);
        }
        for(UINT i=0;i<30;i++)
        {
            if(i==0)
            	sprintf(&pasive[i], "%i",pskill[i]);
            else
                sprintf(&pasive[strlen(pasive)], ",%i",pskill[i]);
        }
        for(UINT i=0;i<30;i++)
        {
            if(i==0)
            	sprintf(&pasivelvl[i], "%i",pskilllvl[i]);
            else
                sprintf(&pasivelvl[strlen(pasivelvl)], ",%i",pskilllvl[i]);
        }
        for(UINT i=0;i<32;i++)
        {
            if(i==0)
            	sprintf(&quick[i], "%i",quickbar[i]);
            else
                sprintf(&quick[strlen(quick)], ",%i",quickbar[i]);
        }
        long int hp = Stats->HP;
        if(hp<=0)
    	    hp=Stats->MaxHP * 10 / 100;
	    if(Stats->MP<0)
	        Stats->MP=0;

        GServer->DB->QExecute("UPDATE characters SET level=%i,face=%i,hairStyle=%i,sex=%i,classid=%i,zuly=%i,str=%i,dex=%i,_int=%i, con=%i,cha=%i,sen=%i,curHp=%i,curMp=%i,id=%i,statp=%i,skillp=%i,exp=%i,stamina=%i,quickbar='%s',basic_skills='%s',askill='%s',askilllvl='%s',pskill='%s',pskilllvl='%s',respawnid=%i,clanid=%i,clan_rank=%i,townid=%i WHERE char_name='%s'",
                    Stats->Level, CharInfo->Face, CharInfo->Hair, CharInfo->Sex, CharInfo->Job, CharInfo->Zulies, Attr->Str, Attr->Dex, Attr->Int, Attr->Con, Attr->Cha, Attr->Sen, hp, Stats->MP, CharInfo->charid, CharInfo->StatPoints, CharInfo->SkillPoints,
                    CharInfo->Exp, CharInfo->stamina, quick, basic, active, activelvl, pasive, pasivelvl, Position->respawn, Clan->clanid, Clan->clanrank, Position->saved, CharInfo->charname );

        GServer->DB->QExecute("UPDATE characters SET unionid=%i,union1points=%i,union2points=%i,union3points=%i,union4points=%i,union5points=%i,union6points=%i,union7points=%i,union8points=%i,union9points=%i,union10points=%i WHERE char_name='%s'",
                    Union_s->unionvar[0], Union_s->unionvar[1],Union_s->unionvar[2], Union_s->unionvar[3], Union_s->unionvar[4], Union_s->unionvar[5], Union_s->unionvar[6], Union_s->unionvar[7], Union_s->unionvar[8], Union_s->unionvar[9], Union_s->unionvar[10], CharInfo->charname );

        //save the inventory
    	saveinventory();
    	//save the storage items
    	savestorage();

        //this lot to be removed later
        //if(!GServer->DB->QExecute("DELETE FROM items WHERE owner=%i", CharInfo->charid)) return;
    	//for(UINT i=0;i<MAX_INVENTORY;i++)
        //{
    	//	if (items[i].count > 0)
        //    {
    	//		GServer->DB->QExecute("INSERT INTO items (owner,itemnum,itemtype,refine,durability,lifespan,slotnum,count,stats,socketed,appraised,gem) VALUES(%i,%i,%i,%i,%i,%i,%i,%i,%i,%i,%i,%i)",
    	//							CharInfo->charid, items[i].itemnum, items[i].itemtype,items[i].refine, items[i].durability,
    	//							items[i].lifespan, i, items[i].count, items[i].stats, (items[i].socketed?1:0),
    	//							(items[i].appraised?1:0),items[i].gem );
    	//	}
    	//}


    	if(!GServer->DB->QExecute("DELETE FROM storage WHERE owner=%i", Session->userid)) return;
    	for(UINT i=0;i<160;i++)
        {
    		if (storageitems[i].count > 0)
            {
    			GServer->DB->QExecute("INSERT INTO storage (owner,itemnum,itemtype,refine,durability,lifespan,slotnum,count,stats,socketed,appraised,gem) VALUES(%i,%i,%i,%i,%i,%i,%i,%i,%i,%i,%i,%i)",
    								Session->userid, storageitems[i].itemnum, storageitems[i].itemtype,storageitems[i].refine, storageitems[i].durability,
    								storageitems[i].lifespan, i, storageitems[i].count, storageitems[i].stats, (storageitems[i].socketed?1:0),
    								(storageitems[i].appraised?1:0), storageitems[i].gem );
    		}
    	}
        savequests( this );
		GServer->DB->QExecute("UPDATE accounts SET zulystorage = %i, logtime = %i,ktpoints=%i WHERE id = %i", CharInfo->Storage_Zulies, Session->logtime,Session->KTPoints, Session->userid);
		Log(MSG_INFO, "Data Saved for char '%s' ", CharInfo->charname );
    }
}

//PY Character save
void CPlayer::quicksave( )
{
     GServer->DB->QExecute("UPDATE characters SET level=%i,face=%i,hairStyle=%i,sex=%i,classid=%i,zuly=%i,str=%i,dex=%i,_int=%i, con=%i,cha=%i,sen=%i,curHp=%i,curMp=%i,id=%i,statp=%i,skillp=%i,exp=%i,stamina=%i,respawnid=%i,clanid=%i,clan_rank=%i,townid=%i WHERE char_name='%s'",
         Stats->Level, CharInfo->Face, CharInfo->Hair, CharInfo->Sex, CharInfo->Job, CharInfo->Zulies, Attr->Str, Attr->Dex, Attr->Int, Attr->Con, Attr->Cha, Attr->Sen, Stats->HP, Stats->MP, CharInfo->charid, CharInfo->StatPoints, CharInfo->SkillPoints,
         CharInfo->Exp, CharInfo->stamina, Position->respawn, Clan->clanid, Clan->clanrank, Position->saved, CharInfo->charname );
     Log(MSG_INFO, "Data Quick Saved for char '%s' ", CharInfo->charname );
}

//PY Inventory save
void CPlayer::saveinventory( )
{
    //build the strings
    char tmp1[1024] = "";
    char tmp2[1024] = "";
    char tmp3[1024] = "";
    char tmp4[1024] = "";
    char tmp5[1024] = "";
    char tmp6[1024] = "";
    char tmp7[1024] = "";
    char tmp8[1024] = "";
    char tmp9[1024] = "";
    char slot[1024] = "";
    long tran = 0;
    for(unsigned int i=0;i<=MAX_INVENTORY; i++) //inventory holds 140
    {
        int socketed = 0;
        int appraised = 0;
        tran += items[i].itemnum + items[i].itemtype + items[i].refine;
        //tran += items[i].durability + items[i].lifespan + items[i].count + items[i].stats;
        //Log(MSG_INFO,"slot: %i itemid: %i type: %i count: %i",i, items[i].itemnum,items[i].itemtype,items[i].count);
        if(i == 0)
        {
            if(items[i].count == 0)
            {
                sprintf(&tmp1[strlen(tmp1)], "0");
                sprintf(&tmp2[strlen(tmp2)], "0");
                sprintf(&tmp3[strlen(tmp3)], "0");
                sprintf(&tmp4[strlen(tmp4)], "0");
                sprintf(&tmp5[strlen(tmp5)], "0");
                sprintf(&tmp6[strlen(tmp6)], "0");
                sprintf(&tmp7[strlen(tmp7)], "0");
                sprintf(&tmp8[strlen(tmp8)], "0");
                sprintf(&tmp9[strlen(tmp9)], "0");
                sprintf(&slot[strlen(slot)], "%i",i);
            }
            else
            {
                if(items[i].socketed == true)
                    socketed = 1;
                if(items[i].appraised == true)
                    appraised = 1;
                sprintf(&tmp1[strlen(tmp1)], "%i",items[i].itemnum);
                sprintf(&tmp2[strlen(tmp2)], "%i",items[i].itemtype);
                sprintf(&tmp3[strlen(tmp3)], "%i",items[i].refine);
                sprintf(&tmp4[strlen(tmp4)], "%i",items[i].durability);
                sprintf(&tmp5[strlen(tmp5)], "%i",items[i].lifespan);
                sprintf(&tmp6[strlen(tmp6)], "%i",items[i].count);
                sprintf(&tmp7[strlen(tmp7)], "%i",items[i].stats);
                sprintf(&tmp8[strlen(tmp8)], "%i",socketed);
                sprintf(&tmp9[strlen(tmp9)], "%i",appraised);
                sprintf(&slot[strlen(slot)], "%i",i);
            }
        }
        else
        {
            if(items[i].count == 0)
            {
                sprintf(&tmp1[strlen(tmp1)], "|0");
                sprintf(&tmp2[strlen(tmp2)], "|0");
                sprintf(&tmp3[strlen(tmp3)], "|0");
                sprintf(&tmp4[strlen(tmp4)], "|0");
                sprintf(&tmp5[strlen(tmp5)], "|0");
                sprintf(&tmp6[strlen(tmp6)], "|0");
                sprintf(&tmp7[strlen(tmp7)], "|0");
                sprintf(&tmp8[strlen(tmp8)], "|0");
                sprintf(&tmp9[strlen(tmp9)], "|0");
                sprintf(&slot[strlen(slot)], "|%i",i);
            }
            else
            {
                if(items[i].socketed == true)
                    socketed = 1;
                if(items[i].appraised == true)
                    appraised = 1;
                if(items[i].gem != 0 && items[i].gem != items[i].stats)
                    items[i].stats = items[i].gem;
                sprintf(&tmp1[strlen(tmp1)], "|%i",items[i].itemnum);
                sprintf(&tmp2[strlen(tmp2)], "|%i",items[i].itemtype);
                sprintf(&tmp3[strlen(tmp3)], "|%i",items[i].refine);
                sprintf(&tmp4[strlen(tmp4)], "|%i",items[i].durability);
                sprintf(&tmp5[strlen(tmp5)], "|%i",items[i].lifespan);
                sprintf(&tmp6[strlen(tmp6)], "|%i",items[i].count);
                sprintf(&tmp7[strlen(tmp7)], "|%i",items[i].stats);
                sprintf(&tmp8[strlen(tmp8)], "|%i",socketed);
                sprintf(&tmp9[strlen(tmp9)], "|%i",appraised);
                sprintf(&slot[strlen(slot)], "|%i",i);
            }
        }
    }
    //Log(MSG_INFO,"Save string1 = %s ", tmp1);
    //Log(MSG_INFO,"Save string2 = %s ", tmp2);
    //Log(MSG_INFO,"Save string3 = %s ", tmp3);
    //Log(MSG_INFO,"Save string4 = %s ", tmp4);
    //Log(MSG_INFO,"Save string5 = %s ", tmp5);
    //Log(MSG_INFO,"Save string6 = %s ", tmp6);
    //Log(MSG_INFO,"Save string7 = %s ", tmp7);
    //Log(MSG_INFO,"Save string8 = %s ", tmp8);
    //Log(MSG_INFO,"Save string9 = %s ", tmp9);
    //check if a record exists for this player
    MYSQL_RES *result = GServer->DB->QStore("SELECT itemid,itemtype,refine,durability,lifespan,count,stats,socketed,appraised,slot,trans_action FROM charitems WHERE owner=%i", CharInfo->charid);
    MYSQL_ROW row;
    bool doesexist = false;
    while(row = mysql_fetch_row(result))
    {
        doesexist = true;
    }
    GServer->DB->QFree( );
    if(doesexist == true)
    {
        //record exists so update it
        //Log(MSG_INFO,"record exists.... updating");
        GServer->DB->QExecute("UPDATE charitems SET itemid='%s',itemtype='%s',refine='%s',durability='%s',lifespan='%s',count='%s',stats='%s',socketed='%s',appraised='%s',slot='%s',trans_action=%i WHERE owner='%i'",
                    tmp1,tmp2,tmp3,tmp4,tmp5,tmp6,tmp7,tmp8,tmp9,slot,tran,CharInfo->charid);
    }
    else
    {
        //record does not exist so make a new one
        //Log(MSG_INFO,"record does not exist.... creating new record");
        //GServer->DB->QExecute("INSERT INTO charitems (owner,trans_action) VALUES ('%i','%i')",CharInfo->charid,tran);
        GServer->DB->QExecute("INSERT INTO charitems (owner,itemid,itemtype,refine,durability,lifespan,count,stats,socketed,appraised,trans_action) VALUES('%i','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%i')",
    				CharInfo->charid,tmp1,tmp2,tmp3,tmp4,tmp5,tmp6,tmp7,tmp8,tmp9,slot,tran);
    }
    //save zuly
    GServer->DB->QExecute("UPDATE characters SET zuly=%i WHERE char_name='%s'",
         CharInfo->Zulies, CharInfo->charname );
    //Log(MSG_INFO,"Inventory saved");
    return;
}

//PY Storage save
void CPlayer::savestorage( )
{
    //build the strings. As we need a slot zero for storage we are going to have to add a buffer entry first
    //for some bizarre reason when we load teh storage it ignores the leading zero so stuff gets shifted left without this
    char tmp1[1024] = "0";
    char tmp2[1024] = "0";
    char tmp3[1024] = "0";
    char tmp4[1024] = "0";
    char tmp5[1024] = "0";
    char tmp6[1024] = "0";
    char tmp7[1024] = "0";
    char tmp8[1024] = "0";
    char tmp9[1024] = "0";
    char slot[1024] = "0";
    long tran = 0;
    for(unsigned int i=0;i<=MAX_STORAGE; i++) //Storage holds 160
    {
        int socketed = 0;
        int appraised = 0;
        tran += storageitems[i].itemnum + storageitems[i].itemtype + storageitems[i].refine;
        tran += storageitems[i].durability + storageitems[i].lifespan + storageitems[i].count + storageitems[i].stats;

        if(storageitems[i].count == 0)
        {
            sprintf(&tmp1[strlen(tmp1)], "|0");
            sprintf(&tmp2[strlen(tmp2)], "|0");
            sprintf(&tmp3[strlen(tmp3)], "|0");
            sprintf(&tmp4[strlen(tmp4)], "|0");
            sprintf(&tmp5[strlen(tmp5)], "|0");
            sprintf(&tmp6[strlen(tmp6)], "|0");
            sprintf(&tmp7[strlen(tmp7)], "|0");
            sprintf(&tmp8[strlen(tmp8)], "|0");
            sprintf(&tmp9[strlen(tmp9)], "|0");
            if(i != 0) //don't write the zero value for slot since we already started the string with a leading zero
                sprintf(&slot[strlen(slot)], "|%i",i);
        }
        else
        {
            if(storageitems[i].socketed == true)
                socketed = 1;
            if(storageitems[i].appraised == true)
                appraised = 1;
            sprintf(&tmp1[strlen(tmp1)], "|%i",storageitems[i].itemnum);
            sprintf(&tmp2[strlen(tmp2)], "|%i",storageitems[i].itemtype);
            sprintf(&tmp3[strlen(tmp3)], "|%i",storageitems[i].refine);
            sprintf(&tmp4[strlen(tmp4)], "|%i",storageitems[i].durability);
            sprintf(&tmp5[strlen(tmp5)], "|%i",storageitems[i].lifespan);
            sprintf(&tmp6[strlen(tmp6)], "|%i",storageitems[i].count);
            sprintf(&tmp7[strlen(tmp7)], "|%i",storageitems[i].stats);
            sprintf(&tmp8[strlen(tmp8)], "|%i",socketed);
            sprintf(&tmp9[strlen(tmp9)], "|%i",appraised);
            if(i != 0) //don't write the zero value for slot since we already started the string with a leading zero
                sprintf(&slot[strlen(slot)], "|%i",i);
        }
    }
    //check if a record exists for this player
    MYSQL_RES *result = GServer->DB->QStore("SELECT itemid,itemtype,refine,durability,lifespan,count,stats,socketed,appraised,slot,trans_action FROM charstorage WHERE owner=%i", Session->userid);
    MYSQL_ROW row;
    bool doesexist = false;
    while(row = mysql_fetch_row(result))
    {
        doesexist = true;
    }
    GServer->DB->QFree( );
    if(doesexist == true)
    {
        //record exists so update it
        //Log(MSG_INFO,"record exists.... updating");
        GServer->DB->QExecute("UPDATE charstorage SET itemid='%s',itemtype='%s',refine='%s',durability='%s',lifespan='%s',count='%s',stats='%s',socketed='%s',appraised='%s',slot='%s',trans_action=%i WHERE owner='%i'",
                    tmp1,tmp2,tmp3,tmp4,tmp5,tmp6,tmp7,tmp8,tmp9,slot,tran,Session->userid);
    }
    else
    {
        //record does not exist so make a new one
        //Log(MSG_INFO,"record does not exist.... creating new record");
        //GServer->DB->QExecute("INSERT INTO charitems (owner,trans_action) VALUES ('%i','%i')",CharInfo->charid,tran);
        GServer->DB->QExecute("INSERT INTO charstorage (owner,itemid,itemtype,refine,durability,lifespan,count,stats,socketed,appraised,slot,trans_action) VALUES('%i','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%i')",
    				Session->userid,tmp1,tmp2,tmp3,tmp4,tmp5,tmp6,tmp7,tmp8,tmp9,slot,tran);
    }
    //save zuly storage
    GServer->DB->QExecute("UPDATE accounts SET zulystorage = %i WHERE id = %i", CharInfo->Storage_Zulies, Session->userid);
    //Log(MSG_INFO,"Storage saved");
    return;
}

//PY Storage save
void CPlayer::SaveBackupStorage( int logstate )
{
    //build the strings. As we need a slot zero for storage we are going to have to add a buffer entry first
    //for some bizarre reason when we load the storage it ignores the leading zero so stuff gets shifted left without this
    char tmp1[1024] = "0";
    char tmp2[1024] = "0";
    char tmp3[1024] = "0";
    char tmp4[1024] = "0";
    char tmp5[1024] = "0";
    char tmp6[1024] = "0";
    char tmp7[1024] = "0";
    char tmp8[1024] = "0";
    char tmp9[1024] = "0";
    char slot[1034] = "0";
    long tran = 0;
    for(unsigned int i=0;i<=MAX_STORAGE; i++) //Storage holds 160
    {
        int socketed = 0;
        int appraised = 0;
        if(storageitems[i].count == 0)
        {
            sprintf(&tmp1[strlen(tmp1)], "|0");
            sprintf(&tmp2[strlen(tmp2)], "|0");
            sprintf(&tmp3[strlen(tmp3)], "|0");
            sprintf(&tmp4[strlen(tmp4)], "|0");
            sprintf(&tmp5[strlen(tmp5)], "|0");
            sprintf(&tmp6[strlen(tmp6)], "|0");
            sprintf(&tmp7[strlen(tmp7)], "|0");
            sprintf(&tmp8[strlen(tmp8)], "|0");
            sprintf(&tmp9[strlen(tmp9)], "|0");
            if(i != 0) //don't write the zero value for slot since we already started the string with a leading zero
                sprintf(&slot[strlen(slot)], "|%i",i);
        }
        else
        {
            if(storageitems[i].socketed == true)
                    socketed = 1;
            if(storageitems[i].appraised == true)
                    appraised = 1;
            sprintf(&tmp1[strlen(tmp1)], "|%i",storageitems[i].itemnum);
            sprintf(&tmp2[strlen(tmp2)], "|%i",storageitems[i].itemtype);
            sprintf(&tmp3[strlen(tmp3)], "|%i",storageitems[i].refine);
            sprintf(&tmp4[strlen(tmp4)], "|%i",storageitems[i].durability);
            sprintf(&tmp5[strlen(tmp5)], "|%i",storageitems[i].lifespan);
            sprintf(&tmp6[strlen(tmp6)], "|%i",storageitems[i].count);
            sprintf(&tmp7[strlen(tmp7)], "|%i",storageitems[i].stats);
            sprintf(&tmp8[strlen(tmp8)], "|%i",socketed);
            sprintf(&tmp9[strlen(tmp9)], "|%i",appraised);
            if(i != 0) //don't write the zero value for slot since we already started the string with a leading zero
                sprintf(&slot[strlen(slot)], "|%i",i);
        }
    }
    MYSQL_RES *result = GServer->DB->QStore("SELECT thisrev FROM backupstorage WHERE owner=%i", Session->userid);
    MYSQL_ROW row;
    int thisrev = 0;
    int tmprev = 0;
    while(row = mysql_fetch_row(result))
    {
        tmprev = atoi(row[0]);
        if(tmprev > thisrev)thisrev = tmprev;
    }
    thisrev ++; // add 1 to the highest existing rev number for this user
    GServer->DB->QFree( );
    // Timestamp
    time_t rtime;
    time(&rtime);
    char *timestamp = ctime(&rtime);
    timestamp[ strlen(timestamp)-1 ] = ' ';
    GServer->DB->QExecute("INSERT INTO backupstorage (owner,itemid,itemtype,refine,durability,lifespan,count,stats,socketed,appraised,slot,trans_action,datestamp,thisrev) VALUES('%i','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%i','%s','%i')",
        Session->userid,tmp1,tmp2,tmp3,tmp4,tmp5,tmp6,tmp7,tmp8,tmp9,slot,logstate,timestamp,thisrev);
    //Log(MSG_INFO,"backup Storage saved for User %i revision number %i logstate %i",Session->userid, thisrev, logstate);
    return;
}


// This is a REALLY bad way of saving the character data, but it works ^^
/*void CPlayer::savedata( )
{
    lastSaveTime = clock();
    if(Session->userid!=0)
    {
        CMap* map = GServer->MapList.Index[Position->Map];
    	CRespawnPoint* thisrespawn = map->GetNearRespawn( this );
    	CPlayer* thisclient = GServer->GetClientByID( clientid );
    	if(thisrespawn == NULL)
    	   Position->respawn = 4;
    	else
    	   Position->respawn = thisrespawn->id;
	    char quick[1024];
	    char basic[1024];
	    //char sclass[1024];
 	    //char slevel[1024];
         char active[1024];
         char activelvl[1024];
         char pasive[1024];
         char pasivelvl[1024];
    	for(UINT i=0;i<30;i++)
    	{
            if(i==0)
            	sprintf(&basic[i], "%i",bskills[i]);
            else
                sprintf(&basic[strlen(basic)], ",%i",bskills[i]);
        }
        for(UINT i=0;i<30;i++)
    	{
            if(i==0)
            	sprintf(&active[i], "%i",askill[i]);
            else
                sprintf(&active[strlen(active)], ",%i",askill[i]);
        }
        for(UINT i=0;i<30;i++)
    	{
            if(i==0)
            	sprintf(&activelvl[i], "%i",askilllvl[i]);
            else
                sprintf(&activelvl[strlen(activelvl)], ",%i",askilllvl[i]);
        }
        for(UINT i=0;i<30;i++)
    	{
            if(i==0)
            	sprintf(&pasive[i], "%i",pskill[i]);
            else
                sprintf(&pasive[strlen(pasive)], ",%i",pskill[i]);
        }
        for(UINT i=0;i<30;i++)
    	{
            if(i==0)
            	sprintf(&pasivelvl[i], "%i",pskilllvl[i]);
            else
                sprintf(&pasivelvl[strlen(pasivelvl)], ",%i",pskilllvl[i]);
        }
        for(UINT i=0;i<32;i++)
        {
            if(i==0)
            	sprintf(&quick[i], "%i",quickbar[i]);
            else
                sprintf(&quick[strlen(quick)], ",%i",quickbar[i]);
        }
        long int hp = Stats->HP;
    	if(hp<=0)
    	   hp=Stats->MaxHP * 10 / 100;
	   if(Stats->MP<0)
	       Stats->MP=0;
  //Save data complete now?? (Pepu)

        GServer->DB->QExecute("UPDATE characters SET level=%i,face=%i,hairStyle=%i,sex=%i,classid=%i,zuly=%i,str=%i,dex=%i,_int=%i, con=%i,cha=%i,sen=%i,curHp=%i,curMp=%i,id=%i,statp=%i,skillp=%i,exp=%i,stamina=%i,quickbar='%s',basic_skills='%s',askill='%s',askilllvl='%s',pskill='%s',pskilllvl='%s',respawnid=%i,clanid=%i,clan_rank=%i,townid=%i WHERE char_name='%s'",
                    Stats->Level, CharInfo->Face, CharInfo->Hair, CharInfo->Sex, CharInfo->Job, CharInfo->Zulies, Attr->Str, Attr->Dex, Attr->Int, Attr->Con, Attr->Cha, Attr->Sen, hp, Stats->MP, CharInfo->charid, CharInfo->StatPoints, CharInfo->SkillPoints,
                    CharInfo->Exp, CharInfo->stamina, quick, basic, active, activelvl, pasive, pasivelvl, Position->respawn, Clan->clanid, Clan->clanrank, Position->saved, CharInfo->charname );

        GServer->DB->QExecute("UPDATE characters SET unionid=%i,union1points=%i,union2points=%i,union3points=%i,union4points=%i,union5points=%i,union6points=%i,union7points=%i,union8points=%i,union9points=%i,union10points=%i WHERE char_name='%s'",
                    Union_s->unionvar[0], Union_s->unionvar[1],Union_s->unionvar[2], Union_s->unionvar[3], Union_s->unionvar[4], Union_s->unionvar[5], Union_s->unionvar[6], Union_s->unionvar[7], Union_s->unionvar[8], Union_s->unionvar[9], Union_s->unionvar[10], CharInfo->charname );


    	if(!GServer->DB->QExecute("DELETE FROM items WHERE owner=%i", CharInfo->charid)) return;
    	for(UINT i=0;i<MAX_INVENTORY;i++)
        {
    		if (items[i].count > 0)
            {
    			GServer->DB->QExecute("INSERT INTO items (owner,itemnum,itemtype,refine,durability,lifespan,slotnum,count,stats,socketed,appraised,gem) VALUES(%i,%i,%i,%i,%i,%i,%i,%i,%i,%i,%i,%i)",
    								CharInfo->charid, items[i].itemnum, items[i].itemtype,items[i].refine, items[i].durability,
    								items[i].lifespan, i, items[i].count, items[i].stats, (items[i].socketed?1:0),
    								(items[i].appraised?1:0),items[i].gem );
    		}
    	}
    	if(!GServer->DB->QExecute("DELETE FROM storage WHERE owner=%i", Session->userid)) return;
    	for(UINT i=0;i<160;i++)
        {
    		if (storageitems[i].count > 0)
            {
    			GServer->DB->QExecute("INSERT INTO storage (owner,itemnum,itemtype,refine,durability,lifespan,slotnum,count,stats,socketed,appraised,gem) VALUES(%i,%i,%i,%i,%i,%i,%i,%i,%i,%i,%i,%i)",
    								Session->userid, storageitems[i].itemnum, storageitems[i].itemtype,storageitems[i].refine, storageitems[i].durability,
    								storageitems[i].lifespan, i, storageitems[i].count, storageitems[i].stats, (storageitems[i].socketed?1:0),
    								(storageitems[i].appraised?1:0), storageitems[i].gem );
    		}
    	}
        savequests( this );
		GServer->DB->QExecute("UPDATE accounts SET zulystorage = %i WHERE id = %i", CharInfo->Storage_Zulies, Session->userid);
		Log(MSG_INFO, "Data Saved for char '%s' ", CharInfo->charname );
    }
}*/

void CPlayer::savequests( CPlayer* thisclient )
{
    //char* questBuffer = new char[sizeof(SQuestData)*3 +1];
	//mysql_real_escape_string(GServer->DB->Mysql, questBuffer, (const char*)&quest, sizeof(SQuestData));
    //GServer->DB->QExecute( "DELETE FROM list_quest WHERE owner=%i",thisclient->CharInfo->charid );
    //GServer->DB->QExecute("INSERT INTO list_quest (owner, quests) VALUES(%i,'%s')",
    //    thisclient->CharInfo->charid, questBuffer);
    //delete questBuffer;

    //PY retro method
    //GServer->DB->QFree( );
    //Log(MSG_DEBUG,"Started SaveQuests");
    GServer->DB->QExecute( "DELETE FROM player_quests WHERE owner=%i",thisclient->CharInfo->charid );
    //Log(MSG_DEBUG,"Ran the player_quests deletion query");
    for(int i=0;i<10;i++)
    {
        if (thisclient->quest.quests[i].QuestID == 0) continue;
        char vbl[100] = ""; //make  the variables list
        for(int j=0;j<10;j++)
        {
            sprintf(&vbl[strlen(vbl)], "%u",thisclient->quest.quests[i].Variables[j]);
            if(j < 9)
                 sprintf(&vbl[strlen(vbl)],"|");
        }
        char switches[10] = ""; //make  the switches list
        for(int j=0;j<4;j++)
        {
            sprintf(&switches[strlen(switches)], "%u",thisclient->quest.quests[i].Switches[j]);
            if(j < 3)
                 sprintf(&switches[strlen(switches)],"|");
        }
        char itemslist[100] = ""; //make  the items list.   Changed items to itemslist due to warning.
        char itemcount[100] = ""; //item counts
        for(int j=0;j<5;j++)
        {
            int ItemID = thisclient->quest.quests[i].Items[j].itemtype * 1000 + thisclient->quest.quests[i].Items[j].itemnum;
            //sprintf(&items[strlen(items)], "%i",thisclient->quest.quests[i].Items[j].itemnum);
            sprintf(&itemslist[strlen(itemslist)], "%i",ItemID);
            sprintf(&itemcount[strlen(itemcount)], "%i",thisclient->quest.quests[i].Items[j].count);
            if(j < 4)
            {
                sprintf(&itemslist[strlen(itemslist)],"|");
                sprintf(&itemcount[strlen(itemcount)],"|");
            }
        }
        char episode[50] = ""; //make  the episode variable list
        for(int j=0;j<5;j++)
        {
            sprintf(&episode[strlen(episode)], "%u",thisclient->quest.EpisodeVar[j]);
            if(j < 4)
                 sprintf(&episode[strlen(episode)],"|");
        }
        char jobvar[30] = ""; //make  the job variable list
        for(int j=0;j<3;j++)
        {
            sprintf(&jobvar[strlen(jobvar)], "%u",thisclient->quest.JobVar[j]);
            if(j < 2)
                 sprintf(&jobvar[strlen(jobvar)],"|");
        }
        char planetvar[70] = ""; //make  the planet variable list
        for(int j=0;j<7;j++)
        {
            sprintf(&planetvar[strlen(planetvar)], "%u",thisclient->quest.PlanetVar[j]);
            if(j < 6)
                 sprintf(&planetvar[strlen(planetvar)],"|");
        }
        char unionvar[100] = ""; //make  the union variable list
        for(int j=0;j<10;j++)
        {
            sprintf(&unionvar[strlen(unionvar)], "%u",thisclient->quest.UnionVar[j]);
            if(j < 9)
                 sprintf(&unionvar[strlen(unionvar)],"|");
        }
        GServer->DB->QExecute("INSERT INTO player_quests (owner, questid, starttime, vbl, switches, items, count, episodevar, jobvar, planetvar, unionvar) VALUES(%i,%i,%i,'%s','%s','%s','%s','%s','%s','%s','%s')",
        thisclient->CharInfo->charid, thisclient->quest.quests[i].QuestID, thisclient->quest.quests[i].StartTime,
        vbl, switches, itemslist, itemcount, episode, jobvar, planetvar, unionvar);
        //Log(MSG_DEBUG,"Ran the player_quests insertion query");
    }
    //GServer->DB->QFree( );
    //now the quest flags
    GServer->DB->QExecute( "DELETE FROM questflags WHERE owner=%i",thisclient->CharInfo->charid );
    //Log(MSG_DEBUG,"Ran the questflag deletion query");
    char qflags[10000] = "";
    int flagCount = 0;
    /*
    for(int i=0;i<1000;i++)
    {
        if(thisclient->quest.Qflags[i] == 1)
        {
            if(sizeof(&qflags) > 995)
            {
                Log(MSG_WARNING,"Quest flag list too big for player %s",thisclient->CharInfo->charname);
                i = 999;
                continue;
            }
            if(flagCount == 0)
            {
                sprintf(&qflags[strlen(qflags)], "%i",i);
                flagCount++;
            }
            else
            {
                sprintf(&qflags[strlen(qflags)],"|");
                sprintf(&qflags[strlen(qflags)], "%i",i);
                flagCount++;
            }
        }
    }
    */

    for(int i=0;i<0x40;i++)
    {
        for(int j=0;j<8;j++)
        {
            if(thisclient->quest.Qflags[i][j] != 0)
            {
                int flagid = (i*8)+j;
                GServer->DB->QExecute("INSERT INTO questflags (owner, flags) VALUES(%i,%i)",
                    thisclient->CharInfo->charid, flagid);
                //Log(MSG_DEBUG,"Ran the questflag deletion query");
                flagCount++;
            }
        }
    }
    //Log(MSG_DEBUG,"%i Quest flags detected during save process",flagCount);

}

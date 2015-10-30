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
#include "player.h"
#include "worldserver.h"


// Load this characters userdata from the database
bool CPlayer::loaddata( )
{
	MYSQL_ROW row;
	MYSQL_RES *result = GServer->DB->QStore("SELECT level,face,hairStyle,sex,classid,zuly,str,dex,_int, con,cha,sen,curHp,curMp,id,statp,skillp,exp,stamina,quickbar,basic_skills, class_skills,class_skills_level,respawnid,clanid,clan_rank,townid,rewardpoints,unionid,unionfame,union01,union02,union03,union04,union05,bonusxp,timerxp,shoptype,timershop,isGM,unique_skills,mileage_skills,driving_skills,unique_skills_level,mileage_skills_level,bonusddrop,timerddrop,bonusstatdrop,timerstatdrop,bonusgraydrop,timergraydrop,isDev FROM characters WHERE char_name='%s'", CharInfo->charname);
	if(result==NULL) return false;
	if(mysql_num_rows(result)!=1)
	{
        Log( MSG_WARNING, "Number of user with charname '%s' is %i", CharInfo->charname,mysql_num_rows(result));
        return false;
    }
	row = mysql_fetch_row(result);
    last_map=-1;	//LMA: maps
    last_coords=-1;	//LMA: maps
	Stats->Level = atoi(row[0]);
	CharInfo->Face = atoi(row[1]);
	CharInfo->Hair = atoi(row[2]);
	CharInfo->Sex = atoi(row[3]);
	CharInfo->Job = atoi(row[4]);
	CharInfo->Zulies = atol(row[5]);
	//CharInfo->Zulies = atoll(row[5]);	//atoll doesn't seem to compile properly in VS2010 using c++ 11. Come back to this later
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
	CharInfo->MaxStamina = 5000;
    Position->respawn = atoi(row[23]);
    Clan->clanid = atoi(row[24]);
    Clan->clanrank = atoi(row[25]);
    Position->saved = atoi(row[26]);
    CharInfo->rewardpoints=atoi(row[27]); //LMA: reward points.
    //Union
    CharInfo->unionid=atoi(row[28]);
    CharInfo->unionfame=atoi(row[29]);
    CharInfo->union01=atoi(row[30]);
    CharInfo->union02=atoi(row[31]);
    CharInfo->union03=atoi(row[32]);
    CharInfo->union04=atoi(row[33]);
    CharInfo->union05=atoi(row[34]);

    //LMA: Pvp according to union:
    //LMA²: Will be set by QSDs now...
    pvp_id=-1;

    //LMA: mileage stuff
    bonusxp=atoi(row[35]);
    timerxp=atoi(row[36]);

    bonusddrop=atoi(row[45]);
    timerddrop=atoi(row[46]);
    bonusstatdrop=atoi(row[47]);
    timerstatdrop=atoi(row[48]);
    bonusgraydrop=atoi(row[49]);
    timergraydrop=atoi(row[50]);

    wait_validation=0;
    wait_validation_ddrop=0;
    wait_validation_statdrop=0;
    wait_validation_graydrop=0;

    last_fuel=0;

    //LMA: shout hack.
    next_shout=0;
    spam_shout=false;
    spam_chat=false;
    spam_whisper=false;
    next_chat=0;
    next_whisper=0;
    nb_chats=0;
    nb_whispers=0;
    nb_chat_union=0;
    next_chat_union=0;
    spam_chat_union=false;
    nb_chat_trade=0;
    next_chat_trade=0;
    spam_chat_trade=false;

    Shop->ShopType=atoi(row[37]);
    Shop->mil_shop_time=atoi(row[38]);

    //GM Additional Security by PurpleYouKo
    //CharInfo->isGM=atoi(row[45]);     // GM Security
    CharInfo->isGM=atoi(row[39]);       // GM Security
    CharInfo->isDev = atoi(row[51]);    // Dev Security

    //resetting some values:
    sp_hp=0;
    sp_mp=0;
    nb_hp=0;
    nb_mp=0;

    //TEST
    time_t rawtime;
    struct tm * timeinfo;

    if (Shop->mil_shop_time>0)
    {
        rawtime=Shop->mil_shop_time;
        timeinfo = localtime ( &rawtime );
        Log(MSG_INFO,"%s:: Shop %i/%i/%i, %i:%i:%i",CharInfo->charname,timeinfo->tm_year+1900,timeinfo->tm_mon+1,timeinfo->tm_mday,timeinfo->tm_hour+1,timeinfo->tm_min+1,timeinfo->tm_sec+1);
    }

    if(timerxp>0)
    {
        rawtime=timerxp;
        timeinfo = localtime ( &rawtime );
        Log(MSG_INFO,"%s:: Bonus Xp %i/%i/%i, %i:%i:%i",CharInfo->charname,timeinfo->tm_year+1900,timeinfo->tm_mon+1,timeinfo->tm_mday,timeinfo->tm_hour+1,timeinfo->tm_min+1,timeinfo->tm_sec+1);
    }

    if(timerddrop>0)
    {
        rawtime=timerddrop;
        timeinfo = localtime ( &rawtime );
        Log(MSG_INFO,"%s:: Medal of Fortune %i/%i/%i, %i:%i:%i",CharInfo->charname,timeinfo->tm_year+1900,timeinfo->tm_mon+1,timeinfo->tm_mday,timeinfo->tm_hour+1,timeinfo->tm_min+1,timeinfo->tm_sec+1);
    }

    if(timerstatdrop>0)
    {
        rawtime=timerstatdrop;
        timeinfo = localtime ( &rawtime );
        Log(MSG_INFO,"%s:: Medal of Excellence %i/%i/%i, %i:%i:%i",CharInfo->charname,timeinfo->tm_year+1900,timeinfo->tm_mon+1,timeinfo->tm_mday,timeinfo->tm_hour+1,timeinfo->tm_min+1,timeinfo->tm_sec+1);
    }

    if(timergraydrop>0)
    {
        rawtime=timergraydrop;
        timeinfo = localtime ( &rawtime );
        Log(MSG_INFO,"%s:: Medal of Retrieval %i/%i/%i, %i:%i:%i",CharInfo->charname,timeinfo->tm_year+1900,timeinfo->tm_mon+1,timeinfo->tm_mday,timeinfo->tm_hour+1,timeinfo->tm_min+1,timeinfo->tm_sec+1);
    }
    //End of test

    time_t etime=time(NULL);
    if(bonusxp<=0) bonusxp=1; //exp *
    if(bonusddrop<=0) bonusddrop=1; //drop *
    if(bonusstatdrop<=0) bonusstatdrop=1;   // %
    if(bonusgraydrop<0) bonusgraydrop=0; //yes or no (1 or 0)?

    if(bonusxp>1&&(etime>=timerxp)) {bonusxp=1; timerxp=0;}
    if(bonusddrop>1&&(etime>=timerddrop))   {bonusddrop=1; timerddrop=0;}
    if(bonusstatdrop>1&&(etime>=timerstatdrop)) {bonusstatdrop=1; timerstatdrop=0;}
    if(bonusgraydrop>0&&(etime>=timergraydrop)) {bonusgraydrop=0; timergraydrop=0;}
    if(bonusddrop>1&&(etime>=timerddrop))
    {
      bonusddrop=1;
      timerddrop=0;
    }
    if(bonusstatdrop>1&&(etime>=timerstatdrop))
    {
      bonusstatdrop=1;
      timerstatdrop=0;
    }
    if(bonusgraydrop>0&&(etime>=timergraydrop))
    {
      bonusgraydrop=0;
      timergraydrop=0;
    }
    if (Shop->ShopType>0&&(etime>=Shop->mil_shop_time))
    {
     Shop->ShopType=0;
     Shop->mil_shop_time=0;
    }

	p_skills = 0;
	for(BYTE i=0;i<48;i++)
    {
        char* tmp=strtok((i==0?row[19]:NULL), ",");
        if (tmp!=NULL)
            quickbar[i]=atoi(tmp);
        else
            quickbar[i]=0;
    }

    //LMA: new way BEGIN
    bool do_save=false;
    int cur_cskills[5];
    int max_skills[5];
    int coff[MAX_CLASS_SKILL];
    int uoff[MAX_UNIQUE_SKILL];
    int moff[MAX_MILEAGE_SKILL];
    const char* tab_names[]={"class","driving","basic","unique","mileage"};

    int good_family=0;
    cur_cskills[0]=0;
    cur_cskills[1]=60;
    cur_cskills[2]=320;
    cur_cskills[3]=90;
    cur_cskills[4]=120;

    max_skills[0]=60;
    max_skills[1]=90;
    max_skills[2]=362;
    max_skills[3]=120;
    max_skills[4]=320;

    //Browsing supposed basic skills.
    good_family=2;
	for(UINT i=0;i<MAX_BASIC_SKILL;i++)
    {
        char* tmp=strtok((i==0?row[20]:NULL), ",");
        if (tmp==NULL)
            continue;

        int temp=atoi(tmp);
        if (temp==0)
            continue;
        int indexfamily=GoodSkill(temp);
        if (indexfamily==-1)
        {
            Log(MSG_WARNING,"Incorrect skill detected %i",temp);
            do_save=true;
            continue;
        }

        if (indexfamily!=good_family)
        {
            Log(MSG_WARNING,"Skill %i should be in family %s instead of family %s",temp,tab_names[indexfamily],tab_names[good_family]);
            do_save=true;
        }

        if(cur_cskills[indexfamily]>=max_skills[indexfamily])
        {
            Log(MSG_WARNING,"Already too much skills in family %s, no room for skill %i",tab_names[indexfamily],temp);
            do_save=true;
            continue;
        }

        cskills[cur_cskills[indexfamily]].id=temp;
        cskills[cur_cskills[indexfamily]].level=1;
        cur_cskills[indexfamily]++;
    }

    //Browsing unique skills.
    good_family=1;
	for(UINT i=0;i<MAX_DRIVING_SKILL;i++)
    {
        char* tmp=strtok((i==0?row[42]:NULL), ",");
        if (tmp==NULL)
            continue;

        int temp=atoi(tmp);
        if (temp==0)
            continue;
        int indexfamily=GoodSkill(temp);
        if (indexfamily==-1)
        {
            Log(MSG_WARNING,"Incorrect skill detected %i",temp);
            do_save=true;
            continue;
        }

        if (indexfamily!=good_family)
        {
            Log(MSG_WARNING,"Skill %i should be in family %s instead of family %s",temp,tab_names[indexfamily],tab_names[good_family]);
            do_save=true;
        }

        if(cur_cskills[indexfamily]>=max_skills[indexfamily])
        {
            Log(MSG_WARNING,"Already too much skills in family %s, no room for skill %i",tab_names[indexfamily],temp);
            do_save=true;
            continue;
        }

        cskills[cur_cskills[indexfamily]].id=temp;
        cskills[cur_cskills[indexfamily]].level=1;
        cur_cskills[indexfamily]++;
    }

    //unique skills.
    good_family=3;
	for(UINT i=0;i<MAX_UNIQUE_SKILL;i++)
    {
        uoff[i]=-1;
        char* tmp=strtok((i==0?row[40]:NULL), ",");
        if (tmp==NULL)
            continue;

        int temp=atoi(tmp);
        if (temp==0)
            continue;
        int indexfamily=GoodSkill(temp);
        if (indexfamily==-1)
        {
            Log(MSG_WARNING,"Incorrect skill detected %i",temp);
            do_save=true;
            continue;
        }

        if (indexfamily!=good_family)
        {
            Log(MSG_WARNING,"Skill %i should be in family %s instead of family %s",temp,tab_names[indexfamily],tab_names[good_family]);
            do_save=true;
        }

        if(cur_cskills[indexfamily]>=max_skills[indexfamily])
        {
            Log(MSG_WARNING,"Already too much skills in family %s, no room for skill %i",tab_names[indexfamily],temp);
            do_save=true;
            continue;
        }

        cskills[cur_cskills[indexfamily]].id=temp;
        cskills[cur_cskills[indexfamily]].level=1;
        uoff[i]=cur_cskills[indexfamily];
        cur_cskills[indexfamily]++;
    }

    //mileage skills.
    good_family=4;
	for(UINT i=0;i<MAX_MILEAGE_SKILL;i++)
    {
        moff[i]=-1;
        char* tmp=strtok((i==0?row[41]:NULL), ",");
        if (tmp==NULL)
            continue;

        int temp=atoi(tmp);
        if (temp==0)
            continue;
        int indexfamily=GoodSkill(temp);
        if (indexfamily==-1)
        {
            Log(MSG_WARNING,"Incorrect skill detected %i",temp);
            do_save=true;
            continue;
        }

        if (indexfamily!=good_family)
        {
            Log(MSG_WARNING,"Skill %i should be in family %s instead of family %s",temp,tab_names[indexfamily],tab_names[good_family]);
            do_save=true;
        }

        if(cur_cskills[indexfamily]>=max_skills[indexfamily])
        {
            Log(MSG_WARNING,"Already too much skills in family %s, no room for skill %i",tab_names[indexfamily],temp);
            do_save=true;
            continue;
        }

        cskills[cur_cskills[indexfamily]].id=temp;
        cskills[cur_cskills[indexfamily]].level=1;
        moff[i]=cur_cskills[indexfamily];
        cur_cskills[indexfamily]++;
    }

    //class skills.
    good_family=0;
	for(UINT i=0;i<MAX_CLASS_SKILL;i++)
    {
        coff[i]=-1;
        char* tmp=strtok((i==0?row[21]:NULL), ",");
        if (tmp==NULL)
            continue;

        int temp=atoi(tmp);
        if (temp==0)
            continue;
        int indexfamily=GoodSkill(temp);
        if (indexfamily==-1)
        {
            Log(MSG_WARNING,"Incorrect skill detected %i",temp);
            do_save=true;
            continue;
        }

        if (indexfamily!=good_family)
        {
            Log(MSG_WARNING,"Skill %i should be in family %s instead of family %s",temp,tab_names[indexfamily],tab_names[good_family]);
            do_save=true;
        }

        if(cur_cskills[indexfamily]>=max_skills[indexfamily])
        {
            Log(MSG_WARNING,"Already too much skills in family %s, no room for skill %i",tab_names[indexfamily],temp);
            do_save=true;
            continue;
        }

        cskills[cur_cskills[indexfamily]].id=temp;
        cskills[cur_cskills[indexfamily]].level=1;
        coff[i]=cur_cskills[indexfamily];
        cur_cskills[indexfamily]++;
    }

    //getting class levels now.
	for(UINT i=0;i<MAX_CLASS_SKILL;i++)
    {
        char* tmp=strtok((i==0?row[22]:NULL), ",");
        int temp=1;
        if (tmp!=NULL)
            temp=atoi(tmp);

        if (coff[i]==-1)
            continue;
        if (temp==0)
            temp=1;
        cskills[coff[i]].level=temp;

        if(cskills[coff[i]].id!=0)
            cskills[coff[i]].thisskill = GServer->GetSkillByID( cskills[coff[i]].id+cskills[coff[i]].level-1 );
    }

    //unique skills levels.
	for(UINT i=0;i<MAX_UNIQUE_SKILL;i++)
    {
        char* tmp=strtok((i==0?row[43]:NULL), ",");
        int temp=1;
        if (tmp!=NULL)
            temp=atoi(tmp);

        if (uoff[i]==-1)
            continue;
        if (temp==0)
            temp=1;
        cskills[uoff[i]].level=temp;

        if(cskills[uoff[i]].id!=0)
            cskills[uoff[i]].thisskill = GServer->GetSkillByID( cskills[uoff[i]].id+cskills[uoff[i]].level-1 );
    }

    //mileage skills levels.
	for(UINT i=0;i<MAX_MILEAGE_SKILL;i++)
    {
        char* tmp=strtok((i==0?row[43]:NULL), ",");
        int temp=1;
        if (tmp!=NULL)
            temp=atoi(tmp);

        if (moff[i]==-1)
            continue;
        if (temp==0)
            temp=1;
        cskills[moff[i]].level=temp;

        if(cskills[moff[i]].id!=0)
            cskills[moff[i]].thisskill = GServer->GetSkillByID( cskills[moff[i]].id+cskills[moff[i]].level-1 );
    }

    //First free offset.
    for (int i=0;i<5;i++)
        cur_max_skills[i]=cur_cskills[i];

    //LMA: reset inventory.
    for(int i=0;i<MAX_INVENTORY;i++)
    {
        items[i].sig_data=-1;
        items[i].sig_head=-1;
        items[i].sig_gem=-1;
        items[i].sp_value=0;
        items[i].last_sp_value=0;
    }

	GServer->DB->QFree( );

	//LMA: Saving Skills if needed.
    if (do_save)
    {
        saveskills();
    }

    //LMA: % rebate and bonus union points and craft talent.
    pc_rebate=0;
    pc_up=0;
    pc_craft_talent=0;

    //LMA: summon jauge:
    summon_jauge=50;
    cur_jauge=0;

    for (int i=0;i<MAX_ALL_SKILL;i++)
    {
        if (cskills[i].thisskill==NULL)
        {
            continue;
        }

        for(int j=0;j<3;j++)
        {
            if(cskills[i].thisskill->buff[j]==0)
            {
                continue;
            }

            if (cskills[i].thisskill->buff[j]==59)
            {
                pc_rebate+=cskills[i].thisskill->value2[j];
                continue;
            }

            if (cskills[i].thisskill->buff[j]==70)
            {
                pc_craft_talent+=cskills[i].thisskill->value1[j];
                continue;
            }

            //LMA: summon jauge.
            if (cskills[i].thisskill->buff[j]==62)
            {
                summon_jauge+=cskills[i].thisskill->value1[j];
                continue;
            }

        }

        //Union points (no stats?)
        if (cskills[i].thisskill->id==5500)
        {
            pc_up=50;
        }

        //LMA: we reset the skills cooldown.
        cskills[i].cooldown_skill=0;
    }

    Log(MSG_INFO,"%s:: craft talent %%bonus is %i",CharInfo->charname,pc_craft_talent);

    //Log(MSG_INFO,"%s:: summon jauge %i",CharInfo->charname,summon_jauge);

	result = GServer->DB->QStore("SELECT itemnum,itemtype,refine,durability,lifespan,slotnum,count,stats,socketed,appraised,gem,sp_value FROM items WHERE owner=%i", CharInfo->charid);
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
		items[itemnum].sp_value = atoi(row[11]);

		//LMA: checking the count too :(
        if((items[itemnum].itemtype<10||items[itemnum].itemtype>12)&&items[itemnum].count!=1)
        {
            Log(MSG_WARNING,"%s:: Wrong item quantity in slot %i, %u*(%u::%u), reseting it to 1.",CharInfo->charname,itemnum,items[itemnum].count,items[itemnum].itemtype,items[itemnum].itemnum);
            items[itemnum].count=1;
        }

		CalculateSignature(itemnum);  //Calculate signature.

        //LMA: up to refine 15 now (2010/05)...
        switch (items[itemnum].refine)
        {
            case 0:
            case 16:
            case 32:
            case 48:
            case 64:
            case 80:
            case 96:
            case 112:
            case 128:
            case 144:
            #ifdef REFINENEW
            case 160:
            case 176:
            case 192:
            case 208:
            case 224:
            case 240:
            #endif
            {
                //Ok.
            }
            break;
            default:
            {
                Log(MSG_WARNING,"Invalid refine %i for item (%i:%i) for %s",items[itemnum].refine,items[itemnum].itemtype,items[itemnum].itemnum,CharInfo->charname);
                items[itemnum].refine=0;
            }
            break;

        }

	}

	GServer->DB->QFree( );
	result = GServer->DB->QStore("SELECT itemnum,itemtype,refine,durability,lifespan,slotnum,count,stats,socketed,appraised,gem FROM storage WHERE owner=%i", Session->userid);
	if(result==NULL) return false;
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
	}
	GServer->DB->QFree( );

	//LMA: Loading ItemMall
    RebuildItemMall();

    if(Clan->clanid!=0)
    {
    	result = GServer->DB->QStore("SELECT logo,back,name,grade,cp FROM list_clan where id=%i", Clan->clanid);
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
    	    Clan->grade = atoi(row[3]);
    	    Clan->CP= atoi(row[4]);
        }
    	GServer->DB->QFree( );
    }

    //Quest, QSD version
    memset(&quest, 0, sizeof(SQuestData));
    result = GServer->DB->QStore("SELECT quests from list_quest_new where owner=%i",CharInfo->charid );
    if(result!=NULL) //return false;
    {
        while(row = mysql_fetch_row(result))
        {
           if( row[0] != NULL ) memcpy(&quest, row[0], sizeof(SQuestData));
        }
    }
    GServer->DB->QFree( );

	Session->isLoggedIn = true;
    GServer->DB->QExecute("UPDATE accounts SET online=true where id=%u", Session->userid );

    // Calculate how many fairies are available online, according to amount of players
	int oldFairyMax = GServer->Config.FairyMax;
    GServer->Config.FairyMax = (int)ceil((float)GServer->ClientList.size() / 50.0); //(1 fairy more every 50 player)
	//if( oldFairyMax < GServer->Config.FairyMax )
	if( GServer->Config.FairyMax > GServer->FairyList.size() )
	{
	    //LMA: Adding a fairy
	    Log(MSG_INFO,"Adding a fairy, needed %u, we have %u",GServer->Config.FairyMax,GServer->FairyList.size());
        CFairy* thisfairy = new (nothrow) CFairy;
        thisfairy->LastTime = clock();
        thisfairy->assigned = 0;
        thisfairy->ListIndex = 0;
        thisfairy->WaitTime = GServer->Config.FairyWait;
        GServer->FairyList.push_back( thisfairy );
    }

    //LMA: test for quest hack (stackable).
    #ifdef QHACK
    die_quest=0;
    for(int k=0;k<10;k++)
    {
        arr_questid[k].questid=0;
        arr_questid[k].die_time=0;
    }
    #endif
    //LMA: end

	return true;
}

//LMA: Calculating item signature
void CPlayer::CalculateSignature( int slot )
{
     long int res_head=-1;
     long int res_data=-1;
     int res=1;


     if(items[slot].itemnum==0)
     {
        items[slot].sig_head=-1;
        items[slot].sig_data=-1;
        items[slot].sig_gem=-1;
        items[slot].sp_value=-1;
        items[slot].last_sp_value=-1;
        return;
     }

     res_head=(long int)pow((double)2,5)*items[slot].itemnum+items[slot].itemtype;

     if( items[slot].itemtype >= 10 && items[slot].itemtype <= 13 )
     {
         res_data=items[slot].count;
     }
     else
     {
        res_data=items[slot].stats;
        if(items[slot].socketed)
        		res_data+=(long int)pow((double)2,26);
        if(items[slot].appraised)
		        res_data+=(long int)pow((double)2,27);

        res_data+=(long int)pow((double)2,16)*(items[slot].lifespan*10);
		res_data+=(long int)pow((double)2,9)*items[slot].durability;
		res_data+=(long int)pow((double)2,28)*items[slot].refine;
     }

     items[slot].sig_head=res_head;
     items[slot].sig_data=res_data;
     items[slot].sig_gem=items[slot].gem;

     //Special PAT case
     if(items[slot].itemtype == 14&&items[slot].sp_value<=0&&items[slot].lifespan>0)
     {
        items[slot].sp_value=10*items[slot].lifespan;
        items[slot].last_sp_value=-1;
     }
     else
     {
         items[slot].last_sp_value=items[slot].sp_value;
     }

     if(items[slot].last_sp_value>1000)
     {
        Log(MSG_INFO,"ERROR sp_value %s:: Previous value %i (sp_value %i, lifespan %u) for item %u::%u slot %i",CharInfo->charname,items[slot].last_sp_value,items[slot].sp_value,items[slot].lifespan,items[slot].itemtype,items[slot].itemnum,slot);
        items[slot].sp_value=items[slot].lifespan*10;
        items[slot].last_sp_value=-1;
        //GServer->DB->QExecuteUpdate("UPDATE items SET sp_value=%i WHERE owner=%i",items[slot].sp_value,CharInfo->charid);
     }


     return;
}


//LMA: Checking item signature.
//0=delete
//1=add/update
//2=do nothing
int CPlayer::CheckSignature( int slot )
{
     long int res_head=-1;
     long int res_data=-1;
     int res=1;


     if(items[slot].itemnum==0)
     {
        items[slot].sig_head=0;
        items[slot].sig_data=0;
        items[slot].sig_gem=0;
        return 0;
     }

     //res_head=(long int)pow(2,5)*items[slot].itemnum+items[slot].itemtype;
     res_head=(long int)GServer->BuildItemHead(items[slot]);

     if( items[slot].itemtype >= 10 && items[slot].itemtype <= 13 )
     {
         res_data=items[slot].count;
         //Log(MSG_INFO,"item: %i(%i*[%i:%i]), data: (%i:%i)",slot,items[slot].count,items[slot].itemtype,items[slot].itemnum,res_data,items[slot].sig_data,res_head,items[slot].sig_head);
         if ((items[slot].sig_head==res_head)&&(items[slot].sig_data==res_data))
            return 2;
     }
     else
     {
         res_data=(long int)GServer->BuildItemData(items[slot]);
		//Log(MSG_INFO,"Signature slot %i: %i*(%i::%i), data: (%li:%li), head(%li:%li)",slot,items[slot].count,items[slot].itemtype,items[slot].itemnum,res_data,items[slot].sig_data,res_head,items[slot].sig_head);

        //special PAT handling
		if(items[slot].itemtype == 14)
		{
             if(items[slot].sp_value<=0&&items[slot].lifespan>0)
             {
                items[slot].sp_value=10*items[slot].lifespan;
                items[slot].last_sp_value=-1;
             }

            if ((items[slot].sig_head==res_head)&&(items[slot].sig_data==res_data)&&(items[slot].sig_gem==items[slot].gem)&&(items[slot].last_sp_value==items[slot].sp_value))
               return 2;
        }
        else
        {
            if ((items[slot].sig_head==res_head)&&(items[slot].sig_data==res_data)&&(items[slot].sig_gem==items[slot].gem))
               return 2;
        }
     }

     items[slot].sig_head=res_head;
     items[slot].sig_data=res_data;
     items[slot].sig_gem=items[slot].gem;
     items[slot].last_sp_value=items[slot].sp_value;


     return res;
}

// This is a REALLY bad way of saving the character data, but it works ^^
//LMA: should be more efficient now ;)
void CPlayer::savedata( )
{
    lastSaveTime = clock();
    if(Session->userid!=0)
    {
        CMap* map = GServer->MapList.Index[Position->Map];
    	CRespawnPoint* thisrespawn = map->GetNearRespawn( this );
    	if(thisrespawn == NULL)
    	   Position->respawn = 4;
    	else
    	   Position->respawn = thisrespawn->id;
	    char quick[1024];

	    //LMA: we save the skills elsewhere now :)
        for(UINT i=0;i<48;i++)
        {
            if(i==0)
            	sprintf(&quick[i], "%i",quickbar[i]);
            else
                sprintf(&quick[strlen(quick)], ",%i",quickbar[i]);
        }

        //long int hp = Stats->HP;
        long long hp = Stats->HP;
    	if(hp<=0)
    	   hp=Stats->MaxHP * 10 / 100;
	   if(Stats->MP<0)
	       Stats->MP=0;

        //LMA: bonus XP (coupon)
        int temp_xp=bonusxp;
        time_t temp_timer=timerxp;
        if(once)    {temp_xp=0; temp_timer=0;}

        //Medal of Fortune
        int temp_ddrop=bonusddrop;
        time_t temp_timer_ddrop=timerddrop;
        if(once_ddrop)  {temp_ddrop=0; temp_timer_ddrop=0;}

        //Medal of Excellence
        int temp_statdrop=bonusstatdrop;
        time_t temp_timer_statdrop=timerstatdrop;
        if(once_statdrop)  {temp_statdrop=0; temp_timer_statdrop=0;}

        //Medal of Retrieval
        int temp_graydrop=bonusgraydrop;
        time_t temp_timer_graydrop=timergraydrop;
        if(once_graydrop)  {temp_graydrop=0; temp_timer_graydrop=0;}


        //LMA: new save.
        //GServer->DB->QExecute("UPDATE characters SET classid=%i,level=%i,zuly=%i,curHp=%i,curMp=%i,str=%i,con=%i,dex=%i,_int=%i,cha=%i,sen=%i,exp=%i,skillp=%i,statp=%i, stamina=%i,quickbar='%s',respawnid=%i,clanid=%i,clan_rank=%i, townid=%i, rewardpoints=%i, bonusxp=%i, timerxp=%i, shoptype=%i, timershop=%i, unionid=%i, unionfame=%i, union01=%i, union02=%i, union03=%i, union04=%i, union05=%i WHERE id=%i",
        GServer->DB->QExecute("UPDATE characters SET classid=%i,level=%i,zuly=%I64i,curHp=%i,curMp=%i,str=%i,con=%i,dex=%i,_int=%i,cha=%i,sen=%i,exp=%i,skillp=%i,statp=%i, stamina=%i,quickbar='%s',respawnid=%i,clanid=%i,clan_rank=%i, townid=%i, rewardpoints=%i, bonusxp=%i, timerxp=%i, shoptype=%i, timershop=%i, unionid=%i, unionfame=%i, union01=%i, union02=%i, union03=%i, union04=%i, union05=%i, bonusddrop=%i, timerddrop=%i, bonusstatdrop=%i, timerstatdrop=%i, bonusgraydrop=%i, timergraydrop=%i WHERE id=%i",

                    CharInfo->Job,Stats->Level, CharInfo->Zulies, (UINT) hp, (UINT) Stats->MP,
                    Attr->Str,Attr->Con,Attr->Dex,Attr->Int,Attr->Cha,Attr->Sen,
                    (UINT) CharInfo->Exp,CharInfo->SkillPoints,CharInfo->StatPoints,CharInfo->stamina,
                    quick,Position->respawn,Clan->clanid,Clan->clanrank,Position->saved,CharInfo->rewardpoints,temp_xp,temp_timer,Shop->ShopType,Shop->mil_shop_time,
                    CharInfo->unionid,CharInfo->unionfame,CharInfo->union01,CharInfo->union02,CharInfo->union03,CharInfo->union04,CharInfo->union05,
                    temp_ddrop,temp_timer_ddrop,temp_statdrop,temp_timer_statdrop,temp_graydrop,temp_timer_graydrop,CharInfo->charid);

        //LMA: intelligent item save.
    	int res_check=0;
    	int nb_saved=0;
    	int nb_delete=0;
    	int sp_item_value=0;
    	for(UINT i=0;i<MAX_INVENTORY;i++)
        {
            //Already deleted
            if ((items[i].itemnum==0)&&(items[i].sig_data==-1))
               continue;
            res_check=CheckSignature(i);
            //not changed
            if(res_check==2)
               continue;
            //2delete
            if(res_check==0)
            {
                //delete item.
                GServer->DB->QExecute("DELETE FROM items WHERE owner=%i and slotnum=%i", CharInfo->charid,i);
                items[i].sig_data=-1;
                items[i].sig_head=-1;
                items[i].sig_gem=-1;
                items[i].sp_value=-1;
                items[i].last_sp_value=-1;
                nb_delete++;
                continue;
            }

            nb_saved++;
            sp_item_value=0;
            if(items[i].itemtype==14)
              sp_item_value=items[i].sp_value;
            GServer->DB->QExecute("INSERT INTO items (owner,slotnum,itemnum,itemtype,refine,durability,lifespan,count,stats,socketed,appraised,gem,sp_value) VALUES(%i,%i,%i,%i,%i,%i,%i,%i,%i,%i,%i,%i,%i) ON DUPLICATE KEY UPDATE owner=VALUES(owner),itemnum=VALUES(itemnum),itemtype=VALUES(itemtype),refine=VALUES(refine),durability=VALUES(durability),lifespan=VALUES(lifespan),slotnum=VALUES(slotnum),count=VALUES(count),stats=VALUES(stats),socketed=VALUES(socketed),appraised=VALUES(appraised),gem=VALUES(gem),sp_value=VALUES(sp_value)",
    								CharInfo->charid, i, items[i].itemnum, items[i].itemtype,items[i].refine, items[i].durability,
    								items[i].lifespan, items[i].count, items[i].stats, (items[i].socketed?1:0),
    								(items[i].appraised?1:0),items[i].gem,sp_item_value );
    	}

		//QSD Save
		//2do, save on used?
        savequests(this);

		Log(MSG_INFO, "Data Saved for char '%s' ", CharInfo->charname );
    }
}

//LMA: Saving skills here...
void CPlayer::saveskills( )
{
    if(Session->userid==0)
        return;

    int pc_temp=0;
    int pc_union=0;
    int val_summon=0;
    char basic[1024];
    char drive[1024];
    char sclass[1024];
    char slevel[1024];
    char uclass[1024];
    char ulevel[1024];
    char mclass[1024];
    char mlevel[1024];


    pc_craft_talent=0;

    //class skills and level.
    for(UINT i=0;i<MAX_CLASS_SKILL;i++)
    {
        if(i==0)
        {
           sprintf(&sclass[0], "%i",cskills[i].id);
           sprintf(&slevel[0], "%i",cskills[i].level);
        }
        else
        {
           sprintf(&sclass[strlen(sclass)], ",%i",cskills[i].id);
           sprintf(&slevel[strlen(slevel)], ",%i",cskills[i].level);
        }

        //LMA: % dealer rebate and summon jauge and craft talent.
        if (cskills[i].thisskill!=NULL)
        {
            for(int j=0;j<3;j++)
            {
                if(cskills[i].thisskill->buff[j]==0)
                {
                    continue;
                }

                if (cskills[i].thisskill->buff[j]==59)
                {
                    pc_temp+=cskills[i].thisskill->value2[j];
                    continue;
                }

                if (cskills[i].thisskill->buff[j]==70)
                {
                    pc_craft_talent+=cskills[i].thisskill->value1[j];
                    continue;
                }

                //LMA: summon jauge.
                if (cskills[i].thisskill->buff[j]==62)
                {
                    val_summon+=cskills[i].thisskill->value1[j];
                    continue;
                }

            }

        }

    }

    //unique skills.
    for(UINT i=90;i<90+MAX_UNIQUE_SKILL;i++)
    {
        if(i==90)
        {
           sprintf(&uclass[0], "%i",cskills[i].id);
           sprintf(&ulevel[0], "%i",cskills[i].level);
        }
        else
        {
           sprintf(&uclass[strlen(uclass)], ",%i",cskills[i].id);
           sprintf(&ulevel[strlen(ulevel)], ",%i",cskills[i].level);
        }

        //LMA: % dealer rebate and craft talent.
        if (cskills[i].thisskill!=NULL)
        {
            for(int j=0;j<3;j++)
            {
                if(cskills[i].thisskill->buff[j]==0)
                {
                    continue;
                }

                if (cskills[i].thisskill->buff[j]==59)
                {
                    pc_temp+=cskills[i].thisskill->value2[j];
                    continue;
                }

                if (cskills[i].thisskill->buff[j]==70)
                {
                    pc_craft_talent+=cskills[i].thisskill->value1[j];
                    continue;
                }

                //LMA: summon jauge.
                if (cskills[i].thisskill->buff[j]==62)
                {
                    val_summon+=cskills[i].thisskill->value1[j];
                    continue;
                }

            }

        }

    }

    //mileage skills and level.
    for(UINT i=120;i<120+MAX_MILEAGE_SKILL;i++)
    {
        if(i==120)
        {
           sprintf(&mclass[0], "%i",cskills[i].id);
           sprintf(&mlevel[0], "%i",cskills[i].level);
        }
        else
        {
           sprintf(&mclass[strlen(mclass)], ",%i",cskills[i].id);
           sprintf(&mlevel[strlen(mlevel)], ",%i",cskills[i].level);
        }

        //LMA: % dealer rebate and other stuff.
        if (cskills[i].thisskill!=NULL)
        {
            for(int j=0;j<3;j++)
            {
                if(cskills[i].thisskill->buff[j]==0)
                {
                    continue;
                }

                if (cskills[i].thisskill->buff[j]==59)
                {
                    pc_temp+=cskills[i].thisskill->value2[j];
                    continue;
                }

                if (cskills[i].thisskill->buff[j]==70)
                {
                    pc_craft_talent+=cskills[i].thisskill->value1[j];
                    continue;
                }

                //LMA: summon jauge.
                if (cskills[i].thisskill->buff[j]==62)
                {
                    val_summon+=cskills[i].thisskill->value1[j];
                    continue;
                }

            }

            //Union points (no stats?)
            if (cskills[i].thisskill->id==5500)
            {
                pc_union=50;
            }

        }

    }

    //basic skills.
    for(UINT i=320;i<320+MAX_BASIC_SKILL;i++)
    {
        if(i==320)
            sprintf(&basic[0], "%i",cskills[i].id);
        else
            sprintf(&basic[strlen(basic)], ",%i",cskills[i].id);

        //LMA: % dealer rebate and craft talent.
        if (cskills[i].thisskill!=NULL)
        {
            for(int j=0;j<3;j++)
            {
                if(cskills[i].thisskill->buff[j]==0)
                {
                    continue;
                }

                if (cskills[i].thisskill->buff[j]==59)
                {
                    pc_temp+=cskills[i].thisskill->value2[j];
                    continue;
                }

                if (cskills[i].thisskill->buff[j]==70)
                {
                    pc_craft_talent+=cskills[i].thisskill->value1[j];
                    continue;
                }

                //LMA: summon jauge.
                if (cskills[i].thisskill->buff[j]==62)
                {
                    val_summon+=cskills[i].thisskill->value1[j];
                    continue;
                }

            }

        }

    }

    //driving skills.
    for(UINT i=60;i<60+MAX_DRIVING_SKILL;i++)
    {
        if(i==60)
            sprintf(&drive[0], "%i",cskills[i]);
        else
            sprintf(&drive[strlen(drive)], ",%i",cskills[i]);

        //LMA: % dealer rebate and craft talent
        if (cskills[i].thisskill!=NULL)
        {
            for(int j=0;j<3;j++)
            {
                if(cskills[i].thisskill->buff[j]==0)
                {
                    continue;
                }

                if (cskills[i].thisskill->buff[j]==59)
                {
                    pc_temp+=cskills[i].thisskill->value2[j];
                    continue;
                }

                if (cskills[i].thisskill->buff[j]==70)
                {
                    pc_craft_talent+=cskills[i].thisskill->value1[j];
                    continue;
                }

                //LMA: summon jauge.
                if (cskills[i].thisskill->buff[j]==62)
                {
                    val_summon+=cskills[i].thisskill->value1[j];
                    continue;
                }

            }

        }

    }

    pc_rebate=pc_temp;
    pc_up=pc_union;
    summon_jauge=50+val_summon;
    Log(MSG_INFO,"%s:: New jauge is %i",CharInfo->charname,summon_jauge);

    //LMA: Saving Skills Data for a player.
    GServer->DB->QExecute("UPDATE characters SET class_skills='%s',class_skills_level='%s',basic_skills='%s',driving_skills='%s',unique_skills='%s',mileage_skills='%s',unique_skills_level='%s',mileage_skills_level='%s' WHERE id=%i",
                sclass,slevel,basic,drive,uclass,mclass,ulevel,mlevel,
                CharInfo->charid);

    Log(MSG_INFO, "Skill Data Saved for char '%s' ", CharInfo->charname );


    return;
}

//For QSD.
unsigned char GetCharVal( char mychar )
{
    switch( mychar )
    {
        case '0':   return 0;
        case '1':   return 1;
        case '2':   return 2;
        case '3':   return 3;
        case '4':   return 4;
        case '5':   return 5;
        case '6':   return 6;
        case '7':   return 7;
        case '8':   return 8;
        case '9':   return 9;
        case 'a':   return 10;
        case 'b':   return 11;
        case 'c':   return 12;
        case 'd':   return 13;
        case 'e':   return 14;
        case 'f':   return 15;
        case 'A':   return 10;
        case 'B':   return 11;
        case 'C':   return 12;
        case 'D':   return 13;
        case 'E':   return 14;
        case 'F':   return 15;
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


void CPlayer::savequests( CPlayer* thisclient )
{
    char* questBuffer = new char[sizeof(SQuestData)*3 +1];
    mysql_real_escape_string(GServer->DB->Mysql, questBuffer, (const char*)&quest, sizeof(SQuestData));
    GServer->DB->QExecute( "DELETE FROM list_quest_new WHERE owner=%i",thisclient->CharInfo->charid );
    GServer->DB->QExecute("INSERT INTO list_quest_new (owner, quests) VALUES(%i,'%s')",
        thisclient->CharInfo->charid, questBuffer);
    delete[] questBuffer;
}


//LMA: getting the family skill.
int CPlayer::GoodSkill(int skill_id)
{
    if (skill_id>GServer->maxSkills)
    {
        Log(MSG_WARNING,"Bad skill_id in GoodSkill (%i>%u)",skill_id,GServer->maxSkills);
        return -1;
    }

    int type=GServer->SkillList[skill_id]->skill_tab;

    if (type==11)
        return 2;   //basic
    if (type==41)
        return 3;   //unique
    if (type==51)
        return 4;   //mileage
    if (type>=20&&type<30)
        return 0;   //class
    if (type>=30&&type<40)
        return 1;   //driving and others?


    return -1;
}

//LMA: Find a skill offset for cskills...
int CPlayer::FindSkillOffset(int family)
{
    if (family>4)
    {
        Log(MSG_WARNING,"Bad family in FindSkillOffset (%i)",family);
        return -1;
    }

    int begin[5];
    int end[5];

    begin[0]=0;
    begin[1]=60;
    begin[2]=320;
    begin[3]=90;
    begin[4]=120;
    end[0]=60;
    end[1]=90;
    end[2]=362;
    end[3]=120;
    end[4]=320;

    int res=cur_max_skills[family];

    if (res<end[family])
    {
        cur_max_skills[family]++;
        return res;
    }


    return -1;
}

//LMA: Find a skill in cskills...
bool CPlayer::FindSkill(int family,UINT skill)
{
    if (family>4)
    {
        Log(MSG_WARNING,"Bad family in FindSkill (%i) for skill %i",family, skill);
        return true;
    }

    int begin[5];
    int end[5];

    begin[0]=0;
    begin[1]=60;
    begin[2]=320;
    begin[3]=90;
    begin[4]=120;
    end[0]=60;
    end[1]=90;
    end[2]=362;
    end[3]=120;
    end[4]=320;

    for (int k=begin[family];k<end[family];k++)
    {
        if (cskills[k].thisskill==NULL)
        {
            continue;
        }

        if(cskills[k].id==skill)
        {
            return true;
        }

    }


    return false;
}


//LMA: reset skill offsets.
void CPlayer::ResetSkillOffset()
{
    int begin[5];


    begin[0]=0;
    begin[1]=60;
    begin[2]=320;
    begin[3]=90;
    begin[4]=120;

    for (int k=0;k<4;k++)
    {
        //We don't reset the basic skills.
        if(k==2)
            continue;
        cur_max_skills[k]=begin[k];
    }


    return;
}

//LMA: searchs all the skills for a player (to be used after allskill gm command for example).
void CPlayer::AttrAllSkills()
{
    for (int k=0;k<MAX_ALL_SKILL;k++)
    {
        if(cskills[k].id==0)
            continue;

        cskills[k].thisskill = GServer->GetSkillByID( cskills[k].id+cskills[k].level-1 );
        if(cskills[k].thisskill==NULL)
        {
            cskills[k].id=0;
            cskills[k].level=1;
        }

    }


    return;
}

//DK: seems to be the same as AttrAllSkills...
void CPlayer::AttrGMSkills()
{
    for (int k=0;k<MAX_ALL_SKILL;k++)
    {
        if(cskills[k].id==0)
            continue;

        cskills[k].thisskill = GServer->GetSkillByID( cskills[k].id+cskills[k].level-1 );
        if(cskills[k].thisskill==NULL)
        {
            cskills[k].id=0;
            cskills[k].level=1;
        }

    }


    return;
}

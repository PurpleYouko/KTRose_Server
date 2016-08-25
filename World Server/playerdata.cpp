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
	MYSQL_RES *result = GServer->DB->QStore("SELECT level,face,hairStyle,sex,classid,zuly,str,dex,_int, con,cha,sen,curHp,curMp,id,statp,skillp,exp,stamina,quickbar,basic_skills, class_skills,class_skills_level,respawnid,clanid,clan_rank,townid,rewardpoints,unionid,unionfame,union01,union02,union03,union04,union05,bonusxp,timerxp,shoptype,timershop,isGM,unique_skills,mileage_skills,driving_skills,unique_skills_level,mileage_skills_level,bonusddrop,timerddrop,bonusstatdrop,timerstatdrop,bonusgraydrop,timergraydrop,isDev,highestoverkill FROM characters WHERE char_name='%s'", CharInfo->charname);
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
	CharInfo->Zulies = atoll(row[5]);	//atoll doesn't exist in c98 (MS compilers prior to 2012). Defined as equal to _atoi64 in CRoseFile.hpp just to keep syntax consistent
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
	CharInfo->Exp = atol(row[17]);
	CharInfo->stamina = atoi(row[18]);
	CharInfo->MaxStamina = 5000;
    Position->respawn = atoi(row[23]);
    Clan->clanid = atoi(row[24]);
    Clan->clanrank = atoi(row[25]);
    Position->saved = atoi(row[26]);
    CharInfo->rewardpoints=atoi(row[27]); //LMA: reward points.
    //Union
    CharInfo->unionid = atoi(row[28]);
    CharInfo->unionfame = atoi(row[29]);
    CharInfo->union01 = atoi(row[30]);
    CharInfo->union02 = atoi(row[31]);
    CharInfo->union03 = atoi(row[32]);
    CharInfo->union04 = atoi(row[33]);
    CharInfo->union05 = atoi(row[34]);
	CharInfo->union06 = 0;
	CharInfo->union07 = 0;
	CharInfo->union08 = 0;
	CharInfo->union09 = 0;
	CharInfo->union10 = 0;


    //LMA: Pvp according to union:
    //LMA²: Will be set by QSDs now...
    pvp_id=-1;

    //LMA: mileage stuff
    bonusxp = atoi(row[35]);
    timerxp = atoi(row[36]);

    bonusddrop = atoi(row[45]);
    timerddrop = atoi(row[46]);
    bonusstatdrop = atoi(row[47]);
    timerstatdrop = atoi(row[48]);
    bonusgraydrop = atoi(row[49]);
    timergraydrop = atoi(row[50]);

    wait_validation = 0;
    wait_validation_ddrop = 0;
    wait_validation_statdrop = 0;
    wait_validation_graydrop = 0;

    last_fuel=0;
	Log( MSG_WARNING, "User '%s' loaded with ID: %i and Str: %i", CharInfo->charname,CharInfo->charid,Attr->Str);
    //LMA: shout hack.
    next_shout = 0;
    spam_shout = false;
    spam_chat = false;
    spam_whisper = false;
    next_chat = 0;
    next_whisper = 0;
    nb_chats = 0;
    nb_whispers = 0;
    nb_chat_union = 0;
    next_chat_union = 0;
    spam_chat_union = false;
    nb_chat_trade = 0;
    next_chat_trade = 0;
    spam_chat_trade = false;

    Shop->ShopType = atoi(row[37]);
    Shop->mil_shop_time = atoi(row[38]);

    //GM Additional Security by PurpleYouKo
    CharInfo->isGM = atoi(row[39]);       // GM Security
    CharInfo->isDev = atoi(row[51]);    // Dev Security

	//just for fun
	CharInfo->HighestOverkill = atof(row[52]);

    //TEST
    time_t rawtime;
    struct tm * timeinfo;

    if (Shop->mil_shop_time > 0)
    {
        rawtime=Shop->mil_shop_time;
        timeinfo = localtime ( &rawtime );
        Log(MSG_INFO,"%s:: Shop %i/%i/%i, %i:%i:%i",CharInfo->charname,timeinfo->tm_year+1900,timeinfo->tm_mon+1,timeinfo->tm_mday,timeinfo->tm_hour+1,timeinfo->tm_min+1,timeinfo->tm_sec+1);
    }

    if(timerxp > 0)
    {
        rawtime = timerxp;
        timeinfo = localtime ( &rawtime );
        Log(MSG_INFO,"%s:: Bonus Xp %i/%i/%i, %i:%i:%i",CharInfo->charname,timeinfo->tm_year+1900,timeinfo->tm_mon+1,timeinfo->tm_mday,timeinfo->tm_hour+1,timeinfo->tm_min+1,timeinfo->tm_sec+1);
    }

    if(timerddrop > 0)
    {
        rawtime = timerddrop;
        timeinfo = localtime ( &rawtime );
        Log(MSG_INFO,"%s:: Medal of Fortune %i/%i/%i, %i:%i:%i",CharInfo->charname,timeinfo->tm_year+1900,timeinfo->tm_mon+1,timeinfo->tm_mday,timeinfo->tm_hour+1,timeinfo->tm_min+1,timeinfo->tm_sec+1);
    }

    if(timerstatdrop > 0)
    {
        rawtime = timerstatdrop;
        timeinfo = localtime ( &rawtime );
        Log(MSG_INFO,"%s:: Medal of Excellence %i/%i/%i, %i:%i:%i",CharInfo->charname,timeinfo->tm_year+1900,timeinfo->tm_mon+1,timeinfo->tm_mday,timeinfo->tm_hour+1,timeinfo->tm_min+1,timeinfo->tm_sec+1);
    }

    if(timergraydrop > 0)
    {
        rawtime=timergraydrop;
        timeinfo = localtime ( &rawtime );
        Log(MSG_INFO,"%s:: Medal of Retrieval %i/%i/%i, %i:%i:%i",CharInfo->charname,timeinfo->tm_year+1900,timeinfo->tm_mon+1,timeinfo->tm_mday,timeinfo->tm_hour+1,timeinfo->tm_min+1,timeinfo->tm_sec+1);
    }
    //End of test

    time_t etime=time(NULL);
    if(bonusxp <= 0) bonusxp = 1; //exp *
    if(bonusddrop <= 0) bonusddrop = 1; //drop *
    if(bonusstatdrop <= 0) bonusstatdrop = 1;   // %
    if(bonusgraydrop < 0) bonusgraydrop = 0; //yes or no (1 or 0)?

    if(bonusxp > 1 && (etime >= timerxp)) {bonusxp = 1; timerxp = 0;}
    if(bonusddrop > 1 && (etime >= timerddrop))   {bonusddrop = 1; timerddrop = 0;}
    if(bonusstatdrop > 1 && (etime >= timerstatdrop)) {bonusstatdrop = 1; timerstatdrop = 0;}
    if(bonusgraydrop > 0 && (etime >= timergraydrop)) {bonusgraydrop = 0; timergraydrop = 0;}
    if(bonusddrop > 1 && (etime >= timerddrop))
    {
      bonusddrop = 1;
      timerddrop = 0;
    }
    if(bonusstatdrop > 1 && (etime >= timerstatdrop))
    {
      bonusstatdrop = 1;
      timerstatdrop = 0;
    }
    if(bonusgraydrop > 0 && (etime>=timergraydrop))
    {
      bonusgraydrop = 0;
      timergraydrop = 0;
    }
    if (Shop->ShopType > 0 && (etime >= Shop->mil_shop_time))
    {
     Shop->ShopType = 0;
     Shop->mil_shop_time = 0;
    }

	p_skills = 0;
	for(BYTE i=0;i<48;i++)
    {
        char* tmp = strtok((i==0?row[19]:NULL), ",");
        if (tmp != NULL)
            quickbar[i] = atoi(tmp);
        else
            quickbar[i] = 0;
    }

	//skills KT method for comparrison
	/*
	for(char i=0;i<MAX_BASIC_SKILL;i++)
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
	*/
	


    //LMA: new way BEGIN
    bool do_save = false;
    int cur_cskills[5];
    int max_skills[5];
    int coff[MAX_CLASS_SKILL];
    int uoff[MAX_UNIQUE_SKILL];
    int moff[MAX_MILEAGE_SKILL];
    const char* tab_names[] = {"class","driving","basic","unique","mileage"};

    int good_family = 0;
    cur_cskills[0] = 0;
    cur_cskills[1] = 60;
    cur_cskills[2] = 320;
    cur_cskills[3] = 90;
    cur_cskills[4] = 120;

    max_skills[0] = 60;
    max_skills[1] = 90;
    max_skills[2] = 362;
    max_skills[3] = 120;
    max_skills[4] = 320;

    //Browsing supposed basic skills.
    good_family = 2;
	for(UINT i=0;i<MAX_BASIC_SKILL;i++)
    {
        char* tmp = strtok((i==0?row[20]:NULL), ",");
        if (tmp == NULL)
            continue;

        int temp = atoi(tmp);
        if (temp == 0)
            continue;
        int indexfamily = GoodSkill(temp);
        if (indexfamily == -1)
        {
            Log(MSG_WARNING,"Incorrect skill detected %i",temp);
            do_save = true;
            continue;
        }

        if (indexfamily != good_family)
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
        if (tmp == NULL)
            continue;

        int temp=atoi(tmp);
        if (temp == 0)
            continue;
        int indexfamily=GoodSkill(temp);
        if (indexfamily == -1)
        {
            Log(MSG_WARNING,"Incorrect skill detected %i",temp);
            do_save=true;
            continue;
        }

        if (indexfamily != good_family)
        {
            Log(MSG_WARNING,"Skill %i should be in family %s instead of family %s",temp,tab_names[indexfamily],tab_names[good_family]);
            do_save=true;
        }

        if(cur_cskills[indexfamily] >= max_skills[indexfamily])
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
    good_family = 3;
	for(UINT i=0;i<MAX_UNIQUE_SKILL;i++)
    {
        uoff[i]=-1;
        char* tmp=strtok((i==0?row[40]:NULL), ",");
        if (tmp == NULL)
            continue;

        int temp = atoi(tmp);
        if (temp == 0)
            continue;
        int indexfamily = GoodSkill(temp);
        if (indexfamily == -1)
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

        cskills[cur_cskills[indexfamily]].id = temp;
        cskills[cur_cskills[indexfamily]].level = 1;
        uoff[i]=cur_cskills[indexfamily];
        cur_cskills[indexfamily]++;
    }

    //mileage skills.
    good_family=4;
	for(UINT i=0;i<MAX_MILEAGE_SKILL;i++)
    {
        moff[i]=-1;
        char* tmp=strtok((i==0?row[41]:NULL), ",");
        if (tmp == NULL)
            continue;

        int temp=atoi(tmp);
        if (temp==0)
            continue;
        int indexfamily=GoodSkill(temp);
        if (indexfamily==-1)
        {
            Log(MSG_WARNING,"Incorrect skill detected %i",temp);
            do_save = true;
            continue;
        }

        if (indexfamily!=good_family)
        {
            Log(MSG_WARNING,"Skill %i should be in family %s instead of family %s",temp,tab_names[indexfamily],tab_names[good_family]);
            do_save = true;
        }

        if(cur_cskills[indexfamily]>=max_skills[indexfamily])
        {
            Log(MSG_WARNING,"Already too much skills in family %s, no room for skill %i",tab_names[indexfamily],temp);
            do_save = true;
            continue;
        }

        cskills[cur_cskills[indexfamily]].id = temp;
        cskills[cur_cskills[indexfamily]].level = 1;
        moff[i]=cur_cskills[indexfamily];
        cur_cskills[indexfamily]++;
    }

    //class skills.
    good_family = 0;
	for(UINT i=0;i<MAX_CLASS_SKILL;i++)
    {
        coff[i] = -1;
        char* tmp = strtok((i == 0? row[21]:NULL), ",");
        if (tmp == NULL)
            continue;

        int temp = atoi(tmp);
        if (temp == 0)
            continue;
        int indexfamily = GoodSkill(temp);
        if (indexfamily == -1)
        {
            Log(MSG_WARNING,"Incorrect skill detected %i",temp);
            do_save = true;
            continue;
        }

        if (indexfamily != good_family)
        {
            Log(MSG_WARNING,"Skill %i should be in family %s instead of family %s",temp,tab_names[indexfamily],tab_names[good_family]);
            do_save=true;
        }

        if(cur_cskills[indexfamily] >= max_skills[indexfamily])
        {
            Log(MSG_WARNING,"Already too much skills in family %s, no room for skill %i",tab_names[indexfamily],temp);
            do_save = true;
            continue;
        }

        cskills[cur_cskills[indexfamily]].id = temp;
        cskills[cur_cskills[indexfamily]].level = 1;
        coff[i] = cur_cskills[indexfamily];
		Log(MSG_INFO,"Skill %i loaded in family %s",temp,tab_names[indexfamily]);
        cur_cskills[indexfamily]++;
    }

    //getting class levels now.
	for(UINT i=0;i<MAX_CLASS_SKILL;i++)
    {
        char* tmp = strtok((i == 0? row[22]:NULL), ",");
        int temp = 1;
        if (tmp != NULL)
            temp = atoi(tmp);

        if (coff[i] == -1)
            continue;
        if (temp == 0)
            temp = 1;
        if(cskills[coff[i]].id != 0)
		{
            cskills[coff[i]].level = temp;
			cskills[coff[i]].thisskill = GServer->GetSkillByID( cskills[coff[i]].id+cskills[coff[i]].level-1 );
			Log(MSG_DEBUG,"[Class Skills] Skill %i in slot %i set to level %i",cskills[coff[i]].id,coff[i],temp);
		}
		else
		{
			cskills[coff[i]].level = 0;
		}
    }

    //unique skills levels.
	for(UINT i=0;i<MAX_UNIQUE_SKILL;i++)
    {
        char* tmp = strtok((i == 0?row[43]:NULL), ",");
        int temp = 1;
        if (tmp != NULL)
            temp=atoi(tmp);

        if (uoff[i] == -1)
            continue;
        if (temp == 0)
            temp = 1;
        cskills[uoff[i]].level = temp;

        if(cskills[uoff[i]].id != 0)
		{
            cskills[uoff[i]].thisskill = GServer->GetSkillByID( cskills[uoff[i]].id+cskills[uoff[i]].level-1 );
			Log(MSG_DEBUG,"[Unique Skills] Skill %i in slot %i set to level %i",cskills[uoff[i]].id,uoff[i],temp);
		}
    }

    //mileage skills levels.
	for(UINT i=0;i<MAX_MILEAGE_SKILL;i++)
    {
        char* tmp = strtok((i == 0?row[43]:NULL), ",");
        int temp=1;
        if (tmp != NULL)
            temp=atoi(tmp);

        if (moff[i] == -1)
            continue;
        if (temp == 0)
            temp = 1;
        cskills[moff[i]].level = temp;

        if(cskills[moff[i]].id != 0)
		{
            cskills[moff[i]].thisskill = GServer->GetSkillByID( cskills[moff[i]].id+cskills[moff[i]].level-1 );
			Log(MSG_DEBUG,"[Milage Skills] Skill %i in slot %i set to level %i",cskills[moff[i]].id,moff[i],temp);
		}
    }

    //First free offset.
    for (int i=0;i<5;i++)
        cur_max_skills[i] = cur_cskills[i];
	
    //LMA: reset inventory.
    for(int i=0;i<MAX_INVENTORY;i++)
    {
        items[i].sp_value = 0;
        items[i].last_sp_value = 0;
    }

	GServer->DB->QFree( );
	

	//LMA: Saving Skills if needed.
    if (do_save)
    {
        saveskills();
    }

    //LMA: % rebate and bonus union points and craft talent.
    pc_rebate = 0;
    pc_up = 0;
    pc_craft_talent = 0;

    //LMA: summon jauge:
    summon_jauge = 50;
    cur_jauge = 0;

    for (int i=0;i<MAX_ALL_SKILL;i++)
    {
        if (cskills[i].thisskill == NULL)
        {
            continue;
        }

        for(int j=0;j<3;j++)
        {
            if(cskills[i].thisskill->buff[j] == 0)
            {
                continue;
            }

            if (cskills[i].thisskill->buff[j] == 59)
            {
                pc_rebate += cskills[i].thisskill->value2[j];
                continue;
            }

            if (cskills[i].thisskill->buff[j] ==70)
            {
                pc_craft_talent += cskills[i].thisskill->value1[j];
                continue;
            }

            //LMA: summon jauge.
            if (cskills[i].thisskill->buff[j] == 62)
            {
                summon_jauge += cskills[i].thisskill->value1[j];
                continue;
            }

        }

        //Union points (no stats?)
        if (cskills[i].thisskill->id == 5500)
        {
            pc_up = 50;
        }

        //LMA: we reset the skills cooldown.
        cskills[i].cooldown_skill = 0;
    }
	//confirming skills
	/*
	for(int i=0; i<MAX_CLASS_SKILL; i++) // Class Skills
 	{
 	    if(cskills[i].id == 0)
 	    {
 	        continue;
 	    }
		Log( MSG_DEBUG, "Checking CSkills.  Slot: %i ID: %i Level: %i", i,cskills[i].id,cskills[i].level);
 	}
	*/
	//PY skills ok at this point


    Log(MSG_INFO,"%s:: craft talent %%bonus is %i",CharInfo->charname,pc_craft_talent);
    //Log(MSG_INFO,"%s:: summon jauge %i",CharInfo->charname,summon_jauge);

	//PY: imported from KTRose to handle all the unionvars and reward points
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
	CharInfo->unionid = atoi(row[0]);
	//PY: this is bizarre. The code below had to be moved into the CharInfo class due to weird run time errors
	for (char points=1; points<11; points++)
	{
        if(points == 9)
        {
            CharInfo->unionvar[points] = Session->KTPoints;
        }
        else
        {
            CharInfo->unionvar[points] = atoi(row[points]);
        }
    }
	GServer->DB->QFree( );
	
		
	// code to reset players back to the correct faction after using the KTRose shop
	if(CharInfo->unionvar[0] == 9)
	{
        //Log(MSG_DEBUG, "UnionVar 7 used to reset player Union from %i to %i",Union_s->unionvar[0],Union_s->unionvar[7] );
        CharInfo->unionvar[0] = CharInfo->unionvar[7];
        CharInfo->unionvar[7] = 0;
    }
	//PY: end imported code
	//confirming skills again
	/*
	Log( MSG_DEBUG, "Checking CSkills Again after loading Unions.");		
	for(int i=0; i<MAX_CLASS_SKILL; i++) // Class Skills
 	{
 	    if(cskills[i].id == 0)
 	    {
 	        continue;
 	    }
		Log( MSG_DEBUG, "Checking CSkills.  Slot: %i ID: %i Level: %i", i,cskills[i].id,cskills[i].level);
 	}
	*/
	if(!loadinventory())
	{
		Log(MSG_WARNING, "Something went wrong with inventory load for character %s", CharInfo->charname);
		//return false;
	}
	/*
	Log( MSG_DEBUG, "Checking CSkills Again after loading Inventory.");		
	for(int i=0; i<MAX_CLASS_SKILL; i++) // Class Skills
 	{
 	    if(cskills[i].id == 0)
 	    {
 	        continue;
 	    }
		Log( MSG_DEBUG, "Checking CSkills.  Slot: %i ID: %i Level: %i", i,cskills[i].id,cskills[i].level);
 	}*/

	if(!loadstorage())
	{
		Log(MSG_WARNING, "Something went wrong with storage load for character %s", CharInfo->charname);
		//return false;
	}
	/*
	Log( MSG_DEBUG, "Checking CSkills Again after loading Storage.");		
	for(int i=0; i<MAX_CLASS_SKILL; i++) // Class Skills
 	{
 	    if(cskills[i].id == 0)
 	    {
 	        continue;
 	    }
		Log( MSG_DEBUG, "Checking CSkills.  Slot: %i ID: %i Level: %i", i,cskills[i].id,cskills[i].level);
 	}*/
	SaveBackupStorage(1); // makes a copy of the account's storage at the moment of login
	Log( MSG_DEBUG, "Checking CSkills Again after saving backup storage.");		
	for(int i=0; i<MAX_CLASS_SKILL; i++) // Class Skills
 	{
 	    if(cskills[i].id == 0)
 	    {
 	        continue;
 	    }
		Log( MSG_DEBUG, "Checking CSkills.  Slot: %i ID: %i Level: %i", i,cskills[i].id,cskills[i].level);
 	}

	//LMA: Loading ItemMall
    //RebuildItemMall(); pointless. We don't have an item mall in 137
	//confirming skills again
	/*
	Log( MSG_DEBUG, "Checking CSkills Again after item mall.");
	for(int i=0; i<MAX_CLASS_SKILL; i++) // Class Skills
 	{
 	    if(cskills[i].id == 0)
 	    {
 	        continue;
 	    }
		Log( MSG_DEBUG, "Checking CSkills.  Slot: %i ID: %i Level: %i", i,cskills[i].id,cskills[i].level);
 	}*/

    if(Clan->clanid != 0)
    {
    	result = GServer->DB->QStore("SELECT logo,back,name,grade,cp FROM list_clan where id=%i", Clan->clanid);
        if(result==NULL) return false;
    	if(mysql_num_rows(result)!=1)
    	{
            Log(MSG_WARNING, "There are %i clan(s) with id %i", mysql_num_rows(result), Clan->clanid );
    	    Clan->clanid = 0;
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
	/*
	//confirming skills again
	Log( MSG_DEBUG, "Checking CSkills Again after clans.");
	for(int i=0; i<MAX_CLASS_SKILL; i++) // Class Skills
 	{
 	    if(cskills[i].id == 0)
 	    {
 	        continue;
 	    }
		Log( MSG_DEBUG, "Checking CSkills.  Slot: %i ID: %i Level: %i", i,cskills[i].id,cskills[i].level);
 	}*/

	//PY KT method of loading quests
	Log( MSG_DEBUG, "starting PY quest load");
    result = GServer->DB->QStore("SELECT questid,starttime,vbl,switches,items,count,episodevar,jobvar,planetvar,unionvar from player_quests where owner=%i",CharInfo->charid );
    //Log( MSG_DEBUG, "ran the query");
	const char *tmpid;
    char *itmp;
    tmpid = "";
    itmp = "";
	int itemid;
	//Clear quest structure first
	for(UINT i=0;i<10;i++)
	{
		quest.quests[i].QuestID = 0;
		quest.quests[i].StartTime = 0;
		for(unsigned int j=0;j<10;j++)                       //quest variables
        {
			quest.quests[i].Variables[j] = 0;
        }
		for(unsigned int j=0;j<4;j++)
        {
            quest.quests[i].Switches[j] = 0;
        }
        for(unsigned int j=0;j<5;j++)
        {
			quest.quests[i].Items[j].itemnum = 0;
			quest.quests[i].Items[j].itemtype = 0;
        }
		for(unsigned int j=0;j<5;j++)
        {
			quest.quests[i].Items[j].count = 0;
        }
        for(unsigned int j=0;j<5;j++)                      
        {
            quest.EpisodeVar[j] = 0;
        }
        for(unsigned int j=0;j<3;j++)
        {
            quest.JobVar[j] = 0;
        }
        for(unsigned int j=0;j<7;j++)
        {
            quest.PlanetVar[j] = 0;
        }
        for(unsigned int j=0;j<10;j++)
        {
            quest.UnionVar[j] = 0;
        }
	}
    if(result != NULL)
    {
        UINT Index = 0;
        while(row = mysql_fetch_row(result))
        {
            quest.quests[Index].QuestID = atoi(row[0]);
            quest.quests[Index].StartTime = atoi(row[1]);

            tmpid = strtok( row[2] , "|");
            for(unsigned int j=0;j<10;j++)                       //quest variables
            {
                if(tmpid != NULL)
                    quest.quests[Index].Variables[j] = atoi(tmpid);
				tmpid = strtok(NULL, ",|");
            }
            tmpid = strtok( row[3] , "|");                        //quest switches
            for(unsigned int j=0;j<4;j++)
            {
                if(tmpid != NULL)
                    quest.quests[Index].Switches[j] = atoi(tmpid);
				tmpid = strtok(NULL, ",|");
            }
            tmpid = strtok( row[4] , "|");                        //quest items
            for(unsigned int j=0;j<5;j++)
            {
                if(tmpid != NULL)
                {
                    itemid = atoi(tmpid);
                    quest.quests[Index].Items[j].itemnum = itemid % 1000;
                    quest.quests[Index].Items[j].itemtype = itemid / 1000;
                }
				tmpid = strtok(NULL, ",|");
            }
            tmpid = strtok( row[5] , "|");                        //quest item count
            for(unsigned int j=0;j<5;j++)
            {
                if(tmpid != NULL)
                    quest.quests[Index].Items[j].count = atoi(tmpid);
				tmpid = strtok(NULL, ",|");
            }
            Index ++;
        }
    }
    //Log( MSG_DEBUG, "loaded the regular quest variables");
    GServer->DB->QFree( );

	//load quest variables from the new table
	result = GServer->DB->QStore("SELECT episode, job, planet, unionvar from qvars where owner=%i",CharInfo->charid );
	//clear variables
	for(unsigned int j=0;j<5;j++)                      //episode variables
	{
		quest.EpisodeVar[j] = 0;
	}                      
	for(unsigned int j=0;j<3;j++)						//job variables
	{
		quest.JobVar[j] = 0;
	}
	for(unsigned int j=0;j<7;j++)						//Planet variables
	{
		quest.PlanetVar[j] = 0;
	}                      
	for(unsigned int j=0;j<10;j++)						//Union variables
	{
		quest.UnionVar[j] = 0;
	}
	if(mysql_num_rows(result) != 1)
    {
        Log(MSG_WARNING, "Loading QVars: There are %i entries for owner %i", mysql_num_rows(result), CharInfo->charid );
    }
	else
	{
		row = mysql_fetch_row(result);
		tmpid = strtok( row[0] , ",");
		for(unsigned int j=0;j<5;j++)                      //episode variables
		{
			if(tmpid != NULL)
				quest.EpisodeVar[j] = atoi(tmpid);
			tmpid = strtok(NULL, ",");
		}
		tmpid = strtok( row[1] , ",");                        //job variables
		for(unsigned int j=0;j<3;j++)
		{
			if(tmpid != NULL)
				quest.JobVar[j] = atoi(tmpid);
			tmpid = strtok(NULL, ",");
		}
		tmpid = strtok( row[2] , ",");                        //Planet variables
		for(unsigned int j=0;j<7;j++)
		{
			if(tmpid != NULL)
				quest.PlanetVar[j] = atoi(tmpid);
			tmpid = strtok(NULL, ",");
		}
		tmpid = strtok( row[3] , ",");                        //Union variables
		for(unsigned int j=0;j<10;j++)
		{
			if(tmpid != NULL)
				quest.UnionVar[j] = atoi(tmpid);
			tmpid = strtok(NULL, ",");
		}
	}
	GServer->DB->QFree( );


    // PY method load quest flags
    //Log( MSG_DEBUG, "starting PY flag load");
    //result = GServer->DB->QStore("SELECT flags from questflags where owner=%i",CharInfo->charid );
	result = GServer->DB->QStore("SELECT switches from qflags where owner=%i",CharInfo->charid );
    //Log( MSG_DEBUG, "ran the query");
    /*
	if(result != NULL)
    {
        tmpid = "";
        itmp = 0;
        UINT Index = 0;
        UINT flagCount = 0;
		UINT flagmax = (UINT)QUEST_SWITCH_CNT / 8;
        //first set all quest flags to zero
        for(int flagid=0;flagid<flagmax;flagid++)
        {
            quest.Qflags[flagid] = 0;
        }
        while(row = mysql_fetch_row(result))
        {
            int flagid = atoi(row[0]);
			quest.SetFlag(flagid, true);
    		flagCount++;
        }
        Log( MSG_DEBUG, "Flag count = %i",flagCount);
    }
	*/
	UINT flagmax = (UINT)QUEST_SWITCH_CNT / 8;
	if(mysql_num_rows(result) != 1)
    {
        Log(MSG_WARNING, "Loading QFlags: There are %i entries for owner %i. There should only be one", mysql_num_rows(result), CharInfo->charid );
		for(unsigned int j=0;j<flagmax;j++)                      //64 bytes of quest flags all comma delimited
		{
			quest.Qflags[j] = 0;
		}
    }
	else
	{
		row = mysql_fetch_row(result);
		
		//clear quest structure
		for(int flagid=0;flagid<flagmax;flagid++)
		{
			quest.Qflags[flagid] = 0;
		}
		tmpid = strtok( row[0] , ",");
		for(unsigned int j=0;j<flagmax;j++)                      //64 bytes of quest flags all comma delimited
		{
			if(tmpid != NULL)
				quest.Qflags[j] = atoi(tmpid);
			tmpid = strtok(NULL, ",");
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
    die_quest = 0;
    for(int k=0;k<10;k++)
    {
        arr_questid[k].questid = 0;
        arr_questid[k].die_time = 0;
    }
    #endif
    //LMA: end
	Log( MSG_WARNING, "End of load sequence. User '%s' now has ID: %i and Str: %i", CharInfo->charname,CharInfo->charid,Attr->Str);
	return true;
}

void CPlayer::savedata( )
{
    lastSaveTime = clock();
	Log(MSG_INFO, "PY_Save data for char '%s' ", CharInfo->charname );
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
    	
        for(UINT i=0;i<32;i++)
        {
            if(i==0)
            	sprintf(&quick[i], "%i",quickbar[i]);
            else
                sprintf(&quick[strlen(quick)], ",%i",quickbar[i]);
        }
		
        long int hp = Stats->HP;
        if(hp <= 0)
    	    hp = Stats->MaxHP * 10 / 100;
	    if(Stats->MP < 0)
	        Stats->MP = 0;
		//Log( MSG_DEBUG, "User '%s' currently has ID: %i and Str: %i", CharInfo->charname,CharInfo->charid,Attr->Str);
        
		GServer->DB->QExecuteUpdate("UPDATE characters SET level=%i,face=%i,hairStyle=%i,sex=%i,classid=%i,zuly=%I64i,str=%i,dex=%i,_int=%i, con=%i,cha=%i,sen=%i,curHp=%i,curMp=%i,statp=%i,skillp=%i,exp=%I32i,stamina=%i,respawnid=%i,clanid=%i,clan_rank=%i,townid=%i,highestoverkill=%f WHERE char_name='%s'",
                    Stats->Level, CharInfo->Face, CharInfo->Hair, CharInfo->Sex, CharInfo->Job, CharInfo->Zulies, Attr->Str, Attr->Dex, Attr->Int, Attr->Con, Attr->Cha, Attr->Sen, hp, Stats->MP, CharInfo->StatPoints, CharInfo->SkillPoints,
                    CharInfo->Exp, CharInfo->stamina, Position->respawn, Clan->clanid, Clan->clanrank, Position->saved, CharInfo->HighestOverkill,
					CharInfo->charname );
		
		//Log(MSG_INFO, "Saved basic data for char '%s' ", CharInfo->charname );

        GServer->DB->QExecute("UPDATE characters SET unionid=%i,union1points=%i,union2points=%i,union3points=%i,union4points=%i,union5points=%i,union6points=%i,union7points=%i,union8points=%i,union9points=%i,union10points=%i WHERE char_name='%s'",
                    CharInfo->unionvar[0], CharInfo->unionvar[1],CharInfo->unionvar[2], CharInfo->unionvar[3], CharInfo->unionvar[4], CharInfo->unionvar[5], CharInfo->unionvar[6], CharInfo->unionvar[7], CharInfo->unionvar[8], CharInfo->unionvar[9], CharInfo->unionvar[10], 
					CharInfo->charname );

		GServer->DB->QExecute("UPDATE characters SET quickbar='%s' WHERE char_name='%s'",quick, CharInfo->charname );

		//Log(MSG_INFO, "Saved unionvar data for char '%s' ", CharInfo->charname );

        //save the inventory
    	saveinventory();
    	//save the storage items
    	savestorage();
		//save the skills
        saveskills();

    	/*
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
		*/
        savequests( this );
		
		GServer->DB->QExecute("UPDATE accounts SET zulystorage = %I64i, logtime = %i,ktpoints=%i WHERE id = %i", CharInfo->Storage_Zulies, Session->logtime,Session->KTPoints, Session->userid);
		Log(MSG_INFO, "Data Saved for char '%s' ", CharInfo->charname );
    }
	//initialize NewLogin counter
	NewLogInCouter = 1;

	

	//this should save the entire collisions structure for all the maps. but only if turned on from GM command
	if(GServer->Config.MapCollsionMode > -1 )
	{
		std::ofstream pFile;
		for(short i = 0;i < NB_MAPS; i++)
		{
			if(GServer->Collisions[i].modified == true)	//no point saving a map that hasn't been modified
			{
				string filename;
				string mapnumber;
				stringstream convert;
				convert << i;
				mapnumber = convert.str();
				filename.append("data/maps/");
				filename.append(mapnumber);
				filename.append(".data");
				pFile.open(filename, std::ios::binary);
				pFile.write((char*)&i, sizeof(i)); //save map id
				pFile.write((char*)&GServer->Collisions[i].MinX, sizeof(GServer->Collisions[i].MinX));
				pFile.write((char*)&GServer->Collisions[i].MinY, sizeof(GServer->Collisions[i].MinY));
				pFile.write((char*)&GServer->Collisions[i].length, sizeof(GServer->Collisions[i].length));
				pFile.write((char*)&GServer->Collisions[i].width, sizeof(GServer->Collisions[i].width));
				Log( MSG_DEBUG, "Saving data for map %i. MinX: %i length: %i  Height: %i",i,GServer->Collisions[i].MinX,GServer->Collisions[i].length,GServer->Collisions[i].width );
		
				for(int j = 0; j < GServer->Collisions[i].length; j++)
				{
					for(int k = 0; k < GServer->Collisions[i].width; k++)
					{
						pFile.write((char*)&GServer->Collisions[i].coords[j][k], sizeof(GServer->Collisions[i].coords[j][k]));
					}
				}
				pFile.close();
			}
		}
		pFile.close();
		Log( MSG_INFO, "Map Collision Data Saved." );
	}
}


//PY Character save
void CPlayer::quicksave( )
{
     GServer->DB->QExecute("UPDATE characters SET level=%i,face=%i,hairStyle=%i,sex=%i,classid=%i,zuly=%i,str=%i,dex=%i,_int=%i, con=%i,cha=%i,sen=%i,curHp=%i,curMp=%i,statp=%i,skillp=%i,exp=%i,stamina=%i,respawnid=%i,clanid=%i,clan_rank=%i,townid=%i WHERE char_name='%s'",
         Stats->Level, CharInfo->Face, CharInfo->Hair, CharInfo->Sex, CharInfo->Job, CharInfo->Zulies, Attr->Str, Attr->Dex, Attr->Int, Attr->Con, Attr->Cha, Attr->Sen, Stats->HP, Stats->MP, CharInfo->StatPoints, CharInfo->SkillPoints,
         CharInfo->Exp, CharInfo->stamina, Position->respawn, Clan->clanid, Clan->clanrank, Position->saved, 
		 CharInfo->charname );
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
	char tmp10[1024] = "";
	char tmp11[1024] = "";
	char tmp12[1024] = "";
	char tmp13[1024] = "";
    char slot[1024] = "";
	
    int tran = 0;
    for(unsigned int i=0;i<MAX_INVENTORY; i++) //inventory holds 140
    {
        int socketed = 0;
        int appraised = 0;
        //tran += items[i].itemnum + items[i].itemtype + items[i].refine;
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
				sprintf(&tmp10[strlen(tmp10)], "0");
				sprintf(&tmp11[strlen(tmp11)], "0");
				sprintf(&tmp12[strlen(tmp12)], "0");
				sprintf(&tmp13[strlen(tmp13)], "0");
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
				sprintf(&tmp10[strlen(tmp10)], "%i",items[i].UStat[0]);
				sprintf(&tmp11[strlen(tmp11)], "%i",items[i].UStat[1]);
				sprintf(&tmp12[strlen(tmp12)], "%i",items[i].UValue[0]);
				sprintf(&tmp13[strlen(tmp13)], "%i",items[i].UValue[1]);
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
				sprintf(&tmp10[strlen(tmp10)], "|0");
				sprintf(&tmp11[strlen(tmp11)], "|0");
				sprintf(&tmp12[strlen(tmp12)], "|0");
				sprintf(&tmp13[strlen(tmp13)], "|0");
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
				sprintf(&tmp10[strlen(tmp10)], "|%i",items[i].UStat[0]);
				sprintf(&tmp11[strlen(tmp11)], "|%i",items[i].UStat[1]);
				sprintf(&tmp12[strlen(tmp12)], "|%i",items[i].UValue[0]);
				sprintf(&tmp13[strlen(tmp13)], "|%i",items[i].UValue[1]);
                sprintf(&slot[strlen(slot)], "|%i",i);
            }
        }
    }
    //check if a record exists for this player
	tran = 100;
    MYSQL_RES *result = GServer->DB->QStore("SELECT itemid FROM charitems WHERE owner=%i", CharInfo->charid);
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
        GServer->DB->QExecute("UPDATE charitems SET itemid='%s',itemtype='%s',refine='%s',durability='%s',lifespan='%s',count='%s',stats='%s',socketed='%s',appraised='%s',ustat1='%s',ustat2='%s',uvalue1='%s',uvalue2='%s',slot='%s',trans_action=%i WHERE owner='%i'",
                    tmp1,tmp2,tmp3,tmp4,tmp5,tmp6,tmp7,tmp8,tmp9,tmp10,tmp11,tmp12,tmp13,slot,tran,CharInfo->charid);
    }
    else
    {
        //record does not exist so make a new one
        //Log(MSG_INFO,"record does not exist.... creating new record");
        //GServer->DB->QExecute("INSERT INTO charitems (owner,trans_action) VALUES ('%i','%i')",CharInfo->charid,tran);
        GServer->DB->QExecute("INSERT INTO charitems (owner,itemid,itemtype,refine,durability,lifespan,count,stats,socketed,appraised,ustat1,ustat2,uvalue1,uvalue2,slot,trans_action) VALUES('%i','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%i')",
    				CharInfo->charid,tmp1,tmp2,tmp3,tmp4,tmp5,tmp6,tmp7,tmp8,tmp9,tmp10,tmp11,tmp12,tmp13,slot,tran);
    }
    //save zuly
    GServer->DB->QExecute("UPDATE characters SET zuly=%I64i WHERE char_name='%s'",
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
	char tmp10[1024] = "0";
	char tmp11[1024] = "0";
	char tmp12[1024] = "0";
	char tmp13[1024] = "0";
    char slot[1024] = "0";
    long tran = 0;
    for(unsigned int i=0;i<MAX_STORAGE; i++) //Storage holds 160
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
			sprintf(&tmp10[strlen(tmp10)], "|0");
			sprintf(&tmp11[strlen(tmp11)], "|0");
			sprintf(&tmp12[strlen(tmp12)], "|0");
			sprintf(&tmp13[strlen(tmp13)], "|0");
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
			sprintf(&tmp10[strlen(tmp10)], "|%i",storageitems[i].UStat[0]);
			sprintf(&tmp11[strlen(tmp11)], "|%i",storageitems[i].UStat[1]);
			sprintf(&tmp12[strlen(tmp12)], "|%i",storageitems[i].UValue[0]);
			sprintf(&tmp13[strlen(tmp13)], "|%i",storageitems[i].UValue[1]);
            if(i != 0) //don't write the zero value for slot since we already started the string with a leading zero
                sprintf(&slot[strlen(slot)], "|%i",i);
        }
    }
    //check if a record exists for this player
    MYSQL_RES *result = GServer->DB->QStore("SELECT itemid FROM charstorage WHERE owner=%i", Session->userid);
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
        GServer->DB->QExecute("UPDATE charstorage SET itemid='%s',itemtype='%s',refine='%s',durability='%s',lifespan='%s',count='%s',stats='%s',socketed='%s',appraised='%s',ustat1='%s',ustat2='%s',uvalue1='%s',uvalue2='%s',slot='%s',trans_action=%i WHERE owner='%i'",
                    tmp1,tmp2,tmp3,tmp4,tmp5,tmp6,tmp7,tmp8,tmp9,tmp10,tmp11,tmp12,tmp13,slot,tran,Session->userid);
    }
    else
    {
        //record does not exist so make a new one
        //Log(MSG_INFO,"record does not exist.... creating new record");
        //GServer->DB->QExecute("INSERT INTO charitems (owner,trans_action) VALUES ('%i','%i')",CharInfo->charid,tran);
        GServer->DB->QExecute("INSERT INTO charstorage (owner,itemid,itemtype,refine,durability,lifespan,count,stats,socketed,appraised,ustat1,ustat2,uvalue1,uvalue2,slot,trans_action) VALUES('%i','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%i')",
    				Session->userid,tmp1,tmp2,tmp3,tmp4,tmp5,tmp6,tmp7,tmp8,tmp9,tmp10,tmp11,tmp12,tmp13,slot,tran);
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
	char tmp10[1024] = "0";
	char tmp11[1024] = "0";
	char tmp12[1024] = "0";
	char tmp13[1024] = "0";
    char slot[1034] = "0";
    long tran = 0;
    for(unsigned int i=0;i<MAX_STORAGE; i++) //Storage holds 160
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
			sprintf(&tmp10[strlen(tmp10)], "|0");
			sprintf(&tmp11[strlen(tmp11)], "|0");
			sprintf(&tmp12[strlen(tmp12)], "|0");
			sprintf(&tmp13[strlen(tmp13)], "|0");
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
			sprintf(&tmp10[strlen(tmp10)], "|%i",storageitems[i].UStat[0]);
			sprintf(&tmp11[strlen(tmp11)], "|%i",storageitems[i].UStat[1]);
			sprintf(&tmp12[strlen(tmp12)], "|%i",storageitems[i].UValue[0]);
			sprintf(&tmp13[strlen(tmp13)], "|%i",storageitems[i].UValue[1]);
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
    GServer->DB->QExecute("INSERT INTO backupstorage (owner,itemid,itemtype,refine,durability,lifespan,count,stats,socketed,appraised,ustat1,ustat2,uvalue1,uvalue2,slot,trans_action,datestamp,thisrev) VALUES('%i','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%i','%s','%i')",
        Session->userid,tmp1,tmp2,tmp3,tmp4,tmp5,tmp6,tmp7,tmp8,tmp9,tmp10,tmp11,tmp12,tmp13,slot,logstate,timestamp,thisrev);
    //Log(MSG_INFO,"backup Storage saved for User %i revision number %i logstate %i",Session->userid, thisrev, logstate);
    return;
}

bool CPlayer::loadinventory()
{
// PY loadup code
	//Log(MSG_INFO,"Loading inventory data");
	MYSQL_ROW row;
    MYSQL_RES *result = GServer->DB->QStore("SELECT itemid,itemtype,refine,durability,lifespan,count,stats,socketed,appraised,ustat1,ustat2,uvalue1,uvalue2,trans_action FROM charitems WHERE owner=%i", CharInfo->charid);
    if(result == NULL)
    {
        GServer->DB->QFree( );
		Log(MSG_WARNING, "Something a bit wrong with this query");
        return false;
    }
    const char *tmpid;
    char *itmp = 0;
    int itemid;
    while( row = mysql_fetch_row(result) )
    {
        tmpid = strtok( row[0] , "|");
        for(unsigned int i=1;i<MAX_INVENTORY; i++)
        {
            if((itmp = strtok( NULL , "|"))!= NULL)
                items[i].itemnum = GServer->GetSafeAtoi(itmp);
        }
		//Log( MSG_DEBUG, "Checking CSkill after itemnum.  Slot: 1 ID: %i Level: %i",cskills[1].id,cskills[1].level);
	    tmpid = strtok( row[1] , "|");
        for(unsigned int i=1;i<MAX_INVENTORY; i++)
        {
            if((itmp = strtok( NULL , "|"))!= NULL)
                items[i].itemtype = GServer->GetSafeAtoi(itmp);
        }
		//Log( MSG_DEBUG, "Checking CSkill after itemtype.  Slot: 1 ID: %i Level: %i",cskills[1].id,cskills[1].level);
        tmpid = strtok( row[2] , "|");
        for(unsigned int i=1;i<MAX_INVENTORY; i++)
        {
            if((itmp = strtok( NULL , "|"))!= NULL)
                items[i].refine = GServer->GetSafeAtoi(itmp) / 16;
        }
		//Log( MSG_DEBUG, "Checking CSkill after refine.  Slot: 1 ID: %i Level: %i",cskills[1].id,cskills[1].level);
        tmpid = strtok( row[3] , "|");
        for(unsigned int i=1;i<MAX_INVENTORY; i++)
        {
            if((itmp = strtok( NULL , "|"))!= NULL)
                items[i].durability = GServer->GetSafeAtoi(itmp);
        }
		//Log( MSG_DEBUG, "Checking CSkill after durability.  Slot: 1 ID: %i Level: %i",cskills[1].id,cskills[1].level);
        tmpid = strtok( row[4] , "|");
        for(unsigned int i=1;i<MAX_INVENTORY; i++)
        {
            if((itmp = strtok( NULL , "|"))!= NULL)
                items[i].lifespan = GServer->GetSafeAtoi(itmp);
        }
		//Log( MSG_DEBUG, "Checking CSkill after lifespan.  Slot: 1 ID: %i Level: %i",cskills[1].id,cskills[1].level);
        tmpid = strtok( row[5] , "|");
        for(unsigned int i=1;i<MAX_INVENTORY; i++)
        {
            if((itmp = strtok( NULL , "|"))!= NULL)
            {
                items[i].count = GServer->GetSafeAtoi(itmp);
                //Log(MSG_INFO,"slot: %i itemid: %i type: %i count: %i",i, items[i].itemnum,items[i].itemtype,items[i].count); //PAT items seem to load ok here
            }
        }
		//Log( MSG_DEBUG, "Checking CSkill after count.  Slot: 1 ID: %i Level: %i",cskills[1].id,cskills[1].level);
        tmpid = strtok( row[6] , "|");
        for(unsigned int i=1;i<MAX_INVENTORY; i++)
        {
            if((itmp = strtok( NULL , "|"))!= NULL)
            {
                items[i].stats = GServer->GetSafeAtoi(itmp);
                if(items[i].stats > 300)
                {
                    items[i].gem = items[i].stats;
                }
                else
                    items[i].gem = 0;
            }
        }
		//Log( MSG_DEBUG, "Checking CSkill after stats.  Slot: 1 ID: %i Level: %i",cskills[1].id,cskills[1].level);
        tmpid = strtok( row[7] , "|");
        for(unsigned int i=1;i<MAX_INVENTORY; i++)
        {
            if((itmp = strtok( NULL , "|"))!= NULL)
                items[i].socketed = (atoi(itmp)==1)?true:false;
            //just in case there is a discrepency in the database this will fix sockets.
            if(items[i].gem != 0)
                items[i].socketed = true;
        }
		//Log( MSG_DEBUG, "Checking CSkill after socket.  Slot: 1 ID: %i Level: %i",cskills[1].id,cskills[1].level);
        tmpid = strtok( row[8] , "|");
        for(unsigned int i=1;i<MAX_INVENTORY; i++)
        {
            if((itmp = strtok( NULL , "|"))!= NULL)
                items[i].appraised = (atoi(itmp)==1)?true:false;
        }
		//Log( MSG_DEBUG, "Checking CSkill after appraisal.  Slot: 1 ID: %i Level: %i",cskills[1].id,cskills[1].level);
		tmpid = strtok( row[9] , "|");
        for(unsigned int i=1;i<MAX_INVENTORY; i++)
        {
            if((itmp = strtok( NULL , "|"))!= NULL)
                items[i].UStat[0] = GServer->GetSafeAtoi(itmp);
        }
		//Log( MSG_DEBUG, "Checking CSkill after Ustat0.  Slot: 1 ID: %i Level: %i",cskills[1].id,cskills[1].level);
		tmpid = strtok( row[10] , "|");
        for(unsigned int i=1;i<MAX_INVENTORY; i++)
        {
            if((itmp = strtok( NULL , "|"))!= NULL)
                items[i].UStat[1] = GServer->GetSafeAtoi(itmp);
        }
		//Log( MSG_DEBUG, "Checking CSkill after Ustat1.  Slot: 1 ID: %i Level: %i",cskills[1].id,cskills[1].level);
		tmpid = strtok( row[11] , "|");
        for(unsigned int i=1;i<MAX_INVENTORY; i++)
        {
            if((itmp = strtok( NULL , "|"))!= NULL)
                items[i].UValue[0] = GServer->GetSafeAtoi(itmp);
        }
		//Log( MSG_DEBUG, "Checking CSkill after Uvalue0.  Slot: 1 ID: %i Level: %i",cskills[1].id,cskills[1].level);
		tmpid = strtok( row[12] , "|");
        for(unsigned int i=1;i<MAX_INVENTORY; i++)
        {
            if((itmp = strtok( NULL , "|"))!= NULL)
                items[i].UValue[1] = GServer->GetSafeAtoi(itmp);
        }
		//Log( MSG_DEBUG, "Checking CSkill after Uvalue1.  Slot: 1 ID: %i Level: %i",cskills[1].id,cskills[1].level);
        for(unsigned int i=1;i<MAX_INVENTORY; i++)
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
		//Log( MSG_DEBUG, "Checking CSkill after final check.  Slot: 1 ID: %i Level: %i",cskills[1].id,cskills[1].level);
    }
	GServer->DB->QFree( );
	return true;
}

bool CPlayer::loadstorage()
{
	// PY loadup code for storage
	//Log(MSG_INFO,"Loading storage data");
	MYSQL_ROW row;
    MYSQL_RES *result = GServer->DB->QStore("SELECT itemid,itemtype,refine,durability,lifespan,count,stats,socketed,appraised,ustat1,ustat2,uvalue1,uvalue2,trans_action FROM charstorage WHERE owner=%i", Session->userid);
    if(result == NULL)
    {
        GServer->DB->QFree( );
		Log(MSG_WARNING, "Something a bit wrong with this query. Maybe the first time a character has loaded this character");
        return false;
    }
	const char *tmpid;
    char *itmp;
    int itemid = 0;
    while( row = mysql_fetch_row(result) )
    {
        tmpid = strtok( row[0] , "|");
        for(unsigned int i=0;i<MAX_STORAGE; i++)
        {
            if((itmp = strtok( NULL , "|"))!= NULL)
                storageitems[i].itemnum = GServer->GetSafeAtoi(itmp);
        }
	    tmpid = strtok( row[1] , "|");
        for(unsigned int i=0;i<MAX_STORAGE; i++)
        {
            if((itmp = strtok( NULL , "|"))!= NULL)
                storageitems[i].itemtype = GServer->GetSafeAtoi(itmp);
        }
        tmpid = strtok( row[2] , "|");
        for(unsigned int i=0;i<MAX_STORAGE; i++)
        {
            if((itmp = strtok( NULL , "|"))!= NULL)
                storageitems[i].refine = GServer->GetSafeAtoi(itmp);
        }
        tmpid = strtok( row[3] , "|");
        for(unsigned int i=0;i<MAX_STORAGE; i++)
        {
            if((itmp = strtok( NULL , "|"))!= NULL)
                storageitems[i].durability = GServer->GetSafeAtoi(itmp);
        }
        tmpid = strtok( row[4] , "|");
        for(unsigned int i=0;i<MAX_STORAGE; i++)
        {
            if((itmp = strtok( NULL , "|"))!= NULL)
                storageitems[i].lifespan = GServer->GetSafeAtoi(itmp);
        }
        tmpid = strtok( row[5] , "|");
        for(unsigned int i=0;i<MAX_STORAGE; i++)
        {
            if((itmp = strtok( NULL , "|"))!= NULL)
                storageitems[i].count = GServer->GetSafeAtoi(itmp);
        }
        tmpid = strtok( row[6] , "|");
        for(unsigned int i=0;i<MAX_STORAGE; i++)
        {
            if((itmp = strtok( NULL , "|"))!= NULL)
            {
                storageitems[i].stats = GServer->GetSafeAtoi(itmp);
                if(storageitems[i].stats > 300)
                {
                    storageitems[i].gem = items[i].stats;
                }
                else
                    storageitems[i].gem = 0;
            }
        }
        tmpid = strtok( row[7] , "|");
        for(unsigned int i=0;i<MAX_STORAGE; i++)
        {
            if((itmp = strtok( NULL , "|"))!= NULL)
                storageitems[i].socketed = (atoi(itmp)==1)?true:false;
            //just in case there is a discrepency in the database this will fix sockets.
            if(storageitems[i].gem != 0)
                storageitems[i].socketed = true;
        }
        tmpid = strtok( row[8] , "|");
        for(unsigned int i=0;i<MAX_STORAGE; i++)
        {
            if((itmp = strtok( NULL , "|"))!= NULL)
                storageitems[i].appraised = (atoi(itmp)==1)?true:false;
        }
		tmpid = strtok( row[9] , "|");
        for(unsigned int i=0;i<MAX_STORAGE; i++)
        {
            if((itmp = strtok( NULL , "|"))!= NULL)
                storageitems[i].UStat[0] = GServer->GetSafeAtoi(itmp);
        }
		tmpid = strtok( row[10] , "|");
        for(unsigned int i=0;i<MAX_STORAGE; i++)
        {
            if((itmp = strtok( NULL , "|"))!= NULL)
                storageitems[i].UStat[1] = GServer->GetSafeAtoi(itmp);
        }
		tmpid = strtok( row[11] , "|");
        for(unsigned int i=0;i<MAX_STORAGE; i++)
        {
            if((itmp = strtok( NULL , "|"))!= NULL)
                storageitems[i].UValue[0] = GServer->GetSafeAtoi(itmp);
        }
		tmpid = strtok( row[12] , "|");
        for(unsigned int i=0;i<MAX_STORAGE; i++)
        {
            if((itmp = strtok( NULL , "|"))!= NULL)
                storageitems[i].UValue[1] = GServer->GetSafeAtoi(itmp);
        }
		nstorageitems = 0;
        for(unsigned int i=0;i<MAX_STORAGE; i++)
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
			if(storageitems[i].type != 0)
				nstorageitems++;
        }
    }
	GServer->DB->QFree( );
	return true;
}


//LMA: Saving skills here...
void CPlayer::saveskills( )
{
    if(Session->userid == 0)
        return;

    int pc_temp = 0;
    int pc_union = 0;
    int val_summon = 0;
    char basic[1024];
    char drive[1024];
    char sclass[1024];
    char slevel[1024];
    char uclass[1024];
    char ulevel[1024];
    char mclass[1024];
    char mlevel[1024];


    pc_craft_talent = 0;

    //class skills and level.
    for(UINT i=0;i < MAX_CLASS_SKILL;i++)
    {
        if(i == 0)
        {
           sprintf(&sclass[0], "%i",cskills[i].id);
           sprintf(&slevel[0], "%i",cskills[i].level);
        }
        else
        {
           sprintf(&sclass[strlen(sclass)], ",%i",cskills[i].id);
           sprintf(&slevel[strlen(slevel)], ",%i",cskills[i].level);
        }
    }

    //unique skills.
    for(UINT i=90;i < 90 + MAX_UNIQUE_SKILL;i++)
    {
        if(i == 90)
        {
           sprintf(&uclass[0], "%i",cskills[i].id);
           sprintf(&ulevel[0], "%i",cskills[i].level);
        }
        else
        {
           sprintf(&uclass[strlen(uclass)], ",%i",cskills[i].id);
           sprintf(&ulevel[strlen(ulevel)], ",%i",cskills[i].level);
        }
    }

    //mileage skills and level.
    for(UINT i=120;i<120 + MAX_MILEAGE_SKILL;i++)
    {
        if(i == 120)
        {
           sprintf(&mclass[0], "%i",cskills[i].id);
           sprintf(&mlevel[0], "%i",cskills[i].level);
        }
        else
        {
           sprintf(&mclass[strlen(mclass)], ",%i",cskills[i].id);
           sprintf(&mlevel[strlen(mlevel)], ",%i",cskills[i].level);
        }
    }

    //basic skills.
    for(UINT i=320;i < 320 + MAX_BASIC_SKILL;i++)
    {
        if(i == 320)
            sprintf(&basic[0], "%i",cskills[i].id);
        else
            sprintf(&basic[strlen(basic)], ",%i",cskills[i].id);
    }

    //driving skills.
    for(UINT i=60;i < 60 + MAX_DRIVING_SKILL;i++)
    {
        if(i == 60)
            sprintf(&drive[0], "%i",cskills[i]);
        else
            sprintf(&drive[strlen(drive)], ",%i",cskills[i]);
    }

    pc_rebate = pc_temp;
    pc_up = pc_union;
    summon_jauge = 50 + val_summon;
    //Log(MSG_INFO,"%s:: New Gauge is %i",CharInfo->charname,summon_jauge);

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
    //char* questBuffer = new char[sizeof(SQuestData)*3 +1];
    //mysql_real_escape_string(GServer->DB->Mysql, questBuffer, (const char*)&quest, sizeof(SQuestData));
    //GServer->DB->QExecute( "DELETE FROM list_quest_new WHERE owner=%i",thisclient->CharInfo->charid );
    //GServer->DB->QExecute("INSERT INTO list_quest_new (owner, quests) VALUES(%i,'%s')",
    //    thisclient->CharInfo->charid, questBuffer);
    //delete[] questBuffer;

	//PY retro method
    //GServer->DB->QFree( );
    //Log(MSG_DEBUG,"Started SaveQuests");
	//make qflgs string to save
	
    GServer->DB->QExecute( "DELETE FROM player_quests WHERE owner=%i",thisclient->CharInfo->charid );
    Log(MSG_DEBUG,"Ran the player_quests deletion query");
	int Questcount = 0;
    for(int i=0;i<10;i++)
    {
        if (thisclient->quest.quests[i].QuestID == 0) continue;
		Questcount++;
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
		GServer->DB->QExecute("INSERT INTO player_quests (owner, questid, starttime, vbl, switches, items, count) VALUES(%i,%i,%i,'%s','%s','%s','%s')",
		thisclient->CharInfo->charid, thisclient->quest.quests[i].QuestID, thisclient->quest.quests[i].StartTime,  vbl, switches, itemslist, itemcount);
	}
        
		
	char episode[50] = ""; //make  the episode variable list
    for(int j=0;j<5;j++)
    {
        sprintf(&episode[strlen(episode)], "%u",thisclient->quest.EpisodeVar[j]);
        if(j < 4)
                sprintf(&episode[strlen(episode)],",");
    }
    char jobvar[30] = ""; //make  the job variable list
    for(int j=0;j<3;j++)
    {
        sprintf(&jobvar[strlen(jobvar)], "%u",thisclient->quest.JobVar[j]);
        if(j < 2)
                sprintf(&jobvar[strlen(jobvar)],",");
    }
    char planetvar[70] = ""; //make  the planet variable list
    for(int j=0;j<7;j++)
    {
        sprintf(&planetvar[strlen(planetvar)], "%u",thisclient->quest.PlanetVar[j]);
        if(j < 6)
                sprintf(&planetvar[strlen(planetvar)],",");
    }
    char unionvar[100] = ""; //make  the union variable list
    for(int j=0;j<10;j++)
    {
        sprintf(&unionvar[strlen(unionvar)], "%u",thisclient->quest.UnionVar[j]);
        if(j < 9)
                sprintf(&unionvar[strlen(unionvar)],",");
    }
	GServer->DB->QExecute( "DELETE FROM qvars WHERE owner=%i",thisclient->CharInfo->charid );
    GServer->DB->QExecute("INSERT INTO qvars (owner, episode, job, planet, unionvar) VALUES(%i, '%s','%s','%s','%s')",
    thisclient->CharInfo->charid, episode, jobvar, planetvar, unionvar);
    Log(MSG_DEBUG,"Ran the qvars insertion query");
    
    //GServer->DB->QFree( );
	Log(MSG_DEBUG,"Found %i active quests in memory and saved them", Questcount);
    //now the quest flags
    
    

	//PY TODO. Recode this to make one entry per player rather than potentially 512 different rows in the DB
    int flagmax = QUEST_SWITCH_CNT / 8;
	char qflgs[1024];
	for(int i=0;i<flagmax;i++)
	{
		sprintf(&qflgs[strlen(qflgs)], "%u",thisclient->quest.Qflags[i]);
		if(i <flagmax-1)
			sprintf(&qflgs[strlen(qflgs)],",");
	}
	GServer->DB->QExecute( "DELETE FROM qflags WHERE owner=%i",thisclient->CharInfo->charid );
	GServer->DB->QExecute("INSERT INTO qflags (owner, switches) VALUES(%i, '%s')", thisclient->CharInfo->charid, qflgs);
	Log(MSG_DEBUG,"Ran the qflags insertion query");
	//PY ok done

	/*
    int flagCount = 0;
	for(int flagid=0;flagid<QUEST_SWITCH_CNT;flagid++) //need to scan all 512 for this
    {
        bool thisflag = quest.GetFlag(flagid);
		if(thisflag == true)
        {
            GServer->DB->QExecute("INSERT INTO questflags (owner, flags) VALUES(%i,%i)", thisclient->CharInfo->charid, flagid);
            flagCount++;
        }
    }
	*/
	//Log(MSG_DEBUG,"inserted %i flags",flagCount);
	Log(MSG_DEBUG,"Finished saving quest data");
}


//LMA: getting the family skill.
int CPlayer::GoodSkill(int skill_id)
{
    if (skill_id>GServer->maxSkills)
    {
        Log(MSG_WARNING,"Bad skill_id in GoodSkill (%i>%u)",skill_id,GServer->maxSkills);
        return -1;
    }

    int type = GServer->SkillList[skill_id]->skill_tab;

    if (type == 11)
        return 2;   //basic
    if (type == 41)
        return 3;   //unique
    if (type == 51)
        return 4;   //mileage
    if (type >= 20 && type < 30)
        return 0;   //class
    if (type >= 30 && type < 40)
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

    begin[0] = 0;
    begin[1] = 60;
    begin[2] = 320;
    begin[3] = 90;
    begin[4] = 120;
    end[0] = 60;
    end[1] = 90;
    end[2] = 362;
    end[3] = 120;
    end[4] = 320;

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


    begin[0] = 0;
    begin[1] = 60;
    begin[2] = 320;
    begin[3] = 90;
    begin[4] = 120;

    for (int k=0;k<4;k++)
    {
        //We don't reset the basic skills.
        if(k == 2)
            continue;
        cur_max_skills[k] = begin[k];
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
        if(cskills[k].thisskill == NULL)
        {
            cskills[k].id = 0;
            cskills[k].level = 1;
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
        if(cskills[k].thisskill == NULL)
        {
            cskills[k].id = 0;
            cskills[k].level = 1;
        }

    }


    return;
}

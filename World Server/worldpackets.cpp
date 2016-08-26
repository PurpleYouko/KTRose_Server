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
#include "worldserver.h"

// Send Characters information
void CWorldServer::pakPlayer( CPlayer *thisclient )
{
    bool has_cart = false;
    bool has_cg = false;
    int nb_30 = 30;
    int nb_skills = 0;
    bool is_dual_scratch = false;


	CRespawnPoint* thisrespawn = GetRespawnByID( thisclient->Position->respawn );
	if(thisrespawn != NULL)
	{
        thisclient->Position->destiny = thisrespawn->dest;
    	thisclient->Position->current = thisclient->Position->destiny;
    	thisclient->Position->Map = thisrespawn->destMap;
    }
    else
    {
        thisclient->Position->destiny.x = 5300;
        thisclient->Position->destiny.y = 5200;
        thisclient->Position->current = thisclient->Position->destiny;
        thisclient->Position->Map = 2;
    }

    CMap* map = MapList.Index[thisclient->Position->Map];
    map->AddPlayer( thisclient );

    //LMA: we execute the map "welcome quest trigger", used to set pvp most of the time.
    if(!thisclient->is_born)
    {
        thisclient->pvp_id=-1;
        dword hash_zone  =map->QSDzone;

        if(hash_zone > 0)
        {
            //int success=thisclient->ExecuteQuestTrigger(hash_zone,false);
            int success = thisclient->ExecuteQuestTrigger(hash_zone,true);
            if (success == QUEST_SUCCESS)
            {
                //Log(MSG_WARNING,"successful first map %i trigger for %s",map->id,thisclient->CharInfo->charname);
            }
            else
            {
                Log(MSG_WARNING,"UNsuccessful first map %i trigger for %s",map->id,thisclient->CharInfo->charname);
            }

        }

        thisclient->is_born = true;
    }

    //LMA: Little fix if players comes with too much HP / MP In Game (exit when fairied?)
    if (thisclient->Stats->HP > thisclient->GetMaxHP())
       thisclient->Stats->HP = thisclient->GetMaxHP();
    if (thisclient->Stats->MP > thisclient->GetMaxMP())
       thisclient->Stats->MP = thisclient->GetMaxMP();

	//driving skills :)
	for (int i=60;i<60+MAX_DRIVING_SKILL;i++)
	{
	    if (thisclient->cskills[i].id == 5000)
            has_cart = true;
	    if (thisclient->cskills[i].id == 5001)
            has_cg = true;
        if (thisclient->cskills[i].id == 0)
            break;
	}

    BEGINPACKET( pak, 0x715 );
	//PY:	Client loads in blocks of memory equal in size to the defined structures in the client end
	//first few data pieces are independent of structures though
	/*
	BYTE				m_btCharRACE;
	short				m_nZoneNO;
	tPOINTF				m_PosSTART;
	short				m_nReviveZoneNO;
	*/
	//So this block doesn't match entirely
	
    ADDBYTE    ( pak, thisclient->CharInfo->Sex );               // Sex		PY: Should be race but seems that the two end up being the same thing in the client
    ADDWORD    ( pak, thisclient->Position->Map );		         // Map
    ADDFLOAT   ( pak, thisclient->Position->current.x*100 );	 // Pos X
    ADDFLOAT   ( pak, thisclient->Position->current.y*100 );	 // Pos Y
    //ADDWORD    ( pak, 0x0000 );
    ADDWORD    ( pak, thisclient->Position->respawn );			// IS respawn the correct position to use?

	//END independent variables
	// Now it needs 
	//tagPartITEM			m_PartITEM[ MAX_BODY_PART   ];		MAX_BODY_PART = 10
	/*
	struct tagPartITEM {
	unsigned int	m_nItemNo		: 18;	// 0~1023	Ah system number(ITEM_ID)		(0 ~ 999)
	unsigned int	m_nGEM_OP		: 9;	// 0~512	Jewelry number(m_bHasSocket==1) Or a number of options(m_bHasSocket==0)
	unsigned int	m_bHasSocket	: 1;	// 0~1		Jewelry, whether or not a socket
	unsigned int	m_cGrade	    : 4;	// 0~15		Rating						(0~9)
	} ;
	*/
    ADDDWORD   ( pak, thisclient->CharInfo->Face );			     // Face
    ADDDWORD   ( pak, thisclient->CharInfo->Hair );	             // Hair
    ADDWORD    ( pak, thisclient->items[2].itemnum );	         // Cap
    ADDWORD    ( pak, BuildItemRefine( thisclient->items[2] ) ); // Cap Refine
    ADDWORD    ( pak, thisclient->items[3].itemnum );	         // Body
    ADDWORD    ( pak, BuildItemRefine( thisclient->items[3] ) ); // Body Refine
    ADDWORD    ( pak, thisclient->items[5].itemnum );	         // Gloves
    ADDWORD    ( pak, BuildItemRefine( thisclient->items[5] ) ); // Gloves Refine
    ADDWORD    ( pak, thisclient->items[6].itemnum );	         // Boots
    ADDWORD    ( pak, BuildItemRefine( thisclient->items[6] ));	 // Boots Refine
    ADDWORD    ( pak, thisclient->items[1].itemnum );	         // Face
    ADDWORD    ( pak, BuildItemRefine( thisclient->items[1] ) ); // Face Refine
    ADDWORD    ( pak, thisclient->items[4].itemnum );	         // Back
    ADDWORD    ( pak, BuildItemRefine( thisclient->items[4] ) ); // Back Refine
    ADDWORD    ( pak, thisclient->items[7].itemnum );	         // Weapon
    ADDWORD    ( pak, BuildItemRefine( thisclient->items[7] ) ); // Weapon Refine
    ADDWORD    ( pak, thisclient->items[8].itemnum );	         // SubWeapon
    ADDWORD    ( pak, BuildItemRefine( thisclient->items[8] ) ); // SubWeapon Refine
    ADDBYTE    ( pak, 0 );
    ADDWORD    ( pak, 0x140f );
	ADDWORD    ( pak, thisclient->CharInfo->Job );			     // Job
    ADDBYTE    ( pak, thisclient->CharInfo->unionid );           //LMA: Union ID

    ADDBYTE( pak, 0 );                                           //LMA: Union Fame
    ADDBYTE( pak, thisclient->CharInfo->unionfame );             //LMA: Union Fame

    ADDWORD    ( pak, thisclient->Attr->Str );			         // Str
    ADDWORD    ( pak, thisclient->Attr->Dex );				     // Dex
    ADDWORD    ( pak, thisclient->Attr->Int );				     // Int
    ADDWORD    ( pak, thisclient->Attr->Con );				     // Con
    ADDWORD    ( pak, thisclient->Attr->Cha );				     // Cha
    ADDWORD    ( pak, thisclient->Attr->Sen );				     // Sen
	//PY:														Start tagGrowAbility (client)	
	ADDWORD    ( pak, thisclient->Stats->HP );                   // Current HP
	ADDWORD    ( pak, thisclient->Stats->MP );                   // Current MP
	ADDWORD	   ( pak, thisclient->Stats->MaxHP );				 // MaxHP. Needed since adding m_MaxHP to tagGrowAbility structure in client
	ADDWORD	   ( pak, thisclient->Stats->MaxMP );				 // MaxMP. Needed since adding m_MaxMP to tagGrowAbility structure in client
	ADDDWORD   ( pak, thisclient->CharInfo->Exp );               // Exp
    //ADDWORD    ( pak, 0 );									//PY: don't think this should be here
	ADDWORD    ( pak, thisclient->Stats->Level );			      // Level
	ADDWORD    ( pak, thisclient->CharInfo->StatPoints );		  // Stat Points
	ADDWORD    ( pak, thisclient->CharInfo->SkillPoints );        // Skill Points
	ADDWORD    ( pak, 0x6464 );									//PY: This should be two bytes. Body_Size, Head_Size. might be useful one day
    //PY: Client structures starting here
	/*
	long	m_lPenalEXP;		// Add experience ...
	short	m_nFameG;			// Leading Indicators: Increased by Quest: 05/27/2004 added
	short	m_nFameB;			// Leading Indicators: Increased by Quest: 05/27/2004 added
	
	*/
	for(int i=0; i<4; i++)										//PY: this loop takes care of the previous 4 bytes. everything up to union points
       ADDWORD( pak,0);       //null
	/*
	short	m_nUnionPOINT[ MAX_UNION_COUNT ];					//PY: (from client) Point combinations: Add 05/27/2004 (MAX_UNION_COUNT = 10)
	*/
    //LMA: Union points
    ADDWORD( pak, thisclient->CharInfo->union01 );
    ADDWORD( pak, thisclient->CharInfo->union02 );
    ADDWORD( pak, thisclient->CharInfo->union03 );
    ADDWORD( pak, thisclient->CharInfo->union04 );
    ADDWORD( pak, thisclient->CharInfo->union05 );				//PY: well this is 5 of the 10 shorts(WORDS). Where are the other 5?


    //rest is 0?
	for(int i=0; i<19; i++)										//PY: Now 19 null bytes?? WTF 10 of them takes care of the missing 5 union points. that leaves 9
        ADDBYTE( pak, 0 );    //null

	//PY: Here is this section from the client
	/*
	int		m_iGuildNO;			// Guild Code: Add 27/05/2004				4 bytes
	short	m_nGuildCNTRB;		// Guild Contribution: 05/27/2004 added		2 bytes
	BYTE	m_btGuildPOS;		// Guild Position: 05.27.2004 Add			1 byte

	short	m_nPKFlag;			// 2004. 6. 17 added..						2 bytes		9 used up to this point so we should line up just fine with stamina
	short	m_nSTAMINA;			// 2004. 8. 23 added..						2 bytes
	*/

    ADDWORD( pak, thisclient->CharInfo->stamina );					// Stamina
	//PY:																	END tagGrowAbility


    for(int i=0; i<320; i++) ADDBYTE( pak, 0 );

    //LMA: Value for driving skill so it doesn't say missing conditions ?
    if (has_cart||has_cg)
    {
        ADDWORD( pak, 3500 );
    }
    else
    {
        ADDWORD( pak, 0 );
    }

    ADDWORD( pak, 0 );
    ADDWORD( pak, 0 );

    //LMA: test 2008/11/07 (reorganisation)
 	for(int i=0; i<MAX_CLASS_SKILL; i++) // Class Skills
 	{
 	    if(thisclient->cskills[i].id == 0)
 	    {
 	        ADDWORD( pak,0);
 	        continue;
 	    }

        ADDWORD( pak, thisclient->cskills[i].id + thisclient->cskills[i].level-1 );
		//Log( MSG_DEBUG, "Sending CSkill.  Slot: %i ID: %i Level: %i", i,thisclient->cskills[i].id,thisclient->cskills[i].level);
 	}

    //Driving skill (and some stuff)
	for (int i=60;i<60+MAX_DRIVING_SKILL;i++)
	{
        ADDWORD( pak, thisclient->cskills[i].id );
	}

    //Unique
    for(int i=90; i<90+MAX_UNIQUE_SKILL; i++)
    {
 	    if(thisclient->cskills[i].id==0)
 	    {
 	        ADDWORD( pak,0);
 	        continue;
 	    }

        ADDWORD( pak, thisclient->cskills[i].id+thisclient->cskills[i].level-1 );
    }

    //Mileage
    for(int i=120; i<120+MAX_MILEAGE_SKILL; i++)
    {
 	    if(thisclient->cskills[i].id==0)
 	    {
 	        ADDWORD( pak,0);
 	        continue;
 	    }

        ADDWORD( pak, thisclient->cskills[i].id+thisclient->cskills[i].level-1 );
    }

    //Basic Skills
	for(int i=320; i<320+MAX_BASIC_SKILL; i++)
	{
        ADDWORD( pak, thisclient->cskills[i].id);
	}

	for(int i=0; i<48; i++)       // QuickBar
        ADDWORD( pak, thisclient->quickbar[i] );

	ADDDWORD   ( pak, thisclient->CharInfo->charid );	            // CharID
	for(int i=0; i<80;i++) ADDBYTE( pak, 0 );						// leaves an 80 byte space equal to client structure		DWORD m_dwCoolTime[MAX_SKILL_RELOAD_TYPE];
	
	//Log( MSG_DEBUG, "0x0715 packet MaxHP = %i Stamina = %i",thisclient->Stats->MaxHP,thisclient->CharInfo->stamina );
	//Log( MSG_DEBUG, "0x0715 packet CharID = %i",thisclient->CharInfo->charid );
    ADDSTRING  ( pak, thisclient->CharInfo->charname );             // Char Name
	ADDBYTE    ( pak, 0 );
    thisclient->client->SendPacket( &pak );
}

// Send Inventory Information
void CWorldServer::pakInventory( CPlayer *thisclient )
{
	BEGINPACKET( pak, 0x716 );
	ADDQWORD( pak, thisclient->CharInfo->Zulies );
	//ADDWORD( pak, 0 );
	for(unsigned j=0; j<140; j++)		//shouldn't this be 160?
    {
        pak = AddItemData(thisclient->items[j], pak);
	}

    thisclient->client->SendPacket( &pak );


    return;
}

// Get this user set up with the encryption and stuff
bool CWorldServer::pakDoIdentify( CPlayer *thisclient, CPacket *P )
{
	MYSQL_ROW row;
	thisclient->Session->userid = GETDWORD((*P), 0);
	memcpy( thisclient->Session->password, &P->Buffer[4], 32 );

	//LMA: checking the password:
    if(!CheckEscapeMySQL(thisclient->Session->password,32,true))
    {
        Log(MSG_HACK,"wrong password %s (size %i>32 or data) for user id %i",thisclient->Session->password,strlen(thisclient->Session->password),thisclient->Session->userid);
        return true;
    }

	MYSQL_RES *result = DB->QStore("SELECT username,lastchar,accesslevel,zulystorage,ktpoints,newpoints,logtime,totlogtime FROM accounts WHERE id=%i AND password='%s'", thisclient->Session->userid, thisclient->Session->password);
    if(result==NULL) return false;
	if (mysql_num_rows( result ) != 1)
    {
		Log( MSG_HACK, "Someone tried to connect to world server with an invalid account" );
		DB->QFree( );
		return false;
	}
	row = mysql_fetch_row(result);
	strncpy( thisclient->Session->username, row[0],16 );
	strncpy( thisclient->CharInfo->charname, row[1],16 );
	thisclient->Session->accesslevel = atoi(row[2]);
	thisclient->CharInfo->Storage_Zulies = atoll( row[3] );	
	thisclient->Session->KTPoints = atoi(row[4]);       //load KTPoints
	thisclient->Session->NewPoints = atoi(row[5]);      //load newPoints. This is where additional KTPoints are added when they are purchased
	thisclient->Session->logtime = atoi(row[6]);        //current log time in hours.
	thisclient->Session->TotalLogTime = atoi(row[7]);   //total log time for this account
	thisclient->Session->award = 0;
	thisclient->Session->RespawnChoice = 0;
	thisclient->Session->Respawned = 0;
	thisclient->Session->inGame = true;
	DB->QFree( );
	Log( MSG_INFO, "Loaded %i KTpoints", thisclient->Session->KTPoints);
	if(thisclient->Session->NewPoints != 0)
	{
        // Add NewPoints to KTPoints
        //Log( MSG_DEBUG, "Loaded up some new KTPoints and stuff" );
        thisclient->Session->KTPoints += thisclient->Session->NewPoints;
        // Set newpoints to zero both here and in the database
        thisclient->Session->NewPoints = 0;
        GServer->DB->QExecute("UPDATE accounts SET newpoints=%i, ktpoints=%i WHERE id=%i AND password='%s'",
                    thisclient->Session->NewPoints,thisclient->Session->KTPoints,thisclient->Session->userid, thisclient->Session->password);
        //GServer->DB->QExecute("UPDATE accounts SET ktpoints=%i WHERE id=%i AND password='%s'",
        //            thisclient->Session->KTPoints,thisclient->Session->userid, thisclient->Session->password);
    }
    if (thisclient->Session->logtime >= Config.KTPointRate) //make the KTPoint reward system variable
    {
        while(thisclient->Session->logtime > Config.KTPointRate)
        {
            thisclient->Session->logtime -= Config.KTPointRate;
            thisclient->Session->award ++;
        }
        thisclient->Session->KTPoints += thisclient->Session->award;
        //thisclient->Union_s->unionvar[9] = thisclient->Session->KTPoints;
        GServer->DB->QExecute("UPDATE accounts SET ktpoints=%i,logtime=%i WHERE id=%i AND password='%s'",
            thisclient->Session->KTPoints, thisclient->Session->logtime, thisclient->Session->userid, thisclient->Session->password);
    }

	//LMA: Anti hack (multi client on the same userid)
	if(thisclient->Session->first_id)
	{
	    if(GetNbUserID(thisclient->Session->userid)>1)
	    {
	        Log(MSG_HACK,"[HACK] UserID %u tryes to log %s but he has already another avatar loaded!",thisclient->Session->userid,thisclient->CharInfo->charname);
	        ForceDiscClient(thisclient->Session->userid);
	        return false;
	    }

	    thisclient->Session->first_id=false;
	}

	if(!thisclient->loaddata( )) return false;
	Log( MSG_INFO, "User '%s'(#%i) logged in with character '%s'", thisclient->Session->username, thisclient->Session->userid, thisclient->CharInfo->charname);
	BEGINPACKET( pak, 0x070c );
	ADDBYTE    ( pak, 0 );
	ADDDWORD   ( pak, 0x87654321 );
	ADDDWORD   ( pak, 0x00000000 );
	thisclient->client->SendPacket( &pak );
    
    pakPlayer(thisclient);
	Log( MSG_INFO, "Character '%s' completed pakPlayer", thisclient->CharInfo->charname);
    pakInventory(thisclient);
	Log( MSG_INFO, "Character '%s' completed pakInventory", thisclient->CharInfo->charname);
    pakQuestData(thisclient);
	Log( MSG_INFO, "Character '%s' completed pakQuestData", thisclient->CharInfo->charname);

    //LMA: Jrose unlimited (zrose)
    RESETPACKET( pak, 0x7de );
    ADDWORD ( pak, 0x1001 ); // 0x1001 to 0x1013 (game plan?)
    ADDDWORD ( pak, 2 ); // options (plan time?) [2 = unlimited]
    thisclient->client->SendPacket( &pak );

	RESETPACKET( pak, 0x702 );
	ADDSTRING  ( pak, Config.WELCOME_MSG );
	ADDBYTE    ( pak, 0 );
	thisclient->client->SendPacket( &pak );

	//SendSysMsg( thisclient, "Open Source Rose Online Private Server" );

	thisclient->SetStats( );
	//PY send client all the cmbat stats through the new system added to the client
	RESETPACKET	( pak, 0x7ed );
	ADDWORD		( pak, thisclient->Stats->Attack_Power );
	ADDWORD		( pak, thisclient->Stats->Defense );
	ADDWORD		( pak, thisclient->Stats->Accury );
	ADDWORD		( pak, thisclient->Stats->Dodge );
	ADDWORD		( pak, thisclient->Stats->Magic_Defense );
	ADDWORD		( pak, thisclient->Stats->Critical );
	SendToVisible( &pak, thisclient );

	return true;
}

// Give the user an ID
bool CWorldServer::pakDoID( CPlayer* thisclient, CPacket* P )
{
	// Assign a new id to this person
   	thisclient->clientid = GetNewClientID();
	if (thisclient->clientid <=1)
    {
		Log( MSG_WARNING, "User '%s'(#%i) denied access. Server is full.", thisclient->Session->username, thisclient->Session->userid );
		return false;
	}

	if(thisclient->Stats->HP <= 0)
        thisclient->Stats->HP = 10 * thisclient->GetMaxHP( )/100;

	Log( MSG_INFO, "User '%s'(#%i) assigned id #%i", thisclient->Session->username, thisclient->Session->userid, thisclient->clientid );
    if( thisclient->Party->party )
    {
        /// send party details
		//receiving structure
		/*
		struct tag_PARTY_MEMBER // John sent by the server
		{	
			DWORD	m_dwUserTAG;
			WORD	m_wObjectIDX;
			short	m_nMaxHP;
			short	m_nHP;
			DWORD	m_dwStatusFALG;

			short	m_nCON;						// BYTE	m_btCON; //Increasing the maximum range of 300 stats as of 2004. 7.21

			BYTE	m_btRecoverHP;				// item_recover_hp + passive_recover_hp
			BYTE	m_btRecoverMP;				// item_recover_mp + passive_recover_mp

			short	m_nSTAMINA;

			//char	m_szCharName[];				// ** PARTY_MEMBER_ADD Only when the value contains
		} ;
		*/
		BEGINPACKET( pak, 0x7d5 );
        ADDDWORD    ( pak, thisclient->CharInfo->charid );
        ADDWORD     ( pak, thisclient->clientid );
        ADDWORD     ( pak, thisclient->GetMaxHP( ) );
        ADDWORD     ( pak, thisclient->Stats->HP );
        ADDDWORD    ( pak, GServer->BuildBuffs( thisclient ));	//Tomiz : Buff Data
        //PY: should be a short and two bytes here. Player's CON followed by HP recovery rate and MP recovery rate. 
		//TODO: To be investigated and fixed later
		ADDDWORD    ( pak, 0x1f40008c );//Tomiz
		//PY: should be stamina so what's this 0x1388 crap. 0x1388 is equal to 5000 decimal so this is max stamina instead of Actual stamina
        //ADDWORD    ( pak, 0x1388 );
		ADDWORD		( pak, thisclient->CharInfo->stamina );
        thisclient->Party->party->SendToMembers( &pak, thisclient );
    }

    //LMA: We do the warp zone here.
    CMap* map = MapList.Index[thisclient->Position->Map];
    //LMA: We do a quest trigger if needed...
    //If we are already in the map, no need to redo all the stuff again.
    //We don't do the qsdzone again if in the UW map...
    dword hash_zone=map->QSDzone;
    //if(hash_zone>0&&((id!=thisclient->Position->Map)||(id!=9)))

    if(thisclient->skip_qsd_zone)
    {
        hash_zone=0;
        thisclient->skip_qsd_zone=false;
    }

    if(hash_zone>0)
    {
        //LMA: we set the pvp vars to nothing again.
        thisclient->pvp_id=-1;
        thisclient->pvp_status=-1;

        //int success=thisclient->ExecuteQuestTrigger(hash_zone,false);
        int success=thisclient->ExecuteQuestTrigger(hash_zone,true);
        if (success==QUEST_SUCCESS)
        {
            //Log(MSG_WARNING,"teleporting player %s, quest %u returned ok",thisclient->CharInfo->charname,hash_zone);

            //Special case for map 9 (UW).
            if(map->id==9)
            {
                if(thisclient->UWPosition->Map>0&&thisclient->UWPosition->source.x>0&&thisclient->UWPosition->source.y>0)
                {
                    Log(MSG_WARNING,"Player %s is UW teleporting to map %i (%.2f,%.2f)",thisclient->CharInfo->charname,thisclient->UWPosition->Map,thisclient->UWPosition->source.x,thisclient->UWPosition->source.y);
                }

            }

        }
        else
        {
            Log(MSG_WARNING,"teleporting player %s, quest %u returned false",thisclient->CharInfo->charname,hash_zone);
        }

    }
    //LMA: end of zone

    BEGINPACKET( pak, 0x753 );
    ADDWORD    ( pak, thisclient->clientid );			// USER ID
    ADDWORD    ( pak, thisclient->Stats->HP );		// CURRENT HP
    ADDWORD    ( pak, thisclient->Stats->MP );		// CURRENT MP
    ADDDWORD   ( pak, thisclient->CharInfo->Exp );				// CURRENT EXP
    ADDDWORD   ( pak, 0x00000000 );						// LVL EXP (UNSUSED)
    // thanks to StrikeX to post this source
        //[economy]
    ADDWORD    ( pak, 0x0063 );  // World Rate
    ADDBYTE    ( pak, 0x70 );
    ADDBYTE    ( pak, 0x69 );
    ADDBYTE    ( pak, 0x68 );
    ADDBYTE    ( pak, 0x67 );
    ADDWORD    ( pak, 0x0062 );  // Town rate
    ADDBYTE    ( pak, 0x61 );  // misc rate
    ADDBYTE    ( pak, 0x32 );  //1
    ADDBYTE    ( pak, 0x32 );  //2
    ADDBYTE    ( pak, 0x32 );  //3
    ADDBYTE    ( pak, 0x32 );  //4
    ADDBYTE    ( pak, 0x32 );  //5
    ADDBYTE    ( pak, 0x32 );  //6
    ADDBYTE    ( pak, 0x32 );  //7
    ADDBYTE    ( pak, 0x32 );  //8
    ADDBYTE    ( pak, 0x32 );  //9
    ADDBYTE    ( pak, 0x32 );  //10
    ADDBYTE    ( pak, 0x32 );  //11

    //CMap* map = MapList.Index[thisclient->Position->Map];

    //LMA: new pvp code.
    if(map->allowpvp!=1)
    {
        //non player vs player map
        ADDWORD(pak, 0x0000);
    }
    else
    {
        //player vs player map
        ADDWORD(pak, 0x0001);
    }

    ADDWORD    (pak, 0x0000 );

    // Map Time
    ADDDWORD( pak, map->MapTime );

    thisclient->pvp_id=thisclient->ReturnPvp(NULL,thisclient);
    ADDWORD(pak,thisclient->pvp_id);

    //LMA: Update.
    ADDWORD(pak, 0x0000 );
    thisclient->client->SendPacket( &pak );

    //LMA: Are we in overweight?
    BYTE pc_weight=1;
    if(thisclient->GetMaxWeight()>0)
    {
        pc_weight=thisclient->GetCurrentWeight()*100/thisclient->GetMaxWeight();
    }

    if(pc_weight>100)
    {
        thisclient->Status->CanRun=false;
        thisclient->Stats->Move_Speed = thisclient->GetMoveSpeed( );
    }

    if(pc_weight>110)
    {
        thisclient->Status->CanAttack=false;
    }

    // set weight
    /*
    RESETPACKET( pak, 0x762 );
    ADDWORD    ( pak, thisclient->clientid );       	// USER ID
    ADDBYTE    ( pak, 1 );								// SOMETHING TO DO WITH WEIGHT
    thisclient->client->SendPacket( &pak );*/

    //LMA: complete version.
    RESETPACKET( pak, 0x762 );
    ADDWORD    ( pak, thisclient->clientid );       	// USER ID
    ADDBYTE    ( pak, pc_weight );						//% weight.
    thisclient->client->SendPacket( &pak );

    // set speed
	RESETPACKET(pak, 0x782 );
	ADDWORD    ( pak, thisclient->clientid );
	ADDBYTE    ( pak, thisclient->Status->Stance );
	ADDWORD    ( pak, thisclient->Stats->Move_Speed );

	SendToVisible( &pak, thisclient );
	
    thisclient->CleanPlayerVector( );
	thisclient->Session->inGame = true;
	thisclient->firstlogin = clock();    //LMA for fairy

	//LMA: In some special cases, we have to warp the guy again...
	if(thisclient->map_warp_zone != 0 && thisclient->Warp_Zone.x != 0 && thisclient->Warp_Zone.y != 0)
	{
	    CMap* mapWarp = MapList.Index[thisclient->map_warp_zone];
	    mapWarp->TeleportPlayer(thisclient,thisclient->Warp_Zone);
	    thisclient->map_warp_zone=0;
	    thisclient->Warp_Zone.x=0;
	    thisclient->Warp_Zone.y=0;
	}


	return true;
}

// Move Characters in map
bool CWorldServer::pakMoveChar( CPlayer* thisclient, CPacket* P )
{
    if( thisclient->Shop->open || (!thisclient->Ride->Drive && thisclient->Ride->Ride) || !thisclient->Status->CanMove || thisclient->Status->Stun != 0xff || thisclient->Status->Sleep != 0xff)
        return true;
    if( thisclient->Status->Stance==1 )
        thisclient->Status->Stance=3;
    ClearBattle( thisclient->Battle );
	thisclient->Battle->target = GETWORD((*P), 0x00 );
	thisclient->Position->destiny.x = GETFLOAT((*P), 0x02 )/100;
    thisclient->Position->destiny.y = GETFLOAT((*P), 0x06 )/100;
    thisclient->Position->lastMoveTime = clock();

    //LMA: Checking if we got fuel.
    if(thisclient->Status->Stance==DRIVING)
    {
        if(thisclient->items[136].itemnum==0)
        {
            Log(MSG_WARNING,"%s should be riding but doesn't have a motor?",thisclient->CharInfo->charname);
            return true;
        }

        if(thisclient->items[136].lifespan==0)
        {
            //No fuel anymore, can't move.
            return true;
        }

    }

    //LMA: Overweight player.
    if(thisclient->Status->Stance==RUNNING)
    {
        if(!thisclient->Status->CanRun)
        {
            thisclient->Status->Stance = WALKING;
            thisclient->Stats->Base_Speed = thisclient->GetMoveSpeed();
            thisclient->Stats->Move_Speed = thisclient->Stats->Base_Speed;
        }

    }

	BEGINPACKET( pak, 0x79a );
	ADDWORD    ( pak, thisclient->clientid );		// USER ID
	ADDWORD    ( pak, thisclient->Battle->target );		// TARGET

	//LMA: Base Speed
	//ADDWORD    ( pak, thisclient->Stats->Move_Speed );	// MSPEED
    //ADDWORD    ( pak, thisclient->Stats->Base_Speed );	// MSPEED

    //LMA: it's in fact the distance from current to destiny and not the base speed...
    ADDWORD    ( pak, (DWORD) (distance(thisclient->Position->current,thisclient->Position->destiny)*100));

	ADDFLOAT   ( pak, GETFLOAT((*P), 0x02 ) );	// POSITION X
	ADDFLOAT   ( pak, GETFLOAT((*P), 0x06 ) );	// POSITION Y
	ADDWORD    ( pak, GETWORD((*P), 0x0a ) );		// POSITION Z (NOT USED)
	SendToVisible( &pak, thisclient );

    if(thisclient->Position->Map==8)
    {
        Log(MSG_INFO,"pakMoveChar %s, (%.2f;%.2f) to (%.2f;%.2f) speed %u/%u",thisclient->CharInfo->charname,thisclient->Position->current.x,thisclient->Position->current.y,thisclient->Position->destiny.x,thisclient->Position->destiny.y,thisclient->Stats->Base_Speed,thisclient->Stats->Move_Speed);
    }

	return true;
}

// Stop Character movement
bool CWorldServer::pakStopChar( CPlayer* thisclient, CPacket* P )
{
    ClearBattle( thisclient->Battle );
	thisclient->Position->destiny.x = GETFLOAT((*P), 0x00 )/100;
    thisclient->Position->destiny.y = GETFLOAT((*P), 0x04 )/100;
	BEGINPACKET( pak, 0x770 );
	ADDWORD    ( pak, thisclient->clientid );		// USER ID
	ADDFLOAT   ( pak, thisclient->Position->destiny.x*100 );	// POSITION X
	ADDFLOAT   ( pak, thisclient->Position->destiny.y*100 );	// POSITION Y
	ADDWORD    ( pak, GETWORD((*P), 0x08 ) );		// POSITION Z (NOT USED)
	SendToVisible( &pak, thisclient );
	return true;
}

//LMA: Spawn Ifo Objects
bool CWorldServer::pakSpawnIfoObject( CPlayer* thisclient, UINT npctype,bool forcerefresh)
{
    if(npctype!=GServer->WarpGate.virtualNpctypeID)
        return true;

    //Credits to Maxxon for this one ;)
    BEGINPACKET(pak, 0x7d6);
    ADDWORD(pak, GServer->WarpGate.clientID);
    //ADDWORD(pak, 0x2022); // unused
    ADDBYTE(pak,GServer->WarpGate.IfoX);
    ADDBYTE(pak,GServer->WarpGate.IfoY);
    ADDWORD(pak, GServer->WarpGate.id); // object number
    ADDWORD(pak, GServer->WarpGate.IfoObjVar[0]); // event id. 0x0000 closed, 0x01 open

    //This can happen (see qsd Actions.
    if(thisclient!=NULL)
    {
        thisclient->client->SendPacket( &pak );
    }

    if (forcerefresh)
    {
        //we send to all people in map, just to be sure ^_^
        SendToAllInMap(&pak,WarpGate.mapid);
    }

    //Log(MSG_INFO,"Spawning WarpGate, does it appear? %i",GServer->WarpGate.IfoObjVar[0]);


	return true;
}


// Spawn NPC
bool CWorldServer::pakSpawnNPC( CPlayer* thisclient, CNPC* thisnpc )
{
	BEGINPACKET( pak, 0x791 );
    ADDWORD( pak, thisnpc->clientid );
	ADDFLOAT( pak, thisnpc->pos.x*100 );
    ADDFLOAT( pak, thisnpc->pos.y*100 );
    ADDFLOAT( pak, thisnpc->pos.x*100 );
    ADDFLOAT( pak, thisnpc->pos.y*100 );
    ADDWORD( pak, 0x0000 );
	ADDWORD( pak, 0x0000 );
	ADDBYTE( pak, 0x00 );
	ADDWORD( pak, 0x03e8 );
	ADDWORD( pak, 0x0000 );
	ADDWORD( pak, 0x0001 );					
	ADDWORD( pak, 0x0000 );//Buffs
	ADDWORD( pak, 0x0000 );//Buffs
	ADDWORD( pak, 0x0000 );//buffs
	ADDWORD( pak, thisnpc->npctype );

	//LMA: Dialog time, or we send the dialogID (default one), or the tempdialogID (event for example).
	if (thisnpc->dialog!=0)
	{
        ADDWORD( pak, thisnpc->dialog );
        //Log(MSG_INFO,"Special dialog %i for NPC %i",thisnpc->dialog, thisnpc->npctype);
    }
    else if(thisnpc->thisnpc->dialogid!=0)
    {
         //Log(MSG_INFO,"Default dialog %i for NPC %i",thisnpc->thisnpc->dialogid, thisnpc->npctype);
         ADDWORD( pak, thisnpc->thisnpc->dialogid );
    }
    else
    {
        // NPC dialog should be type - 900, if not specified in DB
        ADDWORD( pak, thisnpc->npctype - 900 );
        //Log(MSG_INFO,"NO default or special dialog %i for NPC %i",thisnpc->npctype - 900, thisnpc->npctype);
    }

	//PY Now we need to add level and size since this reads into struct GSV_MOB_CHAR
	ADDWORD ( pak, thisnpc->thisnpc->level )
	ADDWORD ( pak, 0x0000 )						//PY: for now send zero size for the NPC. The client will use default size
	//PY end:

	ADDFLOAT( pak, thisnpc->dir );

    if (thisnpc->npctype == 1115)
    {
       ADDBYTE( pak, GServer->Config.Cfmode ) // Burtland Clan Field open/close
       ADDBYTE( pak, 0 );
    }

    //2do: error I guess, extra DWORD in the case of CF NPC.

    //Event:
    if (thisnpc->event!=0&&thisnpc->npctype!=1115)
    {
        //Log(MSG_INFO,"Event number %i for NPC %i",thisnpc->event, thisnpc->npctype);
        ADDWORD ( pak, thisnpc->event);
    }
    else if(thisnpc->npctype!=1115)
    {
        ADDWORD ( pak, 0 );
     }

    //ADDBYTE( pak, 0 );
    thisclient->client->SendPacket( &pak );
	return true;
}

// Changes stance
bool CWorldServer::pakChangeStance( CPlayer* thisclient, CPacket* P )
{
    if(thisclient->Shop->open)
        return true;
	BYTE stancenum = GETBYTE((*P),0x00);
	BYTE previous_stance=thisclient->Status->Stance;
	//Log(MSG_INFO,"Changing stance from %i to %i",previous_stance,stancenum);

	if (stancenum == 0)
	{
        if(thisclient->Status->Stance == RUNNING) //Walking
        {
            stancenum = WALKING;
            thisclient->Status->Stance = WALKING;
        }
        else
        if(thisclient->Status->Stance != DRIVING) // Running
        {
            stancenum = RUNNING;
            thisclient->Status->Stance = RUNNING;
        }
    }
    else
	if (stancenum == 1)
    {
        if(thisclient->Status->Stance!=DRIVING && thisclient->Status->Stance>0x01) //
        {   // Walking
			thisclient->Status->Stance = 0x01;
        }
        else
        if(thisclient->Status->Stance<0x04)
        {   //Runing
			thisclient->Status->Stance = 0x03;
        }
    }
    else
	if (stancenum == 2)
    {
		if (thisclient->Status->Stance==RUNNING || thisclient->Status->Stance==0x02)
		{// // Driving
            // Clean Buffs
            stancenum = DRIVING;
			thisclient->Status->Stance = DRIVING;
            thisclient->Ride->Ride = false;
            thisclient->Ride->Drive = true;
            thisclient->Ride->charid = 0;
        }
		else
		{ // Running
            stancenum = RUNNING;
			thisclient->Status->Stance = RUNNING;
        }
	}
    else
		thisclient->Status->Stance = stancenum;
	if(!thisclient->Status->CanMove)
	   thisclient->Status->Stance = RUNNING;
	thisclient->Stats->Move_Speed = thisclient->GetMoveSpeed( );

	BEGINPACKET( pak, 0x782 );				//Send packet to server containing clientid, stance and move speed
	ADDWORD( pak, thisclient->clientid );
	ADDBYTE( pak, thisclient->Status->Stance );
    ADDWORD( pak, thisclient->Stats->Move_Speed );
    SendToVisible( &pak, thisclient );

    //Fuel.
	if (thisclient->Status->Stance == DRIVING)
	{
       thisclient->last_fuel=clock();
       //forcing refresh for good value :)
       thisclient->TakeFuel();
    }

    if (previous_stance==DRIVING)
    {
       thisclient->TakeFuel();
       thisclient->last_fuel=0;
    }

   //LMA: eh eh ^_^
   //We force the refresh of the engine so the mspeed client side is done again.
   if(thisclient->Fairy&&(previous_stance==DRIVING||thisclient->Status->Stance==DRIVING))
   {
       int cartslot=1;
       int srcslot=136;
       int slot1=srcslot;
       int slot2=slot1;

       if(thisclient->items[slot1].itemnum==0)
       {
           return true;
       }

        RESETPACKET( pak, 0x718 );
        //if(slot2!=0xffff && slot2!=0xffff) {ADDBYTE( pak, 2 );}
        if(slot1!=0xffff && slot2!=0xffff) {ADDBYTE( pak, 2 );}
        else {ADDBYTE( pak, 1 );}
        if(slot1!=0xffff)
        {
            ADDBYTE    ( pak, slot1);
			pak = GServer->AddItemData(thisclient->items[slot1], pak);
            //ADDDWORD   ( pak, GServer->BuildItemHead( thisclient->items[slot1] ) );
            //ADDDWORD   ( pak, GServer->BuildItemData( thisclient->items[slot1] ) );
            //ADDDWORD( pak, 0x00000000 );
            //ADDWORD ( pak, 0x0000 );
        }
        if(slot2!=0xffff)
        {
            ADDBYTE    ( pak, slot2 );
			pak = GServer->AddItemData(thisclient->items[slot2], pak);
            //ADDDWORD   ( pak, GServer->BuildItemHead( thisclient->items[slot2] ) );
            //ADDDWORD   ( pak, GServer->BuildItemData( thisclient->items[slot2] ) );
            //ADDDWORD( pak, 0x00000000 );
            //ADDWORD ( pak, 0x0000 );
        }
        thisclient->client->SendPacket( &pak );
   }
    thisclient->SetStats( );
	return true;
}

// Spawn Drop
bool CWorldServer::pakSpawnDrop( CPlayer* thisclient, CDrop* thisdrop )
{
	BEGINPACKET( pak, 0x7a6 );
	ADDFLOAT( pak, thisdrop->pos.x*100 );
	ADDFLOAT( pak, thisdrop->pos.y*100 );
	if (thisdrop->type==1)
    {
		// -- ZULY --
		ADDDWORD( pak, 0xccccccdf );
		ADDDWORD( pak, thisdrop->amount );
        ADDDWORD( pak, 0xcccccccc );
            ADDWORD ( pak, 0xcccc );
		ADDWORD( pak, thisdrop->clientid );

		//LMA: it's the owner.
		//ADDWORD( pak, 0x0000 );
		ADDWORD( pak, thisdrop->owner );

		ADDWORD( pak, 0x5f90 );
	}
    else
    {
		// -- ITEM --
		//LMA: new way.
		//ADDDWORD( pak, BuildItemHead( thisdrop->item ) );
		//ADDDWORD( pak, BuildItemData( thisdrop->item ) );
        //    ADDDWORD( pak, 0x00000000 );
        //    ADDWORD ( pak, 0x0000 );
		//or the new PY way
		pak = AddItemData(thisdrop->item, pak);
		ADDWORD( pak, thisdrop->clientid );
		ADDWORD( pak, thisdrop->owner );
		ADDWORD( pak, 0x5f90 );
		//LMA: End
	}
	thisclient->client->SendPacket( &pak );
	return true;
}

// Player Left
void CWorldServer::pakClearUser( CPlayer* thisclient )
{
	BEGINPACKET( pak, 0x794 );
	ADDWORD( pak, thisclient->clientid );
	SendToVisible( &pak, thisclient, false );
}

// Drop items on map
bool CWorldServer::pakDoDrop( CPlayer* thisclient, CPacket* P )
{
    //LMA: Can a user drop when he's in shop?
    if(thisclient->Shop->open)
    {
        return true;
    }

	BYTE itemid = GETBYTE((*P), 0x0);
	if(!CheckInventorySlot(thisclient, itemid ))
	{
        return false;
	}

	DWORD amount = GETDWORD((*P), 0x1);
	if (itemid == 0)
    {
		if ( amount<1 )
		{
		    return true;
		}

		if ( thisclient->CharInfo->Zulies < amount )
		{
		    Log(MSG_WARNING,"%s tries to drop too many Zuly %u (he has %I64i)",thisclient->CharInfo->charname,amount,thisclient->CharInfo->Zulies);
		    return true;
		}

		CDrop* thisdrop = new CDrop;
		assert(thisdrop);
		thisdrop->clientid = GetNewClientID();
		thisdrop->type = 1; // ZULY
		thisdrop->pos = RandInCircle( thisclient->Position->current, 3 );
		thisdrop->posMap = thisclient->Position->Map;
		thisdrop->droptime = time(NULL);
		thisdrop->amount = amount;
		thisdrop->owner = 0;
		CMap* map = MapList.Index[thisdrop->posMap];
		map->AddDrop( thisdrop );
		thisclient->CharInfo->Zulies -= amount;
		BEGINPACKET( pak, 0x71d );
		ADDQWORD( pak, thisclient->CharInfo->Zulies );
		thisclient->client->SendPacket( &pak );
	}
    else
    {
        //LMA: is there something to drop to begin with?
        if(thisclient->items[itemid].itemtype==0||thisclient->items[itemid].itemnum==0)
        {
            Log(MSG_WARNING,"%s tries to drop empty item from slot %u",thisclient->CharInfo->charname,itemid);
            return true;
        }

        // fixed by tomciaaa [item count drop correctly and dissaper from inventory]
        bool flag = false;
        if(thisclient->items[itemid].itemtype >=10 && thisclient->items[itemid].itemtype <= 13)
        {
            if(thisclient->items[itemid].count<amount)
            {
                Log(MSG_WARNING,"%s tries to %i*(%u::%u) slot %u but he has only %u",thisclient->CharInfo->charname,amount,thisclient->items[itemid].itemtype,thisclient->items[itemid].itemnum,itemid,thisclient->items[itemid].count);
                return true;
            }


           thisclient->items[itemid].count -= amount;
           flag= (thisclient->items[itemid].count <= 0);
        }
        else
        {
           flag= true;
        }

		CDrop* thisdrop = new CDrop;
		assert(thisdrop);
		thisdrop->clientid = GetNewClientID();
		thisdrop->type = 2; // ITEM
		thisdrop->pos = RandInCircle( thisclient->Position->current, 3 );
		thisdrop->posMap = thisclient->Position->Map;
		thisdrop->droptime = time(NULL);
		thisdrop->amount = amount;
		thisdrop->item = thisclient->items[itemid];
		thisdrop->item.count = amount;
		thisdrop->owner = 0;
		CMap* map = MapList.Index[thisdrop->posMap];
		map->AddDrop( thisdrop );

		if (flag)
		{
		    ClearItem(thisclient->items[itemid]);
		}

        thisclient->UpdateInventory( itemid );
	}


	return true;
}

// Picks up item
bool CWorldServer::pakPickDrop( CPlayer* thisclient, CPacket* P )
{
    if(thisclient->Shop->open)
        return true;
    thisclient->RestartPlayerVal( );
	WORD dropid = GETWORD((*P), 0x00);
	CDrop* thisdrop = GetDropByID( dropid, thisclient->Position->Map );
	if (thisdrop==NULL) return true;
	bool flag = false;
    BEGINPACKET( pak, 0x7a7 );
	ADDWORD    ( pak, thisdrop->clientid );
    CPlayer* dropowner = 0;
    if( (thisdrop->owner==0 || thisdrop->owner==thisclient->CharInfo->charid || time(NULL)-thisdrop->droptime>=30 ) || ( thisclient->Party->party!=NULL && thisclient->Party->party == thisdrop->thisparty ))
    {
        if( thisclient->Party->party!=NULL )
        {
            unsigned int dropparty = thisclient->Party->party->Option/0x10;
            if( dropparty == 8 ) // Orderly
            {
                unsigned int num = 0;
                bool dpflag = false;
                if( thisdrop->type==1 )
                {
                    num = thisclient->Party->party->LastZulies;
                    thisclient->Party->party->LastZulies++;
                    if( thisclient->Party->party->LastZulies>=thisclient->Party->party->Members.size() )
                        thisclient->Party->party->LastZulies = 0;
                }
                else
                if( thisdrop->type==2 )
                {
                    num = thisclient->Party->party->LastItem;
                    thisclient->Party->party->LastItem++;
                    if( thisclient->Party->party->LastItem>=thisclient->Party->party->Members.size() )
                        thisclient->Party->party->LastItem = 0;
                }
                unsigned int n = 0;
                while( !dpflag )
                {
                    n++;
                    if( num>=thisclient->Party->party->Members.size() )
                        num = 0;
                    dropowner = thisclient->Party->party->Members.at( num );
                    if( dropowner == NULL )
                        num++;
                    else
                    {
                        dpflag = true;
                        num++;
                    }
                    if(n>20) // not founded yet? >.>
                    {
                        dropowner = thisclient;
                        dpflag = true;
                    }
                }
            }
            else // Equal Loot
            {
                dropowner = thisclient;
            }
        }
        else
        {
            dropowner = thisclient;
        }
        if( thisdrop->type == 1 ) //Zulie
        {
		    dropowner->CharInfo->Zulies += thisdrop->amount;
		    ADDWORD( pak, 0 );
		    ADDBYTE( pak, 0 );
		    ADDDWORD( pak, 0xccccccdf );
		    ADDDWORD( pak, thisdrop->amount );
                ADDDWORD( pak, 0xcccccccc );
                ADDWORD ( pak, 0xcccc );
		    dropowner->client->SendPacket( &pak );
		    flag = true;
        }
        else
        if( thisdrop->type == 2 ) // Item
        {
            //LMA: drop can be something else than a mere item...
            unsigned int type=0;
            if(thisdrop->item.itemtype==10)
            {
                type = UseList.Index[thisdrop->item.itemnum]->type;
            }

            if (type == 320 && thisdrop->item.itemtype == 10)
            {
                RESETPACKET( pak,0x7a3 );
                ADDWORD    ( pak, dropowner->clientid );
                ADDWORD    ( pak, thisdrop->item.itemnum );
                SendToVisible( &pak, dropowner );
                flag = true;

                //LMA: Adding the auto consumme stuff (most of credits to PY :) ).
                switch (UseList.Index[thisdrop->item.itemnum]->useeffect[0])
                {
                       case 16:
                       {
                            dropowner->Stats->HP += (UseList.Index[thisdrop->item.itemnum]->useeffect[1]);
                            if(dropowner->Stats->HP > dropowner->Stats->MaxHP )
                               dropowner->Stats->HP = dropowner->Stats->MaxHP;
                       }
                        break;
                       case 17:
                       {
                            dropowner->Stats->MP += (UseList.Index[thisdrop->item.itemnum]->useeffect[1]);
                            if(dropowner->Stats->MP > dropowner->Stats->MaxMP )
                               dropowner->Stats->MP = dropowner->Stats->MaxMP;
                       }
                        break;
                       case 76:
                       {
                            dropowner->CharInfo->stamina += (UseList.Index[thisdrop->item.itemnum]->useeffect[1]);
                            if(dropowner->CharInfo->stamina > 5000)
                               dropowner->CharInfo->stamina = 5000;
                       }
                        break;
                       case 92:
                       {
                            if(dropowner->Clan->clanid>0)
                            {
                                //dropowner->Clan->CP += (UseList.Index[thisdrop->item.itemnum]->useeffect[1]);
                                UINT points=UseList.Index[thisdrop->item.itemnum]->useeffect[1];
                                dropowner->Clan->CP=GServer->getClanPoints(dropowner->Clan->clanid);
                                dropowner->Clan->CP+=points;

                                char buffer[200];
                                sprintf( buffer, "You received %i Clan Points !!", points);
                                BEGINPACKET ( pak, 0x702 );
                                ADDSTRING( pak, buffer );
                                ADDBYTE( pak, 0 );
                                dropowner->client->SendPacket( &pak );

                                RESETPACKET( pak, 0x7e0 );
                                ADDBYTE    ( pak, 0xfe );
                                //ADDWORD    ( pak, dropowner->CharInfo->charid);  //charid
                                ADDDWORD    ( pak, dropowner->CharInfo->charid);  //charid
                                ADDDWORD    ( pak, points);  //Clan points (to be added)
                                cryptPacket( (char*)&pak, GServer->cct );
                                send( csock, (char*)&pak, pak.Size, 0 );

                                RESETPACKET( pak, 0x7e0 );
                                ADDBYTE    ( pak, 0x35 );
                                ADDWORD    ( pak, dropowner->clientid );
                                ADDWORD    ( pak, dropowner->Clan->clanid );
                                ADDWORD    ( pak, 0x0000 );
                                ADDWORD    ( pak, dropowner->Clan->back );
                                ADDWORD    ( pak, dropowner->Clan->logo );
                                ADDBYTE    ( pak, dropowner->Clan->grade );
                                ADDBYTE    ( pak, dropowner->Clan->clanrank);
                                ADDSTRING  ( pak, dropowner->Clan->clanname );
                                ADDBYTE    ( pak, 0x00 );
                                dropowner->client->SendPacket(&pak);
                            }
                            else
                            {
                                //Log( MSG_WARNING, "Player %s collected clanpoints but is not in a clan",dropowner->CharInfo->charname);
                            }

                       }
                        break;
                       default:
                       {
                            //Log( MSG_WARNING, "Unknown autoconsume type: %i %i",(UseList.Index[thisdrop->item.itemnum]->usecondition[0]),(UseList.Index[thisdrop->item.itemnum]->usecondition[1]));
                        }
                            break;
                }

            }
            else
            {
                unsigned int tempslot = dropowner->AddItem( thisdrop->item );
                if(tempslot!=0xffff)// we have slot
                {
                    unsigned int slot1 = tempslot;
                    unsigned int slot2 = 0xffff;
                    if(tempslot>MAX_INVENTORY)
                    {
                        slot1 = tempslot/1000;
                        slot2 = tempslot%1000;
                    }
                    ADDBYTE    ( pak, 0x00 );
                    ADDBYTE    ( pak, slot1 );
                    ADDBYTE    ( pak, 0x00 );
					pak = AddItemData(dropowner->items[slot1], pak);
                    //ADDDWORD   ( pak, BuildItemHead( dropowner->items[slot1] ) );
                    //ADDDWORD   ( pak, BuildItemData( dropowner->items[slot1] ) );
                    //ADDDWORD( pak, 0x00000000 );
                    //ADDWORD ( pak, 0x0000 );

                    dropowner->client->SendPacket( &pak );
                    dropowner->UpdateInventory( slot1, slot2 );
                    flag = true;
                }
                else
                {
                    ADDBYTE    (pak, 0x03);
                    dropowner->client->SendPacket(&pak);
                }

            }

        }
    }
	else
	{
		ADDBYTE    (pak, 0x02);
		thisclient->client->SendPacket(&pak);
    }

	if( flag )
	{
        if( thisclient->Party->party!=NULL )
        {
            unsigned int dropparty = thisclient->Party->party->Option/0x10;
            if( dropparty == 8 )
            {
                BEGINPACKET( pak, 0x7d3 );
                ADDWORD    ( pak, dropowner->clientid );
                if( thisdrop->type == 1 )
                {
                    ADDDWORD( pak, 0xccccccdf );
        		    ADDDWORD( pak, thisdrop->amount );
                    ADDDWORD( pak, 0xcccccccc );
                    ADDWORD ( pak, 0xcccc );
                }
                else
                if( thisdrop->type == 2 )
                {
                    pak = AddItemData(thisdrop->item, pak);

                }
                thisclient->Party->party->SendToMembers( &pak, dropowner );
            }
        }

		CMap* map = MapList.Index[thisdrop->posMap];
		//Log(MSG_INFO,"Drop picked in map %u",thisdrop->posMap);
		pthread_mutex_lock( &map->DropMutex );
		map->DeleteDrop( thisdrop );
		pthread_mutex_unlock( &map->DropMutex );
    }
	return true;
}

// Changes equipment
bool CWorldServer::pakChangeEquip( CPlayer* thisclient, CPacket* P )
{
    if(thisclient->Shop->open)
        return true;
	WORD srcslot = GETWORD((*P), 0);
	WORD destslot = GETWORD((*P), 2);

	//Log(MSG_INFO,"Trying to move item %i::%i (slot %i) to slot %i (where is %i::%i)",thisclient->items[srcslot].itemtype,thisclient->items[srcslot].itemnum,srcslot,destslot,thisclient->items[destslot].itemtype,thisclient->items[destslot].itemnum);
	if(!CheckInventorySlot(thisclient, srcslot))
	   return false;
	if(!CheckInventorySlot(thisclient, destslot))
	   return false;

    //LMA: it seems naRose sends the swap weirdly sometimes...
    if(thisclient->items[srcslot].itemtype == 0 || thisclient->items[srcslot].itemnum == 0)
    {
        //Log(MSG_WARNING,"Problem when trying to change slot %i to %i (item not there anymore?)",srcslot,destslot);
        int tempslot = 0;
        tempslot = destslot;
        destslot = srcslot;
        srcslot = tempslot;
        if(thisclient->items[srcslot].itemtype==0||thisclient->items[srcslot].itemnum==0)
        {
            //Log(MSG_WARNING,"Problem (2) when trying to change slot %i to %i (item not there anymore?)",srcslot,destslot);
            return true;
        }

    }

    //Log(MSG_INFO,"pakChangeEquip %i->%i before check",thisclient->CharInfo->charname,srcslot,destslot);

    //LMA: When trying to equip, we check some stats.
    if(destslot > 0 && destslot < 12 && !thisclient->CheckStats(srcslot, destslot))
    {
        //Log(MSG_WARNING,"PakChangeEquip:: Equip refused (see ahead)",thisclient->CharInfo->charname);
        return true;
    }

    //Log(MSG_INFO,"pakChangeEquip:: %s after check",thisclient->CharInfo->charname);


    if (destslot < 13 && GServer->EquipList[thisclient->items[srcslot].itemtype].Index[thisclient->items[srcslot].itemnum]->level > thisclient->Stats->Level)
       return true;
	if( destslot == 0 ) destslot = thisclient->GetNewItemSlot( thisclient->items[srcslot] );
	if( destslot == 0xffff ) return true;

    if( srcslot == 7 || destslot == 7 )//Weapon
    {
        // clear the buffs if we change the weapon
        for(UINT i=0;i<15;i++)
        {
            if(thisclient->MagicStatus[i].Buff != 0)
            {
                switch(thisclient->MagicStatus[i].Buff)
                {
                    case A_ATTACK:
                    case A_ACCUR:
                    case A_HASTE:
                    case A_CRITICAL:
                    case A_STEALTH:
                    case A_CLOAKING:
                    case A_ADDDMG:
                    {
                        thisclient->MagicStatus[i].Duration = 0;
                        //Log(MSG_INFO, "Buff %i Magic Status %i should be down after weapon remove", thisclient->MagicStatus[i].Buff,thisclient->MagicStatus[i].Status);
                    }
                    break;
                    /*default:
                    {
                        Log(MSG_INFO, "Buff %i Magic Status %i have to stay after weapon remove", thisclient->MagicStatus[i].Buff,thisclient->MagicStatus[i].Status);
                    }*/
                }
            }
        }
    }
    if( srcslot==8 || destslot==8 )//Shield
    {
        // clear the buffs if we change the Shield
        for(UINT i=0;i<15;i++)
        {
            if(thisclient->MagicStatus[i].Buff != 0)
            {
                switch(thisclient->MagicStatus[i].Buff)
                {
                    case A_DEFENSE:
                    {
                        thisclient->MagicStatus[i].Duration = 0;
                        //Log(MSG_INFO, "Buff %i Magic Status %i should be down after Shield remove", thisclient->MagicStatus[i].Buff,thisclient->MagicStatus[i].Status);
                    }
                    break;
                    /*default:
                    {
                        Log(MSG_INFO, "Buff %i Magic Status %i have to stay after Shield remove", thisclient->MagicStatus[i].Buff,thisclient->MagicStatus[i].Status);
                    }*/
                }
            }
        }
    }

	CItem tmpitm = thisclient->items[srcslot];
	thisclient->items[srcslot] = thisclient->items[destslot];
	thisclient->items[destslot] = tmpitm;
	thisclient->UpdateInventory( srcslot, destslot );

    //LMA: narose sends change a new way sometimes...
    if (destslot<10)
    {
        BEGINPACKET( pak, 0x7a5 );
        ADDWORD    ( pak, thisclient->clientid );
        ADDWORD    ( pak, destslot );
        ADDWORD    ( pak, thisclient->items[destslot].itemnum );
        ADDWORD    ( pak, BuildItemRefine( thisclient->items[destslot] ) );
        ADDWORD    ( pak, thisclient->Stats->Move_Speed );
        SendToVisible( &pak, thisclient );
    }

    if( srcslot==7 || destslot==7 )
    {
        // if is two hand weapon, we have to check if have shield and unequip it
        UINT weapontype = EquipList[WEAPON].Index[thisclient->items[7].itemnum]->type;
        switch(weapontype)
        {
            case TWO_HAND_SWORD:
            case SPEAR:
            case TWO_HAND_AXE:
            case BOW:
            case GUN:
            case LAUNCHER:
            case STAFF:
            case KATAR:
            case DOUBLE_SWORD:
            case DUAL_GUN:
            {
                if(thisclient->items[8].itemnum!=0)
                { // we should unequip the shield
                    UINT newslot = thisclient->GetNewItemSlot( thisclient->items[8] );
                    if(newslot==0xffff)
                    {
                        // we have no slot for the shield, we have to change back the items
                        tmpitm = thisclient->items[destslot];
                        thisclient->items[destslot] = thisclient->items[srcslot];
                        thisclient->items[srcslot] = tmpitm;
                        SendSysMsg( thisclient, "You don't have free slot" );
                        return true;
                    }
                    thisclient->items[newslot] = thisclient->items[8];
                    ClearItem( thisclient->items[8] );
                    thisclient->UpdateInventory( newslot, 8 );
                    BEGINPACKET( pak, 0x7a5 );
                    ADDWORD    ( pak, thisclient->clientid );
                    ADDWORD    ( pak, 8 );
                    ADDWORD    ( pak, thisclient->items[8].itemnum );
                    ADDWORD    ( pak, BuildItemRefine( thisclient->items[8] ) );
                    ADDWORD    ( pak, thisclient->Stats->Move_Speed );
                    SendToVisible( &pak, thisclient );
                }
            }
            break;
        }
    }
	//PY: Moved 7a5 packet down here so that it is sent AFTER calculating stats
    thisclient->SetStats( );

	BEGINPACKET	( pak, 0x7a5 );
	ADDWORD		( pak, thisclient->clientid );
	ADDWORD		( pak, srcslot );
	ADDWORD		( pak, thisclient->items[srcslot].itemnum );
	ADDWORD    ( pak, BuildItemRefine( thisclient->items[srcslot] ) );		//PY: This seems wrong. The client calls itemnum then the following 3 Shorts
	//ADDWORD		( pak, thisclient->items[srcslot].stats );		//stats and gem are the same thing
	//ADDWORD		( pak, thisclient->items[srcslot].socketed);
	//ADDWORD		( pak, thisclient->items[srcslot].refine );
	//Now movespeed
	ADDWORD		( pak, thisclient->Stats->Move_Speed );
	SendToVisible( &pak, thisclient );
	return true;
}

// Construct a cart/change parts
bool CWorldServer::pakChangeCart( CPlayer* thisclient, CPacket* P )
{
    if(thisclient->Shop->open)
        return true;

	WORD cartslot = GETWORD((*P), 0);
	WORD srcslot = GETWORD((*P), 0) + 135;
	WORD destslot = GETWORD((*P), 2);

    //LMA: no part change if already riding, except for weapons?
    //2do: check if parts are compatible? To avoid cart frame with CG part?
    if(thisclient->Status->Stance==DRIVING&&destslot!=139&&srcslot!=139)
        return true;

	if(!CheckInventorySlot( thisclient, srcslot ))
	   return false;
	if(!CheckInventorySlot( thisclient, destslot ))
	   return false;
	if( destslot == 0 ) destslot=thisclient->GetNewItemSlot( thisclient->items[srcslot] );
	if( destslot == 0xffff ) return true;

	//LMA: we need to check if all items have the same PatType...
    //LMA: test only if equip.
	if (destslot>=135&&destslot<140&&thisclient->items[srcslot].itemnum>0)
	{
        int good_type=0;
        for (int k=135;k<140;k++)
        {
            if(thisclient->items[k].itemnum==0)
                continue;
            if(good_type==0)
            {
                good_type=PatList.Index[thisclient->items[k].itemnum]->parttype;
            }
            else
            {
                if(good_type!=PatList.Index[thisclient->items[k].itemnum]->parttype)
                {
                    Log(MSG_WARNING,"%s has mixed type pieces on his cart...",thisclient->CharInfo->charname);
                    return true;
                }

            }

        }

        if(good_type>0&&(good_type!=PatList.Index[thisclient->items[srcslot].itemnum]->parttype))
        {
            GServer->SendPM(thisclient,"You can't use mixed part");
            Log(MSG_WARNING,"%s tries to use mix pieces for his cart...",thisclient->CharInfo->charname);
            return true;
        }

	}

	CItem tmpitm = thisclient->items[srcslot];
	thisclient->items[srcslot] = thisclient->items[destslot];
	thisclient->items[destslot] = tmpitm;

    //LMA: Getting good mspeed value for packet.
    thisclient->UpdateInventory( srcslot, destslot );
    unsigned int lma_speed = thisclient->GetCartSpeed();
    thisclient->Stats->Move_Speed = thisclient->GetMoveSpeed( );

	BEGINPACKET( pak, 0x7ca );
	ADDWORD    ( pak, thisclient->clientid );
	ADDWORD    ( pak, cartslot);
	ADDWORD    ( pak, thisclient->items[srcslot].itemnum);
	//ADDWORD    ( pak, BuildItemRefine( thisclient->items[srcslot] ) );
	//ADDWORD    ( pak, thisclient->Stats->Move_Speed );

	//ADDWORD    ( pak, tmpitm.itemnum);
	//ADDWORD    ( pak, BuildItemRefine( tmpitm ) );

	//LMA TEST:
	//ADDWORD    ( pak, lma_speed );
	ADDWORD    ( pak,0);

	//LMA: change?
	if(thisclient->Status->Stance == DRIVING)
	{
	    ADDWORD    ( pak, thisclient->Stats->Move_Speed );
	    //ADDWORD    ( pak, thisclient->Stats->Base_Speed );

        //LMA: Used to refresh the fuel and attack_fuel
        thisclient->TakeFuel();
	}

	SendToVisible( &pak, thisclient );

    //thisclient->UpdateInventory( srcslot, destslot );
	//thisclient->Stats->Move_Speed = thisclient->GetMoveSpeed( );
	thisclient->SetStats( );
	return true;
}

// Start attacking a monster or player
bool CWorldServer::pakStartAttack( CPlayer* thisclient, CPacket* P )
{
    if(!thisclient->CanAttack( )) return true;

    //LMA: Do we have fuel?
    if(thisclient->Status->Stance==DRIVING)
    {
        if(thisclient->items[136].itemnum==0||thisclient->items[139].itemnum==0)
        {
            Log(MSG_WARNING,"%s wants to attack but without motor %i or weapon %i ?",thisclient->CharInfo->charname,thisclient->items[136].itemnum,thisclient->items[139].itemnum);
            return true;
        }

        if(thisclient->items[136].lifespan==0)
        {
            //No fuel anymore, can't attack.
            return true;
        }

    }

    WORD clientid = GETWORD((*P),0x00);
    if ( thisclient->Battle->target == clientid )
    {
        Log(MSG_WARNING,"%s:: Already targeting CID %u",thisclient->CharInfo->charname,clientid);
        return true;
    }

    int weapontype = EquipList[WEAPON].Index[thisclient->items[7].itemnum]->type;
    if( weapontype == BOW && thisclient->items[132].count<1 )
        return true;
    else
    if( weapontype == GUN && thisclient->items[133].count<1 )
        return true;
    else
    if( weapontype == LAUNCHER && thisclient->items[134].count<1 )
        return true;
    else
    if( weapontype == CROSSBOW && thisclient->items[132].count<1 )
        return true;

    CMap* map = MapList.Index[thisclient->Position->Map];
    CCharacter* character = map->GetCharInMap( clientid );
    if(character==NULL) return true;
    if(character->IsMonster( ))
    {
        if(map->allowpvp==0 && character->IsSummon( )) return true;
        if(!character->IsSummon())
            character->AddDamage( (CCharacter*)thisclient, 0 );
    }
	if(character->IsMonster())
	{
	   // SET MONSTER HEALTH
        //LMA: Little check, for now we "only" have a DWORD for monster's HP so there is a limit
        //broken by some monsters (Turak boss)
        if(character->Stats->HP>MAXHPMOB)
        {
            character->Stats->HP=(long long) MAXHPMOB;
        }

	   BEGINPACKET( pak, 0x79f );
	   ADDWORD    ( pak, character->clientid );
	   ADDDWORD   ( pak, character->Stats->HP );
	   thisclient->client->SendPacket( &pak );
    }
    thisclient->StartAction( character, sBASIC_ACTION, 0 );
    thisclient->Battle->contatk = true;


	return true;
}

//LMA-Shin: PVP Packet? In fact sent to tell to stop...
bool CWorldServer::pakPvp796( CPlayer* thisclient, CPacket* P )
{
   BEGINPACKET( pak, 0x796 );
   ADDWORD    ( pak, thisclient->clientid );
   ADDWORD    ( pak, thisclient->Battle->target);
   ADDWORD    ( pak, thisclient->GetMoveSpeed() );
   ADDFLOAT   ( pak, thisclient->Position->current.x*100 );
   ADDFLOAT   ( pak, thisclient->Position->current.y*100 );
   SendToVisible( &pak, thisclient );

	return true;
}

//Drakia's telegate hacker fix - needs testing.  Comment above and uncomment below for test
// Goto next map through gate
bool CWorldServer::pakGate( CPlayer* thisclient, CPacket* P )
{
    thisclient->Session->inGame = false;
    word GateID = GETWORD((*P), 0x00);

	//LMA: new way.
    bool is_hack=true;
    fPoint position;
    UINT map = 0;
    CTeleGate* thisgate=NULL;
    CTeleGate* lastgate=NULL;

	for (int k=0;k<6;k++)
	{
	    thisgate = GetTeleGateByID( GateID,k);

	    if (thisgate==NULL)
	    {
	        if(k==0)
	        {
                Log( MSG_HACK, "Player %s[Map: %i X: %f Y: %f] - Gate Hacking[ID: %i], Gates doesn't exist!",
                         thisclient->CharInfo->charname, thisclient->Position->Map,
                         thisclient->Position->current.x, thisclient->Position->current.y, GateID);
	        }
	        else
	        {
	            //Hack.
                Log( MSG_HACK, "Player %s[Map: %i X: %f Y: %f] - Gate Hacking[ID: %i, offset %i], distance %.2f",
                             thisclient->CharInfo->charname, thisclient->Position->Map,
                             thisclient->Position->current.x, thisclient->Position->current.y, GateID,lastgate->offset,distance(thisclient->Position->current, lastgate->src) );
	        }

            return true;
	    }

	    if(thisclient->Position->Map != thisgate->srcMap ||distance(thisclient->Position->current, thisgate->src) > 100)
	    {
            //hack or wrong gate?
            lastgate=thisgate;
            continue;
	    }

	    //good gate :)
	    is_hack=false;
	    Log(MSG_INFO,"Gate %i found, offset %i",thisgate->id,k);
	    break;
	}
	//LMA: end new way.

	if(is_hack)
	{
        Log( MSG_HACK, "Player %s[Map: %i X: %f Y: %f] - Gate Hacking[ID: %i, offset %i], distance %.2f",
                     thisclient->CharInfo->charname, thisclient->Position->Map,
                     thisclient->Position->current.x, thisclient->Position->current.y, GateID,lastgate->offset,distance(thisclient->Position->current, lastgate->src) );
	    return true;
	}

	map = thisgate->destMap;
	position = thisgate->dest;
    MapList.Index[map]->TeleportPlayer( thisclient, position );


	return true;
}


// Set Motion (animation)
bool CWorldServer::pakDoEmote( CPlayer* thisclient, CPacket* P )
{
    if(thisclient->Shop->open)
        return true;
    thisclient->RestartPlayerVal( );
	BEGINPACKET( pak, 0x781 );
	ADDWORD( pak, GETWORD((*P), 0) );		//motion number
	ADDWORD( pak, 0x8000 );					//bitfield data. bit 16 is used to stop motion if set to 1. the rest is unused entirely so why the 8?
	ADDWORD( pak, thisclient->clientid );
	SendToVisible( &pak, thisclient );
	return true;
}

// This function is for increasing your stats
bool CWorldServer::pakAddStats( CPlayer* thisclient, CPacket* P )
{
	BYTE statid = GETBYTE((*P), 0);
	int statval = -1;
	int nstatval = 0;
	switch(statid)
    {
		case 0: nstatval = (int)floor((float)thisclient->Attr->Str / 5); break;
		case 1: nstatval = (int)floor((float)thisclient->Attr->Dex / 5); break;
		case 2: nstatval = (int)floor((float)thisclient->Attr->Int / 5); break;
		case 3: nstatval = (int)floor((float)thisclient->Attr->Con / 5); break;
		case 4: nstatval = (int)floor((float)thisclient->Attr->Cha / 5); break;
		case 5: nstatval = (int)floor((float)thisclient->Attr->Sen / 5); break;
	}
	if (thisclient->CharInfo->StatPoints >= nstatval && nstatval < ((Config.MaxStat / 5) + 1))
    {
		switch(statid)
        {
			case 0: statval = ++thisclient->Attr->Str; break;
			case 1: statval = ++thisclient->Attr->Dex; break;
			case 2: statval = ++thisclient->Attr->Int; break;
			case 3: statval = ++thisclient->Attr->Con; break;
			case 4: statval = ++thisclient->Attr->Cha; break;
			case 5: statval = ++thisclient->Attr->Sen; break;
		}
	}
	if (statval > 0)
    {
		thisclient->CharInfo->StatPoints -= nstatval;
		BEGINPACKET( pak, 0x7a9 );
		ADDBYTE( pak, statid );
		ADDWORD( pak, (unsigned short)statval );
		thisclient->client->SendPacket( &pak );
	}
	thisclient->SetStats( );
	return true;
}

// Normal Chat
bool CWorldServer::pakNormalChat( CPlayer* thisclient, CPacket* P )
{
    //LMA: to avoid chat spamming (1 second between each reset and 5 chat per second max).
    time_t etime=time(NULL);
    if (etime<thisclient->next_chat)
    {
        if (!thisclient->spam_chat)
        {
            thisclient->nb_chats++;
        }

        //A player can chat 5 times a second... Should be enough :)
        if (thisclient->nb_chats>5)
        {
            //Only one hack message...
            if (!thisclient->spam_chat)
            {
                Log(MSG_HACK,"Possible chat hack by player %s",thisclient->CharInfo->charname);
            }

            thisclient->spam_chat=true;
        }

        return true;
    }
    else
    {
        thisclient->nb_chats=0;
        thisclient->next_chat=time(NULL)+1;   //Next second check.
        thisclient->spam_chat=false;
    }

	if (P->Buffer[0]=='/')
    {
		return pakGMCommand( thisclient, P );
        //std::cout << thisclient->CharInfo->charname << ": " << P->Buffer << "\n";
    }
    else
    {
		BEGINPACKET( pak, 0x783 );
		ADDWORD( pak, thisclient->clientid );
		ADDSTRING( pak, P->Buffer );
		ADDBYTE( pak, 0 );
		SendToVisible( &pak, thisclient );
	}


	return true;
};

// Exit rose
bool CWorldServer::pakExit( CPlayer* thisclient, CPacket* P )
{
	BEGINPACKET( pak, 0x707 );
	ADDWORD( pak, 0 );
	thisclient->client->SendPacket( &pak );
	thisclient->client->isActive = false;
	return true;
}

// move skill to function keys
bool CWorldServer::pakMoveSkill ( CPlayer* thisclient, CPacket* P )
{
	BYTE slotid = GETBYTE((*P), 0x0 );
	WORD itemid = GETWORD((*P), 0x1 );
	if(slotid>47) return true;
	thisclient->quickbar[slotid] = itemid;
	BEGINPACKET( pak, 0x7aa );
	ADDBYTE( pak, slotid );
	ADDWORD( pak, itemid );
	thisclient->client->SendPacket( &pak );
	return true;
}

// User is checking up on the server
bool CWorldServer::pakPing ( CPlayer* thisclient, CPacket* P )
{
	BEGINPACKET( pak, 0x0700 );
	ADDWORD( pak, 0 );
	thisclient->client->SendPacket( &pak );
	return true;
}

// GameGuard
bool CWorldServer::pakGameGuard ( CPlayer* thisclient, CPacket* P )
{
	thisclient->lastGG = clock();
	return true;
}

// LMA: Bonus Exp Time coupon (Using Drose fix)
bool CWorldServer::pakExpTC ( CPlayer* thisclient, CPacket* P )
{
    unsigned int action = GETBYTE((*P),0);
    //TEST
    time_t rawtime;
    struct tm * timeinfo;

    /*if (thisclient->timerxp==0||thisclient->wait_validation==0)
    {
       Log(MSG_HACK,"Bonus XP packet received from %s (shouldn't be).",thisclient->CharInfo->charname);
       return true;
    }
    if (thisclient->timerddrop==0||thisclient->wait_validation_ddrop==0)
    {
       Log(MSG_HACK,"Medal of Fortune packet received from %s (shouldn't be).",thisclient->CharInfo->charname);
       return true;
    }
    if (thisclient->timerstatdrop==0||thisclient->wait_validation_statdrop==0)
    {
       Log(MSG_HACK,"Medal of Excellence packet received from %s (shouldn't be).",thisclient->CharInfo->charname);
       return true;
    }
    if (thisclient->timergraydrop==0||thisclient->wait_validation_graydrop==0)
    {
       Log(MSG_HACK,"Medal of Retrieval packet received from %s (shouldn't be).",thisclient->CharInfo->charname);
       return true;
    }*/
    /*//validating Xp bonus :)
    thisclient->bonusxp=thisclient->wait_validation;
    rawtime=thisclient->timerxp;
    //time ( &rawtime );
    timeinfo = localtime ( &rawtime );
    //LMA: Don't use any "Log" function after this since it'll change the timeinfo's value.*/

    switch(action)
    {
        case 0x04: //Medal
        {
             if (thisclient->timerxp==0||thisclient->wait_validation==0)
             {
                Log(MSG_HACK,"Bonus XP packet received from %s (shouldn't be).",thisclient->CharInfo->charname);
                return true;
             }
             //validating Xp bonus :)
             thisclient->bonusxp = thisclient->wait_validation;
             rawtime=thisclient->timerxp;
             //time ( &rawtime );
             timeinfo = localtime ( &rawtime );
             //LMA: Don't use any "Log" function after this since it'll change the timeinfo's value.
             BEGINPACKET( pak, 0x822 );
             ADDBYTE( pak, 0x04 );
             ADDBYTE( pak, 0x00 );
             ADDWORD( pak, 0x0000 );
             ADDWORD(pak,timeinfo->tm_year+1900);   //Year
             ADDBYTE( pak, timeinfo->tm_mon+1);  //Month
             ADDBYTE( pak, timeinfo->tm_mday);  //Day
             ADDBYTE( pak, timeinfo->tm_hour+1);  //hour
             //ADDBYTE( pak, timeinfo->tm_min+1); //minutes?
             ADDBYTE( pak, 0x00);  //minutes?

			 thisclient->client->SendPacket( &pak );
             return true;
        }
        break;
        case 0x05: //Medal of Fortune
        {
			if (thisclient->timerddrop==0||thisclient->wait_validation_ddrop==0)
			{
					Log(MSG_HACK,"Medal of Fortune packet received from %s (shouldn't be).",thisclient->CharInfo->charname);
					return true;
			}
			//validating Medal of Fortune bonus :)
            thisclient->bonusddrop=thisclient->wait_validation_ddrop;
            rawtime=thisclient->timerddrop;
            timeinfo = localtime ( &rawtime );
            BEGINPACKET( pak, 0x822 );
            ADDBYTE( pak, 0x05 );
            ADDBYTE( pak, 0x00 );
            ADDWORD( pak, 0x0000 );
            ADDWORD(pak,timeinfo->tm_year+1900);   //Year
            ADDBYTE( pak, timeinfo->tm_mon+1);  //Month
            ADDBYTE( pak, timeinfo->tm_mday);  //Day
            ADDBYTE( pak, timeinfo->tm_hour+1);  //hour
            //ADDBYTE( pak, timeinfo->tm_min+1); //minutes?
            ADDBYTE( pak, 0x00);  //minutes?
            thisclient->client->SendPacket( &pak );
            return true;
        }
        break;
        case 0x06: //Medal of Excellence
        {
			if (thisclient->timerstatdrop==0||thisclient->wait_validation_statdrop==0)
			{
				Log(MSG_HACK,"Medal of Excellence packet received from %s (shouldn't be).",thisclient->CharInfo->charname);
				return true;
			}
			//validating Medal of Excellence bonus :)
            thisclient->bonusstatdrop=thisclient->wait_validation_statdrop;
            rawtime=thisclient->timerstatdrop;
			timeinfo = localtime ( &rawtime );
            BEGINPACKET( pak, 0x822 );
            ADDBYTE( pak, 0x06 );
            ADDBYTE( pak, 0x00 );
            ADDWORD( pak, 0x0000 );
            ADDWORD(pak,timeinfo->tm_year+1900);   //Year
            ADDBYTE( pak, timeinfo->tm_mon+1);  //Month
            ADDBYTE( pak, timeinfo->tm_mday);  //Day
            ADDBYTE( pak, timeinfo->tm_hour+1);  //hour
            //ADDBYTE( pak, timeinfo->tm_min+1); //minutes?
            ADDBYTE( pak, 0x00);  //minutes?
            thisclient->client->SendPacket( &pak );
            return true;
        }
        break;
        case 0x07: //Medal of Retrieval
        {
			if (thisclient->timergraydrop==0||thisclient->wait_validation_graydrop==0)
			{
				Log(MSG_HACK,"Medal of Retrieval packet received from %s (shouldn't be).",thisclient->CharInfo->charname);
				return true;
			}
			//validating Medal of Retrieval bonus :)
            thisclient->bonusgraydrop=thisclient->wait_validation_graydrop;
            rawtime=thisclient->timergraydrop;
			time ( &rawtime );
            timeinfo = localtime ( &rawtime );

            BEGINPACKET( pak, 0x822 );
            ADDBYTE( pak, 0x07 );
            ADDBYTE( pak, 0x00 );
            ADDWORD( pak, 0x0000 );
            ADDWORD(pak,timeinfo->tm_year+1900);   //Year
            ADDBYTE( pak, timeinfo->tm_mon+1);  //Month
            ADDBYTE( pak, timeinfo->tm_mday);  //Day
            ADDBYTE( pak, timeinfo->tm_hour+1);  //hour
            //ADDBYTE( pak, timeinfo->tm_min+1); //minutes?
            ADDBYTE( pak, 0x00);  //minutes?
			thisclient->client->SendPacket( &pak );
            return true;
        }
        break;
        default:
                Log(MSG_WARNING,"TimeCoupon - unknown action: %i", action );
    }


	return true;
}

// Changing Respawn for a client
bool CWorldServer::pakChangeRespawn( CPlayer* thisclient, CPacket* P )
{
        //player position (map)
       unsigned int spot = 0;
        switch(thisclient->Position->Map)
        {
            case 22:  //AP
            {
                spot = 1;
                break;
            }
            case 1:  //Zant
            {
                spot = 2;
                break;
            }
            case 2:  //Junon
            {
                spot = 4;
                break;
            }
            case 51:  //Eucar
            {
                spot = 5;
                break;
            }
            case 61:  //Xita
            {
                spot = 68;
                break;
            }
        }

                thisclient->Position->saved = spot;
    return true;
}

// User Died
bool CWorldServer::pakUserDied ( CPlayer* thisclient, CPacket* P )
{
    thisclient->Session->inGame = false;
    BYTE respawn = GETBYTE((*P),0);
    //1 - Current / 2 - save point
    CMap* map = MapList.Index[thisclient->Position->Map];
    CRespawnPoint* thisrespawn = NULL;
    CRespawnPoint dungeonSpawn;

    if(respawn==1)
    {
        thisrespawn = map->GetNearRespawn( thisclient );
    }
    else
    {
        //LMA: special case for dungeons... STB[4]==1
        thisrespawn = &dungeonSpawn;
        thisrespawn->destMap=map->id;
        thisrespawn->id=1;
        thisrespawn->dest.z=0;
        switch(map->id)
        {
            case 31: //Goblin Cave B1
            {
                thisrespawn->dest.x=5516;
                thisrespawn->dest.y=5437;
            }
            break;
            case 32: //Goblin Cave B2
            {
                thisrespawn->dest.x=5435;
                thisrespawn->dest.y=5259;
            }
            break;
            case 33: //Goblin Cave B3
            {
                thisrespawn->dest.x=5605;
                thisrespawn->dest.y=5488;
            }
            break;
            case 40: //Grand Ballroom
            {
                thisrespawn->dest.x=5184;
                thisrespawn->dest.y=5211;
            }
            break;
            case 56: //Forgotten temple B1
            {
                thisrespawn->dest.x=5035;
                thisrespawn->dest.y=5200;
            }
            break;
            case 57: //Forgotten temple B1
            {
                thisrespawn->dest.x=5540;
                thisrespawn->dest.y=5145;
            }
            break;
            case 65: //Prison
            {
                thisrespawn->dest.x=5485;
                thisrespawn->dest.y=5304;
            }
            break;

            default:
            {
                thisrespawn = GetRespawnByID( thisclient->Position->saved );
            }
            break;
        }

    }

    if(thisclient->Stats->HP<=0||(thisclient->Stats->HP < (thisclient->Stats->MaxHP * 10 / 100)))
    {
        thisclient->Stats->HP = thisclient->Stats->MaxHP * 10 / 100;
        Log(MSG_INFO,"Player %s died, we give him %I64i Hp",thisclient->CharInfo->charname,thisclient->Stats->HP);
    }
    else
    {
        Log(MSG_INFO,"Player %s died, he has %I64i Hp",thisclient->CharInfo->charname,thisclient->Stats->HP);
    }

    //LMA: cleaning buffs.
    //and resetting stats.
    for(int j = 0; j < 30; j++)
    {
        thisclient->MagicStatus[j].Duration = 0;
        thisclient->MagicStatus[j].BuffTime = 0;
    }

    thisclient->RefreshBuff();
    thisclient->SetStats( );

    if(thisrespawn!=NULL)
    {
        // geo edit for saved town warp // 29 sep 07
        map = MapList.Index[thisrespawn->destMap];
        map->TeleportPlayer( thisclient, thisrespawn->dest, false );
    }
    else
    {
        Log(MSG_WARNING,"Player died, respawn not found for map %i, respawn %i",map->id,respawn);
        fPoint coord;
        coord.x = 5200;
        coord.y = 5200;
        MapList.Index[2]->TeleportPlayer( thisclient, coord, false );
    }

    /*
	for(unsigned int i=0;i<30;i++)
	{	// Clean Buffs
        thisclient->MagicStatus[i].Duration = 0;
    }
    */


	return true;
}

// Shouting
bool CWorldServer::pakShout ( CPlayer* thisclient, CPacket* P )
{
    //LMA: to avoid shout spamming (20 seconds between each shout).
    time_t etime=time(NULL);
    if (etime<thisclient->next_shout)
    {
        //Only one hack message...
        if (!thisclient->spam_shout)
        {
            Log(MSG_HACK,"Possible shout hack by player %s",thisclient->CharInfo->charname);
            thisclient->spam_shout=true;
        }

        return true;
    }

    thisclient->next_shout=time(NULL)+19;   //Can shout at this time.
    thisclient->spam_shout=false;

	BEGINPACKET(pak, 0x0785);
	ADDSTRING  ( pak, thisclient->CharInfo->charname );
	ADDBYTE    ( pak, 0 );
	ADDSTRING  ( pak, &P->Buffer[0] );
	ADDBYTE    ( pak, 0 );
	SendToMap  ( &pak, thisclient->Position->Map );

	return true;
}

// Whispering
bool CWorldServer::pakWhisper ( CPlayer* thisclient, CPacket* P )
{
    //LMA: to avoid whisper spamming (1 second between each reset and 5 whisper per second max).
    time_t etime=time(NULL);
    if (etime<thisclient->next_whisper)
    {
        if (!thisclient->spam_whisper)
        {
            thisclient->nb_whispers++;
        }

        //A player can whisper 5 times a second... Should be enough :)
        if (thisclient->nb_whispers>5)
        {
            //Only one hack message...
            if (!thisclient->spam_whisper)
            {
                Log(MSG_HACK,"Possible whisper hack by player %s",thisclient->CharInfo->charname);
            }

            thisclient->spam_whisper=true;
        }

        return true;
    }
    else
    {
        thisclient->nb_whispers=0;
        thisclient->next_whisper=time(NULL)+1;   //Next second check.
        thisclient->spam_whisper=false;
    }

	char msgto[17];
	memset( &msgto, '\0', 17 );
	strncpy( msgto, (char*)&(*P).Buffer[0],16 );

	//LMA: Case insensitive search
	//CPlayer* otherclient = GetClientByCharName( msgto );
	CPlayer* otherclient = GetClientByCharNameCI( msgto );
	if(otherclient!=NULL)
	{
	   BEGINPACKET( pak, 0x0784 );
	   ADDSTRING( pak, thisclient->CharInfo->charname );
	   ADDBYTE( pak, 0 );
	   ADDSTRING( pak, &P->Buffer[strlen(msgto)+1] );
	   ADDBYTE( pak, 0 );
	   otherclient->client->SendPacket( &pak );
	   return true;
	}

	BEGINPACKET( pak, 0x0784 );
	ADDSTRING( pak, msgto );
	ADDBYTE( pak, 0 );
	ADDBYTE( pak, 0 );
	thisclient->client->SendPacket( &pak );
	return true;
}


// LMA: Union Chat.
bool CWorldServer::pakChatUnion ( CPlayer* thisclient, CPacket* P )
{
    if (thisclient->CharInfo->unionid<=0||thisclient->CharInfo->unionid==2||thisclient->CharInfo->unionid>5)
    {
        return true;
    }

    int union_back=0;
    //union_back=thisclient->CharInfo->unionid+8;

    //LMA: to avoid chat spamming (1 second between each reset and 5 chat per second max).
    time_t etime=time(NULL);
    if (etime<thisclient->next_chat_union)
    {
        if (!thisclient->spam_chat_union)
        {
            thisclient->nb_chat_union++;
        }

        //A player can chat 5 times a second... Should be enough :)
        if (thisclient->nb_chat_union>5)
        {
            //Only one hack message...
            if (!thisclient->spam_chat_union)
            {
                Log(MSG_HACK,"Possible chat union hack by player %s",thisclient->CharInfo->charname);
            }

            thisclient->spam_chat_union=true;
        }

        return true;
    }
    else
    {
        thisclient->nb_chat_union=0;
        thisclient->next_chat_union=time(NULL)+1;   //Next second check.
        thisclient->spam_chat_union=false;
    }

    //LMA: seems to be working with team ID.
    union_back=thisclient->pvp_id;

	BEGINPACKET(pak, 0x0789);
	ADDDWORD    ( pak, union_back );
	ADDSTRING  ( pak, thisclient->CharInfo->charname );
	ADDBYTE    ( pak, 0 );
	ADDSTRING  ( pak, &P->Buffer[0] );
	ADDBYTE    ( pak, 0 );
	SendToMap   (&pak, thisclient->Position->Map);
	//SendToUnionInMap  ( &pak, thisclient->Position->Map,thisclient->CharInfo->unionid);


	return true;
}

// PY: This packet structure doesn't exist in 137 client. We don't have a trade channel. Besides 0x07ed is used for something else
// LMA: Trade Chat.
/*
bool CWorldServer::pakChatTrade ( CPlayer* thisclient, CPacket* P )
{
    //LMA: to avoid chat trade spamming (1 second between each reset and 5 chat per second max).
    time_t etime=time(NULL);
    if (etime<thisclient->next_chat_trade)
    {
        if (!thisclient->spam_chat_trade)
        {
            thisclient->nb_chat_trade++;
        }

        //A player can chat 5 times a second... Should be enough :)
        if (thisclient->nb_chat_trade>5)
        {
            //Only one hack message...
            if (!thisclient->spam_chat_trade)
            {
                Log(MSG_HACK,"Possible chat trade hack by player %s",thisclient->CharInfo->charname);
            }

            thisclient->spam_chat_trade=true;
        }

        return true;
    }
    else
    {
        thisclient->nb_chat_trade=0;
        thisclient->next_chat_trade=time(NULL)+1;   //Next second check.
        thisclient->spam_chat_trade=false;
    }

	BEGINPACKET(pak, 0x07ed);
	ADDSTRING  ( pak, thisclient->CharInfo->charname );
	ADDBYTE    ( pak, 0 );
	ADDSTRING  ( pak, &P->Buffer[0] );
	ADDBYTE    ( pak, 0 );
	SendToMap  ( &pak, thisclient->Position->Map );


	return true;
}
*/

// Return to Char Select
bool CWorldServer::pakCharSelect ( CPlayer* thisclient, CPacket* P )
{
   	thisclient->savedata();
  	if(!thisclient->Session->inGame) return true;
	//Maxxon: Party crash when exit fix.
	OnClientDisconnect(thisclient->client);
    thisclient->Session->isLoggedIn = false;
    if(thisclient->client!=NULL) thisclient->client->isActive = false;
    //send packet to change messenger status (offline)
   	BEGINPACKET( pak, 0x7e1 );
   	ADDBYTE    ( pak, 0xfa );
   	ADDWORD    ( pak, thisclient->CharInfo->charid );
   	ADDBYTE    ( pak, 0x00 );
   	cryptPacket( (char*)&pak, cct );
   	send( csock, (char*)&pak, pak.Size, 0 );
   	pakClearUser( thisclient );
   	ClearClientID( thisclient->clientid );
    thisclient->Saved = true;
	RESETPACKET( pak, 0x505 );
	ADDDWORD( pak, thisclient->Session->userid );
	cryptPacket( (char*)&pak, cct );
	send( csock, (char*)&pak, pak.Size, 0 );
	return true;
}

// Buy from NPC (usual and Clan Shop).
//2do: Count and check Z and reward points BEFORE giving the items...
bool CWorldServer::pakNPCBuy ( CPlayer* thisclient, CPacket* P )
{
    if(thisclient->Shop->open)
        return true;
    WORD NPC = GETWORD((*P), 0);
	CNPC* thisnpc = GetNPCByID( NPC, thisclient->Position->Map );
	if( thisnpc==NULL )
	   return true;

	BYTE buycount = GETBYTE((*P), 2);
	BYTE sellcount = GETBYTE((*P), 3);

	//LMA: temp_price
	long int temp_price_li;
	long long temp_price_ll;

	//LMA: % of rebate.
	float pc_reb=(100-(float)thisclient->pc_rebate)/100;
	if (pc_reb<=0)
	{
	    pc_reb=1;
	}

     //Clan Shop case...
     //Log(MSG_INFO,"Buying /selling from NPC %i",thisnpc->npctype);
     bool is_clanshop=false;
     bool is_union=false;
     UINT nb_union_points=0;
     if (thisnpc->npctype==1752)
     {
        is_clanshop=true;

        //impossible to buy from clan shop if you don't have a clan
        if (thisclient->Clan->clanid==0)
        {
            return true;
        }

     }

     //LMA: Union Shops.
     //Some maps are entirely dedicated to Union.
     if(thisclient->Position->Map>=15&&thisclient->Position->Map<=18)
     {
         is_union=true;
     }

     if(thisnpc->npctype>=1109&&thisnpc->npctype<=1112)
     {
         is_union=true;
     }

    if (is_union)
    {
         switch (thisclient->CharInfo->unionid)
         {
             case 1:
             {
                 nb_union_points=thisclient->CharInfo->union01;
             }
             break;
            case 2:
             {
                 nb_union_points=thisclient->CharInfo->union02;
             }
             break;
             case 3:
             {
                 nb_union_points=thisclient->CharInfo->union03;
             }
             break;
             case 4:
             {
                 nb_union_points=thisclient->CharInfo->union04;
             }
             break;
             case 5:
             {
                 nb_union_points=thisclient->CharInfo->union05;
             }
             break;
             default:
             {
                 //Can't be.
                 Log(MSG_WARNING,"%s tried to shop in Union shop but isn't in an union (%i)",thisclient->CharInfo->charname,thisclient->CharInfo->unionid);
                 return true;
             }
             break;
         }

    }

	BYTE ncount = 0;
	BEGINPACKET( pak, 0x717 );
	ADDQWORD   ( pak, thisclient->CharInfo->Zulies );
	ADDBYTE    ( pak, 0x00 );
	for (int i=0; i<buycount; i++)
    {
		BYTE tabid = GETBYTE((*P), 8+(i*4));
		BYTE itemid = GETBYTE((*P), 9+(i*4));
		WORD count = GETWORD((*P), 10+(i*4));
		unsigned sellid = 0;
		switch(tabid)
		{
            case 0:
                sellid = thisnpc->thisnpc->tab1;
            break;
            case 1:
                sellid = thisnpc->thisnpc->tab2;
            break;
            case 2:
                sellid = thisnpc->thisnpc->tab3;
            break;
            case 3:
                sellid = thisnpc->thisnpc->specialtab;
            break;
            default:
                sellid = 0;
        }
        if(SellList.Index[sellid]==NULL) // invalid tab
            continue;
		int tmpcount = count;
        while (tmpcount>0)
        {
            CItem thisitem;
            //LMA: Shitty old alg :(
            /*thisitem.itemnum = SellList.Index[sellid]->item[itemid] % 1000;
            thisitem.itemtype = SellList.Index[sellid]->item[itemid] / 1000;*/
            thisitem.itemnum = gi(SellList.Index[sellid]->item[itemid],1);
            thisitem.itemtype = gi(SellList.Index[sellid]->item[itemid],0);
            if (thisitem.itemtype==0||thisitem.itemnum==0)
            {
                Log(MSG_WARNING,"Problem with selling list, NPC %i, tabid %i, sellid %i, itemid %i",thisnpc->npctype,tabid,sellid,itemid);
                GServer->SendPM(thisclient,"Sorry, there is a problem with this NPC, warn admin");
                return true;
            }


            //LMA: to avoid mem bug.
            thisitem.count=tmpcount;
            unsigned newslot = thisclient->GetNewItemSlot( thisitem );
			if ( newslot == 0xffff ) { ncount-=1; break; }
			int thisslotcount = 999 - thisclient->items[newslot].count;
			if ( thisslotcount > tmpcount ) thisslotcount=tmpcount;
			tmpcount -= thisslotcount;
			thisitem.count = thisslotcount + thisclient->items[newslot].count;
			thisitem.refine = 0;
			thisitem.durability = GServer->STB_ITEM[thisitem.itemtype-1].rows[thisitem.itemnum][29];
			thisitem.lifespan = 100;
			thisitem.appraised = true;
			thisitem.socketed = false;
			thisitem.stats = 0;
			thisitem.gem = 0;
			thisitem.sp_value=0;

			if(thisitem.itemtype==14)
			{
			    thisitem.sp_value=thisitem.lifespan*10;
			}

			thisitem.last_sp_value=0;

			//checking money / reward points now...
    		switch(thisitem.itemtype)
    		{
                case 1:
                case 2:
                case 3:
                case 4:
                case 5:
                case 6:
                case 8:
                case 9:
                case 14:
                {
                    float price = 0;
                    UINT quality = 0;
                    UINT bprice = 0;
                    if(thisitem.itemtype<10 )
                    {
                        quality = EquipList[thisitem.itemtype].Index[thisitem.itemnum]->quality;
                        bprice = EquipList[thisitem.itemtype].Index[thisitem.itemnum]->price;
                    }
                    else
                    {
                        quality = PatList.Index[thisitem.itemnum]->quality;
                        bprice = PatList.Index[thisitem.itemnum]->price;
                    }
                    price = quality;
                    price += 0x32; // town rate
                    price *= bprice;
                    price += 0.5;
                    price /= 100;

                    //LMA: price tweak.
                    if(!is_clanshop&&!is_union)
                    {
                        //price = (float)round(price/1.25);
                        price = (float)round(price*pc_reb);
                    }
                    else
                    {
                        price = (float)round(price);
                    }

                    if (is_clanshop)
                    {
                        if(thisitem.itemtype<10 )
                        {
                            price=EquipList[thisitem.itemtype].Index[thisitem.itemnum]->craft_difficult;
                        }
                        else
                        {
                            price=PatList.Index[thisitem.itemnum]->craft_difficult;
                        }

                        if (thisclient->CharInfo->rewardpoints<(long int) price)
                        {
                          Log(MSG_HACK, "Not enough reward points player %s, have %u, need %u",thisclient->CharInfo->charname,thisclient->CharInfo->rewardpoints,(long int) price);
                          return true;
                        }

                        temp_price_li=thisclient->CharInfo->rewardpoints;
                        temp_price_li -= (long int) price;

                        if(temp_price_li>thisclient->CharInfo->rewardpoints)
                        {
                            Log(MSG_HACK, "negative reward point hack detected for player %s, have %u, need %u",thisclient->CharInfo->charname,thisclient->CharInfo->rewardpoints,(long int) price);
                            return true;
                        }

                        thisclient->CharInfo->rewardpoints = temp_price_li;
                    }
                    else if (is_union)
                    {
                        if(thisitem.itemtype<10 )
                        {
                            price=EquipList[thisitem.itemtype].Index[thisitem.itemnum]->craft_difficult;
                        }
                        else
                        {
                            price=PatList.Index[thisitem.itemnum]->craft_difficult;
                        }

                        if (nb_union_points<(long int) price)
                        {
                          Log(MSG_HACK, "Not enough Union points player %s, have %u, need %u",thisclient->CharInfo->charname,nb_union_points,(long int) price);
                          return true;
                        }

                        temp_price_li=nb_union_points;
                        temp_price_li -= (long int) price;

                        if(temp_price_li>nb_union_points)
                        {
                            Log(MSG_HACK, "negative union point hack detected for player %s, have %u, need %u",thisclient->CharInfo->charname,nb_union_points,(long int) price);
                            return true;
                        }

                        nb_union_points = temp_price_li;
                    }
                    else
                    {
                        if (thisclient->CharInfo->Zulies<(__int64)price)
                        {
                          Log(MSG_HACK, "Not enough Zuly player %s, have %li, need %li",thisclient->CharInfo->charname,thisclient->CharInfo->Zulies,(long int) price);
                          return true;
                        }

                        temp_price_ll=thisclient->CharInfo->Zulies;
                        temp_price_ll -= (__int64) price;

                        if(temp_price_ll>thisclient->CharInfo->Zulies)
                        {
                            Log(MSG_HACK, "negative zuly hack detected for player %s, have %u, need %u",thisclient->CharInfo->charname,thisclient->CharInfo->Zulies,(long int) price);
                            return true;
                        }

                        thisclient->CharInfo->Zulies = temp_price_ll;
                    }

                    Log( MSG_INFO, "%s:: Item bought: itemnum %i, itemtype %i, itemcount %i, price %0.0f",thisclient->CharInfo->charname,thisitem.itemnum, thisitem.itemtype, thisitem.count, price);
                }
                break;
                case 10:
                case 12:
                {
                    // this values should be the same than packet 753
                    BYTE values[11] = {0x32, 0x32, 0x32, 0x32, 0x32, 0x32, 0x32, 0x32, 0x32, 0x32, 0x32};
                    UINT type = 0;
                    UINT bprice = 0;
                    UINT pricerate = 0;
                    UINT pvalue = 0;
                    if(thisitem.itemtype==10)
                    {
                        type = UseList.Index[thisitem.itemnum]->type;
                        bprice = UseList.Index[thisitem.itemnum]->price;
                        pricerate = UseList.Index[thisitem.itemnum]->pricerate;
                        pvalue = UseList.Index[thisitem.itemnum]->pricevalue;
                    }
                    else
                    {
                        type = NaturalList.Index[thisitem.itemnum]->type;
                        bprice = NaturalList.Index[thisitem.itemnum]->price;
                        pricerate = NaturalList.Index[thisitem.itemnum]->pricerate;
                        pvalue = NaturalList.Index[thisitem.itemnum]->pricevalue;
                    }
                    unsigned int value = 0;
                    float price = 0;
                    bool flag;
                    if(type<421)
                        if(type<311)
                            flag = false;
                        else
                            if(type>312)
                                flag = false;
                            else
                                flag = true;
                    else
                        if(type<=428)
                            flag = true;
                        else
                            if(type<311)
                                flag = false;
                            else
                                flag = true;
                    if(flag)
                    {
                        value = pvalue;
                        if(value>=11)
                            value ^= 0xffffffff;
                        else
                            value = values[value];
                        value -= 0x32;
                        value *= pricerate;
                        price = value * 0.001;
                        price += 1;
                        price *= bprice;
                        price += 0.5;

                        //LMA: price tweak.
                        if(!is_clanshop&&!is_union)
                        {
                            //price = (float)floor(price/1.25);
                            price = (float)floor(price*pc_reb);
                        }
                        else
                        {
                            price = (float)floor(price);
                        }

                        if (is_clanshop)
                        {
                            if(thisitem.itemtype==10)
                            {
                                price = UseList.Index[thisitem.itemnum]->craft_difficult;
                            }
                            else
                            {
                                price = NaturalList.Index[thisitem.itemnum]->craft_difficult;
                            }

                            if (thisclient->CharInfo->rewardpoints<(long int) price*count)
                            {
                              Log(MSG_HACK, "Not enough reward points player %s, have %u, need %u",thisclient->CharInfo->charname,thisclient->CharInfo->rewardpoints,(long int) price*count);
                              return true;
                            }

                            temp_price_li=thisclient->CharInfo->rewardpoints;
                            temp_price_li -= (long int) price*count;

                            if(temp_price_li>thisclient->CharInfo->rewardpoints)
                            {
                                Log(MSG_HACK, "negative reward point hack detected for player %s, have %u, need %u",thisclient->CharInfo->charname,thisclient->CharInfo->rewardpoints,(long int) price*count);
                                return true;
                            }

                            thisclient->CharInfo->rewardpoints = temp_price_li;
                        }
                        else if (is_union)
                        {
                            if(thisitem.itemtype==10)
                            {
                                price = UseList.Index[thisitem.itemnum]->craft_difficult;
                            }
                            else
                            {
                                price = NaturalList.Index[thisitem.itemnum]->craft_difficult;
                            }

                            if (nb_union_points<(long int) price*count)
                            {
                              Log(MSG_HACK, "Not enough Union points player %s, have %u, need %u",thisclient->CharInfo->charname,nb_union_points,(long int) price*count);
                              return true;
                            }

                            temp_price_li=nb_union_points;
                            temp_price_li -= (long int) price*count;

                            if(temp_price_li>nb_union_points)
                            {
                                Log(MSG_HACK, "negative union point hack detected for player %s, have %u, need %u",thisclient->CharInfo->charname,nb_union_points,(long int) price*count);
                                return true;
                            }

                            nb_union_points = temp_price_li;
                        }
                        else
                        {
                            if (thisclient->CharInfo->Zulies<(__int64)price*count)
                            {
                              Log(MSG_HACK, "Not enough Zuly player %s, have %li, need %li",thisclient->CharInfo->charname,thisclient->CharInfo->Zulies,(long int)price*count);
                              return true;
                            }

                            temp_price_ll=thisclient->CharInfo->Zulies;
                            temp_price_ll -= (long int) price*count;

                            if(temp_price_ll>thisclient->CharInfo->Zulies)
                            {
                                Log(MSG_HACK, "negative Zuly hack detected for player %s, have %u, need %u",thisclient->CharInfo->charname,thisclient->CharInfo->Zulies,(long int) price*count);
                                return true;
                            }

                            thisclient->CharInfo->Zulies = temp_price_ll;
                        }

                        Log( MSG_INFO, "%s:: Item bought: itemnum %i, itemtype %i, itemcount %i, price %0.0f",thisclient->CharInfo->charname,thisitem.itemnum, thisitem.itemtype, thisitem.count, price);
                    }
                    else
                    {
                        float price = pricerate;
                        unsigned int value = 0x61 - 0x32; // misc rate - 0x32
                        price *= value;
                        price *= 0.001;
                        price += 1;
                        price *= bprice;
                        price += 0.5;
                        //LMA: price tweak.

                        if(!is_clanshop&&!is_union)
                        {
                            //price = (float)floor(price/1.25);
                            price = (float)floor(price*pc_reb);
                        }
                        else
                        {
                            price = (float)floor(price);
                        }

                        if (is_clanshop)
                        {
                            if(thisitem.itemtype==10)
                            {
                                price = UseList.Index[thisitem.itemnum]->craft_difficult;
                            }
                            else
                            {
                                price = NaturalList.Index[thisitem.itemnum]->craft_difficult;
                            }

                            if (thisclient->CharInfo->rewardpoints<(long int) price*count)
                            {
                              Log(MSG_HACK, "Not enough reward points player %s, have %u, need %u",thisclient->CharInfo->charname,thisclient->CharInfo->rewardpoints,(long int) price*count);
                              return true;
                            }

                            temp_price_li=thisclient->CharInfo->rewardpoints;
                            temp_price_li -= (long int) price*count;

                            if(temp_price_li>thisclient->CharInfo->rewardpoints)
                            {
                                Log(MSG_HACK, "negative reward point hack detected for player %s, have %u, need %u",thisclient->CharInfo->charname,thisclient->CharInfo->rewardpoints,(long int) price*count);
                                return true;
                            }

                            thisclient->CharInfo->rewardpoints = temp_price_li;
                        }
                        else if (is_union)
                        {
                            if(thisitem.itemtype==10)
                            {
                                price = UseList.Index[thisitem.itemnum]->craft_difficult;
                            }
                            else
                            {
                                price = NaturalList.Index[thisitem.itemnum]->craft_difficult;
                            }

                            if (nb_union_points<(long int) price*count)
                            {
                              Log(MSG_HACK, "Not enough Union points player %s, have %u, need %u",thisclient->CharInfo->charname,nb_union_points,(long int) price*count);
                              return true;
                            }

                            temp_price_li=nb_union_points;
                            temp_price_li -= (long int) price*count;

                            if(temp_price_li>nb_union_points)
                            {
                                Log(MSG_HACK, "negative union point hack detected for player %s, have %u, need %u",thisclient->CharInfo->charname,nb_union_points,(long int) price*count);
                                return true;
                            }

                            nb_union_points = temp_price_li;
                        }
                        else
                        {
                            if (thisclient->CharInfo->Zulies<(long int)price*count)
                            {
                              Log(MSG_HACK, "Not enough Zuly player %s, have %li, need %li",thisclient->CharInfo->charname,thisclient->CharInfo->Zulies,(long int)price*count);
                              return true;
                            }

                            temp_price_ll=thisclient->CharInfo->Zulies;
                            temp_price_ll -= (long int) price*count;

                            if(temp_price_ll>thisclient->CharInfo->Zulies)
                            {
                                Log(MSG_HACK, "negative Zuly hack detected for player %s, have %u, need %u",thisclient->CharInfo->charname,thisclient->CharInfo->Zulies,(long int) price*count);
                                return true;
                            }

                            thisclient->CharInfo->Zulies = temp_price_ll;
                        }

                        Log( MSG_INFO, "%s:: Item bought: itemnum %i, itemtype %i, itemcount %i, price %0.0f",thisclient->CharInfo->charname,thisitem.itemnum, thisitem.itemtype, thisitem.count, price);
                    }
                }
                break;
                case 7:
                case 11:
                case 13:
                {
                    float price = 0;
                    UINT bprice = 0;
                    UINT pricerate;
                    if(thisitem.itemtype==7)
                    {
                        pricerate = EquipList[7].Index[thisitem.itemnum]->pricerate;
                        bprice = EquipList[7].Index[thisitem.itemnum]->price;
                    }
                    else
                    {
                        switch(thisitem.itemtype)
                        {
                            case 11:
                                pricerate = JemList.Index[thisitem.itemnum]->pricerate;
                                bprice = JemList.Index[thisitem.itemnum]->price;
                            break;
                            case 13:continue;
                        }
                    }
                    price = pricerate;
                    unsigned int value = 0x61 - 0x32; // misc rate - 0x32
                    price *= value;
                    price *= 0.001;
                    price += 1;
                    price *= bprice;
                    price += 0.5;

                    //LMA: price tweak.
                    if(!is_clanshop&&!is_union)
                    {
                        //price = (float)round(price/1.25);
                        price = (float)round(price*pc_reb);
                    }
                    else
                    {
                        price = (float)round(price);
                    }

                    if (is_clanshop)
                    {
                        if(thisitem.itemtype==7)
                        {
                            price = EquipList[7].Index[thisitem.itemnum]->craft_difficult;
                        }
                        else if(thisitem.itemtype==11)
                        {
                            price = JemList.Index[thisitem.itemnum]->craft_difficult;
                        }

                        if (thisclient->CharInfo->rewardpoints<(long int) price*count)
                        {
                          Log(MSG_HACK, "Not enough reward points player %s, have %u, need %u",thisclient->CharInfo->charname,thisclient->CharInfo->rewardpoints,(long int) price*count);
                          return true;
                        }

                        temp_price_li=thisclient->CharInfo->rewardpoints;
                        temp_price_li -= (long int) price*count;

                        if(temp_price_li>thisclient->CharInfo->rewardpoints)
                        {
                            Log(MSG_HACK, "negative reward point hack detected for player %s, have %u, need %u",thisclient->CharInfo->charname,thisclient->CharInfo->rewardpoints,(long int) price*count);
                            return true;
                        }

                        thisclient->CharInfo->rewardpoints = temp_price_li;
                    }
                    else if (is_union)
                    {
                        if(thisitem.itemtype==7)
                        {
                            price = EquipList[7].Index[thisitem.itemnum]->craft_difficult;
                        }
                        else if(thisitem.itemtype==11)
                        {
                            price = JemList.Index[thisitem.itemnum]->craft_difficult;
                        }

                        if (nb_union_points<(long int) price*count)
                        {
                          Log(MSG_HACK, "Not enough Union points player %s, have %u, need %u",thisclient->CharInfo->charname,nb_union_points,(long int) price*count);
                          return true;
                        }

                        temp_price_li=nb_union_points;
                        temp_price_li -= (long int) price*count;

                        if(temp_price_li>nb_union_points)
                        {
                            Log(MSG_HACK, "negative union point hack detected for player %s, have %u, need %u",thisclient->CharInfo->charname,nb_union_points,(long int) price*count);
                            return true;
                        }

                        nb_union_points = temp_price_li;
                    }
                    else
                    {
                        if (thisclient->CharInfo->Zulies<(long int)price*count)
                        {
                          Log(MSG_HACK, "Not enough Zuly player %s, have %li, need %li",thisclient->CharInfo->charname,thisclient->CharInfo->Zulies,(long int)price*count);
                          return true;
                        }

                        temp_price_ll=thisclient->CharInfo->Zulies;
                        temp_price_ll -= (long int) price*count;

                        if(temp_price_ll>thisclient->CharInfo->Zulies)
                        {
                            Log(MSG_HACK, "negative zuly hack detected for player %s, have %u, need %u",thisclient->CharInfo->charname,thisclient->CharInfo->Zulies,(long int) price*count);
                            return true;
                        }

                        thisclient->CharInfo->Zulies = temp_price_ll;
                    }

                    Log( MSG_INFO, "%s:: Item bought: itemnum %i, itemtype %i, itemcount %i, price %0.0f",thisclient->CharInfo->charname,thisitem.itemnum, thisitem.itemtype, thisitem.count, price);
                }
                break;
                default:
                    Log( MSG_WARNING, "Invalid Item Type: %i", thisitem.itemtype );
            }

            //We add item at the end when checks done.
			thisclient->items[newslot] = thisitem;

			//Saving item in database now.
			thisclient->saveinventory();

			ADDBYTE  ( pak, newslot );
			pak = AddItemData(thisclient->items[newslot], pak);
			ncount++;
		}
	}

	//refresh Reward points from player if needed
	if (is_clanshop&&buycount>0)
	{
       GServer->pakGMClanRewardPoints(thisclient,thisclient->CharInfo->charname,0);
    }

    //LMA: refresh Union Points
    if(is_union&&buycount>0)
    {
         switch (thisclient->CharInfo->unionid)
         {
             case 1:
             {
                 thisclient->CharInfo->union01=nb_union_points;
             }
             break;
            case 2:
             {
                 thisclient->CharInfo->union02=nb_union_points;
             }
             break;
             case 3:
             {
                 thisclient->CharInfo->union03=nb_union_points;
             }
             break;
             case 4:
             {
                 thisclient->CharInfo->union04=nb_union_points;
             }
             break;
             case 5:
             {
                 thisclient->CharInfo->union05=nb_union_points;
             }
             break;
             default:
             {
                 //Can't be.
                 Log(MSG_WARNING,"%s tried to shop in Union shop but isn't in an union (%i)",thisclient->CharInfo->charname,thisclient->CharInfo->unionid);
                 return true;
             }
             break;
         }

        int new_offset=80+thisclient->CharInfo->unionid;
        BEGINPACKET( pak, 0x721 );
        ADDWORD( pak, new_offset );
        ADDWORD( pak, nb_union_points );
        ADDWORD( pak, 0x0000 );
        thisclient->client->SendPacket( &pak );
        RESETPACKET( pak, 0x730 );
        ADDWORD    ( pak, 0x0005 );
        ADDDWORD   ( pak, 0x40b3a24d );
        thisclient->client->SendPacket( &pak );
    }

	for (int i=0; i<sellcount; i++)
    {
		BYTE slotid = GETBYTE((*P), 8+(buycount*4)+(i*3));
		WORD count = GETWORD((*P), 9+(buycount*4)+(i*3));
        if (thisclient->items[slotid].count < count)
           return true;
		CItem thisitem = thisclient->items[slotid];
		thisitem.count = count;

		if (thisclient->items[slotid].count<thisitem.count)
		{
          Log(MSG_HACK, "[NPC-SELL] Player %s tryes to sell %i [%i:%i], but has only %i",thisclient->CharInfo->charname,thisitem.count,thisitem.itemtype,thisitem.itemnum,thisclient->items[slotid].count);
          return true;
        }

		switch(thisitem.itemtype)
		{
            case 1:
            case 2:
            case 3:
            case 4:
            case 5:
            case 6:
            case 8:
            case 9:
            case 14:
            {
                float price = 0;
                price = 7.142857E-05F * 5000;
                if(thisitem.itemtype<10)
                {
                    price *= EquipList[thisitem.itemtype].Index[thisitem.itemnum]->price;
                }
                else
                {
                    price *= PatList.Index[thisitem.itemnum]->price;
                }
                price *= thisitem.durability + 0xc8;
                price *= 40;
                price *= 0xc8 - 0x62; //town rate
                price *= 1.000000E-06;
                price = (float)floor(price);
    			Log( MSG_INFO, "%s:: Item Sold: itemnum %i, itemtype %i, itemcount %i, price %0.0f",thisclient->CharInfo->charname,thisitem.itemnum, thisitem.itemtype, thisitem.count, price);
        		thisclient->CharInfo->Zulies += (long int)price*count;
            }
            break;
            case 10:
            case 12:
            {
                // this values are the same from packet 753
                BYTE values[11] = {0x32, 0x32, 0x32, 0x32, 0x32, 0x32, 0x32, 0x32, 0x32, 0x32, 0x32};
                UINT type = 0;
                UINT bprice = 0;
                UINT pricerate = 0;
                UINT pvalue = 0;
                if(thisitem.itemtype==10)
                {
                   type = UseList.Index[thisitem.itemnum]->type;
                   bprice = UseList.Index[thisitem.itemnum]->price;
                   pricerate = UseList.Index[thisitem.itemnum]->pricerate;
                   pvalue = UseList.Index[thisitem.itemnum]->pricevalue;
                }
                else
                {
                   type = NaturalList.Index[thisitem.itemnum]->type;
                   bprice = NaturalList.Index[thisitem.itemnum]->price;
                   pricerate = NaturalList.Index[thisitem.itemnum]->pricerate;
                   pvalue = NaturalList.Index[thisitem.itemnum]->pricevalue;
                }
                unsigned int value = 0;
                float price = 0;
                bool flag;
                if(type<421)
                {
                    if(type<311)
                        flag = false;
                    else
                        if(type>312)
                            flag = false;
                        else
                            flag = true;
                }
                else
                {
                    if(type<=428)
                        flag = true;
                    else
                        if(type<311)
                            flag = false;
                        else
                            flag = true;
                }
                if(flag)
                {
                    value = pvalue;
                    if(value>=11)
                        value ^= 0xffffffff;
                    else
                        value = values[value];
                    value -= 0x32;
                    value *= pricerate;
                    value += 1000;
                    value *= bprice;
                    value *= (200 - 0x62); //town rate ( 100)
                    price = value * 5.555555555555556E-06;
                    price = (float)floor(price);
                	Log( MSG_INFO, "%s:: Item Sold: itemnum %i, itemtype %i, itemcount %i, price %0.0f",thisclient->CharInfo->charname,thisitem.itemnum, thisitem.itemtype, thisitem.count, price);
                    thisclient->CharInfo->Zulies += (long int)price*count;
                }
                else
                {
                    float price = pricerate;
                    unsigned int value = 0x61 - 0x32; // misc rate -0x32
                    price *= value;
                    price += 1000;
                    price *= bprice;
                    price *= (200 - 0x62); //town rate ( 100)
                    price *= 5.555555555555556E-06;
                    price = (float)floor(price);
                	Log( MSG_INFO, "%s:: Item Sold: itemnum %i, itemtype %i, itemcount %i, price %0.0f",thisclient->CharInfo->charname,thisitem.itemnum, thisitem.itemtype, thisitem.count, price);
                    thisclient->CharInfo->Zulies += (long int)price*count;
                }
            }
            break;
            case 7:
            case 11:
            case 13:
            {
                float price = 0;
                UINT bprice = 0;
                UINT pricerate = 0;
                switch(thisitem.itemtype)
                {
                    case 7:
                        pricerate = EquipList[thisitem.itemtype].Index[thisitem.itemnum]->pricerate;
                        bprice = EquipList[thisitem.itemtype].Index[thisitem.itemnum]->price;
                    break;
                    case 11:
                        pricerate = JemList.Index[thisitem.itemnum]->pricerate;
                        bprice = JemList.Index[thisitem.itemnum]->price;
                    break;
                    case 13:continue;//can we sell quest items? :S

                }
                price = pricerate;
                price *= 0x61 - 0x32;   // misc rate -0x32
                price += 1000;
                price *= bprice;
                price *= (200 - 0x62); //town rate ( 100)
                price *= 5.555555555555556E-06;
                price = (float)floor(price);
        		Log( MSG_INFO, "%s:: Item Sold: itemnum %i, itemtype %i, itemcount %i, price %0.0f",thisclient->CharInfo->charname,thisitem.itemnum, thisitem.itemtype, thisitem.count, price);
            	thisclient->CharInfo->Zulies += (long int)price*count;
            }
            break;
            default:
                Log( MSG_WARNING, "Invalid Item Type: %i", thisitem.itemtype );
        }
        thisclient->items[slotid].count	-= count;
        if( thisclient->items[slotid].count <=0 )
    		ClearItem( thisclient->items[slotid] );
		ADDBYTE( pak, slotid );
		pak = AddItemData(thisclient->items[slotid], pak);
		ncount++;
	    //Saving item in database now.
		thisclient->saveinventory();
	}

	SETQWORD( pak, 0, thisclient->CharInfo->Zulies );
	SETBYTE( pak, 8, ncount );
	thisclient->client->SendPacket( &pak );


	return true;
}

// Attack skill
bool CWorldServer::pakStartSkill ( CPlayer* thisclient, CPacket* P )
{
    if( thisclient->Shop->open || thisclient->Status->Stance==DRIVING || thisclient->Status->Mute !=0xff || !thisclient->Status->CanCastSkill)
        return true;
    fPoint thispoint;
    UINT targetid = GETWORD( (*P), 0 );
    UINT skillnum = GETWORD( (*P), 2 );

    if(skillnum>=MAX_ALL_SKILL)
    {
        Log( MSG_HACK, "Invalid Skill id %i for %s ", skillnum, thisclient->CharInfo->charname );
        return true;
    }

    Log( MSG_INFO, "pakStartSkill for %s (%i)", thisclient->CharInfo->charname,skillnum);

    //LMA: Some Basic skills (double scratch) has no level...
    unsigned int skillid = thisclient->cskills[skillnum].id+thisclient->cskills[skillnum].level-1;
    if(thisclient->cskills[skillnum].id!=0&&thisclient->cskills[skillnum].level==0)
    {
        if (thisclient->GoodSkill(thisclient->cskills[skillnum].id)!=2)
        {
            Log( MSG_HACK, "Invalid Skill id %i has no level and is not basic for %s ", skillnum, thisclient->CharInfo->charname );
            return true;
        }

        skillid = thisclient->cskills[skillnum].id;
    }

    CMap* map = MapList.Index[thisclient->Position->Map];
    CCharacter* character = map->GetCharInMap( targetid );
    if(character==NULL) return true;

	if(character->IsMonster())
	{
        if(character->Stats->HP>MAXHPMOB)
        {
            character->Stats->HP=(long long) MAXHPMOB;
        }

	   BEGINPACKET( pak, 0x79f );
	   ADDWORD( pak, character->clientid );
	   ADDDWORD   ( pak, character->Stats->HP );    //LMA: DDWORD :)
	   thisclient->client->SendPacket( &pak );
    }

	CSkills* thisskill = GetSkillByID( skillid );
    if(thisskill==NULL)
    {
        return true;
    }

    //We Check if The Skill need a Bow, Gun, Launcher or Crossbow and check the number of Arrow, Bullet or Canon the player have is < 1
    bool need_arrows=false;
    if(thisskill->weapon[0]==BOW || thisskill->weapon[0] == GUN || thisskill->weapon[0] == LAUNCHER || thisskill->weapon[0] == CROSSBOW)
    {
        need_arrows=true;
    }
    if(thisskill->weapon[1]==BOW || thisskill->weapon[1] == GUN || thisskill->weapon[1] == LAUNCHER || thisskill->weapon[1] == CROSSBOW)
    {
        need_arrows=true;
    }
    if(need_arrows)
    {
        int weapontype = EquipList[WEAPON].Index[thisclient->items[7].itemnum]->type;
        if (weapontype == BOW || weapontype == GUN || weapontype == LAUNCHER || weapontype == CROSSBOW)
        {
            if( weapontype == BOW && thisclient->items[132].count<1 )
                return true;
            else
            if( weapontype == GUN && thisclient->items[133].count<1 )
                return true;
            else
            if( weapontype == LAUNCHER && thisclient->items[134].count<1)
                return true;
            else
            if( weapontype == CROSSBOW && thisclient->items[132].count<1 )
                return true;
        }
        else
        {
            //Player not equipped, failure
            return true;
        }
    }

	if(thisskill->target==9 && !character->IsDead())
	{
        ClearBattle( thisclient->Battle );
        return true;
    }

    //LMA: time to check the cooldown time.
    time_t etime=time(NULL);
    if (etime<thisclient->cskills[skillnum].cooldown_skill)
    {
        Log(MSG_HACK,"Player %s tried to pakStartSkill skill %u but cooldown wasn't finished %u<%u",thisclient->CharInfo->charname,skillid,etime,thisclient->cskills[skillnum].cooldown_skill);
        return true;
    }

    //checking if player has enough MP too.
    //LMA: Special test, it seems sometimes the MP value is off after a level up??
    if(thisclient->Stats->MP>thisclient->Stats->MaxMP)
    {
        thisclient->Stats->MP=thisclient->Stats->MaxMP;
    }

    UINT needed_mp=(thisskill->mp - (thisskill->mp * thisclient->Stats->MPReduction / 100));
    if (thisclient->Stats->MP<needed_mp)
    {
        Log(MSG_HACK,"Player %s tried to pakStartSkill skill %u but didn't have enough MP %u<%u",thisclient->CharInfo->charname,skillid,thisclient->Stats->MP,needed_mp);
        return true;
    }

    //We set the time the player can cast this skill next time.
    thisclient->cskills[skillnum].cooldown_skill=etime+thisskill->cooldown;
    Log(MSG_WARNING,"Next pakStartSkill skill %u available at %u (is %u)",skillid,thisclient->cskills[skillnum].cooldown_skill,etime);

    if( isSkillTargetFriendly( thisskill ) )
    {
        Log(MSG_INFO,"Buff because friendly");
        thisclient->StartAction( character, SKILL_BUFF, skillid );
    }
    else
    {
        Log(MSG_INFO,"Attack because foe");
        thisclient->StartAction( character, SKILL_ATTACK, skillid );
    }
	return true;
}

// Trade action
bool CWorldServer::pakTradeAction ( CPlayer* thisclient, CPacket* P )
{
    if(thisclient->Shop->open)
        return true;
	CPacket pak;
	BYTE action = GETBYTE( (*P), 0 );
	thisclient->Trade->trade_target = GETWORD( (*P), 1 );
	CPlayer* otherclient = GetClientByID( thisclient->Trade->trade_target, thisclient->Position->Map );
	if (otherclient==NULL) return true;
	switch(action)
    {
		case 0:
			// REQUEST TRADE
			RESETPACKET( pak, 0x7c0 );
			ADDBYTE( pak, 0 );
			ADDWORD( pak, thisclient->clientid );
			ADDBYTE( pak, 0 );
			otherclient->client->SendPacket( &pak );
			thisclient->Trade->trade_status=2;
			otherclient->Trade->trade_status=1;
			break;
		case 1:
			// ACCEPT TRADE
			RESETPACKET( pak, 0x7c0 );
			ADDBYTE( pak, 1 );
			ADDWORD( pak, thisclient->clientid );
			ADDBYTE( pak, 0 );
			otherclient->client->SendPacket( &pak );
			thisclient->Trade->trade_status=3;
			otherclient->Trade->trade_status=3;
			for(int i=0; i<11; i++) thisclient->Trade->trade_count[i]=0;
			for(int i=0; i<10; i++) thisclient->Trade->trade_itemid[i]=0;
			for(int i=0; i<11; i++) otherclient->Trade->trade_count[i]=0;
			for(int i=0; i<10; i++) otherclient->Trade->trade_itemid[i]=0;
			break;
		case 3:
			RESETPACKET( pak, 0x7c0 );
			ADDBYTE( pak, 3 );
			ADDWORD( pak, thisclient->clientid );
			ADDBYTE( pak, 0 );
			otherclient->client->SendPacket( &pak );
			thisclient->Trade->trade_target=0;
			otherclient->Trade->trade_target=0;
			thisclient->Trade->trade_status=0;
			thisclient->Trade->trade_status=0;
			break;
		case 4:
			RESETPACKET( pak, 0x7c0 );
			ADDBYTE( pak, 4 );
			ADDWORD( pak, thisclient->clientid );
			ADDBYTE( pak, 0 );
			otherclient->client->SendPacket( &pak );
			thisclient->Trade->trade_status=4;
			break;
		case 6:
			if (thisclient->Trade->trade_status==6)
			{
			    thisclient->Trade->trade_status=4;
			}
			else
			{
			    thisclient->Trade->trade_status=6;
			}

			if (otherclient->Trade->trade_status==6)
            {
				RESETPACKET( pak, 0x7c0 );
				// Complete the trade
				ADDBYTE( pak, 6 );
				ADDWORD( pak, otherclient->clientid );
				ADDBYTE( pak, 0 );
				thisclient->client->SendPacket( &pak );
				RESETPACKET( pak, 0x7c0 );
				ADDBYTE( pak, 6 );
				ADDWORD( pak, thisclient->clientid );
				ADDBYTE( pak, 0 );
				otherclient->client->SendPacket( &pak );

				// Check if user has enough zuly
                if ( thisclient->CharInfo->Zulies < thisclient->Trade->trade_count[0x0a])
                {
                  Log(MSG_HACK, "[TRADE] Player %s has to trade %li, but has only %li",thisclient->CharInfo->charname,thisclient->Trade->trade_count[0x0a],thisclient->CharInfo->Zulies);
                  return true;
                }
                //LMA: bug... surely...
                //if ( thisclient->CharInfo->Zulies < otherclient->Trade->trade_count[0x0a]) return true;
                if ( otherclient->CharInfo->Zulies < otherclient->Trade->trade_count[0x0a])
                {
                  Log(MSG_HACK, "[TRADE] Player %s has to trade %li, but has only %li",otherclient->CharInfo->charname,otherclient->Trade->trade_count[0x0a],otherclient->CharInfo->Zulies);
                  return true;
                }

				// Update the zuly
				//LMA: anti hack...
				long long zulythis=0;
				long long zulyother=0;
				zulythis = thisclient->CharInfo->Zulies;
				zulyother = otherclient->CharInfo->Zulies;

				thisclient->CharInfo->Zulies -= thisclient->Trade->trade_count[0x0a];
				otherclient->CharInfo->Zulies -= otherclient->Trade->trade_count[0x0a];
				thisclient->CharInfo->Zulies += otherclient->Trade->trade_count[0x0a];
				otherclient->CharInfo->Zulies += thisclient->Trade->trade_count[0x0a];

				unsigned tucount = 0;
				unsigned oucount = 0;

				// Begin this clients inventory update
				BEGINPACKET( pakt, 0x717 );
				ADDQWORD( pakt, thisclient->CharInfo->Zulies );
				ADDBYTE( pakt, 0 );

				// Begin the other clients inventory update
				BEGINPACKET( pako, 0x717 );
				ADDQWORD( pako, otherclient->CharInfo->Zulies );
				ADDBYTE( pako, 0 );

                //LMA: check before actually giving the items to players...
  				for (unsigned i=0; i<10; i++)
                {
                    //LMA: Dupe test.
                    for (int ii=i+1;ii<10;ii++)
                    {
                        if(thisclient->Trade->trade_itemid[i]!=0&&(thisclient->Trade->trade_itemid[i]==thisclient->Trade->trade_itemid[ii]))
                        {
                          Log(MSG_HACK, "[TRADE] Player %s tried to dupe item in slot %i (%u::%u)",thisclient->CharInfo->charname,thisclient->Trade->trade_itemid[i],thisclient->items[thisclient->Trade->trade_itemid[i]].itemtype,thisclient->items[thisclient->Trade->trade_itemid[i]].itemnum);
                          thisclient->CharInfo->Zulies=zulythis;
                          otherclient->CharInfo->Zulies=zulyother;
                          return true;
                        }

                        if(otherclient->Trade->trade_itemid[i]!=0&&(otherclient->Trade->trade_itemid[i]==otherclient->Trade->trade_itemid[ii]))
                        {
                          Log(MSG_HACK, "[TRADE] Player %s tried to dupe item in slot %i (%u::%u)",otherclient->CharInfo->charname,otherclient->Trade->trade_itemid[i],otherclient->items[otherclient->Trade->trade_itemid[i]].itemtype,otherclient->items[otherclient->Trade->trade_itemid[i]].itemnum);
                          thisclient->CharInfo->Zulies=zulythis;
                          otherclient->CharInfo->Zulies=zulyother;
                          return true;
                        }

                    }

					if(thisclient->Trade->trade_count[i] > 0)
                    {
						//LMA: anti hack check.
						if(thisclient->items[thisclient->Trade->trade_itemid[i]].count < thisclient->Trade->trade_count[i])
						{
                          Log(MSG_HACK, "[TRADE] Player %s has to trade %i [%i:%i], but has only %i",thisclient->CharInfo->charname,thisclient->Trade->trade_count[i],thisclient->items[thisclient->Trade->trade_itemid[i]].itemtype,thisclient->items[thisclient->Trade->trade_itemid[i]].itemnum,thisclient->items[thisclient->Trade->trade_itemid[i]].count);
          				  thisclient->CharInfo->Zulies=zulythis;
        				  otherclient->CharInfo->Zulies=zulyother;
                          return true;
                        }

					}
					if(otherclient->Trade->trade_count[i] > 0)
                    {
						//LMA: anti hack check.
						if(otherclient->items[otherclient->Trade->trade_itemid[i]].count<otherclient->Trade->trade_count[i])
						{
                          Log(MSG_HACK, "[TRADE] Player %s has to trade %i [%i:%i], but has only %i",otherclient->CharInfo->charname,otherclient->Trade->trade_count[i],otherclient->items[otherclient->Trade->trade_itemid[i]].itemtype,otherclient->items[otherclient->Trade->trade_itemid[i]].itemnum,otherclient->items[otherclient->Trade->trade_itemid[i]].count);
          				  thisclient->CharInfo->Zulies=zulythis;
        				  otherclient->CharInfo->Zulies=zulyother;
                          return true;
                        }

					}
				}

                //Ok, go, it should be ok now...
				for (unsigned i=0; i<10; i++)
                {
					if(thisclient->Trade->trade_count[i] > 0)
                    {
						CItem thisitem = thisclient->items[thisclient->Trade->trade_itemid[i]];
						thisitem.sp_value = 0;
						thisitem.last_sp_value = 0;
						unsigned newslot = otherclient->GetNewItemSlot( thisitem );
						if(newslot==0xffff) continue;
						thisclient->items[thisclient->Trade->trade_itemid[i]].count -= thisclient->Trade->trade_count[i];
						if( thisclient->items[thisclient->Trade->trade_itemid[i]].count<=0)
      						ClearItem(thisclient->items[thisclient->Trade->trade_itemid[i]]);
                        if (otherclient->items[newslot].count > 0)
                            thisitem.count = otherclient->items[newslot].count + thisclient->Trade->trade_count[i];
                        else
                            thisitem.count = thisclient->Trade->trade_count[i];
                        otherclient->items[newslot] = thisitem;
						ADDBYTE( pakt, (unsigned char)thisclient->Trade->trade_itemid[i] );
						pakt = AddItemData(thisclient->items[thisclient->Trade->trade_itemid[i]], pakt);
						ADDBYTE( pako, newslot );
						pako = AddItemData(otherclient->items[newslot], pako);
						tucount++;
						oucount++;
						//Saving slots in database.
						//otherclient->SaveSlot41(newslot);
						//thisclient->SaveSlot41(thisclient->Trade->trade_itemid[i]);
						thisclient->saveinventory();
						otherclient->saveinventory();
					}
					if(otherclient->Trade->trade_count[i] > 0)
                    {
						CItem thisitem = otherclient->items[otherclient->Trade->trade_itemid[i]];
                        thisitem.sp_value=0;
                        thisitem.last_sp_value=0;
						unsigned newslot = thisclient->GetNewItemSlot( thisitem );
						if(newslot==0xffff) continue;
						otherclient->items[otherclient->Trade->trade_itemid[i]].count -= otherclient->Trade->trade_count[i];
						if( otherclient->items[otherclient->Trade->trade_itemid[i]].count<=0 )
      						ClearItem( otherclient->items[otherclient->Trade->trade_itemid[i]] );
                        if (thisclient->items[newslot].count > 0)
                            thisitem.count = thisclient->items[newslot].count + otherclient->Trade->trade_count[i];
                        else
                            thisitem.count = otherclient->Trade->trade_count[i];
                        thisclient->items[newslot]=thisitem;
						ADDBYTE( pako, (unsigned char)otherclient->Trade->trade_itemid[i] );
						pako = AddItemData(otherclient->items[otherclient->Trade->trade_itemid[i]], pako);
						ADDBYTE( pakt, newslot );
						pakt = AddItemData(thisclient->items[newslot], pakt);
						tucount++;
						oucount++;
						//Saving slots in database.
						//thisclient->SaveSlot41(newslot);
                        //otherclient->SaveSlot41(otherclient->Trade->trade_itemid[i]);
						otherclient->saveinventory();
					}
				}

				// Update itemcount and send
				SETBYTE( pakt, 8, tucount );
				SETBYTE( pako, 8, oucount );
				thisclient->client->SendPacket( &pakt );
				otherclient->client->SendPacket( &pako );
				thisclient->Trade->trade_status=0;
				otherclient->Trade->trade_status=0;
			}
			break;
	}

	return true;
}

// Trade action
bool CWorldServer::pakTradeAdd ( CPlayer* thisclient, CPacket* P )
{
    if(thisclient->Shop->open)
        return true;
	BYTE islot = GETBYTE((*P),0);
	WORD slotid = GETWORD((*P),1);

	//LMA: check:
	if(islot<0x0a&&slotid>=MAX_INVENTORY)
	{
	    Log(MSG_HACK,"Player %s tries to trade an item with incorrect slot %u >= %u",thisclient->CharInfo->charname,slotid,MAX_INVENTORY);
	    return false;
	}

	DWORD count = GETDWORD((*P),3);
	CPlayer* otherclient = GetClientByID( thisclient->Trade->trade_target, thisclient->Position->Map );
	if (otherclient==NULL) return true;
	BEGINPACKET( pak, 0x7c1 );
	ADDBYTE( pak, islot );

	//Zuly
	if( islot == 0x0a )
    {
        if (count > thisclient->CharInfo->Zulies)
           return true;
		ADDDWORD( pak, 0xccccccdf );
		ADDDWORD( pak, count );
        ADDDWORD( pak, 0xcccccccc );
        ADDWORD ( pak, 0xcccc );
		thisclient->Trade->trade_count[islot] = count;
	}
    else
    {
        if(islot>0x0a)
            return false;

        //Items
		if( count != 0 )
        {
            //add a slot.
            if(count>thisclient->items[slotid].count)
                return false;

            for (int ii=islot+1;ii<10;ii++)
            {
                if(slotid!=0&&(slotid==thisclient->Trade->trade_itemid[ii]))
                {
                    Log(MSG_HACK, "[TRADEADD] Player %s tried to dupe item in slot %i (%u::%u)",thisclient->CharInfo->charname,slotid,thisclient->items[slotid].itemtype,thisclient->items[slotid].itemnum);
                    return false;
                }

            }

			thisclient->Trade->trade_count[islot] = count;
			thisclient->Trade->trade_itemid[islot] = slotid;
			CItem tmpitem = thisclient->items[slotid]; tmpitem.count = count;
			pak = AddItemData(tmpitem, pak);
		}
        else
        {
            //Delete a slot.
			thisclient->Trade->trade_count[islot] = 0;
			thisclient->Trade->trade_itemid[islot] = 0;
			ADDDWORD( pak, 0 );
			ADDDWORD( pak, 0 );
            ADDDWORD( pak, 0x00000000 );
            ADDWORD ( pak, 0x0000 );
		}
	}
	otherclient->client->SendPacket( &pak );
	return true;
}

// Send Quest information (QSD version)
void CWorldServer::pakQuestData( CPlayer *thisclient )
{
    BEGINPACKET (pak, 0x71b);
    for(dword i = 0; i < 5; i++) ADDWORD( pak, thisclient->quest.EpisodeVar[i]);
    for(dword i = 0; i < 3; i++) ADDWORD( pak, thisclient->quest.JobVar[i]);
    for(dword i = 0; i < 7; i++) ADDWORD( pak, thisclient->quest.PlanetVar[i]);
    for(dword i = 0; i < 10; i++) ADDWORD( pak, thisclient->quest.UnionVar[i]);

    for( unsigned i = 0; i < 10; i++ )
    {
        ADDWORD( pak,  thisclient->quest.quests[i].QuestID );
        long int Time = 0;
        if (thisclient->quest.quests[i].QuestID > 0 && STB_QUEST.rows[thisclient->quest.quests[i].QuestID][1] > 0) 
		{
            Time += thisclient->quest.quests[i].StartTime; // Start time
            Time += STB_QUEST.rows[thisclient->quest.quests[i].QuestID][1] * 10; // Time to finish
            Time -= time(NULL); // Current time
            if (Time < 0) Time = 0; // Time is up
            Time /= 10; // Divide to get 10's of seconds
        }
        ADDDWORD( pak, Time ); // Time Left
        for(dword j = 0; j < 10; j++) ADDWORD( pak, thisclient->quest.quests[i].Variables[j]);
        for(dword j = 0; j < 4; j++) ADDBYTE( pak, thisclient->quest.quests[i].Switches[j]);
        for(dword j = 0; j < 6; j++) 
		{
            if (thisclient->quest.quests[i].Items[j].itemnum != 0) 
			{
				pak = AddItemData(thisclient->quest.quests[i].Items[j], pak);
            } 
			else 
			{
                CItem NullItem;
				pak = AddItemData(NullItem, pak);
            }
        }
    }
    for( unsigned i = 0; i < 0x40; i++ ) //64 flags
        ADDBYTE( pak, thisclient->quest.Qflags[i] );

    //clan wars. Not coded. Client wants 10 of these
    for( unsigned i = 0; i < 10; i++ )
        ADDWORD( pak, 0x0000 );

    //LMA: Getting wishlist.
    GetWishlist(thisclient);

    for (unsigned i = 0; i < MAX_WISHLIST; i++)
    {
        pak = GServer->AddItemData(thisclient->wishlistitems[i], pak);
    }
    thisclient->client->SendPacket( &pak );
}

// Handle quest triggers
bool CWorldServer::pakQuestTrigger( CPlayer* thisclient, CPacket* P )
{
	// action 1 = send text
	// action 2 = delete quest
	// action 3 = get quest
	// action 4 = unknown
	// action 5 = success
	byte action = GETBYTE((*P),0);
	byte slot = GETBYTE((*P),1);
	dword hash = GETDWORD((*P),2);

	//if( thisclient->questdebug ) SendPM( thisclient, "Event Trigger recieved [%08x] Action %i", hash, action);
	if( thisclient->questdebug )Log(MSG_DEBUG,"Event Trigger recieved [%08x] Action %i", hash, action);
	if (action == 2)
	{
		//if( thisclient->questdebug ) SendPM( thisclient, "Delete quest - Slot %i", slot);
		if( thisclient->questdebug ) Log(MSG_DEBUG, "Delete quest - Slot %i", slot);
		for (dword i = slot; i < 9; i++) 
		{
			thisclient->quest.quests[i] = thisclient->quest.quests[i+1];
		}
		thisclient->quest.quests[9].Clear();
		
		BEGINPACKET ( pak, 0x730);
		ADDBYTE ( pak, 0x03);
		ADDBYTE ( pak, slot);
		ADDDWORD( pak, hash);
		thisclient->client->SendPacket(&pak);
		return true;
	}
	if (action != 3) return false;
	int success = thisclient->ExecuteQuestTrigger(hash);
	//if( thisclient->questdebug ) Log(MSG_DEBUG, "Executed quest trigger with result %i", success);

	BEGINPACKET ( pak, 0x730);
	ADDBYTE ( pak, success);
	ADDBYTE ( pak, 0);
	ADDDWORD( pak, hash);
	thisclient->client->SendPacket(&pak);
	thisclient->savequests(thisclient);
	return true;
}
// Handle quest triggers
// PY: This is crap so we won't use it. Keeps returning hack failure to valid quests
// Switching to pakQuestTrigger from KTRose. Even the name makes more sense
/*
bool CWorldServer::pakGiveQuest( CPlayer* thisclient, CPacket* P )
{
	byte action = GETBYTE((*P),0);
	byte slot = GETBYTE((*P),1);
	dword hash = GETDWORD((*P),2);


	if( thisclient->questdebug )
	{
		SendPM( thisclient, "Event Trigger [%08x] Action %i", hash, action);
	}

	LogDebugPriority(3);
	LogDebug("PakGiveQuest %s:: %u ([%08x]), action %i slot %i",thisclient->CharInfo->charname,hash,hash,action,slot);
	Log(MSG_INFO,"PakGiveQuest %s:: %u ([%08x]), action %i slot %i",thisclient->CharInfo->charname,hash,hash,action,slot);
	LogDebugPriority(4);

	if (action == 2)
	{
		if( thisclient->questdebug ) SendPM( thisclient, "Delete quest - Slot %i", slot);
		for (dword i = slot; i < 9; i++) thisclient->quest.quests[i] = thisclient->quest.quests[i+1];
		thisclient->quest.quests[9].Clear();
		BEGINPACKET ( pak, 0x730);
		ADDBYTE ( pak, 0x03);
		ADDBYTE ( pak, slot);
		ADDDWORD( pak, hash);
		thisclient->client->SendPacket(&pak);

		  LogDebugPriority(3);
		  LogDebug("PakGiveQuest %s:: %u ([%08x]), action %i slot %i delete quest",thisclient->CharInfo->charname,hash,hash,action,slot);
		  Log(MSG_INFO,"PakGiveQuest %s:: %u ([%08x]), action %i slot %i delete quest",thisclient->CharInfo->charname,hash,hash,action,slot);
		  LogDebugPriority(4);
		return true;
	}
	if (action != 3)
	{
		Log(MSG_INFO,"action not 3, %i",action);
		return false;
	}
	//LMA: TEST HACK QUEST
	#ifdef QHACK
	bool is_ok=false;
	//We test if a quest is a stackable one and then if the player is waiting for one of these...
	//Is the quest in the "die monster" list...
	if (MapStackQuest.find(hash) != MapStackQuest.end())
	{
		Log(MSG_WARNING,"Player %s sent special questid %u",thisclient->CharInfo->charname,hash);
		clock_t mytime = clock();
		for (int k=0;k<10;k++)
		{
			if(thisclient->arr_questid[k].questid != hash)
			{
				continue;
			}
			//checking the time now (1/2 second between death packet and quest packet, max).
			if (thisclient->arr_questid[k].die_time >= mytime)
			{
				thisclient->arr_questid[k].questid = 0;
				is_ok = true;
				Log(MSG_WARNING,"Player %s had a valid questid %u waiting in slot %i",thisclient->CharInfo->charname,hash,k);
				break;
			}
		}
		if(!is_ok)
		{
			Log(MSG_HACK,"Player %s tried to use stackable questid %u but didn't expect it.",thisclient->CharInfo->charname,hash);
			for (int k=0;k<10;k++)
			{
				if(thisclient->arr_questid[k].questid==0)
				{
					continue;
				}
				Log(MSG_WARNING,"qid for %s[%i]=%u (time %u), now=%u",thisclient->CharInfo->charname,k,thisclient->arr_questid[k].questid,thisclient->arr_questid[k].die_time,mytime);
			}
			int success = QUEST_FAILURE;    //sending failure to the client.
			BEGINPACKET ( pak, 0x730);
			ADDBYTE ( pak, success);
			ADDBYTE ( pak, 0);
			ADDDWORD( pak, hash);
			thisclient->client->SendPacket(&pak);
			return true;
		}
	}

	#endif
	//LMA END.

	int success = thisclient->ExecuteQuestTrigger(hash);

	BEGINPACKET ( pak, 0x730);
	ADDBYTE ( pak, success);
	ADDBYTE ( pak, 0);
	ADDDWORD( pak, hash);
	thisclient->client->SendPacket(&pak);

	Log(MSG_INFO,"PakGiveQuest %s:: %u end",thisclient->CharInfo->charname,hash);

	LogDebugPriority(3);
	LogDebug("PakGiveQuest %s:: %u ([%08x]), action %i slot %i result %i",thisclient->CharInfo->charname,hash,hash,action,slot,success);
	Log(MSG_INFO,"PakGiveQuest %s:: %u ([%08x]), action %i slot %i result %i",thisclient->CharInfo->charname,hash,hash,action,slot,success);
	LogDebugPriority(4);


	return true;
}
*/

// Self skills
bool CWorldServer::pakSkillSelf( CPlayer* thisclient, CPacket* P )
{
    if( thisclient->Shop->open || thisclient->Status->Stance==DRIVING ||
        thisclient->Status->Mute!=0xff || !thisclient->Status->CanCastSkill)
        return true;
    WORD num = GETWORD((*P),0);
    if(num>=MAX_ALL_SKILL)
    {
        Log( MSG_HACK, "Invalid Skill id %i for %s ", num, thisclient->CharInfo->charname );
        return true;
    }

	unsigned int skillid = thisclient->cskills[num].id+thisclient->cskills[num].level-1;
	CSkills* thisskill = GetSkillByID( skillid );
	Log( MSG_INFO, "pakSkillSelf for %s (slot %i, skill %i)", thisclient->CharInfo->charname,num,skillid);

	if(thisskill == NULL)
	   return true;
	unsigned int skilltarget = thisskill->target;
    unsigned int skillrange = thisskill->aoeradius;

    //LMA: time to check the cooldown time.
    time_t etime=time(NULL);
    if (etime<thisclient->cskills[num].cooldown_skill)
    {
        Log(MSG_HACK,"Player %s tried to pakSkillSelf skill %u but cooldown wasn't finished %u<%u",thisclient->CharInfo->charname,skillid,etime,thisclient->cskills[num].cooldown_skill);
        return true;
    }

    //checking if player has enough MP too.
    //LMA: Special test, it seems sometimes the MP value is off after a level up??
    if(thisclient->Stats->MP>thisclient->Stats->MaxMP)
    {
        thisclient->Stats->MP=thisclient->Stats->MaxMP;
    }

    UINT needed_mp=(thisskill->mp - (thisskill->mp * thisclient->Stats->MPReduction / 100));
    if (thisclient->Stats->MP<needed_mp)
    {
        Log(MSG_HACK,"Player %s tried to pakSkillSelf skill %u but didn't have enough MP %u<%u",thisclient->CharInfo->charname,skillid,thisclient->Stats->MP,needed_mp);
        return true;
    }

    //We set the time the player can cast this skill next time.
    thisclient->cskills[num].cooldown_skill=etime+thisskill->cooldown;
    Log(MSG_WARNING,"Next pakSkillSelf skill %u available at %u (is %u)",skillid,thisclient->cskills[num].cooldown_skill,etime);

	if( thisskill->aoe == 0 )
	{
        thisclient->StartAction( NULL, BUFF_SELF, skillid );
    }
    else
    {
        if(isSkillTargetFriendly( thisskill ))
        {
            thisclient->StartAction( NULL, BUFF_AOE, skillid );
        }
        else
        {
            thisclient->StartAction( NULL, SKILL_AOE, skillid );
        }
    }
    return true;
}

// Consumible Items
bool CWorldServer::pakUseItem ( CPlayer* thisclient, CPacket* P )
{
    if(thisclient->Shop->open)
    {
        return true;
    }

    BYTE slot = GETBYTE((*P),0);
    if(!CheckInventorySlot( thisclient, slot))
    {
        return false;
    }

    if( thisclient->items[slot].count<=0 )
    {
        return true;
    }

    CUseInfo* thisuse = GetUseItemInfo( thisclient, slot);
    if(thisuse == NULL)
    {
        Log(MSG_WARNING,"%s [%i]:: Invalid Item, Item: (%i::%i).",thisclient->CharInfo->charname,
        thisclient->clientid,thisclient->items[slot].itemtype,thisclient->items[slot].itemnum);
        return true;
    }

    bool flag = false;
	//if the useitem has an attached skill.....
	if( thisuse->skillid != 0 )
	{
		CSkills* thisskill = GetSkillByID( thisuse->skillid );
		//This one line of code should replace almost the entire switch below
		thisclient->StartAction( NULL, thisskill->skilltype, thisskill->id );

		if( thisskill->WarpZone != 0 ) //return scrolls or any other item that runs a warp in the skill. this is NOT handled in skill handling for some reason
		{
			BEGINPACKET( pak,0x7a3 );
            ADDWORD    ( pak,thisclient->clientid );
            ADDWORD    ( pak, thisuse->itemnum );
            ADDBYTE    ( pak,slot );
            thisclient->client->SendPacket( &pak );
            fPoint thispoint;
            thispoint.x = thisskill->WarpX * 10;
			thispoint.y = thisskill->WarpY * 10;			
            TeleportTo ( thisclient, thisskill->WarpZone, thispoint );
			thisuse->usescript = 999;
		}
		flag = true;
	}

    switch(thisuse->usescript)
    {
        //PY case 0 covered by startaction call above
		//case 0: //  cherry berrys
            //thisclient->StartAction( NULL, BUFF_SELF, thisuse->usevalue );
            //thisclient->items[slot].count -= 1;
            //if( thisclient->items[slot].count == 0 )
            //    ClearItem( thisclient->items[slot] );
            //flag = true;
        //break;
        case 1: // Food and mana jam
        {
            //LMA: Skill points (Mana Jam)
            if(thisuse->usetype == 37)
            {
                thisclient->CharInfo->SkillPoints += thisuse->usevalue;
                BEGINPACKET( pak,0x7a3 );
                ADDWORD    ( pak, thisclient->clientid );
                ADDWORD    ( pak, thisuse->itemnum );
                SendToVisible( &pak, thisclient );
                flag = true;
                break;
            }

            thisclient->UsedItem->usevalue = thisuse->usevalue;
            thisclient->UsedItem->usetype = thisuse->usetype;
            thisclient->UsedItem->userate = 15;
            thisclient->UsedItem->used = 0;
            thisclient->UsedItem->lastRegTime=0;

            //LMA: handling cooldown for food.
            if(thisuse->cooldown_type>0&&thisuse->cooldown>0)
            {
                thisclient->UsedItem->userate=thisuse->usevalue/thisuse->cooldown;
            }

            BEGINPACKET( pak,0x7a3 );
            ADDWORD    ( pak, thisclient->clientid );
            ADDWORD    ( pak, thisuse->itemnum );
            SendToVisible( &pak, thisclient );
            flag = true;
        }
        break;
        //case 2: // Return Scroll
        //{
        //    BEGINPACKET( pak,0x7a3 );
        //    ADDWORD    ( pak,thisclient->clientid );
        //    ADDWORD    ( pak, thisuse->itemnum );
        //    ADDBYTE    ( pak,slot );
        //    thisclient->client->SendPacket( &pak );
            //thisclient->items[slot].count -= 1;
            //if( thisclient->items[slot].count == 0 )
            //    ClearItem( thisclient->items[slot] );
        //    fPoint thispoint;
		//	UINT tmpval = thisuse->usevalue/10000;
        //    thispoint.x = floor( (double)thisuse->usevalue/10000 );
			// Modulus operator returns the remainder so thisuse->usevalue%10000 will always return an integer so no floor necessary
            //thispoint.y = floor( thisuse->usevalue % 10000 );	//old code
		//	thispoint.y = thisuse->usevalue % 10000;			//new code for c++ 11
        //    TeleportTo ( thisclient, thisuse->usetype, thispoint );
        //    flag = true;
        //}
        //break;

        /*			//PY covered above
		case 3: // Charm Scroll
        {
	        CSkills* thisskill = GetSkillByID( thisuse->usetype );
            if(thisskill == NULL)
            {
                Log(MSG_WARNING, "Char %s tried to use invalid scroll type: %i", thisclient->CharInfo->charname, thisuse->usetype );
                delete thisuse;
          	    return true;
            }
        	int skilltarget = thisskill->target;
            int skillrange = thisskill->aoeradius;
        	if( thisskill->aoe == 0 )
        	{
                thisclient->StartAction( NULL,BUFF_SELF,thisuse->usetype );
            }
            else
            {
                if(isSkillTargetFriendly( thisskill ))
                {
                    thisclient->StartAction( NULL,BUFF_AOE,thisuse->usetype );
                }
                else
                {
                    thisclient->StartAction( NULL,SKILL_AOE,thisuse->usetype );
                }
            }
            flag = true;
        }
        break;
		*/
        case 4: // Dance Scroll | FireCrackers | Emotion
        {
            //Start Animation
            BEGINPACKET( pak, 0x7b2 );
            ADDWORD    ( pak, thisclient->clientid );
            ADDWORD    ( pak, thisuse->usevalue );
            SendToVisible( &pak, thisclient );
            //Finish Animation
            RESETPACKET( pak, 0x7bb );
            ADDWORD    ( pak, thisclient->clientid );
            SendToVisible( &pak, thisclient );
            //????
            RESETPACKET( pak, 0x7b9);
            ADDWORD    ( pak, thisclient->clientid);
            ADDWORD    ( pak, thisuse->usevalue );
	        SendToVisible( &pak, thisclient );
            flag = true;
        }
        break;
        case 5: // Summons
        {
            CNPCData* thisnpc = GetNPCDataByID( thisuse->usevalue );
            if(thisnpc==NULL)
            {
                Log( MSG_WARNING,"[%i]NPCDATA NOT Founded: %i.",
                                    thisclient->clientid,thisuse->usevalue );
                delete thisuse;
                return true;
            }
            //Start Animation
            BEGINPACKET( pak, 0x7b2 );
            ADDWORD    ( pak, thisclient->clientid );
            ADDWORD    ( pak, thisuse->usetype );
            //ADDBYTE    ( pak, 6 );
            SendToVisible( &pak, thisclient );

            /*
            //Finish Animation
            RESETPACKET( pak, 0x7bb );
            ADDWORD    ( pak, thisclient->clientid );
            SendToVisible( &pak, thisclient );
            */

            //????
            RESETPACKET( pak, 0x7b9);
            ADDWORD    ( pak, thisclient->clientid);
            ADDWORD    ( pak, thisuse->usetype );
	        SendToVisible( &pak, thisclient );

	        // Add our Mob to the mobs list
           	fPoint position = RandInCircle( thisclient->Position->current, 5 );
           	CMap* map = MapList.Index[thisclient->Position->Map];
           	CMonster * this_summon = map->AddMonster( thisuse->usevalue, position, thisclient->clientid );

            if (this_summon!=NULL)
            {
           	   this_summon->skillid=thisuse->usetype;
           	   this_summon->buffid=thisuse->use_buff;

           	   //LMA: new HP amount.
                this_summon->Stats->MaxHP=SummonFormula(thisclient,this_summon);
                this_summon->Stats->HP=this_summon->Stats->MaxHP;
            }

            flag = true;
        }
        break;
        case 6: // Snowball
        {
            WORD clientid = GETWORD((*P),2);
            fPoint thispoint;
            CMap* map = MapList.Index[thisclient->Position->Map];
            CCharacter* character = map->GetCharInMap( clientid );
            if(character==NULL)
            {
                delete thisuse;
                return true;
            }

            thisclient->StartAction( character, SKILL_BUFF, thisuse->usevalue );
            flag = true;
        }
        break;
        case 7: // scrolls [damage,maxhp,...]
        {
	        CSkills* thisskill = GetSkillByID( thisuse->usetype );
            if(thisskill == NULL)
            {
                Log(MSG_WARNING, "Char %s tried to use invalid scroll type: %i", thisclient->CharInfo->charname, thisuse->usetype );
                delete thisuse;
          	    return true;
            }
        	int skilltarget = thisskill->target;
            int skillrange = thisskill->aoeradius;
        	if( thisskill->aoe == 0 )
        	{
                thisclient->StartAction( NULL, BUFF_SELF, thisuse->usetype );
            }
            else
            {
                if(isSkillTargetFriendly( thisskill ))
                {
                    thisclient->StartAction( NULL, BUFF_AOE, thisuse->usetype );
                }
                else
                {
                    thisclient->StartAction( NULL, SKILL_AOE, thisuse->usetype );
                }
            }
            flag = true;
        }
        break;
        case 10: // Skill Book
        {
            flag = LearnSkill( thisclient, thisuse->usevalue );
        }
        break;
        case 11:
        {
             //LMA: some special skill cases where it's handled in quest itself :)
             flag=false;
        }
        break;
        case 12:
        {
             //LMA: Refuel :)
            thisclient->TakeFuel(thisuse->usevalue);
            flag=true;
        }
        break;
        case 13:
        {
             //LMA: Clan Points :)
            thisclient->GiveCP(thisuse->usevalue);
            flag=true;
        }
        break;
        case 14:
        {
            //LMA: PY's code, different script for dance scrolls
            CSkills* thisskill = GetSkillByID( thisuse->usetype );
            if(thisskill == NULL)
            {
                Log(MSG_WARNING, "Char %s tried to use invalid Dance scroll type: %i", thisclient->CharInfo->charname, thisuse->usetype );
                delete thisuse;
                return true;
            }
            Log(MSG_INFO, "Char %s used a Dance scroll type: %i", thisclient->CharInfo->charname, thisuse->usetype );
            int skilltarget = thisskill->target;
            int skillrange = thisskill->aoeradius;
            flag = true;
            Log(MSG_INFO, "Sending skill request. AOE range = %i", skillrange);
            if(skillrange > 0 ) //AOE effect
            {
                thisclient->StartAction( NULL,BUFF_AOE,thisuse->usetype );
            }
            else
            {
                thisclient->StartAction( NULL,BUFF_SELF,thisuse->usetype );
            }
            flag = true;
        }
        break;
        case 15: //LMA: Rocks
        {
            WORD clientid = GETWORD((*P),2);
            fPoint thispoint;
            CMap* map = MapList.Index[thisclient->Position->Map];
            CCharacter* character = map->GetCharInMap( clientid );
            if(character==NULL)
            {
                delete thisuse;
                return true;
            }

            thisclient->StartAction( character, SKILL_ATTACK, thisuse->usevalue );
            flag = true;
        }
        break;
        case 16: //LMA: bombs
        {
            WORD clientid = GETWORD((*P),2);

            //LMA: for now we use this...
            fPoint thispoint;
            CMap* map = MapList.Index[thisclient->Position->Map];
            CCharacter* character = map->GetCharInMap( clientid );
            if(character==NULL)
            {
                delete thisuse;
                return true;
            }

            thisclient->StartAction( character, SKILL_ATTACK, thisuse->usevalue );
            flag = true;
        }
        break;
        case 17:
        {
            //LMA: Skill resets...
            int l_b=0;
            int l_e=0;
            switch(thisuse->itemnum)
            {
                case 95:
                case 898:
                case 900:
                {
                    //Job skill.
                    l_b=0;
                    l_e=60;
                }
                break;
                case 96:
                case 901:
                {
                    //unique
                    l_b=90;
                    l_e=120;
                }
                break;
                case 97:
                case 902:
                {
                    //mileage
                    l_b=120;
                    l_e=320;
                }
                break;
                case 98:
                case 903:
                {
                    //all skills
                    l_b=0;
                    l_e=320;
                }
                break;
                default:
                {
                    Log(MSG_WARNING,"Unknown 323 case for %i",thisuse->itemnum);
                }
                break;

            }

            if(l_b==0&&l_e==0)
            {
                delete thisuse;
                return true;
            }

            //let's delete some skills...
            int nb_skills_points=0;
            for (int k=l_b;k<l_e;k++)
            {
                //How many skill points we had to spend to learn this one?
                if (thisclient->cskills[k].thisskill!=NULL&&thisclient->cskills[k].thisskill->sp>0)
                {
                    nb_skills_points+=thisclient->cskills[k].level;
                }

                thisclient->cskills[k].id=0;
                thisclient->cskills[k].level=1;
                thisclient->cskills[k].thisskill=NULL;
            }

            Log(MSG_INFO,"Skill reset for %s, he gets %i skills points back",thisclient->CharInfo->charname,nb_skills_points);
            thisclient->CharInfo->SkillPoints+=nb_skills_points;

            //LMA: for tests.
            thisclient->saveskills();
            BEGINPACKET( pak,0x7a3 );
            ADDWORD    ( pak, thisclient->clientid );
            ADDWORD    ( pak, thisuse->itemnum );
            SendToVisible( &pak, thisclient );

            thisclient->items[slot].count -= 1;
            if( thisclient->items[slot].count <= 0 )
                ClearItem( thisclient->items[slot] );
            thisclient->UpdateInventory(slot,0xffff);
            flag=false;
        }
        break;
        case 18:
        {
            //LMA: sometimes nothing to do...
            flag = true;
        }
		case 999: //PY to catch skills that are handled right here at the top of this function
		break;
		default:
		{
			Log(MSG_WARNING,"Item used has invalid usescript %i",thisuse->usescript);
		}
		break;
    }
    if(flag == true)
    {
        thisclient->items[slot].count -= 1;
        if( thisclient->items[slot].count <= 0 )
            ClearItem( thisclient->items[slot] );
        BEGINPACKET( pak,0x7a3 );
        ADDWORD    ( pak, thisclient->clientid );
        ADDWORD    ( pak, thisuse->itemnum );
        ADDBYTE    ( pak, slot );
        thisclient->client->SendPacket( &pak );
    }
    delete thisuse;


    return true;
}

//LMA: Level UP Skill (new way)
bool  CWorldServer::pakLevelUpSkill( CPlayer *thisclient, CPacket* P )
{
    WORD pos = GETWORD ((*P),0);   // number of skill. appears to be an array index
    WORD skill = GETWORD ((*P),2); // skill id
    if(pos >= MAX_ALL_SKILL)
    {
        Log( MSG_HACK, "Invalid Skill id %i for %s ", pos, thisclient->CharInfo->charname );
        return false;
    }
    CSkills* thisskill = GetSkillByID( skill );
    if(thisskill==NULL)
        return true;
    if(thisclient->cskills[pos].id != skill - thisclient->cskills[pos].level)
    {
        Log(MSG_WARNING,"Skill Upgrade, wrong org skill %i != %i",thisclient->cskills[pos].id,skill - thisclient->cskills[pos].level);
        return true;
    }

    // checks made for prerequisite skills here.
    UINT hasPreskill = 0;
    for(int i=0;i<3;i++)
    {
        int preskill = thisskill->rskill[i];
        if(thisskill->lskill[i] > 0)
            preskill += thisskill->lskill[i] - 1;
        if(preskill == 0)
        {
            hasPreskill ++; // no preskill defined in this element so give a credit then skip to the next preskill.
            continue;
        }
        //Log( MSG_INFO, "[DEBUG] Checking %i / 3, preskill %i",i,preskill);

        for(int skillid=0;skillid<MAX_ALL_SKILL;skillid++)
        {
            //Log( MSG_INFO, "[DEBUG] Skillid %i < %u",skillid,MAX_SKILL);
            if(thisclient->cskills[skillid].id == thisskill->rskill[i] && thisclient->cskills[skillid].level >= thisskill->lskill[i])
            //int checkskill = thisclient->cskills[skillid].id + thisclient->cskills[skillid].level -1;
            //if(preskill == checkskill)
            {
                hasPreskill ++;
                //Log( MSG_INFO, "[DEBUG] Skill matched to requirements");
                break; // no need to carry on. Skill found
            }
        }
   }

    //Log( MSG_INFO, "[DEBUG] hasPreskill state = %i",hasPreskill);
    if(hasPreskill != 3)
    {
        Log( MSG_INFO, "Prerequisite skills not all found" );
        return true; //doesn't have the necessary prerequisite skill
    }

    // is character a high enough level?
    if(thisskill->clevel > thisclient->Stats->Level)
    {
        Log( MSG_INFO, "Character level too low" );
        return true; //not high enough level
    }

    //check that it is the next skill in the series
    if(thisclient->cskills[pos].id != skill - thisclient->cskills[pos].level) //check that it is the next skill in the series
    {
        Log( MSG_INFO, "Skill Id doesn't match next in the CSkills series" );
        return true;
    }

    if(thisclient->CharInfo->SkillPoints >= thisskill->sp)
    {
       thisclient->CharInfo->SkillPoints -= 1;

       BEGINPACKET( pak, 0x7b1 );
       ADDBYTE    ( pak, 0x00);
       ADDWORD    ( pak, pos);
       ADDWORD    ( pak, skill);
       ADDWORD    ( pak, thisclient->CharInfo->SkillPoints);
       thisclient->client->SendPacket( &pak );
       thisclient->SetStats( );

       thisclient->cskills[pos].level+=1;
       thisclient->cskills[pos].thisskill = thisskill;
       thisclient->cskills[pos].cooldown_skill=0;

       //thisclient->UpgradeSkillInfo(pos,thisclient->cskills[pos].id,1);
       thisclient->saveskills();
    }


    return true;
}



// Equip bullets arrows and cannons
bool CWorldServer::pakEquipABC ( CPlayer* thisclient, CPacket* P )
{
    if(thisclient->Shop->open)
        return true;
    BYTE itemslot = GETBYTE((*P),0);
    int slot = ((itemslot-32)/4)+72;
    int dest = 0;
    int type = (itemslot-32)%4;
    if(!CheckInventorySlot( thisclient, slot))
        return false;
    if(itemslot>3)
    {
        dest = type+132;
    }
    else
    {
        slot = itemslot + 132;
        dest = thisclient->GetNewItemSlot( thisclient->items[slot] );
        if(dest==0xffff)
            return true;
        type=300;
    }
	CItem tmpitm = thisclient->items[slot];
	thisclient->items[slot] =thisclient->items[dest];
	thisclient->items[dest] = tmpitm;

	thisclient->UpdateInventory( dest, slot );
    BEGINPACKET( pak, 0x7ab );
    ADDWORD    ( pak, thisclient->clientid );
    unsigned int effect = ( thisclient->items[dest].itemnum * 32 ) + type;
    ADDWORD    ( pak, effect );
    SendToVisible( &pak, thisclient );
    thisclient->Stats->Attack_Power = thisclient->GetAttackPower( );
    return true;
}


// Craft item  Completely recoded by: PurpleYouko)  
bool CWorldServer::pakCraft( CPlayer* thisclient, CPacket* P )
{
	if(thisclient->Shop->open == true)
        return true;
	CItem item;
	UINT quality = 0;
	int	m = 0;

	// determine the item number and type
	item.itemnum = (GETWORD((*P), 3));
	item.itemtype = (GETBYTE((*P), 0x2));

	//get material used lookup number
	int	materialnumber = 0;
	int craftID = 0;
	int craftlevel = 0;
	if(item.itemtype < 10)
	{
        materialnumber = EquipList[item.itemtype].Index[item.itemnum]->material;
    }
    else
    {
        switch(item.itemtype)
        {
		case 10:
			materialnumber = UseList.Index[item.itemnum]->material;
			craftID = UseList.Index[item.itemnum]->craft;
            craftlevel = UseList.Index[item.itemnum]->craftlevel;
			break;
		case 11:
			materialnumber = JemList.Index[item.itemnum]->material;
			craftID = JemList.Index[item.itemnum]->craft;
            craftlevel = JemList.Index[item.itemnum]->craftlevel;
			break;
		case 12:
			materialnumber = NaturalList.Index[item.itemnum]->material;
			craftID = NaturalList.Index[item.itemnum]->craft;
            craftlevel = NaturalList.Index[item.itemnum]->craftlevel;
			break;
        case 14:
			materialnumber = PatList.Index[item.itemnum]->material;
			craftID = PatList.Index[item.itemnum]->craft;
            craftlevel = PatList.Index[item.itemnum]->craftlevel;
			break;
        default:
            break;
        }
    }
	if(thisclient->Session->codedebug)
	{
       SendPM(thisclient, "Attempting to craft an item. type = %i. id = %i",item.itemtype, item.itemnum);
       SendPM(thisclient, "Material number = %i", materialnumber);
    }
	//check material slots before we start making the new item
	if(ProductList.Index[materialnumber]->family != 0) // recipe has an item type listed rather than a specific item
    {
        WORD material = GETWORD((*P), 4);
        UINT matid = thisclient->items[material].itemnum;
        quality = NaturalList.Index[matid]->quality;
        if(thisclient->Session->codedebug)
    	{
           SendPM(thisclient, "Recipe recquires an item with type = %i.",ProductList.Index[materialnumber]->family);
           SendPM(thisclient, "Type of material in slot = %i",thisclient->items[material].type);
        }
        if(ProductList.Index[materialnumber]->family != thisclient->items[material].type)
        {
            if(thisclient->Session->codedebug)
        	{
               SendPM(thisclient, "Rejected as mat types do not match");
            }
            return true; //item in the slot is the wrong type
        }
    }
    m = 0;
    for(char used = 5; used != 13; used += 2)
    {
        WORD material = GETWORD((*P), used);//gets inventory location
        if(thisclient->Session->codedebug)
    	{
           SendPM(thisclient, "Slot number = %i", material);
           SendPM(thisclient, "Recipe recquires an item with type = %i. id = %i",ProductList.Index[materialnumber]->itemtype[m],ProductList.Index[materialnumber]->itemid[m]);
           SendPM(thisclient, "Type of material in slot = %i. id = %i",thisclient->items[material].itemtype,thisclient->items[material].itemnum);
           SendPM(thisclient, "Material count required = %i", ProductList.Index[materialnumber]->amount[m]);
           SendPM(thisclient, "Material count in slot = %i", thisclient->items[material].count);
        }
        if(!CheckInventorySlot( thisclient, material))
        {
            SendPM(thisclient, "Crafting Error 0: Missing or incorrect material in inventory slot.");
            return true;
        }
        if (material != 0)
        {
            if(ProductList.Index[materialnumber]->family != 0 && m == 0) //only checked for the first item in the recipe
            {
                if(thisclient->items[material].type != ProductList.Index[materialnumber]->family)
                {
                    SendPM(thisclient, "Crafting Error %i 1: Server and Client crafting recipes do not match",m);
					BEGINPACKET( pak, 0x07d8);
                    ADDWORD( pak, thisclient->clientid );
                    ADDBYTE( pak, 0x02);					//craft/drill code 2: craft fail
                    ADDBYTE( pak, 0x00);					//item type. Not used in client
					ADDWORD( pak, 0x0000);					//item number. not used in client
                    thisclient->client->SendPacket( &pak );
                    return true; // item type does not match recipe
                }
            }
            else
            {
                if(thisclient->items[material].itemtype != ProductList.Index[materialnumber]->itemtype[m])
                {
                    SendPM(thisclient, "Crafting Error %i 2: Server and Client crafting recipes do not match",m);
					BEGINPACKET( pak, 0x07d8);
                    ADDWORD( pak, thisclient->clientid );
                    ADDBYTE( pak, 0x02);					//craft/drill code 2: craft fail
                    ADDBYTE( pak, 0x00);					//item type. Not used in client
					ADDWORD( pak, 0x0000);					//item number. not used in client
                    thisclient->client->SendPacket( &pak );
                    return true; // item type does not match recipe
                }
                if(thisclient->items[material].itemnum != ProductList.Index[materialnumber]->itemid[m])
                {
                    SendPM(thisclient, "Crafting Error %i 3: Server and Client crafting recipes do not match",m);
					BEGINPACKET( pak, 0x07d8);
                    ADDWORD( pak, thisclient->clientid );
                    ADDBYTE( pak, 0x02);					//craft/drill code 2: craft fail
                    ADDBYTE( pak, 0x00);					//item type. Not used in client
					ADDWORD( pak, 0x0000);					//item number. not used in client
                    thisclient->client->SendPacket( &pak );
                    return true; // item id does not match recipe
                }
            }
            if(thisclient->items[material].count < ProductList.Index[materialnumber]->amount[m])
            {
                SendPM(thisclient, "Crafting Error %i 4: Server and Client crafting recipes do not match",m);
				BEGINPACKET( pak, 0x07d8);
                ADDWORD( pak, thisclient->clientid );
                ADDBYTE( pak, 0x02);					//craft/drill code 2: craft fail
                ADDBYTE( pak, 0x00);					//item type. Not used in client
				ADDWORD( pak, 0x0000);					//item number. not used in client
                thisclient->client->SendPacket( &pak );
                return true; //not enough of the required item
            }
        }
        m += 1;
    }

	//skills check not coded yet


    if(thisclient->Session->codedebug)
   	{
        SendPM(thisclient, "Item cleared the checks and will be created");
    }

	// item durability randomizer
	int lowest = thisclient->Attr->Con/ 10 + 17;
	int highest = thisclient->Attr->Con / 10 + 70;
	int range = (highest - lowest) + 1;
	item.durability = lowest+int(range*rand()/(RAND_MAX + 1.0));

	// stats randomizer
	int changeofstatslow = thisclient->Attr->Sen / 13 + 10;
	int changeofstatshigh = thisclient->Attr->Sen / 13 + 50;
	int changeofstatsrange = (changeofstatshigh-changeofstatslow)+1;
	if (changeofstatslow+int(changeofstatsrange*rand()/(RAND_MAX + 1.0)) > 50)
    {
    	int statslowget = 1;
    	int statshighget = 256;
    	int setstatrange=(statshighget-statslowget)+1;
      	item.stats = statslowget + int(setstatrange*rand()/(RAND_MAX + 1.0));
    	item.appraised = 1;
	}
    else
    {
        item.stats = 0;
        item.appraised = 0;
	}
	item.socketed = 0;
	item.gem = 0;
	item.lifespan = 100; //Its new so 100%
	item.refine = 0;
	item.count = 1;
	//need to add UStats still


    unsigned newslot = thisclient->GetNewItemSlot( item );
	if (newslot != 0xffff)
    {
        //removes material items from the slots
        item.count +=(thisclient->items[newslot].count);
		thisclient->items[newslot] = item;
		m = 0;
		for(char used=5; used != 13; used += 2)
        {
            WORD material= GETWORD((*P), used);//gets inventory location
            if (material != 0)
            {
		        thisclient->items[material].count -= ProductList.Index[materialnumber]->amount[m];
    	        m += 1;
		        if (thisclient->items[material].count <= 0)
                {
			        ClearItem(thisclient->items[material]);
                }
            }
        }

        BEGINPACKET( pak, 0x07d8);				//send the success packet
        ADDWORD( pak, thisclient->clientid );
        ADDBYTE( pak, 0x01 );					//Craft success
		ADDBYTE( pak, 0x00 )
        ADDWORD( pak, item.itemnum);			//item number
        thisclient->client->SendPacket(&pak);

        RESETPACKET( pak, 0x07af);				//make the item
        ADDBYTE( pak, 0x00);					//RESULT_CREATE_ITEM_SUCCESS
        ADDWORD( pak, newslot);					//send the inventory index

        // Make craft bars
        int bar1 = item.durability * 9;
        int bar2 = changeofstatsrange * 9;
        int bar3 = item.durability + changeofstatsrange * 6;
        int bar4 = item.durability + changeofstatsrange + bar3 / 3;

        ADDWORD( pak, bar1);																//progress bar1 0 is empty 0x0400 is full bar
        ADDWORD( pak, bar2);																//progress bar2 0 is empty 0x0400 is full bar
        if ((GETWORD((*P),  8))==0){ ADDWORD( pak, 0x99a0);}else{ ADDWORD( pak, bar3);}		//progress bar3 0 is empty 0x0400 is full bar
        if ((GETWORD((*P), 10))==0){ ADDWORD( pak, 0x99a0);}else{ ADDWORD( pak, bar4);}		//progress bar4 0 is empty 0x0400 is full bar
        pak = GServer->AddItemData(item, pak);
        int crafting_exp = item.durability + changeofstatsrange * (thisclient->Stats->Level/ 5);
		thisclient->CharInfo->Exp += crafting_exp;//  add exp
		thisclient->client->SendPacket(&pak);

		if(!thisclient->CheckPlayerLevelUP( ))
		{
			RESETPACKET( pak, 0x79b );
			ADDDWORD   ( pak, thisclient->CharInfo->Exp );
			ADDWORD    ( pak, thisclient->CharInfo->stamina );
			ADDWORD    ( pak, 0 );
			thisclient->client->SendPacket( &pak );
		}
	}
	else
	{
		BEGINPACKET (pak, 0x702);
		ADDSTRING(pak, "No free slot !");
		ADDBYTE(pak, 0);
		thisclient->client->SendPacket(&pak);
	}
	thisclient->saveinventory();
	return true;
}



/*
bool CWorldServer::pakCraft( CPlayer* thisclient, CPacket* P )
{
    //0x00=success
    //0x01=failure
    //0x02=not enough MP
    //0x03=not enough materials
    //0x04=no materials
    //0x05=craft skill level is insufficient.
    bool painting = false;
	if(thisclient->Shop->open == true)
        return true;
	CItem item;
	item.count = 1;
	// item durability randomizer
	int lowest = thisclient->GetCon() / 10 + 17;
	int highest = thisclient->GetCon() / 10 + 70;
	int range = (highest - lowest) + 1;
	item.durability = lowest + int(range * rand() / (RAND_MAX + 1.0));

	// durability set
	item.itemnum = (GETWORD((*P), 3));
	item.itemtype = (GETBYTE((*P), 0x2));
	item.lifespan = 100; //Its new so 100%
	item.refine = 0;

	// stats randomizer
	//LMA: depends from level too now... (very basic formula).
	int changeofstatslow = thisclient->GetSen() / 13 + 10;
	//int changeofstatshigh = thisclient->GetSen() / 13 + 50;
	int changeofstatshigh = (thisclient->GetSen() + thisclient->Stats->Level) / 13 + 50;
	int changeofstatsrange = (changeofstatshigh-changeofstatslow)+1;

	if (changeofstatslow + int(changeofstatsrange * rand() / (RAND_MAX + 1.0)) > 50)
	{
        int statslowget = 1;
        int statshighget = 25;

        //LMA: The more level and SEN the player has, the better stats he can have.
        int myrange = (int)(thisclient->GetSen() / 2) + thisclient->Stats->Level;
        if (myrange < 100)
        {
            statshighget = 50;
        }
        else if (myrange >= 100 && myrange < 150)
        {
            statshighget = 100;
        }
        else if (myrange >= 150 && myrange < 200)
        {
            statshighget = 150;
        }
        else if (myrange >= 200 && myrange < 250)
        {
            statshighget = 200;
        }
        else if (myrange >= 250 && myrange < 300)
        {
            statshighget = 250;
        }
        else if (myrange >= 300 && myrange < 350)
        {
            statshighget = 275;
        }
        else if (myrange >= 350)
        {
            statshighget = 300;
        }

        int setstatrange = (statshighget - statslowget) + 1;
        item.stats = statslowget+int(setstatrange*rand()/(RAND_MAX + 1.0));

        if(item.stats > 300)
        {
            item.stats = 300;
        }

        Log(MSG_INFO,"Craft item has a stat %i",item.stats);
        item.appraised = 1;
	}
	else
	{
        item.stats = 0;
        item.appraised = 0;
        Log(MSG_INFO,"Craft item has NO stat");
	}


    //LMA: Craft success or failure?
    int success = 100;
    UINT craftID = 0; //LMA: Searching skill
    UINT craft_level = 0; //LMA: Searching skill
    item.sp_value = 0;
    item.last_sp_value = -1;
    switch(item.itemtype)
    {
        case 10:
        {
            //Use
            //success = UseList.Index[item.itemnum]->craft_difficult;
            craftID = UseList.Index[item.itemnum]->craft;
            craft_level = UseList.Index[item.itemnum]->craftlevel;
        }
        break;
        case 11:
        {
            //JEM
            //success = JemList.Index[item.itemnum]->craft_difficult;
            craftID = JemList.Index[item.itemnum]->craft;
            craft_level = JemList.Index[item.itemnum]->craft_level;
        }
        break;
		case 12:
		{
			//natural material
			//success = NaturalList.Index[item.itemnum]->craft_difficult;
            craftID = NaturalList.Index[item.itemnum]->craft;
            craft_level = NaturalList.Index[item.itemnum]->craft_level;
		}
        case 14:
        {
            //PAT
            //success = PatList.Index[item.itemnum]->craft_difficult;
            item.sp_value = item.lifespan * 10;
            craftID = PatList.Index[item.itemnum]->craft;
            craft_level = PatList.Index[item.itemnum]->craft_level;

            //LMA: Let's check if it's a painting job.
            for(char used = 5; used != 13; used += 2)
            {
                WORD material= GETWORD((*P), used);
                if(CheckInventorySlot(thisclient, material))
                {
                    //PAT painting: 12::442
                    if (thisclient->items[material].itemnum == 442 && thisclient->items[material].itemtype == 12)
                    {
                        painting = true;
                        break;
                    }

                }

            }

        }
        break;
        default:
        {
            //Equiplist.
            if(item.itemtype >= 1 && item.itemtype <= 9)
            {
                //success = EquipList[item.itemtype].Index[item.itemnum]->craft_difficult;
                craftID = EquipList[item.itemtype].Index[item.itemnum]->craft;
                craft_level = EquipList[item.itemtype].Index[item.itemnum]->craft_level;
            }
            else
            {
                Log(MSG_HACK,"Incorrect Itemtype in craft %i, player %s",item.itemtype,thisclient->CharInfo->charname);
                return true;
            }

        }
        break;
    }

    //LMA: Searching for the good skill in player's skill list.
    if(craftID == 0 || craft_level == 0)
    {
        Log(MSG_HACK,"Player %s tried to craft item %i::%i with incorrect craft values %i,%i",thisclient->CharInfo->charname,item.itemtype,item.itemnum,craftID,craft_level);
        return true;
    }

    bool skill_found = true;

    //LMA: no MP taken and no failure when painting.
    if(!painting)
    {
        skill_found = false;

        for (int k=0;k<MAX_ALL_SKILL;k++)
        {
            if(thisclient->cskills[k].thisskill == NULL || thisclient->cskills[k].thisskill->skilltype != 2)  //make sure it's a crafting skill
            {
                continue;
            }

            if(thisclient->cskills[k].thisskill->atkpower == craftID && thisclient->cskills[k].level >= craft_level)
            {
                skill_found = true;
                //LMA: checking MP amount.
                for (int j=0;j<2;j++)
                {
                    if(thisclient->cskills[k].thisskill->costtype[j] == 0 || thisclient->cskills[k].thisskill->costamount[j] == 0)
                    {
                        continue;
                    }

                    if(thisclient->cskills[k].thisskill->costtype[j] == A_MP)
                    {
                        if(thisclient->Stats->MP < thisclient->cskills[k].thisskill->costamount[j])	//failure due to insufficient MP
                        {
                            Log(MSG_WARNING,"Player %s tried to craft item %i::%i, craft values %i,%i, not enough MP",thisclient->CharInfo->charname,item.itemtype,item.itemnum,craftID,craft_level);
                            //not enough MP
                            BEGINPACKET( pak, 0x07d8);
                            ADDWORD( pak, thisclient->clientid );
                            ADDBYTE( pak, 0x02);					//craft/drill code 2: craft fail
                            ADDBYTE( pak, 0x00);					//item type. Not used in client
							ADDWORD( pak, 0x0000);					//item number. not used in client
                            thisclient->client->SendPacket( &pak );
                            return true;
                        }
                        thisclient->Stats->MP -= thisclient->cskills[k].thisskill->costamount[j];
                    }
                    else if(thisclient->cskills[k].thisskill->costtype[j] == A_HP)
                    {
                        if(thisclient->Stats->HP < thisclient->cskills[k].thisskill->costamount[j])
                        {
                            Log(MSG_WARNING,"Player %s tried to craft item %i::%i, craft values %i,%i, not enough HP",thisclient->CharInfo->charname,item.itemtype,item.itemnum,craftID,craft_level);
                            //not enough HP
                            BEGINPACKET( pak, 0x07d8);
                            ADDWORD( pak, thisclient->clientid );
                            ADDBYTE( pak, 0x02);
                            ADDBYTE( pak, 0x00);
							ADDWORD( pak, 0x0000);
                            thisclient->client->SendPacket( &pak );
                            return true;
                        }

                        thisclient->Stats->HP -= thisclient->cskills[k].thisskill->costamount[j];
                    }
                    else
                    {
                        Log(MSG_WARNING,"Player %s tried to craft item %i::%i, craft values %i,%i, unknown costype %i",thisclient->CharInfo->charname,item.itemtype,item.itemnum,craftID,craft_level,thisclient->cskills[k].thisskill->costtype[j]);
                    }
                }
                break;
            }
        }

        if (!skill_found)
        {
            Log(MSG_HACK,"Player %s tried to craft item %i::%i, craft values %i,%i but without the skill...",thisclient->CharInfo->charname,item.itemtype,item.itemnum,craftID,craft_level);
            BEGINPACKET( pak, 0x07d8);
            ADDWORD( pak, thisclient->clientid );
            ADDBYTE( pak, 0x05);						//5 = Drill Fail. WTF?? shouldn't this be a 2
            ADDBYTE( pak, 0x00);
			ADDWORD( pak, 0x0000);
            thisclient->client->SendPacket( &pak );
            return true;
        }

        //end of search.

        //LMA: we got now a formula according to player's lvl, con and sen...
        //very basic one in the meantime...
        //LMA: trying to smooth things for very hard items, hardened and so on which are like 600+...
        if (success > 100)
        {
            //Adding an extra% depending on extra diff.
            //like 816 will give: "8" and failure "116" and then 80% more of 116.
            //116 will give: "1" and failure "116" and 10% of 116.
            //int extra_diff = (int)(failure/100);
            //failure = failure - 100 * (extra_diff - 1);
            //failure += (int)((failure * extra_diff * 10) / 100);
			success = 100;
        }

        //success -= (int) (thisclient->GetCon() / 40);
        //success -= (int) (thisclient->GetSen() / 40);
        //success -= (int) (thisclient->Stats->Level / 40);
        //success -= (int) (thisclient->pc_craft_talent);   //craft talent.

        

        //LMA: Failure.
        //bool test_fail = false;
        //test_fail=true;

        if((GServer->RandNumber(0,100) >= success))	//PY can't ever happen now since success is defined as 100
        {
            Log(MSG_INFO,"Craft has failed");		

            BEGINPACKET( pak, 0x07d8);
            ADDWORD( pak, thisclient->clientid );
            ADDBYTE( pak, 0x00);					//0 = craft started. Strange thing to send for a failure. Should be 2
            ADDBYTE( pak, item.itemtype);
            ADDWORD( pak, item.itemnum);			// item id not shifted
            thisclient->client->SendPacket(&pak);

            RESETPACKET( pak, 0x07af);
            ADDBYTE( pak, 0x01);

            int nb_items = 0;
            int nb_fails = 0;
            for(char used = 5; used != 13; used += 2)
            {
                if(GETWORD((*P), used) != 0)
                {
                    nb_items++;
                }

            }

            nb_fails=GServer->RandNumber(0,nb_items);

            if(nb_fails>(nb_items-1))
            {
                nb_fails=nb_items-1;
            }

            if(nb_fails<0)
            {
                nb_fails=0;
            }

            ADDWORD( pak, nb_fails);

            int bar[4];
            bar[0] = item.durability * 9;
            bar[1] = changeofstatsrange * 9;
            bar[2] = item.durability + changeofstatsrange * 6;
            bar[3] = item.durability + changeofstatsrange + bar[2] / 3;

            nb_items=0;
            for(char used=5; used != 13; used +=2)
            {
                if(GETWORD((*P), used)!=0&&nb_items<=nb_fails)
                {
                    ADDWORD( pak, bar[nb_items]);
                }
                else
                {
                    ADDWORD( pak, 0x99a0);
                }

                nb_items++;
            }

            //LMA: change in packet (2010/08/02), naRose client 290+
            ADDWORD( pak, 0xcc);
            ADDWORD( pak, 0xcc);
            ADDWORD( pak, 0xcc);
            ADDWORD( pak, 0xcc);
            //LMA: end of change.

            ADDDWORD(pak, 0xCD48FB40);
            ADDDWORD(pak, 0x00CD01CD);
            ADDDWORD( pak, 0x00000000 );
            ADDWORD ( pak, 0x03E8 );
            thisclient->client->SendPacket( &pak );

            //deleting first item in player's inventory.
            int	materialnumber = 0;
            if(item.itemtype<10)
            {
                materialnumber = EquipList[item.itemtype].Index[item.itemnum]->material;
            }
            else
            {
                switch(item.itemtype)
                {
                    //UseList by core
                    case 10:materialnumber = UseList.Index[item.itemnum]->material;break;
                    case 11:materialnumber = JemList.Index[item.itemnum]->material;break;
                    case 14:materialnumber = PatList.Index[item.itemnum]->material;break;
                }
            }

            nb_items=0;
            int	m = 0;
            for(char used=5; used != 13; used +=2)
            {
                if(nb_items>nb_fails)
                {
                    break;
                }

                WORD material= GETWORD((*P), used);
                if (material != 0)
                {
                    if(!CheckInventorySlot( thisclient, material))
                    {
                        Log(MSG_HACK, "Player: %s, can't delete items in craft failure0 slot %i", thisclient->CharInfo->charname,material);
                        return true;
                    }

                    if (thisclient->items[material].count < ProductList.Index[materialnumber]->amount[m])
                    {
                        Log(MSG_HACK, "Player: %s, can't delete items in craft failure1", thisclient->CharInfo->charname);
                        return true;
                    }

                    UINT clientMat = (thisclient->items[material].itemtype * 1000) + thisclient->items[material].itemnum;

                    if (ProductList.Index[materialnumber]->item[m]>0&&clientMat != ProductList.Index[materialnumber]->item[m])
                    {
                        Log(MSG_HACK, "Player: %s, can't delete items in craft failure2", thisclient->CharInfo->charname);
                        return true;
                    }

                    //LMA: The first material can be generic.
                    if(m==0&&ProductList.Index[materialnumber]->item[m]==0&&ProductList.Index[materialnumber]->item_0_family!=0)
                    {
                        if(ReturnItemType(thisclient,material)!=ProductList.Index[materialnumber]->item_0_family)
                        {
                            Log(MSG_HACK, "Player: %s, can't delete items in craft failure3 (incorrect type)", thisclient->CharInfo->charname);
                            return true;
                        }

                    }

                    thisclient->items[material].count -= ProductList.Index[materialnumber]->amount[m];
                    m++;

                    if (thisclient->items[material].count <=0)
                    {
                        ClearItem(thisclient->items[material]);
                    }

                }

                nb_items++;
            }

            return true;
        }

    }

	// stats set
	item.socketed = 0;
	item.gem = 0;

	unsigned newslot= thisclient->GetNewItemSlot( item );
	if (newslot !=0xffff)
    {
		//get material used lookup number
		int	materialnumber = 0;
		if(item.itemtype<10)
		{
            materialnumber = EquipList[item.itemtype].Index[item.itemnum]->material;
        }
        else
        {
            switch(item.itemtype)
            {
                //UseList by core
                case 10:materialnumber = UseList.Index[item.itemnum]->material;break;
                case 11:materialnumber = JemList.Index[item.itemnum]->material;break;
                case 14:materialnumber = PatList.Index[item.itemnum]->material;break;
            }
        }

        //LMA: We have the material number, we try to get the numbers of materials expected.
        UINT mat_needed[4];
        int nb_mat_needed=0;
        int nb_items_packet=0;
        for(int j=0;j<4;j++)
        {
            mat_needed[j]=0;

            if(ProductList.Index[materialnumber]->amount[j]>0)
            {
                nb_mat_needed++;
            }

        }

        //checking if we have the nb of slots expected.
		for(char used=5; used != 13; used +=2)
        {
            WORD material= GETWORD((*P), used);
            if (material<=0)
            {
                continue;
            }

            nb_items_packet++;
        }

        if (nb_items_packet!=nb_mat_needed)
        {
            Log(MSG_HACK,"Craft:: Player %s tried to craft %u::%u receipe %u but needs %i materials and packet only sent %i",thisclient->CharInfo->charname,item.itemtype,item.itemnum,materialnumber,nb_mat_needed,nb_items_packet);
            return true;
        }
        //LMA end


		int	m = 0;

		//LMA: TEST fix 4th tab by Shakar
		//for(char used=5; used != 11; used +=2)
		for(char used=5; used != 13; used +=2)
        {
            WORD material= GETWORD((*P), used);//gets inventory location
            if (material != 0)
            {
                if(!CheckInventorySlot( thisclient, material))
                {
                    BEGINPACKET( pak, 0x07d8);
                    ADDWORD( pak, thisclient->clientid );
                    ADDBYTE( pak, 0x03);
                    ADDBYTE( pak, 0x00);
                    thisclient->client->SendPacket( &pak );
                    return true;
                }

                if (thisclient->items[material].count < ProductList.Index[materialnumber]->amount[m])
                {
                    Log(MSG_HACK, "Nb Client craft mats don't equal server. Player: %s", thisclient->CharInfo->charname);
                    BEGINPACKET( pak, 0x07d8);
                    ADDWORD( pak, thisclient->clientid );
                    ADDBYTE( pak, 0x03);
                    ADDBYTE( pak, 0x00);
                    thisclient->client->SendPacket( &pak );
                    return true;
                }

                UINT clientMat = (thisclient->items[material].itemtype * 1000) + thisclient->items[material].itemnum;
                //Log(MSG_HACK, "Clientmat: %i | Productlist: %i | materialnumber: %i | material: %i", clientMat, ProductList.Index[materialnumber]->item[m], materialnumber, material);

                //LMA: sometimes there is only a check on the amount.
                if (ProductList.Index[materialnumber]->item[m]>0&&clientMat != ProductList.Index[materialnumber]->item[m])
                {
                    Log(MSG_HACK, "Client craft mats don't equal server. Player: %s", thisclient->CharInfo->charname);
                    BEGINPACKET( pak, 0x07d8);
                    ADDWORD( pak, thisclient->clientid );
                    ADDBYTE( pak, 0x03);
                    ADDBYTE( pak, 0x00);
                    thisclient->client->SendPacket( &pak );
                    return true;
                }

                //LMA: The first material can be generic.
                if(m==0&&ProductList.Index[materialnumber]->item[m]==0&&ProductList.Index[materialnumber]->item_0_family!=0)
                {
                    if(ReturnItemType(thisclient,material)!=ProductList.Index[materialnumber]->item_0_family)
                    {
                        Log(MSG_HACK, "Player: %s, can't delete items in craft failure3 (incorrect type)", thisclient->CharInfo->charname);
                        BEGINPACKET( pak, 0x07d8);
                        ADDWORD( pak, thisclient->clientid );
                        ADDBYTE( pak, 0x03);
                        ADDBYTE( pak, 0x00);
                        thisclient->client->SendPacket( &pak );
                        return true;
                    }

                }

                thisclient->items[material].count -= ProductList.Index[materialnumber]->amount[m];
                m++;

                if (thisclient->items[material].count <=0)
                {
                    ClearItem(thisclient->items[material]);
                }

            }

        }

        //LMA: Stackable fix.
        if (item.itemtype>=10&&item.itemtype<=12)
        {
            item.count+=thisclient->items[newslot].count;
        }

		thisclient->items[newslot] = item;

        BEGINPACKET( pak, 0x07d8);
        ADDWORD( pak, thisclient->clientid );

        //LMA: Itemtype?
        //ADDWORD( pak, 0x0100);
        ADDBYTE( pak, 0x00);
        ADDBYTE( pak, item.itemtype);

        ADDWORD( pak, item.itemnum);// item id not shifted
        thisclient->client->SendPacket(&pak);

        RESETPACKET( pak, 0x07af);
        ADDBYTE( pak, 0x00);//00
        ADDWORD( pak, newslot);

        // Make craft bars
        int bar1 = item.durability * 9;
        int bar2 = changeofstatsrange * 9;
        int bar3 = item.durability + changeofstatsrange * 6;
        int bar4 = item.durability + changeofstatsrange + bar3 / 3;

        //ADDWORD( pak, bar1);//progress bar1 0 is empty 0x0400 is full bar
        //ADDWORD( pak, bar2);//progress bar2 0 is empty 0x0400 is full bar
        if ((GETWORD((*P),  5))==0){ ADDWORD( pak, 0x99a0);}else{ ADDWORD( pak, bar1);}
        if ((GETWORD((*P),  7))==0){ ADDWORD( pak, 0x99a0);}else{ ADDWORD( pak, bar2);}
        if ((GETWORD((*P),  9))==0){ ADDWORD( pak, 0x99a0);}else{ ADDWORD( pak, bar3);}//progress bar3 0 is empty 0x0400 is full bar
        if ((GETWORD((*P), 11))==0){ ADDWORD( pak, 0x99a0);}else{ ADDWORD( pak, bar4);}//progress bar4 0 is empty 0x0400 is full bar

        //LMA: change in packet (2010/08/02), naRose client 290+
        ADDWORD( pak, 0xcc);
        ADDWORD( pak, 0xcc);
        ADDWORD( pak, 0xcc);
        ADDWORD( pak, 0xcc);
        //LMA: end of change.

        //ADDDWORD(pak, BuildItemHead(item));
		ADDBYTE(pak, item.itemtype );
		ADDWORD(pak, item.itemnum );
		ADDBYTE(pak, item.isCreated );
		item.count = 1;
		pak = AddItemData( item, pak );
        thisclient->client->SendPacket(&pak);

        //LMA: No Exp for painting.
        if(!painting)
        {
            //LMA: stat bonus according to item stat (basic formula)...
            UINT crafting_exp = item.durability + changeofstatsrange * (thisclient->Stats->Level/ 15);
            crafting_exp += (UINT)((100000*item.stats)/300);
            thisclient->CharInfo->Exp += crafting_exp;//  add exp

            //LMA: Only if not level up
            if(thisclient->CharInfo->Exp<thisclient->GetLevelEXP())
            {
                BEGINPACKET( pak, 0x79b );
                ADDDWORD   ( pak, thisclient->CharInfo->Exp );
                ADDWORD    ( pak, thisclient->CharInfo->stamina );
                ADDWORD    ( pak, 0 );
                thisclient->client->SendPacket( &pak );
            }

        }

     }
     else
     {
        BEGINPACKET( pak, 0x07d8);
        ADDWORD( pak, thisclient->clientid );
        ADDBYTE( pak, 0x03);
        ADDBYTE( pak, 0x00);
        thisclient->client->SendPacket( &pak );

         RESETPACKET (pak, 0x702);
         ADDSTRING(pak, "No free slot !");
         ADDBYTE(pak, 0);
         thisclient->client->SendPacket(&pak);
     }
	 thisclient->saveinventory();

     return true;
}
*/

///////////////////////////////////////////////////////////////////////////////////
// After Craft  (From RageZone)  (if you coded this tell me to add your name)    //
///////////////////////////////////////////////////////////////////////////////////
bool CWorldServer::pakModifiedItemDone( CPlayer* thisclient, CPacket* P )
{
     DWORD result = GETDWORD((*P), 0x00 );
     BEGINPACKET( pak, 0x07d8);
     ADDWORD( pak, thisclient->clientid );
     ADDDWORD( pak, result);
     SendToVisible( &pak , thisclient);
     return true;

}

// Aoe Skill
bool CWorldServer::pakSkillAOE( CPlayer* thisclient, CPacket* P)
{
    Log(MSG_INFO,"[pakSkillAOE] AOE_TARGET");

    if( thisclient->Shop->open || thisclient->Status->Stance==DRIVING ||
        thisclient->Status->Mute!=0xff || !thisclient->Status->CanCastSkill) return true;
    UINT num = GETWORD( (*P), 0 );

    //LMA: position of monster targeted (in fact it's a zone, monster is just a mean to an end).
    thisclient->Position->aoedestiny.x=GETFLOAT((*P), 0x02 )/100;
    thisclient->Position->aoedestiny.y=GETFLOAT((*P), 0x06 )/100;
    thisclient->Position->aoedestiny.z=0;

    //Log(MSG_INFO,"AOE Current %.2f,%.2f",thisclient->Position->current.x,thisclient->Position->current.y);
    //Log(MSG_INFO,"[pakSkillAOE] num=%i,x=%.2f, y=%.2f",num,thisclient->Position->aoedestiny.x,thisclient->Position->aoedestiny.y);
    if(num>=MAX_ALL_SKILL)
    {
        Log( MSG_HACK, "Invalid Skill id %i for %s ", num, thisclient->CharInfo->charname );
        return false;
    }

    Log( MSG_INFO, "pakSkillAOE for %s (%i)", thisclient->CharInfo->charname,num);

    unsigned int skillid = thisclient->cskills[num].id+thisclient->cskills[num].level-1;
    //Log(MSG_INFO,"[pakSkillAOE] skillid=%i",skillid);
    CSkills* thisskill = GetSkillByID( skillid );
    if(thisskill==NULL) return true;

    if(thisskill->aoe==1)
    {
        //LMA: time to check the cooldown time.
        time_t etime=time(NULL);
        if (etime<thisclient->cskills[num].cooldown_skill)
        {
            Log(MSG_HACK,"Player %s tried to pakSkillAOE skill %u but cooldown wasn't finished %u<%u",thisclient->CharInfo->charname,skillid,etime,thisclient->cskills[num].cooldown_skill);
            return true;
        }

        //checking if player has enough MP too.
        //LMA: Special test, it seems sometimes the MP value is off after a level up??
        if(thisclient->Stats->MP>thisclient->Stats->MaxMP)
        {
            thisclient->Stats->MP=thisclient->Stats->MaxMP;
        }

        UINT needed_mp=(thisskill->mp - (thisskill->mp * thisclient->Stats->MPReduction / 100));
        if (thisclient->Stats->MP<needed_mp)
        {
            Log(MSG_HACK,"Player %s tried to pakSkillAOE skill %u but didn't have enough MP %u<%u",thisclient->CharInfo->charname,skillid,thisclient->Stats->MP,needed_mp);
            return true;
        }

        //We set the time the player can cast this skill next time.
        thisclient->cskills[num].cooldown_skill=etime+thisskill->cooldown;
        Log(MSG_WARNING,"Next pakSkillAOE skill %u available at %u (is %u)",skillid,thisclient->cskills[num].cooldown_skill,etime);

        thisclient->StartAction( NULL , AOE_TARGET, skillid );
    }


    return true;
}

// Identify Item
bool CWorldServer::pakidentify( CPlayer* thisclient, CPacket* P)
{
    WORD itemslot = GETWORD ((*P),0);
    if(!CheckInventorySlot( thisclient, itemslot))
        return false;
    thisclient->items[itemslot].appraised = true;
    BEGINPACKET( pak, 0x7ba );
    ADDWORD    ( pak, itemslot );
    ADDBYTE    ( pak, 0x00 );
    thisclient->client->SendPacket( &pak );
    return true;
}

// Show Storage Items
bool CWorldServer::pakStorage( CPlayer* thisclient, CPacket* P)
{
    BYTE action = GETBYTE((*P),0);
	Log( MSG_INFO, "Storage request for action: %i ", action);
    switch(action) //PY. Client has a bunch of different action codes for this. 0 initializes, 1 displays, 2 needs a password, 6 is for Platinum and so on
    {
        case 0x00:
        {
            BEGINPACKET( pak, 0x7ad );
            ADDBYTE    ( pak, 0x00 );	
            ADDBYTE    ( pak, thisclient->nstorageitems ); //number of items
            for(int i=0;i<160;i++)
            {
           		if( thisclient->storageitems[i].itemtype != 0 )
           		{
                    ADDBYTE    ( pak, i );
					pak = AddItemData(thisclient->storageitems[i], pak);
                }
            }
            ADDQWORD( pak, thisclient->CharInfo->Storage_Zulies );
            thisclient->client->SendPacket( &pak );
        }
        break;
        default:
            Log( MSG_INFO, "Storage unknown action: %i ", action);
    }
    return true;
}

// Change Storage (Deposit/Withdraw items)
//2do: take Zulyes from player's money when getting / putting items from / into storage.
bool CWorldServer::pakChangeStorage( CPlayer* thisclient, CPacket* P)
{
    BYTE action = GETBYTE((*P),0);
    switch(action)
    {
        case 0x00: //Deposit
        {
            BYTE itemslot = GETBYTE((*P),1);
			
            if(!CheckInventorySlot( thisclient, itemslot ))
                return false;

            CItem newitem = thisclient->items[itemslot];
			Log(MSG_INFO,"Depositing an item. slot %i type: %i number: %i",itemslot,newitem.itemtype,newitem.itemnum);
 			//Maxxon: Deposit Fee
            int storageprice = 0;
            switch (newitem.itemtype) 
			{
                case 1:
                case 2:
                case 3:
                case 4:
                case 5:
                case 6:
                case 7:
                case 8:
                case 9:
				{
					// EQ
					CEquip* neweq = EquipList[newitem.itemtype].Index[newitem.itemnum];
					if (neweq == NULL) 
					{
						storageprice = -1;
					} else 
					{
						storageprice = (neweq->price / 200 + 1) * (neweq->pricerate + 1);
					}
				}
                break;
                case 10:
				{
					// consumes
					CUseData* newuse = UseList.Index[newitem.itemnum];
					if (newuse == NULL) 
					{
						storageprice = -1;
					} 
					else 
					{
						storageprice = GETWORD((*P),6) * ((newuse->price / 200 + 1) * (newuse->pricerate + 1));
					}
				}
                break;
                case 11:
				{
					// gems
					CJemData* thisjem = JemList.Index[newitem.itemnum];
					if (thisjem == NULL) 
					{
						storageprice = -1;
					} 
					else 
					{
						storageprice = GETWORD((*P),6) * ((thisjem->price / 200 + 1) * (thisjem->pricerate + 1));
					}
				}
                break;
                case 12:
				{
					// mats
					CNaturalData* newnatural = NaturalList.Index[newitem.itemnum];
					if (newnatural == NULL) 
					{
						storageprice = -1;
					} 
					else 
					{
						storageprice = GETWORD((*P),6) * ((newnatural->price / 200 + 1) * (newnatural->pricerate + 1));
					}
				}
                break;
                case 14:
				{
					// PAT
					CPatData* newpat = PatList.Index[newitem.itemnum];
					if (newpat == NULL) 
					{
						storageprice = -1;
					} 
					else 
					{
						storageprice = (newpat->price / 200 + 1) * (newpat->pricerate + 1);
					}
				}
                break;
                default:
                    Log (MSG_ERROR, "pakChangeStorage: item type %i unknown!", newitem.itemtype);
                    storageprice = -1;
                break;
            }

            if (storageprice < 0) 
			{
                Log(MSG_HACK, "pakChangeStorage: %s tried to hack your server slot %i, price %i",thisclient->CharInfo->charname,itemslot,storageprice);
                return false;
            }

            if (storageprice > thisclient->CharInfo->Zulies)
            {
                // not enough zulies
                return true;
            }
			//End of deposit Fee.

            int newslot = 0;
            if(newitem.itemtype > 9 && newitem.itemtype < 14)
            {
                WORD count = GETWORD((*P),6);
                if(count>thisclient->items[itemslot].count)
                    count = thisclient->items[itemslot].count;
                newitem.count = count;

                newslot = thisclient->GetNewStorageItemSlot ( newitem );
                if(newslot == 0xffff)
                    return true;

                thisclient->items[itemslot].count -= count;
                if(thisclient->items[itemslot].count <= 0)
                    ClearItem(thisclient->items[itemslot]);
            }
            else
            {
                newslot = thisclient->GetNewStorageItemSlot ( newitem );
                if(newslot == 0xffff)
                    return true;

                ClearItem(thisclient->items[itemslot]);
            }

            thisclient->CharInfo->Zulies -= storageprice;
            //Log(MSG_INFO,"New (?) slot for deposit: %i",newslot);

            //LMA: New code (stackables?)
            if (thisclient->storageitems[newslot].itemnum != 0)
            {
               //Log(MSG_INFO,"it should be a stackable in slot: %i, from %i (+%i)",newslot,thisclient->storageitems[newslot].count,newitem.count);
               newitem.count += thisclient->storageitems[newslot].count;
               //Log(MSG_INFO,"so new=%i",newitem.count);
            }
            else
            {
                //Log(MSG_INFO,"it should be a new slot: %i",newslot);
                thisclient->nstorageitems++;
            }

            BEGINPACKET( pak, 0x7ae );
            ADDWORD    ( pak, itemslot );		//inv slot
            ADDWORD    ( pak, newslot );		//bank slot
			pak = AddItemData(thisclient->items[itemslot], pak);
			pak = AddItemData(newitem, pak);
    		ADDQWORD   ( pak, thisclient->CharInfo->Zulies );
            thisclient->client->SendPacket( &pak );

            thisclient->storageitems[newslot] = newitem;
			thisclient->saveinventory();
			thisclient->savestorage();
        }
        break;//thanks to anon for post that this break was missing
        case 0x01: //Withdraw
        {
            BYTE storageslot = GETBYTE((*P),1);
            if(storageslot >= 160)
            {
                Log( MSG_HACK, "%s, Invalid storage slot %i from %s",thisclient->CharInfo->charname, storageslot, thisclient->Session->username );
                return false;
            }

            //LMA: get the slot concerned to refresh it from MySQL storage
            if(!GetSlotStorage(thisclient,storageslot))
            {
                Log( MSG_HACK, "%s, Invalid storage slot %i from %s (from Mysql)",thisclient->CharInfo->charname, storageslot, thisclient->Session->username );
                return false;
            }

            //CItem newitem =  newitem = thisclient->storageitems[storageslot];
            CItem newitem = thisclient->storageitems[storageslot];
			Log(MSG_INFO,"Withdrawing an item from storage. slot %i type: %i number: %i count: %i",storageslot,newitem.itemtype,newitem.itemnum, newitem.count);
            if(newitem.itemtype>9 && newitem.itemtype<14)
            {
                WORD count = GETWORD((*P),6);
                if( count>thisclient->storageitems[storageslot].count )
                    count = thisclient->storageitems[storageslot].count;
                newitem.count = count;
                thisclient->storageitems[storageslot].count -= count;
                if(thisclient->storageitems[storageslot].count<=0)
                    ClearItem(thisclient->storageitems[storageslot]);
            }
            else
            {
                ClearItem(thisclient->storageitems[storageslot]);
            }

            int newslot= thisclient->GetNewItemSlot ( newitem );
            //no place in player's inventory, so back to storage.
            if(newslot == 0xffff)
            {
                thisclient->storageitems[storageslot] = newitem;
                return true;
            }

            int amount = 0;
            if(thisclient->items[newslot].count > 0)
            {
              int amount = thisclient->items[newslot].count;
			  newitem.count+=amount;
            }
            if( newitem.count > 999 )
            {
                amount = 999 - newitem.count;
                newitem.count = 999;
            }
            thisclient->items[newslot] = newitem;
            if( amount > 0 )
            {
                newitem.count = amount;
                unsigned int newslot2 = thisclient->GetNewItemSlot( newitem );
                if( newslot2 == 0xffff )
                {
                    thisclient->storageitems[storageslot] = thisclient->items[newslot];
                    thisclient->items[newslot].count = amount;
                    return true;
                }
                thisclient->items[newslot2] = newitem;
                thisclient->UpdateInventory( newslot2 );
            }
            BEGINPACKET( pak, 0x7ae );
            ADDWORD    ( pak, newslot );		//inv slot
            ADDWORD    ( pak, storageslot );	//bank slot
			pak = AddItemData(thisclient->items[newslot], pak);
			pak = AddItemData(thisclient->storageitems[storageslot], pak);
    		ADDQWORD   ( pak, thisclient->CharInfo->Zulies );
            ADDBYTE    ( pak, 0x00 );

            thisclient->client->SendPacket( &pak );

            if(thisclient->storageitems[storageslot].itemnum == 0)
                thisclient->nstorageitems--;

            //LMA: need to save the storage item...
            //We save the inventory slot as well
			thisclient->saveinventory();
			thisclient->savestorage();
			//TO DO. Send another packet to update newslot2 for when stackables overflow. probably a 0x07ad packet

        }
        break;
        default:
            Log( MSG_INFO, "Storage unknown action: %i ", action);
    }
    return true;
}

// Deposit/Widthdraw zulies
bool CWorldServer::pakStoreZuly( CPlayer* thisclient, CPacket* P)
{
    BYTE action = GETBYTE ((*P), 0 );
    QWORD zuly = GETQWORD ((*P), 1 );
    switch(action)
    {
        case 0x10://deposit'
        {
            if(zuly > thisclient->CharInfo->Zulies)
                return true;
            thisclient->CharInfo->Zulies -= zuly;
            thisclient->CharInfo->Storage_Zulies += zuly;
            BEGINPACKET( pak, 0x7da );
            ADDQWORD   ( pak, thisclient->CharInfo->Zulies );
            ADDQWORD   ( pak, thisclient->CharInfo->Storage_Zulies );
            ADDBYTE    ( pak, 0x00 );
            thisclient->client->SendPacket( &pak );

            //LMA: Saving Zuly Storage
            SaveZulyStorage(thisclient);
        }
        break;
        case 0x11://withdraw
        {
             //LMA: refreshing Zuly from MySQL...
             if(!GetZulyStorage(thisclient))
                   return true;
            if(zuly > thisclient->CharInfo->Storage_Zulies )
                return true;
            thisclient->CharInfo->Zulies += zuly;
            thisclient->CharInfo->Storage_Zulies -= zuly;
            BEGINPACKET( pak, 0x7da );
            ADDQWORD   ( pak, thisclient->CharInfo->Zulies );
            ADDQWORD   ( pak, thisclient->CharInfo->Storage_Zulies );
            ADDBYTE    ( pak, 0x00 );
            thisclient->client->SendPacket( &pak );

            //LMA: Saving Zuly Storage
            SaveZulyStorage(thisclient);
        }
        break;
        default:
            Log( MSG_INFO, "Storage unknown action: %i ", action);
    }
    return true;
}


// Open Shop
bool CWorldServer::pakOpenShop( CPlayer* thisclient, CPacket* P )
{
     //LMA 139+:
     //They added six extra 0x00 between items and prices + shop name...
     UINT lma_offset=6;

    //You can't open a shop when you have already one or you're driving.
    if(thisclient->Ride->Drive||thisclient->Shop->open)
    {
        return true;
    }

    BYTE nselling = GETBYTE((*P),0);
    BYTE nbuying = GETBYTE((*P),1);
    if(nselling>30 || nbuying>30)
        return true;
    int nchar = ((nselling + nbuying) * (12+lma_offset) ) + nselling + nbuying + 2;
    strncpy(thisclient->Shop->name ,(char*)&(*P).Buffer[nchar] , P->Size-nchar );

    Log(MSG_INFO,"[Create] Begin Shop %s:",thisclient->Shop->name);
    thisclient->Shop->Selling = nselling;
    thisclient->Shop->Buying = nbuying;
    for(int i=0;i<nselling;i++)
    {
        int n=(i*(13+lma_offset))+2;
        BYTE slot =  GETBYTE((*P),n);
        if(!CheckInventorySlot( thisclient, slot))
            return false;
        thisclient->Shop->SellingList[i].slot = slot;
        if( thisclient->items[slot].itemtype>9 && thisclient->items[slot].itemtype<14 )
        {
            int itemCount = GETWORD((*P),n+5);
            if (thisclient->items[slot].count < itemCount)
               return true;
            thisclient->Shop->SellingList[i].count = itemCount; // geo edit for invisible stackables // 30 sep 07
        }
        else
            thisclient->Shop->SellingList[i].count = 1;
        thisclient->Shop->SellingList[i].price = GETDWORD((*P),n+9+lma_offset);

        //LMA Log:
        Log(MSG_INFO,"[S-%i/%i], Item (%i:%i), slot %i, Nb %i, Price %i",i+1,nselling,thisclient->items[slot].itemtype,thisclient->items[slot].itemnum,slot,thisclient->Shop->SellingList[i].count,thisclient->Shop->SellingList[i].price);
    }
    for(int i=0;i<nbuying;i++)
    {
        unsigned int n=(nselling*(13+lma_offset))+2+(i*(12+lma_offset))+i;
        BYTE slot = GETBYTE((*P),n);
        thisclient->Shop->BuyingList[slot].slot = slot;
        DWORD head = GETDWORD((*P),n+1);
        DWORD data = GETDWORD((*P),n+5);
        CItem thisitem = GetItemByHeadAndData( head, data );
        thisclient->Shop->BuyingList[slot].item = thisitem;
        if( thisitem.itemtype>9 && thisitem.itemtype<14 )
            thisclient->Shop->BuyingList[slot].count = GETWORD((*P),n+5);
        else
            thisclient->Shop->BuyingList[slot].count = 1;
        thisclient->Shop->BuyingList[slot].price = GETDWORD((*P),n+9+lma_offset);

        //LMA Log:
        Log(MSG_INFO,"[B-%i/%i], Item (%i:%i), slot %i, Nb %i, Price %i",i+1,nbuying,thisitem.itemtype,thisitem.itemnum,slot,thisclient->Shop->BuyingList[slot].count,thisclient->Shop->BuyingList[slot].price);
    }

    Log(MSG_INFO,"[Create] End Shop %s.",thisclient->Shop->name);

    BEGINPACKET( pak, 0x796 );
    ADDWORD    ( pak, thisclient->clientid );
    ADDFLOAT   ( pak, thisclient->Position->current.x );
    ADDFLOAT   ( pak, thisclient->Position->current.y );
    ADDWORD    ( pak, 0x9057 );//>???
    SendToVisible( &pak, thisclient );

    RESETPACKET( pak, 0x7c2 );
    ADDWORD    ( pak, thisclient->clientid );
    ADDWORD    ( pak, thisclient->Shop->ShopType );
    ADDSTRING  ( pak, thisclient->Shop->name );
    ADDBYTE    ( pak, 0x00 );
    SendToVisible( &pak, thisclient );
    thisclient->Shop->open = true;
    return true;
}


// Show Shop to other players
bool CWorldServer::pakShowShop( CPlayer* thisclient, CPacket* P )
{
    WORD otherclientid = GETWORD((*P),0);
    CPlayer* otherclient = GetClientByID ( otherclientid, thisclient->Position->Map );
    if(otherclient==NULL)
        return true;
    BEGINPACKET( pak, 0x7c4 );
    ADDBYTE    ( pak, otherclient->Shop->Selling );
    ADDBYTE    ( pak, otherclient->Shop->Buying );
    for(unsigned int i = 0; i<otherclient->Shop->Selling;i++)
    {
        CItem thisitem =  otherclient->items[otherclient->Shop->SellingList[i].slot];
        thisitem.count = otherclient->Shop->SellingList[i].count;
        ADDBYTE     ( pak, i );
		pak = AddItemData(thisitem, pak);
        ADDDWORD    ( pak, otherclient->Shop->SellingList[i].price );
    }
    for(unsigned int i = 0; i<30;i++)
    {
        if(otherclient->Shop->BuyingList[i].count>0)
        {
            CItem thisitem = otherclient->Shop->BuyingList[i].item;
            ADDBYTE     ( pak, i );
			pak = AddItemData(thisitem, pak);
            ADDDWORD    ( pak, otherclient->Shop->BuyingList[i].price );
        }
    }
    ADDBYTE    ( pak, 0x00 );
    thisclient->client->SendPacket( &pak );
    return true;
}

// Buy From player Shop
//LMA: checking for Zuly hacks
bool CWorldServer::pakBuyShop( CPlayer* thisclient, CPacket* P )
{
    WORD otherclientid = GETWORD((*P),0);
    BYTE action = GETBYTE((*P),2);
    switch(action)
    {
        case 0x01://check this
        {
            CPlayer* otherclient = GetClientByID( otherclientid, thisclient->Position->Map );
            if( otherclient==NULL )
                return true;

            //LMA: checking if a player (the seller) is in a shop (hack check).
            if (!otherclient->Shop->open)
            {
                return true;
            }

            BYTE slot = GETBYTE((*P),3);

            //LMA: checking buyer slot too.
            if(slot>=30)
            {
                Log(MSG_HACK,"%s tried to buy something from %s in wrong slot %i",thisclient->CharInfo->charname,otherclient->CharInfo->charname,slot);
                return true;
            }

            unsigned int count = 0;
            unsigned int invslot = otherclient->Shop->SellingList[slot].slot;
            CItem newitem =  otherclient->items[invslot];

            //LMA: some checks on the item sold.
            if(newitem.itemtype==0||newitem.itemtype==0||newitem.count==0)
            {
                Log(MSG_HACK,"%s tried to buy wrong item %u::%u from %s",thisclient->CharInfo->charname,newitem.itemtype,newitem.itemnum,otherclient->CharInfo->charname);
                return true;
            }

            if(otherclient->items[invslot].itemtype>9&&otherclient->items[invslot].itemtype<14)
            {
                count = GETWORD((*P),8);
            }
            else
            {
                count = 1;
            }

            //LMA: check on amount.
            if(count>otherclient->Shop->SellingList[slot].count)
            {
                Log(MSG_HACK,"%s tried to buy too many items from %s, buys %u but seller sells only %u",thisclient->CharInfo->charname,otherclient->CharInfo->charname,count,otherclient->Shop->SellingList[slot].count);
                return true;
            }

            if( count > otherclient->Shop->SellingList[slot].count )
                return true;
            newitem.count = count;
            if( thisclient->CharInfo->Zulies<(otherclient->Shop->SellingList[slot].price*count) )
                return true;
            unsigned int newslot = thisclient->GetNewItemSlot ( newitem );
            if(newslot==0xffff)
                return true;

            //LMA: check for hacks...
            if (thisclient->CharInfo->Zulies < (otherclient->Shop->SellingList[slot].price*count))
            {
              Log(MSG_HACK, "[Buy in Shop] Not enough Zuly player %s, have %li, need %li",thisclient->CharInfo->charname,thisclient->CharInfo->Zulies,otherclient->Shop->SellingList[slot].price*count);
              return true;
            }

            thisclient->CharInfo->Zulies -= (otherclient->Shop->SellingList[slot].price*count);
            otherclient->CharInfo->Zulies += (otherclient->Shop->SellingList[slot].price*count);

            //LMA: hack check (number of items).
            if(otherclient->items[invslot].count < count)
            {
                Log(MSG_WARNING,"Player %s tried to buy %u*(%u::%u) from %s but vendor had only %u in stock, Synch problem?",thisclient->CharInfo->charname,count,otherclient->items[invslot].itemtype,otherclient->items[invslot].itemnum,otherclient->CharInfo->charname,otherclient->items[invslot].count);

                //update shop
                BEGINPACKET( pak, 0x7c6 );
                ADDWORD    ( pak, otherclient->clientid );
                ADDBYTE    ( pak, 0x05 );
                ADDBYTE    ( pak, 0x01 );
                ADDBYTE    ( pak, slot );

                //We update the quantity...
                if(otherclient->items[invslot].count==0)
                {
                     otherclient->Shop->SellingList[slot].slot = 0;
                     otherclient->Shop->SellingList[slot].count = 0;
                     otherclient->Shop->SellingList[slot].price = 0;
                }
                else
                {
                    otherclient->Shop->SellingList[slot].count=otherclient->items[invslot].count;
                }

                if(otherclient->Shop->SellingList[slot].count<=0)
                {
					ADDDWORD ( pak, 0x00000000 );
					ADDDWORD ( pak, 0x00000000 );
					ADDDWORD( pak, 0x00000000 );
					ADDWORD ( pak, 0x0000 );
                }
                else
                {
					pak = AddItemData(otherclient->items[invslot], pak);
                }

                SendToVisible( &pak, otherclient );

                return true;
            }

            if(otherclient->items[invslot].itemtype>9&&otherclient->items[invslot].itemtype<14)
            {
                if(otherclient->items[invslot].count <= count)
                {
                     ClearItem(otherclient->items[invslot])
                     otherclient->Shop->SellingList[slot].slot = 0;
                     otherclient->Shop->SellingList[slot].count = 0;
                     otherclient->Shop->SellingList[slot].price = 0;
                }
                else
                {
                    if(otherclient->Shop->SellingList[slot].count<=count)
                    {
                        otherclient->Shop->SellingList[slot].slot = 0;
                        otherclient->Shop->SellingList[slot].count = 0;
                        otherclient->Shop->SellingList[slot].price = 0;
                    }
                    else
                    {
                        otherclient->Shop->SellingList[slot].count -= count;
                    }
                    otherclient->items[invslot].count-=count;
                }
            }
            else
            {
                ClearItem( otherclient->items[invslot] )
                otherclient->Shop->SellingList[slot].slot = 0;
                otherclient->Shop->SellingList[slot].count = 0;
                otherclient->Shop->SellingList[slot].price = 0;
            }
            if(thisclient->items[newslot].count!=0)
                thisclient->items[newslot].count += newitem.count;
            else
                thisclient->items[newslot] = newitem;
            //update inventory (buyer)
            BEGINPACKET( pak, 0x7c7 );
            ADDQWORD   ( pak, thisclient->CharInfo->Zulies );
            ADDBYTE    ( pak, 0x01 ); //buy action
            ADDBYTE    ( pak, newslot );
			pak = AddItemData(thisclient->items[newslot], pak);
            thisclient->client->SendPacket( &pak );


            //update inventory (seller)
            RESETPACKET( pak, 0x7c7 );
            ADDQWORD   ( pak, otherclient->CharInfo->Zulies );
            ADDBYTE    ( pak, 0x01 ); //buy action
            ADDBYTE    ( pak, invslot );
			pak = AddItemData(otherclient->items[invslot], pak);
            otherclient->client->SendPacket( &pak );

            //PY: saving inventory
            thisclient->saveinventory();
			otherclient->saveinventory();

            //update shop
            RESETPACKET( pak, 0x7c6 );
            ADDWORD    ( pak, otherclient->clientid );
            ADDBYTE    ( pak, 0x05 );
            ADDBYTE    ( pak, 0x01 );
            ADDBYTE    ( pak, slot );
            if(otherclient->Shop->SellingList[slot].count <= 0)
            {
				ADDDWORD ( pak, 0x00000000 );
				ADDDWORD ( pak, 0x00000000 );
				ADDDWORD( pak, 0x00000000 );
				ADDWORD ( pak, 0x0000 );
            }
            else
            {
				pak = AddItemData(otherclient->items[invslot], pak);
            }
            SendToVisible( &pak, otherclient );
        }
        break;
        default:
            Log( MSG_WARNING, "Buy unknown action: %i", action );
    }
    return true;
}

// Sell in Shop
bool CWorldServer::pakSellShop( CPlayer* thisclient, CPacket* P )
{
    //LMA: otherclient is in a shop and this part is the wishlist (buying list).
    //so thisclient sells an item to other client.
    WORD otherclientid = GETWORD((*P),0);
    BYTE action = GETBYTE((*P),2);
    switch(action)
    {
        case 0x01:
        {
            CPlayer* otherclient = GetClientByID( otherclientid, thisclient->Position->Map );
            if( otherclient==NULL )
                return true;

            //LMA: checking if a player (the buyer) is in a shop (hack check).
            if (!otherclient->Shop->open)
            {
                return true;
            }

            unsigned short count = 0;
            BYTE invslot = GETBYTE((*P),3);
            BYTE slot = GETBYTE((*P),4);
            if(!CheckInventorySlot( thisclient, slot))
                return false;

            //LMA: checking buyer slot too.
            if(slot>=30)
            {
                Log(MSG_HACK,"%s tried to sell something to %s in wrong slot %i",thisclient->CharInfo->charname,otherclient->CharInfo->charname,slot);
                return true;
            }

            CItem newitem =  thisclient->items[invslot];

            //LMA: some checks, is the item sent by seller is the one expected by buyer?
            if(thisclient->items[invslot].itemtype==0||thisclient->items[invslot].itemtype==0||thisclient->items[invslot].count==0)
            {
                Log(MSG_HACK,"%s tried to sell wrong item %u::%u to %s",thisclient->CharInfo->charname,thisclient->items[invslot].itemtype,thisclient->items[invslot].itemnum,otherclient->CharInfo->charname);
                return true;
            }

            if((thisclient->items[invslot].itemtype!=otherclient->Shop->BuyingList[slot].item.itemtype)||(thisclient->items[invslot].itemnum!=otherclient->Shop->BuyingList[slot].item.itemnum))
            {
                Log(MSG_HACK,"%s tried to sell %u::%u to %s who was expecting %u::%u",thisclient->CharInfo->charname,thisclient->items[invslot].itemtype,thisclient->items[invslot].itemnum,otherclient->CharInfo->charname,otherclient->Shop->BuyingList[slot].item.itemtype,otherclient->Shop->BuyingList[slot].item.itemnum);
                return true;
            }

            if(thisclient->items[invslot].itemtype>9 && thisclient->items[invslot].itemtype<14)
            {
                count = GETWORD((*P),9);
            }
            else
            {
                count = 1;
            }

            //LMA: check on amount.
            if(count > thisclient->items[invslot].count)
            {
                Log(MSG_HACK,"%s tried to sell too many items to %s, sells %u but has only %u",thisclient->CharInfo->charname,otherclient->CharInfo->charname,count,thisclient->items[invslot].count);
                return true;
            }

            //LMA: checking count since we take money here...
            if(count>otherclient->Shop->BuyingList[slot].count)
            {
                count=otherclient->Shop->BuyingList[slot].count;
            }

            newitem.count = count;
            if( otherclient->CharInfo->Zulies<(otherclient->Shop->BuyingList[slot].price*count) )
                return true;
            unsigned int newslot = otherclient->GetNewItemSlot ( newitem );
            if( newslot==0xffff )
                return true;
            if( thisclient->items[invslot].count < count )
                return true;

            thisclient->CharInfo->Zulies += (otherclient->Shop->BuyingList[slot].price*count);
            otherclient->CharInfo->Zulies -= (otherclient->Shop->BuyingList[slot].price*count);
            if(thisclient->items[invslot].itemtype>9 &&
                    thisclient->items[invslot].itemtype<14)
            {
                if(thisclient->items[invslot].count <= count)
                {
                    ClearItem( thisclient->items[invslot] );
                    if(count>=otherclient->Shop->BuyingList[slot].count)
                    {
                         ClearItem( otherclient->Shop->BuyingList[slot].item );
                         otherclient->Shop->BuyingList[slot].slot = 0;
                         otherclient->Shop->BuyingList[slot].count = 0;
                         otherclient->Shop->BuyingList[slot].price = 0;
                    }
                    else
                    {
                         otherclient->Shop->BuyingList[slot].count -= count;
                    }
                }
                else
                {
                    if(otherclient->Shop->BuyingList[slot].count<=count)
                    {
                         ClearItem( otherclient->Shop->BuyingList[slot].item );
                         otherclient->Shop->BuyingList[slot].slot = 0;
                         otherclient->Shop->BuyingList[slot].count = 0;
                         otherclient->Shop->BuyingList[slot].price = 0;
                    }
                    else
                    {
                        otherclient->Shop->BuyingList[slot].count -= count;
                    }
                    thisclient->items[invslot].count-=count;
                }
            }
            else
            {
                ClearItem( thisclient->items[invslot] )
                ClearItem( otherclient->Shop->BuyingList[slot].item );
                otherclient->Shop->BuyingList[slot].slot = 0;
                otherclient->Shop->BuyingList[slot].count = 0;
                otherclient->Shop->BuyingList[slot].price = 0;
            }
            if(otherclient->items[newslot].count!=0)
                otherclient->items[newslot].count += newitem.count;
            else
                otherclient->items[newslot] = newitem;

            //update inventory (seller)
            BEGINPACKET( pak, 0x7c7 );
            ADDQWORD   ( pak, thisclient->CharInfo->Zulies );
            ADDBYTE    ( pak, 0x01 );
            ADDBYTE    ( pak, invslot );
			pak = AddItemData(thisclient->items[invslot], pak);
            thisclient->client->SendPacket( &pak );

            //update inventory (buyer)
            RESETPACKET( pak, 0x7c7 );
            ADDQWORD   ( pak, otherclient->CharInfo->Zulies );
            ADDBYTE    ( pak, 0x01 );
            ADDBYTE    ( pak, newslot );
			pak = AddItemData(otherclient->items[newslot], pak);
            otherclient->client->SendPacket( &pak );

            //PY: saving inventory
            thisclient->saveinventory();
			otherclient->saveinventory();

            //update shop
            CItem thisitem = otherclient->Shop->BuyingList[slot].item;
            RESETPACKET( pak, 0x7c6 );
            ADDWORD    ( pak, otherclient->clientid );
            ADDBYTE    ( pak, 0x07 );
            ADDBYTE    ( pak, 0x01 );
            ADDBYTE    ( pak, slot );
			pak = AddItemData(thisitem, pak);
            SendToVisible( &pak, otherclient );
        }
        break;
        default:
            Log( MSG_WARNING, "Sell unknown action: %i", action );
    }
    return true;
}

// Close Shop
bool CWorldServer::pakCloseShop( CPlayer* thisclient, CPacket* P )
{
    //LMA: to solve the "gem" visible bug.
    if(thisclient->Shop->open)
    {
        BEGINPACKET( pak, 0x7c3 );
        ADDWORD    ( pak, thisclient->clientid );
        ADDBYTE    ( pak, 0x00 );
        SendToVisible( &pak, thisclient );
    }

    thisclient->Shop->open = false;

    for(unsigned int j=0;j<30;j++)
    {
        thisclient->Shop->SellingList[j].slot = 0;
        thisclient->Shop->SellingList[j].count = 0;
        thisclient->Shop->SellingList[j].price = 0;
        thisclient->Shop->BuyingList[j].slot = 0;
        thisclient->Shop->BuyingList[j].count = 0;
        thisclient->Shop->BuyingList[j].price = 0;
        ClearItem( thisclient->Shop->BuyingList[j].item );

    }
    return true;
}

// Modified item (put gem, refine, Drill )
bool CWorldServer::pakModifiedItem( CPlayer* thisclient, CPacket* P )
{
    if(thisclient->Shop->open == true)
        return true;
    BYTE action = GETBYTE((*P),0);
    switch(action)
    {
        case 0x01://Gemming
        {
            BYTE destslot = GETBYTE((*P),1);
            BYTE srcslot = GETBYTE((*P),2);
            if(!CheckInventorySlot( thisclient, destslot))
                return false;
            if(!CheckInventorySlot( thisclient, srcslot))
                return false;
            if( thisclient->items[srcslot].count<=0 )
            {
                BEGINPACKET( pak, 0x7bc );
                ADDBYTE    ( pak, 0x12 ); // dont have required material
                ADDBYTE    ( pak, 0x00 );
                thisclient->client->SendPacket( &pak );
                return true;
            }
            if( thisclient->items[destslot].gem != 0 )
            {
                BEGINPACKET( pak, 0x7bc );
                ADDBYTE    ( pak, 0x02 ); //no socket available
                ADDBYTE    ( pak, 0x00 );
                thisclient->client->SendPacket( &pak );
                return true;
            }
            thisclient->items[destslot].gem = thisclient->items[srcslot].itemnum;
			thisclient->UpdateInventory(destslot);

            thisclient->items[srcslot].count -= 1;
            if( thisclient->items[srcslot].count < 1 )
                ClearItem( thisclient->items[srcslot] );
			thisclient->UpdateInventory(srcslot);
			Log(MSG_INFO,"Gemming success. Item %i::%i refine: %i gem id: %i",thisclient->items[destslot].itemtype, thisclient->items[destslot].itemnum, thisclient->items[destslot].refine, thisclient->items[destslot].gem );

            BEGINPACKET( pak, 0x7bc );
            ADDBYTE    ( pak, 0x01 );//gemming success
            ADDBYTE    ( pak, 0x02 );
            ADDBYTE    ( pak, destslot );
			pak = AddItemData(thisclient->items[destslot], pak);
            ADDBYTE    ( pak, srcslot );
			pak = AddItemData(thisclient->items[srcslot], pak);
            thisclient->client->SendPacket( &pak );
            thisclient->SetStats( );
        }
        break; //case 0x02 ( Surprise gift box )

       case 0x02: // Disassembly
        {
            unsigned int Slot = GETBYTE((*P), 3);
            unsigned int rewardCount = (RandNumber(0, 100) < 20)?2:1;

            //Chests are only itemtype 10
			Log(MSG_INFO,"The item in the slot is of type %i and id %i",thisclient->items[Slot].itemtype, thisclient->items[Slot].itemnum);
            if(UseList.Index[thisclient->items[Slot].itemnum]->type == 0)
            {
                Log(MSG_INFO,"Item 10::%i is not a valid item. it has no type",thisclient->items[Slot].itemnum );
				return true;;
            }
			if(thisclient->items[Slot].itemtype == 10)
            {
                Log(MSG_INFO,"Recognized a type 10 item. Sending type 13 response");
				return GiveDisassembledItems(thisclient,Slot, 19);  //0x13
            }
			else
            {
                return GiveDisassembledItems(thisclient,Slot, 7);
            }
        }
        break;

        case 0x05://Refine
        {
            BYTE item = GETBYTE((*P),3);
            BYTE material = GETBYTE((*P),4);

            if(!CheckInventorySlot( thisclient, item))
                return false;
            if(!CheckInventorySlot( thisclient, material))
                return false;

            //LMA: We need to get the "real" item needed for upgrade.
            if (thisclient->items[item].itemtype == 0 || thisclient->items[item].itemtype > 14 || thisclient->items[item].itemnum == 0 || thisclient->items[item].itemnum >= EquipList[thisclient->items[item].itemtype].max)
            {
                Log(MSG_HACK,"Player %s tried to refine an item with wrong values slot %u, item %u::%u",thisclient->CharInfo->charname,item,thisclient->items[item].itemtype,thisclient->items[item].itemnum);
                return false;
            }

            UINT gradeIndex = EquipList[thisclient->items[item].itemtype].Index[thisclient->items[item].itemnum]->itemgradeID;	//Refine item type

            if(gradeIndex == 0 || gradeIndex >= ProductList.max)
            {
                Log(MSG_HACK,"Player %s can't refine %u::%u, index %u",thisclient->CharInfo->charname,thisclient->items[item].itemtype,thisclient->items[item].itemnum,gradeIndex);
                return false;
            }

            UINT needed_itemtype = 0;
            UINT needed_itemnum = 0;
            UINT needed_amount = 0;
            UINT zulyamount = 0;
            int quality = 0;

            needed_amount = ProductList.Index[gradeIndex]->amount[0];
            needed_itemtype = gi(ProductList.Index[gradeIndex]->item[0],0);
            needed_itemnum = gi(ProductList.Index[gradeIndex]->item[0],1);

            
            if(needed_itemtype != thisclient->items[material].itemtype || needed_itemnum != thisclient->items[material].itemnum)
            {
                Log(MSG_HACK,"Player %s uses wrong item (%u::%u instead of %u::%u) to refine %u::%u",thisclient->CharInfo->charname,thisclient->items[material].itemtype,thisclient->items[material].itemnum,needed_itemtype,needed_itemnum,thisclient->items[item].itemtype,thisclient->items[item].itemnum);
                return false;
            }

            //LMA: item amount
            //if(thisclient->items[item].count<1 || thisclient->items[material].count<1)
            if(thisclient->items[item].count < 1 || thisclient->items[material].count < needed_amount)
            {
                BEGINPACKET( pak, 0x7bc );
                ADDBYTE    ( pak, 0x12 );
                ADDBYTE    ( pak, 0x00 );
                thisclient->client->SendPacket( &pak );
                Log(MSG_HACK,"Player %s hasn't enough item %i*(%u::%u) for refine",thisclient->CharInfo->charname,needed_amount,needed_itemtype,needed_itemnum);
                return true;
            }

            //LMA: taking the money, we need the quality.
            if(thisclient->items[item].itemtype <= 0 || thisclient->items[item].itemtype >= 11)
            {
                Log(MSG_WARNING,"Error, player %s tryes to refine item %u::%u",thisclient->CharInfo->charname,thisclient->items[item].itemtype,thisclient->items[item].itemnum);
            }
            else
            {
                if(thisclient->items[item].itemnum >= EquipList[thisclient->items[item].itemtype].max)
                {
                    Log(MSG_WARNING,"Error, player %s tryes to refine itemn %u::%u but >= %u ",thisclient->CharInfo->charname,thisclient->items[item].itemtype,thisclient->items[item].itemnum,EquipList[thisclient->items[item].itemtype].max);
                }
                else
                {
                    quality = EquipList[thisclient->items[item].itemtype].Index[thisclient->items[item].itemnum]->quality;
                }

            }

            //end of zuly check.
			unsigned int currGrade = ( thisclient->items[item].refine / 16 );
            unsigned int nextGrade = ( thisclient->items[item].refine / 16 ) + 1;
            if( nextGrade > 9 )
                return true;

            //LMA: How much zuly? :)
            if(quality > 0 && nextGrade > 1)
            {
                //float basis = (pow(quality, three) * (float)((float)127 / (float)7685496) + pow(quality, two) * (float)((float)435665/(float)1097928) + quality * (float)((float)31437683 / (float)3842748) - (float)((float)77200/(float)24633));
                float zulyamount = (int)( currGrade * nextGrade * quality  * (quality + 20 ) * 0.2f );	//formula used in the client
            }

            if(zulyamount > 0)
            {
                if (thisclient->CharInfo->Zulies < zulyamount)
                {
                    SendPM(thisclient,"You don't have enough money to refine, %u needed");
                    Log(MSG_WARNING,"Error, player %s tryes to refine item %u::%u but hasn't enough money, %u needed, he has %li",thisclient->CharInfo->charname,thisclient->items[item].itemtype,thisclient->items[item].itemnum,zulyamount,thisclient->CharInfo->Zulies);
                    return true;
                }

                //taking the money (packet not needed).
                thisclient->CharInfo->Zulies -= zulyamount;
                /*
                BEGINPACKET( pak, 0x71d );
                ADDQWORD( pak, thisclient->CharInfo->Zulies );
                thisclient->client->SendPacket( &pak );*/
            }

            unsigned int prefine = rand()%100;
            bool success = false;
            if( prefine <= UpGradeList[nextGrade].BindSuccess )		//PY: for now just using bindrune success rate across the board. Can be expanded later
            {
                success = true;
            }
			Log(MSG_DEBUG,"Refining item %i::%i curr refine %i success rate: %i",thisclient->items[item].itemtype,thisclient->items[item].itemnum,currGrade,UpGradeList[nextGrade].BindSuccess);
            BEGINPACKET( pak, 0x7bc );
            if( success )
            {
                thisclient->items[item].refine = nextGrade * 16;
                ADDBYTE    ( pak, 0x10 );// 0x10 successful
				Log(MSG_DEBUG,"Refine success new Refine level: %i",thisclient->items[item].refine);
            }
            else
            {
                Log(MSG_DEBUG,"Refine fail");
				if (nextGrade <= 7)
                {
                    if(nextGrade > 5)
                    {
                        //LMA: 5% chance to degrade from 2 levels.
                        if(RandNumber(0,100) <= 5)
                        {
                            if((currGrade - 2) <= 0)
                            {
                                thisclient->items[item].refine = 0;
                            }
                            else
                            {
                                thisclient->items[item].refine = (currGrade - 2) * 16;
                            }

                        }
                        else
                        {
                            thisclient->items[item].refine = (currGrade - 1) * 16;
                        }

                    }
                    ADDBYTE    ( pak, 0x11 );// 0x11 Fail
                }
                else
                {
                    //Ouch, break time.
                    ClearItem( thisclient->items[item] );
                    ADDBYTE    ( pak, 0x11 );// 0x11 Fail
                }
            }
			
            thisclient->items[material].count -= 1; 
            if(thisclient->items[material].count < 1)
                ClearItem( thisclient->items[material] );

			Log(MSG_DEBUG,"Packet: Mat. Refine: %i, count: %i",thisclient->items[material].refine,thisclient->items[material].count);
			Log(MSG_DEBUG,"Packet: item. Refine: %i, appraised: %i, count: %i",thisclient->items[item].refine,thisclient->items[item].appraised, thisclient->items[item].count);
            ADDBYTE    ( pak, 0x03 );				//items a actualizar

            ADDBYTE    ( pak, material );							//slot
			pak = AddItemData(thisclient->items[material], pak);

            ADDBYTE    ( pak, item );
			pak = AddItemData(thisclient->items[item], pak);

            ADDBYTE    ( pak, 0x00 );
            //ADDDWORD   ( pak, 0x002f0000 );						//don't see what this is for. Sending an empty block instead
            //ADDDWORD   ( pak, 0x00000017 );
			ADDDWORD( pak, 0x00000000 );
			ADDDWORD( pak, 0x00000000 );
            thisclient->client->SendPacket( &pak );
			thisclient->UpdateInventory(material);
			thisclient->UpdateInventory(item);
			
        }
        break;
        case 0x06: // Drill
        {
            BYTE material = GETBYTE((*P), 1);
            BYTE item = GETBYTE((*P), 3);
            if(!CheckInventorySlot( thisclient, item))
                return false;
            if(!CheckInventorySlot( thisclient, material))
                return false;
            if( thisclient->items[material].count <= 0 )	
            {
                BEGINPACKET( pak, 0x7bc );
                ADDBYTE    ( pak, 0x12 );
                ADDBYTE    ( pak, 0x00 );
                thisclient->client->SendPacket( &pak );
                return true;
            }
            //LMA: already drilled
            if(thisclient->items[item].socketed)
            {
                BEGINPACKET( pak, 0x7bc );
                ADDBYTE    ( pak, 0x23 );
                ADDBYTE    ( pak, 0x00 );
                thisclient->client->SendPacket( &pak );
               return true;
            }

            thisclient->items[material].count -= 1;

            if( thisclient->items[material].count <= 0 )
                ClearItem(thisclient->items[material]);
			thisclient->UpdateInventory(material);

            //LMA: Adding some probability there :)
            UINT luck_time = RandNumber(0, 100);
            Log(MSG_INFO,"Drill succes? %i/%i",luck_time,UseList.Index[thisclient->items[material].itemnum]->useeffect[1]);
            if(luck_time > UseList.Index[thisclient->items[material].itemnum]->useeffect[1])
            {
                //fail
                thisclient->items[item].socketed = false;
                BEGINPACKET( pak, 0x7bc );
                ADDBYTE    ( pak, 0x21 );
                ADDBYTE    ( pak, 0x01 );
				//drill item
                ADDBYTE    ( pak, material);
				pak = AddItemData(thisclient->items[material], pak);
				ADDWORD ( pak, thisclient->items[material].count );	
                thisclient->client->SendPacket(&pak);
				
            }
            else
            {
                thisclient->items[item].socketed = true;
				thisclient->UpdateInventory(item);
                BEGINPACKET( pak, 0x7bc );
                ADDBYTE    ( pak, 0x20 );
                ADDBYTE    ( pak, 0x02 );
                //drill item
				ADDBYTE    ( pak, material);
				pak = AddItemData(thisclient->items[material], pak);
				//item to slot
                ADDBYTE ( pak, item );
				pak = AddItemData(thisclient->items[item], pak);
                thisclient->client->SendPacket(&pak);
            }

            //PY: saving inventory
            thisclient->saveinventory();
        }
        break;
        case 0x08:
        {
            //LMA: NOT perfect separation drill for example (separate item from gem).
            BYTE material = GETBYTE((*P), 1);
            BYTE item = GETBYTE((*P), 3);
            if(!CheckInventorySlot( thisclient, item))
                return false;
            if(!CheckInventorySlot( thisclient, material))
                return false;

            if( thisclient->items[material].count<=0 )
            {
                BEGINPACKET( pak, 0x7bc );
                ADDBYTE    ( pak, 0x12 );
                ADDBYTE    ( pak, 0x00 );
                thisclient->client->SendPacket( &pak );
                return true;
            }

            if(thisclient->items[material].itemtype!=10)
            {
                Log(MSG_WARNING,"%s tried to degem with a wrong item %u::%u (not a useitem)",thisclient->CharInfo->charname,thisclient->items[material].itemtype,thisclient->items[material].itemnum);
                BEGINPACKET( pak, 0x7bc );
                ADDBYTE    ( pak, 0x12 );
                ADDBYTE    ( pak, 0x00 );
                thisclient->client->SendPacket( &pak );
                return true;
            }

            if (thisclient->items[material].itemnum>=UseList.max)
            {
                Log(MSG_WARNING,"%s trying to separate gem with useitem %u which is >= to %u (nb max of useitem)",thisclient->CharInfo->charname,thisclient->items[material].itemnum,UseList.max);
                return true;
            }

            switch (UseList.Index[thisclient->items[material].itemnum]->type)
            {
                case 319:
                {
                    //it can fail.
                }
                break;
                default:
                {
                    Log(MSG_WARNING,"%s tried to degem with a useitem %u::%u which has an unhandled type %u",thisclient->CharInfo->charname,thisclient->items[material].itemtype,thisclient->items[material].itemnum,UseList.Index[thisclient->items[material].itemnum]->type);
                    return true;
                }
                break;
            }

            //LMA: no gem on this one.
            if( thisclient->items[item].gem == 0 )
            {
                BEGINPACKET( pak, 0x7bc );
                ADDBYTE    ( pak, 0x12 );
                ADDBYTE    ( pak, 0x00 );
                thisclient->client->SendPacket( &pak );
               return true;
            }

            if(thisclient->items[item].gem>=JemList.max)
            {
                Log(MSG_WARNING,"%s trying to separate gem %u which is >= to %u (nb max of jems)",thisclient->CharInfo->charname,thisclient->items[item].gem,JemList.max);
                return true;
            }

            //Ok let's try to see how it's going.
            bool failed=false;
            //Let's get a % chance.
            //if it suceeds, separation is still ok but the gem is destroyed.
            if(RandNumber(0,100) > UseList.Index[thisclient->items[material].itemnum]->useeffect[1])
            {
                failed=true;
            }

            //Saving material.
            thisclient->items[material].count -= 1;
            if( thisclient->items[material].count < 1)
                ClearItem( thisclient->items[material] );
            thisclient->UpdateInventory(material);

            if(failed)
            {
                //same as drilling failed.
                BEGINPACKET( pak, 0x7bc );
                ADDBYTE    ( pak, 0x21 );
                ADDBYTE    ( pak, 0x01 );
                ADDBYTE    ( pak, material);
				pak = AddItemData(thisclient->items[material], pak);
                thisclient->client->SendPacket(&pak);
                return true;
            }

            //let's unsocket and ungem.
            thisclient->items[item].socketed = true;
            thisclient->items[item].gem = 0;
            thisclient->UpdateInventory(item);


            //packet time.
            BEGINPACKET( pak, 0x7bc );
            ADDBYTE    ( pak, 0x06 );
            ADDBYTE    ( pak, 0x02 );
            ADDBYTE    ( pak, material);
			pak = AddItemData(thisclient->items[material], pak);
            ADDBYTE    ( pak, item );
			pak = AddItemData(thisclient->items[item], pak);
            thisclient->client->SendPacket(&pak);
        }
        break;
        case 0x09:
        {
            //LMA: perfect separation drill for example (separate item from gem).
            BYTE material = GETBYTE((*P), 1);
            BYTE item = GETBYTE((*P), 3);
            if(!CheckInventorySlot( thisclient, item))
                return false;
            if(!CheckInventorySlot( thisclient, material))
                return false;

            if( thisclient->items[material].count<=0 )
            {
                BEGINPACKET( pak, 0x7bc );
                ADDBYTE    ( pak, 0x12 );
                ADDBYTE    ( pak, 0x00 );
                thisclient->client->SendPacket( &pak );
                return true;
            }

            if(thisclient->items[material].itemtype!=10)
            {
                Log(MSG_WARNING,"%s tried to degem with a wrong item %u::%u (not a useitem)",thisclient->CharInfo->charname,thisclient->items[material].itemtype,thisclient->items[material].itemnum);
                BEGINPACKET( pak, 0x7bc );
                ADDBYTE    ( pak, 0x12 );
                ADDBYTE    ( pak, 0x00 );
                thisclient->client->SendPacket( &pak );
                return true;
            }

            if (thisclient->items[material].itemnum>=UseList.max)
            {
                Log(MSG_WARNING,"%s trying to separate gem with useitem %u which is >= to %u (nb max of useitem)",thisclient->CharInfo->charname,thisclient->items[material].itemnum,UseList.max);
                return true;
            }

            switch (UseList.Index[thisclient->items[material].itemnum]->type)
            {
                case 324:
                {
                    //perfect separation
                }
                break;

                default:
                {
                    Log(MSG_WARNING,"%s tried to degem with a useitem %u::%u which has an unhandled type %u",thisclient->CharInfo->charname,thisclient->items[material].itemtype,thisclient->items[material].itemnum,UseList.Index[thisclient->items[material].itemnum]->type);
                    return true;
                }
                break;
            }

            //LMA: no gem on this one.
            if( thisclient->items[item].gem == 0 )
            {
                BEGINPACKET( pak, 0x7bc );
                ADDBYTE    ( pak, 0x12 );
                ADDBYTE    ( pak, 0x00 );
                thisclient->client->SendPacket( &pak );
               return true;
            }

            if( thisclient->items[item].gem >= JemList.max )
            {
                Log(MSG_WARNING,"%s trying to separate gem %u which is >= to %u (nb max of jems)",thisclient->CharInfo->charname,thisclient->items[item].gem,JemList.max);
                return true;
            }

            //Ok let's try to see how it's going.
            CItem newgem;
            ClearItem( newgem );
            unsigned int tempslot=0;

            newgem.count = 1;
            newgem.itemtype = 11;
            newgem.itemnum = thisclient->items[item].gem;
            int grade_gem = newgem.itemnum%10;
            bool degrade = false;

            //we don't degrade the enchanted stones (itemnum>=500)
            if(grade_gem>1&&newgem.itemnum<500)
            {
                newgem.itemnum -= 1;
                degrade = true;
            }

            tempslot = thisclient->AddItem( newgem );
            if (tempslot == 0xffff)
            {
                //TODO: error message: no place in inventory.
                /*BEGINPACKET( pak, 0x7bc );
                ADDBYTE    ( pak, 0x23 );
                ADDBYTE    ( pak, 0x00 );
                thisclient->client->SendPacket( &pak );*/

                BEGINPACKET( pak, 0x7a7 );
                ADDWORD( pak, 0x00 );
                ADDBYTE( pak, 0x03 );
                ADDBYTE( pak, 0x00 );
                thisclient->client->SendPacket( &pak );

                return true;
            }

            //saving gem.
            thisclient->UpdateInventory(tempslot);

            //let's unsocket and ungem.
            thisclient->items[item].socketed = true;
            thisclient->items[item].gem = 0;
            thisclient->UpdateInventory( item );

            //Saving material.
            thisclient->items[material].count -= 1;
            if( thisclient->items[material].count < 1)
                ClearItem( thisclient->items[material] );
            thisclient->UpdateInventory(material);

            //packet time.
            //TODO: message if no degrade.
            BEGINPACKET( pak, 0x7bc );
            ADDBYTE    ( pak, 0x05 );
            ADDBYTE    ( pak, 0x03 );
            ADDBYTE    ( pak, material);
			pak = AddItemData(thisclient->items[material], pak);
            ADDBYTE    ( pak, item );
			pak = AddItemData(thisclient->items[item], pak);
            ADDBYTE    ( pak, tempslot );
			pak = AddItemData(thisclient->items[tempslot], pak);

            thisclient->client->SendPacket(&pak);
        }
        break;
        default:
            Log( MSG_WARNING,"Modified Item unknown action: %i", action);
    }
    return true;
}

// Repair (NPC and tool)
bool CWorldServer::pakRepairItem( CPlayer* thisclient, CPacket* P,int packet_type)
{
    BYTE slot_tool=0;
    BYTE slot = GETBYTE((*P),2);
    if(!CheckInventorySlot( thisclient, slot)) return false;
    if(thisclient->items[slot].count<1) return true;

    //LMA: We have NPC or tool repair, comes from different packet.
    switch( packet_type)
    {
        case 1:
        {
            //NPC repair (NPC client ID).
            BYTE npc_id=GETWORD((*P),0);
            //let's check the NPC is amongst the good ones...
            CNPC* testnpc=GServer->GetNPCByID(npc_id, thisclient->Position->Map);
            if (testnpc==NULL)
            {
                Log(MSG_HACK,"%s tried to repair with an unknown NPC!",thisclient->CharInfo->charname);
                return true;
            }

            //checking the NPC to a list of "correct" NPC.
            int liste_npc[6];
            liste_npc[0]=1008;   //Raffle
            liste_npc[1]=1093;   //Crune
            liste_npc[2]=1034;   //Ronk
            liste_npc[3]=1062;   //Punwell
            liste_npc[4]=1181;   //pavrick
            liste_npc[5]=1223;   //nel eldora

            bool is_found=false;
            int k = 0;
            for (k=0;k<6;k++)
            {
                if(testnpc->npctype == liste_npc[k])
                {
                    is_found = true;
                    break;
                }

            }

            if (!is_found)
            {
                Log(MSG_HACK,"%s tried to repair with an wrong NPC (%i)!",thisclient->CharInfo->charname,testnpc->npctype);
                return true;
            }

            //checking distance now.
            if(GServer->distance(thisclient->Position->current,testnpc->pos)>20)
            {
                Log(MSG_HACK,"%s tried to repair but too far from NPC %i!",thisclient->CharInfo->charname,testnpc->npctype);
                return true;
            }

            Log(MSG_INFO,"Npc index %i (%i) is repairing for %s",npc_id,liste_npc[k],thisclient->CharInfo->charname);
        }
        break;
        case 2:
        {
            //Tool repair.
            slot_tool=GETBYTE((*P),0);
            //let's check if we have a tool in stock :)
            if(!CheckInventorySlot( thisclient, slot_tool)) return false;
            if(thisclient->items[slot_tool].count<1)
            {
                Log(MSG_HACK,"Player %s tryed to repair without tool",thisclient->CharInfo->charname);
                return true;
            }

            //Is it a repair tool?
            if(thisclient->items[slot_tool].itemnum>=UseList.max||UseList.Index[thisclient->items[slot_tool].itemnum]->type!=315)
            {
                Log(MSG_HACK,"Player %s tryed to repair without a good tool",thisclient->CharInfo->charname);
                return true;
            }

            //it's a good tool, let's take one of them.
            thisclient->items[slot_tool].count--;
            if (thisclient->items[slot_tool].count==0)
                ClearItem( thisclient->items[slot_tool] );

            //Ok now the quality of the tool :)
            if (UseList.Index[thisclient->items[slot_tool].itemnum]->quality<=50)
            {
                //standard quality, so durability goes away...
                thisclient->items[slot].durability-=1;
                if(thisclient->items[slot].durability<=0)
                    thisclient->items[slot].durability=1;
            }

        }
        break;

    }

    thisclient->items[slot].lifespan = 100;

    //Still TODO: find where prices of storage and repair are and add it to the code.
    if( packet_type == 1)
    {
        BEGINPACKET( pak, 0x7cd );
        ADDQWORD   ( pak, thisclient->CharInfo->Zulies );
        ADDBYTE    ( pak, 0x01 );
        ADDBYTE    ( pak, slot );
		pak = AddItemData(thisclient->items[slot], pak);
        thisclient->client->SendPacket( &pak );
        thisclient->SetStats( );
        //PY: saving inventory
        thisclient->saveinventory();
        return true;
    }

    //repair tool.
    BEGINPACKET( pak, 0x7cb );
    ADDBYTE    ( pak, 0x02 );
    ADDBYTE    ( pak, slot_tool );
	pak = AddItemData(thisclient->items[slot_tool], pak);
    ADDBYTE    ( pak, slot );
	pak = AddItemData(thisclient->items[slot], pak);
    thisclient->client->SendPacket( &pak );
    thisclient->SetStats( );
    //PY: saving inventory
    thisclient->saveinventory();

    return true;
}

// Ride request
bool CWorldServer::pakRideRequest( CPlayer* thisclient, CPacket* P )
{
    if(thisclient->Shop->open)
        return true;

    BYTE action = GETBYTE((*P),0);
    switch(action)
    {
        case 0x01://Ride Request
        {
            WORD tclientid = GETWORD((*P),1);
            WORD oclientid = GETWORD((*P),3);
            if( tclientid != thisclient->clientid )
                return true;
            CPlayer* otherclient = GetClientByID( oclientid, thisclient->Position->Map );
            if(otherclient==NULL)
                return true;

            //LMA: check if both aren't already driving.
            if (thisclient->Ride->Drive&&otherclient->Ride->Drive)
            {
                return true;
            }

            BEGINPACKET( pak, 0x7dd );
            ADDBYTE    ( pak, 0x01 );
            ADDWORD    ( pak, thisclient->clientid );
            ADDWORD    ( pak, oclientid );
            ADDFLOAT   ( pak, thisclient->Position->current.x*100 );
            ADDFLOAT   ( pak, thisclient->Position->current.y*100 );
            otherclient->client->SendPacket( &pak );
        }
        break;
        case 0x02://Accept
        {
            WORD oclientid = GETWORD((*P),1);
            WORD tclientid = GETWORD((*P),3);
            if( tclientid != thisclient->clientid )
                return true;
            CPlayer* otherclient = GetClientByID( oclientid, thisclient->Position->Map );
            if(otherclient==NULL)
                return true;

            //LMA: check if both aren't already driving.
            if (thisclient->Ride->Drive&&otherclient->Ride->Drive)
            {
                return true;
            }

            BEGINPACKET( pak, 0x796 );
            ADDWORD    ( pak, tclientid );
            ADDFLOAT   ( pak, thisclient->Position->current.x*100 );
            ADDFLOAT   ( pak, thisclient->Position->current.y*100 );
            ADDWORD    ( pak, 0x0000 );
            SendToVisible( &pak, otherclient );

            RESETPACKET( pak, 0x7dd );
            ADDBYTE    ( pak, 0x02 );
            ADDWORD    ( pak, oclientid );
            ADDWORD    ( pak, tclientid );
            SendToVisible( &pak, otherclient );
            thisclient->Ride->Ride = true;
            thisclient->Ride->Drive = false;
            thisclient->Ride->charid = otherclient->CharInfo->charid;
            otherclient->Ride->Ride = true;
            otherclient->Ride->Drive = true;
            otherclient->Ride->charid = thisclient->CharInfo->charid;
        }
        break;
        case 0x03://Denied
        {
            WORD oclientid = GETWORD((*P),1);
            WORD tclientid = GETWORD((*P),3);
            if( tclientid != thisclient->clientid )
                return true;
            CPlayer* otherclient = GetClientByID( oclientid, thisclient->Position->Map );
            if(otherclient==NULL)
                return true;
            BEGINPACKET( pak, 0x7dd );
            ADDBYTE    ( pak, 0x03 );
            ADDWORD    ( pak, oclientid );
            ADDWORD    ( pak, tclientid );
            otherclient->client->SendPacket( &pak );
        }
        break;
    }
	return true;
}

bool CWorldServer::pakWeight( CPlayer* thisclient, CPacket* P )
{
    BYTE weight = GETBYTE((*P),0) & 0xff;
    if(weight > 110)
        thisclient->Status->CanAttack = false;
    else
        thisclient->Status->CanAttack = true;
    if(weight > 100)
        thisclient->Status->CanRun = false;
    else
        thisclient->Status->CanRun = true;
    thisclient->Stats->Move_Speed = thisclient->GetMoveSpeed( );
    BEGINPACKET( pak, 0x762 );
    ADDWORD    ( pak, thisclient->clientid );
    ADDBYTE    ( pak, weight );
    thisclient->client->SendPacket( &pak );
    return true;
}

// Show Monster HP or Player Regeneration
bool CWorldServer::pakShowHeal( CPlayer* thisclient, CPacket* P )
{
    WORD clientid = GETWORD((*P),0);
    CCharacter* character = MapList.Index[thisclient->Position->Map]->GetCharInMap( clientid );
    if(character == NULL) return true;
    if(character->IsMonster())
    {
    	// SET MONSTER HEALTH
        if(character->Stats->HP > MAXHPMOB)
        {
            character->Stats->HP = (long long) MAXHPMOB;
        }

    	BEGINPACKET( pak, 0x79f );
    	ADDWORD( pak, character->clientid );
    	ADDDWORD   ( pak, character->Stats->HP );    //LMA: DDWORD :)
        thisclient->client->SendPacket( &pak );
        thisclient->Battle->contatk = false;
    }
    thisclient->Battle->target = clientid;
    return true;
}

// Repair Tool
bool CWorldServer::pakRepairTool( CPlayer* thisclient, CPacket* P )
{
    BEGINPACKET( pak, 0x7cb );

    thisclient->items[(GETBYTE((*P), 0x0))].count -= 1;
    thisclient->items[(GETBYTE((*P), 0x2))].lifespan = 100;
    if(thisclient->items[(GETBYTE((*P), 0x0))].count <= 0)
	{
        ClearItem(thisclient->items[(GETBYTE((*P), 0x0))]);
        RESETPACKET (pak, 0x7cb);
        ADDBYTE(pak, 1);
        ADDBYTE(pak,GETBYTE((*P), 0x2));
		pak = AddItemData(thisclient->items[GETBYTE((*P), 0x2)], pak);

        thisclient->client->SendPacket(&pak);
    }
    else
	{
        RESETPACKET (pak, 0x7cb);
        ADDBYTE(pak, 2);
        ADDBYTE(pak,GETBYTE((*P), 0x0));
		pak = AddItemData(thisclient->items[GETBYTE((*P), 0x0)], pak);
		 
        ADDBYTE(pak,GETBYTE((*P), 0x2));
		pak = AddItemData(thisclient->items[GETBYTE((*P), 0x2)], pak);

		thisclient->client->SendPacket(&pak);
    }
    return true;
}

// Disconnect char
bool CWorldServer::pakCharDSClient( CPlayer* thisclient, CPacket* P )
{
    unsigned int userid = GETDWORD((*P), 1 );
    CPlayer* otherclient = GetClientByUserID( userid );
    if(otherclient==NULL) return true;

    BYTE action = GETBYTE((*P),0);
    switch(action)
    {
        case 1:
        {
            if(otherclient==NULL)
            {
                Log(MSG_WARNING, "userid '%s' not found online (pakCharDSClient)", userid );
                return true;
            }

            BEGINPACKET( pak, 0x707 );
            ADDWORD( pak, 0 );
            otherclient->client->SendPacket( &pak );

            otherclient->client->isActive = false;
        }
        break;
    }
    return true;
}


// Pack Printscreen
bool CWorldServer::pakPrintscreen( CPlayer* thisclient, CPacket* P )
{
    BEGINPACKET( pak, 0x7eb );
    ADDWORD    ( pak, thisclient->Session->userid );
    ADDWORD    ( pak, 0x0302 );
    ADDWORD    ( pak, 0x2d17 );
    thisclient->client->SendPacket( &pak );
    return true;
}

// CharServer Connected
bool CWorldServer::pakCSReady ( CPlayer* thisclient, CPacket* P )
{
	if(thisclient->Session->isLoggedIn) return false;
	if(GETDWORD((*P),0) != Config.WorldPass )
    {
        Log( MSG_HACK, "CharServer Tried to connect WorldServer with wrong password ");
        return true;
    }
	thisclient->Session->isLoggedIn=true;
	return true;
}

// CharServer - Disconnect User
bool CWorldServer::pakCSCharSelect ( CPlayer* thisclient, CPacket* P )
{
    if(!thisclient->Session->isLoggedIn) return false;
	Log( MSG_INFO, "Char server requested client kill" );
	DWORD userid = GETDWORD( (*P), 0 );
	CPlayer* otherclient = GetClientByUserID( userid );
	if(otherclient!=NULL)
	{
        if(otherclient->client==NULL) return true;
        otherclient->client->isActive = false;
    }
	Log( MSG_INFO, "Client killed" );
	return true;
}

//LMA: ItemMall handling
bool CWorldServer::pakItemMall( CPlayer* thisclient, CPacket* P )
{

     switch (GETBYTE((*P), 0x0))
     {
            case 0x01:
            {
                 //list of player's itemmall
                 //LMA: Doing this because of CMS which can add items on their own...
                 /*if (thisclient->nsitemmallitems==0)
                    return true;*/
                 ReturnItemMallList(thisclient);
            }
            break;
            case 0x02:
            {
                 //transfer to another player?
                 return true;
            }
            break;
            case 0x03:
            {
                 //takes one item to itemmall to player's inventory
                 WORD mal_qty=GETWORD( (*P), 0x01);
                 BYTE mall_slot=GETBYTE((*P), 0x03);
                 Log(MSG_INFO,"%s Trying to get %i from slot %i in mileage",thisclient->CharInfo->charname,mal_qty,mall_slot);
                 //Handle this...
                 TakeItemMallList(thisclient,mal_qty,mall_slot);
                 return true;
            }
            break;
            default:
            {
                    Log(MSG_INFO,"Unknown command %i for ItemMall",GETBYTE((*P), 0x0));
                    return true;
            }
            break;
     }


    return true;
}

/**
	* add item to the wishlist
	* @param thisclient CPlayer class
	* @param P CPacket class
	* @return bool true if the item was added else false
*/
bool CWorldServer::pakAddWishList( CPlayer* thisclient , CPacket* P ) //PY What's this BS about head and data. It's a standard CItem structure. Good grief
{
	if(thisclient == NULL) return false;

	//UINT slot = GETBYTE( (*P), 0 );
	//if(slot>=MAX_WISHLIST) return true;
	//UINT head = GETDWORD((*P),1);
	//UINT data = GETDWORD((*P),5);

	//PY ALL WRONG. Client sends the item and THEN the wishslot
	//let's just make a CITem object and populate it

	CItem wishitem;
	wishitem.itemtype = GETWORD((*P), 0);
	wishitem.itemnum = GETWORD((*P), 2);
	wishitem.isCreated = GETBYTE((*P), 3);
	wishitem.gem = GETWORD((*P), 4);
	wishitem.durability = GETBYTE((*P), 6);
	wishitem.lifespan = GETWORD((*P), 7);
	wishitem.socketed = GETBYTE((*P), 9);
	wishitem.appraised = GETBYTE((*P), 10);
	wishitem.refine = GETBYTE((*P), 11);
	wishitem.UStat[1] = GETWORD((*P),12);
	wishitem.UStat[2] = GETWORD((*P),14);
	wishitem.UValue[1] = GETWORD((*P),16);
	wishitem.UValue[2] = GETWORD((*P),18);
	// we skip 6 bytes for timers
	byte wishslot = GETBYTE((*P),24);

    //testing slot.
    if ( wishslot < 0 || wishslot >= MAX_WISHLIST )
    {
        Log(MSG_WARNING,"Wrong wishlist slot for %s (slot %i, item %u::%u)",thisclient->CharInfo->charname,wishslot,wishitem.itemtype,wishitem.itemnum);
        return true;
    }

    //LMA: We delete a slot.
	if (wishitem.itemtype == 0 || wishitem.itemnum == 0)
	{
	    DB->QExecute( "DELETE FROM wishlist WHERE itemowner=%u AND slot=%i",thisclient->CharInfo->charid, wishslot );
	    return true;
	}

    //CItem testitem = GetItemByHeadAndData( head , data); //useless and not needed
	// check if is a valid item
	if(wishitem.itemtype > 14 || wishitem.itemtype < 1)
	{
	    Log(MSG_WARNING,"Wrong item for %s (slot %i, item %u::%u)",thisclient->CharInfo->charname,wishslot,wishitem.itemtype,wishitem.itemnum);
	    return true;
	}
	thisclient->wishlistitems[wishslot] = wishitem;

    //DB->QExecute("INSERT INTO wishlist (itemowner,slot,itemhead,itemdata) VALUES(%u,%i,%u,%u) ON DUPLICATE KEY UPDATE itemowner=VALUES(itemowner),slot=VALUES(slot),itemhead=VALUES(itemhead),itemdata=VALUES(itemdata)",
    //    thisclient->CharInfo->charid, slot, head, data );

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
    for(unsigned int i=0;i<=MAX_WISHLIST; i++) //wishlist holds 30
    {
        int socketed = 0;
        int appraised = 0;
        if(i == 0)
        {
            if(thisclient->wishlistitems[i].count == 0)
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
                if(thisclient->wishlistitems[i].socketed == true)
                    socketed = 1;
                if(thisclient->wishlistitems[i].appraised == true)
                    appraised = 1;
                sprintf(&tmp1[strlen(tmp1)], "%i",thisclient->wishlistitems[i].itemnum);
                sprintf(&tmp2[strlen(tmp2)], "%i",thisclient->wishlistitems[i].itemtype);
                sprintf(&tmp3[strlen(tmp3)], "%i",thisclient->wishlistitems[i].refine);
                sprintf(&tmp4[strlen(tmp4)], "%i",thisclient->wishlistitems[i].durability);
                sprintf(&tmp5[strlen(tmp5)], "%i",thisclient->wishlistitems[i].lifespan);
                sprintf(&tmp6[strlen(tmp6)], "%i",thisclient->wishlistitems[i].count);
                sprintf(&tmp7[strlen(tmp7)], "%i",thisclient->wishlistitems[i].stats);
                sprintf(&tmp8[strlen(tmp8)], "%i",socketed);
                sprintf(&tmp9[strlen(tmp9)], "%i",appraised);
				sprintf(&tmp10[strlen(tmp10)], "%i",thisclient->wishlistitems[i].UStat[0]);
				sprintf(&tmp11[strlen(tmp11)], "%i",thisclient->wishlistitems[i].UStat[1]);
				sprintf(&tmp12[strlen(tmp12)], "%i",thisclient->wishlistitems[i].UValue[0]);
				sprintf(&tmp13[strlen(tmp13)], "%i",thisclient->wishlistitems[i].UValue[1]);
                sprintf(&slot[strlen(slot)], "%i",i);
            }
        }
        else
        {
            if(thisclient->wishlistitems[i].count == 0)
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
                if(thisclient->wishlistitems[i].socketed == true)
                    socketed = 1;
                if(thisclient->wishlistitems[i].appraised == true)
                    appraised = 1;
                if(thisclient->wishlistitems[i].gem != 0 && thisclient->wishlistitems[i].gem != thisclient->wishlistitems[i].stats)
                    thisclient->wishlistitems[i].stats = thisclient->wishlistitems[i].gem;
                sprintf(&tmp1[strlen(tmp1)], "|%i",thisclient->wishlistitems[i].itemnum);
                sprintf(&tmp2[strlen(tmp2)], "|%i",thisclient->wishlistitems[i].itemtype);
                sprintf(&tmp3[strlen(tmp3)], "|%i",thisclient->wishlistitems[i].refine);
                sprintf(&tmp4[strlen(tmp4)], "|%i",thisclient->wishlistitems[i].durability);
                sprintf(&tmp5[strlen(tmp5)], "|%i",thisclient->wishlistitems[i].lifespan);
                sprintf(&tmp6[strlen(tmp6)], "|%i",thisclient->wishlistitems[i].count);
                sprintf(&tmp7[strlen(tmp7)], "|%i",thisclient->wishlistitems[i].stats);
                sprintf(&tmp8[strlen(tmp8)], "|%i",socketed);
                sprintf(&tmp9[strlen(tmp9)], "|%i",appraised);
				sprintf(&tmp10[strlen(tmp10)], "|%i",thisclient->wishlistitems[i].UStat[0]);
				sprintf(&tmp11[strlen(tmp11)], "|%i",thisclient->wishlistitems[i].UStat[1]);
				sprintf(&tmp12[strlen(tmp12)], "|%i",thisclient->wishlistitems[i].UValue[0]);
				sprintf(&tmp13[strlen(tmp13)], "|%i",thisclient->wishlistitems[i].UValue[1]);
                sprintf(&slot[strlen(slot)], "|%i",i);
            }
        }
    }
    //check if a record exists for this player
	tran = 100;
    MYSQL_RES *result = GServer->DB->QStore("SELECT itemid FROM wishlist WHERE owner=%i", thisclient->CharInfo->charid);
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
        GServer->DB->QExecute("UPDATE wishlist SET itemid='%s',itemtype='%s',refine='%s',durability='%s',lifespan='%s',count='%s',stats='%s',socketed='%s',appraised='%s',ustat1='%s',ustat2='%s',uvalue1='%s',uvalue2='%s',slot='%s',trans_action=%i WHERE owner='%i'",
                    tmp1,tmp2,tmp3,tmp4,tmp5,tmp6,tmp7,tmp8,tmp9,tmp10,tmp11,tmp12,tmp13,slot,tran,thisclient->CharInfo->charid);
    }
    else
    {
        //record does not exist so make a new one
        GServer->DB->QExecute("INSERT INTO wishlist (owner,itemid,itemtype,refine,durability,lifespan,count,stats,socketed,appraised,ustat1,ustat2,uvalue1,uvalue2,slot,trans_action) VALUES('%i','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%i')",
    				thisclient->CharInfo->charid,tmp1,tmp2,tmp3,tmp4,tmp5,tmp6,tmp7,tmp8,tmp9,tmp10,tmp11,tmp12,tmp13,slot,tran);
    }
    
    Log(MSG_INFO,"Wishlist saved");


	return true;
}


//LMA: Code used for disassemble.
bool CWorldServer::GiveDisassembledItems( CPlayer* thisclient,UINT src, BYTE CraftRslt)
{
    if(CraftRslt == 0)
	{
		Log(MSG_WARNING,"The CraftRslt is Zero. Aborting disassembly");
		return true;
	}
	//start disassemble
    if(!CheckInventorySlot( thisclient, src))
        return false;
    if(thisclient->items[src].count < 1)
        return false;

    //Rapid search.
    bool is_ok = false;
    int k = 0;
    bool is_failed = true;
    bool not_found = true;

    UINT breakid = GetBreakID(thisclient->items[src].itemnum,thisclient->items[src].itemtype);
    if (breakid == 0)
    {
        //trying the old way...
		Log(MSG_WARNING,"The breakid is Zero. Trying the old way");
        for(int i=0;i<maxBreak;i++)
        {
            if(thisclient->items[src].itemnum == BreakList[i]->itemnum && thisclient->items[src].itemtype == BreakList[i]->itemtype)
            {
                breakid = i;
                is_failed = false;
                not_found = false;
                break;
            }

        }

    }
    else
    {
        is_failed = false;
        not_found = false;
		Log(MSG_WARNING,"The breakid returned is. %i",breakid);
    }

    k = breakid;

	UINT totalprob = BreakList[k]->TotalChance;

	if(totalprob == 0)
	{
		is_failed = true;
	}

    //Handling failure!
    if(is_failed)
    {
        return true;
		//this is bullshit
		//return GiveDefaultDasm(thisclient, src, not_found, is_failed);
    }

    bool give_all = false;
    CItemChests mybreak[4];
    int nb_items=0;
    UINT m[4];

    for (int kk=0;kk<4;kk++)
    {
       m[kk]=99;
    }

	

    //How many rewards for this break?
	// Needs to be from minDis to maxDis
	int RandNum  = RandNumber(1, 100);
	UINT MaxRewards = 4;					//Default 4 rewards (10%)
	if(RandNum < 90) MaxRewards = 3;	//75 - 90. 15% chance of 3 rewards
	if(RandNum < 75) MaxRewards = 2;	//50 - 75. 25% cahnce of 2 rewards
	if(RandNum < 50) MaxRewards = 1;	//0 - 50. 50% chance of only one
	
	if( MaxRewards < 1 ) MaxRewards = 1;
	if( MaxRewards < BreakList[k]->minDis ) MaxRewards = BreakList[k]->minDis;		//some items MUST give more than 1 item
	if( MaxRewards > BreakList[k]->maxDis ) MaxRewards = BreakList[k]->maxDis;
	if( MaxRewards > 4 ) MaxRewards = 4; //Some of them go up to 15 in the breaklist STB

    //MaxRewards = 1;		//fixed it to 1 for tests
    //Log(MSG_WARNING,"Giving %i items from the break with id %i", MaxRewards, k);
	dword thisMin = 0;	//this number is the index for the start of each iteration
    for (UINT z=0;z<MaxRewards;z++)
    {
        //Now we find which sub-item to give
		dword ItemSelector = RandNumber(thisMin, totalprob);
		//Log(MSG_DEBUG,"ItemSelector: %i Min: %i Max: %i", ItemSelector, thisMin, totalprob);
		dword RunningTotal = 0;
		short s = 0;
		short j = 0;
		for(s=0;s<20;s++)	//scan all 20 possible slots
		{
			//Log(MSG_DEBUG," Is ItemSelector: %i  less than running total: %i ?", ItemSelector, BreakList[k]->RunningTotal[s]);
			if(ItemSelector < BreakList[k]->RunningTotal[s])
			{
				//found our item
				j = s;
				//Log(MSG_DEBUG,"Selected item: %i from the break list", j);
				s = 21;
			}
		}
		thisMin += BreakList[k]->RunningTotal[z];						//increment the start point for next time
		if(thisMin > totalprob)thisMin = 0;
		if(thisMin > 19)thisMin = 1;									//reset if it goes off the end
		
		//Log(MSG_DEBUG,"MinChance: %i. MaxChance %i selected item %i", BreakList[k]->minChance[s], BreakList[k]->maxChance[s], s);
		
		
		mybreak[z].item.itemnum = gi(BreakList[k]->product[j],1);
        mybreak[z].item.itemtype = gi(BreakList[k]->product[j],0);
		mybreak[z].item.isCreated = 0;
        mybreak[z].item.socketed = false;
        mybreak[z].item.appraised = true;
        mybreak[z].item.lifespan = 100;
        mybreak[z].item.durability = RandNumber(40,90);
        mybreak[z].item.refine = 0;
        mybreak[z].item.stats = 0;
		if(mybreak[z].item.itemtype > 9 && mybreak[z].item.itemtype < 14) //it's stackable 
		{
			mybreak[z].item.UStat[0] = 0;
			mybreak[z].item.UStat[1] = 0;
			mybreak[z].item.UValue[0] = 0;
			mybreak[z].item.UValue[1] = 0;
		}
		else
		{
			mybreak[z].item.UStat[0] = GetUStat(20);
			mybreak[z].item.UStat[1] = GetUStat(5);
			mybreak[z].item.UValue[0] = GetUValue(20,mybreak[z].item.UStat[0]);
			mybreak[z].item.UValue[1] = GetUValue(5,mybreak[z].item.UStat[1]);
		}
        mybreak[z].item.gem = 0;
        mybreak[z].item.sp_value = 0;
        mybreak[z].item.last_sp_value = 0;
        mybreak[z].slot = 0;
        mybreak[z].is_ok = true;

        if(BreakList[k]->amount_min[j] != BreakList[k]->amount_max[j])
        {
            mybreak[z].item.count = RandNumber(BreakList[k]->amount_min[j],BreakList[k]->amount_max[j]-BreakList[k]->amount_min[j]+1);
        }
        else
        {
            mybreak[z].item.count = BreakList[k]->amount_max[j];
        }
		//Log(MSG_DEBUG,"Item %i is number %i on the list. ID: %i. count %i", z+1, j, BreakList[k]->product[j], mybreak[z].item.count);
    }

	

    //Checking the slots.
    int nb_ok = 0;
    for(UINT z=0;z<MaxRewards;z++)
    {
        unsigned int tempslot = thisclient->AddItem(mybreak[z].item);
        if (tempslot == 0xffff)
        {
            Log(MSG_WARNING,"Can not give a reward to %s (not enough place for item %u*(%i::%i) )",thisclient->CharInfo->charname,mybreak[z].item.count,mybreak[z].item.itemtype,mybreak[z].item.itemnum);
            mybreak[z].is_ok = false;
        }
        else
        {
            nb_ok++;
            mybreak[z].slot = tempslot;
        }

    }

    //taking the broken item.
	thisclient->items[src].count -= 1;
	if( thisclient->items[src].count < 1)
		ClearItem( thisclient->items[src] );
	thisclient->UpdateInventory(src);

    //packet time.
	Log(MSG_WARNING,"Sending disassembly packet type %i",CraftRslt);
	BEGINPACKET( pak, 0x7bc );
	ADDBYTE    ( pak, CraftRslt );		//7 disassemble success, 19 Chest Congratulations
	ADDBYTE    ( pak, nb_ok + 1 );		//number of items to follow
    for(UINT z=0;z<MaxRewards;z++)
    {
        if(!mybreak[z].is_ok)
        {
            continue;
        }
        unsigned int tempslot = mybreak[z].slot;
        ADDBYTE (pak, tempslot);											// The new item that we just received
		pak = AddItemData(thisclient->items[tempslot], pak);
    }
	ADDBYTE ( pak, src );													// The item we just disassembled or the chest that we just used. Count has already been decremented
	pak = AddItemData(thisclient->items[src], pak);
	thisclient->client->SendPacket( &pak );
	return true;
}


//LMA: We give a banana or a bogus item to the player (disassemble).
bool CWorldServer::GiveDefaultDasm( CPlayer* thisclient,UINT src, bool not_found, bool is_failed)
{
   Log(MSG_WARNING,"Player %s tried to disassemble item (%i::%i), it's not in break list or in error! not found %i, failed %i",thisclient->CharInfo->charname,thisclient->items[src].itemtype,thisclient->items[src].itemnum,not_found,is_failed);

    //let's give him a banana for his trouble ;)
   CItem newitem;

    //LMA: we give him other items if item mall, event or unique gear.
   if(not_found)
   {
        int grade=0;
        if(thisclient->items[src].itemtype>9)
        {
            Log(MSG_WARNING,"Weird itemtype for disassemble %i::%i for %s",thisclient->items[src].itemtype,thisclient->items[src].itemnum,thisclient->CharInfo->charname);
        }
        else
        {
            if(thisclient->items[src].itemnum>=EquipList[thisclient->items[src].itemtype].max)
            {
                Log(MSG_WARNING,"Weird itemnum for disassemble %i::%i for %s (>= %u)",thisclient->items[src].itemtype,thisclient->items[src].itemnum,thisclient->CharInfo->charname,EquipList[thisclient->items[src].itemtype].max);
            }
            else
            {
                grade=EquipList[thisclient->items[src].itemtype].Index[thisclient->items[src].itemnum]->itemgrade;
            }

        }

        if(grade==13)
        {
            //item mall
            newitem.itemnum = 449;
            newitem.itemtype = 12;
            newitem.count = RandNumber(1,4);
        }
        else if(grade==14)
        {
            //event
            newitem.itemnum = 448;
            newitem.itemtype = 12;
            newitem.count = RandNumber(1,4);
        }
        else if(grade==11)
        {
            //unique
            //newitem.itemnum = RandNumber(392,394);
            newitem.itemnum = RandNumber(392,3);
            newitem.itemtype = 12;
            newitem.count = RandNumber(1,4);
        }
        else
        {
            //banana
            newitem.itemnum = 102;
            newitem.itemtype = 10;
            newitem.count = 1;
        }

   }
   else
   {
       //banana
        newitem.itemnum = 102;
        newitem.itemtype = 10;
        newitem.count = 1;
   }

   newitem.refine = 0;
   newitem.lifespan = 100;
   newitem.durability = 40;
   newitem.socketed=0;
   newitem.appraised=0;
   newitem.stats=0;
   newitem.gem=0;
   newitem.sp_value=0;
   newitem.last_sp_value=0;

   unsigned newslot = thisclient->AddItem(newitem);
   if(newslot == 0xffff)
   {
       //This should never happen since client is handling that.
       //We let the client crashes in this case ^_^
       return false;
   }

  thisclient->items[src].count -= 1;
  if( thisclient->items[src].count < 1)
      ClearItem( thisclient->items[src] );
  thisclient->UpdateInventory(src);

  BEGINPACKET( pak, 0x7bc );
  ADDBYTE    ( pak, 0x07 );//disassemble success
  ADDBYTE    ( pak, 0x02 );//number of items to follow
  ADDBYTE    ( pak, newslot );
  pak = AddItemData(thisclient->items[newslot], pak);
  //ADDDWORD   ( pak, BuildItemHead( thisclient->items[newslot] ) );
  //ADDDWORD   ( pak, BuildItemData( thisclient->items[newslot] ) );
  //ADDWORD    ( pak, 0x0000);
  //ADDWORD    ( pak, 0x0000);
  //ADDWORD    ( pak, 0x0000);
  ADDBYTE    ( pak, src );
  pak = AddItemData(thisclient->items[src], pak);
  //ADDDWORD   ( pak, BuildItemHead( thisclient->items[src] ) );
  //ADDDWORD   ( pak, BuildItemData( thisclient->items[src] ) );
  //ADDWORD    ( pak, 0x0000);
  //ADDWORD    ( pak, 0x0000);
  //ADDWORD    ( pak, 0x0000);
  thisclient->client->SendPacket( &pak );
   return true;
}

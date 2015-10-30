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
#ifndef _ROSE_PLAYER_
#define _ROSE_PLAYER_

//LMA: new refine system
#define REFINENEW

//LMA: test static client ID.
#ifndef STATICID
#define STATICID
#endif

//LMA: TEST HACK QUEST
#define QHACK
//LMA: End

#include "character.h"

//LMA: test for quest hack (stackable).
#ifdef QHACK
struct CquestCheck
{
    dword questid;
    clock_t die_time;
};
#endif
//LMA: end

class CPlayer: public CCharacter
{

    public:
        CPlayer( CClientSocket* );
        ~CPlayer( );

    //
    CClientSocket* client;
    bool questdebug;// Show Quest message in console?
    bool Saved;// already save this player?(used ondisconnect/returnchar)
    bool isInvisibleMode;

    //LMA: Pvp flag
    long int pvp_id;

    //LMA: Warp flag
    int map_warp_zone;
    fPoint Warp_Zone;

    //LMA: HP / MP regeneration by bonfires and others...
    int sp_hp;
    int sp_mp;
    int nb_hp;
    int nb_mp;

    //Fuel
    clock_t last_fuel;

   //LMA: last shout.
    time_t next_shout;
    bool spam_shout;
    bool spam_whisper;
    bool spam_chat;
    time_t next_chat;
    time_t next_whisper;
    int nb_chats;
    int nb_whispers;

    //LMA: for union chat spam
    bool spam_chat_union;
    time_t next_chat_union;
    int nb_chat_union;

    //LMA: for chat trade
    bool spam_chat_trade;
    time_t next_chat_trade;
    int nb_chat_trade;

    // Fairy
    bool Fairy;
    UINT FairyListIndex;

    // variables
    bool is_born;   //LMA: just came into play?
    bool is_invisible;  //LMA: for AIP.
    UINT uw_kills;  //LMA: for Union War.
    unsigned int hits;
    USEDITEM* UsedItem;
    INFO* CharInfo;
    RIDE* Ride;
    TRADE* Trade;
    PARTY* Party;
    SHOP* Shop;
    SESSION* Session;
    CLAN* Clan;
    ATTRIBUTES* Attr;
    CRespawnPoints GMRespawnPoints;

    //Medal xp
    int wait_validation;
    time_t timerxp;
    int bonusxp;
    bool once;

    //LMA: Null exp.
    time_t timer_no_exp;
    bool no_exp;

    //LMA: test for quest hack (stackable).
    #ifdef QHACK
    CquestCheck arr_questid[10];
    #endif
    //LMA: end

    //Medal of Fortune(id.1300/type.10)_One additional drop chance for defeating a monster is added (Double Drop -> ddrop)
    int wait_validation_ddrop;
    time_t timerddrop;
    int bonusddrop;
    bool once_ddrop;

    //Medal of Excellence(id.1301/type.10)_find equipment of excellence and will result in better bonus stats than normal (statdrop)
    int wait_validation_statdrop;
    time_t timerstatdrop;
    int bonusstatdrop;
    bool once_statdrop;

    //Medal of Retrieval(id.1302/type.10)_you are able to retreive items from monsters that would normally not drop items for you. (graydrop)
    int wait_validation_graydrop;
    time_t timergraydrop;
    int bonusgraydrop;
    bool once_graydrop;

    //Quest variables
    bool speaksLuna;
    bool canUseFlyingVessel;

    //LMA New Quests
    UINT QuestVariables[25];

    // Inventory/storage
    CItem storageitems[MAX_STORAGE];
    CItem itemmallitems[MAX_ITEMMALL];
    unsigned int nstorageitems;
    unsigned int nsitemmallitems;
    BSItem wishlistitems[MAX_WISHLIST];

    CItem items[MAX_INVENTORY];

    float attack_fuel;   //LMA: conso fuel.

    // skills/quickbar
    SKILLS cskills[MAX_ALL_SKILL];

    //LMA: % for dealer (when buying from NPC).
    int pc_rebate;

    //LMA: %craft talent (when crafting)
    int pc_craft_talent;

    //LMA: summon jauge.
    int summon_jauge;
    int cur_jauge;

    //LMA: union points bonus.
    int pc_up;

    int cur_max_skills[5];

    UINT quickbar[MAX_QUICKBAR];
    int p_skills;
    int dual_scratch_index; //LMA: trying to guess dual scratch skill index...

    // ExJam Quest Code - Info
    DWORD ActiveQuest;  //active quest #
    int CheckQuest; // The currently checked quest.
    strings TriggerName[10];
    SQuestData quest; // Character's quest information

    //LMA: for no qsd zone
    bool skip_qsd_zone;

    // Time
	clock_t lastRegenTime;
	clock_t lastRegenTime_hp;   //LMA: regen for HP
	clock_t lastRegenTime_mp;   //LMA: regen for MP
	clock_t lastShowTime;       //LMA HP Jumping
	clock_t firstlogin;         //LMA for fairy
	clock_t lastSaveTime;
    clock_t lastportalchecktime;  //custom events
    clock_t lastEventTime;        //custom events
    clock_t RefreshEventTimer;	  //custom events
	clock_t lastGG;

	// Visible Lists
    vector<CPlayer*>	        VisiblePlayers;	   // Visible players
    vector<CDrop*>		        VisibleDrops;	   // Visible drops

    //LMATEST
    //vector<class CMonster*>		    VisibleMonsters;   // Visible monsters
    vector<unsigned int>            VisibleMonsters;   // Visible monsters

    vector<CNPC*>			    VisibleNPCs;	   // Visible npcs

    // Functions
    bool CheckPlayerLevelUP( );
    bool CheckDoubleEquip( );  //LMA: Core fix for double weapon and shield
    bool CheckZulies( );

    #ifdef PYCUSTOM
    bool CheckPortal( );  //custom events
    bool CheckEvents( );  //custom events
    //bool CheckCustomQuest( );
    bool PrizeExchange(CPlayer* thisclient, UINT prizeid );  //custom events
    #endif

    bool CheckItems ( );
    void SetStats( );
    bool GetPlayerInfo( );
    bool IsMonster( );
    bool CleanPlayerVector( );
    bool loaddata( );
    void saveskills();  //LMA: Only saving skills there.
    void savedata( );
    int GoodSkill(int skill_id);    //LMA: which skill family?
    int FindSkillOffset(int family);    //LMA: Find a skill offset...
    bool FindSkill(int family,UINT skill); //LMA: Does a skill already exist?
    void ResetSkillOffset(); //LMA: reset skill offsets.
    void AttrAllSkills();   //LMA: searchs all the skills for a player (to be used after allskill gm command for example).
    void AttrGMSkills();   //DK: ???
    void SaveSkillInfo(int family,int offset,int id,int level);    //LMA: Save some skills informations for later...
    //void UpgradeSkillInfo(int offset,int skillid,int nb_upgrade);  //LMA: Upgrade a skill level...
    void CalculateSignature( int slot );    //LMA: get item signature
    int CheckSignature( int slot );         //LMA: check signature
    UINT GetNewStorageItemSlot( CItem thisitem );
    UINT GetPlayerSkill( unsigned int id ); // Required skill check by insider
    UINT GetNewItemSlot( CItem thisitem );
    bool ClearObject( unsigned int otherclientid );
    void RestartPlayerVal( );
    bool Regeneration( );
    bool PlayerHeal( );
    bool VisiblityList( );
    bool ForceRefreshMonster(bool refresh_all, UINT monster_id);      //LMA: force refresh (20070621-211100)
    bool SpawnToPlayer( CPlayer* player, CPlayer* otherclient );
    long int ReturnPvp( CPlayer* player, CPlayer* otherclient );    //LMA: for PVP.
    bool UpdateValues( );
    bool RefreshHPMP();  //LMA HP / MP Jumping
    void ReduceABC(int amount=1,bool do_packet=true);
    int SearchABC ( int slot ); //LMA: Searching for replacement arrows, bullets...
    CParty* GetParty( );
    CLAN* GetClan( );
    unsigned int AddItem( CItem item );
    void TakeFuel(int add_fuel=0);   //LMA: Using fuel :)
    void GiveCP(unsigned int points);    //LMA: Give Clan Points
    void UpdateInventory( unsigned int slot1, unsigned int slot2=0xffff, bool save=true);   //LMA: this one saves into database immediatly
    void UpdateInventoryNoSave( unsigned int slot1, unsigned int slot2=0xffff );   //LMA: This one doesn't immediatly save into database.
    void SaveSlot( unsigned int slot); //LMA: Saving slot into MySQL database.
    void SaveSlot41( unsigned int slot); //LMA: Saving slot into MySQL database (Mysql 4.1+ function).
    void reduceItemsLifeSpan( bool attacked);
    bool SaveQuest( QUESTS* myquest );       //LMA: Saving quests data (Mysql 4.1+ function).
    bool PlasticSurgeon(CQuest* thisquest);      //LMA: Plastic Surgery coupons
    bool CheckItem(int itemnb,int familyid,int nb);   //Check if a peculiar item is in inventory
    bool CheckStats(int slot, int dest_slot); //LMA: Checking if a stat is ok to equip an item.
    bool CheckJobs(int item_job);  //LMA: checking job requirements.

    // ExJam Quest Code
    void savequests( CPlayer* thisclient );
    SQuest* GetActiveQuest( );
    int GetQuestVar(short nVarType, short nVarNO);
    void SetQuestVar(short nVarType, short nVarNO, short nValue);
    //int ExecuteQuestTrigger(dword hash);
    int ExecuteQuestTrigger(dword hash, bool send_packet=false, UINT index=0);    //LMA: Special case, we send the "good" quest name.

    // Player Stats
    unsigned int GetAttackPower( );
    unsigned int GetDefense( );
    unsigned int GetDodge( );
    unsigned int GetAccury( );
    unsigned int GetMagicDefense( );
    unsigned int GetCritical( );
    unsigned int GetAttackSpeed( );
    unsigned int GetMoveSpeed( );
    unsigned int GetCartSpeed( );

    //unsigned int GetMaxHP( );
    unsigned long long GetMaxHP( );

    unsigned int GetMaxMP( );
    float GetAttackDistance( );
    unsigned int GetHPRegenAmount( );
    unsigned int GetMPRegenAmount( );
    unsigned int GetMaxWeight( );
    unsigned int GetCurrentWeight( );
    unsigned int GetMPReduction( );
    unsigned int GetMaxSummonGauge( );
    void GetExtraStats( );
    void GetExtraStatsSkills( );
    unsigned int GetLevelEXP( );
    unsigned int GetInt( );
    unsigned int GetSen( );
    unsigned int GetCon( );
    unsigned getWeaponType();
    //bool AddClanPoints(unsigned int count);
    void RebuildItemMall(); //LMA: Rebuild player's itemmall.
    CClientSocket* getClient();
};

#endif

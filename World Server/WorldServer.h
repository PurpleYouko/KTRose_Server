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
#ifndef __ROSE_SERVERS__
#define __ROSE_SERVERS__

#include "datatypes.h"
#include "../common/sockets.h"
#include "worldmonster.h"
#include "worldmap.h"
#include "player.h"
#include "character.h"
#include "party.h"

#include "stbhandler.h"

//STL handler.
#include "stlhandler.h"

#include "quest/strhashing.h"
#include "quest/CRoseArray.hpp"
#include "quest/CRoseFile.hpp"
#include "quest/CStrStb.hpp"
#include "quest/CIntStb.hpp"
#include "quest/QuestTrigger.h"

//LMA: LTB:
#include "ltbhandler.h"

#define MAXVISUALRANGE 100
#define MINVISUALRANGE 60
#define ClearItem(i) { i.appraised=false; i.count=0; i.durability=0; i.itemnum=0; i.itemtype=0; i.lifespan=0; i.refine=0; i.socketed=false; i.stats=0; i.gem=0; }

#define WORLD_THREAD 0
#define VISUALITY_THREAD 1
#define SHUTDOWN_THREAD 2

//LMA: Grids
#define NB_MAPS 124        //Nb maps (122 maps, including bad and non existant).
#define NB_GRIDS 96        //nb active grids (active maps)

//LMA: LTB.
#define LTBINDEX 0
#define LTBKOREAN 1
#define LTBENGLISH 2

//LMA: Refine Rules:
#ifdef REFINENEW
    #define NB_REF_RULES 31
#else
    #define NB_REF_RULES 17
#endif

//LMA: for monster slicer and drop_me
//define to use it.
//#define LMA_SPAWNM

//LMA: AIP
#include "aip/Aip.h"

//LMA TESTLOOP
extern UINT lma_loop;
extern UINT fmmonstertype;
extern UINT ftypeskill;
extern UINT fskill;


//LMA: test
	struct SQuestDatum1 {
		int size;
		int opcode;
		byte* data;
	};
//LMA: test.


// Main loginserver server class
class CWorldServer : public CServerSocket
{
    public:
        //LMA: static client ID and party ID.
        #ifdef STATICID
        static unsigned last_cid;
        static unsigned last_pid;
        #endif
    	//------------------ MAIN (worldserver.cpp)
    	CWorldServer ( string );
        CClientSocket* CreateClientSocket( );
    	~CWorldServer( );
    	bool OnReceivePacket( CClientSocket* thisclient, CPacket* P );
    	void OnClientDisconnect( CClientSocket* thisclient );
    	void DeleteClientSocket( CClientSocket* thisclient );
    	bool OnServerReady( );
    	void SpawnMonsters( );
    	void LoadConfigurations( const char* file);
        void LoadCommandLevels( void );
        void ServerLoop( );
        bool isBanned( sockaddr_in* ClientInfo );

        //LMA
        //Used for Grids
        CListMap allmaps[NB_MAPS];   //all maps (even those not used)
        CGridMap gridmaps[NB_GRIDS];  //only active maps
        //LMA End

      	//------------------ WORLD PROCESS (worldprocess.cpp)
      	//bool GiveExp( CMonster* thismon );
      	//LMA BEGIN
      	//20070621-211100
      	//FOR CF
      	bool GiveExp( CMonster* thismon, UINT special_lvl, UINT special_exp);
      	//LMA END

        //LMA BEGIN
        //20070623-232300
        bool Ping();        //MySQL Ping
        //LMA END

        //------------------ QUEST DATA (quest.cpp)
    	bool pakGiveQuest( CPlayer* thisclient, CPacket* P );

    	//------------------ BUFFS (buff.cpp)
        CBValue GetBuffValue( CSkills* thisskill, CCharacter* character, UINT Evalue, UINT i, UINT up, UINT down, UINT CurrentValue, bool Buff=true, bool Status=false );
        //CBValue GetBuffValue( CSkills* thisskill, CCharacter* character, UINT Evalue, UINT i, UINT up, UINT down, UINT CurrentValue, bool Buff=true );
        bool CheckABuffs( CSkills* thisskill, CCharacter* character, int Evalue ,int i);
        //bool CheckDBuffs( CSkills* thisskill, CCharacter* character, int Evalue ,int i);
        bool AddBuffs( CSkills* thisskill, CCharacter* character, int Evalue , bool flag);
        //bool AddDeBuffs( CSkills* thisskill, CCharacter* character, int Evalue );
        bool AddBuffs( CSkills* thisskill, CCharacter* character, int Evalue );
        bool InstantBuff( CSkills* thisskill, CCharacter* character, int Evalue, int i );
        UINT BuildUpBuffs( CCharacter* player );
        UINT BuildDeBuffs( CCharacter* player );


    	//------------------ SERVER EXTRAS (extrafunctions.cpp)
        bool IsValidItem(UINT type, UINT id );
        unsigned BuildItemShow(CItem thisitem);
        UINT GetUIntValue( const char* s , void* var=NULL );
        int GetIntValue( const char* s , void* var=NULL );
        char* GetStrValue( const char* s , void* var=NULL );
    	bool SendSysMsg( CPlayer* thisclient, string message );
    	UINT RandNumber( UINT init, UINT range, UINT seed=0 );
        UINT ReturnItemType( CPlayer* thisclient, int slot );   //LMA: returns a "type" from an item in a slot.
    	UINT GetColorExp( UINT playerlevel,UINT moblevel, UINT exp );
    	bool CheckInventorySlot( CPlayer* thisclient, int slot );
    	bool pakGMClass( CPlayer* thisclient, char* classid );
        bool pakGMKillInRange( CPlayer* thisclient, int range );
        bool pakGMnpcshout( CPlayer* thisclient, char* shan, char* aipqsd, int npctype, int ltbid );    //LMA: LTB use.
    	bool pakGMHide( CPlayer* thisclient, int mode );
    	void SendToVisible( CPacket* pak, CPlayer* thisclient, bool dothisclient=true );
    	void SendToVisible( CPacket* pak, CPlayer* thisclient, CPlayer* xotherclient );
    	void SendToVisibleAIP( CPacket* pak, CMonster* thismon, CDrop* thisdrop );  //LMA: used for AIP.

    	void SendToVisible( CPacket* pak, class CCharacter* character, CDrop* thisdrop=NULL );

    	bool IsMonInCircle( CPlayer* thisclient, CMonster* thismon, float radius );
        bool IsPlayerInCircle( CPlayer* thisclient, CPlayer* otherclient, float radius );
        CUseInfo* GetUseItemInfo(  CPlayer* thisclient, UINT slot );
    	bool IsVisible( CPlayer* thisclient, CPlayer* otherclient );
    	bool IsMonInCircle( fPoint center, fPoint position, float radius );
    	bool IsVisible( CPlayer* thisclient, CMonster* thismon );
    	bool IsVisible( CPlayer* thisclient, CDrop* thisdrop );
        CItem GetItemByHeadAndData( unsigned head, unsigned data );
    	bool IsVisible( CPlayer* thisclient, CNPC* thisnpc );
    	bool IsVisibleNPCType( CPlayer* thisclient, UINT npc_type );    //LMA: Used for AIP.
    	void SendToVisible( CPacket* pak, CMonster* thismon, CDrop* thisdrop=NULL );
    	void SendToVisible( CPacket* pak, CDrop* thisdrop );
        CPlayer* GetClientByUserName( char *username );
    	UINT BuildBuffs( CCharacter* player );
    	void CheckForLevelUp ( CPlayer* thisclient );
        bool isSkillTargetFriendly( CSkills* thisskill );
    	CRespawnPoint* GetRespawnByID( UINT id );
    	CPlayer* GetClientByCID( DWORD id, UINT map=0 );
    	CPlayer* GetClientByCharName( char* name );
    	CPlayer* GetClientByCharNameCI( char* name ); //LMA: search but case insensitive
    	CPlayer* GetClientByID( UINT id, UINT map=0 );
    	//CTeleGate* GetTeleGateByID( UINT id );
    	CTeleGate* GetTeleGateByID( UINT id, UINT offset=0);  //LMA: Telegates, with extra gate.
    	UINT BuildItemHead( CItem thisitem );
    	UINT BuildItemData( CItem thisitem );
    	CMonster* GetMonsterByID( UINT id, UINT map );
    	//CSpawnArea* GetSpawnArea( UINT id, UINT map=0 );
    	//CNMSpawns* GetSpawnArea( UINT id, UINT map=0 );
    	CSpawnArea* GetSpawnArea( UINT id, UINT map );
        CMobGroup* GetMobGroup( UINT id, UINT map=0 );
        bool DeleteSpawn( CSpawnArea* spawn );
        //bool DeleteSpawn( CNMSpawns* spawn );
        CNPCData* GetNPCDataByID( UINT id );
    	void SendToMap( CPacket* pak, int mapid );
    	bool IsUnionAlly( int union_player, int union_other_player);    //LMA: check if two players are in the same alliance.
    	void SendToUnionInMap( CPacket* pak, int mapid, int union_player);  //LMA: Send to ally (union) in a map.
        unsigned BuildItemRefine(CItem thisitem );
        CMDrops* GetDropData( UINT id );
    	CRespawnPoint* GetRespawnByMap( int id );
        CSkills* GetSkillByID( UINT id );
        CStatus* GetStatusByID( unsigned int id );  //getting status.
    	void ClearClientID( UINT id );
    	CDrop* GetDropByID( UINT id, UINT map=0 );
        CChest* GetChestByID( UINT id );
    	CNPC* GetNPCByID( UINT id, UINT map=0 );
    	char* GetNPCNameByType(UINT id);    //LMA: getting the npc name.
    	char* GetSTLObjNameByID(UINT family, UINT idorg,bool comment=false);   //LMA: getting the STL name for objects (1-14)
    	char* GetSTLMonsterNameByID(UINT idorg); //LMA: getting the STL name of a monster / NPC by ID.
        char* GetSTLItemPrefix(int family, UINT idorg);  //LMA: to get an item prefix.
        char* GetSTLQuestByID(UINT idorg);  //LMA: getting quest name by STL.
        char* GetSTLSkillByID(UINT idorg);  //LMA: to get a skill name :)
        char* GetSTLZoneNameByID(UINT idorg);   //LMA: get a zone name :)
        CNPC* GetNPCByType( UINT npctype );
        //CDrop* GetDrop( CMonster* thismon );
        CDrop* GetPYDrop( CMonster* thismon, UINT droptype );   //hidden
        CDrop* GetPYDropAnd( CMonster* thismon, UINT droptype );   //LMA: And system.
		CDrop* GetNewDrop( CMonster* thismon );						//KT drop system
    	void SendToAll( CPacket* pak );
    	void SendToAllInMap( CPacket* pak, int mapid);     //LMA: Send a message to all people in a given map
        UINT gi(UINT itemvalue, short type);    //LMA: to get a clean itemtype and itemnum.
        UINT getClanPoints(int clanid);   //LMA: We get the exact Clan Points amount through mysql database
        UINT getClanGrade(int clanid);    //LMA: We get the exact Clan Grade through mysql database
        UINT SummonFormula(CPlayer* thisclient,CMonster* thismonster);    //LMA: Formulas for summons.
        string EscapeMe(char* texte);  //LMA: Escaping.
        UINT GetTimerFromAIP(dword script_id, bool is_npc); //LMA: used to get a timer from a script.
        UINT GetBreakID(UINT itemnum,UINT itemtype);  //LMA: Getting the breakid from an item.
        int GetNbUserID( UINT userid ); //LMA: Used to count all avatars from a single userid.
        bool ForceDiscClient( unsigned int userid );  //LMA: Disc a client.
    	UINT GetNewClientID( );
    	unsigned GetNewPartyID( );  //LMA: Getting a Party ID.
    	void DisconnectAll();
    	CPlayer* GetClientByUserID( UINT userid );
        UINT GetLevelGhost( UINT map, UINT level );
        UINT GetFairyRange( UINT part );
        UINT GetGridNumber(int mapid, int posx, int posy);    //LMA: maps
        UINT GetGridNumber(int mapid, int posx, int posy, CMonster* thismonster);    //LMA: maps for monsters.
        bool SaveAllStorage( CPlayer* thisclient);    //LMA: Saving Storage (all).
        bool GetAllStorage( CPlayer* thisclient);     //LMA: Getting Storage (all).
        bool GetSlotStorage( CPlayer* thisclient,UINT slotnum);   //LMA: refreshing one slot from MySQL storage.
        bool SaveSlotStorage( CPlayer* thisclient,UINT slotnum);  //LMA: Saving one slot into MySQL storage.
        bool SaveSlotMall( CPlayer* thisclient,UINT slotnum);  //LMA: Saving one slot into MySQL Item Mall.
        bool GetZulyStorage( CPlayer* thisclient);     //LMA: Get Zuly from Storage (from MySQL)
        bool GetWishlist( CPlayer* thisclient);   //LMA: Getting Wishlist.
        bool SaveZulyStorage( CPlayer* thisclient);    //LMA: Save Zuly to Storage (to MySQL)
        CMonster* LookAOEMonster( CPlayer* thisclient);       //LMA: Trying to get a monster from a location (for AOE_TARGET paket mainly).
        CMonster* LookAOEMonster( class CCharacter* character);       //LMA: Character version.
        void ReturnItemMallList(CPlayer* thisclient);   //LMA: Return ItemMall List
        void TakeItemMallList(CPlayer* thisclient,int qty,int slot); //LMA: takes an item from Item Mall to player's inventory
        void RefreshFairy( );

        bool CheckValidName(CPlayer* thisclient, const char* my_name);    //LMA: Checking avatar name to avoid GM tags and not valid chars.
        bool EscapeMySQL(const char* data,string & mystring,int nb_car_max,bool check_same);   //LMA: escaping.
        bool CheckEscapeMySQL(const char* data,int nb_car_max,bool check_same);    //LMA: escaping (this version only checks escaped and unescaped versions of a string are the same).

        // PY extra stats lookup
        UINT GetExtraStats( UINT modifier );
        // PY end

        //------------------ Fairies ---------------------
        void DoFairyStuff( CPlayer* targetclient, int action );
        void DoFairyFree( int fairy );

    	//------------------ MATH (extrafunctions.cpp)
    	fPoint RandInCircle(fPoint center, float radius);
    	fPoint RandInPoly(fPoint p[], int pcount);
    	fPoint RandInTriangle(fPoint p1, fPoint p2, fPoint p3);
    	float AreaOfTriangle(fPoint p1, fPoint p2, fPoint p3);
    	float distance( fPoint pos1, fPoint pos2 );

    	//------------------ PACKETS (worldpackets.cpp)
    	bool pakItemMall( CPlayer* thisclient, CPacket* P );
    	bool pakWeight( CPlayer* thisclient, CPacket* P );
    	bool pakPrintscreen( CPlayer* thisclient, CPacket* P );
    	bool pakSpawnChar( CPlayer* thisclient, CPlayer* otherclient );
        bool pakModifiedItemDone( CPlayer* thisclient, CPacket* P );
    	bool pakSpawnDrop( CPlayer* thisclient, CDrop* thisdrop );
        bool pakChangeStorage( CPlayer* thisclient, CPacket* P );
        bool pakModifiedItem( CPlayer* thisclient, CPacket* P );
        bool pakPartyManager( CPlayer* thisclient, CPacket* P );
        bool pakLevelUpSkill( CPlayer *thisclient, CPacket* P );
    	bool pakChangeStance( CPlayer* thisclient, CPacket* P );
        bool pakCharDSClient( CPlayer* thisclient, CPacket* P );
    	bool pakChangeEquip( CPlayer* thisclient, CPacket* P );
    	bool pakStartAttack( CPlayer* thisclient, CPacket* P );
       	bool pakSpawnNPC( CPlayer* thisclient, CNPC* thisnpc );
        bool pakSpawnIfoObject( CPlayer* thisclient, UINT npctype,bool forcerefresh=false);    //LMA: Spawning IfoObjects.
        bool pakRideRequest( CPlayer* thisclient, CPacket* P );
        bool pakPartyActions( CPlayer* thisclient, CPacket* P );
        bool pakPartyOption( CPlayer* thisclient, CPacket* P );
    	bool pakTradeAction( CPlayer* thisclient, CPacket* P );
    	bool pakChangeCart( CPlayer* thisclient, CPacket* P );
        bool pakSkillSelf ( CPlayer* thisclient, CPacket* P );
    	bool pakDoIdentify( CPlayer *thisclient, CPacket *P );
    	bool pakNormalChat( CPlayer* thisclient, CPacket* P );
    	bool pakCharSelect( CPlayer* thisclient, CPacket* P );
    	bool pakChatUnion ( CPlayer* thisclient, CPacket* P );  //LMA: chat union.
        bool pakChatTrade ( CPlayer* thisclient, CPacket* P );  //LMA: chat trade.
    	bool pakStartSkill( CPlayer* thisclient, CPacket* P );
        bool pakPvp796( CPlayer* thisclient, CPacket* P );   //LMA: test for packet 0x796
        bool pakRepairItem( CPlayer* thisclient, CPacket* P,int packet_type);
    	bool pakMoveSkill( CPlayer* thisclient, CPacket* P );
        bool pakCloseShop( CPlayer* thisclient, CPacket* P );
        bool pakStoreZuly( CPlayer* thisclient, CPacket* P );
        bool pakPartyChat( CPlayer* thisclient, CPacket* P );
    	bool pakGameGuard( CPlayer* thisclient, CPacket* P );
        bool pakOpenShop( CPlayer* thisclient, CPacket* P );
        bool pakShowShop( CPlayer* thisclient, CPacket* P );
        bool pakSellShop( CPlayer* thisclient, CPacket* P );
    	bool pakMoveChar( CPlayer* thisclient, CPacket* P );
    	bool pakStopChar( CPlayer* thisclient, CPacket* P );
    	bool pakPickDrop( CPlayer* thisclient, CPacket* P );
    	bool pakAddStats( CPlayer* thisclient, CPacket* P );
        bool pakShowHeal( CPlayer* thisclient, CPacket* P );
        bool pakSkillAOE( CPlayer* thisclient, CPacket* P );
        bool pakidentify( CPlayer* thisclient, CPacket* P );
    	bool pakUserDied( CPlayer* thisclient, CPacket* P );
    	bool pakTradeAdd( CPlayer* thisclient, CPacket* P );
        bool pakUseItem ( CPlayer* thisclient, CPacket* P );
    	bool pakWhisper( CPlayer* thisclient, CPacket* P );
    	bool pakEquipABC(CPlayer* thisclient, CPacket *P );
        bool pakStorage( CPlayer* thisclient, CPacket* P );
        bool pakBuyShop( CPlayer* thisclient, CPacket* P );
    	bool pakDoEmote( CPlayer* thisclient, CPacket* P );
    	bool pakNPCBuy( CPlayer* thisclient, CPacket* P );
    	bool pakDoDrop( CPlayer* thisclient, CPacket* P );
    	bool pakShout( CPlayer* thisclient, CPacket* P );
    	bool pakCraft( CPlayer* thisclient, CPacket* P );
    	bool pakDoID( CPlayer* thisclient, CPacket* P );
    	bool pakGate( CPlayer* thisclient, CPacket* P );
    	bool pakChangeRespawn( CPlayer* thisclient, CPacket* P );
    	bool pakExpTC ( CPlayer* thisclient, CPacket* P );   //LMA: Bonus Exp Time Coupon
        bool pakExit( CPlayer* thisclient, CPacket* P );
    	bool pakPing( CPlayer* thisclient, CPacket* P );
      	bool SendLevelUPtoChar(CPlayer* thisclient );
       bool pakRepairTool( CPlayer* thisclient, CPacket* P );
        void pakPlayer( CPlayer *thisclient );
        void pakInventory( CPlayer *thisclient );
        void pakQuestData( CPlayer *thisclient );
    	void pakClearUser( CPlayer* thisclient );
//        bool LearnSkill( CPlayer* thisclient, UINT skill );
        bool LearnSkill( CPlayer* thisclient, UINT skill, bool takeSP = true );
        bool CheckCompatibleClass(UINT rclass, UINT player_job);
   		bool pakAddWishList( CPlayer* , CPacket* );
        bool GiveChestItems( CPlayer* thisclient,UINT chest_slot,CChest* thischest); //LMA: used for chests and slots.
        bool GiveDasmItems( CPlayer* thisclient,UINT src);    //LMA: Used for disassemble.
        bool GiveDefaultDasm( CPlayer* thisclient,UINT src, bool not_found, bool is_failed);

        //-------------------------- Clan functions
        bool pakClanManager ( CPlayer* thisclient, CPacket* P );
        bool pakCreateClan ( CPlayer* thisclient, CPacket* P );

    	//------------------ GM COMMANDS (gmcmds.cpp)
    	bool pakGMMoveTo( CPlayer* thisclient, fPoint position );
    	bool pakGMTeleAllHere( CPlayer* thisclient );
    	bool GMShowTargetInfo( CPlayer* thisclient );
    	bool pakGMServerInfo( CPlayer* thisclient );
    	bool pakGMHeal( CPlayer* thisclient );
    	bool pakGMStat( CPlayer* thisclient, const char* statname, int statvalue );
        bool pakGMItemtoplayer(CPlayer* thisclient, char* name , UINT itemid , UINT itemtype , UINT itemamount , UINT itemrefine , UINT itemls, UINT itemstats , UINT itemsocket) ;
        bool pakGMItem( CPlayer* thisclient, UINT itemid , UINT itemtype , UINT itemamount , UINT itemrefine , UINT itemls, UINT itemstats , UINT itemsocket );

        //LMA: for Quests
        bool pakGMItemQuest( CPlayer* thisclient, UINT itemid, UINT itemtype, UINT itemamount, UINT itemrefine, UINT itemls, UINT itemstats, UINT itemsocket, char buffer2[200] );
        bool pakGMTeleOtherPlayer( CPlayer *thisclient, char* name, int map, float x, float y );
        bool pakGMZuly( CPlayer* thisclient, int mode, int amount, char* charname );
        bool pakGMEventType(CPlayer* thisclient, int npctype, int dialog, long int type); //Event
        bool pakGMEventName(CPlayer* thisclient, char* eventname, int is_on); //Event (by its name)
        bool pakGMEventIFO(CPlayer* thisclient, int ifoType,int eventID);   //LMA: for Ifo Objects
    	bool pakGMTele( CPlayer* thisclient, int map, float x, float y, int no_qsd=0);
    	bool pakGMMon( CPlayer* thisclient, int montype, int moncount,int AIP = 0);
        //bool pakGMZulygive(CPlayer* thisclient, char* name, int zuly);
        bool pakGMZulygive(CPlayer* thisclient, char* name, long long zuly);
        bool pakGMFairyto(CPlayer* thisclient, char* name, int mode);
        bool pakGMClanRewardPoints(CPlayer* thisclient, char* name, int points);    //reward points
        bool pakGMClanPoints(CPlayer* thisclient, char* name, int points);    //clan points
        bool pakGMRaiseCG(CPlayer* thisclient); //LMA: Raising Clan Grade
        bool pakGMManageFairy(CPlayer* thisclient, int mode);
        bool pakGMHurtHim(CPlayer* thisclient, char* name);   //LMA: Gm command.
        bool pakGMChangeFairyWait(CPlayer* thisclient, int newvalue);
        bool pakGMChangeFairyStay(CPlayer* thisclient, int newvalue);
        bool pakGMChangeFairyTestMode(CPlayer* thisclient, int mode);
        bool pakGMTelePlayerHere( CPlayer* thisclient, char* name );
        bool pakGMAllSkill ( CPlayer* thisclient, char* name); // by crashinside
        bool pakGMDelSkills ( CPlayer* thisclient, char* name); // by rl2171
        bool pakGMObjVar(CPlayer* thisclient, int npctype, int output=1); //LMA: ObjVar for a NPC.
        bool pakGMSetObjVar(CPlayer* thisclient, int npctype, int index, int value);    //LMA: set an ObjVar for a NPC.
        bool pakGMForceUW(CPlayer* thisclient, int time); //LMA: forcing Union Wars.
        bool pakGMForceGemQuest(CPlayer* thisclient, int time); //LMA: forcing gem quest.
        bool pakGMForceUWPlayers(CPlayer* thisclient, int nb_players);   //LMA: forcing Union Wars (nb players).
        bool pakGMExportSTBSTL(CPlayer* thisclient);    //LMA: export STB and STL to a .sql file.
        bool pakGMSpawnList(CPlayer* thisclient, int range); //LMA: New spawn handling
        bool pakGMSpawnDetail(CPlayer* thisclient, UINT id, UINT map); //LMA: New spawn handling
        bool pakGMSpawnForceRefresh(CPlayer* thisclient, UINT id, UINT map);   //LMA: New spawn handling
        bool pakGMReborn( CPlayer* thisclient); //Reborn by Core
        bool pakGMLevel( CPlayer* thisclient, int level , char* name);
        bool pakGMTeleToPlayer( CPlayer* thisclient, char* name );
        bool pakGMDoEmote( CPlayer* thisclient, int emotionid );
    	bool pakGMCommand( CPlayer* thisclient, CPacket* P );
        bool ReloadMobSpawn( CPlayer* thisclient, int id );
        bool pakGMKick( CPlayer* thisclient, char* name );
    	bool pakGMAnn( CPlayer* thisclient, CPacket *P );
        bool pakGMBan( CPlayer* thisclient, char* name );
        bool pakGMInfo(CPlayer* thisclient, char* name);
        bool pakGMNpc(CPlayer* thisclient, int npcid, int dialogid,int eventid);
        bool pakGMUnion(CPlayer* thisclient, char* name, int which_union);      //LMA: UW.
        bool pakGMUnionPoints(CPlayer* thisclient, char* name, int nb_points);    //LMA: giving faction points
      	bool pakGMGotomap( CPlayer* thisclient, int map, bool no_qsd_zone=false);
      	bool pakGMMute( CPlayer* thisclient, char* name, int time);
      	bool pakGMPartylvl( CPlayer* partyclient, int level );
      	bool pakGMChangePlayerDmg(CPlayer* thisclient, int rate);
      	bool pakGMChangeMonsterDmg(CPlayer* thisclient, int rate);
      	bool pakGMChangeCfmode(CPlayer* thisclient, int mode);
      	bool pakGMWhoAttacksMe(CPlayer* thisclient);
      	bool pakGMChangeAtkSpeedModif(CPlayer* thisclient, int modif);
      	bool pakGMChangeHitDelayModif(CPlayer* thisclient, int modif);
      	bool pakGMChangeMSpeedModif(CPlayer* thisclient, int modif);
        bool pakGMBuff(CPlayer* thisClient, int strength); // by Drakia
        bool pakGMDebuff(CPlayer* thisClient); // by Drakia
        bool pakGMGiveBuff(CPlayer* thisClient, CPlayer* targetClient, int skillID, int strength); // by Drakia
        bool pakGMMaxStats(CPlayer* thisClient);
        bool pakGMGMSkills ( CPlayer* thisclient, char* name);

    	//------------------ CHAR SERVER COMMANDS (worldpackets.cpp)
    	bool TeleportTo ( CPlayer* thisclient, int map, fPoint position );
    	bool pakCSCharSelect( CPlayer* thisclient, CPacket* P );
    	bool pakCSReady( CPlayer* thisclient, CPacket* P );

    	//----------------- Server StartUp Functions (startup.cpp)
    	bool LoadSTBData( );

        bool LoadIfoObjects( ); //LMA: Loading Objects ID.

        //lma: initializing arrays default values...
        bool InitDefaultValues();

        bool LoadLTB( );            //LMA: LTB.
    	bool LoadZoneData( );
    	bool LoadGrids( );          //LMA: maps
    	bool LoadConsItem( );
    	bool LoadSellData( );
    	bool LoadProductItem( );
    	bool LoadPatItem( );
    	bool LoadNaturalItem( );
    	bool LoadJemItem( );
    	bool LoadEquip( );
    	bool LoadItemStats( );
    	bool LoadGrades();          //LMA: loading refines (grades) bonuses.
    	bool LoadJobReq( );         //LMA: Job requirements.
    	bool LoadStatLookup( );
        bool LoadTeleGateData( );
        bool LoadWayPoints( );      //PY load waypoints for TD

        #ifdef PYCUSTOM
        bool LoadCustomTeleGate( );
        bool LoadCustomEvents( );
        #endif

        //bool LoadMonsterSpawn( );
        bool LoadMobGroups( );
        //bool LoadMobGroupsSpecial( );   //LMA: Special Spawns (Halloween for example)
        bool LoadRespawnData( );

        // QSD Fuctions
        void ReadQSD(strings path, dword index);
        //void ExportQSDData(byte* dataorg,int opcode,int size,CQuestTrigger::SQuestDatum* dataraw);
        void ExportQSDData(byte* dataorg,int size,int opcode);  //LMA: exporting QSD conditions.
        void ExportQSDDataA(byte* dataorg,int size,int opcode);  //LMA: exporting QSD actions.
        char* Operators(byte btOp,char* buffer); //LMA: operators.
        char* Abilities(int btOp,char* buffer); //LMA: abilities.
        void LoadQuestData( );
        bool LoadQuestSTB();
        bool LoadBreakChestBlueList( );


        vector<CQuestTrigger*> TriggerList;
        fpQuestCond qstCondFunc[31];
        fpQuestRewd qstRewdFunc[35];

        //AIP:
		vector<CAip*> AipList;
        fpQuestCondC qstCondFuncC[31];
        fpQuestRewdC qstRewdFuncC[35];

        CSTBData                STB_QUEST;

        //PY spawning stuff
        UINT maxSpawnId;
        TowerDef WPList[100][100];                      //Way point list for tower def. [mapid][wp id]

        //LMA: AIP:
        inline int round(double x) {return int(x > 0.0 ? x + 0.5 : x - 0.5);};
        //vector<CAip*> AipList;
        map<dword,CAip*> AipListMap;    //LMA: testing maps...
        map<dword,UINT> AipListTimer;    //LMA: Timer for AIP...

        //map<UINT,UINT> NPC_AIP;       //LMA: NPC AIP.
        map<UINT,vector<UINT> > NPC_AIP;       //LMA: NPC AIP.
        map<UINT,vector<CMDrops*> > DropsAnd;       //LMA: Drops And system.

        //LMA: test for quest hack (stackable).
        #ifdef QHACK
        map<UINT,int> MapStackQuest;       //LMA: map of stackable quests.
        #endif
        //LMA: end

        fpAiCond aiCondFunc[32];
        fpAiAct aiActFunc[38];

        //int ObjVar[4000][20];  //NPC variables used in AI
        int ObjVar[MAX_NPC][20];  //NPC variables used in AI

        // AIP Functions
        void ReadAIP(strings path, dword index);
        void LoadAipData();

        //LMA: exporting.
        void ExportAipData(byte* dataorg,int size,int opcode);  //LMA: exporting AIP condition data
        void ExportAipDataA(byte* dataorg,int size,int opcode);  //LMA: exporting AIP actions data
        //LMA: END exporting.

        bool LoadNewDrops( );
		//bool LoadDropsData( );
        //bool LoadPYDropsData( );    //hidden
        //bool LoadPYDropsDataAnd( );    //hidden
        //bool LoadSkillBookDropsData( ); //hidden
        bool LoadConfig( );
        //bool LoadLTB( );
        bool LoadChestData( );
        bool LoadSkillDataOld( );  //LMA: Old version, for debug
        bool TimerForNPC( );    //LMA: loading timer for NPC and monsters
        bool LoadStatusData( ); //loading status.
        bool LMACheckSkills( );  //LMA: for skill debug
        bool LMACheckStuff();   //LMA: for different tests.
        bool LoadSkillData( );
        bool LoadMonsters( );
        bool LoadNPCData( );
        bool LoadQuestItemData( );
        bool LoadUpgrade( );
        bool LoadNPCs( );
        bool LoadNPCsSpecial( );
        bool LoadNPCsEvents( ); //LMA: loading events.
        bool CleanConnectedList( );
        bool LoadNMSpawns( );       //loading NM type spawns
        bool LoadMonsterSpawn( );   //KTRose monster loading

        CQuest* GetQuestByID( unsigned long int id );
        CQuest* GetQuestByQuestID( unsigned long int questid );
        CQuest* GetQuestByFinalID( unsigned long int questid );
        bool DoQuestScript( CPlayer* thisclient, CQuest* thisquest );
        QUESTS* GetQuestByItemID( unsigned long int itemid );

        // Server Functions
        //bool SendPM( CPlayer* thisclient, char msg[200] );
        bool SendPM( CPlayer* thisclient, const char* Format, ... );
        bool SendGlobalMSG( CPlayer* thisclient, char msg[200] );

        //LMA: AIP and custom events.
        //bool NPCShout( CMonster* thismon, char msg[200], char npc[50], int mapid=0);
        bool NPCShout( CMonster* thismon, char* msg, char* npc, int mapid=0);
        //bool NPCAnnounce( char msg[200], char npc[50] );
        bool NPCAnnounce( char* msg, char* npc, int mapid=0 );
        bool NPCShout2( CMonster* thismon, char* msg, char* npc );
        bool NPCWhisper( CPlayer* thisclient, CMonster* thismon, char msg[200], char npc[50] );
        bool NPCMessage( CPlayer* thisclient, char msg[200], char npc[50] );

        UINT GetMaxPartyExp( UINT partylevel );
        bool DoSkillScript( CCharacter* character, CSkills* thisskill );

        bool AddParty( CParty* );
        bool RemoveParty( CParty* );

    	UINT				    ClientIDList[0x10000];	// Clients List
    	bool				    PartyIDList[0x1000];     //LMA: Party IDs
    	SOCKET					csock;					// Socket for accessing the char server
    	char*					cct;					// Encryption table for char server

    	vector<CTeleGate*>		TeleGateList;			// Telegates List
        CStatus                 **StatusList;
        int                     maxStatus;
        int                     maxZone;                //LMA: max map amount
    	map<int,char*>          NpcNameList;            //LMA: npc name map.
    	map<UINT,char*>         STLNameList;            //LMA: Name for all things taken from STL.
        map<UINT,char*>         STLCommentList;            //LMA: Comment for all things taken from STL.
    	IfoObject               WarpGate;               //LMA: Warp Gate for Union.
    	int                     LastTempleAccess[2];    //LMA: For temple Access.
    	word                    UWForceFrom;            //LMA: Forcing Union Wars (test)
    	word                    GemQuestForce;          //LMA: Forcing gem quest.
    	int                     GemQuestReset;          //LMA: Forcing gem quest to reset spawn counter.
    	int                     UWNbPlayers;            //LMA: Forcing Nb players required for UW.
        vector<CCustomGate*>    CustomGateList;         // Custom Telegate list
        vector<CCustomEvent*>   CustomEventList;        //Custom events list

        vector<CQuest*>         QuestList;              // Quest List
        vector<CSkills*>        SkillListOld;           // Skills List (LMA: for debug)

        CSkills                 **SkillList;
        int                     maxSkills;              //Nb Skills

        CGrade                 **GradeList;
        int                     maxGrades;              //Nb grades (refines)

        vector<CMDrops*>        MDropList;              // Drops List
        vector<CMDrops*>        SkillbookList;          // Skillbook drop list (hidden)

        CNPCData                **NPCData;
        CQuestItemData          **QuestItemData;
        int                     maxNPC;                 //Nb NPC/Mobs
        int                     maxQuestItems;          //Nb quest Items.
        vector<UINT>            **ClassList;            //LMA: to store class requirements.
        int                     maxClass;               //LMA: Nb of class requirements.

        vector<CParty*>         PartyList;              // Party List
        vector<CFairy*>         FairyList;              // Fairy List
        vector<CChest*>         ChestList;              // Chest List

        map<int,vector<int> >    ListAllNpc;             //LMA: we store all the NPC in a single map.

       // CItemStas               StatsList[500];
        CItemStas               **StatsList;            //LMA: without limits...
        int                     maxStats;               //Nb Stats.
        //CExtraStats             StatLookup[301];        //PY: Item Stats
        CExtraStats             StatLookup[MAX_EXTRA_STATS];        //PY: Item Stats

        //UINT                    upgrade[2][10];

        #ifdef REFINENEW
            UINT                    upgrade[16];         //LMA: new way (after 2010/05)
            UINT                    refine_grade[NB_REF_RULES];    //lma: refining rules.
        #else
            UINT                    upgrade[10][2];         //LMA: new way (before 2010/05)
            UINT                    refine_grade[NB_REF_RULES][2];    //lma: refining rules.
        #endif

        //UINT                    refine_grade[15][2];    //lma: refining rules.
        CEquipList              EquipList[10];
        CJemList                JemList;
        CNaturalList            NaturalList;
        CPatList                PatList;
        CProductList            ProductList;
        CSellList               SellList;
        CUseList                UseList;
        CMapList                MapList;

        LTBData                 MyLTB;                  //LMA: LTB
        LTBData                 MyLTBQSD;               //LMA: LTB for QSD
        CLTBstring              **Ltbstring;            //LMA: LTB
        CLTBstring              **LtbstringQSD;         //LMA: LTB for QSD
        int                     maxltbaip;              //LMA: max aip ltb
        int                     maxltbqsd;              //LMA: max qsd ltb

        //LMA: test for quest hack (stackable).
        #ifdef QHACK
        CSTBDataChar			STB_NPC;				// NPC data
        #else
        CSTBData				STB_NPC;				// NPC data
        #endif


        CSTBData                STB_SKILL;              // Skill data
        CSTBData                STB_STATUS;             // Status Data
        //CSTBData				STB_QUEST;				// Quest data - already declared in line 393
        CSTBData				STB_ITEM[14];			// Item data
        CSTBData                STB_DROP;               // Drop list
        //CSTBData                STB_ZONE;               // Zone data
        CSTBData                STB_PRODUCT;            // Crafting data
        CSTBData                STB_SELL;               // npc sell index
        //CSTBData                upgradeData;            // Upgrade - refine data (LMA: we use a csv file now).
        CSTBData                BreakData;              // Break - Used for Break list, chests and blue crafts.
        CSTBDataChar            ZoneData;               // LMA: zone Data.
        CSTBData                ListClass;              // LMA: used to store the class list, actually to check equip requirements.
        CSTBData                ListGrade;              // LMA: used to store the refine bonuses (% and +).

    	clock_t				   lastServerStep;			// Last Update
    	clock_t                LastUpdateTime;          // Store the last update World Time
    	bool                   ServerOnline;
        string filename;

        pthread_t WorldThread[3];
        pthread_t MapThread[200];
        pthread_attr_t at;
        pthread_mutex_t MapMutex,SQLMutex,PlayerMutex;

        CDatabase* DB;

        //Test thingies
        unsigned ATTK_SPEED_MODIF;
        unsigned HIT_DELAY_MODIF;
        unsigned MOVE_SPEED_MODIF;
       // geo edit for disassemble // 22 oct 07
        UINT BreakListSize;

        //bool LoadBreakList();

        //lma: putting a limit.
        //CBreakList BreakList[3000];
        //CBreakList BreakList[MAX_BREAK];
        //vector<CBreakList*>         BreakList;
        CBreakList                 **BreakList;   //LMA: new way.
        int                     maxBreak;

        // console
        //bool handleCommand( char* );		//PY had to comment this out to stop a linker error
        bool pakConsoleAnn( char* from, char* message );

};
extern class CWorldServer* GServer;

PVOID VisibilityProcess( PVOID TS );
PVOID MapProcess( PVOID TS );
PVOID WorldProcess( PVOID TS );
PVOID ShutdownServer( PVOID TS );

void StartSignal( );
void StopSignal( );
void HandleSignal( int num );
// -----------------------------------------------------------------------------------------

#endif

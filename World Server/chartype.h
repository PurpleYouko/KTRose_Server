/*
    Rose Online Server Emulator
    Copyright (C) 2006,2007,2008,2009 OSRose Team http://www.dev-osrose.com

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

    developed with Main erose/hrose source server + some change from the original eich source
*/
#ifndef _CHARACTER_TYPE_
#define _CHARACTER_TYPE_
#define MAX_STORAGE 160
#define MAX_ITEMMALL 48
#define MAX_WISHLIST 30
#define MAX_INVENTORY 140
#define MAX_BASIC_SKILL 42
#define MAX_QUICKBAR 48
//#define ClearBattle(i) { i->target=0;i->atktarget=0; i->bufftarget=0; i->skilltarget=0; i->skillid=0; i->atktype=0; i->contatk = false;  }
#define ClearBattle(i) { i->target=0;i->atktarget=0; i->bufftarget=0; i->skilltarget=0; i->skillid=0; i->atktype=0; i->contatk = false; i->hitby=0;  }
typedef unsigned char BYTE;
#include "datatypes.h"

#define TPLAYER 1
#define TMONSTER 2
#define TSUMMON 3

//**************** SHARED ****************//
struct POSITION
{
    fPoint source;          // source position
    fPoint current;         // current position
    fPoint destiny ;         // destiny  position
    fPoint aoedestiny;       //LMA: AOE destiny
    unsigned int Map;       // current map
    unsigned int respawn;   // respawn id [player = respawnid | monster = spawnid]
    unsigned int saved;     // saved id [player town | monster = 0]
    clock_t lastMoveTime;
    //unsigned int arriveDelay;    // delay caused by various dmg while travelling (mostly
    //bool stop;
    //fPoint battle;
};

struct UNION_s	//PY used for union variables and reward points
{
    UINT unionvar;
};

//LMA: For UW.
struct UWPOSITION
{
    fPoint source;
    unsigned int Map;
};

struct STATS
{
    int Level;
    long HP;
    long MP;
    long MaxHP;
    long MaxMP;
    int stance;
	int CheatMaxHP;
	int CheatMaxMP;
	unsigned int CheatMoveSpeed;
	int CheatAttackPower;
    int Attack_Power;
	unsigned int CheatDefense;
    unsigned int Defense;
	unsigned int CheatCritical;
    unsigned int Critical;
	unsigned int CheatDodge;
    unsigned int Dodge;
	unsigned int CheatAccuracy;
    unsigned int Accury;
	unsigned int CheatMagicDefense;
    unsigned int Magic_Defense;
    unsigned int Move_Speed;
	unsigned int Mspd_base;
    unsigned int Base_Speed;
	unsigned int CheatAttackSpeed;
    unsigned int Attack_Speed;
    float Attack_Distance;
	unsigned int CheatXPRate;
	unsigned int xprate;
	unsigned int CheatItemDropRate;
    unsigned int itemdroprate;
	unsigned int CheatZulyDropRate;
	unsigned int zulydroprate;
	unsigned int itemdropcountrate;
    unsigned int MaxWeight;
	unsigned int CheatMaxSummonGauge;
    unsigned int MaxSummonGauge;
    unsigned int MPReduction;
    unsigned int side; //future expansion of drop system
    unsigned int sidechance; //future expansion of drop system
    unsigned int ExtraDamage_add;
	unsigned int CheatMagicAttack;
	unsigned int magicattack;
	int ItemXPRate;
    int ItemDropRate;
    clock_t ItemXPTimer;
    unsigned int ItemXPTime;
    clock_t ItemDropRateTimer;
//    unsigned int ShieldDamage;
};

struct BATTLE
{
    unsigned int target;
    unsigned int atktarget;
    unsigned int bufftarget;
    unsigned int skilltarget;
    unsigned int skillid;
    unsigned int atktype;
    unsigned int hitby;
    bool contatk;
    clock_t lastAtkTime;
    clock_t castTime;
};

struct STATUS
{
    //Stats up
    BYTE Attack_up;
    BYTE Defense_up;
    BYTE Magic_Defense_up;
    BYTE Accury_up;
    BYTE Critical_up;
    BYTE Dodge_up;
    BYTE Haste_up;
    BYTE Dash_up;
    BYTE HP_up;
    BYTE MP_up;
    BYTE ShieldDamage_up;
    BYTE ExtraDamage_up;

    //Stats down
    BYTE Attack_down;
    BYTE Defense_down;
    BYTE Magic_Defense_down;
    BYTE Accury_down;
    BYTE Critical_down;
    BYTE Dodge_down;
    BYTE Haste_down;
    BYTE Dash_down;
    BYTE HP_down;
    BYTE MP_down;
    BYTE ShieldDamage_down;
    BYTE ExtraDamage_down;

    //LMA: ospRose skill.
    //Status Effects
    BYTE Poisoned;
    BYTE Muted;
    BYTE Sleep;
    BYTE Faint;
    BYTE Stuned;
    BYTE Camo;
    BYTE Invis;
    BYTE Shield;
    BYTE Curse;
    BYTE Recover;//Purify
    BYTE Dispell;
    BYTE Detect;
    BYTE Invinc;
    //BYTE Flame;
    BYTE Flamed;
    BYTE Taunt;
    BYTE Weary; //Stealth(hawker skill. 1626-1629) 2nd deBuff (can't skill)

    //Status
    //BYTE Sleep;
    BYTE Stun;
    BYTE Poison;
    BYTE Mute;
    //BYTE Flame;
    BYTE Stealth;
    BYTE Cloaking;

    bool CanAttack;
    bool CanRun;
    bool CanMove;
    bool CanCastSkill;
    bool IsTowerDef;
    bool dayonly;
    bool nightonly;

    /*//Status Madifiers
    BYTE Stun;
    BYTE Poison;
    BYTE Mute;*/

    UINT spawnid;
    // Stance
    BYTE Stance;
};

struct MAGICSTATUS
{
    unsigned int Status;      // Store the skill Status
    unsigned int Buff;        // Store the buff type
    unsigned int Value;       // Buff Value
    unsigned int Duration;    // Skill Duration
    clock_t BuffTime; // Buff Time
};

//**************************************************************/
//***************Player only******************************************/
struct ATTRIBUTES
{
    unsigned int Str;
    unsigned int Dex;
    unsigned int Int;
    unsigned int Con;
    unsigned int Cha;
    unsigned int Sen;

    unsigned int Estr;
    unsigned int Edex;
    unsigned int Eint;
    unsigned int Econ;
    unsigned int Echa;
    unsigned int Esen;

};

struct USEDITEM
{
    clock_t lastRegTime; // Last Regeneration
    UINT usevalue; // Max Heal Value
    UINT usetype;  // Consumible Type (MP/HP/STAMINA/...)
    UINT userate;  // consumible rate
    UINT used;     // Consumible Used
};

// Player information
struct INFO
{
	char charname[17];
	DWORD charid;
    long int rewardpoints;    //LMA: reward points
    int unionid;              //LMA: Union
    int unionfame;            //LMA: Union
    int union01;              //LMA: Union
    int union02;              //LMA: Union
    int union03;              //LMA: Union
    int union04;              //LMA: Union
    int union05;              //LMA: Union
    int nb_kills;             //LMA: Union
	UINT unionvar[11];		//PY: new array to hold all the union points and stuff
    BYTE Sex;
    UINT Face;
    UINT Hair;
    long Exp;
	UINT Pending_Exp;		//temporary storage space for exp during calculations
	float HighestOverkill;	//just a bit of fun. We can track highest ever overkill rate just for giggles. Maybe give rewards or something
    UINT Job;
    //long int Zulies;
    //long int Storage_Zulies;
    //DevilKing's update for more than 2 billion zuly
    __int64 Zulies;
    __int64 Storage_Zulies;
    long int LastGlobal;
    int StatPoints;
    int SkillPoints;
    unsigned int stamina;
    unsigned int MaxStamina;

    // event tracking
    bool ActiveEvent;
    bool isGM; // GM Security
    bool isDev; // protect specific dev only commands
};

struct SESSION
{
	unsigned int userid;
	char username[17];
	char password[33];
	int accesslevel;
	bool isLoggedIn;
	bool inGame;
	UINT Respawned;
	BYTE RespawnChoice;
    bool codedebug;
    bool first_id;
	UINT KTPoints;
    UINT NewPoints;
    UINT logtime;		// tracks total log points. 1 point = 1 hour. 5 points = 1KTPoint
    clock_t LogTime;	// time for the current session
    UINT TotalLogTime;
    UINT award;
};

struct RIDE
{
    bool Drive;
    bool Ride;
    DWORD charid;
};

struct TRADE
{
	unsigned int short trade_itemid[0xa];
	unsigned int trade_count[0xb];
	unsigned int trade_status;
	unsigned int trade_target;
};

struct PARTY
{
	class CParty* party;
	bool IsMaster;
};

struct SHOP
{
    bool open;
    char name[64];
    CBuying BuyingList[30];
    CSelling SellingList[30];
    UINT Buying;
    UINT Selling;
    UINT ShopType;
    time_t mil_shop_time;   //LMA: Timer for shop time (mileage).
};

struct SKILLS
{
	unsigned id;
	unsigned level;
	time_t cooldown_skill;   //LMA: cooldown for skills.
	CSkills* thisskill;
};

struct QUESTS
{
    unsigned long int questid;
    int items[5];
    CQuest* thisquest;
    bool active;
};

struct CLAN
{
    unsigned int CP;
    unsigned int clanid;
    unsigned int clanrank;
    unsigned int grade;
    unsigned int logo;
    unsigned int back;
    char clanname[17];
};



//***************Monster only******************************************/

#endif


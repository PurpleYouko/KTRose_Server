/*
    Rose Online Server Emulator
    Copyright (C) 2006,20072008,2009 OSRose Team http://www.dev-osrose.com

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
#ifndef __ROSE_DATATYPES__
#define __ROSE_DATATYPES__

//LMA: new refine system
#ifndef REFINENEW
#define REFINENEW
#endif

//LMA: test static client ID.
#ifndef STATICID
#define STATICID
#endif

//LMA: TEST HACK QUEST
#ifndef QHACK
#define QHACK
#endif
//LMA END

//For QSD system.
#include "datatypes_compat.h"

//*upgraded status
//#define NULL 2
//#define STRENGTH 10
//#define DEXTERY 11
//#define INTELIGENCE 12
//#define CONCENTRATION 13
//#define CHARM 14
//#define SENSE 15
//#define C_HP 16
//#define C_MP 17
//#define ATTACK_POWER 18
//#define DEFENCE 19
//#define ACCURY 20
//#define MAGIC_RESISTENCE 21
//#define DODGE 22
//#define MOVE_SPEED 23
//#define ATTACK_SPEED 24
//#define INV_CAPACITY 25
//#define CRITICAL 26
//#define HP_REC_RATE 27         //HP Recovery Rate
//#define MP_REC_RATE 28         //MP Recovery Rate
#define MP_CONSUME 29          //MP MP Consumption
#define XP_RATE 30            //Experience Rate
//#define LEVEL 31               //Level
//#define POINT 32               //Point
//#define TENDENCY 33            //Tendency
//#define PK_LVL 34              //PK Level
//#define HEAD_SIZE 35           //Head Size
//#define BODY_SIZE 36           //Body Size TAKE CARE 36 is ATM, EXTRA_DAMAGE
//#define SP 37                  //SP
//#define MAX_HP 38              //Max HP
//#define MAX_MP 39              //Max MP
//#define MONEY 40               //Money
//#define UNARMED_AP 41          //Unarmed AP
//#define EXTRA_DAMAGE 36
#define ONE_HAND_WEAP_AP 42
#define TWO_HAND_WEAP_AP 43
#define BOW_WEAP_AP 44         //Bow AP
#define GUN_WEAP_AP 45         //Gun AP
#define MAGIC_WEAP_AP 46       //Magic Weapon AP
#define CBOW_WEAP_AP 47        //CrossBow AP
#define COMBAT_WEAP_AP 48      //Combat Weapon AP
#define BOW_WEAP_ASPD 49       //Bow Atk Speed
#define GUN_WEAP_ASPD 50       //Gun Atk Speed
#define COMBAT_WEAP_ASPD 51    //Combat Weapon Atk Speed
#define MOV_SPEED 52
#define DEFENSE 53
#define MAX_HP 54
#define MAX_MP 55
#define HP_REC_AMONT 56
#define MP_REC_RATE 57
#define BAGPACK_CAPACITY 58
//#define SALES_DISCOUNT 59      //Sales Discount
//#define SALES_PREMIUM 60       //Sales Premium
#define MP_COST_RED 61
#define SUMMON_GAUGE 62
//#define ITEM_DROP_RATE_INC 63  //Item Drop Rate Increase
//#define PLANET_REQ 75          //Planet Requirements
//#define STAMINA 76             //Stamina
//#define STORAGE_FREE 94        //No Storage Charge
//#define STORAGE_EXP 95         //Storage Expansion
//#define SHOP_REMOD 96          //Personl Shop Remodeling
//#define CART_GAUGE 97          //Cart Gauge
#define MAGIC_RESISTENCE_2 98    //Magic Resistance
#define ATK_ACCURACY 99
#define CRITICAL 100           //Critical
#define DODGE 101              //Dodge
#define SHIELD_DEFENSE 102
//#define IMMUNITY 103          //Immunity
#define ONE_HAND_SWORD_AP 104
#define ONE_HAND_MELEE_AP 105
#define TWO_HAND_SWORD_AP 106
#define TWO_HAND_SPEAR_AP 107
#define TWO_HAND_AXE_AP 108
#define BOW_AP 109
#define GUN_AP 110
#define LAUNCHER_AP 111
#define STAFF_POWER 113
#define WAND_POWER 114
#define KATAR_AP 115
#define DUALSWORD_AP 116
#define CBOW_AP 117                  //CrossBow AP
#define ONE_HAND_SWORD_ATK_SPD 118   //One Hand Sword Atk Speed
#define ONE_HAND_MELEE_ATK_SPD 119   //One Hand Melee Atk Speed
#define TWO_HAND_SWORD_ATK_SPD 120   //Tow Hand Sword Atk Speed
#define TWO_HAND_SPEAR_ATK_SPD 121   //Tow Hand Spear Atk Speed
#define TWO_HAND_AXE_ATK_SPD 122     //Tow Hand Axe Atk Speed
#define BOW_ATK_SPD 123
#define GUN_ATK_SPD 124
#define LAUNCHER_ATK_SPD 125
#define DUAL_GUN_ATK_SPD 126        //Dual Gun Atk Speed
#define STAFF_ATK_SPD 127           //Staff Atk Speed
#define WAND_ATK_SPD 128            //Wand Atk Speed*/
#define KATAR_ATK_SPD 129
#define DUALSWORD_ATK_SPD 130
#define CBOW_ATK_SPD 131            //Crossbow Atk Speed
//#define EXP_ITEM 133                //EXP Item
//#define ITEM_DROP_RATE_INC 134      //Item Drop Rate Increase
//#define ITEM_DROP_IMPROV 135        //Improved Monster Dropped Equipment
//#define ITEM_DROP_GREY 136          //Item Drop from Grey Monsters

//*status/condition
#define A_STR 10
#define A_DEX 11
#define A_INT 12
#define A_CON 13
#define A_CHA 14
#define A_SEN 15
#define A_HP 16
#define A_MP 17
#define A_ATTACK 18
#define A_DEFENSE 19
#define A_ACCUR 20
#define A_MRESIST 21
#define A_DODGE 22
#define A_DASH 23
#define A_HASTE 24
#define A_INVENTORY_CAPACITY 25
#define A_CRITICAL 26
#define A_HP_REC_RATE 27
#define A_MP_REC_RATE 28
#define A_MP_CONSUMATION 29
#define A_STEALTH 33
#define A_CLOAKING 34
#define A_Shield_Damage 35
#define A_Extra_Damage 36
#define A_MAX_HP 38
#define A_MAX_MP 39
#define A_STUN 40
#define A_MUTE 41
#define A_POISON 42
#define A_SLEEP 43
#define A_FLAMED 58
#define A_ADDDMG 83

//*spawn values
//BYTE1
#define UNK1 0x01
#define MAX_HP_MP 0x02//revizar
#define POISED 0x04
#define POISONED 0x04
#define FLAMED 0x08
//#define UNK2 0x08
#define HP_UP 0x10//revizar
#define MP_UP 0x20//revizar
//#define MP_UP_HP_UP 0x30
#define DASH_UP 0x40
#define DASH_DOWN 0x80
//BYTE2
#define HASTE_UP 0x01
#define HASTE_DOWN 0x02
#define ATTACK_UP 0x04
#define ATTACK_DOWN 0x08
#define DEFENSE_UP 0x10
#define DEFENSE_DOWN 0x20
#define MDEFENSE_UP 0x40
#define MDEFENSE_DOWN 0x80
//BYTE3
#define HITRATE_UP 0x01
#define HITRATE_DOWN 0x02
#define CRITICAL_UP 0x04
#define CRITICAL_DOWN 0x08
#define DODGE_UP 0x10
#define SUMMON 0x20
#define MUTED 0x40
#define WEARY 0x40      //Stealth(hawker skill. 1626-1629) 2nd deBuff (can't skill)
#define SLEEPING 0x80
//BYTE4
#define STUN 0x01
//Invisible (Devilking)
#define STEALTH 0x02
#define CLOAKING 0x04
//#define INVISIBLE_1 0x02
//#define INVISIBLE_2 0x04
#define UNK3 0x08
#define DAMAGE_UP 0x10
#define SHIELD_DAMAGE 0x40 // Test for the shield effect if is the right one
#define SUMMON 0x20
#define UNK4 0x40
#define UNK5 0x80

//Stance
#define mRUNNING 1
#define ATTACKING 1
#define WALKING 2
#define RUNNING 3
#define DRIVING 4

//Target Type
#define T_MONSTER 1
#define T_PLAYER 2
#define T_SELF 3
#define T_PARTY 4
#define T_OTHER 5

// attack type
#define NORMAL_ATTACK 1
#define SKILL_ATTACK 2
#define SKILL_BUFF 3
#define SKILL_AOE 4
#define SKILL_SELF 5
#define BUFF_SELF 6
#define BUFF_AOE 7
#define AOE_TARGET 8
#define STAY_STILL_ATTACK 15

//LMA for monsters
#define SUMMON_BUFF 16
#define MONSTER_BUFF_SELF 17
#define MONSTER_SKILL_ATTACK 18
#define MONSTER_SKILL_BUFF 19

// other constants
#define MAX_CLASS_SKILL 60
//LMA: driving, unique, mileage...
#define MAX_ALL_SKILL 362
#define MAX_DRIVING_SKILL 30
#define MAX_UNIQUE_SKILL 30
#define MAX_MILEAGE_SKILL 200
#define SKILL_DELAY 500 //ms

//Job Class ID
#define VISITOR 0
#define SOLDIER 111
#define KNIGHT 121
#define CHAMPION 122
#define MUSE 211
#define MAGE 221
#define CLERIC 222
#define HAWKER 311
#define RAIDER 321
#define SCOUT 322
#define DEALER 411
#define BOURGEOIS 421
#define ARTISAN 422

// weapon type
#define ONE_HAND_SWORD 211
#define ONE_HAND_BUNT_SWORD 212
#define TWO_HAND_SWORD 221
#define SPEAR 222
#define TWO_HAND_AXE 223
#define BOW 231
#define GUN 232
#define LAUNCHER 233
#define STAFF 241
#define WAND 242
#define KATAR 251
#define DOUBLE_SWORD 252
#define DUAL_GUN 253
#define CROSSBOW 271

//equip type
#define MASK 1
#define CAP 2
#define BODY 3
#define GLOVE 4
#define SHOE 5
#define BACK 6
#define JEWEL 7
#define WEAPON 8
#define SUBWEAPON 9
#define CONSUMIBLE 10
#define JEM 11
#define NATURAL 12
#define QUEST 13
#define PAT 14

//time
#define DAY 0
#define MORNING 1
#define EVENING 2
#define NIGHT 3

//LMA: maps
#define NB_CELL_MAX 400

// Status Type (osPRose)
#define sNone 0
#define sPoison1 7
#define sPoison2 8
#define sPoison3 9
#define sPoison4 10
#define sPoison5 11
#define sMaxHPUp 12
#define sMaxMPUp 13
#define sDash 14
#define sSlow 15
#define sHasteAtk 16
#define sSlowAtk 17
#define sAtkUp 18
#define sAtkDown 19
#define sDefUp 20
#define sDefDown 21
#define sMResUp 22
#define sMResDown 23
#define sAccUp 24
#define sAccDown 25
#define sCritUp 26
#define sCritDown 27
#define sDodgeUp 28
#define sDodgeDown 29
#define sMute 30
#define sSleep 31
#define sFaint 32
#define sCamo 33
#define sInvis 34
#define sShield 35
#define sAddDmg 36
#define sCurse 37
#define sRecover 38
#define sDispell 39
#define sHPMPRest 40
#define sHPMPRest2 41
#define sHPMPRest3 42
#define sHPDown 43
#define sMaxHPUp2 44
#define sMaxMPUp2 45
#define sDash2 46
#define sHasteAtk2 47
#define sAtkUp2 48
#define sDefUp2 49
#define sMResUp2 50
#define sAccUp2 51
#define sCritUp2 52
#define sDodgeUp2 53
#define sAddDmg2 54
#define sDetect 55
#define sTaunt 56
#define sInvinc 57
#define sFlame 58
#define sDefDown2 59
#define sSlow2 60
#define sFlame2 61
#define sHPRes100 62
#define sHPRes200 63
#define sHPRes300 64
#define sHPRes40  65
#define sHPRes80  66
#define sHPRes120 67
#define sMPRes40  68
#define sMPRes80  69
#define sMPRes120 70
#define sMPRes10  71
#define sMPRes15  72
#define sMPRes20  73
#define sFlame3 74
#define sHPRes160 75
#define sMPRes25  76
#define sFlame4 77
#define sFlame5 78
#define sFlame6 79
#define sFlame7 80
#define sAddDmg3 83


//LMA: Max hp mob
#define MAXHPMOB 0xFFFFFFFF
#define MAX_NPC 4000    //max npc amount in Objvar.
#define MAX_EXTRA_STATS 302   //max stats (for Py's code).

#include "../common/sockets.h"

// Hold party experience when kill a moster
struct CPartyExp
{
    class CParty* thisparty;
    //UINT exp;
    unsigned long long exp;
    UINT num;
    /*UINT partymember[50];*/
    //UINT partymember[7];            //LMA: Only 7 people allowed in a party.
    DWORD partymember[7];            //LMA: Only 7 people allowed in a party.
    UINT maxlevel;
    int cheat_min_lvl; //LMA: cheat test max lvl
    int cheat_max_lvl; //LMA: cheat test min lvl
    bool flag;
};

// Equip information
struct CEquip
{
    UINT id;
    UINT is_mileage;
    UINT equiptype;
    UINT type;
    UINT price;
    UINT pricerate;
    UINT weight;
    UINT quality;
    UINT craft;
    UINT craft_level;
    UINT level;
    UINT material;
    UINT craft_difficult;
    UINT occupation[3];
    UINT condition1[3];
    UINT condition2[3];
    UINT stat1[2];
    UINT stat2[2];
    UINT defense;
    UINT magicresistence;
    UINT attackdistance;
    UINT attackpower;
    UINT attackspeed;
	UINT magicattack;
    UINT itemgradeID;   //LMA: Used for refine.
    UINT itemgrade;
    UINT movespeed;
    UINT STLId;
    UINT STLPrefix;
	UINT raretype;
	UINT unionpoint;
    UINT breakid;   //LMA: used for breakid.
};

// Jem Data
struct CJemData
{
    UINT id;
    UINT type;
    UINT price;
    UINT pricerate;
    UINT weight;
    UINT craft;
    UINT craft_level;
    UINT quality;
    UINT material;
    UINT craft_difficult;
    UINT stat1[2];/**/
    UINT stat2[2];/**/
    UINT STLId;
};

// natural data
struct CNaturalData
{
    UINT id;
    UINT type;
    UINT price;
    UINT pricerate;
    UINT weight;
    UINT quality;
    UINT pricevalue;
    UINT STLId;
    UINT craft_difficult;   //LMA: Used for union prices...
};

// pat data
struct CPatData
{
    UINT id;
    UINT type;
    UINT price;
    UINT pricerate;
    UINT weight;
    UINT quality;
    UINT craft;
    UINT craft_level;
    UINT material;
    UINT craft_difficult;
    UINT parttype;
    UINT partversion;
	UINT level;
	UINT condition[2];
    UINT jauge;
    UINT modifier[2];
    UINT options[2];
    UINT val_options[2];
    UINT maxfuel;
    UINT fuelcons;
    UINT speed;
    UINT attackdistance;
    UINT attackpower;
    UINT attackspeed;
    UINT STLId;
	UINT durability;
	UINT unionpoint;
};

struct CProductData
{
    UINT id;
    UINT item_0_family;
    UINT item_1_family;
    UINT item_2_family;
    UINT item_3_family;
    /*UINT item[50];
    UINT amount[50];*/
    UINT item[4];
    UINT amount[4];
};

struct CCSellData
{
    UINT id;
    UINT item[48];
};

struct CUseData
{
    UINT id;
    UINT restriction;
    UINT type;
    UINT price;
    UINT pricerate;
    UINT is_mileage;
    UINT weight;
    UINT quality;
    UINT craft; //LMA: for craft.
    UINT craftlevel;    //LMA: for craft.
    UINT material; //core
    UINT craft_difficult; //LMA
    UINT usecondition[2];
    UINT useeffect[2];
    UINT pricevalue;
    UINT cooldown_type;  //LMA: cooldown type.
    UINT cooldown;  //LMA: cooldown.
    UINT STLId;
};

//LMA: QuestItemData
struct CQuestItemData {
    UINT id;
    UINT STLId; //LMA: STL ID
};

// List Mob Data
struct CNPCData {
    UINT id;
    UINT life;
    UINT wspeed;
    UINT rspeed;
    BYTE stance;    //LMA: AIP?
    //UINT dspeed; //dspeed is monster size at startup.cpp...
    UINT weapon;
    UINT subweapon;
    UINT level;
    //UINT hp;
    unsigned long hp;
	unsigned long MaxHP;
    UINT atkpower;
    UINT hitrate;
    UINT defense;
    UINT magicdefense;
	UINT magicattack;
    UINT dodge;
    float atkspeed;
    UINT AI;
    UINT AiTimer;
    //UINT exp;
    unsigned long exp;
  	UINT dropid;
  	UINT money;
  	UINT item;
  	UINT tab1;
  	UINT tab2;
  	UINT tab3;
  	UINT specialtab;
    float atkdistance;
    UINT aggresive;
    UINT helpless;  //LMA: doesn't fight back when attacked.
    UINT shp;
    UINT dialogid;
    UINT eventid;
    UINT askills[4];   //Attack skills
    UINT bskills[4];   //self buff skills
    UINT dskills[4];   //debuff skills
    UINT sigskill;     //signature
    clock_t lastskill; //last skill time
    UINT delayskill;   //delay between two skills
    UINT side;  //hidden
    UINT sidechance;  //hidden
    UINT refNPC;    //LMA: AIP?
    UINT STLId; //LMA: STL ID

    //LMA: test for quest hack (stackable).
    #ifdef QHACK
    dword die_quest;
    #endif
    dword _EntityType;
};

// Store the damage for monster to give exp
struct MonsterDamage
{
    //UINT charid;
    DWORD charid;
    //long int damage;
    long damage;
};

struct CDropInfo
{
    BYTE type;
    DWORD item;
    QWORD prob;
};

//LMA: Drop info And system.
struct CDropInfoAnd
{
    int type;
    int item;
    int prob;
    int alternate[8];
    int nb_alternate;
};

// For store the drops info
struct CMDrops
{
    UINT id;
    UINT zuly;
    UINT zulyprob;
    vector<CDropInfo*> Drops;
    UINT level_min;//for map drops
    UINT level_max;//for map drops
    UINT level_boss;//for map drops
    UINT probmax;
    UINT prob;  //hidden
    UINT map;   //hidden
    UINT mob;   //hidden
    UINT itemtype;   //hidden
    UINT itemnum;   //hidden
    UINT alt[8];   //hidden
	//experimental structure for New Drops
    UINT Dropid;
    UINT item[100];
    UINT chance[100];
    dword RunningTotal[100];
    dword TotalChance;
    float a_x;   //LMA: defining an area.
    float a_y;
    float b_x;
    float b_y;
};

// For store the Buff info
struct CBuff
{
    unsigned int Buff;//store the buff type
    unsigned int value1;//buff value 1 (direct)
    unsigned int value2;//buff value 2 (%)
    unsigned int Evalue;//Extra value 1 (buffer int +)
    unsigned int Duration; //Skill Duration
    clock_t bufftime;//Buff Time
};

// -----------------------------------------------------------------------------------------
// For store the Buff info
// -----------------------------------------------------------------------------------------
struct CBuffValue
{
    int value1;
    int value2;
    int value3;
};

// -----------------------------------------------------------------------------------------
// For store the Buff info
// -----------------------------------------------------------------------------------------
struct CPSkill
{
    int buff[3];//Buff type
    int value1[3];
    int value2[3];
};

// -----------------------------------------------------------------------------------------
// For store the clan info
// -----------------------------------------------------------------------------------------
struct SClan
{
    int id;
    int logo;
    int back;
    int grade;
    unsigned int cp;
    char name[50];
    char slogan[100];
    char news[200];
};

// -----------------------------------------------------------------------------------------
// A 2d point, for positions
// -----------------------------------------------------------------------------------------
struct fPoint {
	float x;
	float y;
	float z; // Unused?
};

// -----------------------------------------------------------------------------------------
// GM Respawn Mobs  (50 points max)
// -----------------------------------------------------------------------------------------
struct CRespawnPoints
{
    int d;//flag to duplicate
    int b;//flag to store
    int n;//num points
    int min;//mob min
    int max;//mob max
    int map;//map
    int mobID;//mob id
    int respawntime;//respawn time
    fPoint points[50];//points
};

// -----------------------------------------------------------------------------------------
// An item that a client owns (qsd version)
// -----------------------------------------------------------------------------------------
class CItem 
{
public:
    unsigned short GetPakHeader( );
    unsigned GetPakData( );
    unsigned GetVisInfo( );
    void Clear( );

    inline bool IsStackable(){
        return (itemtype >= 10) && (itemtype <= 13);
    };

    UINT    itemnum;
    UINT    itemtype;
    UINT    refine;
    UINT    lifespan;
    UINT    durability;
    bool    socketed;
    bool    appraised;
    UINT    count;
    UINT    stats;
    UINT    gem;
    UINT    durabLeft;
    long int sig_head;
    long int sig_data;
    int sig_gem;
    int sp_value;
    int last_sp_value;
	UINT    type;
	UINT    raretype;
};

//LMA: Used for boxes and chests
struct CItemChests {
    CItem item;
	unsigned int slot;
	bool is_ok;
};


// -----------------------------------------------------------------------------------------
// Selling / buying items
// -----------------------------------------------------------------------------------------
struct BSItem {
    int slot;
    int count;
    long int price;
    UINT head;
    UINT data;
};

// -----------------------------------------------------------------------------------------
// A typical npc
// -----------------------------------------------------------------------------------------
struct CNPC {
	unsigned short clientid;
	fPoint pos;
	float dir;
	//unsigned char posMap; //LMA: problem
	unsigned short posMap;
	unsigned npctype;
	CNPCData* thisnpc;
	unsigned dialog;
	long int event;
	clock_t lastAiUpdate;   //LMA: AIP.
};

// Item data object
struct CItemData
{
	unsigned type;
	unsigned id;
};


//LMA: For LTB.
struct NPCLTB
{
    string name;
    string sentence;
};

struct LTBData
{
    int nb_sentence;
	vector <NPCLTB> record;
};

struct CLTBstring
{
    /*
    char LTBstring[200];
    char NPCname[50];
    */
    char* LTBstring;
    char* NPCname;
};

struct Dialog
{
    long int dialognb;
    DWORD offsetd;
    WORD lengthd;
};

// -----------------------------------------------------------------------------------------
// Skill Data
// -----------------------------------------------------------------------------------------
struct CSkills
{
    UINT id;
    UINT skillnumber;
    UINT level;
    UINT type;
    UINT skill_tab;
    UINT skilltype; // New
    UINT range;
    UINT target;
    UINT status[3]; // New LMA: 3 for osRose...
    UINT aoerange; // New
    UINT hostilitycheck;    //PY New and possibly not used
    UINT successrate; // New
    UINT costtype[2]; // New
    UINT costamount[2]; // New
    UINT duration;
    UINT formula;//skill type magical or weapon
    UINT atkpower;
    UINT mp;
    UINT costtype2; // New
    UINT costamount2; // New
    UINT cooldown; // New
    // zone warp data
    UINT WarpZone;  //applies to Return Scrolls
    UINT WarpX;
    UINT WarpY;
    //end

    UINT SummonMobID;
    UINT ProgressType;
    UINT weapon[5];
    UINT rskill[3];
    UINT lskill[3];
    UINT c_class[4];
    UINT req[2]; // New
    UINT reqam[2]; // New
    UINT zuly;   // New
    UINT clevel;
    UINT success;
    UINT sp;
    UINT buff[3];
    UINT value1[3];
    UINT value2[3];
    UINT nbuffs;
    UINT BlockSkill;
    UINT Unlocks[4];
    UINT UnlockLevel[4];
    UINT SkillBook;
    unsigned short int aoe;
    UINT aoeradius;
    UINT script;
    UINT svalue1;
    UINT gm_aoe;
    UINT STLId;
};

//LMA: Grade structure.
struct CGrade {
    UINT atk_percent;
    UINT atk_addbonus;
    UINT acc_percent;
    UINT acc_addbonus;
    UINT def_percent;
    UINT def_addbonus;
    UINT mdef_percent;
    UINT mdef_addbonus;
    UINT dodge_percent;
    UINT dodge_addbonus;
};


//LMA: IFO Objects
struct IfoObject
{
    UINT id;
    UINT virtualNpctypeID;
    fPoint IfoPosition;
    UINT direction;
    UINT mapid;
    bool hidden;
    int IfoObjVar[20];
    int IfoX;
    int IfoY;
    UINT clientID;
    CNPCData* NPCData;
    CNPC* Npc;
};

// -----------------------------------------------------------------------------------------
// Status Object
// -----------------------------------------------------------------------------------------
struct CStatus
{
  UINT id; //
  UINT type; // Unk [1]
  //UINT duplication; // Unk [2]  wrong and not used
  UINT stackable;   //can we stack it? 0 = NO. 1 = YES
  UINT dir; // 0: Up 1: Down 2: ? [3]
  UINT repeat; // 1: Repeat 2: Once 3: Special [4]
  UINT ability[2]; // Status # to call? [5][7]
  UINT amount[2]; // # to increase/decrease by? [6][8]
  UINT decrease; // 1: decrease 0: increase [17]
};

// -----------------------------------------------------------------------------------------
// Sell data object
// -----------------------------------------------------------------------------------------
struct CSellData
{
	unsigned id;
	CItem items[48];
};

// -----------------------------------------------------------------------------------------
// User skill info
// -----------------------------------------------------------------------------------------
struct CSkill
{
	unsigned id;
	unsigned level;
	CSkills* thisskill;
};

// -----------------------------------------------------------------------------------------
// Quest structure
// -----------------------------------------------------------------------------------------
struct CQuest
{
    int id;
    unsigned long int finalid;
    unsigned long int itemid[10];
    UINT QVoffset; //LMA
    UINT QVvalue; //LMA
    BYTE startItems[10];
    unsigned long int questid;
    int mobs[10];
    int items[10];
    int numitems[10];
    int Itemreward[10];
    int ItemType[10];
    int CountItem[10];
    int CItem[5];
    int ZulieReward;
    int ExpReward;
    int script;
    UINT value1;
    UINT value2;
    UINT value3;
};

// -----------------------------------------------------------------------------------------
// An item or zuly dropped on the ground
// -----------------------------------------------------------------------------------------
struct CDrop
{
	unsigned short clientid;
	unsigned char posMap;
	fPoint pos;
	char type;
	unsigned amount;
	CItem item;
	time_t droptime;
	unsigned short owner;
    class CParty* thisparty;
};

// -----------------------------------------------------------------------------------------
// pvp zones
// -----------------------------------------------------------------------------------------
struct CPVPZones
{
    int map;
};
// -----------------------------------------------------------------------------------------
// A teleport gate object
// -----------------------------------------------------------------------------------------
struct CTeleGate
{
	unsigned short id;
	fPoint src;
	unsigned char srcMap;
	fPoint dest;
	unsigned char destMap;
	UINT offset;
};


//------------------------------------------------------------------------------------------
// Custom events, quests and games
//------------------------------------------------------------------------------------------

struct CCustomString
{
    char prizename[50];
};

struct CCustomEvent
{
    unsigned short id;
    UINT eventtype;
    char npcname[50];
    fPoint location;
    unsigned short map;
    UINT active;
    UINT prizetype[10];
    UINT prizeid[10];
    UINT prizecost[10];
    CCustomString prizename[10];
    UINT collecttype;
    UINT collectid;
    char script1[200];
    char script2[200];
    char script3[200];
    char script4[200];
    UINT radius;
    char itemname[20];
    bool inuse;
    UINT level;
};

struct CCustomGate
{
  unsigned short id;
  fPoint source;
  unsigned short sourcemap;
  fPoint dest;
  unsigned short destmap;
  bool active;
  unsigned short radius;
};


// -----------------------------------------------------------------------------------------
// A respawn point object
// -----------------------------------------------------------------------------------------
struct CRespawnPoint
{
	unsigned short id;
	fPoint dest;
	//unsigned char destMap;  //LMA: problem...
	unsigned short destMap;
	unsigned char radius;
	bool masterdest;
};

// -----------------------------------------------------------------------------------------
// A monster spawn zone
// -----------------------------------------------------------------------------------------
struct CNMSpawns  //experimental spawn type PY
{
    UINT ID;
    UINT Map;
    UINT MonType;
    UINT MonCount;
    fPoint point;
    UINT idInMap;
};

struct CSpawnArea
{
	UINT id;
	UINT type;
    //0 = inactive
    //1 = normal
    //2 = cascade monster triggered by specific trigger defined in typeoftrigger
    //3 = cascade to max. As 2 but all possible mobs are spawned up to the spawn maximum
    //4 = minion spawns. i.e. scarabs around a Guardian Tree. don't use a spawn area but spawn around the boss
    //5 = minion spawns to max. As 4 but all possible mobs are spawned to the spawn maximum.
    UINT typeoftrigger;
    //0 = disabled
    //1 = triggered from any event in the map
    //2 = triggered only from events in a specific spawn
    UINT limit;
    //0 = day or night spawns permitted
    //1 = daytime spawns only
    //2 = nighttime spawns only
    UINT spawnkey;
    //0 = no key defined
    //1 = On spawn
    //2 = On die
    //3 = On being in a battle ( Isonbattle() )
    //4 = On target mob being alive
	UINT map;
	UINT min;
	UINT max;
	UINT respawntime;
	UINT montype;
	UINT pcount;
	UINT triggertype;
	UINT triggeramount;
	UINT triggercount;
	UINT aggressive;
	UINT amon;
	fPoint* points;
	fPoint point;
	UINT radius;
	clock_t lastRespawnTime;
	CNPCData* thisnpc;
	CMDrops* mobdrop;
	CMDrops* mapdrop;
	UINT AI;
    UINT RefVar;
    UINT RefVal;
};
struct CMob {
  UINT id;
  UINT mobId;
  bool tactical;
  UINT amount;
  UINT real_amount; //LMA: real amount (amount is the max amount).
  CNPCData* thisnpc;
    CMDrops* mobdrop;
    CMDrops* mapdrop;
};

struct CMobGroup {
  UINT id;
  UINT map;
  UINT limit;
  UINT active;
  UINT tacticalpoints;
  UINT daynight;    //LMA: day, night or both?
  UINT respawntime;
  UINT basicKills;
  UINT lastKills;   //LMA: last kills
  bool group_ready; //LMA: ready to spawn?
  UINT curTac;
  UINT curBasic;
  fPoint point;
  UINT range;
  clock_t lastRespawnTime;
  vector<CMob*> basicMobs;
  vector<CMob*> tacMobs;
};

struct CUseInfo
{
    int itemnum;
    int itemtype;
    int usescript;
    int usetype;
    int usevalue;
    int use_buff;
    int cooldown_type;  //LMA: cooldown.
    int cooldown;   //LMA: cooldown.
};

struct CItemType
{
    int itemnum;
    int itemtype;
    int probability;
};

struct CItemSlot
{
    UINT newslot[2];
    UINT nslot;
    bool flag;
};

// Buying List (Shop)
struct CBuying
{
    CItem item;
    long int price;
    UINT slot;
    UINT count;
};

// Selling List (Shop)
struct CSelling
{
    UINT slot;
    UINT count;
    long int price;
};

// MONSTER <---------------------------
struct MPosition // Monster position
{
    UINT Map;
    UINT SpawnID;
    fPoint Spos; // Start position
    fPoint Cpos; // Current Position
    fPoint Dpos; // destiny  Position
};

struct MDrop    // monster drops
{
    bool drop;
    //unsigned int firsthit;
    DWORD firsthit;
    unsigned int firstlevel;
	CMDrops* mobdrop;
	CMDrops* mapdrop;
};

struct CReward
{
    unsigned int id;
    unsigned int type;
    unsigned int prob;
    unsigned int rewardamount_min;
    unsigned int rewardamount_max;
    unsigned int rewardposs;
};

struct CChest
{
    unsigned int chestid;
    vector<CReward*> Rewards;
    unsigned int probmax;
    unsigned int rewardamount;
    unsigned int reward_min;
    unsigned int reward_max;
    int nb_reward;
    UINT breakid;
    //unsigned int rewardposs;    //LMA: TODO: strip, left for compatibility for now
};

struct CBValue
{
    UINT NewValue;
    UINT Value;
    UINT Position;
};


// ITEMS
struct CEquipList
{
    CEquip **Index;
    int max;
    //vector<CEquip*> Data;
    CEquip* nullequip;
	UINT STBMax;
};

struct CJemList
{
    CJemData **Index;
    int max;
    //vector<CJemData*> Data;
    //std::map<int,CJemData*> DataMap; //LMA: testing map.
    CJemData* nulljem;
};

struct CNaturalList
{
    CNaturalData **Index;
    int max;
    //vector<CNaturalData*> Data;
    CNaturalData* nullnatural;
};

struct CPatList
{

    CPatData **Index;
    int max;
    //vector<CPatData*> Data;
    CPatData* nullpat;
};

struct CProductList
{
    CProductData **Index;  //LMA: Sometimes 2000 wasn't enough, constant based now.
    int max;
    //vector<CProductData*> Data;
    CProductData* nullproduct;
};

struct CSellList
{

    CCSellData **Index;
    int max;
    //vector<CCSellData*> Data;
    CCSellData* nullsell;
};

struct CUseList
{
    CUseData **Index;
    int max;
    //vector<CUseData*> Data;
    CUseData* nulluse;
};

struct CMapList
{
    class CMap **Index;
    int max;

    vector<class CMap*> Map;
    class CMap* nullzone;
};

struct CItemStas
{
  UINT stat[2];
  UINT value[2];
};

//PY stat lookup table
struct CExtraStats
{
 UINT id;
 UINT statnumber;
 };
 //PY end


struct CFairy
{
    //UINT ListIndex;
    DWORD ListIndex;
	clock_t LastTime;
    UINT WaitTime;
    bool assigned;
};

//LMA: for map grids:
struct CGridMap
{
    //int coords[NB_CELL_MAX]; //10*10 map grid (8*8 for map and a 1 border) (now it can change according to map size).
    int *coords; //real size.
    int length;       //length of the map
    int width;       //Width of the map
    bool always_on;              //always display or not? (default 0).
    int org_x;                   //point of Origine (never 0,0 and sometimes not the same)...
    int org_y;
    UINT nb_cells;
};

struct CListMap
{
    int grid_id;      //Grid ID
    bool always_on;              //always display or not? (default 0).
    int nb_col;                //nb col (according to MINVISUALRANGE)
    int nb_row;                //nb row (according to MINVISUALRANGE)
};

//LMA End

//////////////////////// Geo edit for disassembles 22 oct 07
//LMA: We need 20 items now...
struct CBreakList
{
    UINT itemnum;
    UINT itemtype;
    UINT product[20];
    UINT amount_min[20];
    UINT amount_max[20];
    UINT prob[20];
    //UINT numToGive;
    //UINT total;
    UINT reward_min;
    UINT reward_max;
    int nb_reward;
};

// Quests (qsd)
struct SQuest
{
    word QuestID;
    dword StartTime;
    word Variables[10];
    byte Switches[4];
    CItem Items[5];
    byte unknown[6];

    bool GetSwitchBit( dword switchId )
    {
        dword byteId = switchId / 8;
        dword bitId = switchId % 8;
        return ((Switches[byteId] >> bitId) & 1);
    };

    void SetSwitchBit( byte switchId, byte value )
    {
        dword byteId = switchId / 8;
        dword bitId = switchId % 8;
        Switches[byteId] = Switches[byteId] | ((value?1:0) << bitId);
    };

    //LMA: Let's cheat ;)
    /*void AddItem(CItem* item, byte btOp = 2)
    {
        for(dword i = 0; i < 6; i++){
            if(Items[i].GetPakHeader() == item->GetPakHeader()) {
                if(btOp == 1){
                    Items[i].count += item->count;
                    return;
                }else if(btOp == 0){
                    if(Items[i].count <= item->count)
                        Items[i].Clear();
                    else
                        Items[i].count -= item->count;
                    return;
                }
            }
            if(btOp == 0) continue;
            if(Items[i].GetPakHeader() != 0) continue;
            Items[i].itemnum = item->itemnum;
            Items[i].itemtype = item->itemtype;
            Items[i].count = item->count;
            return;
        }
    };
    */

    UINT AddItem(CItem* item, byte btOp = 2)
    {
        for(dword i = 0; i < 6; i++){
            if(Items[i].GetPakHeader() == item->GetPakHeader()) {
                if(btOp == 1){
                    Items[i].count += item->count;
                    return Items[i].count;
                }else if(btOp == 0){
                    if(Items[i].count <= item->count)
                    {
                        Items[i].Clear();
                        return 0;
                    }
                    else
                    {
                        Items[i].count -= item->count;
                        return Items[i].count;
                    }

                }
            }
            if(btOp == 0) continue;
            if(Items[i].GetPakHeader() != 0) continue;
            Items[i].itemnum = item->itemnum;
            Items[i].itemtype = item->itemtype;
            Items[i].count = item->count;
            return Items[i].count;
        }
    };


    void Clear(){
        memset(this, 0, sizeof(SQuest));
    };

};

struct SQuestData
{
    word EpisodeVar[5];
    word JobVar[3];
    word PlanetVar[7];
    word UnionVar[10];

    SQuest quests[10];
    byte flags[0x40];
    //CNPC* selectedNpc;    //LMA: outdated.
    word ClanVar[5];   //LMA: Clan Var (at the end to preserve the existing records...)
    int RefNPC;

    void SetFlag( dword flagid, bool value ){
        dword byteid = flagid / 8;
        dword bitid = flagid % 8;

        //LMA: buggy, doesn't comes back to 0
        //flags[byteid] = flags[byteid] | ((value?1:0) << bitid);

        if(value)
        {
            flags[byteid] = flags[byteid] | (1 << bitid);
        }
        else
        {
            if (GetFlag(flagid))
            {
                flags[byteid] = flags[byteid] ^ (1 << bitid);
            }

        }

    }

    bool GetFlag( dword flagid ){
        dword byteid = flagid / 8;
        dword bitid = flagid % 8;
        return ((flags[byteid] >> bitid) & 1);
    }
};

//QSD End

struct TowerDef
{
    fPoint pos;
    float X;
    float Y;
    UINT WPType;
};

#endif


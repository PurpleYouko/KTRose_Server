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

// Props to ExJam for this code :D Ported to OSpRose by Drakia
#define QUESTCOND(cond) int QUEST_COND_ ## cond (class CWorldServer* server, class CPlayer* client, byte* raw)
typedef int (*fpQuestCond)(class CWorldServer*, class CPlayer*, byte*);

//LMA: for AIP we need one that handles CCharacter.
#define QUESTCONDC(cond) int QUEST_CONDC_ ## cond (class CWorldServer* server, class CCharacter* client, byte* raw)
typedef int (*fpQuestCondC)(class CWorldServer*, class CCharacter*, byte*);

#define GETCONDDATA(cond) STR_COND_ ## cond  * data = (STR_COND_ ## cond *)raw

QUESTCOND(000);QUESTCOND(001);QUESTCOND(002);
QUESTCOND(003);QUESTCOND(004);QUESTCOND(005);
QUESTCOND(006);QUESTCOND(007);QUESTCOND(008);
QUESTCOND(009);QUESTCOND(010);QUESTCOND(011);
QUESTCOND(012);QUESTCOND(013);QUESTCOND(014);
QUESTCOND(015);QUESTCOND(016);QUESTCOND(017);
QUESTCOND(018);QUESTCOND(019);QUESTCOND(020);
QUESTCOND(021);QUESTCOND(022);QUESTCOND(023);
QUESTCOND(024);QUESTCOND(025);QUESTCOND(026);
QUESTCOND(027);QUESTCOND(028);QUESTCOND(029);
QUESTCOND(030);

//LMA: For AIP
QUESTCONDC(000);QUESTCONDC(001);QUESTCONDC(002);
QUESTCONDC(003);QUESTCONDC(004);QUESTCONDC(005);
QUESTCONDC(006);QUESTCONDC(007);QUESTCONDC(008);
QUESTCONDC(009);QUESTCONDC(010);QUESTCONDC(011);
QUESTCONDC(012);QUESTCONDC(013);QUESTCONDC(014);
QUESTCONDC(015);QUESTCONDC(016);QUESTCONDC(017);
QUESTCONDC(018);QUESTCONDC(019);QUESTCONDC(020);
QUESTCONDC(021);QUESTCONDC(022);QUESTCONDC(023);
QUESTCONDC(024);QUESTCONDC(025);QUESTCONDC(026);
QUESTCONDC(027);QUESTCONDC(028);QUESTCONDC(029);
QUESTCONDC(030);


struct STR_COND_000 {
	int iQuestSN;
};

struct STR_COND_001 {
	int iDataCnt;
	STR_QUEST_DATA* CheckData;
};

struct STR_COND_002 {
	int iDataCnt;
	STR_QUEST_DATA* CheckData;
};

struct STR_COND_003 {
	int iDataCnt;
	STR_ABIL_DATA* CheckData;
};

struct STR_COND_004 {
	int iDataCnt;
	STR_ITEM_DATA* CheckData;
};

struct STR_COND_005 {
	byte btIsLeader;
	int iLevel;
	byte btReversed;
};

struct STR_COND_006 {
	int iZoneSN;
	int iX;
	int iY;
	int iZ;
	int iRadius;
};

struct STR_COND_007 {
	dword ulTime;
	dword ulEndTime;
};

struct STR_COND_008 {
	dword ulTime;
	byte btOp;
};

struct STR_COND_009 {
	int iSkillSN1;
	int iSkillSN2;
	byte btOp;
};

struct STR_COND_010 {
	byte btLowPcnt;
	byte btHighPcnt;
};

struct STR_COND_011 {
	union	{
		byte btWho;
		word usWho;
	};
	word nVarNo;
	int iValue;
	byte btOp;
};

struct STR_COND_012 {
	union {
		word iZone;
		dword ulZone;
	};
	int iX;
	int iY;
	union {
		int iEventID;
		dword m_HashEventOBJ;
	};
};

struct STR_COND_013 {
	int iNpcNo;
};

struct STR_COND_014 {
	word nSN;
	byte btOp;
};

struct STR_COND_015 {
	word nNumber1;
	word nNumber2;
};

struct STR_COND_016 {
	union {
		byte btWho;
		dword ulWho;
	};
	dword ulTime;
	dword ulEndTime;
};

struct STR_COND_017 {
	STR_NPCVAR NpcVar1;
	STR_NPCVAR NpcVar2;
	byte btOp;
};

struct STR_COND_018 {
	byte btDate;
	byte btHour1;
	byte btMin1;
	byte btHour2;
	byte btMin2;
};

struct STR_COND_019 {
	byte btWeekDay;
	byte btHour1;
	byte btMin1;
	byte btHour2;
	byte btMin2;
};

struct STR_COND_020 {
	int iNo1;
	int iNo2;
};

struct STR_COND_021 {
	union {
		byte btSelObjType;
		dword ulSelObjType;
	};
	int iRadius;
};

struct STR_COND_022 {
	word nX;
	word nY;
};

struct STR_COND_023 {
	byte btReg;
};

struct STR_COND_024 {
	word nPOS;
	byte btOP;
};

struct STR_COND_025 {
	word nCONT;
	byte btOP;
};

struct STR_COND_026 {
	word nGRD;
	byte btOP;
};

struct STR_COND_027 {
    //LMA: dword...
	dword nPOINT;
	byte btOP;
};

struct STR_COND_028 {
	word iMONEY;
	byte btOP;
};

struct STR_COND_029 {
	word nMemberCNT;
	byte btOP;
};

struct STR_COND_030 {
	word nSkill1;
	word nSkill2;
	byte btOP;
};

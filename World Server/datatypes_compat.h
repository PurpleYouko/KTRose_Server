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

/* Datatypes Compatibility Header, to be merges into osRose */

#ifndef __DATATYPES_COMPAT__
#define __DATATYPES_COMPAT__

// Quests

#define sIsSummon 1
#define sGender 2
#define sBirthStone 3
#define sJob 4
#define sUnion 5
#define sRanking 6
#define sReputation 7
#define sFace 8
#define sHair 9

#define sStrength 10
#define sDexterity 11
#define sIntelligence 12
#define sConcentration 13
#define sCharm 14
#define sSensibility 15

#define sHP 16
#define sMP 17

#define sAttackPower 18
#define sDefense 19
#define sAccuracy 20
#define sMagicResistance 21
#define sDodge 22
#define sMoveSpeed 23
#define sAttackSpeed 24
#define sMaxWeight 25
#define sCritical 26

#define sHPRate 27
#define sMPRate 28
#define sMPDecrease 29

#define sEXPRate 30
#define sLevel 31
#define sStatPoints 32
#define sTendency 33
#define sPvp 34
#define sHeadSize 35
#define sBodySize 36
#define sSkillPoints 37

#define sMaxHP 38
#define sMaxMP 39

#define sMoney 40
#define sUnarmedAtkPow 41
#define sOneHandAtkPow 42
#define sTwoHandAtkPow 43
#define	sBowAtkPow 44
#define sGunAtkPow 45
#define sMagicAtkPow 46
#define sCrossbowAtkPow 47
#define sCombatAtkPow 48

#define sBowAtkSpd 49
#define sGunAtkSpd 50
#define sCombatAtkSpd 51

#define sMoveSpeed2 52	 //Passive
#define sDefense2 53	 //Passive

#define sMaxHP2 54		 //Passive
#define sMaxMP2 55		 //Passive
#define sHPRecovery 56
#define sMPRecovery 57

#define sMaxWeight2 58
#define sBuyDiscount 59
#define sSalesPremium 60
#define sMPSpendDecrease 61
#define sExpandSummonGauge 62
#define sItemDropIncrease 63
#define sIncreaseSummonGauge 63

#define sCurrentPlanet 75
#define sStamina 76

#define sClanPoints 92

#define sNoStorageCharge 93
#define sExpandStorage 95
#define sRemodelShop 96
#define sCartGauge 97

// Stat Type
#define sNone 0
#define sPoison 7
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
#define sMRresDown 23
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


// Skills - Classes
#define cVisitor 0
#define cSoldier 1
#define cMuse 2
#define cHawker 3
#define cDealer 4
#define cKnight 11
#define cChampion 12
#define cMage 13
#define cCleric 14
#define cRaider 15
#define cScout 16
#define cBourgeois 17
#define cArtisan 18
#define cSoldierJob 41
#define cMuseJob 42
#define cHawkerJob 43
#define cDealerJob 44
#define cFirstJob 46
#define cSecondJob 47
#define cThirdJob 48
#define cSoldier2nd 51
#define cMuse2nd 52
#define cHawker2nd 53
#define cDealer2nd 54
#define cSoldier3rd 56
#define cMuse3rd 57
#define cHawker3rd 58
#define cDealer3rd 59
#define cKnightJob 61
#define cChampionJob 62
#define cMageJob 63
#define cClericJob 64
#define cRaiderJob 65
#define cScoutJob 66
#define cBourgeoisJob 67
#define cArtisanJob 68
#define cSoldierHawker 71

// Skills - Types
#define tBasic 1
#define tCraft 2
#define tDamage 3           // Seen (Start)
#define tWeaponUp 4
#define tWeaponUp2 5
#define tSpell 6            // Seen (Start) (This one is odd, an AOE skill, that uses Start)
#define tAOEDamage 7        // Seen (AOE)
#define tContinueSelf2 8    // Seen (Self)
#define tContinuing2  9
#define tRecoverSelf 10     // Seen (Self)
#define tRecover 11         // Seen (Start)
#define tContinueSelf 12    // Seen (Self)
#define tContinue 13
#define tSummon 14
#define tPassive 15
#define tEmotion 16
#define tAOEAttackSelf 17   // Seen (Self)
#define tReservation 18
#define tAbsorb 19
#define tResurrect 20

// Skills - Targets
#define tYourself 0
#define tPartyMember 1
#define tClanMember 2
#define tAlly 3
#define tMonster 4
#define tHostileCharacter 5
#define tHostileTeamMember 6
#define tAllCharacters 7
#define tAllMembers 8
#define tFaintedAlly 9
#define tHostileMonster 10

// Stats calc - Jobs
#define jVisitor 0
#define jSoldier 111
#define jKnight 121
#define jChampion 122
#define jMuse 211
#define jMage 221
#define jCleric 222
#define jHawker 311
#define jRaider 321
#define jScout 322
#define jDealer 411
#define jBourgeois 421
#define jArtisan 422

#endif

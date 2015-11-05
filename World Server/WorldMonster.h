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
#ifndef __ROSE_MONSTER__
#define __ROSE_MONSTER__
#include "character.h"
#include "player.h"
#include "worldserver.h"

class CMonster : public CCharacter
{
    public:
        CMonster( fPoint Position, UINT MonsterType, UINT MonsterMap, UINT Owner=0, UINT spawnid=0 );
        ~CMonster( );

        // times
        clock_t SpawnTime;
     	clock_t lastSighCheck;
     	clock_t lastDegenTime;
		time_t lastLifeUpdate;
		clock_t DeathDelayTimer;
		UINT life_time; //LMA: life time, for summons mainly...


		//LMA: daynight
		int daynight;
		//LMA: special AIP.
		int sp_aip;

        MDrop* MonsterDrop;
        UINT activecycle;

        unsigned int team;  //LMA: PVP team or friendly.
    	unsigned int montype;
    	unsigned int owner;
    	unsigned int MonAI;
    	unsigned int AITimer;
    	unsigned int owner_user_id;  //LMA: used by summons for the summon jauge.
    	unsigned int hitcount;       //LMA: hit count for some monsters (MC and so on...) Arnold.
    	unsigned int maxhitcount;    //LMA: max hit count for some monsters (MC and so on...) Arnold.
    	bool stay_still;             //LMA: if yes, the monster will never move from the place it had been placed.
    	UINT skillid;                //LMA: skill info (generally for summons like bonfires...)
    	UINT buffid;                 //LMA: some monsters do buff.
        UINT minvalue;
        UINT maxvalue;
        UINT bonushp;
        UINT bonusmp;
        UINT range;
        int aip_npctype;    //LMA: AIP
        int aip_clientid;   //LMA: AIP
        bool is_tactical;   //LMA: Is Tactical or basic?
        bool suicide;   //LMA: did the monster committ suicide?

        unsigned int NextWayPoint;

    	CParty* thisparty;
    	CNPCData* thisnpc;

        vector<MonsterDamage*> PlayersDamage;

        // Monster Functions
        void DoAi(int ainumber,char type);  //LMA: AIP.
        bool SummonUpdate(CMonster* monster, CMap* map, UINT j);
        bool SetStats(bool all=true);   //LMA: update in some occasions.
        bool UnspawnMonster();
      	bool IsGhost( );
      	bool IsGhostSeed( );
      	bool IsBonfire( );
      	bool CanMove( );
      	void OnEnemyDie( void* enemy );
      	bool ReadyToMove( );
      	void MoveTo( fPoint nPos, bool randcircle=true );
      	void Move( );
      	CPlayer* GetOwner( );
      	void SpawnMonster( CPlayer* player, CMonster* thismon );
      	bool PlayerInRange( );
      	bool PlayerInGrid( );      //LMA: maps
      	CPlayer* GetNearPlayer( UINT mdist = 20 );
        //void AddDamage( CCharacter* enemy, long int hitpower);
        void AddDamage( CCharacter* enemy, long long hitpower);
        CDrop* GetDrop( );

        //
        bool IsMonster( );
        bool UpdateValues( );

      	// Stats
        unsigned int GetAttackPower();
        unsigned int GetDefense();
        unsigned int GetDodge();
		unsigned int GetXPRate( );
        unsigned int GetItemDropRate( );
        unsigned int GetItemDropCountRate( );
        unsigned int GetZulyDropRate( );
        unsigned int GetAccury();
        unsigned int GetMagicDefense();
		unsigned int GetMagicAttack( );
        unsigned int GetCritical();
        unsigned int GetAttackSpeed();
        unsigned int GetMoveSpeed();

        //unsigned int GetMaxHP( );
        unsigned long long GetMaxHP( );
        bool ForceMaxHP( );   //LMA: Forcing maxHP

        float GetAttackDistance();

        // Events
        bool OnBeAttacked( CCharacter* Enemy );
        bool OnEnemyOnSight( CPlayer* Enemy );
        bool OnDie( );
        bool OnEnemyDie( CCharacter* Enemy );
        bool OnSpawn( bool );
        bool OnAlmostDie( );
        bool OnFar( );
};
#endif

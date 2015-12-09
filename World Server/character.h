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
#ifndef _ROSE_CHARACTER_
#define _ROSE_CHARACTER_
#include "chartype.h"

//LMA: for monster slicer and drop_me
//define to use it.
//#define LMA_SPAWNM

class CCharacter
{
    public:
        CCharacter( );
        ~CCharacter( );

        clock_t lastAiUpdate;   //LMA: AIP
		clock_t SpawnTime;		//PY: Needed for monitoring how old a summon is

        #ifdef LMA_SPAWNM
            clock_t lastSpawnUpdate;   //LMA: AIP
            unsigned int last_monster;
            unsigned int mdeb;
            unsigned int mend;
            unsigned int last_monstercid;
            unsigned int playertime;
            unsigned int xx;
            unsigned int yy;
        #endif

        unsigned int clientid;
        unsigned int char_montype;  //LMA: montype ID for CCharacter
        unsigned int char_owner;    //LMA: owner for CCharacter.
        BYTE CharType; // 0 = undefined | 1 = player | 2 = monster | 3 = summon
        CCharacter* nearChar;
        CCharacter* findChar;
        //int damagecounter;
        long long damagerecieved;	//received damage
		long long damagecounter;	//not used any more
		long long damagedealt;		//damage done
        bool drop_dead; //LMA: he dropped when he died.

        //LMA: test for quest hack (stackable).
        #ifdef QHACK
        dword die_quest;
        #endif
        //LMA END

        //LMA: maps grids
        int last_map;
        int last_coords;
        STATS *Stats;
        POSITION *Position;
        UWPOSITION *UWPosition;  //LMA: Special Position, used for UW mainly.
        BATTLE* Battle;
        STATUS* Status;
        MAGICSTATUS MagicStatus[30];
        int AIVar[20];  //LMA: AIP.

        int pvp_status; //LMA: pvp status (used by maps qsd warp mainly)

        //LMA: for AIP.
        CNPC* refnpc;
       int ExecuteQuestTrigger(dword hash,UINT index=0);

       // Battle Functions
        void DoAttack( );
        void NormalAttack( CCharacter* Enemy );
        bool ResumeNormalAttack( CCharacter* Enemy,bool was_aoe=false);    //LMA: Do we have to resume NormalAttack?
        bool SkillAttack( CCharacter* Enemy, CSkills* skill );
        bool BuffSkill( CCharacter* Target, CSkills* skill );
        bool DebuffSkill ( CCharacter* Enemy, CSkills* skill );//netwolf
        bool SummonBuffSkill( CCharacter* Target, CSkills* skill );  //LMA: Special for supportive summons :)
        bool AoeSkill( CSkills* skill, CCharacter* Enemy=NULL );
        bool AoeBuff( CSkills* skill );
        bool AoeDebuff ( CSkills* skill, CCharacter* Enemy=NULL ); //netwolf
        void UseDebuffSkill ( CCharacter* Enemy, CSkills* skill );//netwolf
        void UseBuffSkill( CCharacter* Target, CSkills* skill );
        void UseAtkSkill( CCharacter* Enemy, CSkills* skill, bool deBuff= false );
        bool UseSkill( CSkills* skill, CCharacter *Target = NULL ); //for gm command
        bool TakeExp( CCharacter *Target ); //taking exp from a dead player.
        void UWKill(CCharacter* Enemy); //LMA: Union War kill.

        //LMA: test for quest hack (stackable).
        #ifdef QHACK
        void QuestKill(dword die_quest); //LMA: Adding a kill to the special quest kill list for a player.
        #endif
        //LMA END

        //functions
        bool IsMonster( );
        bool IsPlayer( );
        bool IsDead( );
        bool IsSummon( );
        bool IsOnBattle( );
        void StartAction( CCharacter* Target, BYTE action, UINT skillid=0, bool restart=false, CCharacter* receiver=NULL);
        bool IsTargetReached( CCharacter* , CSkills* skill=NULL );
        bool CanAttack( );
        bool IsMoving( );
        bool stopMoving( );
        CCharacter* GetCharTarget( );
		CCharacter* GetCharAttacker( );
        CCharacter* GetCharBuffTarget( );           //LMA: Used for some summons.
        bool IsAttacking( );
        //bool first_attack;//Tomiz: Used for CCharacter::DoAttack( ), monster->DoAi(monster->MonAI, 3);
        clock_t nextAi_attacked;   //LMA: AIP

        // virtual functions
        // Events
        virtual bool OnBeAttacked( CCharacter* );
        virtual bool OnDie( );
        virtual bool OnEnemyDie( CCharacter* );
        virtual bool OnEnemyOnSight( void* );
        virtual bool OnSpawn( bool );
        virtual bool OnAlmostDie( );
        virtual bool OnFar( );

        // Functions
        virtual CDrop* GetDrop( );
        //virtual void AddDamage( CCharacter* enemy, long int hitpower);
        virtual void AddDamage( CCharacter* enemy, int hitpower);
        virtual void UpdatePosition( bool monster_stay_still);       //LMA: changed for some monsters.
        virtual bool UpdateValues( );
        virtual void ReduceABC( int amount=1,bool do_packet=true );
        virtual int SearchABC ( int slot ); //LMA: Searching for replacement arrows, bullets...
        virtual int GetEVBuff( );
        void RefreshBuff( );
        virtual CParty* GetParty( );
        virtual CLAN* GetClan( );
        virtual void reduceItemsLifeSpan( bool);

        // stats
        virtual unsigned int GetAttackPower( );
        virtual unsigned int GetDefense( );
		virtual unsigned int GetXPRate( );
		virtual unsigned int GetItemDropRate( );
		virtual unsigned int GetItemDropCountRate( );
        virtual unsigned int GetDodge( );
        virtual unsigned int GetAccury( );
        virtual unsigned int GetMagicDefense( );
		virtual unsigned int GetMagicAttack( );
        virtual unsigned int GetCritical( );
        virtual unsigned int GetAttackSpeed( );
        virtual unsigned int GetMoveSpeed( );
        //virtual unsigned int GetMaxHP( );
        virtual unsigned int GetMaxHP( );
        virtual unsigned int GetMaxMP( );
        virtual float GetAttackDistance( );
        virtual unsigned int GetInt( );
        virtual unsigned int GetSen( );
        virtual unsigned int GetCon( );
        virtual unsigned getWeaponType( );
        //virtual bool AddClanPoints(unsigned int count);
        virtual CClientSocket* getClient();
};

#endif

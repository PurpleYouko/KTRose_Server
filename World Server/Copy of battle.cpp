/*
    Rose Online Server Emulator
    Copyright (C) 2006,2007 OSRose Team http://www.osrose.net

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
#include "character.h"
#include "worldserver.h"

void CCharacter::DoAttack( )
{
    //if(Status->Sleep <= 0xff)
    //{
    //    ClearBattle( Battle );
    //    return; // can't take any action while we are asleep can we?
    //}
    //Log(MSG_INFO,"skill type %i selected. Chartype = %i",Battle->atktype, CharType);
    if(Battle->iscasting == 1) 
    {
        CCharacter* Enemy = GetCharTarget( );
        if(Enemy == NULL)
        {
            ClearBattle( Battle );
            return;
        }
        //Log(MSG_DEBUG,"Iscasting detected as true. Reset to false.");
        if(CanAttack())
        {
            if(IsPlayer())
            {
                Log(MSG_DEBUG,"Should reach here once on normal attack. iscasting state = %i",Battle->iscasting);
            }
            ClearBattle( Battle );
            
            StartAction(Enemy, NORMAL_ATTACK, 0);
            
            return;
        }
        else
        {
            return;
        }
    }
    if(!CanAttack())
    {
        return;
    }
    if(IsSummon())
    {
        CCharacter* Enemy = GetCharTarget( );
        if(Enemy == NULL || (Battle->atktype != SKILL_AOE && Battle->atktype != BUFF_AOE))
        {
            //Log(MSG_DEBUG,"No Enemy found");
            ClearBattle( Battle );
            return;
        }
        if(this == Enemy) //summoned monster is attacking itself. It has been observed to happen
        {
            //Log(MSG_INFO,"Clearing Battle for this summon");
            ClearBattle( Battle );
            return;
        }
        if(Enemy->IsSummon())
        {
            CMonster* thisMonster = reinterpret_cast<CMonster*>(this);
            if(thisMonster == NULL)
            {
                ClearBattle( Battle );
                return;           
            }
            CMonster* otherMonster = reinterpret_cast<CMonster*>(Enemy);
            if(otherMonster == NULL)
            {
                ClearBattle( Battle );
                return;           
            }
            if(thisMonster->owner == otherMonster->owner)
            {
                //Log(MSG_INFO,"Clearing Battle for this summon");
                ClearBattle( Battle );
                return;                    
            }                
        }
    }
    CMap* map = GServer->MapList.Index[Position->Map];
    //Log(MSG_INFO,"DoAttack Battle Attack type = %i",Battle->atktype);
    switch(Battle->atktype)
    {
        case NORMAL_ATTACK://normal attack
        {
            //if(!Status->CanAttack)
            //    return; //can't attack right now.
            CCharacter* Enemy = GetCharTarget( );
            if(Enemy == NULL)
            {
                ClearBattle( Battle );
                return;
            }
            if(Enemy == this)
            {
                //Log(MSG_INFO,"WTF?? I AM trying to attack myself");
                ClearBattle( Battle );
            }
            if(IsTargetReached( Enemy ))
            {
                //if(IsMonster())
                //    Log(MSG_INFO,"Monster is trying to hit the player");
                NormalAttack( Enemy );
                if (Enemy->IsMonster()) // do monster AI script when monster is attacked.
                {
                    CMonster* monster = GServer->GetMonsterByID(Enemy->clientid, Enemy->Position->Map);
                    if(monster == NULL)return;
                    //monster->DoAi(monster->thisnpc->AI, 3);
                    monster->DoAi(monster->monAI, 3);
                }
            }
        }
        break;
        case SKILL_ATTACK://skill attack
        {
            CCharacter* Enemy = GetCharTarget( );
            if(Enemy == NULL)
            {
                ClearBattle( Battle );
                return;
            }
            CSkills* skill = GServer->GetSkillByID( Battle->skillid );
            if(skill == NULL)
            {
                //ClearBattle( Battle );
                return;
            }
            if(IsTargetReached( Enemy, skill )) 
            {
                SkillAttack( Enemy, skill );
                if (Enemy->IsMonster())
                {
                    CMonster* monster = GServer->GetMonsterByID(Enemy->clientid, Enemy->Position->Map);
                    if(monster == NULL)return;
                    monster->DoAi(monster->monAI, 3);
                }
            }
        }
        break;
        case SKILL_BUFF://buffs
        {
            CCharacter* Enemy = GetCharTarget( );
            if(Enemy == NULL)
            {
                //Log(MSG_DEBUG,"this target is NULL");
                ClearBattle( Battle );
                return;
            }
            CSkills* skill = GServer->GetSkillByID( Battle->skillid );
            if(skill == NULL)
            {
                //Log(MSG_DEBUG,"this skill is NULL");
                return;
            }
            if(IsTargetReached( Enemy, skill ))
            {
                //Log(MSG_DEBUG,"Skill successfully cast");
                BuffSkill( Enemy, skill );
            }
        }
        break;
        case SKILL_AOE:
        //case SKILL_SELF: //this is impossible. it can never be set
        case AOE_TARGET:
        {
            //Log(MSG_DEBUG,"Called skill type %i",Battle->atktype);
            CCharacter* Enemy = NULL;
            CSkills* skill = GServer->GetSkillByID( Battle->skillid );
            if(skill == NULL)
            {
                ClearBattle( Battle );
                return;
            }
            if(Battle->atktype == AOE_TARGET)
            {
                float distance = GServer->distance( Position->current, Position->destiny );
                if(distance <= skill->range)
                {
                    Position->destiny = Position->current;
                    AoeSkill( skill, Enemy );
                }
            }
            else
            {
                Position->skilltargetpos = Position->current;
                AoeSkill( skill, NULL );
            }
        }
        break;
        case BUFF_SELF:
        {
            //    return; // can't cast skills right now
            CSkills* skill = GServer->GetSkillByID( Battle->skillid );
            if(skill == NULL)
            {
                //ClearBattle( Battle );
                return;
            }
            BuffSkill( this, skill );
        }
        break;
        case BUFF_AOE:
        {
            //Log(MSG_INFO,"BUFF AOE selected");
            CSkills* skill = GServer->GetSkillByID( Battle->skillid );
            if(skill == NULL)
            {
                //ClearBattle( Battle );

                return;
            }
            AoeBuff( skill );
        }
        break;
        default:
            return;
        break;
    }
}


// do normal attack
void CCharacter::NormalAttack( CCharacter* Enemy )
{
    Enemy->OnBeAttacked( this );
    Position->destiny  = Position->current;
    // PY code
    float mod = 0;
    if(IsPlayer())
    {
        mod = 23 + (Stats->Level / 1.5);
    }
    else
    {
        mod = 23 + (Stats->Level / 4);

    }
    float attack = 0;
    if(Stats->magicattack == 1)
    {
        attack = (float) Stats->Attack_Power / Enemy->Stats->Magic_Defense;
        attack *= mod;
    }
    else
    {
        attack = (float) Stats->Attack_Power / Enemy->Stats->Defense;
        attack *= mod;
    }
    //PY end
    if(attack < 5) attack = 5;
    float d_attack = attack / 100;
    mod = GServer->RandNumber( 0, 10 ) * d_attack;
    attack += mod;
    long int hitpower = (long int)floor(attack + GServer->RandNumber(0, 10));
    if(IsPlayer( )) //temp fix to find balance btw monster and player
    {
        hitpower = (long int)floor(attack * (GServer->Config.PlayerDmg/100.00));
        hitpower+=((hitpower*(Stats->ExtraDamage))/100);
    }
    if(IsMonster( )) //temp fix to find balance btw monster and player
        hitpower = (long int)floor(attack * (GServer->Config.MonsterDmg/100.00));
    bool critical = false;
    if(hitpower <= 5)
    {
        hitpower = 5;
    }
    else
    {
        if(GServer->RandNumber(0,300)<Stats->Critical)
        {
            hitpower = (long int)floor(hitpower * 1.5);
            critical = true;
        }
    }
    // dodge
    unsigned int hitvalue = (unsigned int)floor((double)(Stats->Accury * 100 / Enemy->Stats->Dodge));
    if(hitvalue>100) hitvalue = 100;
    if(GServer->RandNumber( 0, 100 )>hitvalue)
        hitpower = 0; // dodged
    if (hitpower > 0x7ff)//2047 packet size limit.
    {
       hitpower=0x7ff;
    }

    //maybe this little line here will fix my ghosty problem
    //they are definitely attacking. But no animation so maybe atktarget is empty
    Battle->atktarget = Battle->target;
    //Well it does make the damage show up at teh client but the animation is still dodgey

    if(IsMonster())
    {
        //Log(MSG_INFO,"Monster hits player for %i damage. Attack target = %i",hitpower,Battle->atktarget);
    }
    //Log( MSG_INFO, "hitpower %i. Attack %f ", hitpower,attack );
    if(!Enemy->IsSummon( ) && Enemy->IsMonster( ))
    {
        Enemy->AddDamage( this, hitpower );
        Enemy->damagecounter += hitpower;// is for AI
    }
    Enemy->Stats->HP -= hitpower;
    if(IsPlayer())
        Log(MSG_DEBUG,"Player attack did damage %i",hitpower);
    if(IsMonster())
        Log(MSG_DEBUG,"Monster attack did damage %i",hitpower);



    BEGINPACKET( pak, 0x799 );
    ADDWORD    ( pak, clientid );
    ADDWORD    ( pak, Battle->atktarget );

    if(Enemy->IsDead())
    {
        CDrop* thisdrop = NULL;
        ADDWORD ( pak, (hitpower |   (    critical?0xb000:0x8000   )    ));
        if(!Enemy->IsSummon( ) && !Enemy->IsPlayer( ))
        {
            thisdrop = Enemy->GetDrop( );
            if(thisdrop != NULL)
            {
                ADDFLOAT   ( pak, thisdrop->pos.x*100 );
                ADDFLOAT   ( pak, thisdrop->pos.y*100 );
                if(thisdrop->type == 1)
                {
                    ADDWORD( pak, 0xccdf );
                    ADDDWORD( pak, thisdrop->amount );
                }
                else
                {
                    ADDWORD   ( pak, GServer->BuildItemHead( thisdrop->item ) );
                    ADDDWORD   ( pak, GServer->BuildItemData( thisdrop->item ) );
                }
                ADDWORD    ( pak, thisdrop->clientid );
                ADDWORD    ( pak, thisdrop->owner );
                CMap* map = GServer->MapList.Index[thisdrop->posMap];
                map->AddDrop( thisdrop );
            }
        }
        GServer->SendToVisible( &pak, Enemy, thisdrop );
        OnEnemyDie( Enemy );
    }
    else
    {
        ADDWORD   ( pak, (hitpower|(hitpower>0?(critical?0x4000:0):0)));
        GServer->SendToVisible( &pak, Enemy );
    }
    ReduceABC( );
    Battle->lastAtkTime = clock( );
}

// do skill attack
bool CCharacter::SkillAttack( CCharacter* Enemy, CSkills* skill )
{
    Position->destiny = Position->current;
    //if(Battle->castTime == 0)
    //{
        BEGINPACKET( pak, 0x7bb );
        ADDWORD    ( pak, clientid );
        GServer->SendToVisible( &pak, this );
    //    Battle->castTime = clock();
    //    return true;
    //}
    //else
    //{
    //    clock_t etime = clock() - Battle->castTime;
    //   if(etime < SKILL_DELAY)
    //        return true;
    //}
    //Battle->castTime = 0;
    UseAtkSkill( Enemy, skill );
    Stats->MP -= (skill->mp - (skill->mp * Stats->MPReduction / 100));
    if(Stats->MP < 0)
        Stats->MP = 0;
    //Battle->atktype = NORMAL_ATTACK;
    //Battle->skilltarget = 0;
    //Battle->atktarget = Battle->target;
    //Battle->skillid = 0;
    GServer->DoSkillScript( this, skill );
    Battle->lastAtkTime = clock( );
    if(Enemy->IsDead())
        ClearBattle(Battle);
    Battle->iscasting = 1;
    //Log(MSG_DEBUG,"Cast a skill. Iscasting set to true");
    return true;
}

// do buff skill
bool CCharacter::BuffSkill( CCharacter* Target, CSkills* skill )
{
    Position->destiny = Position->current;
    //if(Battle->castTime == 0)
    //{
        BEGINPACKET( pak, 0x7bb );
        ADDWORD    ( pak, clientid );
        GServer->SendToVisible( &pak, (CCharacter*)this );
    //    Battle->castTime = clock();
    //    return true;
    //}
    //else
    //{
    //    clock_t etime = clock() - Battle->castTime;
    //    if(etime < SKILL_DELAY)
    //        return true;
    //}
    //Battle->castTime = 0;
    //Log(MSG_DEBUG,"applied buff skill %i",skill->id);
    UseBuffSkill( Target, skill );
    Stats->MP -= (skill->mp - (skill->mp * Stats->MPReduction / 100));
    if(Stats->MP < 0) Stats->MP = 0;
    GServer->DoSkillScript( this, skill ); //summons a monster. Why do we do this for all skills?
    if(!IsMonster())
    {
        ClearBattle( Battle ); // clear battle for players when they use buff skills
        //Battle->bufftarget = 0;
        //Battle->skilltarget = 0;
        //Battle->skillid = 0;
        //Battle->atktype = NORMAL_ATTACK;
    }
    else //Monsters need to be reset to normal attack and clear skill attacks.
    {
        //Battle->atktarget = Battle->target;
        //Battle->bufftarget = 0;
        //Battle->skilltarget = 0;
        //Battle->skillid = 0;
        //Battle->atktype = NORMAL_ATTACK;
        //StartAction(Target, NORMAL_ATTACK);
    }
    Battle->lastAtkTime = clock( );
    Battle->iscasting = 1;
    return true;
}
//arnold
// do AoE skill
bool CCharacter::AoeSkill( CSkills* skill, CCharacter* Enemy )
{
    Position->destiny = Position->current;
    //Log(MSG_INFO,"Doing AOE Skill");
    //if(Battle->castTime == 0)
    //{
        BEGINPACKET( pak, 0x7bb );
        ADDWORD    ( pak, clientid );
        GServer->SendToVisible( &pak, (CCharacter*)this );
    //    Battle->castTime = clock();
    //    return true;
    //}
    //else
    //{
    //    clock_t etime = clock() - Battle->castTime;
    //    if(etime < SKILL_DELAY)
    //       return true;
    //}
    //Battle->castTime = 0;
    CMap* map = GServer->MapList.Index[Position->Map];

    //PY replacement code
    if(IsPlayer() || IsSummon())
    {
        for(UINT i=0;i<map->MonsterList.size();i++)
        {
            CMonster* monster = map->MonsterList.at(i);
            if(monster == NULL) continue;
            if(monster->clientid == clientid) continue;
            if(monster->IsSummon( ) && (map->allowpvp == 0 || monster->owner == clientid)) continue;
            if(GServer->IsMonInCircle( Position->skilltargetpos, monster->Position->current,(float)skill->aoeradius + 1))
                UseAtkSkill( (CCharacter*) monster, skill );
        }
    }
    if(IsMonster() && !IsSummon())
    {
        for(UINT i=0;i<map->PlayerList.size();i++)
        {
            CPlayer* player = map->PlayerList.at(i);
            if(player == NULL)continue;
            if(player->clientid == clientid) continue;
            if(GServer->IsMonInCircle( Position->skilltargetpos,player->Position->current,(float)skill->aoeradius+1))
                UseAtkSkill( (CCharacter*) player, skill );
        }
    }
    //PY end

    // this code appears to me to be completely wrong replaced with code above
    /*
    for(UINT i=0;i<map->MonsterList.size();i++)
    {
        CMonster* monster = map->MonsterList.at(i);
        if(monster->clientid == clientid) continue;
        if(IsSummon( ) || IsPlayer( ))
        {
            if(monster->IsSummon( ) && (map->allowpvp==0 || monster->owner == clientid)) continue;
        }
        else
        {
            if(!monster->IsSummon( )) continue;
        }
        if(GServer->IsMonInCircle( Position->skilltargetpos, monster->Position->current,(float)skill->aoeradius + 1))
            UseAtkSkill( (CCharacter*) monster, skill );
    }
    if(map->allowpvp!=0 || (IsMonster( ) && !IsSummon( )))
    {
        for(UINT i=0;i<map->PlayerList.size();i++)
        {
            CPlayer* player = map->PlayerList.at(i);
            if(player->clientid==clientid) continue;
            if(GServer->IsMonInCircle( Position->skilltargetpos,player->Position->current,(float)skill->aoeradius+1))
                UseAtkSkill( (CCharacter*) player, skill );
        }
    }*/
    Battle->iscasting = 1;
    //Log(MSG_DEBUG,"Cast a skill. Iscasting set to true");
    if(Enemy != NULL)
    {
        if(Enemy->IsDead( ))
            ClearBattle( Battle );
    }
    else
        ClearBattle( Battle );
    Stats->MP -= (skill->mp - (skill->mp * Stats->MPReduction / 100));
    if(Stats->MP < 0) Stats->MP = 0;
    Battle->lastAtkTime = clock( );

    return true;
}

bool CCharacter::AoeBuff( CSkills* skill )
{
    //Log(MSG_INFO,"BUFF AOE called");
    Position->destiny = Position->current;
    BEGINPACKET( pak, 0x7bb );
    ADDWORD    ( pak, clientid );
    GServer->SendToVisible( &pak, (CCharacter*)this );
    CMap* map = GServer->MapList.Index[Position->Map];

    if(CharType != 2) //player || Summon || NPC.  Anything that is not a monster
    {
        for(UINT i=0;i<map->PlayerList.size();i++)
        {
            CPlayer* player = map->PlayerList.at(i);
            if(player == NULL)continue;
            switch(skill->target)
            {
                case 1: //tPartyMember
                    if(player->Party->party == GetParty())
                    {
                        //Log(MSG_INFO,"Applying AOE buff as long as I can find a close enough party member");
                        if (player == this) //it is me so I had better cast it anyway ^-^
                        {
                            UseBuffSkill( (CCharacter*)player, skill );
                        }
                        else if(player->Party->party != NULL) // Don't cast the skill on players who are not in any party at all.
                        {
                            if(GServer->IsMonInCircle( Position->current,player->Position->current,(float)skill->aoeradius + 1))
                                UseBuffSkill( (CCharacter*)player, skill );
                        }
                    }
                break;
                case 2: //tClanMember
                    if (player->Clan == GetClan( ) && (player->Clan != 0 || player == this))
                    {
                        //Log(MSG_INFO,"Applying AOE buff as long as I can find a close enough clan member");
                        if(GServer->IsMonInCircle( Position->current, player->Position->current, (float)skill->aoeradius + 1 ) )
                            UseBuffSkill( (CCharacter*)player, skill );
                    }
                break;
                case 3: //tAlly
                case 7: //tAllCharacters
                case 8: //tAllMembers. all characters
                {
                     Log(MSG_INFO,"Applying AOE buff as long as I can find a close enough allied Player");
                     if(GServer->IsMonInCircle( Position->current,player->Position->current,(float)skill->aoeradius + 1))
                            UseBuffSkill( (CCharacter*)player, skill );
                }
                break;
                case 5: //tHostileCharacter
                {
                     if (CharType == 2)
                     {
                        //Log(MSG_INFO,"Applying AOE buff as long as I can find a close enough hostile Player");
                        if(GServer->IsMonInCircle( Position->current,player->Position->current,(float)skill->aoeradius + 1))
                            UseBuffSkill( (CCharacter*)player, skill );
                     }
                }
                break;
            }
        }
    }
    else if (CharType == 2) //monster
    {
        for(UINT i=0;i<map->MonsterList.size();i++)
        {
            CMonster* monster = map->MonsterList.at(i);
            if(monster == NULL)continue;
            switch(skill->target)
            {
                case tPartyMember: // party
                break;
                case 2: //tClanMember
                break;
                case 3: //tAlly
                case 7: //tAllCharacters
                case 8: //tAllMembers. all characters
                {
                     //Log(MSG_INFO,"Applying AOE buff as long as I can find a close enough monster");
                     if(GServer->IsMonInCircle( Position->current,monster->Position->current,(float)skill->aoeradius + 1))
                            UseBuffSkill( (CCharacter*)monster, skill );
                }
                break;
                case tHostileCharacter:
                break;
            }
        }
    }
    Stats->MP -= (skill->mp - (skill->mp * Stats->MPReduction / 100));
    if(Stats->MP < 0) Stats->MP = 0;
    //Battle->atktarget = Battle->target;
    //Battle->bufftarget = 0;
    //Battle->skilltarget = 0;
    //Battle->skillid = 0;
    //Battle->atktype = NORMAL_ATTACK;
    ClearBattle( Battle );
    Battle->lastAtkTime = clock( );
    Battle->iscasting = 1;
    return true;
}



// use skill attack
void CCharacter::UseAtkSkill( CCharacter* Enemy, CSkills* skill, bool deBuff)
{
    long int skillpower = skill->atkpower;
    UINT weapontype = 0;
    float mod = 0;
    float attack = 0;
    if(IsPlayer())
    {
        mod = 20 + (Stats->Level / 2);
    }
    else
    {
        mod = 15 + (Stats->Level / 2);
    }
    if(skill->skilltype == 6 || skill->skilltype == 7) //magical damage
    {
        attack = skillpower  / (Enemy->Stats->Magic_Defense / 2);
    }
    else
    {
        attack = skillpower / (Enemy->Stats->Defense / 2);
    }
    attack *= mod;
    attack += (Stats->Attack_Power / 2); // let's add our AP to the skillpower to make it worthwhile to cast skills.

    skillpower = (int)attack;
    bool bflag = false;
    Enemy->OnBeAttacked( this );
    if(skillpower <= 0) skillpower = 0;
    if(IsPlayer())
    {
        skillpower += (int)(skillpower * (Stats->ExtraDamage))/100;
    }
    if (skill->atkpower == 0)
    {
        skillpower = 0; // If the skill has a 0 atk power, it isn't meant to damage
        Log( MSG_INFO, "This skill does no damage" );
        //ClearBattle( Battle );
        return; // probably need to add some alternative code here for legitimate skills that have no attack power.
    }
    if(skillpower >2046)skillpower = 2046;
    //Log(MSG_DEBUG,"Cast a skill. damage is %i",skillpower);
    if(!Enemy->IsSummon( ) && Enemy->IsMonster( ))
    {
        Enemy->AddDamage( this, skillpower );
        Enemy->damagecounter += skillpower;// is for AI
    }
    Enemy->Stats->HP -= skillpower;
    if(IsPlayer())
        Log(MSG_DEBUG,"Player Spell did damage %i",skillpower);
    if(IsMonster())
        Log(MSG_DEBUG,"Monster spell did damage %i",skillpower);
    BEGINPACKET( pak, 0x7b6 );
    ADDWORD    ( pak, Enemy->clientid );
    ADDWORD    ( pak, clientid );
    ADDDWORD   ( pak, 0x000007f8 );
    ADDBYTE    ( pak, 0x00 );
    if (Enemy->Stats->HP<1)
    {
        ADDWORD   ( pak, skillpower + 0x8000 );
    }
    else
    {
        ADDWORD   ( pak, skillpower + 0x2000 );
    }
    if(Enemy->IsDead())
    {
        //Log(MSG_DEBUG,"Enemy died");
        CDrop* thisdrop = NULL;
        ADDDWORD   ( pak, 16 );
        if(!Enemy->IsSummon( ) && !Enemy->IsPlayer( ))
        {
            thisdrop = Enemy->GetDrop( );
            if(thisdrop != NULL)
            {
                ADDFLOAT   ( pak, thisdrop->pos.x*100 );
                ADDFLOAT   ( pak, thisdrop->pos.y*100 );
                if(thisdrop->type==1)
                {
                    ADDWORD( pak, 0xccdf );
                    ADDDWORD( pak, thisdrop->amount );
                }
                else
                {
                    ADDWORD   ( pak, GServer->BuildItemHead( thisdrop->item ) );
                    ADDDWORD   ( pak, GServer->BuildItemData( thisdrop->item ) );
                }
                ADDWORD    ( pak, thisdrop->clientid );
                ADDWORD    ( pak, thisdrop->owner );
                CMap* map = GServer->MapList.Index[thisdrop->posMap];
                map->AddDrop( thisdrop );
            }
        }
        GServer->SendToVisible( &pak, Enemy, thisdrop );
        OnEnemyDie( Enemy );
    }
    else
    {
        ADDDWORD   ( pak, 4 );
        GServer->SendToVisible( &pak, Enemy );
        bflag = GServer->AddBuffs( skill, Enemy, GetInt( ) );

        //Send (de)buff information to the whole world
        if(skill->nbuffs > 0 && bflag)
        {
            BEGINPACKET( pak, 0x7b5 );
            ADDWORD    ( pak, Battle->skilltarget );
            ADDWORD    ( pak, clientid );
			ADDWORD    ( pak, Battle->skillid );
            ADDWORD    ( pak, GetInt( ) );
            ADDBYTE    ( pak, skill->nbuffs );
            GServer->SendToVisible( &pak, Enemy );
        }
    }
    if (deBuff) return;
    //Send skill animation to the world
    RESETPACKET( pak, 0x7b9);
    ADDWORD    ( pak, clientid);
    ADDWORD    ( pak, Battle->skillid);
    ADDWORD    ( pak, 1);
    GServer->SendToVisible( &pak, this );
    Battle->bufftarget = 0;
    Battle->skilltarget = 0;
    Battle->skillid = 0;
    //Battle->atktype = NORMAL_ATTACK;
}

// use buff skill
void CCharacter::UseBuffSkill( CCharacter* Target, CSkills* skill )
{
    bool bflag = false;
    bflag = GServer->AddBuffs( skill, Target, GetInt( ) );
    if(skill->nbuffs > 0 && bflag == true)
    {
        BEGINPACKET( pak, 0x7b5 );
        ADDWORD    ( pak, Target->clientid );
        ADDWORD    ( pak, clientid );
        ADDWORD    ( pak, Battle->skillid );
        ADDWORD    ( pak, GetInt( )/4 );
        ADDBYTE    ( pak, skill->nbuffs );
        GServer->SendToVisible( &pak, Target );
        //Log(MSG_DEBUG,"Buff Skill. buffs applied 1");
    }

    BEGINPACKET( pak, 0x7b9);
    ADDWORD    ( pak, clientid);
    ADDWORD    ( pak, Battle->skillid);
    ADDWORD    ( pak, 1);
	GServer->SendToVisible( &pak, this );
	//Log(MSG_DEBUG,"Buff Skill. buffs applied 2 nbuffs = %i",skill->nbuffs);
	//Log(MSG_INFO,"Target HP = %i",Target->Stats->HP);
}

// Use a skill
bool CCharacter::UseSkill( CSkills* skill, CCharacter *Target )
{
  if (skill->atkpower > 0)
  {
    Log(MSG_INFO, "Need to do %i%s %s in range %i to target %i",
        ( skill->atkpower ),
        ( (skill->range > 0) ? " AOE" : "" ),
        ( (GServer->isSkillTargetFriendly(skill)) ? "healing" : "damage" ),
        ( skill->range ),
        ( skill->target ) );
  }
  for (int i = 0; i < 2; i++) {
    Log(MSG_INFO, "Status ID: %i", skill->status[i]);
    if (skill->status[i] == 0) continue;
    CStatus* status = GServer->GetStatusByID(skill->status[i]);
    if (status == NULL) continue;
    /************************
    We'll probably need to use status->decrease (Figure out whether buf is up/down
    status->repeat will tell us whether it's a one-time (Stat Boost [2]), repeat
    (Recovery, continueing damage [1]) or Special (Status Effect [3]).
    ************************/
    if (status->repeat == 1) // Continuous
    {
        Log(MSG_INFO, "Need to take stat %i and %s it by %i%s over %i seconds", skill->buff[i],
          ((status->decrease) ? "decrease" : "increase"),
          ((skill->value1[i] != 0) ? skill->value1[i] : skill->value2[i]),
          ((skill->value1[i] != 0) ? "" : "%"), skill->duration);
    }
    else if (status->repeat == 2) // Once (Stat Boost)
    {
      Log(MSG_INFO, "Need to take stat %i and %s it by %i%s for %i seconds", skill->buff[i],
          ((status->decrease) ? "decrease" : "increase"),
          ((skill->value1[i] != 0) ? skill->value1[i] : skill->value2[i]),
          ((skill->value1[i] != 0) ? "" : "%"), skill->duration);

    } else if (status->repeat == 3) // Status Effect (Poison,etc)
    {
      Log(MSG_INFO, "Need to give user status effect %i for %i seconds", skill->buff[i],
          ((skill->value1[i] != 0) ? skill->value1[i] : skill->value2[i]),
          ( skill->duration ) );

    }
  }
  return true;
}



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
    //if(IsPlayer())
    //{
    //    Log(MSG_INFO,"DoAttack Attack type %i selected. Chartype = %i casting = %i",Battle->atktype, CharType, Battle->iscasting);
    //}
    //if(CharType = 4) //it's an npc
    //{
     //   Log(MSG_DEBUG,"NPC battle sequence");
    //}
    if(Battle->iscasting == 1)
    {
        CCharacter* Enemy = GetCharTarget( );
        if(Enemy == NULL)
        {
            ClearBattle( Battle );
            return;
        }
        //Log(MSG_DEBUG,"Iscasting detected as true.");
        if(CanAttack())
        {
            if(IsPlayer())
            {
                //Log(MSG_DEBUG,"Should reach here once on normal attack. iscasting state = %i attack type = %i",Battle->iscasting, Battle->atktype);

                if(Battle->atktype == 3) //buff
                {

                    ClearBattle( Battle );
                    return;
                }
                else                     // if not buffing then resume normal attack.
                {
                    //ClearBattle( Battle );
                    StartAction(Enemy, NORMAL_ATTACK, 0);
                }
            }
            else //monsters should always resume normal attack
            {
                StartAction(Enemy, NORMAL_ATTACK, 0);
            }
        }
        else
        {
            return;
        }
        return;
    }
    if(!CanAttack())
    {
        return;
    }
    if(IsSummon())
    {
        //Log(MSG_DEBUG,"Summon attacking in function DoAttack");
        CCharacter* Enemy = GetCharTarget( );
        if(Enemy == NULL) // || (Battle->atktype != SKILL_AOE && Battle->atktype != BUFF_AOE))
        {
            Log(MSG_DEBUG,"No Enemy found");
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
    //if(IsPlayer())
    //{
    //    Log(MSG_INFO,"DoAttack Battle Attack type = %i",Battle->atktype);
    //}
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
                //Log(MSG_DEBUG,"Skill Attack reached target and started");
                SkillAttack( Enemy, skill );
                if (Enemy->IsMonster())
                {
                    CMonster* monster = GServer->GetMonsterByID(Enemy->clientid, Enemy->Position->Map);
                    if(monster == NULL)return;
                    monster->DoAi(monster->monAI, 3);
                    Battle->atktype = NORMAL_ATTACK;
                }
            }
            //else
            //{
                //Log(MSG_DEBUG,"Skill Attack failed to reach target");
            //}
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
    //Log(MSG_DEBUG,"(NormalAttack) Destiny set to current position X: %f Y: %f.",Position->current.x,Position->current.y);


    // Even newer formula based on Jayce's idea
    //dodge. gives a percentage chance
    unsigned int hitpower = 0;
    bool critical = false;
    unsigned int totalchance = Stats->Accury + Enemy->Stats->Dodge;
    double acdm = Stats->Accury * 100 / totalchance;
    unsigned int hitchance = (unsigned int)floor(acdm);
    if(GServer->RandNumber( 0, 100 ) > hitchance)
    {
        hitpower = 0; // dodged
        //Log(MSG_DEBUG,"accuracy %i dodge %i",Stats->Accury,Enemy->Stats->Dodge);
        //Log(MSG_DEBUG,"Dodged total %i mod %f hit chance %i",totalchance,acdm,hitchance);
    }
    else
    {
        //level adjustment. Not yet implemented
        unsigned int leveltotal = Stats->Level + Enemy->Stats->Level;
        double levelmod = Stats->Level * 200 / leveltotal;

        //calculate damage
        if(Stats->magicattack == 1) //magic attacks such as wands and some monsters
        {
            unsigned int totalpower = Stats->Attack_Power + Enemy->Stats->Magic_Defense;
            double hitmod = Stats->Attack_Power * 100 / totalpower + Stats->Level; //percentage of hitpower to use * 100
            hitpower = (unsigned int)floor((double)(hitmod * Stats->Attack_Power / 100));
        }
        else
        {
            unsigned int totalpower = Stats->Attack_Power + Enemy->Stats->Defense;
            double hitmod = Stats->Attack_Power * 100 / totalpower + Stats->Level; //percentage of hitpower to use * 100
            hitpower = (unsigned int)floor((double)(Stats->Attack_Power * hitmod / 100));
            //Log(MSG_DEBUG,"Normal Attack: AP %i DEF %i",Stats->Attack_Power,Enemy->Stats->Defense);
            //Log(MSG_DEBUG,"Normal Attack: hit total %i hitmod %f hit power %i",totalpower,hitmod,hitpower);
        }
        //add some randomness. + or - 5% of hitpower
        int min = (int)floor((double)(hitpower * 0.85));
        int max = (int)floor((double)(hitpower * 1.15));
        int dif = max - min;
        int mod = GServer->RandNumber( 0, dif );
        hitpower = min + mod;
        //Log(MSG_DEBUG,"min %i max %i dif %i mod %i modified hit power %i", min, max, dif, mod, hitpower);
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
                //Log(MSG_DEBUG,"CRITICAL hit power %i", hitpower);
            }
        }
        if (hitpower > 0x7ff)//2047 packet size limit.
           hitpower = 0x7ff;
        //Log(MSG_DEBUG,"Successful hit hit chance %i damage %i",hitchance, hitpower);
    }


    //end jayce formula
    Battle->atktarget = Battle->target;
    //if(IsMonster())
    //{
        //Log(MSG_INFO,"Monster hits player for %i damage. Attack target = %i",hitpower,Battle->atktarget);
    //}
    //Log( MSG_INFO, "hitpower %i. Attack %f ", hitpower,attack );
    if(!Enemy->IsSummon( ) && Enemy->IsMonster( ))
    {
        Enemy->AddDamage( this, hitpower );
        Enemy->damagecounter += hitpower;// is for AI
    }
    Enemy->Stats->HP -= hitpower;
    //if(IsPlayer())
    //    Log(MSG_DEBUG,"Player attack did damage %i",hitpower);
    //if(IsMonster())
    //    Log(MSG_DEBUG,"Monster attack did damage %i",hitpower);



    BEGINPACKET( pak, 0x799 );
    ADDWORD    ( pak, clientid );
    ADDWORD    ( pak, Battle->atktarget );

    if(Enemy->IsDead())
    {
        CDrop* thisdrop = NULL;
        ADDWORD ( pak, (hitpower |   (    critical?0xb000:0x8000   )    ));
        if(!Enemy->IsSummon( ) && !Enemy->IsPlayer( ))
        {
            int thisdroprate = Stats->itemdroprate + GServer->Config.DROP_RATE;
            //Log(MSG_DEBUG,"Monster killed. calling drops function %i times",thisdroprate);
            for(int j=0;j<thisdroprate;j++)
            {
                thisdrop = Enemy->GetDrop( );
                if(thisdrop != NULL)
                {
                    CMap* map = GServer->MapList.Index[thisdrop->posMap];
                    map->AddDrop( thisdrop );
                }
            }
        }
        GServer->SendToVisible( &pak, Enemy );
        ClearBattle(Battle);
        //ClearBattle(Enemy->Battle);
        if(Enemy->IsMonster())
        {
            CMonster* monster = reinterpret_cast<CMonster*>(Enemy);
            if(monster == NULL)
            {
                //Log(MSG_DEBUG,"Monster killed. Failed to create monster object");
                return;
            }
            else
            {
                //Log(MSG_DEBUG,"Monster killed. Setting death delay timer");
                monster->DeathDelayTimer = clock();
            }
        }
        OnEnemyDie( Enemy );
        ClearBattle(Battle);
        GServer->SendToVisible( &pak, Enemy );
    }
    else
    {
        ADDWORD   ( pak, (hitpower|(hitpower>0?(critical?0x4000:0):0)));
        //ADDDWORD   ( pak, (hitpower>0?(critical?12:0):0) );
        GServer->SendToVisible( &pak, Enemy );
    }
    ReduceABC( );
    Battle->lastAtkTime = clock( );
    //Battle->lastAtkTime = Battle->NextAtkTime;
}

// do skill attack
bool CCharacter::SkillAttack( CCharacter* Enemy, CSkills* skill )
{
    Position->destiny = Position->current;
    //Log(MSG_DEBUG,"(SkillAttack) Destiny set to current position X: %f Y: %f.",Position->current.x,Position->current.y);
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
    //Battle->lastAtkTime = Battle->NextAtkTime;
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
    //Log(MSG_DEBUG,"(BuffSkill) Destiny set to current position X: %f Y: %f.",Position->current.x,Position->current.y);
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
        //ClearBattle( Battle );
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
    //Battle->lastAtkTime = clock( );
    Battle->lastAtkTime = Battle->NextAtkTime;
    Battle->iscasting = 1;
    return true;
}
//arnold
// do AoE skill
bool CCharacter::AoeSkill( CSkills* skill, CCharacter* Enemy )
{
    Position->destiny = Position->current;
    //Log(MSG_DEBUG,"(AoeSkill) Destiny set to current position X: %f Y: %f.",Position->current.x,Position->current.y);
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
                UseAtkSkill( (CCharacter*) monster, skill,1 );
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
                UseAtkSkill( (CCharacter*) player, skill,1 );
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
    //Battle->lastAtkTime = Battle->NextAtkTime;
    //Send skill animation to the world
    RESETPACKET( pak, 0x7b9);
    ADDWORD    ( pak, clientid);
    ADDWORD    ( pak, Battle->skillid);
    ADDWORD    ( pak, 1);
    GServer->SendToVisible( &pak, this );
    Battle->bufftarget = 0;
    Battle->skilltarget = 0;
    Battle->skillid = 0;
    return true;
}

bool CCharacter::AoeBuff( CSkills* skill )
{
    //Log(MSG_INFO,"BUFF AOE called with skill target ",skill->target);
    Position->destiny = Position->current;
    //Log(MSG_DEBUG,"(AoeBuff) Destiny set to current position X: %f Y: %f.",Position->current.x,Position->current.y);
    BEGINPACKET( pak, 0x7bb );
    ADDWORD    ( pak, clientid );
    GServer->SendToVisible( &pak, (CCharacter*)this );
    CMap* map = GServer->MapList.Index[Position->Map];

    if(CharType != 2) //player || Summon || NPC.  Anything that is not a monster
    {
        //Log(MSG_INFO,"It's a Player, Summon or NPC");
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
                        //Log(MSG_INFO,"Applying AOE buff as long as I can find a close enough allied Player");
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
                default:
                    //Log(MSG_INFO,"No valid skill target found");
                break;
            }
        }
    }
    else if (CharType == 2) //monster
    {
        //Log(MSG_INFO,"It's a monster");
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
                default:
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
    //Battle->lastAtkTime = Battle->NextAtkTime;
    Battle->iscasting = 1;
    //Send skill animation to the world
    BEGINPACKET( pak1, 0x7b9);
    ADDWORD    ( pak1, clientid);
    ADDWORD    ( pak1, Battle->skillid);
    ADDWORD    ( pak1, 1);
	GServer->SendToVisible( &pak1, this );
    return true;
}



// use skill attack
void CCharacter::UseAtkSkill( CCharacter* Enemy, CSkills* skill, bool deBuff)
{

    // Even newer formula based on Jayce's idea
    //dodge. gives a percentage chance
    Battle->iscasting = 1;
    unsigned int hitpower = 0;
    unsigned int Attack = 0;
    // skill attacks now based on a percentage of base attack power so that they scale with level
    Attack = (unsigned int)floor((double)skill->atkpower * Stats->Attack_Power / 100);
    int success = GServer->RandNumber(0,100);
    if(success <= skill->success)
    {
        //calculate damage
        if(skill->skilltype == 6 || skill->skilltype == 7) //magical damage
        {
            //unsigned int totalpower = skill->atkpower + Enemy->Stats->Magic_Defense;
            unsigned int totalpower = Attack + Enemy->Stats->Magic_Defense;
            double hitmod = Attack * 100 / totalpower; //percentage of hitpower to use * 100
            hitpower = (unsigned int)floor((double)(hitmod * Attack / 100));
            //Log(MSG_DEBUG,"Skill magic: AP %i DEF %i",Stats->Attack_Power,Enemy->Stats->Defense);
            //Log(MSG_DEBUG,"Skill magic: Skill power %i. Attack %i",skill->atkpower,Attack);
            //Log(MSG_DEBUG,"Skill magic: hit total %i mod %f hit power %i",totalpower,hitmod,hitpower);
        }
        else
        {
            //unsigned int totalpower = skill->atkpower + Enemy->Stats->Defense;
            unsigned int totalpower = Attack + Enemy->Stats->Defense;
            double hitmod = Attack * 100 / totalpower; //percentage of hitpower to use * 100
            hitpower = (unsigned int)floor((double)(hitmod * Attack / 100));
            //Log(MSG_DEBUG,"Skill physical: AP %i DEF %i",Stats->Attack_Power,Enemy->Stats->Defense);
            //Log(MSG_DEBUG,"Skill physical: Skill power %i. Attack %i",skill->atkpower,Attack);
            //Log(MSG_DEBUG,"skill physical: hit total %i mod %f hit power %i",totalpower,hitmod,hitpower);
        }
        //add some randomness. + or - 5% of hitpower
        int min = (int)floor((double)(hitpower * 0.85));
        int max = (int)floor((double)(hitpower * 1.15));
        int dif = max - min;
        int mod = GServer->RandNumber( 0, dif );
        hitpower = min + mod;
    }

    bool bflag = false;
    Enemy->OnBeAttacked( this );
    if(hitpower <= 0) hitpower = 0;
    if(IsPlayer())
    {
        hitpower += (int)(hitpower * (Stats->ExtraDamage))/100;
    }
    if (skill->atkpower == 0)
    {
        hitpower = 0; // If the skill has a 0 atk power, it isn't meant to damage
        Log( MSG_INFO, "This skill does no damage" );
        //ClearBattle( Battle );
        return; // probably need to add some alternative code here for legitimate skills that have no attack power.
    }
    if(hitpower > 2046)hitpower = 2046;
    //Log(MSG_DEBUG,"Cast a skill. damage is %i",skillpower);
    if(!Enemy->IsSummon( ) && Enemy->IsMonster( ))
    {
        Enemy->AddDamage( this, hitpower );
        Enemy->damagecounter += hitpower;// is for AI
    }
    //Log( MSG_INFO, "modified skillpower = %i",hitpower );
    Enemy->Stats->HP -= hitpower;
    //if(IsPlayer())
     //   Log(MSG_DEBUG,"Player Spell did damage %i",hitpower);
    //if(IsMonster())
    //    Log(MSG_DEBUG,"Monster spell did damage %i",hitpower);
    BEGINPACKET( pak, 0x7b6 );
    ADDWORD    ( pak, Enemy->clientid );
    ADDWORD    ( pak, clientid );
    ADDDWORD   ( pak, 0x000007f8 );
    ADDBYTE    ( pak, 0x00 );
    if (Enemy->Stats->HP < 1)
    {
        ADDWORD   ( pak, hitpower + 0x8000 );
    }
    else
    {
        ADDWORD   ( pak, hitpower + 0x2000 );
    }
    if(Enemy->IsDead())
    {
        //Log(MSG_DEBUG,"Enemy died");
        CDrop* thisdrop = NULL;
        ADDDWORD   ( pak, 16 );
        if(!Enemy->IsSummon( ) && !Enemy->IsPlayer( ))
        {
            //Log(MSG_DEBUG,"Monster killed. calling drops function %i times",Stats->itemdroprate);
            int thisdroprate = Stats->itemdroprate + GServer->Config.DROP_RATE;
            for(int j=0;j<thisdroprate;j++)
            {
                thisdrop = Enemy->GetDrop( );
                if(thisdrop != NULL)
                {
                    CMap* map = GServer->MapList.Index[thisdrop->posMap];
                    map->AddDrop( thisdrop );
                }
            }
        }

        CMonster* monster = reinterpret_cast<CMonster*>(Enemy);
        if(Enemy->IsMonster())
        {
            if(monster == NULL)
            {
                //Log(MSG_DEBUG,"Monster killed. Failed to create monster object");
                return;
            }
            else
            {
                //Log(MSG_DEBUG,"Monster killed. Setting death delay timer");
                monster->DeathDelayTimer = clock();
            }
        }
        OnEnemyDie( Enemy );
        ClearBattle(Battle);
        GServer->SendToVisible( &pak, Enemy );
    }
    else
    {
        ADDDWORD   ( pak, 4 );
        GServer->SendToVisible( &pak, Enemy );
        bflag = GServer->AddBuffs( skill, Enemy, GetInt( ) );

        //Send (de)buff information to the whole world
        if(skill->nbuffs > 0 && bflag)
        {
            RESETPACKET( pak, 0x7b5 );
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
    //Battle->lastAtkTime = clock( );
    Battle->lastAtkTime = Battle->NextAtkTime; //trying this to time skills better
    //Battle->atktype = NORMAL_ATTACK;
}

// use buff skill
void CCharacter::UseBuffSkill( CCharacter* Target, CSkills* skill, bool deBuff )
{
    bool bflag = false;
    bflag = GServer->AddBuffs( skill, Target, GetInt( ) );
    //Log(MSG_INFO,"In UseBuffSkills, skill %i, nbuffs %i, bflag %i to %u",skill->id,skill->nbuffs,bflag,Target->clientid);
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
    if (deBuff) return;
    BEGINPACKET( pak, 0x7b9);
    ADDWORD    ( pak, clientid);
    ADDWORD    ( pak, Battle->skillid);
    ADDWORD    ( pak, 1);
	GServer->SendToVisible( &pak, this );
	//Log(MSG_DEBUG,"Buff Skill. buffs applied 2 nbuffs = %i",skill->nbuffs);
	//Log(MSG_INFO,"UseBuffSkill: Target HP = %i",Target->Stats->HP);
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



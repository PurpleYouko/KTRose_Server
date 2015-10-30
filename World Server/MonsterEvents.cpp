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
#include "worldmonster.h"

// called when a monster is attacked  [attack/use atk skill/use buff/run/summon]
bool CMonster::OnBeAttacked( CCharacter* Enemy )
{
    Battle->hitby = Enemy->clientid;

    if(!IsOnBattle( ))
    {

        //LMA: yeah hurt me (used for Santa Rudolph).
        if(thisnpc->helpless==1)
        {
            StartAction( Enemy, STAY_STILL_ATTACK, 0 );
            return true;
        }

        //Some monsters do not attack and stay still (mc)
        if(!stay_still)
        {
            StartAction( Enemy, NORMAL_ATTACK, 0 );
        }
        else
        {
            Log(MSG_INFO,"Stay still attack");
            StartAction( Enemy, STAY_STILL_ATTACK, 0 );
        }

    }

    return true;
}

// called when a monster die [give exp/give extra drop]
bool CMonster::OnDie( )
{
    //CMap* map = GServer->MapList.Index[Position->Map];

    //LMA begin
    //CF mode 1
    //20070621-211100
    //Deleted the 20091114
    UINT special_exp=0;
    UINT special_lvl=0;
    GServer->GiveExp( this , special_lvl, special_exp );
    //LMA End


    return true;
}
// called when we create a new monster [attack players?]
bool CMonster::OnSpawn( bool Attack )
{
     lastSighCheck = clock();
    if(IsGhost( ) && Attack )
    {
        CPlayer* player = GetNearPlayer( );
        if(player==NULL) // no players or too far
            return true;
        StartAction( (CCharacter*)player, NORMAL_ATTACK, 0 );
    }

    return true;
}

// [not]called when a monster is almost dead [run/use only skills/call a healer? :S xD]
bool CMonster::OnAlmostDie( )
{
    return true;
}

// called when a enemy is on sight
bool CMonster::OnEnemyOnSight( CPlayer* Enemy )
{
    clock_t etime = clock() - lastSighCheck;
    if(etime<5000) return true;
    if(!IsOnBattle( ))
    {
        if(thisnpc->aggresive>1)
        {
            UINT aggro = GServer->RandNumber(2,15);
            if(thisnpc->aggresive>=aggro && !IsGhostSeed( ))
            {
                //osprose.
                /*
                Enemy->ClearObject( this->clientid );
				SpawnMonster(Enemy, this );
				*/
                StartAction( (CCharacter*) Enemy, NORMAL_ATTACK, 0 );
            }
            else
            if(IsGhostSeed( ) || thisnpc->aggresive>5)
                MoveTo( Enemy->Position->current, true );
        }
        lastSighCheck = clock();
    }
    return true;
}

// called when enemy die
bool CMonster::OnEnemyDie( CCharacter* Enemy )
{
    Log(MSG_INFO,"An enemy died, let's stop battle");
    Position->destiny = Position->source; //ON MOB DIE
    ClearBattle( Battle );
    MoveTo( Position->source );
    return true;
}

bool CMonster::OnFar( )
{
    //if Owner too far away, we kill the bonfire.
    if(IsBonfire())
    {
       UnspawnMonster( );
       return true;
    }

    //osprose.
    if(!IsSummon())return true;

    Position->destiny = Position->source; //ON TARGET LOST
    ClearBattle( Battle );
    MoveTo( Position->source );
    return true;
}

//osprose's summon update.
//LMA: adding the new life_time for summons :)
bool CMonster::SummonUpdate(CMonster* monster, CMap* map, UINT j)
{
    UINT die_amount=0;
 	clock_t etime = clock() - lastDegenTime;

    if( etime >= 4 * CLOCKS_PER_SEC && Stats->HP > 0 )
    {
        CPlayer* ownerclient = monster->GetOwner( );

        if(monster->life_time>0)
        {
            die_amount=(UINT)(monster->Stats->MaxHP/monster->life_time);
            //Log(MSG_INFO,"die amount for %i is %u",monster->montype,die_amount);
        }
        else
        {
            die_amount=2;
            //Log(MSG_INFO,"DEFAULT die amount for %i is %u",monster->montype,die_amount);
        }

        Stats->HP -= (die_amount*4);    //*4 because each 4 seconds ;)
        lastDegenTime = clock();
        if (Stats->HP < 1||ownerclient == NULL||ownerclient->IsDead())//orphan check
        {
            //LMA: We update the jauge, only if it was a skill summon
            if(ownerclient != NULL&&monster->owner_user_id>0)
            {
                ownerclient->cur_jauge-=GServer->NPCData[monster->montype]->tab1;
                if(ownerclient->cur_jauge<=0)
                {
                    ownerclient->cur_jauge=0;
                }

                Log(MSG_INFO,"summon died (took %u), jauge is now %i/%i",GServer->NPCData[monster->montype]->tab1,ownerclient->cur_jauge,ownerclient->summon_jauge);
            }
            else
            {
                //LMA: We have to look for the owner with his userid, perhaps he warped...
                if(monster->owner_user_id>0)
                {
                    CPlayer* tempplayer=GServer->GetClientByUserID(monster->owner_user_id);
                    if(tempplayer!=NULL)
                    {
                        tempplayer->cur_jauge-=GServer->NPCData[monster->montype]->tab1;
                        if(tempplayer->cur_jauge<=0)
                        {
                            tempplayer->cur_jauge=0;
                        }

                        Log(MSG_INFO,"summon died (owner found by userid) (took %u), jauge is now %i/%i",GServer->NPCData[monster->montype]->tab1,tempplayer->cur_jauge,tempplayer->summon_jauge);
                    }

                }

            }

            //he's dead.
            Log(MSG_INFO,"Summon should be dead now, orphan");
            BEGINPACKET( pak, 0x799 );
            ADDWORD    ( pak, monster->clientid );
            ADDWORD    ( pak, monster->clientid );
            ADDDWORD   ( pak, monster->thisnpc->hp*monster->thisnpc->level );
            ADDDWORD   ( pak, 16 );
            GServer->SendToVisible( &pak, monster );
            map->DeleteMonster( monster, true, j );
            return true;
        }
        else
        {
            if(Stats->HP >0)
            {
                //LMA: Trying to update real HP amount.
                //Log(MSG_INFO,"Summon is alive");
                BEGINPACKET( pak, 0x79f );
                ADDWORD    ( pak, monster->clientid );
                ADDDWORD   ( pak, Stats->HP );
                GServer->SendToVisible( &pak, monster );
            }

        }

    }
    else if (Stats->HP < 1)
    {
        CPlayer* ownerclient = monster->GetOwner( );

        //LMA: We update the jauge (only for skill summons)
        if(ownerclient != NULL&&monster->owner_user_id>0)
        {
            ownerclient->cur_jauge-=GServer->NPCData[monster->montype]->tab1;
            if(ownerclient->cur_jauge<=0)
            {
                ownerclient->cur_jauge=0;
            }

            Log(MSG_INFO,"summon died (took %u), jauge is now %i/%i",GServer->NPCData[monster->montype]->tab1,ownerclient->cur_jauge,ownerclient->summon_jauge);
        }
        else
        {
            //LMA: We have to look for the owner with his userid, perhaps he warped...
            if(monster->owner_user_id>0)
            {
                CPlayer* tempplayer=GServer->GetClientByUserID(monster->owner_user_id);
                if(tempplayer!=NULL)
                {
                    tempplayer->cur_jauge-=GServer->NPCData[monster->montype]->tab1;
                    if(tempplayer->cur_jauge<=0)
                    {
                        tempplayer->cur_jauge=0;
                    }

                    Log(MSG_INFO,"summon died (owner found by userid 2) (took %u), jauge is now %i/%i",GServer->NPCData[monster->montype]->tab1,tempplayer->cur_jauge,tempplayer->summon_jauge);
                }

            }

        }

        //Log(MSG_INFO,"Summon should be dead now 2");
        BEGINPACKET( pak, 0x799 );
        ADDWORD    ( pak, monster->clientid );
        ADDWORD    ( pak, monster->clientid );
        ADDDWORD   ( pak, monster->thisnpc->hp*monster->thisnpc->level );
        ADDDWORD   ( pak, 16 );
        GServer->SendToVisible( &pak, monster );
        map->DeleteMonster( monster, true, j );
    }


    return true;
}

//LMA: AIP
//ainumber is monster->AI type is add=0 idle=1 attacking=2 attacked=3 after killing target=4 hp<1=5
void CMonster::DoAi(int ainumberorg,char type)
{
    //LMA: reseting some timers.
    if (type==0||type==1||type==4||type==5)
    {
        nextAi_attacked=clock();
    }

    //LMA: does the monster have a special AIP?
    int ainumber=ainumberorg;
    if (sp_aip!=0)
    {
        ainumber=sp_aip;
        Log(MSG_INFO,"AIP overwritten from %i to %i for monster %i (cid %i)",ainumberorg,ainumber,montype,clientid);
    }

    CAip* script = NULL;
    int AIWatch = GServer->Config.AIWatch;
    int aiindex = (ainumber*0x10000)+(type*0x100);

    bool lma_debug=false;
    int nb_turns=0;


    /*if(ainumber==25||ainumber==499)
    {
        Log(MSG_INFO,"a monster does AIP index %i, type %i",type);
        lma_debug=true;
        LogDebugPriority(3);
    }*/

    //NPC Leum and chelsie and gawain
    /*if(ainumber==1113||ainumber==1090||ainumber==1091)
    {
        lma_debug=true;
        LogDebugPriority(3);
    }*/

    //void NPCs used in Ulverick stuff (NPC 1791 to 1795 but aip is 471 to 475).
    //Only for 471 / 1791 for now.
    //if(ainumber>=471&&ainumber<=475)
    //if(ainumber==471)
    /*if((ainumber>=455&&ainumber>=457)||ainumber==471)
    {
        //Log(MSG_INFO,"Doing AI for NPC %u",ainumber);
        lma_debug=true;
        LogDebugPriority(3);
    }*/

    /*if (Position->Map==2&&ainumber==1805)
    {
        lma_debug=true;
        LogDebugPriority(3);
    }*/

    //LMA: findchar and nearchar set to NULL.
    nearChar=NULL;
    findChar=NULL;

    //LMA: New way, faster?
    while(GServer->AipListMap.find(aiindex)!=GServer->AipListMap.end())
    {
        nb_turns++;
        if (lma_debug)
            LogDebug("BEGIN%i CDT Turn %i",ainumber,nb_turns);

        script = GServer->AipListMap[aiindex];

        int success = AI_SUCCESS; //needs to be AI_SUCCESS otherwise would not perform conditionless actions
        int thisaction = 0;

        for (dword i = 0; i < script->ConditionCount; i++)
        {
            int command = script->Conditions[i]->opcode;
            //if (command > 30 || command < 0) continue;
            if (command > 32 || command < 0) continue;

            //LMA: adding the AIPId
            //success = (*GServer->aiCondFunc[command])(GServer, this, script->Conditions[i]->data);
            success = (*GServer->aiCondFunc[command])(GServer, this, script->Conditions[i]->data,ainumber);
            if(ainumber == AIWatch)LogDebug( "aiCondition %03u returned %d", command, success);

            //LMA: Special case (quite a stupid one if you're asking me)
            if(command==20&&type==1&&success==AI_FAILURE)
            {
                //they tried to do a check target level on idle... Let's say it went ok...
                success=AI_SUCCESS;
            }

            if (success == AI_FAILURE)
            {
                if (lma_debug)
                {
                    LogDebug( "DoAI%i aiCondition %03u, %i/%i Failure.",ainumber,command,i,script->ConditionCount-1);
                }

                break;
            }
            else
            {
                if (lma_debug)
                {
                    LogDebug( "DoAI%i aiCondition %03u, %i/%i Success.",ainumber,command,i,script->ConditionCount-1);
                }

            }

        }

        if (lma_debug)
        {
            LogDebug("DoAI%i END CDT Turn %i",ainumber,nb_turns);
        }


        if (success == AI_SUCCESS)
        {
            if (lma_debug)
            {
                LogDebug("DoAI%i BEGIN ACT Turn %i",ainumber,nb_turns);
            }


            //LMA: Special case, if we have a LTB and a qsd trigger in the same script, we do the LTB first.
            if(script->offset_ltb!=-1&&script->offset_qsd_trigger!=-1)
            {
                //We do the LTB.
                int command = script->Actions[script->offset_ltb]->opcode;
                if (command > 38 || command < 0) continue;

                //LMA: adding the AIPId
                //success = (*GServer->aiActFunc[command])(GServer, this, script->Actions[script->offset_ltb]->data);
                success = (*GServer->aiActFunc[command])(GServer, this, script->Actions[script->offset_ltb]->data,ainumber);
                if(ainumber == AIWatch)LogDebug( "aiAction forced: %03u returned %d", command, success);

                if(lma_debug)
                {
                    LogDebug( "DoAI%i aiAction Forced: %03u returned %d, %i/%i",ainumber,command, success,script->offset_ltb,script->ActionCount-1);
                }

            }

            for (dword i = 0; i < script->ActionCount; i++)
            {
                if(script->offset_ltb!=-1&&script->offset_qsd_trigger!=-1&&i==script->offset_ltb)
                {
                    //We cancel the LTB, already done earlier.
                    LogDebug( "DoAI%i aiAction: We cancel %i since it should be already done",ainumber,i);
                    continue;
                }

                int command = script->Actions[i]->opcode;
                if (command > 38 || command < 0) continue;

                //LMA: adding the AIPId
                //success = (*GServer->aiActFunc[command])(GServer, this, script->Actions[i]->data);
                success = (*GServer->aiActFunc[command])(GServer, this, script->Actions[i]->data,ainumber);
                if(ainumber == AIWatch)LogDebug( "aiAction: %03u returned %d", command, success);

                if(lma_debug)
                {
                    LogDebug( "DoAI%i aiAction: %03u returned %d, %i/%i",ainumber,command, success,i,script->ActionCount-1);
                }

            }

            if(success == AI_SUCCESS)
            {
                if(lma_debug)
                {
                    LogDebug("DoAI%i END ACT SUCCESS Turn %i",ainumber,nb_turns);
                }

                //LMA: Santa is special, he continues (Xmas Tree Spawning), same for some special others (gifts summoning)
                if(ainumber!=1205&&ainumber!=2048&&ainumber!=2049)
                {
                    if(lma_debug)
                    {
                        GServer->pakGMObjVar(NULL, 1249,2);
                        LogDebugPriority(4);
                    }

                    thisnpc->refNPC=0;
                    nearChar=NULL;
                    findChar=NULL;

                    return; //automatically return after performing the first successful action
                }

            }
            else
            {
              if(lma_debug)
              {
                    LogDebug("DoAI%i END ACT FAILURE Turn %i",ainumber,nb_turns);
              }

            }

        }

        aiindex++;
    }

    if(lma_debug)
    {
        GServer->pakGMObjVar(NULL, 1249,2);
        LogDebugPriority(4);
    }


    thisnpc->refNPC=0;
    nearChar=NULL;
    findChar=NULL;


    return;
}

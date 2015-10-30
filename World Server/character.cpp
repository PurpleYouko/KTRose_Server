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
#include "character.h"
#include "worldserver.h"

//constructor
CCharacter::CCharacter( )
{
    // STATS
    Stats = new STATS;
    refnpc=NULL;
    assert(Stats);
    Stats->Level = 0;
    Stats->HP = 0;
    Stats->MP = 0;
    Stats->MaxHP = 0;
    Stats->MaxMP = 0;
    Stats->Attack_Power = 0;
    Stats->Defense = 0;
    Stats->Critical = 0;
    Stats->Dodge = 0;
    Stats->Accury = 0;
    Stats->Magic_Defense = 0;
    Stats->Move_Speed = 0;
    Stats->Base_Speed=0;
    Stats->Attack_Speed = 0;
    Stats->Attack_Distance = 0;
    Stats->MaxWeight = 0;
    Stats->MaxSummonGauge = 0;
    Stats->MPReduction = 0;
    Stats->ExtraDamage_add=0;
//    Stats->ShieldDamage = 0;

    // POSITION
    Position = new POSITION;
    assert(Position);
    Position->Map = 0;
    Position->lastMoveTime = 0;
    Position->saved = 0;
    Position->respawn = 0;

    //LMA: UW Position
    UWPosition = new UWPOSITION;
    assert(UWPosition);
    UWPosition->Map=0;
    UWPosition->source.x=0;
    UWPosition->source.y=0;
    UWPosition->source.z=0;

    //LMA: pvp status.
    pvp_status=-1;

    //BATLE
    Battle = new BATTLE;
    assert(Battle);
    ClearBattle( Battle );
    Battle->castTime = 0;
    //STATUS
    Status = new STATUS;
    assert(Status);

        //Stats up
    Status->Attack_up = 0xff;
    Status->Defense_up = 0xff;
    Status->Magic_Defense_up = 0xff;
    Status->Accury_up = 0xff;
    Status->Critical_up = 0xff;
    Status->Dodge_up = 0xff;
    Status->Haste_up = 0xff;
    Status->Dash_up = 0xff;
    Status->HP_up = 0xff;
    Status->MP_up = 0xff;
    Status->ShieldDamage_up = 0xff;
    Status->ExtraDamage_up = 0xff;  //LMA: Devilking/Arnold

        //Stats down
    Status->Attack_down = 0xff;
    Status->Defense_down = 0xff;
    Status->Magic_Defense_down = 0xff;
    Status->Accury_down = 0xff;
    Status->Critical_down = 0xff;
    Status->Dodge_down = 0xff;
    Status->Haste_down = 0xff;
    Status->Dash_down = 0xff;
    Status->HP_down = 0xff;
    Status->MP_down = 0xff;
    Status->ExtraDamage_down = 0xff;  //LMA: Devilking/Arnold
    Status->ShieldDamage_down = 0xff;

        //Status
    Status->Stun = 0xff;
    //Status->Spoisoned = 0xff;
    //Status->Muted = 0xff;
    Status->Weary = 0xff;   //Stealth(hawker skill. 1626-1629) 2nd deBuff (can't skill)
    Status->Stealth = 0xff;
    Status->Cloaking = 0xff;
    Status->CanAttack = true;
    Status->CanRun = true;
    Status->CanMove = true;
    Status->CanCastSkill = true;
    Status->Poison = 0xff;
    Status->Mute = 0xff;
    //Status->Flame = 0xff;
    Status->Flamed = 0xff;
    Status->Sleep = 0xff;
    Status->Detect = 0xff;
    Status->Taunt = 0xff;
    Status->Recover = 0xff;//Purify

    //LMA: ospRose.
    Status->Stuned = 0xff;
    Status->Poisoned = 0xff;
    Status->Muted = 0xff;

        // Stance
    Status->Stance = RUNNING;
    for(int i=0;i<30;i++)
    {
        MagicStatus[i].Buff = 0;
        MagicStatus[i].Value =0;
        MagicStatus[i].Duration = 0;
        MagicStatus[i].BuffTime = 0;
        MagicStatus[i].Status=0;
    }
    CharType = 0;
    clientid = 0xffff;
    nearChar=NULL;
    findChar=NULL;
}

//deconstructor
CCharacter::~CCharacter( )
{
    delete Stats;
    delete Position;
    delete Battle;
    delete Status;
}

// Virtual
bool CCharacter::UpdateValues( )
{
    return true;
}

//LMA: AIP (qsd quests).
int CCharacter::ExecuteQuestTrigger(dword hash, UINT index)
{
    if(IsPlayer())
    {
        Log(MSG_WARNING,"In CHARACTER!!! %u",hash);
    }

    CQuestTrigger* trigger = NULL;
    CQuestTrigger* nexttrigger = NULL;

    //LMA: To counter some very weird cases where the hashes exist several times...
    UINT my_index=0;
    //for(unsigned j=0; j < GServer->TriggerList.size(); j++)
    for(unsigned j=index; j < GServer->TriggerList.size(); j++)
    {
      if (GServer->TriggerList.at(j)->TriggerHash == hash)
      {
        trigger = GServer->TriggerList.at(j);

        //LMA: bug on next trigger.
        if(j+1<GServer->TriggerList.size())
        {
            my_index=j+1;
            nexttrigger = GServer->TriggerList.at(j + 1);
        }

        break;
      }

    }

    if (trigger == NULL)
    {
        LogDebug( "EXTC::Trigger not found hash %u, index %u", hash,index);
        return QUEST_FAILURE;
    }

    int success = QUEST_SUCCESS;
    LogDebug( "EXTC::Trigger Executed: %s[%i]", trigger->TriggerName, trigger->CheckNext);

    for (dword i = 0; i < trigger->ConditionCount; i++)
    {
      int command = trigger->Conditions[i]->opcode;
      if (command > 30 || command < 0) continue;
      success = (*GServer->qstCondFuncC[command])(GServer, this, trigger->Conditions[i]->data);
      LogDebug( "EXTC::Condition %03u returned %d", command, success);

      if (success == QUEST_FAILURE)
      {
        if (!trigger->CheckNext)
        {
            LogDebug("EXTC::No checknext, FAILURE");
            return success;
        }
        else
        {
            LogDebug("EXTC::checknext, FAILURE");
            if(nexttrigger==NULL)
            {
                Log(MSG_WARNING,"CCharacter::ExecuteQuestTrigger, Next trigger but NULL! %u",hash);
                return QUEST_SUCCESS;
            }

            return ExecuteQuestTrigger(nexttrigger->TriggerHash,my_index);
        }

      }

      LogDebug("EXTC::Quest cdt success");
    }

    for (dword i = 0; i < trigger->ActionCount; i++)
    {
      int command = trigger->Actions[i]->opcode;

       //LMA: command 29 is ok.
      //if ((command > 28 || command < 0) && command != 34)
      if ((command > 29 || command < 0) && command != 34)
      {
          LogDebug( "EXTC::unknown Action command %i", command);
          continue;
      }

      //LogDebug("EXTC::QSD ACT %03u BEGIN",command);
      LogDebug( "EXTC::Reward %03u returned %d", command, (*GServer->qstRewdFuncC[command])(GServer, this, trigger->Actions[i]->data));
    }


    return success;
}

// update position
void CCharacter::UpdatePosition( bool monster_stay_still )
{
    //LMA: osprose.
    if(IsOnBattle( ) && Battle->target!=0)
    {
        CCharacter* Target = GetCharTarget( );

        if(Target == NULL)
        {
            ClearBattle( Battle );
        }
        else
        {
            //LMA: Don't need to move if the target is reached.
            //Bad idea since the monster can go away and it seems
            //player goes automatically after him client side...
            /*
            if (Battle->atktype==NORMAL_ATTACK)
            {
                if(!IsTargetReached( Target ))
                {
                    Position->destiny=Target->Position->current;
                }

            }
            else
            {
                Position->destiny=Target->Position->current;
            }
            */
            //So let's go back to the old way.
            //LMA: done in DoAttack...
            //Can be "dangerous" since it checks range for normal attack
            //and if we're with skills the range isn't the same so player changes
            //its destiny all the time from skill range to weapon range.
            /*if(!IsTargetReached( Target ))
            {
                Position->destiny=Target->Position->current;
            }*/

        }

    }
	//osprose end.

    //LMA: position in map 8
    /*if(Position->Map==8)
    {
        float tempdist = GServer->distance(Position->current,Position->destiny);

        if(IsPlayer())
        {
            Log(MSG_INFO,"Player HP %I64i, (%.2f:%.2f)->(%.2f:%.2f)=%.2f speed %u",Stats->HP,Position->current.x,Position->current.y,Position->destiny.x,Position->destiny.y,tempdist,Stats->Move_Speed);
        }
        else
        {
            Log(MSG_INFO,"Monster HP %I64i, (%.2f:%.2f)->(%.2f:%.2f)=%.2f speed %u",Stats->HP,Position->current.x,Position->current.y,Position->destiny.x,Position->destiny.y,tempdist,Stats->Move_Speed);
            //Log(MSG_LOAD,"Monster (%.2f:%.2f)->(%.2f:%.2f)",Position->current.x,Position->current.y,Position->destiny.x,Position->destiny.y);
        }

    }*/

    //LMA maps: special case (arrive in Game)
    //and he changed map (GM or scroll or teleporter or boat?)
    //2do: other cases too, all in fact...
    bool is_done=false;
    if (IsPlayer( )&&((last_map==-1&&last_coords==-1)||(last_map!=Position->Map)))
    {
      	//updating player's grid
    	int new_coords=0;
    	int new_map=0;
    	int grid_id=0;

      	//deleting previous presence...
      	if (last_map!=-1&&last_coords!=-1)
      	{
          	grid_id=GServer->allmaps[last_map].grid_id;
            if (grid_id!=-1&&!GServer->allmaps[last_map].always_on&&GServer->gridmaps[grid_id].coords[last_coords]>0)
               GServer->gridmaps[grid_id].coords[last_coords]--;
        }

        //New coordinates
    	new_map=Position->Map;
    	grid_id=GServer->allmaps[new_map].grid_id;

        if (grid_id==-1)
        {
            Log(MSG_WARNING,"It seems you forgot to declare map %i in map_grid.csv",new_map);
        }

        new_coords=GServer->GetGridNumber(new_map,(UINT) floor(Position->current.x),(UINT) floor(Position->current.y));
    	last_map=new_map;
    	last_coords=new_coords;

    	//if (grid_id!=-1||!GServer->allmaps[new_map].always_on)
    	if (grid_id!=-1&&!GServer->allmaps[new_map].always_on)
           GServer->gridmaps[grid_id].coords[new_coords]++;

   	    is_done=true;
    }

    if(!IsMoving())
    {
        //osprose
        if(IsPlayer())Position->lastMoveTime = clock();
        //osprose end
        return;
    }

	float dx = Position->destiny.x - Position->current.x;
	float dy = Position->destiny.y - Position->current.y;
	float distance = sqrt( (dx*dx) + (dy*dy) );
    float ntime = ( distance / Stats->Move_Speed * GServer->MOVE_SPEED_MODIF );
    clock_t etime = clock() - Position->lastMoveTime;

    //LMA: bad, that's bad...
	//if (ntime<=etime || distance<1.0 )
	if (ntime<=etime || distance<0.01 )
    {
        // if (IsPlayer()) printf("Arrived! X: %i, Y: %i\n", (int)Position->current.x, (int)Position->current.y);
        if(Position->Map==8&&IsMonster())
        {
            Log(MSG_INFO," Monster Arrived, J (%.2f:%.2f)->(%.2f:%.2f)",Position->current.x,Position->current.y,Position->destiny.x,Position->destiny.y);
        }

		Position->current.x = Position->destiny.x;
		Position->current.y = Position->destiny.y;
    }
	else
    {
		Position->current.x += dx*(etime/ntime);
		Position->current.y += dy*(etime/ntime);
	}

	Position->lastMoveTime = clock( );

	//LMA: maps (for player)
	if(!IsPlayer()||is_done)
	   return;

	//updating player's grid
	int new_coords=0;
	int new_map=0;
	int grid_id=0;

	new_map=Position->Map;
	grid_id=GServer->allmaps[new_map].grid_id;

	if (grid_id==-1)
	{
	    Log(MSG_WARNING,"It seems you forgot to declare map %i in map_grid.csv",new_map);
	}

	new_coords=GServer->GetGridNumber(new_map,(UINT) floor(Position->current.x),(UINT) floor(Position->current.y));
	//changed?
    if (last_map==new_map&&new_coords==last_coords)
         return;

     //Let's update.
	//if (grid_id!=-1||!GServer->allmaps[new_map].always_on)
	if (grid_id!=-1&&!GServer->allmaps[new_map].always_on)
	   GServer->gridmaps[grid_id].coords[new_coords]++;

   //deleting player from his previous map
   grid_id=GServer->allmaps[last_map].grid_id;
   if (grid_id!=-1&&!GServer->allmaps[last_map].always_on&&GServer->gridmaps[grid_id].coords[last_coords]>0)
      GServer->gridmaps[grid_id].coords[last_coords]--;

    //Log(MSG_INFO,"Now[%i,%i],Was[%i,%i]",new_map,new_coords,last_map,last_coords);
	last_map=new_map;
	last_coords=new_coords;
}

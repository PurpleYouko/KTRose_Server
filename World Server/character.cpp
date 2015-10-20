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

//constructor
CCharacter::CCharacter( )
{
    // STATS
    Stats = new STATS;
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
    Stats->Attack_Speed = 0;
    Stats->Attack_Distance = 0;
    Stats->MaxWeight = 0;
    Stats->MaxSummonGauge = 0;
    Stats->MPReduction = 0;
    Stats->ExtraDamage = 0;
    // POSITION
    Position = new POSITION;
    assert(Position);
    Position->Map = 0;
    Position->lastMoveTime = 0;
    Position->saved = 0;
    Position->respawn = 0;
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
        //Status
    Status->Stuned = 0xff;
    Status->Poisoned = 0xff;
    Status->Muted = 0xff;
    Status->ExtraDamage_up = 0xff;
    Status->ExtraDamage_down = 0xff;
    Status->Sleep = 0xff;
    Status->CanAttack = true;
    Status->CanRun = true;
    Status->CanMove = true;
    Status->CanCastSkill = true;
        // Stance
    Status->Stance = RUNNING;
    for(int i=0;i<30;i++)
    {
        MagicStatus[i].Status = 0;
        MagicStatus[i].Buff = 0;
        MagicStatus[i].Value =0;
        MagicStatus[i].Duration = 0;
        MagicStatus[i].BuffTime = 0;
    }
    CharType = 0;
    clientid = 0xffff;
    nearChar = NULL;
    findChar = NULL;
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

// update position
void CCharacter::UpdatePosition( )
{
    float ntime;
    clock_t etime = clock() - Position->lastMoveTime;
    if(IsOnBattle( ) && Battle->target != 0)
    {
        CCharacter* Target = GetCharTarget( );
        if(Target == NULL)
            ClearBattle( Battle );
    }
    if(!IsMoving())
    {
        if(IsPlayer())Position->lastMoveTime = clock();
        return;
    }
	float dx = Position->destiny.x - Position->current.x;
	float dy = Position->destiny.y - Position->current.y;
	float distance = sqrt( (dx*dx) + (dy*dy) );
	if(IsPlayer())
	{

        ntime = ( distance / (float(Stats->Move_Speed)/GServer->MOVE_SPEED_MOD)) * CLOCKS_PER_SEC; // calculates clock ticks to target position
        //Log( MSG_DEBUG, "Dist: %f Speed: %i Mod: %i ntime: %f", distance, Stats->Move_Speed, GServer->MOVE_SPEED_MOD, ntime);
    }
    if(IsMonster())
    {
        ntime = ( distance / (float(Stats->Move_Speed)/GServer->MOVE_SPEED_MOD)) * CLOCKS_PER_SEC; // calculates clock ticks to target position
        Position->current.x = dx*(etime/ntime) + Position->current.x;
    	Position->current.y = dy*(etime/ntime) + Position->current.y;

    }
    //float ntime = ( distance / Stats->Move_Speed * GServer->MOVE_SPEED_MOD ); // lmame method
    /*if(IsPlayer())
    {
        Log(MSG_DEBUG,"ntime = %f distance = %f ",ntime, distance );
    }*/

    if(IsOnBattle() && distance <= Stats->Attack_Distance) //this will prevent the character from moving if in battle and in range
    {
        Position->destiny.x = Position->current.x;
        Position->destiny.y = Position->current.y;
        dx = 0;
        dy = 0;
    }
	if (ntime <= etime || distance == 0)
    {
		/*if(IsPlayer())
		{
            Log(MSG_DEBUG,"Movement timed out or target reached. Set current to destiny.");
        }*/
        Position->current.x = Position->destiny.x;
		Position->current.y = Position->destiny.y;
    }
	else
    {
        if(IsPlayer())
        {
    		/*{
                Log(MSG_DEBUG,"Still moving. Distance to target %f time to target %f", distance, ntime);
            }*/
            Position->current.x = dx*(etime/ntime) + Position->current.x;
    		Position->current.y = dy*(etime/ntime) + Position->current.y;
        }
	}
	/*if(IsPlayer())
	{
    	if(Position->current.x == Position->destiny.x)
    	{
            Log(MSG_DEBUG,"Character is at destiny position in the X direction.");
        }
        else
        {
            Log(MSG_DEBUG,"Character is still moving. current x %f Destination  x %f",Position->current.x,Position->destiny.x);
        }
    }*/
	Position->lastMoveTime = clock();
}

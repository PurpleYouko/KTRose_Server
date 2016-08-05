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

// Props to ExJam for this code :D
#include "../WorldServer.h"

// extern CRandomMersenne rg;

//Stop all actions
AIACT(000)
{
	//SetCMD_Stop (Stop all actions i guess?)
    //entity->StopAll();
    CMonster* monster = reinterpret_cast<CMonster*>(entity);
    ClearBattle( monster->Battle );
    monster->Position->destiny = monster->Position->current;
	return AI_SUCCESS;
}

//Set Emotion
AIACT(001)
{
	//byte cAction;	//Pos: 0x00
	//CObjMOB::Set_EMOTION (pak 781)
	GETAIACTDATA(001);
	CMonster* monster = reinterpret_cast<CMonster*>(entity);

    BEGINPACKET( pak, 0x781 );
    ADDWORD    ( pak, data->cAction );
	ADDWORD    ( pak, 0);

	//LMA: bad clientid.
	//ADDWORD    ( pak, entity->clientid );
	ADDWORD    ( pak, monster->aip_clientid );

	//LMA: problem, sometimes it's a NPC, not a monster... And it looks in the wrong list.
	//GServer->SendToVisible(&pak, entity,NULL );
	GServer->SendToVisibleAIP(&pak, monster,NULL );


	return AI_SUCCESS;
}

//Say LTB String
AIACT(002)
{
	//Client side say text
	//Log(MSG_DEBUG, "Say LTB String");
	//dword iStrID;	      Pos: 0x00
	GETAIACTDATA(002);

	//Think this is crashing the server. Taken out for the time being.
    //lets have the NPC also say the LTB string over the regular message system. This will show up green ^-^
	CMonster* thisMonster = reinterpret_cast<CMonster*>(entity);
    //if(thisMonster->CharType == TNPC) // check if it is an NPC and not a normal monster.
	//{
    	// NPCname and LTBstring not set up correctly. commenting for now
		//BEGINPACKET( pak, 0x0784 );
        //ADDSTRING( pak, GServer->Ltbstring[data->iStrID].NPCname );
        //ADDBYTE( pak, 0 );
        //ADDSTRING( pak, GServer->Ltbstring[data->iStrID].LTBstring );
        //ADDBYTE( pak, 0 );
        //GServer->SendToVisible(&pak, thisMonster);
    //}
	return AI_SUCCESS;
}

//Move (walk or run) to a random location within iDistance of my present location
AIACT(003)
{
	//dword iDistance;	//Pos: 0x00
	//byte cSpeed;	//Pos: 0x04
	//move randomly within iDistance use randincircle
	GETAIACTDATA(003);
	CMonster* monster = reinterpret_cast<CMonster*>(entity);
    monster->UpdatePosition( false );
	int iDist = data->iDistance;// * 2;//Get it to our coord system!
	entity->Position->destiny = GServer->RandInCircle(entity->Position->current, iDist);
	monster->thisnpc->stance = data->cSpeed;
	monster->SetStats(false);
    entity->Position->lastMoveTime = clock();
    BEGINPACKET( pak, 0x797 );
	ADDWORD    ( pak, entity->clientid );
	ADDWORD    ( pak, 0x0000 );
	ADDWORD    ( pak, entity->Stats->Move_Speed ); //speed
	ADDFLOAT   ( pak, entity->Position->destiny.x*100 );
	ADDFLOAT   ( pak, entity->Position->destiny.y*100 );
	ADDWORD    ( pak, 0xcdcd );
	ADDBYTE    ( pak, monster->thisnpc->stance );
	GServer->SendToVisible(&pak, entity);
    //Log(MSG_DEBUG, "move1 stance %i",data->cSpeed);
    //Log(MSG_DEBUG, "move(1)");
	return AI_SUCCESS;
}

//Move (walk or run) to a random location within iDistance of my spawn location
AIACT(004)
{
	//dword iDistance;	//Pos: 0x00
	//byte cSpeed;	//Pos: 0x04
	//move randomly within iDistance of spawn position 
	GETAIACTDATA(004);
	CMonster* monster = reinterpret_cast<CMonster*>(entity);
    monster->UpdatePosition(false );
	int iDist = data->iDistance;//Get it to our coord system!
	entity->Position->destiny = GServer->RandInCircle(entity->Position->source, iDist);
	monster->thisnpc->stance = data->cSpeed;
	monster->SetStats(false);
    entity->Position->lastMoveTime = clock();
    BEGINPACKET( pak, 0x797 );
	ADDWORD    ( pak, entity->clientid );
	ADDWORD    ( pak, 0x0000 );//???
	ADDWORD    ( pak, entity->Stats->Move_Speed ); //speed
	ADDFLOAT   ( pak, entity->Position->destiny.x*100 );
	ADDFLOAT   ( pak, entity->Position->destiny.y*100 );
	ADDWORD    ( pak, 0xcdcd );
	ADDBYTE    ( pak, monster->thisnpc->stance );
	GServer->SendToVisible(&pak, entity);
    LogDebug( "move2 stance %i",data->cSpeed);

	return AI_SUCCESS;
}

//Move (walk or run) to a random location within iDistance of my findChar location
AIACT(005)
{
	//byte cSpeed;	//Pos: 0x00
	//move randomly within 200 of "m_pFindCHAR" 
	GETAIACTDATA(005);
	if(entity->findChar == NULL)
        return AI_FAILURE;
    CMonster* monster = reinterpret_cast<CMonster*>(entity);
    monster->UpdatePosition(false );
	entity->findChar->UpdatePosition(false);

	int iDist = 2;//Get it to our coord system!
	entity->Position->destiny = GServer->RandInCircle(entity->findChar->Position->current, iDist);
	monster->thisnpc->stance = data->cSpeed;
	monster->SetStats(false);
    BEGINPACKET( pak, 0x797 );
	ADDWORD    ( pak, entity->clientid );
	ADDWORD    ( pak, 0x0000 );//???
	ADDWORD    ( pak, entity->Stats->Move_Speed ); //speed
	ADDFLOAT   ( pak, entity->Position->destiny.x*100 );
	ADDFLOAT   ( pak, entity->Position->destiny.y*100 );
	ADDWORD    ( pak, 0xcdcd );
	ADDBYTE    ( pak, monster->thisnpc->stance );
	GServer->SendToVisible(&pak, entity);
    //LogDebug( "move3 stance %i",data->cSpeed);
    //	LogDebug( "move (3)");
    //    RESETPACKET(pak, 0x783);
    //    ADDWORD(pak,entity->findChar->clientid);//entity->clientid);
    //    ADDSTRING(pak,"hello there");
    //    ADDBYTE(pak,0);
    //    GServer->SendToVisible(&pak, entity);
    //		server->SendPacketToZone(entity, &pakout);

	return AI_SUCCESS;
}

//Run and Attack aiobj within iDistance that has the lowest or greatest cAbType
AIACT(006)
{
	//dword iDistance;	//Pos: 0x00
	//byte cAbType;	//Pos: 0x04
	//byte cMoreLess;	//Pos: 0x05
	//Run and Attack aiobj within iDistance that has the lowest or greatest cAbType
	//cAbType
	//0 = Level
	//1 = Attack
	//2 = Defense
	//3 = "Res"? Magic Resistance?
	//4 = HP
	//5 = Charm
	GETAIACTDATA(006);

    dword eCount = 0;
    int highestAB = -9999999;
    //	CWorldEntity* highestEntity = NULL;
    int lowestAB = 9999999;
    //	CWorldEntity* lowestEntity = NULL;
    CMonster* monster = reinterpret_cast<CMonster*>(entity);

	if(entity->Position->Map == 8)
	{
	    Log(MSG_INFO,"AIACT(006) monster moves");
	}

    CMap* map= GServer->MapList.Index[entity->Position->Map];
    CCharacter* Target = NULL;
    for(UINT i=0;i<map->PlayerList.size();i++)
    {
        CPlayer* player = map->PlayerList.at(i);
        if(player == NULL)continue;
        if(GServer->IsMonInCircle( monster->Position->current, player->Position->current, data->iDistance))
        {
            switch(data->cAbType)
            {
                case 0: //level
                    if(data->cMoreLess == 0)
                    {
                        if(player->Stats->Level > lowestAB)
                        {
                            lowestAB = player->Stats->Level;
                            Target = player;
                        }else continue;
                    }else{
                        if(player->Stats->Level < highestAB)
                        {
                            highestAB = player->Stats->Level;
                            Target = player;
                        }else continue;
                    }
                break;
                case 1: //Attack power
                    if(data->cMoreLess == 0)
                    {
                        if(player->Stats->Attack_Power > lowestAB)
                        {
                            lowestAB = player->Stats->Attack_Power;
                            Target = player;
                        }else continue;
                    }else{
                        if(player->Stats->Attack_Power < highestAB)
                        {
                            highestAB = player->Stats->Attack_Power;
                            Target = player;
                        }else continue;
                    }
                break;
                case 2: //defense
                    if(data->cMoreLess == 0)
                    {
                        if(player->Stats->Defense> lowestAB)
                        {
                            lowestAB = player->Stats->Defense;
                            Target = player;
                        }else continue;
                    }else{
                        if(player->Stats->Defense < highestAB)
                        {
                            highestAB = player->Stats->Defense;
                            Target = player;
                        }else continue;
                    }
                break;
                case 3: //Magic Def
                    if(data->cMoreLess == 0)
                    {
                        if(player->Stats->Magic_Defense> lowestAB)
                        {
                            lowestAB = player->Stats->Magic_Defense;
                            Target = player;
                        }else continue;
                    }else{
                        if(player->Stats->Magic_Defense < highestAB)
                        {
                            highestAB = player->Stats->Magic_Defense;
                            Target = player;
                        }else continue;
                    }
                break;
                case 4: // HP
                    if(data->cMoreLess == 0)
                    {
                        if(player->Stats->HP> lowestAB)
                        {
                            lowestAB = player->Stats->HP;
                            Target = player;
                        }else continue;
                    }else{
                        if(player->Stats->HP < highestAB)
                        {
                            highestAB = player->Stats->HP;
                            Target = player;
                        }else continue;
                    }
                break;
                case 5: // Charm
                    if(data->cMoreLess == 0)
                    {
                        if(player->Attr->Cha > lowestAB)
                        {
                            lowestAB = player->Attr->Cha;
                            Target = player;
                        }else continue;
                    }else{
                        if(player->Attr->Cha < highestAB)
                        {
                            highestAB = player->Attr->Cha;
                            Target = player;
                        }else continue;
                    }
                break;
            }
        }
    }
    // OK we found the new Target player. now attack him.
    if (Target==NULL)
    {
        Log(MSG_INFO,"No suitable target found for type %i dist %i",data->cAbType,data->iDistance);
        //return AI_FAILURE;	//PY: Should never return AI_FAILURE from an action. If it did then the AIP will move to the next condition and that's just not right
    }

    monster->thisnpc->stance = 1;
    monster->SetStats(false);
    monster->StartAction( Target, sBASIC_ACTION, 0 );

    //	int searchDistance = data->iDistance * 100;

    //	CWorldEntity** entityList = entity->thisZone->GetEntityList();
    //	dword entityCount = entity->thisZone->GetEntityCount();
    //	for(dword i = 1; i < MAX_ZONE_CLIENTID; i++){
    //		CWorldEntity* other = entityList[i];
    //		if(eCount >= entityCount) break;
    //		if(other == NULL) continue;
    //		eCount++;
    //		if(other == entity) continue;
    //		if(other->_EntityType == ENTITY_NO_TYPE) continue;
    //		if(other->_EntityType == ENTITY_DROP) continue;

    //		int dX = (int)abs((int)other->curBlock.x - (int)entity->curBlock.x);
    //		if(dX > 1) continue;
    //		int dY = (int)abs((int)other->curBlock.y - (int)entity->curBlock.y);
    //		if(dY > 1) continue;

    //		int iDistance = other->basic.pos.distance(entity->basic.pos);
    //		if(iDistance > searchDistance) continue;

    //		int value = AI_GetAbility(other, data->cAbType);

    //		if(value < lowestAB){
    //			lowestAB = value;
    //			lowestEntity = other;
    //		}
    //		if(value > highestAB){
    //			highestAB = value;
    //			highestEntity = other;
    //		}
    //	}
    //	if(data->cMoreLess){
    //		if(lowestEntity){
			//SetCmdRUN_nATTACK
    //			entity->Attack(lowestEntity);
    //		}
    //	}else{
    //		if(highestEntity){
			//SetCmdRUN_nATTACK
    //			entity->Attack(highestEntity);
    //		}
    //	}
    //LogDebug( "move (?)");
	return AI_SUCCESS;
}

//Unknown
AIACT(007)
{
	//CObjCHAR::Special_ATTACK
	//LogDebug( "AIACT(007)");
	return AI_SUCCESS;
}

//move towards target. Stop at a specified precentage away from target
AIACT(008)
{
	//dword iDistance;	//Pos: 0x00
	//byte cSpeed;	//Pos: 0x04
	//move towards target. Stop at a specified precentage away from target

	GETAIACTDATA(008);
    CMonster* monster = reinterpret_cast<CMonster*>(entity);
    monster->UpdatePosition(false);
	CCharacter* target = entity->GetCharTarget( );
	if(target == NULL)
        return AI_SUCCESS;
	int iDist = data->iDistance;//Get it to our coord system!
	float xDist = target->Position->current.x - monster->Position->current.x;
	float yDist = target->Position->current.y - monster->Position->current.y;
	float fDistance = GServer->distance( entity->Position->current, target->Position->current);
	float nX = monster->Position->source.x + (xDist * iDist / 100);
	float nY = monster->Position->source.y + (yDist * iDist / 100);
	monster->Position->destiny.x = nX;
	monster->Position->destiny.y = nY;
	monster->thisnpc->stance = data->cSpeed;
	monster->SetStats(false);
    monster->Position->lastMoveTime = clock();
    ClearBattle( entity->Battle );// this also seems to clear the attackers battle??
    //entity->Battle->atktype = 0;
    BEGINPACKET( pak, 0x797 );
	ADDWORD    ( pak, entity->clientid );
	ADDWORD    ( pak, 0x0000 );//???
	ADDWORD    ( pak, monster->Stats->Move_Speed ); //speed
	ADDFLOAT   ( pak, monster->Position->destiny.x*100 );
	ADDFLOAT   ( pak, monster->Position->destiny.y*100 );
	ADDWORD    ( pak, 0xcdcd );
	ADDBYTE    ( pak, monster->thisnpc->stance );
	GServer->SendToVisible(&pak, entity);
	return AI_SUCCESS;
}

//Convert Monster to new monster (1)
AIACT(009)
{
	//word wMonster;	//Pos: 0x00
	//Transform into wMonster
	GETAIACTDATA(010);
	CMap* map= GServer->MapList.Index[entity->Position->Map];
	CMonster* monster = reinterpret_cast<CMonster*>(entity);
	int oldmon = monster->montype;
	map->ConverToMonster(monster,data->wMonster,1);
	//LogDebug( "AIACT(009)convert monster type %i to %i",oldmon,data->wMonster);
	return AI_SUCCESS;
}

//Spawn Monster (2)
AIACT(010)
{
	//word wMonster;	//Pos: 0x00
	//Spawn wMonster at current X,Y
	GETAIACTDATA(010);
	CMap* map = GServer->MapList.Index[entity->Position->Map];
	fPoint position;
    //CMonster* newmonster;
    position = GServer->RandInCircle( entity->Position->current, 1 );
    map->AddMonster( data->wMonster, position, 0);
    //	entity->thisZone->SpawnMonster(data->wMonster, entity->basic.pos, entity->basic.map);
    //LogDebug( "AIACT(010)spawn %i",data->wMonster);
	return AI_SUCCESS;
}

//Call for backup
AIACT(011)
{
	//dword iDistance;	//Pos: 0x00
	//dword iNumOfMonster;	//Pos: 0x04
	//force iNumOfMonster of same team within iDistance to attack my target
    //CWorldEntity* target = entity->thisZone->GetEntity(entity->_TargetID);
    //if(target == NULL) return AI_FAILURE;
    //LogDebug( "AIACT(011)");
    CCharacter* target = entity->GetCharTarget( );
    if(target == NULL) return AI_FAILURE;
	GETAIACTDATA(011);

	int chrCount = 0;
	dword eCount = 0;
	int searchDistance = data->iDistance;

	CMap* map = GServer->MapList.Index[entity->Position->Map];
	dword entityCount = map->MonsterList.size();
	for(UINT j=0;j<map->MonsterList.size();j++)
    {
        CMonster* other = map->MonsterList.at(j);
        if(eCount >= entityCount) break;
        if(other == NULL) continue;
        eCount++;
        if(other == entity) continue;
        if(other->CharType != 0) continue;

        //	CWorldEntity** entityList = entity->thisZone->GetEntityList();
        //	dword entityCount = entity->thisZone->GetEntityCount();
        //	for(dword i = 1; i < MAX_ZONE_CLIENTID; i++){
        //		CWorldEntity* other = entityList[i];
        //		if(eCount >= entityCount) break;
        //		if(other == NULL) continue;
        //		eCount++;
        //		if(other == entity) continue;
        //		if(other->_EntityType == ENTITY_MONSTER) continue;
        //		if(other->team != entity->team) continue;
        //		int dX = (int)abs((int)other->curBlock.x - (int)entity->curBlock.x);
        //		if(dX > 1) continue;
        //		int dY = (int)abs((int)other->curBlock.y - (int)entity->curBlock.y);
        //		if(dY > 1) continue;
        int iDistance = (int) GServer->distance( other->Position->current, entity->Position->current );
        //		int iDistance = other->basic.pos.distance(entity->basic.pos);
		if(iDistance > searchDistance) continue;
		chrCount++;
		other->Battle->target = entity->Battle->target;
		other->StartAction( (CCharacter*) target, sBASIC_ACTION, 0 );

        //		other->Attack(target);

		if(chrCount >= data->iNumOfMonster) return AI_SUCCESS;
	}

	return AI_SUCCESS;
}

//start attack
AIACT(012)
{
    //LogDebug( "AIACT(012)");
	//Run and attack "m_pNearCHAR" Nearest Character
    //	entity->Attack(entity->nearChar);
    //    LogDebug( "AIACT(012)");
    if(entity->nearChar == NULL)return AI_FAILURE;
    //entity->Battle->target=entity->nearChar->clientid;
    CMonster* monster = reinterpret_cast<CMonster*>(entity);

    /*LMA: ospRose test, agressiveness */
    if(monster->thisnpc->aggresive == 0)
            return AI_FAILURE; //monster has no agression value so cannot attack a player
        int aggro = GServer->RandNumber(1,20);
        if(aggro > monster->thisnpc->aggresive)
            return AI_FAILURE; // failed aggressiveness check
    /*LMA: ospRose test, agressiveness END */

    monster->thisnpc->stance=1;
    monster->SetStats();
    monster->StartAction( (CCharacter*) entity->nearChar, sBASIC_ACTION, 0 );
    //LogDebug( "AIACT(012)%i",entity->nearChar->clientid);

	if(entity->Position->Map==8)
	{
	    //Log(MSG_INFO,"AIACT(012) Start Attack");
	}


	return AI_SUCCESS;
}

//start attack
AIACT(013)
{
    //LogDebug( "AIACT(013)");
	//Run and attack "m_pFindCHAR" Character found from aiobj loop things
    //	entity->Attack(entity->findChar);
    if(entity->findChar == NULL)return AI_FAILURE;
    //entity->Battle->target=entity->findChar->clientid;
    CMonster* monster = reinterpret_cast<CMonster*>(entity);

    /*LMA: ospRose test, agressiveness */
    if(monster->thisnpc->aggresive == 0)
            return AI_FAILURE; //monster has no agression value so cannot attack a player
        int aggro = GServer->RandNumber(1,20);
        if(aggro > monster->thisnpc->aggresive)
            return AI_FAILURE; // failed aggressiveness check
    /*LMA: ospRose test, agressiveness END */

    monster->thisnpc->stance = 1;
    monster->SetStats();
    monster->Battle->target = entity->findChar->clientid;
    monster->StartAction( (CCharacter*) entity->findChar, sBASIC_ACTION, 0 );
    //    LogDebug( "AIACT(013)%i",entity->findChar->clientid);

	/*if(entity->Position->Map==8||entity->Position->Map==59)
        Log(MSG_INFO,"AIACT(013) monster %i attacks %i",entity->clientid,entity->findChar->clientid);*/

	return AI_SUCCESS;
}


//Unknown
AIACT(014)
{
	//dword iDistance;	//Pos: 0x00
	//find aiobj within iDistance and force them to attack my target
    //	CWorldEntity* target = entity->thisZone->GetEntity(entity->_TargetID);
    //	if(target == NULL) return AI_FAILURE;
	GETAIACTDATA(014);


    //	dword eCount = 0;
    //	int searchDistance = data->iDistance * 100;

    //	CWorldEntity** entityList = entity->thisZone->GetEntityList();
    //	dword entityCount = entity->thisZone->GetEntityCount();
    //	for(dword i = 1; i < MAX_ZONE_CLIENTID; i++){
    //		CWorldEntity* other = entityList[i];
    //		if(eCount >= entityCount) break;
    //		if(other == NULL) continue;
    //		eCount++;
    //		if(other == entity) continue;
    //		if(other->_EntityType == ENTITY_NO_TYPE) continue;
    //		if(other->_EntityType == ENTITY_DROP) continue;
    //		if(other->team != entity->team) continue;

    //		int dX = (int)abs((int)other->curBlock.x - (int)entity->curBlock.x);
    //		if(dX > 1) continue;
    //		int dY = (int)abs((int)other->curBlock.y - (int)entity->curBlock.y);
    //		if(dY > 1) continue;

    //		int iDistance = other->basic.pos.distance(entity->basic.pos);
    //		if(iDistance > searchDistance) continue;

    //		other->Attack(target);
    //		return AI_SUCCESS;
    //	}
    //LogDebug( "AIACT(014)");
	return AI_SUCCESS;
}

//Retaliate against the last character that hit me
/*
AIACT(015)
{
	//Run and attack "m_pDestCHAR" Blah?
    //	LogDebug( "AIACT(015)");
    CMonster* monster = reinterpret_cast<CMonster*>(entity);
    //if(monster->Battle->hitby == monster->Battle->target)
    //    return AI_FAILURE; //no need to initiate combat. I am already fighting this player

    if(monster->IsOnBattle())
        return AI_FAILURE; //already in battle

    monster->thisnpc->stance = 1;
    monster->SetStats();
    monster->Battle->target = monster->Battle->hitby;
    CCharacter* target = entity->GetCharTarget( );
    monster->StartAction( target, sBASIC_ACTION, 0 );

	if(entity->Position->Map==8)
        Log(MSG_INFO,"AIACT(015) monster retaliates");

	return AI_SUCCESS;
}
*/

/* PurpleYouko version */
//Retaliate against the last character that hit me
AIACT(015)
{
    //Run and attack "m_pDestCHAR" Blah?
    //LogDebug( "AIACT(015)");
    CMonster* monster = reinterpret_cast<CMonster*>(entity);
    if(monster->thisnpc->aggresive == 0)
    {
        LogDebug( "AIACT(015) returned false as monster has no aggression");
        return AI_FAILURE; //monster has no agression value so cannot attack a player
    }

    //LMA: Bad idea.
    //-> it's causing zombie monsters.
    //if a monster is in battle mode and player skills it, it goes into "is attacked" mode but has already the player as target
    //since he was in battle mode. So the refresh isn't done client side...
    /*if(monster->IsOnBattle())
    {
        //already in battle so we don't want it switching all the time. maybe 75% of the time it will do nothing?
        int myrand=GServer->RandNumber(0,100);
        if(myrand > 25)
        {
            LogDebug( "AIACT(015) returned false as we got a really bad random number %i",myrand);
            return AI_FAILURE; //denied!!!
        }

        if(monster->Battle->hitby == monster->Battle->target)
        {
            LogDebug( "AIACT(015) returned false as monster is already in combat with this player %i",monster->Battle->target);
            return AI_FAILURE; //no need to initiate combat. I am already fighting this player
        }
    }*/

    // otherwise change target to the person who just hit me
    monster->thisnpc->stance = 1;
    monster->SetStats();
    monster->Battle->target = monster->Battle->hitby;
    CCharacter* target = entity->GetCharTarget( );
    if(target == NULL)
    {
        LogDebug( "I seem to have lost my target so I can't attack any more");
        return AI_FAILURE;
    }
    LogDebug( "Starting normal attack against opponent %i",target->clientid);
    monster->StartAction( target, sBASIC_ACTION, 0 );

	if(entity->Position->Map==8)
	{
	    Log(MSG_INFO,"AIACT(015) monster retaliates");
	}


    return AI_SUCCESS;
}
/* PurpleYouko version */


//Run away
AIACT(016)
{
	//dword iDistance;
	//Run Away!
    //LogDebug( "AIACT(016)");
	GETAIACTDATA(016);
	CMonster* monster = reinterpret_cast<CMonster*>(entity);
    monster->UpdatePosition(monster->stay_still);

	float radius = data->iDistance;
    float angle = rand()%(360);
    //	float angle = rg.IRandom(0,360);
    //	entity->UpdatePosition();
    //	entity->MoveTo(entity->basic.pos.x + (radius * cos(angle)), entity->basic.pos.y + (radius * sin(angle)));
	float nX = (entity->Position->source.x + (radius * cos(angle)));
	entity->Position->destiny.x=nX;
	float nY = (entity->Position->source.y + (radius * cos(angle)));
	entity->Position->destiny.y=nY;

	monster->thisnpc->stance = 1;
	monster->SetStats();

    entity->Position->lastMoveTime = clock();
    //ClearBattle( entity->Battle );// this also seems to clear the attackers battle??
    entity->Battle->atktype = 0;
    BEGINPACKET( pak, 0x797 );
	ADDWORD    ( pak, entity->clientid );
	ADDWORD    ( pak, 0x0000 );//???
	ADDWORD    ( pak, entity->Stats->Move_Speed ); //speed
	ADDFLOAT   ( pak, entity->Position->destiny.x*100 );
	ADDFLOAT   ( pak, entity->Position->destiny.y*100 );
	ADDWORD    ( pak, 0xcdcd );
	ADDBYTE    ( pak, 0x01 );
	GServer->SendToVisible(&pak, entity);


	if(entity->Position->Map==8)
	{
	    //Log(MSG_INFO,"AIACT(016) monster runs away");
	}


	return AI_SUCCESS;
}

//Drop Item
AIACT(017)
{
	//word item0;	//Pos: 0x00
	//word item1;	//Pos: 0x02
	//word item2;	//Pos: 0x04
	//word item3;	//Pos: 0x06
	//word item4;	//Pos: 0x08
	//dword iToOwner;	//Pos: 0x0c
	//Random drop one of item 1-5
	GETAIACTDATA(017);
	//LogDebug( "AIP DROP 017");

	//srand(time(NULL));
    int itemRand = rand()%(5);
	//int itemRand = rg.IRandom(0,360);
	int nItem = data->items[itemRand];
	if (nItem == 0)return AI_FAILURE;

	//CItem dropItem;
    CDrop* newdrop = new (nothrow) CDrop;
	newdrop->clientid = GServer->GetNewClientID( );
    newdrop->posMap = entity->Position->Map;
    newdrop->pos = GServer->RandInCircle( entity->Position->current, 3 );
    newdrop->droptime = time(NULL);
    newdrop->owner = 0;//entity->MonsterDrop->firsthit;
    newdrop->thisparty = 0;//entity->thisparty;
    ClearItem(newdrop->item);
	newdrop->amount = 1;
	newdrop->type = 2;

	//LMA: correct way...
	//newdrop->item.itemnum = nItem % 1000;
	//newdrop->item.itemtype = nItem / 1000;
	newdrop->item.itemtype = GServer->gi(nItem,0);
	newdrop->item.itemnum = GServer->gi(nItem,1);

	if(newdrop->item.IsStackable())
    {
		newdrop->item.count = 1;
	}
    else
    {
		newdrop->item.stats = 0;
		newdrop->item.durability = 40;
		newdrop->item.lifespan = 1000;
		newdrop->item.socketed = 0;
		newdrop->item.appraised = 1;
		newdrop->item.refine = 0;
	}
    //LogDebug( "AIP DROP item type=  %i item num = %i", newdrop->item.itemtype, newdrop->item.itemnum);
	newdrop->item.count = 1;


	CMap* map = GServer->MapList.Index[newdrop->posMap];
		map->AddDrop( newdrop );
    //	entity->DropEntityItem(&dropItem);

	return AI_SUCCESS;
}

//Make nearby monsters attack my target
AIACT(018)
{
	//word cMonster;	//Pos: 0x00
	//word wHowMany;	//Pos: 0x02
	//dword iDistance;	//Pos: 0x04
	//make wHowMany monsters of type cMonster within iDistance attack my target
	GETAIACTDATA(018);
    CMonster* monster = reinterpret_cast<CMonster*>(entity);
	CMap* map = GServer->MapList.Index[entity->Position->Map];
	CCharacter* target = entity->GetCharTarget( );
	if(target == NULL)
        return AI_SUCCESS;		//PY: NEVER return AI_FAILURE from actions
	int monstercount = 0;
    for(UINT j=0;j<map->MonsterList.size();j++)
    {
        CMonster* othermon = map->MonsterList.at(j);
        if(othermon == NULL)
            continue;
        if(othermon == monster)
            continue;
        if(othermon->montype != data->cMonster)
            continue;
        if(GServer->IsMonInCircle( monster->Position->current, othermon->Position->current, data->iDistance))
        {
            othermon->StartAction( target, sBASIC_ACTION, 0);
            monstercount++;
            if(monstercount >= data->wHowMany)
            return AI_SUCCESS;
        }
    }
	return AI_SUCCESS;
}

//Unknown
AIACT(019)
{
	//Run and attack "m_pNearCHAR" //Identical to 012
	return F_AI_ACT_012(server, entity, raw,AipId);
}

//Spawn Monster (5)
AIACT(020)
{
	//word cMonster;	//Pos: 0x00
	//byte btPos;	//Pos: 0x02
	//dword iDistance;	//Pos: 0x04

	//Spawn cMonster within iDistance of btPos

	//btPos 0 = my pos
	//btPos 1 = "m_pDestCHAR" pos
	//btPos 2 = targets pos
	GETAIACTDATA(020);
	CMap* map = GServer->MapList.Index[entity->Position->Map];
	CMonster* monster = reinterpret_cast<CMonster*>(entity); //need this to get the spawnid of the entity
	if(monster == NULL) return AI_SUCCESS;

	fPoint position;

	if(data->btPos == 0)
    {
        position = GServer->RandInCircle( entity->Position->current, data->iDistance );
        map->AddMonster( data->cMonster, position, 0, monster->Status->spawnid);
    }
    else if(data->btPos == 1)
    {
        position = GServer->RandInCircle( entity->Position->current, data->iDistance );
        map->AddMonster( data->cMonster, position, 0,  monster->Status->spawnid );
		//dest char
	}
    else if(data->btPos == 2)
    {
        CCharacter* target = entity->GetCharTarget( );
        if(target == NULL) return AI_FAILURE;
        position = GServer->RandInCircle( target->Position->current, data->iDistance );
        map->AddMonster( data->cMonster, position, 0,  monster->Status->spawnid );
    }
    //LogDebug( "AIACT(020)option spawn %i distance %i monstertype %i",data->btPos,data->iDistance,data->cMonster);
	return AI_SUCCESS;
}

//Unknown
AIACT(021)
{
	//Does absolutely nothing
	return AI_SUCCESS;
}

//Unknown
AIACT(022)
{
	//Does absolutely nothing
	return AI_SUCCESS;
}

//ghostseeds and ghosts and player summons commit suicide
AIACT(023)
{
	//Commit suicide
	//LogDebug( "AIACT(023) Suicide");
    CMonster* thisMonster = reinterpret_cast<CMonster*>(entity);
    LogDebug( "AIACT(023) Suicide for monster %u, %i",thisMonster->montype,entity->clientid);
    entity->Stats->HP = -1;
    BEGINPACKET( pak, 0x799 );
    ADDWORD    ( pak, entity->clientid );
    ADDWORD    ( pak, entity->clientid );
    ADDDWORD   ( pak, thisMonster->thisnpc->hp*thisMonster->thisnpc->level );
    ADDDWORD   ( pak, 16 );
    GServer->SendToVisible( &pak, entity );
    thisMonster->suicide=true;
	return AI_SUCCESS;
}

//Do Skill
AIACT(024)
{
	//byte btTarget;	//Pos: 0x00
	//word nSkill;	//Pos: 0x02
	//word nMotion;	//Pos: 0x04 //8 = SKILL_ATTACK 6 = magic

	//Use nSkill & nMotion?
	//btTarget 0 = m_pFindCHAR
	//btTarget 1 = target
	//btTarget 2 = self skill
	GETAIACTDATA(024);
    //	LogDebug( "Do Skill btTarget %i nSkill %i nMotion %i",data->btTarget,data->nSkill,data->nMotion);
    //	LogDebug( "entity %i",entity->clientid);
    //	LogDebug( "entity->findChar %i",entity->findChar->clientid);
    //	LogDebug( "entity->nearChar %i", entity->nearChar->clientid);
	CSkills* thisskill = GServer->GetSkillByID( data->nSkill );
	if(thisskill==NULL)
	{
        Log(MSG_WARNING,"AIA024:: Skill %i doesn't exist!",data->nSkill);
        return AI_FAILURE;
	}

	CCharacter* monster = entity;
	int skill_timer=thisskill->duration*1000;
	//if(monster->Battle->atktype != 0)
	//{
    //    LogDebug("Cannot apply battle attacktype. Monster already has one");
    //    return AI_FAILURE;
    //}
    //LogDebug("AIACT(024)skill attack id: %i target: %i type: %i btTarget: %i nMotion: %i", data->nSkill, thisskill->target, thisskill->skilltype, data->btTarget, data->nMotion);
	switch (thisskill->skilltype)
	{
        case 3: //damage action
        case 6: //magic attack
        {
            CCharacter* target = entity->GetCharTarget( );
            //LogDebug("SKILL_ATTACK or DAMAGE_ACTION selected");
            if(target==NULL)
            {
                //LogDebug("No target can be found for this skill");
                return AI_FAILURE;
            }
            //LogDebug("SKILL_ATTACK selected. target: %i",target->clientid);
            monster->StartAction( (CCharacter*) target, SKILL_ATTACK, data->nSkill );
        }
        break;
        case 7: //AOE magic attack
        {
            CCharacter* target = entity->GetCharTarget( );
            //LogDebug("AOE_TARGET selected");
            if(target == NULL)
            {
                //LogDebug("No target can be found for this skill");
                return AI_FAILURE;
            }
            //LogDebug("AOE_TARGET selected. target: %i",target->clientid);
            monster->StartAction( (CCharacter*) target, AOE_TARGET, data->nSkill );
        }
        break;
        case 8: //self buff
        case 9: //skill buff
        case 13://skill buff
        case 11: //heal
        {
            switch(thisskill->target)
            {
                case 0://self buff
                {
                    //LogDebug("BUFF_SELF selected.");
                    monster->StartAction( NULL, BUFF_SELF, data->nSkill );
                    //LMA: no special time to add.
                    skill_timer=0;
                }
                break;
                default:
                {
                    if(thisskill->aoeradius == 0)
                    {
                        CCharacter* target = entity->GetCharTarget( );
                        if(target == NULL) //some skills always return a NULL target here
                        {
                            if(entity->nearChar == NULL) // maybe there really is no possible target.
                            {
                                //LogDebug("No target can be found for this skill");
                                return AI_FAILURE;
                            }
                            else //so we are going to have them select a NEW target (if possible) rather than return AI_FAILURE
                            {
                                //LogDebug("SKILL_BUFF type 13 selected. NEW target: %i",entity->nearChar->clientid);
                                monster->StartAction( (CCharacter*) entity->nearChar, SKILL_BUFF, data->nSkill );
                            }
                        }
                        else
                        {
                            //LogDebug("SKILL_BUFF type %i selected. target: %i",thisskill->skilltype, target->clientid);
                            monster->StartAction( (CCharacter*) target, SKILL_BUFF, data->nSkill );
                        }
                    }
                    else
                    {
                        //LMA: no need to do the same AOE buff all over again if not already done...
                        if(monster->Battle->atktype!=SKILL_AOE&&monster->Battle->skillid!=data->nSkill)
                        {
                            monster->StartAction( NULL, SKILL_AOE, data->nSkill );
                        }
                    }
                }
                break;
            }
        }
        break;

        case 10: //heal AOE centered on self
            //LogDebug("BUFF_AOE selected. Skill id %i", data->nSkill);
            monster->StartAction( NULL, BUFF_AOE, data->nSkill );
        break;
        case 17: //skill AOE centered on self
        {
            //LogDebug("SKILL_AOE selected. Skill id %i", data->nSkill);
            //LMA: no need to do the same AOE buff all over again if not already done...
            if(monster->Battle->atktype!=SKILL_AOE&&monster->Battle->skillid!=data->nSkill)
                monster->StartAction( NULL, SKILL_AOE, data->nSkill );
        }
        break;
        default:
        {
            ////Log(MSG_WARNING,"Skill type not recognized. type: %i",thisskill->skilltype);
        }
        break;
    }

    //LMA: next attack.
    monster->nextAi_attacked+=skill_timer;


	return AI_SUCCESS;
}

//Set Variable (1)
AIACT(025)
{
    //	if(entity->_EntityType != ENTITY_NPC) return AI_FAILURE;
	GETAIACTDATA(025);
	//LogDebug("AIP25 BEGIN");

	CMonster* monster = reinterpret_cast<CMonster*>(entity);
	if(monster == NULL) return AI_FAILURE;
	if(data->btVarIDX > 19) return AI_FAILURE;
	if(monster->thisnpc->refNPC>=MAX_NPC)
	{
	    //Log(MSG_WARNING,"It seems NPC %i>=%u , change MAX_NPC value!",monster->thisnpc->refNPC,MAX_NPC);
	    return AI_FAILURE;
	}

	short tempval = GServer->ObjVar[monster->thisnpc->refNPC][data->btVarIDX];

	//LogDebug("AIP025:: Set Objvar[%i][%i], data->btOp=%i, data->iValue=%i",monster->thisnpc->refNPC,data->btVarIDX,data->btOp,data->iValue);

	switch(data->btOp)
    {
        case 5:
            tempval = data->iValue;
            break;
        case 6:
            tempval += data->iValue;
            break;
        case 7:
            tempval -= data->iValue;
            break;
        case 9:
            tempval++;
            break;
        default:
            return AI_SUCCESS;
            break;
    }
    if(tempval < 0)tempval = 0;

	//PY: Loads more LMA special cases down here. I'll figure these out later. They shouldn't be necessary

    /*
    if(monster->thisnpc->refNPC==1201)
    {
        Log(MSG_INFO,"Judy changes [%i] to %i",data->btVarIDX,tempval);
    }
    */

    GServer->ObjVar[monster->thisnpc->refNPC][data->btVarIDX] = tempval;
    //LogDebug("AIP025:: new value of Objvar[%i][%i]=%i",monster->thisnpc->refNPC,data->btVarIDX,GServer->ObjVar[monster->thisnpc->refNPC][data->btVarIDX]);

    //LMA: EventID changed?
    //Mainprocess will take care of it since we don't have client ID here.
    if(data->btVarIDX == 0)
    {
        //LogDebug("AIACT(025) eventID has changed from %i to %i",monster->thisnpc->eventid,tempval);
        monster->thisnpc->eventid=tempval;
    }

    //LMA: Special case for UW, we saev the uwside for next time.
    if(monster->thisnpc->refNPC == 1113 && data->btVarIDX == 2)
    {
        GServer->DB->QExecute("UPDATE list_config SET uwside=%i",GServer->ObjVar[monster->thisnpc->refNPC][data->btVarIDX]);
    }

    //LMA: Special case for gem quest (spawn reset)
    if (AipId==1104&&monster->thisnpc->refNPC==1104&&data->btVarIDX==13&&GServer->GemQuestReset>0&&GServer->ObjVar[1104][13]>GServer->GemQuestReset)
    {
        GServer->ObjVar[1104][13]=0;
    }


	return AI_SUCCESS;
}

//Set Variable (2)
AIACT(026)
{
	//Set WorldVAR
	//word nVarNo;	//Pos: 0x00
	//dword iValue;	//Pos: 0x04
	//byte btOp;	//Pos: 0x08
	GETAIACTDATA(026);
	//PY: we don't know how to use worldVar so here are some fun things we can do with custom AI
	switch(data->nVarNo)
	{
	    case 1:	//change world exp rate in the config
            GServer->Config.EXP_RATE = data->iValue;
            return AI_SUCCESS;
            break;
        default:
            return AI_SUCCESS;
            break;
    }
}

//Set Variable (3)
AIACT(027)
{
	//Set EconomyVAR
	//word nVarNo;	//Pos: 0x00
	//byte btOp;	//Pos: 0x08
	//dword iValue;	//Pos: 0x04
	return AI_SUCCESS;
}

//Shout/Ann LTB String
AIACT(028)
{
	GETAIACTDATA(028);

    dword npcId = 0;
    //if(entity->_EntityType == ENTITY_NPC)

    if(data->iStrID>=GServer->maxltbaip)
    {
        //Log(MSG_INFO,"AIACT(028) AIP LTB index error %i>=%i",data->iStrID,GServer->maxltbaip);
        return AI_SUCCESS;
    }

    CMonster* thisMonster = reinterpret_cast<CMonster*>(entity);
    switch(data->btMsgType)
    {
        case 0: //whisper to client
        {
            // don't think this is possible. No way to get a character to send it to??
            //LMA: In fact he's talking to his master.
            CMap* map = GServer->MapList.Index[thisMonster->Position->Map];
            CPlayer* caller = map->GetPlayerInMap( thisMonster->owner );
                if(caller == NULL) return AI_FAILURE;

            if(thisMonster->thisnpc->refNPC!=0)
            {
                GServer->NPCWhisper(caller,thisMonster,GServer->Ltbstring[data->iStrID]->LTBstring,GServer->GetSTLMonsterNameByID(thisMonster->thisnpc->refNPC));
            }
            else
            {
                GServer->NPCWhisper(caller,thisMonster,GServer->Ltbstring[data->iStrID]->LTBstring,GServer->GetSTLMonsterNameByID(thisMonster->aip_npctype));
            }

        }
        break;
        case 1: //shout to map
        {
            //Log(MSG_INFO,"AIP NPC %s (%i) shouts to map.",GServer->GetNPCNameByType(thisMonster->aip_npctype),thisMonster->aip_npctype);
            //GServer->NPCShout(thisMonster,GServer->Ltbstring[data->iStrID]->LTBstring,GServer->Ltbstring[data->iStrID]->NPCname);
            //GServer->NPCShout(thisMonster,GServer->Ltbstring[data->iStrID]->LTBstring,GServer->GetNPCNameByType(thisMonster->aip_npctype));
            if(thisMonster->thisnpc->refNPC!=0)
            {
                GServer->NPCShout(thisMonster,GServer->Ltbstring[data->iStrID]->LTBstring,GServer->GetSTLMonsterNameByID(thisMonster->thisnpc->refNPC));
            }
            else
            {
                GServer->NPCShout(thisMonster,GServer->Ltbstring[data->iStrID]->LTBstring,GServer->GetSTLMonsterNameByID(thisMonster->aip_npctype));
            }

        }
        break;
        case 2: //announce to server.
        {
            //Log(MSG_INFO,"AIP NPC %s (%i) announces to server.",GServer->GetNPCNameByType(thisMonster->aip_npctype),thisMonster->aip_npctype);
            //GServer->NPCAnnounce(GServer->Ltbstring[data->iStrID]->LTBstring,GServer->Ltbstring[data->iStrID]->NPCname);
            //GServer->NPCAnnounce(GServer->Ltbstring[data->iStrID]->LTBstring,GServer->GetNPCNameByType(thisMonster->aip_npctype));
            if(thisMonster->thisnpc->refNPC!=0)
            {
                GServer->NPCAnnounce(GServer->Ltbstring[data->iStrID]->LTBstring,GServer->GetSTLMonsterNameByID(thisMonster->thisnpc->refNPC),thisMonster->Position->Map);
            }
            else
            {
                GServer->NPCAnnounce(GServer->Ltbstring[data->iStrID]->LTBstring,GServer->GetSTLMonsterNameByID(thisMonster->aip_npctype),thisMonster->Position->Map);
            }

        }
        break;
    }

    LogDebug( "AIACT(028)");


	return AI_SUCCESS;
}

//Move to my owner's location
AIACT(029)
{
	//Move within 20% of the distance of my "CALLER"
	GETAIACTDATA(029);
    //	if(entity->_EntityType != ENTITY_MONSTER) return AI_FAILURE;
    //	CMonster* thisMonster = reinterpret_cast<CMonster*>(entity);
    //	CWorldEntity* caller = thisMonster->thisZone->GetEntity(thisMonster->_CallerID);
    //	if(caller == NULL) return AI_FAILURE;
    //	caller->UpdatePosition();
    //	thisMonster->UpdatePosition();
    if(!entity->IsMonster( )) return AI_FAILURE;
	CMonster* thisMonster = reinterpret_cast<CMonster*>(entity);
	CMap* map = GServer->MapList.Index[thisMonster->Position->Map];
	CCharacter* caller = map->GetCharInMap( thisMonster->owner );
	if(caller == NULL) return AI_FAILURE;
	caller->UpdatePosition(false);
	thisMonster->UpdatePosition(thisMonster->stay_still);


    //	float fMoveDistance = thisMonster->basic.pos.distance(caller->basic.pos) * 0.2;

    //	float m = float(caller->basic.pos.y - thisMonster->basic.pos.y) / float(caller->basic.pos.x - thisMonster->basic.pos.x);
    //	float c = (m * thisMonster->basic.pos.x) + thisMonster->basic.pos.y;
    //	float y = (m * fMoveDistance) + c;
    //	float x = (fMoveDistance - c) / m;
    //	thisMonster->stance = 1;
	thisMonster->thisnpc->stance = 1;
    thisMonster->SetStats(false);
    thisMonster->Position->source = caller->Position->current;
    thisMonster->MoveTo(caller->Position->current);
    //	thisMonster->CalculateStats();
    //	thisMonster->MoveTo(x, y);

	return AI_SUCCESS;
}

//Do Trigger
AIACT(030)
{
	//word lenszTrigger;	//Pos: 0x00
	//char* szTrigger;	//Pos: 0x02
	GETAIACTDATA(030);
	char* tempName = reinterpret_cast<char*>(&data->szTrigger) - 2;
	dword hash = MakeStrHash(tempName);
	//Log(MSG_INFO, "Execute Quest Trigger %s[%d] [%08x]", tempName, data->lenszTrigger, hash);

    //CCharacter test...
    //LMA: Sometimes the QSD changes the refNPC for its own purpose, we have to take it back to the previous value.
    CMonster* monster = reinterpret_cast<CMonster*>(entity);
    UINT savelma = monster->thisnpc->refNPC;
    int is_ok = entity->ExecuteQuestTrigger(hash);

    /*
    if (savelma!=monster->thisnpc->refNPC)
    {
        Log(MSG_WARNING,"AIACT(030) RefNPC has changed, we swap it back from %u to %u",monster->thisnpc->refNPC,savelma);
    }
    */

    monster->thisnpc->refNPC=savelma;

    if(is_ok != QUEST_SUCCESS)
    {
        //return AI_FAILURE;		//PY never return AI_FAILURE from aiactions
		return AI_SUCCESS;
    }
    else
    {
        return AI_SUCCESS;
    }

}

//set monster's attack target to the same as the owner
AIACT(031)
{
    LogDebug( "AIACT(031)");
	//Attack my "CALLER"(s target) if he is not an ally (must be caller's target...)
    //	if(entity->_EntityType != ENTITY_MONSTER) return AI_FAILURE;
    //	CMonster* thisMonster = reinterpret_cast<CMonster*>(entity);

    //	CWorldEntity* caller = thisMonster->thisZone->GetEntity(thisMonster->_CallerID);
    //	if(caller == NULL) return AI_FAILURE;

    //	CWorldEntity* target = caller->thisZone->GetEntity(caller->_TargetID);
    //	if(target == NULL) return AI_FAILURE;

    //	thisMonster->Attack(target);
    if(!entity->IsMonster( )) return AI_SUCCESS;
	CMonster* thisMonster = reinterpret_cast<CMonster*>(entity);
	CMap* map = GServer->MapList.Index[thisMonster->Position->Map];
	CCharacter* caller = map->GetCharInMap( thisMonster->owner );
	if(caller == NULL) return AI_SUCCESS;
	thisMonster->Position->source = caller->Position->current;
	CCharacter* target = map->GetCharInMap(caller->Battle->target);
	if(target == NULL) return AI_SUCCESS;
	thisMonster->StartAction( (CCharacter*) target, NORMAL_ATTACK, 0 );
    //LogDebug( "AIACT(031 END");

	return AI_SUCCESS;
}

//Zone (1)
AIACT(032)
{
	//word nZoneNo;	//Pos: 0x00
	//byte btOnOff;	//Pos: 0x02
	//Set PK Flag (btOnOff) in nZoneNo

	//LMA: Used ofr Union Wars, mainly.
	GETAIACTDATA(032);
	BEGINPACKET(pak, 0x70f);
    ADDDWORD(pak, data->btOnOff);
    GServer->SendToAllInMap(&pak,data->nZoneNo);

	LogDebug( "AIACT(032) zone (%i) set to %i",data->nZoneNo,data->btOnOff);
	return AI_SUCCESS;
}

//Zone (2)
AIACT(033)
{
	//word nZoneNo //if 0, current map
	//word nNewValue
	//if nNewValue == 2 -> toggle
	//else set regen system nNewValue (0 = off, 1 = on)
	GETAIACTDATA(033);
	LogDebug( "AIACT(033) zone (2)");
	return AI_SUCCESS;
}

//Gives item to caller (credits PY).
AIACT(034)
{
	//word nItemNum;	//Pos: 0x00
	//word nCount;	//Pos: 0x02
	//Give item to "CALLER"
	GETAIACTDATA(034);
	if(!entity->IsMonster( )) return AI_SUCCESS;
	CMonster* thisMonster = reinterpret_cast<CMonster*>(entity);
	CMap* map = GServer->MapList.Index[thisMonster->Position->Map];
	CPlayer* caller = map->GetPlayerInMap( thisMonster->owner);
	if(caller == NULL) return AI_SUCCESS;
	int nItem = data->nItemNum;
	if (nItem == 0)return AI_SUCCESS;
	char itemtypes[15] = {4,0,0,0,0,0,0,0,0,0,1,2,2,4,3};
    CItem thisitem;
	thisitem.Clear();

	//LMA: exact way.
	//thisitem.itemtype = nItem / 1000;
    //thisitem.itemnum = nItem % 1000;
	thisitem.itemtype = GServer->gi(nItem,0);
    thisitem.itemnum = GServer->gi(nItem,1);

    thisitem.count = data->nCount;
    if( itemtypes[thisitem.itemtype] == 0 )
    {
		thisitem.appraised = true;
		thisitem.lifespan = 100;
		thisitem.gem = 0;
		thisitem.durability = GServer->STB_ITEM[thisitem.itemtype-1].rows[thisitem.itemnum][29];
	}

	if (thisitem.durability==0)
	{
        thisitem.durability=100;
	}

    int thisslot = caller->AddItem( thisitem );
    LogDebug("Item type = %i item ID = %i Count = %i Slot = %i",thisitem.itemtype,thisitem.itemnum,thisitem.count,thisslot);

    //Failure.
    if (thisslot == 0xffff)
	{
         BEGINPACKET( pak, 0x7a7);
         ADDWORD(pak, 0x00);
         ADDBYTE(pak, 0x03);
         ADDBYTE(pak, 0x00);
         caller->client->SendPacket(&pak);
         return AI_SUCCESS;
    }

    //success.
    BEGINPACKET( pak, 0x71f);
    ADDBYTE(pak, 0x01);
    ADDBYTE(pak, thisslot);
	pak = GServer->AddItemData(caller->items[thisslot], pak);
    //ADDDWORD(pak, GServer->BuildItemHead ( caller->items[thisslot]) );
    //ADDDWORD(pak, GServer->BuildItemData ( caller->items[thisslot]) );
    //ADDDWORD( pak, 0x00000000 );
    //ADDWORD ( pak, 0x0000 );
    caller->client->SendPacket(&pak);


	return AI_SUCCESS;
}

//Set Variable (4)
AIACT(035)
{
	//Set AiVAR
	GETAIACTDATA(035);
	CMonster* thisMonster = reinterpret_cast<CMonster*>(entity);
	if(data->nVarIDX >19)return AI_SUCCESS;
    int tempval = thisMonster->AIVar[data->nVarIDX];
    switch(data->btOp)
    {
        case 6:
            tempval += data->iValue;
            break;
        case 7:
            tempval -= data->iValue;
            if(tempval <= 0)
            {
                thisMonster->AIVar[data->nVarIDX] = 0;
                return AI_SUCCESS;
            }
            break;
        case 9:
            tempval++;
            break;
        default:
            return AI_SUCCESS;
            break;
    }
    thisMonster->AIVar[data->nVarIDX] = tempval;
    Log(MSG_INFO,"AI[%i] set to %i for monster %i",data->nVarIDX,tempval,thisMonster->montype);
	//thisMonster->thisnpc->Objvar->SetVar(data->btOp, data->iValue); //code not working
	return AI_SUCCESS;
}

//Monster (1)
AIACT(036)
{
	//word nMonster;	//Pos: 0x00
	//byte btMaster;	//Pos: 0x02
	//Spawn nMonster at my position (has btMaster)?
	GETAIACTDATA(036);
    //	CWorldEntity* master = NULL;
    //	if(data->btMaster) master = entity;
    UINT owner=0;
	CMonster* monster = reinterpret_cast<CMonster*>(entity); //need this to get the spawnid of the entity
	if(monster == NULL) return AI_SUCCESS;

    if(data->btMaster==1)
    {
        owner=monster->clientid;
    }

	CMap* map = GServer->MapList.Index[entity->Position->Map];
    CMonster* thismonster = map->AddMonster( data->nMonster, entity->Position->current, owner, monster->Status->spawnid );

    //LMA: master is actually a monster, not a player.
    //So even if he has a master, he's just a monster ;)
    if(data->btMaster == 1 && thismonster != NULL)
    {
        thismonster->CharType = TMONSTER;
        thismonster->ForceMaxHP();  //LMA: Forcing the HP amount to the "monster" one.
    }
	return AI_SUCCESS;
}

//Monster (2)
AIACT(037)
{
	//word nMonster;	//Pos: 0x00
	//word nPos;	//Pos: 0x02
	//dword iDistance;	//Pos: 0x04
	//byte btMaster;	//Pos: 0x08

	//Spawn nMonster at nPos within iDistance (has btMaster)?
	//nPos = 0 my pos
	//nPos = 1 m_pDestCHAR pos
	//nPos = 2 target pos
    GETAIACTDATA(037);

    UINT owner=0;
    CMap* map = GServer->MapList.Index[entity->Position->Map];
	CMonster* monster = reinterpret_cast<CMonster*>(entity); //need this to get the spawnid of the entity
	if(monster == NULL) return AI_FAILURE;

    if(data->btMaster==1)
    {
        owner=monster->clientid;
    }

	fPoint position;
	CMonster* thismonster=NULL;

	if(data->nPos == 0) //spawn monster in a circle with radius iDistance around my current position
    {
        position = GServer->RandInCircle( entity->Position->current, data->iDistance );
        thismonster=map->AddMonster( data->nMonster, position, owner, monster->Status->spawnid );
    }
    else if(data->nPos == 1) //spawn monster in a circle with radius iDistance around my destiny position
    {
        position = GServer->RandInCircle( entity->Position->destiny, data->iDistance );
        thismonster=map->AddMonster( data->nMonster, position, owner, monster->Status->spawnid );
		//dest char
	}
    else if(data->nPos == 2) //spawn monster in a circle with radius iDistance around my target's current position
    {
        CCharacter* target = entity->GetCharTarget( );
        if(target == NULL)
        {
            return AI_FAILURE;
        }

        position = GServer->RandInCircle( target->Position->current, data->iDistance );
        thismonster=map->AddMonster( data->nMonster, position, owner, monster->Status->spawnid );
    }

    //LMA: master is actually a monster, not a player.
    //So even if he has a master, he's just a monster ;)
    if(data->btMaster==1&&thismonster!=NULL)
    {
        thismonster->CharType = TMONSTER;
        thismonster->ForceMaxHP();  //LMA: Forcing the HP amount to the "monster" one.
    }


	return AI_SUCCESS;
}


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

//extern CRandomMersenne rg;

int AI_GetAbility(CCharacter* entity, byte btAbType)
{
	switch(btAbType)
    {
		case 0:
			return entity->Stats->Level;
		break;
		case 1:
			return entity->Stats->Attack_Power;
		break;
		case 2:
			return entity->Stats->Defense;
		break;
		case 3:
			return entity->Stats->Magic_Defense;
		break;
		case 4:
			return entity->Stats->HP;
		break;
		case 5:
             return 0;
		    //Log(MSG_WARNING, "Don't have monsters charm yet...");
		    //return -1;
		break;
		default:
			//Log(MSG_ERROR, "Unknown cAbType [%d]", btAbType);
			return -1;
		break;
	}
}

//Hold or Attack
AICOND(000)
{
	//Always False
	//Log(MSG_DEBUG, "AICOND(000) called");
	return AI_FAILURE;
}

//(Damage > iDamage) == true iDamage cRecvOrGive
AICOND(001)
{
    //Log(MSG_DEBUG, "AICOND(001) called");
    //dword iDamage               //Pos: 0x00
	//byte cRecvOrGive	          //Pos: 0x04
    GETAICONDDATA(001);
    //Log(MSG_DEBUG, "AIcond(001) %i %i %i",data->iDamage,data->cRecvOrGive,entity->damagecounter);
    if(data->cRecvOrGive == 0)
    {
		if(entity->damagecounter > data->iDamage)
        {
            entity->damagecounter = 0;
            //Log(MSG_DEBUG, "AIcond(001) Success ");
            return AI_SUCCESS;
        }
	}
    else
    {
		//PY this is wrong. if data->cRecvOrGive is 1 then it should return FALSE
		/*if(entity->damagecounter < data->iDamage)
        {
            entity->damagecounter = 0;
            //Log(MSG_DEBUG, "AIcond(001) Success ");
            return AI_SUCCESS;
        }*/
        return AI_FAILURE;
	}
    return AI_FAILURE;

}

//Check Near (1)
AICOND(002)
{
	/*
	dword iDistance;	//Pos: 0x00
	byte btIsAllied;	//Pos: 0x04
	word nLevelDiff;	//Pos: 0x06
	word nLevelDiff2;	//Pos: 0x08
	word wChrNum;	//Pos: 0x0a
	*/
	//count how many aiobj between nLevelDiff and nLevelDiff2 who is btIsAllied within iDistance
	//must be >= wChrNum

	////Log(MSG_DEBUG, "AICOND(002) called");
    GETAICONDDATA(002);
	dword chrCount = 0;
	dword eCount = 0;
	int nearestDistance = 9999999;
	int searchDistance = data->iDistance;
	CMap* map = GServer->MapList.Index[entity->Position->Map];
    ////Log(MSG_DEBUG, "Check Near (1)iDistance %i btIsAllied %i nLevelDiff %i nLevelDiff2 %i wChrNum %i", data->iDistance, data->btIsAllied, data->nLevelDiff, data->nLevelDiff2, data->wChrNum);
    if(data->btIsAllied == 1)
    {
    	dword entityCount = map->MonsterList.size();
        for(UINT j=0;j<map->MonsterList.size();j++)
        {
            CMonster* other = map->MonsterList.at(j);
            if(eCount >= entityCount) break;
            if(other == NULL) continue;
            eCount++;
            if(other == entity) continue;

    		int levelDiff = (entity->Stats->Level - other->Stats->Level);
    		if(levelDiff < data->nLevelDiff) continue;
    		if(levelDiff > data->nLevelDiff2) continue;
            int iDistance = (int) GServer->distance( other->Position->current, entity->Position->current );
    		if(iDistance > searchDistance) continue;

    		chrCount++;
    		if(iDistance < nearestDistance)
            {
    			nearestDistance = iDistance;
    			entity->nearChar = other;
    		}
    		if(chrCount >= data->wChrNum)
            {
    			entity->findChar = other;
    			return AI_SUCCESS;
    		}
    	}
    }
    else
    {
    	dword entityCount = map->PlayerList.size();
        ////Log(MSG_DEBUG, "entityCount %i", entityCount);
        for(UINT j=0;j<map->PlayerList.size();j++)
        {
            CPlayer* other = map->PlayerList.at(j);
            ////Log(MSG_DEBUG, "player %i", other->clientid);
            if(eCount >= entityCount) break;
            ////Log(MSG_INFO, "1");
            if(other == NULL) continue;
            ////Log(MSG_INFO, "2");
            eCount++;

    		int levelDiff = (entity->Stats->Level - other->Stats->Level);
            ////Log(MSG_DEBUG, "levelDiff %i < %i", levelDiff,data->nLevelDiff);
    		if(levelDiff < data->nLevelDiff) continue;
            ////Log(MSG_INFO, "3");
            ////Log(MSG_DEBUG, "levelDiff %i > %i", levelDiff,data->nLevelDiff2);
    		if(levelDiff > data->nLevelDiff2) continue;
            ////Log(MSG_INFO, "4");
            int iDistance = (int) GServer->distance( other->Position->current, entity->Position->current );
    		if(iDistance > searchDistance) continue;
            ////Log(MSG_INFO, "5");

    		chrCount++;
    		if(iDistance < nearestDistance)
            {
    			nearestDistance = iDistance;
    			entity->nearChar = other;
                ////Log(MSG_INFO, "6");
    		}
    		if(chrCount >= data->wChrNum)
            {
    			entity->findChar = other;
                ////Log(MSG_INFO, "7");
    			return AI_SUCCESS;
    		}
    	}
    }
	return AI_FAILURE;
}

//Check Distance (1)
AICOND(003)
{
	//dword iDistance;	//Pos: 0x00
	//CObjCHAR::Get_MoveDISTANCE > iDistance == true
	//distance from spawn point?
	//Log(MSG_DEBUG, "AICOND(003) called");
    GETAICONDDATA(003);

	entity->UpdatePosition(false);
    int moveDistance = (int) GServer->distance( entity->Position->current, entity->Position->source );
	if(moveDistance > (data->iDistance))
    {
		return AI_SUCCESS;
	}
	return AI_FAILURE;
}

//Check Distance (2)
AICOND(004)
{
	//dword iDistance;	//Pos: 0x00
	//byte cMoreLess;	//Pos: 0x04
	//distance to target (cMoreLess == 0 then >= else <=) iDistance
	//Log(MSG_DEBUG, "AICOND(004) called");
    GETAICONDDATA(004);
	//Log(MSG_DEBUG, "AICOND(004) retrieved data");
	CCharacter* target = entity->GetCharTarget( );
    //CWorldEntity* target = entity->thisZone->GetEntity(entity->_TargetID);
	if(target == NULL)
    {
        //Log(MSG_DEBUG, "AICOND(004) found no valid target");
        return AI_FAILURE;
    }
	target->UpdatePosition(false);
	entity->UpdatePosition(false);
	int distance = (int) GServer->distance( entity->Position->current, target->Position->current );
    //int distance = entity->basic.pos.distance(target->basic.pos);
	if(data->cMoreLess == 0)
    {
		if(distance >= data->iDistance)
        {
            //Log(MSG_DEBUG, "Check Distance (2) This distance: %i > %i", distance, data->iDistance);
            return AI_SUCCESS;
        }
	}
    else
    {
		if(distance <= data->iDistance)
        {
            //Log(MSG_DEBUG, "Check Distance (2) This distance: %i < %i", distance, data->iDistance);
            return AI_SUCCESS;
        }
	}
	return AI_FAILURE;
}

//Check Ability of my Target
AICOND(005)
{
	/*
	byte cAbType;	//Pos: 0x00
	dword iDiff;	//Pos: 0x04
	byte cMoreLess;	//Pos: 0x08
	*/
	//Log(MSG_DEBUG, "AICOND(005) called");
    GETAICONDDATA(005);
    CCharacter* target = entity->GetCharTarget( );
	if(target == NULL)
    {
        //Log(MSG_DEBUG, "AICOND(004) found no valid target");
        return AI_FAILURE;
    }
	//cAbType
	//0 = Level
	//1 = Attack
	//2 = Defense
	//3 = "Res"? Magic Resistance?
	//4 = HP
	//5 = Charm

	//Check if "ability" (cMoreLess == 0 then >= else <=) iDiff
	//Log(MSG_DEBUG, "Check AB (1)");
	int value = AI_GetAbility(target, data->cAbType); //PY This should be checking my target's ability NOT MY OWN. Corrected now
    //Log(MSG_DEBUG, "Check AB (1) type %i diff %i", data->cAbType, data->iDiff);
	if(value < 0) return AI_FAILURE;
	if(data->cMoreLess == 0)
    {
		if(value > data->iDiff)
        {
            //Log(MSG_DEBUG, "Check AB (1) > %i", data->iDiff);
            return AI_SUCCESS;
        }
	}
    else
    {
		if(value < data->iDiff)
        {
            //Log(MSG_DEBUG, "Check AB (1) < %i", data->iDiff);
            return AI_SUCCESS;
        }
	}
	return AI_FAILURE;
}

//Check HP
AICOND(006)
{
	//dword wHP;	//Pos: 0x00
	//byte cMoreLess;	//Pos: 0x04
	//Check if hp% (cMoreLess == 0 then >= else <=) wHP
	//Log(MSG_DEBUG, "AICOND(000) called");
    GETAICONDDATA(006);
	dword value = (dword)(((float)entity->Stats->HP/ (float)entity->Stats->MaxHP) * 100.0f);
    //Log(MSG_DEBUG, "Check HP %i %i", data->cMoreLess, value);
	if(data->cMoreLess == 0)
    {
		if(value >= data->wHP) return AI_SUCCESS;
	}
    else
    {
		if(value <= data->wHP) return AI_SUCCESS;
	}
	return AI_FAILURE;
}

//Random Chance
AICOND(007)
{
	//byte cPercent;	//Pos: 0x00
	//Random number 0->100 if random number < cPercent then return true
	//Log(MSG_DEBUG, "AICOND(007) called");
    GETAICONDDATA(007);
    //byte rand = rg.IRandom(0, 100) & 0xFF;
    //srand(time(NULL));
    byte brand = rand()%100;
    //Log(MSG_DEBUG, "brand =  %i chance = %i", brand, data->cPercent);
	if(brand <= data->cPercent) return AI_SUCCESS;
	return AI_FAILURE;
}

//Check Near (2)
AICOND(008)
{
	//dword iDistance;	//Pos: 0x00
	//word nLevelDiff;	//Pos: 0x04
	//word nLevelDiff2;	//Pos: 0x06
	//byte btIsAllied;	//Pos: 0x08

	//find atleast 1 aiobj between nLevelDiff and nLevelDiff2 who is btIsAllied within iDistance

	//Log(MSG_DEBUG, "AICOND(008) called");
    GETAICONDDATA(008);

	dword eCount = 0;
	int searchDistance = data->iDistance;// * 10;
	CMap* map = GServer->MapList.Index[entity->Position->Map];


	if(data->btIsAllied == 1)
    {
        dword entityCount = map->MonsterList.size();
        for(UINT j=0;j<map->MonsterList.size();j++)
        {
            CMonster* other = map->MonsterList.at(j);

            if(eCount >= entityCount) break;
            if(other == NULL) continue;
            eCount++;
            if(other == entity) continue;
            if(other->CharType != TMONSTER) continue;

            int levelDiff = abs(entity->Stats->Level - other->Stats->Level);
            if(levelDiff < data->nLevelDiff) continue;
            if(levelDiff > data->nLevelDiff2) continue;

            int iDistance = (int)GServer->distance( other->Position->current, entity->Position->current );
            if(iDistance > searchDistance) continue;

            entity->findChar = other;
            entity->nearChar = other;
            return AI_SUCCESS;
        }
    }
    else
    {
        dword entityCount = map->PlayerList.size();
        for(UINT j=0;j<map->PlayerList.size();j++)
        {
            CPlayer* other = map->PlayerList.at(j);

            if(eCount >= entityCount) break;
            if(other == NULL) continue;
            eCount++;
            if(other == entity) continue;
            if(other->CharType != TPLAYER) continue;

            int levelDiff = abs(entity->Stats->Level - other->Stats->Level);
            if(levelDiff < data->nLevelDiff) continue;
            if(levelDiff > data->nLevelDiff2) continue;

            int iDistance = (int)GServer->distance( other->Position->current, entity->Position->current );
            if(iDistance > searchDistance) continue;

            entity->findChar = other;
            entity->nearChar = other;
            return AI_SUCCESS;
        }
    }
	return AI_FAILURE;
}

//check target
//PY This is actually incorrect. Should be comparing my target with my destination character. TRUE if they are different
//To be corrected!!!
AICOND(009)
{
	//Do i have a target?
    //CWorldEntity* target = entity->thisZone->GetEntity(entity->_TargetID);
    //if(target == NULL){entity->_TargetID = 0; return AI_FAILURE;}
    //return AI_SUCCESS;

    //TO BE RECODED
    CCharacter* target = entity->GetCharTarget( );
    if(target == NULL) return AI_FAILURE;
    return AI_SUCCESS;
}

//Check AB (2)
AICOND(010)
{
	//byte cAbType;	//Pos: 0x00
	//byte cMoreLess;	//Pos: 0x01

	//Is my ability >= or <= than my targets
	//PY This is wrong too. Should be comparing ability of my target against the one that just hit me
	//NOT against MY ability

	//TO BE RECODED
	GETAICONDDATA(010);
    //CWorldEntity* target = entity->thisZone->GetEntity(entity->_TargetID);
    CCharacter* target = entity->GetCharTarget( );
	if(target == NULL) return AI_FAILURE;
    //Log(MSG_DEBUG, "Check AB (2) type %i ", data->cAbType);
	int myValue = AI_GetAbility(entity, data->cAbType);
	if(myValue < 0) return AI_FAILURE;

	int targetValue = AI_GetAbility(target, data->cAbType);
	if(targetValue < 0) return AI_FAILURE;

	if(data->cMoreLess == 0)
    {
		if(myValue > targetValue) return AI_SUCCESS;
	}
    else
    {
		if(myValue < targetValue) return AI_SUCCESS;
	}
	return AI_FAILURE;
}

//Check AB (3)
//PY Again this has to be my Attacker's ability. NOT my own or even my Target's
AICOND(011)
{
	//byte cAbType;	//Pos: 0x00
	//dword iValue;	//Pos: 0x04
	//byte cMoreLess;	//Pos: 0x08
	//check ability of "m_pDestCHAR" <= or >= iValue
	GETAICONDDATA(011);
	dword tValue = data->iValue;
	//if(tValue == 0)tValue = AI_GetAbility(entity, data->cAbType);
	CCharacter* target = entity->GetCharTarget( );
	if(target == NULL)
    {
        //Log(MSG_DEBUG, "AICOND(004) found no valid target");
        return AI_FAILURE;
    }
	int myValue = AI_GetAbility(target, data->cAbType);
	//CCharacter* target = entity->GetCharTarget( );
	//if(target == NULL)
    //{
    //    Log(MSG_DEBUG,"AICOND(011) could not find a valid target");
    //    return AI_FAILURE;
    //}
	//myValue = AI_GetAbility(target, data->cAbType);
    //Log(MSG_DEBUG, "AICOND(011) myValue: %i <> iValue: %i moreless: %i type: %i", myValue, data->iValue, data->cMoreLess, data->cAbType);
	if(data->cMoreLess == 0) //0 = more
    {
        if(myValue >= data->iValue)
        {
            //Log(MSG_DEBUG, "AICOND(011) myValue: %i <= tValue: %i ",myValue,tValue);
            return AI_SUCCESS;
        }
	}
    else // 1 = less
    {
        if(myValue <= data->iValue)
        {
            //Log(MSG_DEBUG, "AICOND(011) myValue: %i <= tValue: %i ",myValue,tValue);
            return AI_SUCCESS;
        }
	}
	return AI_FAILURE;
}

//Check Time (1)
AICOND(012)
{
	//byte cWhen;	//Pos: 0x00
	//Is it night or day? cWhen: ?1 = night 0 = day?
	GETAICONDDATA(012);
	CMap* map = GServer->MapList.Index[entity->Position->Map];
	if (data->cWhen == 1)
    {
        if (map->CurrentTime > 2) // night
            return AI_SUCCESS;
    }
    else
    {
        if (map->CurrentTime < 3) // day
            return AI_SUCCESS;
    }
    return AI_FAILURE;
}

//Check Target for buffs(1)
AICOND(013)
{
	//byte btCheckTarget;	//Pos: 0x00 //0 = self 1 = target
	//byte btStatusType;	//Pos: 0x01 //
	//byte btHave;	//Pos: 0x02

	//Does my target btHave MagicSTATUS (btStatusType)
	//btCheckTarget does Get_MagicSTATUS but uses differ AND masks
	//btCheckTarget = 0 AND EAX,1EAAA84
	//btCheckTarget = 1 AND EAX,1E155570
	//btCheckTarget = 2 normal
	CCharacter* target = NULL;
	UINT buffs=0;
	GETAICONDDATA(013);
	if(data->btCheckTarget == 1) //check enemy
	{
       target = entity->GetCharTarget( );
       if(target == NULL)return AI_FAILURE;
    }
    else if(data->btCheckTarget == 0) //check self
    {
         target = entity;
    }
    else  //should never be anything other than 0 or 1
    {
        //Log(MSG_DEBUG, "AICOND(013) Check Target (1) btCheckTarget %i ", data->btCheckTarget);
        return AI_FAILURE;
    }
    if (target == NULL) //target not found
    {
       //Log(MSG_DEBUG, "AICOND(013) Check Target (1) not found btCheckTarget %i ", data->btCheckTarget);
       return AI_FAILURE;
    }

    switch(data->btStatusType)
    {
        case 0: //debuff
            buffs = GServer->BuildDeBuffs(target);
        break;
        case 1: //buff
            buffs = GServer->BuildUpBuffs(target);
        break;
        case 2: //effect such as burning
            //not sure how to handle this so i will come back to it. PY
        break;
    }
    //Log(MSG_DEBUG, "Target: %i btStutusType: %i buffs: %i btHave: %i", target->clientid, data->btStatusType, buffs, data->btHave);
    if (data->btHave == 1) //True if HAS buffs
    {
       if(buffs != 0)return AI_SUCCESS;
       return AI_FAILURE;
    }
    else // true if NOT HAS buffs
    {
       if(buffs != 0)return AI_FAILURE;
       return AI_SUCCESS;
    }
	//CCharacter* target = entity->GetCharTarget( );
	//Log(MSG_DEBUG, "Check Target (1) shouldn't be able to get this far");
	return AI_SUCCESS;
	//return AI_FAILURE;
}

//Check Variable (1) (ObjVar)
AICOND(014)
{
	GETAICONDDATA(014);
	//Log(MSG_DEBUG,"Checking Object Variable using NPC ref number AICOND(015)");
	//Log(MSG_DEBUG,"btVarIDX = %i iValue = %i btOp = %i",data->btVarIDX, data->iValue,data->btOp);
	CMonster* monster = reinterpret_cast<CMonster*>(entity);
	if(monster == NULL) return AI_FAILURE;
	if(data->btVarIDX > 19) return AI_FAILURE;
	int refNPC = monster->thisnpc->refNPC;
	int ObjvarIndex = data->btVarIDX;

	int tempval = GServer->ObjVar[refNPC][ObjvarIndex];
	//Log(MSG_DEBUG,"ObjVar value [%i][%i] = %i", refNPC,ObjvarIndex,tempval);
	switch(data->btOp)
	{
        case 0:
             if(tempval == data->iValue)
                 return AI_SUCCESS;
             break;
        case 1:
             if(tempval > data->iValue)
                 return AI_SUCCESS;
             break;
        case 2:
             if(tempval >= data->iValue)
                 return AI_SUCCESS;
             break;
        case 3:
             if(tempval < data->iValue)
                 return AI_SUCCESS;
             break;
        case 4:
             if(tempval <= data->iValue)
                 return AI_SUCCESS;
             break;
        case 10:
             if(tempval != data->iValue)
                 return AI_SUCCESS;
             break;
        default:
             return AI_FAILURE;
             break;
    }


    //if(entity->_EntityType != ENTITY_NPC) return AI_FAILURE;
    //CNpc* thisNpc = reinterpret_cast<CNpc*>(entity);
    //thisNpc = thisNpc->SelectedNpc;
    //if(thisNpc == NULL) return AI_FAILURE;
    //if(OperateValues<short>(data->btOp, &VarValue, data->iValue & 0xFFFF)) return AI_SUCCESS;
	return AI_FAILURE;
}

//Check Variable (2) (WorldVAR) //PY should work just like NPC ObjVar.  To Be Coded later
AICOND(015)
{
	//word nVarIDX;	//Pos: 0x00
	//byte btOp;	//Pos: 0x08
	//dword iValue;	//Pos: 0x04

	//Check WorldVAR
	return AI_FAILURE;
}

//Check Variable (3) (EconomyVar)
AICOND(016)
{
	//word nVarIDX;	//Pos: 0x00
	//dword iValue;	//Pos: 0x04
	//byte btOp;	//Pos: 0x08

	//Check EconomyVar
	return AI_FAILURE;
}

//Select ObjVar reference
AICOND(017)
{
	GETAICONDDATA(017);
	//Log(MSG_DEBUG,"Setting NPC ref number AICOND(017)");
	CMonster* monster = reinterpret_cast<CMonster*>(entity);
	if(monster == NULL) return AI_FAILURE;
	monster->thisnpc->refNPC = data->iNpcNo; // sets the reference variable for the correct ObjVar
    //Log(MSG_DEBUG,"Set NPC ref number %i successfully", monster->thisnpc->refNPC);
	return AI_SUCCESS;
}

//Check Distance (3)
AICOND(018)
{
	//dword iDistance;	//Pos: 0x00
	//byte btOp;	//Pos: 0x04

	//Distance to "CALLER" ??Possibly the one who summoned the monster??
	GETAICONDDATA(018);
		if(!entity->IsMonster( )) return AI_FAILURE;
    //Log(MSG_DEBUG, "AICOND(018 1)");
	CMonster* thisMonster = reinterpret_cast<CMonster*>(entity);
	CMap* map = GServer->MapList.Index[thisMonster->Position->Map];
	CCharacter* caller = map->GetCharInMap( thisMonster->owner );
	if(caller == NULL ) return AI_FAILURE; // I think this is wrong. It should return true if there is no owner. Otherwise orphaned bonfires will live forever
    if(thisMonster->owner == 0) return AI_FAILURE; //Monster has no owner
    //Log(MSG_DEBUG, "AICOND(018 2)");
    //if(entity->_EntityType != ENTITY_MONSTER) return AI_FAILURE;
    //CMonster* thisMonster = reinterpret_cast<CMonster*>(entity);
    //CWorldEntity* caller = thisMonster->thisZone->GetEntity(thisMonster->_CallerID);
    //if(caller == NULL) return AI_FAILURE;
	caller->UpdatePosition(false);
	thisMonster->UpdatePosition(thisMonster->stay_still);
	int distance = (int)GServer->distance( caller->Position->current, thisMonster->Position->current );
    //int distance = thisMonster->basic.pos.distance(caller->basic.pos);
    //Log(MSG_DEBUG, "AICOND(018 3)");
	if(OperateValues<int>(data->btOp, &distance, data->iDistance)) return AI_SUCCESS;
    //Log(MSG_DEBUG, "AICOND(018 4)");
	return AI_FAILURE;
}

//Check Time (2)
AICOND(019)
{
	//dword ulTime;	//Pos: 0x00
	//dword ulEndTime;	//Pos: 0x04

	//Get_ZoneTIME between ulTime and ulEndTime
	GETAICONDDATA(019);
	CMonster* thisMonster = reinterpret_cast<CMonster*>(entity);
	CMap* map = GServer->MapList.Index[thisMonster->Position->Map];
	int StartTime = int(data->ulTime);
	int EndTime = int(data->ulEndTime);
	if(map->ZoneTime >= StartTime && map->ZoneTime <= EndTime)
        return AI_SUCCESS;
	return AI_FAILURE;
}

//Check My AB (4) NOT target's
AICOND(020)
{
	//byte btAbType;	//Pos: 0x00
	//dword iValue;	    //Pos: 0x04
	//byte btOp;	    //Pos: 0x08
	

	//cAbType
	//0 = Level
	//1 = Attack
	//2 = Defense
	//3 = "Res"? Magic Resistance?
	//4 = HP
	//5 = Charm
	GETAICONDDATA(020);
    //Log(MSG_DEBUG, "Check AB (4) type %i iValue %i", data->btAbType, data->iValue);
	int value = AI_GetAbility(entity, data->btAbType);
	if(value < 0) return AI_FAILURE;
	if(OperateValues<int>(data->btOp, &value, data->iValue)) return AI_SUCCESS;
    //Log(MSG_INFO,"check ab (4) successfull");
	return AI_FAILURE;
}

//Do I have a caller
//PY. Needs to return AI_SUCCESS if I have no owner
AICOND(021)
{
	//Do i have a "CALLER" ??Possibly the one who summoned the monster??
	// more like Am I an orphan?
	GETAICONDDATA(021);

	if(!entity->IsMonster())return AI_SUCCESS;
    CMonster* thisMonster = reinterpret_cast<CMonster*>(entity);
	if(thisMonster->owner == 0) return AI_SUCCESS;
    //Log(MSG_DEBUG, "AICOND(021) Yes I Have no owner");
	return AI_FAILURE;
}

//Does my caller have a target
AICOND(022)
{
	//Does my "CALLER" have a target?
    //Log(MSG_DEBUG, "AICOND(022) Does my `CALLER` have a target?");
	GETAICONDDATA(022);

	if(!entity->IsMonster( )) return AI_FAILURE;
    //Log(MSG_DEBUG, "AICOND(022 1)");
	CMonster* thisMonster = reinterpret_cast<CMonster*>(entity);
	CMap* map = GServer->MapList.Index[thisMonster->Position->Map];
	CCharacter* caller = map->GetCharInMap( thisMonster->owner );
	if(caller == NULL) return AI_FAILURE;
    //Log(MSG_DEBUG, "AICOND(022 2)");
	CCharacter* target = map->GetCharInMap(caller->Battle->target);
	if(target == entity) return AI_FAILURE;
	if(target != NULL) return AI_SUCCESS;
    //Log(MSG_DEBUG, "AICOND(022 3)");
	return AI_FAILURE;
}

//Check Time (3) (World time?)
AICOND(023)
{
	//dword ulTime;	//Pos: 0x00
	//dword ulEndTime;	//Pos: 0x04

	//Get_WorldTIME between ulTime and ulEndTime
	return AI_FAILURE;
}

//Check Date Time (4)
AICOND(024)
{
    //Log(MSG_INFO,"AIP Cdt 024 for aipId %i",AipId);
	GETAICONDDATA(024);
    //byte btDate      Pos: 0x00
	//byte btHour1     Pos: 0x01
	//byte btMin1      Pos: 0x02
	//byte btHour2     Pos: 0x03
	//byte btMin2      Pos: 0x04
    SYSTEMTIME sTIME;
    GetLocalTime(&sTIME);

    if(data->btDate != 0)
    {
    	if(sTIME.wDay != data->btDate)
    		return AI_FAILURE;
    }
    word wMin = ((sTIME.wHour * 60) + sTIME.wMinute);
    word wFrom = (data->btHour1 * 60) + data->btMin1;
    word wTo = (data->btHour2 * 60) + data->btMin2;
    if(wMin >= wFrom && wMin <= wTo)
    	return AI_SUCCESS;
	return AI_FAILURE;
}

//Check Weekday Time (5)
AICOND(025)
{
	GETAICONDDATA(025);
	//byte btWweekDay      Pos: 0x00
	//byte btHour1     Pos: 0x01
	//byte btMin1      Pos: 0x02
	//byte btHour2     Pos: 0x03
	//byte btMin2      Pos: 0x04
    SYSTEMTIME sTIME;
    GetLocalTime(&sTIME);

    if(data->btWeekDay != sTIME.wDayOfWeek)
        return AI_FAILURE;
    word wMin = ((sTIME.wHour * 60) + sTIME.wMinute);
    word wFrom = (data->btHour1 * 60) + data->btMin1;
    word wTo = (data->btHour2 * 60) + data->btMin2;
    if(wMin > wFrom && wMin < wTo)
        return AI_SUCCESS;
	return AI_FAILURE;
}

//Check Server/Channel
AICOND(026)
{
	GETAICONDDATA(026);
	//channel >= min && channel <= max

	//LMA: Success for now ;)
	return AI_SUCCESS;
	//return AI_FAILURE;
}

//Check Near Character
AICOND(027)
{
	//dword iDistance;	//Pos: 0x00
	//byte btIsAllied;	//Pos: 0x04
	//word nLevelDiff;	//Pos: 0x06
	//word nLevelDiff2;	//Pos: 0x08
	//word wChrNum;	//Pos: 0x0a
	//byte btOp;	//Pos: 0x0c

	//count how many aiobj between nLevelDiff and nLevelDiff2 who is btIsAllied within iDistance
	//must be (btOp) wChrNum
	GETAICONDDATA(027);

	short chrCount = 0;
	dword eCount = 0;
	int nearestDistance = 9999999;
	int searchDistance = data->iDistance;// * 10;

	CMap* map = GServer->MapList.Index[entity->Position->Map];
	dword entityCount = map->MonsterList.size();

    if(data->btIsAllied == 1)
    {
        for(UINT j=0;j<map->MonsterList.size();j++)
        {
            CMonster* other = map->MonsterList.at(j);
            if(eCount >= entityCount) break;
            if(other == NULL) continue;
            eCount++;
            if(other == entity) continue;
            int levelDiff = (entity->Stats->Level - other->Stats->Level);
    		if(levelDiff < data->nLevelDiff) continue;
    		if(levelDiff > data->nLevelDiff2) continue;
            int iDistance = (int)GServer->distance( other->Position->current, entity->Position->current );
            if(iDistance > searchDistance) continue;
    		chrCount++;
    		if(iDistance < nearestDistance)
    		{
    			nearestDistance = iDistance;
    			entity->nearChar = other;
    		}
    	}
    	entity->findChar = entity->nearChar;
    	if(OperateValues<short>(data->btOp, &chrCount, data->wChrNum)) return AI_SUCCESS;
    }
    else
    {
        for(UINT j=0;j<map->PlayerList.size();j++)
        {
            CPlayer* other = map->PlayerList.at(j);
            if(eCount >= entityCount) break;
            if(other == NULL) continue;
            eCount++;
            if(other == entity) continue;
            int levelDiff = (entity->Stats->Level - other->Stats->Level);
    		if(levelDiff < data->nLevelDiff) continue;
    		if(levelDiff > data->nLevelDiff2) continue;
            int iDistance = (int)GServer->distance( other->Position->current, entity->Position->current );
            if(iDistance > searchDistance) continue;
    		chrCount++;
    		if(iDistance < nearestDistance){
    			nearestDistance = iDistance;
    			entity->nearChar = other;
    		}
    	}
    	entity->findChar = entity->nearChar;
    	if(OperateValues<short>(data->btOp, &chrCount, data->wChrNum)) return AI_SUCCESS;
    }

    ////Log(MSG_INFO,"AICOND(027) failure");
	return AI_FAILURE;
}

//Check Variable (4)
AICOND(028)
{
	//word nVarIDX;	//Pos: 0x00
	//byte btOp;	//Pos: 0x08
	//dword iValue;	//Pos: 0x04

	//Get_AiVAR
	GETAICONDDATA(028);
	CMonster* thisMonster = reinterpret_cast<CMonster*>(entity);
	if(data->nVarIDX >19)return AI_FAILURE;
    UINT tempval = thisMonster->AIVar[data->nVarIDX];
	switch(data->btOp)
	{
        case 0:
             if(tempval == data->iValue)
                 return AI_SUCCESS;
             break;
        case 1:
             if(tempval > data->iValue)
                 return AI_SUCCESS;
             break;
        case 2:
             if(tempval >= data->iValue)
                 return AI_SUCCESS;
             break;
        case 3:
             if(tempval < data->iValue)
                 return AI_SUCCESS;
             break;
        case 4:
             if(tempval <= data->iValue)
                 return AI_SUCCESS;
             break;
        case 10:
             if(tempval != data->iValue)
                 return AI_SUCCESS;
             break;
        default:
             return AI_FAILURE;
             break;
    }
	return AI_FAILURE;
}

//Check Target (2)
AICOND(029)
{
	//byte btTargetType;	//Pos: 0x00
	//UNKNOWN :@
	GETAICONDDATA(029);
	//Log(MSG_DEBUG,"aicon 029 check target type %i",data->btTargetType);
	return AI_FAILURE;
}

//LMA: Timer since spawn (seconds).
AICOND(030)
{
	return AI_FAILURE;
}

//from here on this is 100% server sided. AIP scripts with these elements in should not be uploaded to the client


//LMA: Unknown and emtpy...
AICOND(031)
{
	return AI_SUCCESS;
}

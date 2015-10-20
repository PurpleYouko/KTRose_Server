// Props to ExJam for this code :D Ported to OSpRose by Drakia
#include "../WorldServer.h"

//Check Quest
QUESTCOND(000)
{
	GETCONDDATA(000);
    if( client->questdebug )
        server->SendPM(client, "Check quest: %i", data->iQuestSN);
	for(dword i = 0; i < 10; i++)
    {
		if(client->quest.quests[i].QuestID == data->iQuestSN)
        {
		    client->CheckQuest = i;
		    client->ActiveQuest = data->iQuestSN;
            if( client->questdebug )
                server->SendPM(client, "Check Quest - Found!");
			return QUEST_SUCCESS;
        }
	}
    if( client->questdebug )
        server->SendPM(client, "Check Quest - Not Found!");
	return QUEST_FAILURE;
}

//Check Quest Variable
QUESTCOND(001)
{
    // int iDataCnt          pos 0x00
    // dword iType           pos 0x02
    // short m_wVarNO        pos 0x02
    // word m_wVarTYPE       pos 0x04
	// word nValue           pos 0x06
	// byte btOp             pos 0x08
    GETCONDDATA(001);
	int retVal = 1;
	if( client->questdebug )server->SendPM(client, "Data Count for this quest %i",data->iDataCnt);
	for(int i = 0; i < data->iDataCnt; i++)
    {
		dword address = i * sizeof(STR_QUEST_DATA);
		address += (dword)data;
		address += 4;
		STR_QUEST_DATA* curQst = (STR_QUEST_DATA*)address;
		word nValue = curQst->nValue;
		word tempValue = client->GetQuestVar(curQst->m_wVarTYPE, curQst->m_wVarNO);
		//word tempValue = client->quest.quests[curQst->m_wVarTYPE].Variables[curQst->m_wVarNO];
		//retVal = OperateValues<word>(curQst->btOp, &tempValue, curQst->nValue);
		switch(curQst->btOp)
		{
		    case 0:
		        retVal = (tempValue == curQst->nValue);
		        break;
		    case 1:
                retVal = (tempValue > curQst->nValue);
		        break;
            case 2:
                retVal = (tempValue >= curQst->nValue);
		        break;
		    case 3:
                retVal = (tempValue < curQst->nValue);
		        break;
		    case 4:
                retVal = (tempValue <= curQst->nValue);
		        break;
		    case 10:
                retVal = (tempValue != curQst->nValue);
		        break;
		    default:
                retVal = 0;
                Log( MSG_DEBUG, "QuestCond 001 bad value for btOp %i",curQst->btOp );
                break;
        }
        if( client->questdebug )
        {
            server->SendPM(client, "is %i < %i ? retval %i",tempValue, curQst->nValue, retVal);
            server->SendPM(client, "Check Quest Var[%04x][%i] - %i (Op: %i)", curQst->m_wVarTYPE, curQst->m_wVarNO, curQst->nValue, curQst->btOp);
        }
        if(retVal == 0) return QUEST_FAILURE;
    }
	return (retVal == 0)?QUEST_FAILURE:QUEST_SUCCESS;
}

//Check Quest Variable
QUESTCOND(002)
{
	return QUEST_COND_001(server, client, raw);
}

//Check Stats
QUESTCOND(003)
{
	//dword iDataCnt;              pos 0x00
	//dword iType;                 pos 0x04
	//dword iValue;                pos 0x08
	//byte btOp;                   pos 0x0c
    GETCONDDATA(003);
	for(int i = 0; i < data->iDataCnt; i++)
    {
		dword address = i * 0x0C;
		address += (dword)data;
		address += 4;
		STR_ABIL_DATA* curAbil = (STR_ABIL_DATA*)address;
        if( client->questdebug )
            server->SendPM(client, "Check Stat[%i] %i (Op: %i)", curAbil->iType, curAbil->iValue, curAbil->btOp);
		int tempValue;
        switch( curAbil->iType )
		{
    		case sGender:
    			if(!OperateValues<int>(curAbil->btOp, (int*)&client->CharInfo->Sex, curAbil->iValue))
    			return QUEST_FAILURE;
    		break;
    		case sJob:
                tempValue = client->CharInfo->Job / 100;
    	      	if( client->questdebug )
                    server->SendPM(client, "Job Tempvalue = %i", tempValue);
    			//if(!OperateValues<int>(curAbil->btOp, (int*)&tempValue, curAbil->iValue))
    			if(!OperateValues<int>(curAbil->btOp, &tempValue, curAbil->iValue))
    				return QUEST_FAILURE;
    		break;
    		case sUnion: // Unions aren't implemented? - Drakia
				if(!OperateValues<int>(curAbil->btOp, (int*)&client->Union_s->unionvar[0], curAbil->iValue))
				return QUEST_FAILURE;
			break;
    		case sStrength:
    			if(!OperateValues<int>(curAbil->btOp, (int*)&client->Attr->Str, curAbil->iValue))
    				return QUEST_FAILURE;
    		break;
    		case sDexterity:
    			if(!OperateValues<int>(curAbil->btOp, (int*)&client->Attr->Dex, curAbil->iValue))
    				return QUEST_FAILURE;
    		break;
    		case sIntelligence:
    			if(!OperateValues<int>(curAbil->btOp, (int*)&client->Attr->Int, curAbil->iValue))
    				return QUEST_FAILURE;
    		break;
    		case sConcentration:
    			if(!OperateValues<int>(curAbil->btOp, (int*)&client->Attr->Con, curAbil->iValue))
    				return QUEST_FAILURE;
    		break;
    		case sCharm:
    			if(!OperateValues<int>(curAbil->btOp, (int*)&client->Attr->Cha, curAbil->iValue))
    				return QUEST_FAILURE;
    		break;
    		case sSensibility:
    			if(!OperateValues<int>(curAbil->btOp, (int*)&client->Attr->Sen, curAbil->iValue))
    				return QUEST_FAILURE;
    		break;
    		case sLevel:
    			if(!OperateValues<int>(curAbil->btOp, (int*)&client->Stats->Level, curAbil->iValue))
    				return QUEST_FAILURE;
    		break;
    		case sStatPoints:
    			if(!OperateValues<int>(curAbil->btOp, (int*)&client->CharInfo->StatPoints, curAbil->iValue))
    				return QUEST_FAILURE;
    		break;
    		case sMoney:
    			if(!OperateValues<int>(curAbil->btOp, (int*)&client->CharInfo->Zulies, curAbil->iValue))
    			    return QUEST_FAILURE;
    		break;
            case sEXPRate:
    			if(!OperateValues<int>(curAbil->btOp, (int*)&client->Stats->xprate, curAbil->iValue))
    				return QUEST_FAILURE;
            break;
    		default:
    			Log(MSG_WARNING, "QuestCond 003: Type Unknown: '%i'", curAbil->iType);
   			break;
		}
	}
	return QUEST_SUCCESS;
}

//Check Items
QUESTCOND(004)
{
	GETCONDDATA(004);
	for(int i = 0; i < data->iDataCnt; i++)
    {
		dword address = i * 0x10;
		address += (dword)data;
		address += 4;
		STR_ITEM_DATA* curItem = (STR_ITEM_DATA*)address;
        if( client->questdebug )
            server->SendPM(client, "Check Item - [Slot:%i][Type:%i]", curItem->iWhere, curItem->uiItemSN);
        // Check quest items for a specific amount
		if (curItem->iWhere == 13) // it's a quest item
		{
            // Ok, check the current Check'd quest.
            if (client->CheckQuest < 0) return QUEST_SUCCESS;
            unsigned int j = client->CheckQuest;
            bool Hasitem = false;
            bool FoundItem = false;
            for (unsigned int k = 0; k < 5; k++) //5 quest item slots
            {
                if (client->quest.quests[j].Items[k].itemnum == (curItem->uiItemSN % 1000))
                {
                    FoundItem = true;
                    if( client->questdebug )
                        server->SendPM(client, "Operate - btOp: %i Val1: %i Val2: %i", curItem->btOp, client->quest.quests[j].Items[k].count, curItem->iRequestCnt);
                    if(OperateValues<int>(curItem->btOp, (int*)(&client->quest.quests[j].Items[k].count), curItem->iRequestCnt))
                   	    Hasitem = true;
                }
            }
            if(FoundItem == false) // didn't find any of the item in any of the quest slots
            {
                for (unsigned int k = 0; k < 5; k++) //Find the first empty slot
                {
                    if (client->quest.quests[j].Items[k].count == 0 && Hasitem == false)
                    {
                        // Since the item doesn't exist, treat it as if there's 0 of them.
                        int tempInt = 0;
                        if(OperateValues<int>(curItem->btOp, &tempInt, curItem->iRequestCnt))
                            Hasitem = true;
                        if( client->questdebug )
                            server->SendPM(client, "Requested item is not found. Comparing first empty slot instead");
                    }
                }
            }
            if(Hasitem == false)
            {
                if( client->questdebug )
                    server->SendPM(client, "Requested item is not present. Returning quest failure");
                return QUEST_FAILURE;
            }
        }
        else //this needs to be recoded. It doesn't do a damn thing right now WTF?
        {
            //item slot is not 13
            // Check equipped items for a specific item? - Drakia
    		if(curItem->uiItemSN == 0 && curItem->iWhere != 0)
            {
    		    int itemHead = client->items[curItem->iWhere].itemtype;
    		    if(!OperateValues<int>(curItem->btOp, &itemHead, curItem->uiItemSN))
    			    return QUEST_FAILURE;
    		}
        }
	}
	if( client->questdebug )
        server->SendPM(client, "QuestCond 004 Returning Quest Success");
	return QUEST_SUCCESS;
}

//Check Party
QUESTCOND(005)
{
	//byte btIsLeader    pos 0x00
	//int iLevel         pos 0x04
	//byte btReversed    pos 0x08
	GETCONDDATA(005);
    return QUEST_SUCCESS;
}

//Near Point
QUESTCOND(006)
{
	//int iZoneSN; pos 0x00
	//int iX;      pos 0x04
	//int iY;      pos 0x08
	//int iZ;      pos 0x0c
	//int iRadius; pos 0x10
    GETCONDDATA(006);
	client->UpdatePosition();
	if(data->iZoneSN != 0)
    {
		if(client->Position->Map != data->iZoneSN)
			return QUEST_FAILURE;
	}
	float dx = client->Position->current.x - (float)(data->iX / 100);
	float dy = client->Position->current.y - (float)(data->iY / 100);
	float distance = sqrt((dx*dx) + (dy*dy));
	return (distance < data->iRadius)?QUEST_SUCCESS:QUEST_FAILURE;
}

//World Time
QUESTCOND(007)
{
    //dword ulTime            pos 0x00
	//dword ulEndTime         pos 0x04
    return QUEST_SUCCESS;
}

//Quest Time
QUESTCOND(008)
{
    //dword ulTime       pos 0x00
	//byte btOp          pos 0x04
    GETCONDDATA(008);
    if( client->questdebug )
        server->SendPM(client, "QuestTime - ulTime: %i btOp: %i", data->ulTime, data->btOp);
    if (client->CheckQuest < 0) return QUEST_FAILURE; // Not checking a quest, don't return success
    long int Time = 0;
    if (server->STB_QUEST.rows[client->quest.quests[client->CheckQuest].QuestID][1] > 0)
    {
        Time += client->quest.quests[client->CheckQuest].StartTime; // Start time
        Time += server->STB_QUEST.rows[client->quest.quests[client->CheckQuest].QuestID][1] * 10; // Time to finish
        Time -= time(NULL); // Current time
        if (Time < 0) Time = 0; // Time is up
        Time /= 10; // Divide to get 10's of seconds
    }
    if (!OperateValues<dword>(data->btOp, (dword*)&Time, data->ulTime))
        return QUEST_FAILURE;
    else
        return QUEST_SUCCESS;
}

//Check Skill
QUESTCOND(009)
{
    //int iSkillSN1          pos 0x00
	//int iSkillSN2          pos 0x02
	//byte btOp              pos 0x04
    GETCONDDATA(009);
	// Line not needed, never used - Drakia
	//int checkVal = (data->btOp)?QUEST_SUCCESS:QUEST_FAILURE;
    if( client->questdebug )
        server->SendPM(client, "Check Skill: %i", data->iSkillSN1);
    bool s1 = false;
    bool s2 = false;
	for(dword i = 0; i < 30; i++)
    {
        if(client->bskills[i] == data->iSkillSN1)
            s1 = true;
        if(client->bskills[i] == data->iSkillSN2)
            s2 = true;
        if(client->askill[i] == data->iSkillSN1)
            s1 = true;
        if(client->askill[i] == data->iSkillSN2)
            s2 = true;
        if(client->pskill[i] == data->iSkillSN1)
            s1 = true;
        if(client->pskill[i] == data->iSkillSN2)
            s2 = true;
    }
    if(data->btOp == 0)
    {
        if(s1 == false && s2 == false)
            return QUEST_SUCCESS;     //true if player does not have either skill
        else
            return QUEST_FAILURE;
    }
    else
    {
        if(s1 == true || s2 == true)
            return QUEST_SUCCESS;    //true if player has either skill
        else
            return QUEST_FAILURE;
    }
    //Log(MSG_DEBUG,"Questcond 009 btOp is neither 0 nor 1.");
	//return QUEST_FAILURE;
}

//Unknown
QUESTCOND(010)
{
	GETCONDDATA(010);
    //byte btLowPcnt;        pos 0x00
	//byte btHighPcnt;       pos 0x01
	// I think this is a percentage chance range.
	// might be something to do with the item count though???

	//int randv = GServer->RandNumber(0,100);
	//if(randv >= data->btLowPcnt && randv <= data->btHighPcnt)return QUEST_SUCCESS;

	// problem is that this is only called in quest triggers from the client where it has already
	// passed the random number test.
	// we need to always return true or some quests will never finish
    return QUEST_SUCCESS;
}

//Object Variable
QUESTCOND(011)
{

	//byte btWho         pos 0x00
	//word nVarNo        pos 0x02
	//int iValue         pos 0x04
	//byte btOp          pos 0x09
    //if(entity->_EntityType != ENTITY_NPC) return QUEST_FAILURE;
	GETCONDDATA(011);
    /*
	if(data->btWho == 0)
    {
		CNpc* thisNpc = reinterpret_cast<CNpc*>(entity);
		thisNpc = thisNpc->SelectedNpc;
		if(thisNpc == NULL) return QUEST_FAILURE;

		short VarValue = thisNpc->ObjVar.GetVar(data->nVarNo);
		if(OperateValues(data->btOp, &VarValue, (short)data->iValue)) return QUEST_SUCCESS;
	}*/
	if(data->btWho == 0)
	{

        if(data->nVarNo > 19) return AI_FAILURE;
    	int refVal = GServer->ObjVar[client->NPCvar][data->nVarNo];
    	int tempval = 0;
    	switch( data->btOp )
    	{
            case 0:
                if(refVal == data->iValue)return QUEST_SUCCESS;
                break;
            case 1:
                if(refVal > data->iValue)return QUEST_SUCCESS;
                break;
            case 2:
                if(refVal >= data->iValue)return QUEST_SUCCESS;
                break;
            case 3:
                if(refVal < data->iValue)return QUEST_SUCCESS;
                break;
            case 4:
                if(refVal <= data->iValue)return QUEST_SUCCESS;
                break;
            case 10:
                if(refVal != data->iValue)return QUEST_SUCCESS;
                break;
            default:
                Log(MSG_WARNING,"Unknown btOp in QuestCond 011");
                break;
        }
    }
	return QUEST_FAILURE;
}

//check Proximity and Trigger in Zone
QUESTCOND(012)
{
	GETCONDDATA(012);
    //word iZone;            pos 0x00
	//dword ulZone;          pos 0x00
	//int iX;                pos 0x04
	//int iY;                pos 0x06
	//int iEventID;          pos 0x08
	//dword m_HashEventOBJ;  pos 0x08
	//this should be easy enough - PY

    return QUEST_SUCCESS;
}

//Select NPC
QUESTCOND(013)
{
	//int iNpcNo         pos 0x00
    GETCONDDATA(013);
	// dammit!!!! This darn function is set up to use the CCharacter class so I can't use entity (CCharacter)
	// This is going to be a serious pain in the ass
	// OK so we now have a temporary player structure so we shuold be able to do this.

    //Log(MSG_DEBUG,"Setting NPC ref number AICOND(017). NPC Ref = %i",data->iNpcNo);
	//CMonster* monster = reinterpret_cast<CMonster*>(entity);
	//if(monster == NULL) return AI_FAILURE;
	//monster->thisnpc->refNPC = data->iNpcNo; // sets the reference variable for the correct ObjVar
    //Log(MSG_DEBUG,"Set NPC ref number %i successfully", monster->thisnpc->refNPC);
    //not sure this is going to work properly for an NPC.
    //NPC only exists as a temporary entity so refNPC may be lost after it returns from here.


	/*if(entity->_EntityType != ENTITY_NPC) return QUEST_FAILURE;

	CNpc* thisNpc = reinterpret_cast<CNpc*>(entity);
	std::map<dword, CNpc*>::iterator triggerITR = server->NpcList.find(data->iNpcNo);
	if(triggerITR == server->NpcList.end()){
		thisNpc->SelectedNpc = NULL;
		return QUEST_FAILURE;
	}
	thisNpc->SelectedNpc = triggerITR->second;
	*/
	client->NPCvar = data->iNpcNo;
	//Actually that was kind of easy.. lol - PY

	//return QUEST_SUCCESS;
	// More NPC specific stuff
	return QUEST_SUCCESS;
}

//Check Quest Flag
QUESTCOND(014)
{
    GETCONDDATA(014);
    //word nSN;          pos 0x00
	//byte btOp;         pos 0x02
	bool dataOp = (data->btOp)?true:false;
	if( client->questdebug )
        server->SendPM(client, "Check QFlag[%i] == %i", data->nSN, data->btOp);
	if(client->quest.GetFlag(data->nSN) == dataOp)
    {
        if( client->questdebug )
            server->SendPM(client, "Quest cond 014 (check quest flag) returned Success");
        return QUEST_SUCCESS;
    }
    else
    {
        if( client->questdebug )
            server->SendPM(client, "Quest cond 014 (check quest flag) returned Failure");
        return QUEST_FAILURE;
    }
}

//Party member count between X and Y - PY
QUESTCOND(015)
{

    //word nNumber1       pos 0x00
	//word nNumber2       pos 0x02
    GETCONDDATA(015);


    return QUEST_SUCCESS;
}

//Zone Time
QUESTCOND(016)
{

    //byte btWho           pos 0x00
    //dword ulWho          pos 0x00
	//dword ulTime         pos 0x04
	//dword ulEndTime      pos 0x08
	GETCONDDATA(016);

    return QUEST_SUCCESS;
}

//NPC & Obj Variables?
QUESTCOND(017)
{
    //dword iNpcNo       pos 0x00
	//word nVarNo        pos 0x04
	//word unused        pos 0x06
	//dword iNpcNo       pos 0x08
	//word nVarNo        pos 0x0c
	//word unused        pos 0x0e
	//byte btOp          pos 0x0f
    return QUEST_SUCCESS;
}

//Time on Date
QUESTCOND(018)
{
    //Returns SUCCESS if the system time of the server is between the defined start and end times. Else returns FAILURE.
    //Allows for time dependent quests
    //byte btDate;       pos 0x00
	//byte btHour1;      pos 0x01
	//byte btMin1;       pos 0x02
	//byte btHour2;      pos 0x03
	//byte btMin2;       pos 0x04
	GETCONDDATA(018);
	SYSTEMTIME sTIME;
    GetLocalTime(&sTIME);

    if(data->btDate != 0)
    {
    	if(sTIME.wDay != data->btDate)
    		return QUEST_FAILURE;
    }
    word wMin = ((sTIME.wHour * 60) + sTIME.wMinute);
    word wFrom = (data->btHour1 * 60) + data->btMin1;
    word wTo = (data->btHour2 * 60) + data->btMin2;
    if(wMin >= wFrom && wMin <= wTo)
    	return QUEST_SUCCESS;
	return QUEST_FAILURE;
}

//Time on Day
QUESTCOND(019)
{
    //byte btWeekDay;     pos 0x00
	//byte btHour1;       pos 0x01
	//byte btMin1;        pos 0x02
	//byte btHour2;       pos 0x03
	//byte btMin2;        pos 0x04
	GETCONDDATA(019);
	SYSTEMTIME sTIME;
    GetLocalTime(&sTIME);
    if(data->btWeekDay != 0)
    {
        if(sTIME.wDayOfWeek != data->btWeekDay)
            return QUEST_FAILURE;
    }
    word wMin = ((sTIME.wHour * 60) + sTIME.wMinute);
    word wFrom = (data->btHour1 * 60) + data->btMin1;
    word wTo = (data->btHour2 * 60) + data->btMin2;
    if(wMin >= wFrom && wMin <= wTo)
    	return QUEST_SUCCESS;
	return QUEST_FAILURE;
}

//Unknown
QUESTCOND(020)
{
    //int iNo1           pos 0x00
	//int iNo2           pos 0x02
	GETCONDDATA(020);
    return QUEST_SUCCESS;
}

//Distance check
QUESTCOND(021)
{
    // byte btSelObjType pos 0x00
    // byte iRadius      pos 0x04
    return QUEST_SUCCESS;
}

//Check Server/Channel
QUESTCOND(022)
{
    //word nX         pos 0x00
	//word nY         pos 0x02
    return QUEST_SUCCESS;
}

//In Clan?
QUESTCOND(023)
{
    //byte btReg         pos 0x00
    GETCONDDATA(023);
    int Clanid = client->Clan->clanid;
    if(data->btReg == 0)
    {
        if(Clanid == 0)return QUEST_SUCCESS;
        else return QUEST_FAILURE;
    }
    else
    {
        if(Clanid != 0)return QUEST_SUCCESS;
        else return QUEST_FAILURE;
    }
    //btReg should only ever be 1 or 0 but just in case......
    //return QUEST_FAILURE;
}

//Clan Position
QUESTCOND(024)
{
    //check my current rank in the clan against data->nPOS using btOP
    GETCONDDATA(024);
    //word nPOS;         pos 0x00
	//byte btOP;         pos 0x02
	int myRank = client->Clan->clanrank;
    switch(data->btOP)
	{
        case 0:
             if(myRank == data->nPOS)
                 return QUEST_SUCCESS;
             break;
        case 1:
             if(myRank > data->nPOS)
                 return QUEST_SUCCESS;
             break;
        case 2:
             if(myRank >= data->nPOS)
                 return QUEST_SUCCESS;
             break;
        case 3:
             if(myRank < data->nPOS)
                 return QUEST_SUCCESS;
             break;
        case 4:
             if(myRank <= data->nPOS)
                 return QUEST_SUCCESS;
             break;
        case 10:
             if(myRank != data->nPOS)
                 return QUEST_SUCCESS;
             break;
        default:
             return QUEST_FAILURE;
             break;
    }
}

//Clan Contribution
QUESTCOND(025)
{
    //word nCONT              pos 0x00
	//byte btOP               pos 0x02
    return QUEST_SUCCESS;
}

//Clan Grade
QUESTCOND(026)
{
    //possibly compare client's current clan grade with data->nGRD using data->btOp?
    GETCONDDATA(026);
    //word nGRD;
	//byte btOP;
	int myGrade = client->Clan->grade;
	switch(data->btOP)
	{
        case 0:
             if(myGrade == data->nGRD)
                 return QUEST_SUCCESS;
             break;
        case 1:
             if(myGrade > data->nGRD)
                 return QUEST_SUCCESS;
             break;
        case 2:
             if(myGrade >= data->nGRD)
                 return QUEST_SUCCESS;
             break;
        case 3:
             if(myGrade < data->nGRD)
                 return QUEST_SUCCESS;
             break;
        case 4:
             if(myGrade <= data->nGRD)
                 return QUEST_SUCCESS;
             break;
        case 10:
             if(myGrade != data->nGRD)
                 return QUEST_SUCCESS;
             break;
        default:
             return QUEST_FAILURE;
             break;
    }
}

//Clan Points
QUESTCOND(027)
{
    //word nPOINT             pos 0x00
	//byte btOP               pos 0x02
    GETCONDDATA(027);
    if( client->questdebug )
        server->SendPM(client, "QUEST - Clan Points");
    switch(data->btOP)
    {
        case 0x01:
            BEGINPACKET( pak, 0x7e0 );
            ADDBYTE    ( pak, 0x5C ); //action to update clan points (charserver)
	        ADDWORD    ( pak, client->Clan->clanid);
	        ADDWORD    ( pak, client->clientid );
            ADDDWORD    ( pak, client->Clan->CP );
            ADDWORD    ( pak, 0x00);
            GServer->SendISCPacket( &pak );
        break;
        //default:
        //break;
    }
	return QUEST_SUCCESS;
}

//Clan Funds
QUESTCOND(028)
{
    //word iMONEY;       pos 0x00
	//byte btOP;         pos 0x02
	GETCONDDATA(027);
    return QUEST_SUCCESS;
}

//Clan Members
QUESTCOND(029)
{
    //word nMemberCNT;   pos 0x00
	//byte btOP;         pos 0x02
    return QUEST_SUCCESS;
}

//Clan Skills
QUESTCOND(030)
{
    //word nSkill1;      pos 0x00
	//word nSkill2;      pos 0x02
	//byte btOP;         pos 0x05
    GETCONDDATA(030);
    //not sure what to do here. Looks like we compare something with two skill ids
    if( client->questdebug )server->SendPM(client, "Clan skills. Skill1: %i Skill2 %i btOP: %i",data->nSkill1,data->nSkill2,data->btOP);
    return QUEST_SUCCESS;
}

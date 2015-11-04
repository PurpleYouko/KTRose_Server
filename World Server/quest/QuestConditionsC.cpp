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

// Props to ExJam for this code :D Ported to OSpRose by Drakia

//LMA: This file if specifically for CCharacter QSD (NPC, monsters...)
#include "../worldserver.h"

//Check Quest
QUESTCONDC(000){
	return QUEST_SUCCESS;
}

//Check Quest Variable
QUESTCONDC(001){
	return QUEST_SUCCESS;
}

//Check Quest Variable
QUESTCONDC(002){
	return QUEST_SUCCESS;
}

//Check Stats
QUESTCONDC(003){
	return QUEST_SUCCESS;
}

//Check Items
QUESTCONDC(004){
	return QUEST_SUCCESS;
}

//Check Party
QUESTCONDC(005){
	return QUEST_SUCCESS;
}

//Near Point
QUESTCONDC(006){
	return QUEST_SUCCESS;
}

//World Time
QUESTCONDC(007){
	return QUEST_SUCCESS;
}

//Quest Time
QUESTCONDC(008){
	return QUEST_SUCCESS;
}

//Check Skill
QUESTCONDC(009){
	return QUEST_SUCCESS;
}

//Unknown
QUESTCONDC(010){
	return QUEST_SUCCESS;
}

//Object Variable
QUESTCONDC(011){
    GETCONDDATA(011);
	//LogDebug("QSDC(011) nVarNo = %i, iValue = %i, btOp = %i",data->nVarNo, data->iValue,data->btOp);
	CMonster* monster = reinterpret_cast<CMonster*>(client);
	if(monster == NULL)
	{
	    //LogDebug("QUESTCONDC(011) FAILED, monster NULL");
	    return QUEST_FAILURE;
	}

	int refNPC = monster->thisnpc->refNPC;
	int ObjvarIndex = data->nVarNo;
	int tempval = 0;
	int orgvalue=0;

    //LMA: WarpGate or standard NPC?
    if(monster->thisnpc->refNPC>10000&&monster->thisnpc->refNPC==GServer->WarpGate.virtualNpctypeID)
    {
        //WarpGate.
        if(ObjvarIndex>19)
            return QUEST_FAILURE;
        tempval = GServer->WarpGate.IfoObjVar[ObjvarIndex];
    }
    else
    {
        if(refNPC>=MAX_NPC)
        {
            Log(MSG_WARNING,"It seems NPC %i>=%u , change MAX_NPC value!",refNPC,MAX_NPC);
            return QUEST_FAILURE;
        }

        tempval = GServer->ObjVar[refNPC][ObjvarIndex];
    }

    //LMA: We force the nb players required for UW.
    orgvalue=data->iValue;

    //LMA: 2010/07/30: naRose changed its requirements from 30/33 to 20/23.
    //if(ObjvarIndex==2&&monster->thisnpc->refNPC>=1088&&monster->thisnpc->refNPC<=1091&&(orgvalue==30||orgvalue==33))
    if(ObjvarIndex==2&&monster->thisnpc->refNPC>=1088&&monster->thisnpc->refNPC<=1091&&(orgvalue==20||orgvalue==23))
    {
        //if(orgvalue==30&&data->btOp==2)
        if(orgvalue==20&&data->btOp==2)
        {
            orgvalue=GServer->UWNbPlayers;
            LogDebug("QSDCC11::UW We trapped NPC %i[%i]>=%i (replaces >= %i)",monster->thisnpc->refNPC,ObjvarIndex,orgvalue,data->iValue);
        }

        //if(orgvalue==232&data->btOp==4)
        if(orgvalue==23&&data->btOp==4)
        {
            orgvalue=GServer->UWNbPlayers+3;
            LogDebug("QSDCC11::UW We trapped NPC %i[%i]<=%i (replaces <= %i)",monster->thisnpc->refNPC,ObjvarIndex,orgvalue,data->iValue);
        }

    }

	switch(data->btOp)
	{
        case 0:
             if(tempval == orgvalue)
                 return QUEST_SUCCESS;
             break;
        case 1:
             if(tempval > orgvalue)
                 return QUEST_SUCCESS;
             break;
        case 2:
             if(tempval >= orgvalue)
                 return QUEST_SUCCESS;
             break;
        case 3:
             if(tempval < orgvalue)
                 return QUEST_SUCCESS;
             break;
        case 4:
             if(tempval <= orgvalue)
                 return QUEST_SUCCESS;
             break;
        case 10:
             if(tempval != orgvalue)
                 return QUEST_SUCCESS;
             break;
        default:
        {
            //LogDebug("QUESTCONDC(011) FAILED, switch default");
             return QUEST_FAILURE;
        }
        break;
    }

    //LogDebug("QUESTCONDC(011) FAILED, compare false");
	return QUEST_FAILURE;
}

//Select Ifo Object
QUESTCONDC(012){
    //Log(MSG_WARNING,"Monster/NPC using QuestCondition 012");
    GETCONDDATA(012);

    //LMA: For now we do only this stuff for the warp gate :)
    if(GServer->WarpGate.id!=data->iEventID||GServer->WarpGate.IfoX!=data->iX||GServer->WarpGate.IfoY!=data->iY||GServer->WarpGate.mapid!=data->iZone)
    {
        LogDebug("QSDCC12:: This is not the gate, event object %i/%i, X=%i/%i, Y=%i/%i, map=%i/%i",data->iEventID,GServer->WarpGate.id,data->iX,GServer->WarpGate.IfoX,data->iY,GServer->WarpGate.IfoY,data->iZone,GServer->WarpGate.mapid);
        return QUEST_FAILURE;
    }

    CMonster* monster = reinterpret_cast<CMonster*>(client);
	if(monster == NULL)
	{
	    LogDebug("QSDCC12:: FAILED, monster NULL");
	    return QUEST_FAILURE;
    }

	monster->thisnpc->refNPC = GServer->WarpGate.virtualNpctypeID; // sets the reference variable for the correct ObjVar
    Log(MSG_INFO,"CDTCC12:: Warp Gate Selected");


	return QUEST_SUCCESS;
}


//Select NPC
QUESTCONDC(013){
    GETCONDDATA(013);

	//LogDebug("Setting NPC ref number QUESTCONDC(013)");
	CMonster* monster = reinterpret_cast<CMonster*>(client);
	if(monster == NULL)
	{
	    //Log(MSG_WARNING,"QUESTCONDC(013) FAILED, monster NULL");
	    return QUEST_FAILURE;
    }
	monster->thisnpc->refNPC = data->iNpcNo; // sets the reference variable for the correct ObjVar
    ////LogDebug("Set NPC ref number %i successfully", monster->thisnpc->refNPC);


    return QUEST_SUCCESS;
}



//Check Quest Flag
QUESTCONDC(014){
	return QUEST_SUCCESS;
}

//Unknown
QUESTCONDC(015){
	return QUEST_SUCCESS;
}

//Zone Time
QUESTCONDC(016){
	return QUEST_SUCCESS;
}

//LMA: Compare two NPC & Obj Variables.
QUESTCONDC(017)
{
    GETCONDDATA(017);

    int var1=0;
    int var2=0;

    LogDebug("QDC17 We compare NPC %i[%i] with (%i) NPC %i[%i]",data->NpcVar1.iNpcNo,data->NpcVar1.nVarNo,data->btOp,data->NpcVar1.iNpcNo,data->NpcVar1.nVarNo);

    if(data->NpcVar1.iNpcNo>=MAX_NPC||data->NpcVar2.iNpcNo>=MAX_NPC||data->NpcVar1.nVarNo>=20||data->NpcVar2.nVarNo>=20)
    {
        if(data->NpcVar1.iNpcNo>=MAX_NPC)
        {
            Log(MSG_WARNING,"It seems NPC %i>=%u , change MAX_NPC value!",data->NpcVar1.iNpcNo,MAX_NPC);
            return QUEST_FAILURE;
        }

        if(data->NpcVar2.iNpcNo>=MAX_NPC)
        {
            Log(MSG_WARNING,"It seems NPC %i>=%u , change MAX_NPC value!",data->NpcVar2.iNpcNo,MAX_NPC);
            return QUEST_FAILURE;
        }

        LogDebug("QDC17 Failure 1");
        return QUEST_FAILURE;
    }

    var1=GServer->ObjVar[data->NpcVar1.iNpcNo][data->NpcVar1.nVarNo];
    var1=GServer->ObjVar[data->NpcVar2.iNpcNo][data->NpcVar2.nVarNo];

    if(!OperateValues<int>(data->btOp, (int*)&var1, var2))
    {
        LogDebug("QDC17 Failure 2");
        return QUEST_FAILURE;
    }


    LogDebug("QDC17 Success");
    return QUEST_SUCCESS;
}

//Time on Date
QUESTCONDC(018){
	return QUEST_SUCCESS;
}

//Time on Day
QUESTCONDC(019){
	return QUEST_SUCCESS;
}

//Unknown
QUESTCONDC(020){
	return QUEST_SUCCESS;
}

//Unknown
QUESTCONDC(021){
	return QUEST_SUCCESS;
}

//Check Server/Channel
QUESTCONDC(022){
	return QUEST_SUCCESS;
}

//In Clan
QUESTCONDC(023){
	return QUEST_SUCCESS;
}

//Clan Position
QUESTCONDC(024){
    return QUEST_SUCCESS;
}

//Clan Contribution
QUESTCONDC(025){
    return QUEST_SUCCESS;
}

//Clan Grade
QUESTCONDC(026){
    return QUEST_SUCCESS;
}

//Clan Points
QUESTCONDC(027){
	return QUEST_SUCCESS;
}

//Clan Funds
QUESTCONDC(028){
	return QUEST_SUCCESS;
}

//Clan Members
QUESTCONDC(029){
	return QUEST_SUCCESS;
}

//Clan Skills
QUESTCONDC(030){
	return QUEST_SUCCESS;
}


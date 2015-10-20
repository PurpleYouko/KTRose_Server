#include "main.h"

int OperateValues(byte btOp, int* value1PTR, int value2){
	int value1 = *value1PTR;
	switch(btOp){
		case 0:
			return (value1 == value2);
		case 1:
			return (value1 <= value2);
		case 2:
			return (value1 >= value2);
		case 3:
			return (value1 > value2);
		case 4:
			return (value1 < value2);
		case 5:
			(*value1PTR) = value2;
			return 1;
		case 6:
			(*value1PTR) = value1 + value2;
			return 1;
		case 7:
			(*value1PTR) = value1 - value2;
			return 1;
		default:
			return 0;
	}
}

//Check Quest
QUESTCOND(000){
	GETCONDDATA(000);
	for(dword i = 0; i < 10; i++){
		if(client->quest.quests[i].QuestID == data->iQuestSN)
			return QUEST_SUCCESS;
	}
	return QUEST_FAILURE;
}

//Check Quest Variable
QUESTCOND(001){
	return QUEST_SUCCESS;
}

//Check Quest Variable
QUESTCOND(002){
	return QUEST_SUCCESS;
}

//Check Stats
QUESTCOND(003){
	GETCONDDATA(003);
	for(int i = 0; i < data->iDataCnt; i++){
		qword address = i * 0x0C;
		address += (qword)data;
		address += 4;
		STR_ABIL_DATA* curAbil = (STR_ABIL_DATA*)address;
		if(!OperateValues(curAbil->btOp, &client->stats_base[curAbil->iType], curAbil->iValue))
			return QUEST_FAILURE;
	}
	return QUEST_SUCCESS;
}

//Check Items
QUESTCOND(004){
	GETCONDDATA(004);
	for(int i = 0; i < data->iDataCnt; i++){
		//QWord for 64bit safe pointers.
		qword address = i * 0x10;
		address += (qword)data;
		address += 4;
		STR_ITEM_DATA* curItem = (STR_ITEM_DATA*)address;
		if(curItem->uiItemSN == 0 && curItem->iWhere != 0){
			int itemHead = client->inv.items[curItem->iWhere].itemhead;
			if(!OperateValues(curItem->btOp, &itemHead, curItem->uiItemSN))
				return QUEST_FAILURE;
		}
		Log(MSG_INFO,"uiItemSN: %u iWhere: %u iRequestCnt: %u btOp: %u", curItem->uiItemSN, curItem->iWhere, curItem->iRequestCnt, curItem->btOp);
	}
	return QUEST_SUCCESS;
}

//Check Party
QUESTCOND(005){
	return QUEST_SUCCESS;
}

//Near Point
QUESTCOND(006){
	GETCONDDATA(006);
	client->UpdatePosition();
	if(data->iZoneSN != 0){
		if(client->basic.map != data->iZoneSN)
			return QUEST_FAILURE;
	}

	float dx = client->basic.pos.x - (float)data->iX;
	float dy = client->basic.pos.y - (float)data->iY;
	float distance = sqrt((dx*dx) + (dy*dy));
	return (distance < data->iRadius)?QUEST_SUCCESS:QUEST_FAILURE;
}

//World Time
QUESTCOND(007){
	return QUEST_SUCCESS;
}

//Quest Time
QUESTCOND(008){
	return QUEST_SUCCESS;
}

//Check Skill
QUESTCOND(009){
	GETCONDDATA(009);
	int checkVal = (data->btOp)?QUEST_SUCCESS:QUEST_FAILURE;
	for(dword i = 0; i < 90; i++){
		if(client->skill.Skills[i] == data->iSkillSN1)
			return (data->btOp)?QUEST_SUCCESS:QUEST_FAILURE;
	}
	return (data->btOp)?QUEST_FAILURE:QUEST_SUCCESS;
}

//Unknown
QUESTCOND(010){
	return QUEST_SUCCESS;
}

//Quest Variable
QUESTCOND(011){
	return QUEST_SUCCESS;
}

//Execute Trigger in Zone
QUESTCOND(012){
	return QUEST_SUCCESS;
}

//Near NPC
QUESTCOND(013){
	return QUEST_SUCCESS;
}

//Check Quest Flag
QUESTCOND(014){
	GETCONDDATA(014);
	bool dataOp = (data->btOp)?true:false;
	return (client->quest.GetFlag(data->nSN) == dataOp)?QUEST_SUCCESS:QUEST_FAILURE;
}

//Unknown
QUESTCOND(015){
	return QUEST_SUCCESS;
}

//Zone Time
QUESTCOND(016){
	return QUEST_SUCCESS;
}

//NPC Variables
QUESTCOND(017){
	return QUEST_SUCCESS;
}

//Time on Date
QUESTCOND(018){
	return QUEST_SUCCESS;
}

//Time on Day
QUESTCOND(019){
	return QUEST_SUCCESS;
}

//Unknown
QUESTCOND(020){
	return QUEST_SUCCESS;
}

//Unknown
QUESTCOND(021){
	return QUEST_SUCCESS;
}

//Check Server/Channel
QUESTCOND(022){
	return QUEST_SUCCESS;
}

//In Clan
QUESTCOND(023){
	return QUEST_SUCCESS;
}

//Clan Position
QUESTCOND(024){
	return QUEST_SUCCESS;
}

//Clan Contribution
QUESTCOND(025){
	return QUEST_SUCCESS;
}

//Clan Grade
QUESTCOND(026){
	return QUEST_SUCCESS;
}

//Clan Points
QUESTCOND(027){
	return QUEST_SUCCESS;
}

//Clan Funds
QUESTCOND(028){
	return QUEST_SUCCESS;
}

//Clan Members
QUESTCOND(029){
	return QUEST_SUCCESS;
}

//Clan Skills
QUESTCOND(030){
	return QUEST_SUCCESS;
}
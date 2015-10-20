#include "main.h"

dword GetRewardValue(dword function, dword amount, CWorldClient* client, word nDupCNT){
	switch(function){
		case 0:
		{
			dword tempVal = amount + 0x1E;
			tempVal *= client->GetStat(sCharm) + 0x0a;
			tempVal *= (100 & 0xFFFF);//World Rate
			tempVal *= 0x14;//Fame + 0x14
			tempVal = ((tempVal / (client->stats_base[sLevel] + 0x46)) / 0x7530) + amount;
			return tempVal;
		}
		break;
		case 1:
		{
			dword tempVal = (client->stats_base[sLevel] + 3) * amount;
			tempVal *= (client->GetStat(sCharm) >> 1) + client->stats_base[sLevel] + 0x28;
			tempVal *= (100 & 0xFFFF);//World Rate
			return tempVal / 0x2710;
		}
		break;
		case 2:
			return (amount * nDupCNT);
		break;
		case 3:
		case 5:
		{
			dword tempVal = amount + 0x14;
			tempVal *= client->GetStat(sCharm) + 0x0a;
			tempVal *= (100 & 0xFFFF);//World Rate
			tempVal *= 0x14;//Fame + 0x14
			tempVal = ((tempVal / (client->stats_base[sLevel] + 0x46)) / 0x7530) + amount;
			return tempVal;	
		}
		break;
		case 4:
		{
			dword tempVal = amount + 0x2;
			tempVal *= (client->GetStat(sCharm) + client->stats_base[sLevel] + 0x28);
			tempVal *= 0x28;//Fame + 0x28
			tempVal *= (100 & 0xFFFF);//World Rate
			return tempVal / 0x222E0;
		}
		break;
		case 6:
		{
			dword tempVal = amount + 0x14;
			tempVal *= client->GetStat(sCharm) + client->stats_base[sLevel];
			tempVal *= 0x14;//Fame + 0x14
			tempVal *= (100 & 0xFFFF);//World Rate
			return (tempVal / 0x2DC6C0) + amount;
		}
		break;
	}
	return amount;
}

//Update Quest
QUESTREWD(000){
	GETREWDDATA(000);
	
	switch(data->btOp){//0 remove, 1 start, 4 select
		case 0:
		{
			for(dword i = 0; i < 10; i++){
				if(client->quest.quests[i].QuestID != data->iQuestSN) continue;
				
				memset(&client->quest.quests[i], 0, sizeof(SQuest));
				break;
			}
		}
		break;
		case 1:
		{
			for(dword i = 0; i < 10; i++){
				if(client->quest.quests[i].QuestID == 0) continue;				
				memset(&client->quest.quests[i], 0, sizeof(SQuest));
				client->quest.quests[i].QuestID = data->iQuestSN;
				client->quest.quests[i].Time = clock_t();
				break;
			}
		}
		break;
		case 4:
		{
			client->ActiveQuest = data->iQuestSN;
		}
		break;
	}
	return QUEST_SUCCESS;
}

//Update Quest Items
QUESTREWD(001){	
	return QUEST_SUCCESS;
}

//Set Quest Variable
QUESTREWD(002){
	return QUEST_SUCCESS;
}

//Update Stats
QUESTREWD(003){
	GETREWDDATA(003);
	for(dword i = 0; i < data->iDataCnt; i++){
		qword address = i * 0x0C;
		address += (qword)data;
		address += 4;
		STR_ABIL_DATA* curAbil = (STR_ABIL_DATA*)address;
		if(!OperateValues(curAbil->btOp, &client->stats_base[curAbil->iType], curAbil->iValue))
			return QUEST_FAILURE;
	}
	return QUEST_SUCCESS;
}

//Set Quest Variable
QUESTREWD(004){
	return QUEST_SUCCESS;
}

//Give Reward
QUESTREWD(005){	
	GETREWDDATA(005);

	switch(data->btTarget){
		case 0://EXP
		{
			client->basic.exp += GetRewardValue(data->btEquation, data->iValue, client, 0);
			CPacket pakout(0x79b);
			pakout.Add<dword>(client->basic.exp);
			pakout.Add<dword>(client->stats_base[sStamina]);
			pakout.Add<word>(0);
			server->SendPacket(client, &pakout);
		}
		break;
		case 1://Zuly
			client->inv.money += GetRewardValue(data->btEquation, data->iValue, client, 1);//dunno nDupCount for this one!
			CPacket pakout(0x71D);
			pakout.Add<qword>(client->inv.money);
			server->SendPacket(client, &pakout);
		break;
		case 2://Item
		{		
			CItem nItem;
			nItem.type = data->iItemSN / 1000;
			nItem.id = data->iItemSN - (nItem.type * 1000);
			if(nItem.IsStackable()){
				nItem.count = GetRewardValue(data->btEquation, data->iValue, client, 0);
			}else{
				nItem.itemdata = 0;
				nItem.durability = 40;
				nItem.lifespan = 1000;
				nItem.appraised = 1;
			}
			client->AddItem(&nItem, true);
		}
		break;
	}

	return QUEST_SUCCESS;
}

//Restore HP/MP
QUESTREWD(006){
	return QUEST_SUCCESS;
}

//Teleport
QUESTREWD(007){
	GETREWDDATA(007);
	client->MoveMap(data->iZoneSN, (float)data->iX, (float)data->iY);
	return QUEST_SUCCESS;
}

//Spawn Monster
QUESTREWD(008){
	return QUEST_SUCCESS;
}

//Execute Quest Trigger
QUESTREWD(009){
	return QUEST_SUCCESS;
}

//Reset Stats
QUESTREWD(010){
	client->stats_base[sStatPoints] = 0;

	client->stats_base[sStrength] = 15;
	client->stats_base[sDexterity] = 15;
	client->stats_base[sIntelligence] = 15;
	client->stats_base[sConcentration] = 15;
	client->stats_base[sCharm] = 10;
	client->stats_base[sSensibility] = 10;

	for(int i = 0; i <= client->stats_base[sLevel]; i++)
		client->stats_base[sStatPoints] += (i + 9);
	
	return QUEST_SUCCESS;
}

//Update Quest Var
QUESTREWD(011){
	return QUEST_SUCCESS;
}

//NPC Speak
QUESTREWD(012){
	return QUEST_SUCCESS;
}

//Unknown
QUESTREWD(013){
	return QUEST_SUCCESS;
}

//Learn Skill
QUESTREWD(014){

	return QUEST_SUCCESS;
}

//Set Quest Flag
QUESTREWD(015){
	GETREWDDATA(015);
	client->quest.SetFlag(data->nSN, (data->btOp == 1)?true:false);
	return QUEST_SUCCESS;
}

//Unknown
QUESTREWD(016){
	return QUEST_SUCCESS;
}

//Reset All Quest Flags
QUESTREWD(017){
	memset(&client->quest.flags, 0, 244);
	return QUEST_SUCCESS;
}

//Send Announcement
QUESTREWD(018){
	return QUEST_SUCCESS;
}

//Execute Quest Trigger in Other Map
QUESTREWD(019){
	return QUEST_SUCCESS;
}

//PvP Status
QUESTREWD(020){
	return QUEST_SUCCESS;
}

//Set Respawn Position
QUESTREWD(021){
	return QUEST_SUCCESS;
}

//Unknown
QUESTREWD(022){
	return QUEST_SUCCESS;
}

//Raise Clan Grade
QUESTREWD(023){
	return QUEST_SUCCESS;
}

//Clan Money
QUESTREWD(024){
	return QUEST_SUCCESS;
}

//Clan Points
QUESTREWD(025){
	return QUEST_SUCCESS;
}

//Clan Skill
QUESTREWD(026){
	return QUEST_SUCCESS;
}

//Clan Contribution
QUESTREWD(027){
	return QUEST_SUCCESS;
}

//Clan Teleportation
QUESTREWD(028){
	return QUEST_SUCCESS;
}
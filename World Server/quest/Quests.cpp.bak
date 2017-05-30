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
#include "../worldserver.h"

void CWorldServer::ReadQSD(strings path, dword index)
{
    //LMA: mass exporter.
    bool lma_export=false;
    if(Config.massexport)
        lma_export=true;

	CRoseFile* fh = new CRoseFile(path, FM_READ | FM_BINARY);
	if(fh->IsOpen())
	{ // goto done;

        Log(MSG_LOAD, "Loading %s", path);

        fh->Seek(4, SEEK_CUR);
        dword BlockCount = fh->Get<dword>();

        if(lma_export)
        {
            LogSp(MSG_INFO, " ");
            LogSp(MSG_INFO, " ");

            if(BlockCount==0)
            {
                LogSp(MSG_INFO, "Exporting %s :: 0 block", path);
            }
            else if (BlockCount==1)
            {
                LogSp(MSG_INFO, "Exporting %s :: 1 block", path);
            }
            else
            {
                LogSp(MSG_INFO, "Exporting %s :: %i blocks", path,BlockCount);
            }

            LogSp(MSG_INFO, "{");

        }

        fh->Seek(fh->Get<word>(), SEEK_CUR);
        for(dword i = 0; i < BlockCount; i++)
        {
            dword RecordCount = fh->Get<dword>();

            if(lma_export)
            {
                if(i>0)
                    LogSp(MSG_INFO, "\t ");

                if(RecordCount==0)
                {
                    LogSp(MSG_INFO, "\t Block %i / %i :: 0 Function",i+1,BlockCount);
                }
                else if (RecordCount==1)
                {
                    LogSp(MSG_INFO, "\t Block %i / %i :: 1 Function",i+1,BlockCount);
                }
                else
                {
                    LogSp(MSG_INFO, "\t Block %i / %i :: %i Functions",i+1,BlockCount,RecordCount);
                }

                LogSp(MSG_INFO, "\t {");

            }

            fh->Seek(fh->Get<word>(), SEEK_CUR);
            for(dword j = 0; j < RecordCount; j++)
            {
                CQuestTrigger* trigger = new CQuestTrigger();
                trigger->id=((index*0x10000)+(i*0x100)+j);
                trigger->CheckNext = fh->Get<byte>();
                trigger->ConditionCount = fh->Get<dword>();
                trigger->ActionCount = fh->Get<dword>();

                dword len = fh->Get<word>();
                trigger->TriggerName = new char[len+1];
                fh->Read(trigger->TriggerName, len, 1);
                trigger->TriggerName[len] = 0;

                if(lma_export)
                {
                    char bactions[20];
                    char bconditions[20];

                    if(trigger->ConditionCount==0)
                    {
                        sprintf(bconditions,"0 condition");
                    }
                    else if(trigger->ConditionCount==1)
                    {
                        sprintf(bconditions,"1 condition");
                    }
                    else
                    {
                        sprintf(bconditions,"%i conditions",trigger->ConditionCount);
                    }

                    if(trigger->ActionCount==0)
                    {
                        sprintf(bactions,"0 action");
                    }
                    else if(trigger->ActionCount==1)
                    {
                        sprintf(bactions,"1 action");
                    }
                    else
                    {
                        sprintf(bactions,"%i actions",trigger->ActionCount);
                    }

                    if(j>0)
                        LogSp(MSG_INFO, "\t\t ");

                    LogSp(MSG_INFO, "\t\t Function %i / %i (%s / %u) :: %s, %s",j+1,RecordCount,trigger->TriggerName,MakeStrHash(trigger->TriggerName),bconditions,bactions);
                    if(trigger->CheckNext)
                        LogSp(MSG_INFO, "\t\t\t <If Function %s returns False, we'll check the next Function (check_next activated)>",trigger->TriggerName);
                    LogSp(MSG_INFO, "\t\t {");
                }

                if(trigger->ConditionCount > 0)
                {
                    if(lma_export)
                    {
                        LogSp(MSG_INFO, "\t\t\t Conditions (%i)",trigger->ConditionCount);
                        LogSp(MSG_INFO, "\t\t\t {");
                    }

                    trigger->Conditions = new CQuestTrigger::SQuestDatum*[trigger->ConditionCount];
                    for(dword k = 0; k < trigger->ConditionCount; k++)
                    {
                        CQuestTrigger::SQuestDatum* data = new CQuestTrigger::SQuestDatum();
                        data->size = fh->Get<int>();
                        data->opcode = fh->Get<int>();
                        data->data = new byte[data->size - 8];
                        fh->Read(data->data, data->size - 8, 1);
                        trigger->Conditions[k] = data;

                        //LMA: Export
                        if (lma_export)
                        {
                            //ExportQSDData(data->data,data->opcode,data->size,data);
                            ExportQSDData(data->data,data->size,data->opcode);
                        }

                    }

                    if(lma_export)
                    {
                        LogSp(MSG_INFO, "\t\t\t }");
                    }

                }
                else
                {
                    if(lma_export)
                    {
                        LogSp(MSG_INFO, "\t\t\t 0 Condition");
                    }

                    trigger->Conditions = NULL;
                }

                if(trigger->ActionCount > 0)
                {
                    trigger->Actions = new CQuestTrigger::SQuestDatum*[trigger->ActionCount];
                    if(lma_export)
                    {
                        LogSp(MSG_INFO, "\t\t\t Actions (%i)",trigger->ActionCount);
                        LogSp(MSG_INFO, "\t\t\t {");
                    }

                    for(dword k = 0; k < trigger->ActionCount; k++)
                    {
                        CQuestTrigger::SQuestDatum* data = new CQuestTrigger::SQuestDatum();
                        data->size = fh->Get<int>();
                        data->opcode = fh->Get<int>() - 0x01000000;
                        data->data = new byte[data->size - 8];
                        fh->Read(data->data, data->size - 8, 1);
                        trigger->Actions[k] = data;

                        /*
                        if(data->opcode==8)
                        {
                            STR_REWD_008 * data8 = (STR_REWD_008 *)data->data;
                            //Log(MSG_INFO,"%s has an action 8 monster %i",path,data8->iMonsterSN);

                            //if(data8->iMonsterSN>=3050&&data8->iMonsterSN<=3090)
                            if(data8->iMonsterSN>=3040&&data8->iMonsterSN<=3041)
                                Log(MSG_INFO,"%s has an action 8 monster=%i,btwho=%i,many=%i,range=%i,team=%i,iwho=%i,x=%i,y=%i,zone=%i",path,data8->iMonsterSN,data8->btWho,data8->iHowMany,data8->iRange,data8->iTeamNo,data8->iWho,data8->iX,data8->iY,data8->iZoneSN);
                        }
                        */

                        //LMA: Export
                        if (lma_export)
                        {
                            ExportQSDDataA(data->data,data->size,data->opcode);
                        }

                    }

                    if(lma_export)
                    {
                        LogSp(MSG_INFO, "\t\t\t }");
                    }

                }
                else
                {
                    if(lma_export)
                    {
                        LogSp(MSG_INFO, "\t\t\t 0 Action");
                    }

                    trigger->Actions = NULL;
                }

                trigger->TriggerHash = MakeStrHash(trigger->TriggerName);
                TriggerList.push_back( trigger );

                if(lma_export)
                {
                    LogSp(MSG_INFO, "\t\t ");
                    LogSp(MSG_INFO, "\t\t }");
                }

            }

            if(lma_export)
            {
                LogSp(MSG_INFO, "\t ");
                LogSp(MSG_INFO, "\t }");
            }

        }

        if(lma_export)
        {
            LogSp(MSG_INFO, " ");
            LogSp(MSG_INFO, "}");
        }

    }
    else
    {
         //Log( MSG_ERROR, "QSD File: '%s'", path );
         Log( MSG_WARNING, "QSD File: '%s'", path );
    }

    fh->Close();
	delete fh;
}


void CWorldServer::LoadQuestData()
{
    //LMA: loading Pegasus data.
    CStrStb* stbQuest=NULL;
    if(Config.is_pegasus==1)
    {
        stbQuest = new CStrStb("3DDataPeg/stb/LIST_QUESTDATA.STB");
    }
    else
    {
        stbQuest = new CStrStb("3ddata/stb/LIST_QUESTDATA.STB");
    }

	char buffer [100];
	char path[100];

	for(dword i = 1; i < stbQuest->Rows(); i++){
		if(stbQuest->Data(i, 0)){
			strcpy((char*)&buffer, stbQuest->Data(i, 0));

			for (int loop = 0; buffer[loop] !=0; loop++)
			{
				buffer[loop] = toupper(buffer[loop]);
				if (buffer[loop] == '\\')
				{
					buffer[loop] = '/';
				}

                //LMA: For Pegasus.
			    if(Config.is_pegasus==1&&loop<7)
                    path[loop]=toupper(buffer[loop]);
			}

			//LMA: Patch for Pegasus server data files:
			if(Config.is_pegasus==1)
			{
			    char temp[] = "3DDATA/";
			    if(strcmp (temp,path)==0)
			    {
			        string temps=buffer;
			        temps="3DDATAPEG/"+temps.substr(7);
                    strcpy ((char*)&buffer,temps.c_str());
                    buffer[temps.size()]=0;
			    }

			}

			GServer->ReadQSD((strings)&buffer,i);
		}
	}

	Log(MSG_INFO, "Finished loading quest data                              ");

	qstCondFunc[0] = &QUEST_COND_000;
	qstCondFunc[1] = &QUEST_COND_001;
	qstCondFunc[2] = &QUEST_COND_002;
	qstCondFunc[3] = &QUEST_COND_003;
	qstCondFunc[4] = &QUEST_COND_004;
	qstCondFunc[5] = &QUEST_COND_005;
	qstCondFunc[6] = &QUEST_COND_006;
	qstCondFunc[7] = &QUEST_COND_007;
	qstCondFunc[8] = &QUEST_COND_008;
	qstCondFunc[9] = &QUEST_COND_009;
	qstCondFunc[10] = &QUEST_COND_010;
	qstCondFunc[11] = &QUEST_COND_011;
	qstCondFunc[12] = &QUEST_COND_012;
	qstCondFunc[13] = &QUEST_COND_013;
	qstCondFunc[14] = &QUEST_COND_014;
	qstCondFunc[15] = &QUEST_COND_015;
	qstCondFunc[16] = &QUEST_COND_016;
	qstCondFunc[17] = &QUEST_COND_017;
	qstCondFunc[18] = &QUEST_COND_018;
	qstCondFunc[19] = &QUEST_COND_019;
	qstCondFunc[20] = &QUEST_COND_020;
	qstCondFunc[21] = &QUEST_COND_021;
	qstCondFunc[22] = &QUEST_COND_022;
	qstCondFunc[23] = &QUEST_COND_023;
	qstCondFunc[24] = &QUEST_COND_024;
	qstCondFunc[25] = &QUEST_COND_025;
	qstCondFunc[26] = &QUEST_COND_026;
	qstCondFunc[27] = &QUEST_COND_027;
	qstCondFunc[28] = &QUEST_COND_028;
	qstCondFunc[29] = &QUEST_COND_029;
	qstCondFunc[30] = &QUEST_COND_030;

	qstRewdFunc[0] = &QUEST_REWD_000;
	qstRewdFunc[1] = &QUEST_REWD_001;
	qstRewdFunc[2] = &QUEST_REWD_002;
	qstRewdFunc[3] = &QUEST_REWD_003;
	qstRewdFunc[4] = &QUEST_REWD_004;
	qstRewdFunc[5] = &QUEST_REWD_005;
	qstRewdFunc[6] = &QUEST_REWD_006;
	qstRewdFunc[7] = &QUEST_REWD_007;
	qstRewdFunc[8] = &QUEST_REWD_008;
	qstRewdFunc[9] = &QUEST_REWD_009;
	qstRewdFunc[10] = &QUEST_REWD_010;
	qstRewdFunc[11] = &QUEST_REWD_011;
	qstRewdFunc[12] = &QUEST_REWD_012;
	qstRewdFunc[13] = &QUEST_REWD_013;
	qstRewdFunc[14] = &QUEST_REWD_014;
	qstRewdFunc[15] = &QUEST_REWD_015;
	qstRewdFunc[16] = &QUEST_REWD_016;
	qstRewdFunc[17] = &QUEST_REWD_017;
	qstRewdFunc[18] = &QUEST_REWD_018;
	qstRewdFunc[19] = &QUEST_REWD_019;
	qstRewdFunc[20] = &QUEST_REWD_020;
	qstRewdFunc[21] = &QUEST_REWD_021;
	qstRewdFunc[22] = &QUEST_REWD_022;
	qstRewdFunc[23] = &QUEST_REWD_023;
	qstRewdFunc[24] = &QUEST_REWD_024;
	qstRewdFunc[25] = &QUEST_REWD_025;
	qstRewdFunc[26] = &QUEST_REWD_026;
	qstRewdFunc[27] = &QUEST_REWD_027;
	qstRewdFunc[28] = &QUEST_REWD_028;
	qstRewdFunc[29] = &QUEST_REWD_029;
	qstRewdFunc[34] = &QUEST_REWD_034;


    //LMA: AIP
	qstCondFuncC[0] = &QUEST_CONDC_000;
	qstCondFuncC[1] = &QUEST_CONDC_001;
	qstCondFuncC[2] = &QUEST_CONDC_002;
	qstCondFuncC[3] = &QUEST_CONDC_003;
	qstCondFuncC[4] = &QUEST_CONDC_004;
	qstCondFuncC[5] = &QUEST_CONDC_005;
	qstCondFuncC[6] = &QUEST_CONDC_006;
	qstCondFuncC[7] = &QUEST_CONDC_007;
	qstCondFuncC[8] = &QUEST_CONDC_008;
	qstCondFuncC[9] = &QUEST_CONDC_009;
	qstCondFuncC[10] = &QUEST_CONDC_010;
	qstCondFuncC[11] = &QUEST_CONDC_011;
	qstCondFuncC[12] = &QUEST_CONDC_012;
	qstCondFuncC[13] = &QUEST_CONDC_013;
	qstCondFuncC[14] = &QUEST_CONDC_014;
	qstCondFuncC[15] = &QUEST_CONDC_015;
	qstCondFuncC[16] = &QUEST_CONDC_016;
	qstCondFuncC[17] = &QUEST_CONDC_017;
	qstCondFuncC[18] = &QUEST_CONDC_018;
	qstCondFuncC[19] = &QUEST_CONDC_019;
	qstCondFuncC[20] = &QUEST_CONDC_020;
	qstCondFuncC[21] = &QUEST_CONDC_021;
	qstCondFuncC[22] = &QUEST_CONDC_022;
	qstCondFuncC[23] = &QUEST_CONDC_023;
	qstCondFuncC[24] = &QUEST_CONDC_024;
	qstCondFuncC[25] = &QUEST_CONDC_025;
	qstCondFuncC[26] = &QUEST_CONDC_026;
	qstCondFuncC[27] = &QUEST_CONDC_027;
	qstCondFuncC[28] = &QUEST_CONDC_028;
	qstCondFuncC[29] = &QUEST_CONDC_029;
	qstCondFuncC[30] = &QUEST_CONDC_030;

	qstRewdFuncC[0] = &QUEST_REWDC_000;
	qstRewdFuncC[1] = &QUEST_REWDC_001;
	qstRewdFuncC[2] = &QUEST_REWDC_002;
	qstRewdFuncC[3] = &QUEST_REWDC_003;
	qstRewdFuncC[4] = &QUEST_REWDC_004;
	qstRewdFuncC[5] = &QUEST_REWDC_005;
	qstRewdFuncC[6] = &QUEST_REWDC_006;
	qstRewdFuncC[7] = &QUEST_REWDC_007;
	qstRewdFuncC[8] = &QUEST_REWDC_008;
	qstRewdFuncC[9] = &QUEST_REWDC_009;
	qstRewdFuncC[10] = &QUEST_REWDC_010;
	qstRewdFuncC[11] = &QUEST_REWDC_011;
	qstRewdFuncC[12] = &QUEST_REWDC_012;
	qstRewdFuncC[13] = &QUEST_REWDC_013;
	qstRewdFuncC[14] = &QUEST_REWDC_014;
	qstRewdFuncC[15] = &QUEST_REWDC_015;
	qstRewdFuncC[16] = &QUEST_REWDC_016;
	qstRewdFuncC[17] = &QUEST_REWDC_017;
	qstRewdFuncC[18] = &QUEST_REWDC_018;
	qstRewdFuncC[19] = &QUEST_REWDC_019;
	qstRewdFuncC[20] = &QUEST_REWDC_020;
	qstRewdFuncC[21] = &QUEST_REWDC_021;
	qstRewdFuncC[22] = &QUEST_REWDC_022;
	qstRewdFuncC[23] = &QUEST_REWDC_023;
	qstRewdFuncC[24] = &QUEST_REWDC_024;
	qstRewdFuncC[25] = &QUEST_REWDC_025;
	qstRewdFuncC[26] = &QUEST_REWDC_026;
	qstRewdFuncC[27] = &QUEST_REWDC_027;
	qstRewdFuncC[28] = &QUEST_REWDC_028;
	qstRewdFuncC[29] = &QUEST_REWDC_029;
	qstRewdFuncC[34] = &QUEST_REWDC_034;


	delete stbQuest;
}


//LMA: Abilities
char* CWorldServer::Abilities(int btOp,char* buffer)
{
    switch( btOp )
    {
        case sReputation:
            sprintf(buffer,"Fame (PLAYERS)");
            break;
        case sSkillPoints:
            sprintf(buffer,"Skill Pts (PLAYERS)");
            break;
        case sPvp:
            sprintf(buffer,"PvP (PLAYERS)");
            break;
        case sGender:
            sprintf(buffer,"gender");
            break;
        case sFace:
            sprintf(buffer,"face");
            break;
        case sHair:
            sprintf(buffer,"hair");
            break;
        case sJob:
            sprintf(buffer,"job");
            break;
        case sUnion:
            sprintf(buffer,"union");
            break;
        case 81:    //LMA: Union Points (no break, it's NOT a mistake)...
            sprintf(buffer,"union points01");
            break;
        case 82:
            sprintf(buffer,"union points02");
            break;
        case 83:
            sprintf(buffer,"union points03");
            break;
        case 84:
            sprintf(buffer,"union points04");
            break;
        case 85:
            sprintf(buffer,"union points05");
            break;
        case sStrength:
            sprintf(buffer,"strength");
            break;
        case sDexterity:
            sprintf(buffer,"dexterity");
            break;
        case sIntelligence:
            sprintf(buffer,"intelligence");
            break;
        case sConcentration:
            sprintf(buffer,"concentration");
            break;
        case sCharm:
            sprintf(buffer,"charm");
            break;
        case sSensibility:
            sprintf(buffer,"sensibility");
            break;
        case sLevel:
            sprintf(buffer,"level");
            break;
        case sStatPoints:
            sprintf(buffer,"stat points");
            break;
        case sMoney:
            sprintf(buffer,"money");
            break;
    case sEXPRate:
            sprintf(buffer,"exp rate");
            break;
        default:
            sprintf(buffer,"?%i?",btOp);
            break;
    }


	return buffer;
}

//LMA: Operators
char* CWorldServer::Operators(byte btOp,char* buffer)
{
	switch(btOp)
	{
		case 0:
			sprintf(buffer,"==");
			break;
		case 1:
			sprintf(buffer,">");
			break;
		case 2:
			sprintf(buffer,">=");
			break;
		case 3:
			sprintf(buffer,"<");
			break;
		case 4:
			sprintf(buffer,"<=");
			break;
		case 5:
			sprintf(buffer,"=");
			break;
		case 6:
			sprintf(buffer,"+=");
			break;
		case 7:
			sprintf(buffer,"-=");
			break;
		case 9:
			sprintf(buffer,"++");
			break;
		case 10:
			sprintf(buffer,"!=");
			break;
		default:
			sprintf(buffer,"?%i?",btOp);
			break;
	}


	return buffer;
}


//LMA: We export QSD Conditions here
//void CWorldServer::ExportQSDData(byte* dataorg,int opcode,int size,CQuestTrigger::SQuestDatum* dataraw)
void CWorldServer::ExportQSDData(byte* dataorg,int size,int opcode)
{
    char buffero[20];
    char buffera[20];
    UINT itemtype=0;
    UINT itemnum=0;


    //check Quest.
    if(opcode==0)
    {
        STR_COND_000 * data = (STR_COND_000 *)dataorg;
        LogSp(MSG_INFO,"\t\t\t\t\t CDT %.3i: Check if Quest %i (%s) exists",opcode,data->iQuestSN,GServer->GetSTLQuestByID(data->iQuestSN));
        return;
    }

    //check Quest Variable.
    if(opcode==1)
    {
        STR_COND_001 * data = (STR_COND_001 *)dataorg;

        LogSp(MSG_INFO,"\t\t\t\t\t CDT %.3i: Check %i Player's quest vars",opcode,data->iDataCnt);
        for (int i=0;i<data->iDataCnt;i++)
        {
            dword address = i * sizeof(STR_QUEST_DATA);
            address += (dword)dataorg;
            address += 4;
            STR_QUEST_DATA* curQst = (STR_QUEST_DATA*)address;
            LogSp(MSG_INFO,"\t\t\t\t\t\t |-> Var[0x%04x (%u)][%i] %s %i",curQst->m_wVarTYPE,curQst->m_wVarTYPE,curQst->m_wVarNO,Operators(curQst->btOp,buffero),curQst->nValue);
        }

        return;
    }

    //check Quest Variable (same as opcode 1)
    if(opcode==2)
    {
        STR_COND_002 * data = (STR_COND_002 *)dataorg;

        LogSp(MSG_INFO,"\t\t\t\t\t CDT %.3i: Check %i Player's quest vars",opcode,data->iDataCnt);
        for (int i=0;i<data->iDataCnt;i++)
        {
            dword address = i * sizeof(STR_QUEST_DATA);
            address += (dword)dataorg;
            address += 4;
            STR_QUEST_DATA* curQst = (STR_QUEST_DATA*)address;
            LogSp(MSG_INFO,"\t\t\t\t\t\t |-> Var[0x%04x (%u)][%i] %s %i",curQst->m_wVarTYPE,curQst->m_wVarTYPE,curQst->m_wVarNO,Operators(curQst->btOp,buffero),curQst->nValue);
        }


        return;
    }

    //check Stats
    if(opcode==3)
    {
        STR_COND_003 * data = (STR_COND_003 *)dataorg;
        LogSp(MSG_INFO,"\t\t\t\t\t CDT %.3i: Check %i Player's stats",opcode,data->iDataCnt);

        for(int i = 0; i < data->iDataCnt; i++)
        {
            dword address = i * 0x0C;
            address += (dword)dataorg;
            address += 4;
            STR_ABIL_DATA* curAbil = (STR_ABIL_DATA*)address;
            LogSp(MSG_INFO,"\t\t\t\t\t\t |-> Check Stat %s  %s %i",Abilities(curAbil->iType,buffera),Operators(curAbil->btOp,buffero),curAbil->iValue);
        }


        return;
    }

    //check items.
    if(opcode==4)
    {
        STR_COND_004 * data = (STR_COND_004 *)dataorg;
        LogSp(MSG_INFO,"\t\t\t\t\t CDT %.3i: Check %i Player's items",opcode,data->iDataCnt);

        for(int i = 0; i < data->iDataCnt; i++)
        {
            dword address = i * 0x10;
            address += (dword)dataorg;
            address += 4;
            STR_ITEM_DATA* curItem = (STR_ITEM_DATA*)address;

            itemtype=gi(curItem->uiItemSN,0);
            itemnum=gi(curItem->uiItemSN,1);

            //where not reliable...
            //if(curItem->iWhere!=13)
            if(itemtype!=13)
            {
                if (curItem->uiItemSN==0)
                {
                    LogSp(MSG_INFO,"\t\t\t\t\t\t |-> Check if equipped item in slot %i is %s (0:0)",curItem->iWhere,Operators(curItem->btOp,buffero),curItem->iRequestCnt);
                }
                else
                {
                    LogSp(MSG_INFO,"\t\t\t\t\t\t |-> Check Item %i (%i:%i, %s %s) %s %i, where %i",curItem->uiItemSN,itemtype,itemnum,GServer->GetSTLItemPrefix(itemtype,itemnum),GServer->GetSTLObjNameByID(itemtype,itemnum),Operators(curItem->btOp,buffero),curItem->iRequestCnt,curItem->iWhere);
                }

            }
            else
            {
                LogSp(MSG_INFO,"\t\t\t\t\t\t |-> Check Quest Item %i (%i:%i, %s %s) %s %i, where %i",curItem->uiItemSN,itemtype,itemnum,GServer->GetSTLItemPrefix(itemtype,itemnum),GServer->GetSTLObjNameByID(itemtype,itemnum),Operators(curItem->btOp,buffero),curItem->iRequestCnt,curItem->iWhere);
            }

        }


        return;
    }

    //check party
    if(opcode==5)
    {
        STR_COND_005 * data = (STR_COND_005 *)dataorg;
        LogSp(MSG_INFO,"\t\t\t\t\t CDT %.3i: Check Party btIsLeader %i, iLevel %i, btReversed %i (NOT CODED)",opcode,data->btIsLeader,data->iLevel,data->btReversed);
        return;
    }

    //Near point
    if(opcode==6)
    {
        STR_COND_006 * data = (STR_COND_006 *)dataorg;
        LogSp(MSG_INFO,"\t\t\t\t\t CDT %.3i: Player near point (%.2f,%.2f), map %i (%s), distance < %i",opcode,(float)(data->iX / 100),(float)(data->iY / 100),data->iZoneSN,GServer->GetSTLZoneNameByID(data->iZoneSN),data->iRadius);
        return;
    }

    //world time
    if(opcode==7)
    {
        STR_COND_007 * data = (STR_COND_007 *)dataorg;
        LogSp(MSG_INFO,"\t\t\t\t\t CDT %.3i: World time (NOT CODED, begintime %u, endtime %u)",opcode,data->ulTime,data->ulEndTime);
        return;
    }

    //Quest Time
    if(opcode==8)
    {
        STR_COND_008 * data = (STR_COND_008 *)dataorg;
        LogSp(MSG_INFO,"\t\t\t\t\t CDT %.3i: Check Player's quest time %s %u",opcode,Operators(data->btOp,buffero),data->ulTime);
        return;
    }

    //Check Skill
    if(opcode==9)
    {
        STR_COND_009 * data = (STR_COND_009 *)dataorg;

        if(data->btOp)
        {
            LogSp(MSG_INFO,"\t\t\t\t\t CDT %.3i: Check if skill %i (%s) DOES exist (sn2: %u)",opcode,data->iSkillSN1,GServer->GetSTLSkillByID(data->iSkillSN1),data->iSkillSN2);
        }
        else
        {
            LogSp(MSG_INFO,"\t\t\t\t\t CDT %.3i: Check if skill %i (%s) does NOT exist (sn2: %u)",opcode,data->iSkillSN1,GServer->GetSTLSkillByID(data->iSkillSN1),data->iSkillSN2);
        }


        return;
    }

    //Unknow
    if(opcode==10)
    {
        STR_COND_010 * data = (STR_COND_010 *)dataorg;
        LogSp(MSG_INFO,"\t\t\t\t\t CDT %.3i: Unknown (NOT CODED) lowpc %u, highpc %u",opcode,data->btLowPcnt,data->btHighPcnt);
        return;
    }

    //Object variable
    if(opcode==11)
    {
        STR_COND_011 * data = (STR_COND_011 *)dataorg;
        LogSp(MSG_INFO,"\t\t\t\t\t CDT %.3i: Object variable",opcode);
        LogSp(MSG_INFO,"\t\t\t\t\t\t |-> Check ObjVar[%i] %s %i (btwho %u)",data->nVarNo,Operators(data->btOp,buffero),data->iValue,data->btWho);
        return;
    }

    //Select Object in zone.
    if(opcode==12)
    {
        STR_COND_012 * data = (STR_COND_012 *)dataorg;
        LogSp(MSG_INFO,"\t\t\t\t\t CDT %.3i: Select Object %i in map %i (%s), IFO %i_%i.IFO (CODED FOR MONSTERS/NPC)",opcode,data->iEventID,data->iZone,GServer->GetSTLZoneNameByID(data->iZone),data->iX,data->iY);
        return;
    }

    //Select NPC
    if(opcode==13)
    {
        STR_COND_013 * data = (STR_COND_013 *)dataorg;
        LogSp(MSG_INFO,"\t\t\t\t\t CDT %.3i: Select NPC %i (%s)",opcode,data->iNpcNo,GServer->GetSTLMonsterNameByID(data->iNpcNo));
        return;
    }

    //Check Quest Flag
    if(opcode==14)
    {
        STR_COND_014 * data = (STR_COND_014 *)dataorg;
        LogSp(MSG_INFO,"\t\t\t\t\t CDT %.3i: Check Quest Flag[%i] == %i",opcode,data->nSN,data->btOp);
        return;
    }

    //Unknow
    if(opcode==15)
    {
        STR_COND_015 * data = (STR_COND_015 *)dataorg;
        LogSp(MSG_INFO,"\t\t\t\t\t CDT %.3i: Unknown %u, %u (size %i) (NOT CODED)",opcode,data->nNumber1,data->nNumber2,size);
        return;
    }

    //Zone Time
    if(opcode==16)
    {
        STR_COND_016 * data = (STR_COND_016 *)dataorg;
        LogSp(MSG_INFO,"\t\t\t\t\t CDT %.3i: Zone Time (NOT CODED) (who %u, ultime %u, ulendtime %u)",opcode,data->btWho,data->ulTime,data->ulEndTime);
        return;
    }

    //Compare two NPC & Obj Variables.
    if(opcode==17)
    {
        STR_COND_017 * data = (STR_COND_017 *)dataorg;
        LogSp(MSG_INFO,"\t\t\t\t\t CDT %.3i: Does NPC %i (%s) ObjVar[%i] %s NPC %i %(%s) ObjVar[%i], junk1 %i, junk2 %i",
            opcode,data->NpcVar1.iNpcNo,GServer->GetSTLMonsterNameByID(data->NpcVar1.iNpcNo),data->NpcVar1.nVarNo,
            Operators(data->btOp,buffero),
            data->NpcVar2.iNpcNo,GServer->GetSTLMonsterNameByID(data->NpcVar2.iNpcNo),
            data->NpcVar2.nVarNo,data->NpcVar1.unused,data->NpcVar2.unused);
        return;
    }

    //Time on Date
    if(opcode==18)
    {
        STR_COND_018 * data = (STR_COND_018 *)dataorg;

        if(data->btDate!=0)
        {
            LogSp(MSG_INFO,"\t\t\t\t\t CDT 0%i: Check day == %i and %.2i:%.2i:00<=time<=%.2i:%.2i:00",opcode,data->btDate,data->btHour1,data->btMin1,data->btHour2,data->btMin2);
        }
        else
        {
            LogSp(MSG_INFO,"\t\t\t\t\t CDT 0%i: Check %.2i:%.2i:00<=time<=%.2i:%.2i:00",opcode,data->btHour1,data->btMin1,data->btHour2,data->btMin2);
        }


        return;
    }

    //Time on Day
    if(opcode==19)
    {
        STR_COND_019 * data = (STR_COND_019 *)dataorg;
        LogSp(MSG_INFO,"\t\t\t\t\t CDT %.3i: Time on Day (NOT CODED) (weekday %u, hour1 %u, min1 %u, hour2 %u, min2 %u)",opcode,data->btWeekDay,data->btHour1,data->btMin1,data->btHour2,data->btMin2);
        return;
    }

    //Unknow
    if(opcode==20)
    {
        STR_COND_020 * data = (STR_COND_020 *)dataorg;
        LogSp(MSG_INFO,"\t\t\t\t\t CDT %.3i: Unknown (NOT CODED) (no1 %u, no2 %u)",opcode,data->iNo1,data->iNo2);
        return;
    }

    //Near Selected NPC
    if(opcode==21)
    {
        STR_COND_021 * data = (STR_COND_021 *)dataorg;
        LogSp(MSG_INFO,"\t\t\t\t\t CDT %.3i: Near Selected NPC, range %u (CODED FOR PLAYERS), (type, %u)",opcode,data->iRadius,data->btSelObjType);
        return;
    }

    //Check Server/Channel
    if(opcode==22)
    {
        STR_COND_022 * data = (STR_COND_022 *)dataorg;
        LogSp(MSG_INFO,"\t\t\t\t\t CDT %.3i: Check Server/Channel %u, %u (NOT CODED)",opcode,data->nX,data->nY);
        return;
    }

    //In Clan
    if(opcode==23)
    {
        STR_COND_023 * data = (STR_COND_023 *)dataorg;

        if(data->btReg == 1)
        {
            LogSp(MSG_INFO,"\t\t\t\t\t CDT %.3i: Check if in Clan",opcode);
        }
        else if (data->btReg == 0)
        {
            LogSp(MSG_INFO,"\t\t\t\t\t CDT %.3i: Check if NOT in Clan",opcode);
        }
        else
        {
            LogSp(MSG_INFO,"\t\t\t\t\t CDT %.3i: ?%i? Clan",opcode,data->btReg);
        }


        return;
    }

    //Clan Position
    if(opcode==24)
    {
        STR_COND_024 * data = (STR_COND_024 *)dataorg;
        char buffer[20];

        switch(data->btOP)
        {
            case 0:
                sprintf(buffer,"==");
                 break;
            case 1:
                sprintf(buffer,">");
                break;
            case 2:
                sprintf(buffer,">=");
                break;
            case 3:
                sprintf(buffer,"<");
                break;
            case 4:
                sprintf(buffer,"<=");
                break;
            case 10:
                sprintf(buffer,"!=");
                break;
            default:
                sprintf(buffer,"?%i?",data->btOP);
                break;
        }

        LogSp(MSG_INFO,"\t\t\t\t\t CDT %.3i: Check if Clan Position is %s %i",opcode,buffer,data->nPOS);
        return;
    }

    //Clan Contribution
    if(opcode==25)
    {
        STR_COND_025 * data = (STR_COND_025 *)dataorg;
        LogSp(MSG_INFO,"\t\t\t\t\t CDT %.3i: Clan Contribution (NOT CODED) (ncont %u, op %u)",opcode,data->nCONT,data->btOP);
        return;
    }

    //Clan Grade
    if(opcode==26)
    {
        STR_COND_026 * data = (STR_COND_026 *)dataorg;
        char buffer[20];

        switch(data->btOP)
        {
            case 0:
                sprintf(buffer,"==");
                 break;
            case 1:
                sprintf(buffer,">");
                break;
            case 2:
                sprintf(buffer,">=");
                break;
            case 3:
                sprintf(buffer,"<");
                break;
            case 4:
                sprintf(buffer,"<=");
                break;
            case 10:
                sprintf(buffer,"!=");
                break;
            default:
                sprintf(buffer,"?%i?",data->btOP);
                break;
        }

        LogSp(MSG_INFO,"\t\t\t\t\t CDT %.3i: Check if Clan Grade is %s %i",opcode,buffer,data->nGRD);

        return;
    }

    //Check Clan Points amount.
   if(opcode==27)
    {
        STR_COND_027 * data = (STR_COND_027 *)dataorg;
        char buffer[20];

        switch(data->btOP)
        {
            case 0:
                sprintf(buffer,"==");
                 break;
            case 1:
                sprintf(buffer,">");
                break;
            case 2:
                sprintf(buffer,">=");
                break;
            case 3:
                sprintf(buffer,"<");
                break;
            case 4:
                sprintf(buffer,"<=");
                break;
            case 10:
                sprintf(buffer,"!=");
                break;
            default:
                sprintf(buffer,"?%i?",data->btOP);
                break;
        }

        LogSp(MSG_INFO,"\t\t\t\t\t CDT %.3i: Check if Clan Points is %s %i",opcode,buffer,data->nPOINT);

        return;
    }


    //Clan Grade
    if(opcode==28)
    {
        STR_COND_028 * data = (STR_COND_028 *)dataorg;
        LogSp(MSG_INFO,"\t\t\t\t\t CDT %.3i: Clan Grade (NOT CODED) (money %u, op %u)",opcode,data->iMONEY,data->btOP);
        return;
    }

    //Clan Members
    if(opcode==29)
    {
        STR_COND_029 * data = (STR_COND_029 *)dataorg;
        LogSp(MSG_INFO,"\t\t\t\t\t CDT %.3i: Clan Members (NOT CODED) (membercnt %u, op %u)",opcode,data->nMemberCNT,data->btOP);
        return;
    }

    //Clan Skills
    if(opcode==30)
    {
        STR_COND_030 * data = (STR_COND_030 *)dataorg;
        LogSp(MSG_INFO,"\t\t\t\t\t CDT %.3i: Clan Skills (NOT CODED) (skiil1 %u, skill2 %u, op %u)",opcode,data->nSkill1,data->nSkill2,data->btOP);
        return;
    }

    //Really unknown :)
    LogSp(MSG_WARNING,"\t\t\t\t\t CDT %.3i: Impossible to export QSD opcode, size %u",opcode,size-8);


    return;
}



//LMA: exporting QSD Actions
void CWorldServer::ExportQSDDataA(byte* dataorg,int size,int opcode)
{
    char buffero[20];
    char buffera[20];
    char buffer[100];
    UINT itemtype=0;
    UINT itemnum=0;


    //Update quest
    if(opcode==0)
    {
        STR_REWD_000 * data = (STR_REWD_000 *)dataorg;

        //0 remove, 1 start, 2 replace quest keep items, 3 replace quest delete items, 4 select
        switch(data->btOp)
        {
            case 0:
            {
                sprintf(buffer,"delete");
            }
            break;
            case 1:
            {
                sprintf(buffer,"start");
            }
            break;
            case 2:
            {
                sprintf(buffer,"replace (keep items)");
            }
            break;
            case 3:
            {
                sprintf(buffer,"replace (delete items)");
            }
            break;
            case 4:
            {
                sprintf(buffer,"select");
            }
            break;
            default:
            {
                sprintf(buffer,"?%i?",data->btOp);
            }
            break;
        }

        LogSp(MSG_INFO,"\t\t\t\t\t ACT %.3i: %s quest %i (%s)",opcode,buffer,data->iQuestSN,GServer->GetSTLQuestByID(data->iQuestSN));


        return;
    }

    //Update quest items
    if(opcode==1)
    {
        STR_REWD_001 * data = (STR_REWD_001 *)dataorg;

        //0 remove, 1 start, 2 replace quest keep items, 3 replace quest delete items, 4 select
        switch(data->btOp)
        {
            case 0:
            {
                sprintf(buffer,"delete");
            }
            break;
            case 1:
            {
                sprintf(buffer,"add");
            }
            break;
            default:
            {
                sprintf(buffer,"?%i?",data->btOp);
            }
            break;
        }

        itemtype=gi(data->uiItemSN,0);
        itemnum=gi(data->uiItemSN,1);

        LogSp(MSG_INFO,"\t\t\t\t\t ACT %.3i: current quest: %s %i items %u (%i:%i, %s %s), (partyop %u)",opcode,buffer,data->nDupCNT,data->uiItemSN,itemtype,itemnum,GServer->GetSTLItemPrefix(itemtype,itemnum),GServer->GetSTLObjNameByID(itemtype,itemnum),data->btPartyOpt);


        return;
    }

    //Set quest variable
    if(opcode==2)
    {
        STR_REWD_002 * data = (STR_REWD_002 *)dataorg;

        LogSp(MSG_INFO,"\t\t\t\t\t ACT %.3i: set %i Quest Variables",opcode,data->iDataCnt);

        for(dword i = 0; i < data->iDataCnt; i++)
        {
            dword address = i * sizeof(STR_QUEST_DATA);
            address += (dword)dataorg;
            address += 4;
            STR_QUEST_DATA* curQst = (STR_QUEST_DATA*)address;
            LogSp(MSG_INFO,"\t\t\t\t\t\t |-> Set Quest Var[0x%04x (%u)][%i] %s %i",curQst->m_wVarTYPE,curQst->m_wVarTYPE,curQst->m_wVarNO,Operators(curQst->btOp,buffero),curQst->nValue);
        }


        return;
    }


    //Udapte Stats
    if(opcode==3)
    {
        STR_REWD_003 * data = (STR_REWD_003 *)dataorg;
        LogSp(MSG_INFO,"\t\t\t\t\t ACT %.3i: Update %i Stats",opcode,data->iDataCnt);

        for(dword i = 0; i < data->iDataCnt; i++)
        {
            dword address = i * 0x0C;
            address += (dword)dataorg;
            address += 4;
            STR_ABIL_DATA* curAbil = (STR_ABIL_DATA*)address;
            LogSp(MSG_INFO,"\t\t\t\t\t\t |-> Set Stat %s  %s %i",Abilities(curAbil->iType,buffera),Operators(curAbil->btOp,buffero),curAbil->iValue);
        }


        return;
    }

    //Set quest variable
    if(opcode==4)
    {
        STR_REWD_004 * data = (STR_REWD_004 *)dataorg;

        LogSp(MSG_INFO,"\t\t\t\t\t ACT %.3i: set %i Quest Variables",opcode,data->iDataCnt);

        for(dword i = 0; i < data->iDataCnt; i++)
        {
            dword address = i * sizeof(STR_QUEST_DATA);
            address += (dword)dataorg;
            address += 4;
            STR_QUEST_DATA* curQst = (STR_QUEST_DATA*)address;
            LogSp(MSG_INFO,"\t\t\t\t\t\t |-> Set Quest Var[0x%04x (%u)][%i] %s %i",curQst->m_wVarTYPE,curQst->m_wVarTYPE,curQst->m_wVarNO,Operators(curQst->btOp,buffero),curQst->nValue);
        }


        return;
    }

    //Give reward
    if(opcode==5)
    {
        STR_REWD_005 * data = (STR_REWD_005 *)dataorg;

        switch(data->btTarget)
        {
            case 0://EXP
                {
                    LogSp(MSG_INFO,"\t\t\t\t\t ACT %.3i: Give %u Exp, (partyop %u, itemopt %u, nPartyOpt %u)",opcode,data->iValue,data->btPartyOpt,data->nItemOpt,data->nPartyOpt);
                }
                break;
            case 1://Zuly
                {
                    LogSp(MSG_INFO,"\t\t\t\t\t ACT %.3i: Give %u Zuly, (partyop %u, itemopt %u, nPartyOpt %u)",opcode,data->iValue,data->btPartyOpt,data->nItemOpt,data->nPartyOpt);
                }
                break;
            case 2://Item
                {
                    CItem nItem;
                    itemtype=gi(data->iItemSN,0);
                    itemnum=gi(data->iItemSN,1);

                    nItem.itemtype = itemtype;
                    nItem.itemnum = itemnum;

                    if(nItem.IsStackable())
                    {
                        LogSp(MSG_INFO,"\t\t\t\t\t ACT %.3i: Give %i (+formula()) item %u (%i:%i, %s %s), (partyop %u, itemopt %u, nPartyOpt %u)",opcode,data->iValue,data->iItemSN,nItem.itemtype,nItem.itemnum,GServer->GetSTLItemPrefix(itemtype,itemnum),GServer->GetSTLObjNameByID(nItem.itemtype,nItem.itemnum),data->btPartyOpt,data->nItemOpt,data->nPartyOpt);
                    }
                    else
                    {
                        LogSp(MSG_INFO,"\t\t\t\t\t ACT %.3i: Give 1 item %u (%i:%i, %s %s), (partyop %u, itemopt %u, nPartyOpt %u)",opcode,data->iItemSN,nItem.itemtype,nItem.itemnum,GServer->GetSTLItemPrefix(itemtype,itemnum),GServer->GetSTLObjNameByID(nItem.itemtype,nItem.itemnum),data->btPartyOpt,data->nItemOpt,data->nPartyOpt);
                    }

                }
                break;
            default:
            {
                LogSp(MSG_INFO,"\t\t\t\t\t ACT %.3i: Give Unknown reward %i, item %u, count / value %u, (partyop %u, itemopt %u, nPartyOpt %u)",opcode,data->btTarget,data->iItemSN,data->iValue,data->btPartyOpt,data->nItemOpt,data->nPartyOpt);
            }
            break;

        }


        return;
    }

    //restore HP / MP
    if(opcode==6)
    {
        STR_REWD_006 * data = (STR_REWD_006 *)dataorg;
        LogSp(MSG_INFO,"\t\t\t\t\t ACT %.3i: Restore %i%% HP, %i%% MP (partyop %u)",opcode,data->iPercentOfHP, data->iPercentOfMP,data->btPartyOpt);
        return;
    }

    //teleport
    if(opcode==7)
    {
        STR_REWD_007 * data = (STR_REWD_007 *)dataorg;
        if(data->btPartyOpt!=0)
        {
            LogSp(MSG_INFO,"\t\t\t\t\t ACT %.3i: Teleport to map %i (%s) at (%.2f,%.2f), with party members. (partyop %u)",opcode,data->iZoneSN,GServer->GetSTLZoneNameByID(data->iZoneSN),(float)(data->iX/100),(float)(data->iY/100),data->btPartyOpt);
        }
        else
        {
            LogSp(MSG_INFO,"\t\t\t\t\t ACT %.3i: Teleport to map %i (%s) at (%.2f,%.2f), (partyop %u)",opcode,data->iZoneSN,GServer->GetSTLZoneNameByID(data->iZoneSN),(float)(data->iX/100),(float)(data->iY/100),data->btPartyOpt);
        }


        return;
    }

    //Spawn monster
    if(opcode==8)
    {
        STR_REWD_008 * data = (STR_REWD_008 *)dataorg;

        if(data->iX==0||data->iY==0||data->iZoneSN==0)
        {
            LogSp(MSG_INFO,"\t\t\t\t\t ACT %.3i: Spawn %i monsters %i (%s) near me (team %i), (btwho)",opcode,data->iHowMany,data->iMonsterSN,GServer->GetSTLMonsterNameByID(data->iMonsterSN),data->iTeamNo,data->btWho);
        }
        else
        {
            LogSp(MSG_INFO,"\t\t\t\t\t ACT %.3i: Spawn %i monsters %i (%s) to map %i (%s) at (%.2f,%.2f) (team %i), (btwho %u)",opcode,data->iHowMany,data->iMonsterSN,GServer->GetSTLMonsterNameByID(data->iMonsterSN),data->iZoneSN,GServer->GetSTLZoneNameByID(data->iZoneSN),(float)(data->iX/100),(float)(data->iY/100),data->iTeamNo,data->btWho);
        }


        return;
    }

    //Execute Quest Trigger
    if(opcode==9)
    {
        STR_REWD_009 * data = (STR_REWD_009 *)dataorg;
        char* tempName = reinterpret_cast<char*>(&data->szNextTriggerSN) - 2;
        dword hash = MakeStrHash(tempName);
        LogSp(MSG_INFO,"\t\t\t\t\t ACT %.3i: Execute Quest Trigger %s (%u)",opcode,tempName,hash);
        return;
    }

    //Reset Stats.
    if(opcode==10)
    {
        LogSp(MSG_INFO,"\t\t\t\t\t ACT %.3i: Reset Stats to default values",opcode);
        return;
    }

    //Update Object Var..
    if(opcode==11)
    {
        STR_REWD_011 * data = (STR_REWD_011 *)dataorg;
        LogSp(MSG_INFO,"\t\t\t\t\t ACT %.3i: Update variable",opcode);
        if(data->btWho==0)
        {
            LogSp(MSG_INFO,"\t\t\t\t\t\t |-> Update ObjVar[%i] %s %i, who=%i (NPC)",data->nVarNo,Operators(data->btOp,buffero),data->iValue,data->btWho);
        }
        else if (data->btWho==1)
        {
            LogSp(MSG_INFO,"\t\t\t\t\t\t |-> Update ObjVar[%i] %s %i, who=%i (Event Object)",data->nVarNo,Operators(data->btOp,buffero),data->iValue,data->btWho);
        }
        else
        {
            LogSp(MSG_INFO,"\t\t\t\t\t\t |-> Update ObjVar[%i] %s %i, who=?(%i)?",data->nVarNo,Operators(data->btOp,buffero),data->iValue,data->btWho);
        }


        return;
    }

    //NPC Speak...
    if(opcode==12)
    {
        STR_REWD_012 * data = (STR_REWD_012 *)dataorg;
        LogSp(MSG_INFO,"\t\t\t\t\t ACT %.3i: NPC Speak (NOT CODED for Client) (CODED for Monsters / NPC)",opcode);

        if(data->btMsgType == 1)
        {
            LogSp(MSG_INFO,"\t\t\t\t\t\t |-> NPC Shouts LTB string %i, %s",data->iStrID,GServer->LtbstringQSD[data->iStrID]->LTBstring);
        }
        else if(data->btMsgType == 2)
        {
            LogSp(MSG_INFO,"\t\t\t\t\t\t |-> NPC Announces LTB string %i, %s",data->iStrID,GServer->LtbstringQSD[data->iStrID]->LTBstring);
        }
        else
        {
            LogSp(MSG_INFO,"\t\t\t\t\t\t |-> NPC ?(%i)? LTB string %i, %s",data->btMsgType,data->iStrID,GServer->LtbstringQSD[data->iStrID]->LTBstring);
        }


        return;
    }

    //Unknown (execute quest trigger?)
    if(opcode==13)
    {
        STR_REWD_013 * data = (STR_REWD_013 *)dataorg;
        char* tempName = reinterpret_cast<char*>(&data->szTriggerName) - 2;
        dword hash = MakeStrHash(tempName);
        LogSp(MSG_INFO,"\t\t\t\t\t ACT %.3i: Execute Quest Trigger %s (%u) (who %u, isec %u, nexttrigger %u)",opcode,tempName,hash,data->btWho,data->iSec,data->m_HashNextTrigger);

        return;
    }

    //Learn Skill
    if(opcode==14)
    {
        STR_REWD_014 * data = (STR_REWD_014 *)dataorg;
        LogSp(MSG_INFO,"\t\t\t\t\t ACT %.3i: Learn Skill %i (%s), (op %u)",opcode,data->iSkillNo,GServer->GetSTLSkillByID(data->iSkillNo),data->btOp);
        return;
    }

    //Set Quest Flag
    if(opcode==15)
    {
        STR_REWD_015 * data = (STR_REWD_015 *)dataorg;
        LogSp(MSG_INFO,"\t\t\t\t\t ACT %.3i: Set Quest Flag[%u]=%i",opcode,data->nSN,data->btOp);
        return;
    }

    //Unknown...
    if(opcode==16)
    {
        STR_REWD_016 * data = (STR_REWD_016 *)dataorg;
        LogSp(MSG_INFO,"\t\t\t\t\t ACT %.3i: Unknown (NOT CODED), (op %u)",opcode,data->nGroupSN);
        return;
    }

    //reset all quest flags...
    if(opcode==17)
    {
        LogSp(MSG_INFO,"\t\t\t\t\t ACT %.3i: Reset All Quest Flags",opcode);
        return;
    }

    //Send Announcment...
    if(opcode==18)
    {
        STR_REWD_018 * data = (STR_REWD_018 *)dataorg;
        LogSp(MSG_INFO,"\t\t\t\t\t ACT %.3i: Send Announcment (NOT CODED), (strid %u, ncnt %u)",opcode,data->iStrID,data->nCnt);
        return;
    }

    //Execute Quest Trigger in Other Map...
    if(opcode==19)
    {
        STR_REWD_019 * data = (STR_REWD_019 *)dataorg;
        char* tempName = reinterpret_cast<char*>(&data->TriggerName) - 2;
        dword hash = MakeStrHash(tempName);
        LogSp(MSG_INFO,"\t\t\t\t\t ACT %.3i: Execute Quest %s (hash %u) in Other Map %i (%s) for team %i (NOT CODED for Client) (CODED for Monsters / NPC), (triggerlength %u)",opcode,tempName,hash,data->nZoneNo,GServer->GetSTLZoneNameByID(data->nZoneNo),data->nTeamNo,data->nTriggerLength);
        return;
    }

    //PvP Status...
    if(opcode==20)
    {
        STR_REWD_020 * data = (STR_REWD_020 *)dataorg;
        LogSp(MSG_INFO,"\t\t\t\t\t ACT %.3i: PvP Status set to %i (for maps mainly) (CODED FOR PLAYERS)",opcode,data->btNoType);
        return;
    }

    //Set Respawn Position...
    if(opcode==21)
    {
        STR_REWD_021 * data = (STR_REWD_021 *)dataorg;
        float X=(float)(data->iX)/((float)100);
        float Y=(float)(data->iY)/((float)100);
        LogSp(MSG_INFO,"\t\t\t\t\t ACT %.3i: Set Respawn Position to current map [%.2f;%.2f].",opcode,X,Y);
        return;
    }

    //Unknown...
    if(opcode==22)
    {
        STR_REWD_022 * data = (STR_REWD_022 *)dataorg;
        LogSp(MSG_INFO,"\t\t\t\t\t ACT %.3i: Unknown (NOT CODED), (op %u)",opcode,data->btOp);
        return;
    }

    //Raise Clan Grade
    if(opcode==23)
    {
        LogSp(MSG_INFO,"\t\t\t\t\t ACT %.3i: Raise clan grade (+1) (NOT FULLY CODED) ",opcode);
        return;
    }

    //Clan Money...
    if(opcode==24)
    {
        STR_REWD_024 * data = (STR_REWD_024 *)dataorg;
        LogSp(MSG_INFO,"\t\t\t\t\t ACT %.3i: Clan Money btop=%i, money %i (NOT CODED)",opcode,data->btOP,data->iMONEY);
        return;
    }

    //Clan Points...
    if(opcode==25)
    {
        STR_REWD_025 * data = (STR_REWD_025 *)dataorg;
        LogSp(MSG_INFO,"\t\t\t\t\t ACT %.3i: Clan Points %s %i",opcode,Operators(data->btOP,buffero),data->nPOINT);
        return;
    }

    //Clan Skill...
    if(opcode==26)
    {
        STR_REWD_026 * data = (STR_REWD_026 *)dataorg;
        LogSp(MSG_INFO,"\t\t\t\t\t ACT %.3i: Clan Skill (NOT CODED), (skill %u,op %u)",opcode,data->nSkillNo,data->btOP);
        return;
    }

    //Clan Contribution...
    if(opcode==27)
    {
        STR_REWD_027 * data = (STR_REWD_027 *)dataorg;
        LogSp(MSG_INFO,"\t\t\t\t\t ACT %.3i: Clan Contribution (NOT CODED), (cont %u,op %u)",opcode,data->nCONT,data->btOP);
        return;
    }

    //Clan teleport
    if(opcode==28)
    {
        STR_REWD_028 * data = (STR_REWD_028 *)dataorg;

        LogSp(MSG_INFO,"\t\t\t\t\t ACT %.3i: Teleport all me and my clan members to map %i at (%.2f,%.2f) random range %i ",opcode,data->nZoneNo,(float)(data->iX/100),(float)(data->iY/100),data->iRange);
        return;
    }

    //LMA: Lua?
    if(opcode==29)
    {
        STR_REWD_029 * data = (STR_REWD_029 *)dataorg;
        char* tempName = reinterpret_cast<char*>(&data->LuaName)-2;
        LogSp(MSG_INFO,"\t\t\t\t\t ACT %.3i: execute Lua %s (unk1=%u, unk2=%u) (NOT CODED, CLIENT SIDE ONLY?)",opcode,tempName,data->unk1,data->unk2);
        return;
    }

    //Unspawn a NPC
    if(opcode==34)
    {
        STR_REWD_034 * data = (STR_REWD_034 *)dataorg;
        LogSp(MSG_INFO,"\t\t\t\t\t ACT %.3i: Removing previously selected NPC. (op %u)",opcode,data->btOP);
        return;
    }

    //Really unknown :)
    LogSp(MSG_WARNING,"\t\t\t\t\t ACT %.3i: Impossible to export QSD opcode, size %u",opcode,size-8);


    return;
}


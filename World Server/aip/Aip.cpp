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

void CWorldServer::ReadAIP(strings path, dword index)
{
	//PY: This whole function is bullshit I'm commenting it out and replacing it with something that makes sense
	/*
    //LMA: mass exporter.
    bool lma_export=false;
    if(Config.massexport)
        lma_export=true;

    char titlegen[20];

	CRoseFile* fh = new CRoseFile(path, FM_READ | FM_BINARY);
	if(fh->IsOpen())
	{ // goto done;
        Log(MSG_LOAD, "Loading %s", path);

		dword BlockCount = fh->Get<dword>();
	    dword mintime = fh->Get<dword>();
		dword mindamage = fh->Get<dword>();

        if(lma_export)
        {
            LogSp(MSG_INFO, " ");
            LogSp(MSG_INFO, " ");
            if(NPC_AIP.find(index)!=NPC_AIP.end())
            {
                //LogSp(MSG_INFO, "Exporting %s (AIP index %i, NPC %i (%s))",path,index,NPC_AIP[index],GetSTLMonsterNameByID(NPC_AIP[index]));

                if(NPC_AIP[index].size()==1)
                {
                    LogSp(MSG_INFO, "Exporting %s, timing %i sec (AIP index %i for NPC/monster %i (%s) )",path,mintime,index,NPC_AIP[index].at(0),GetSTLMonsterNameByID(NPC_AIP[index].at(0)));
                }
                else
                {
                    LogSp(MSG_INFO, "Exporting %s, timing %i sec (AIP index %i for %i monsters:",path,mintime,index,NPC_AIP[index].size());

                    for(int k=0;k<NPC_AIP[index].size();k++)
                    {
                        if(k!=(NPC_AIP[index].size()-1))
                        {
                            LogSp(MSG_INFO, "\t-> NPC/monster %i (%s),",NPC_AIP[index].at(k),GetSTLMonsterNameByID(NPC_AIP[index].at(k)));
                        }
                        else
                        {
                            LogSp(MSG_INFO, "\t-> NPC/monster %i (%s))",NPC_AIP[index].at(k),GetSTLMonsterNameByID(NPC_AIP[index].at(k)));
                        }
                    }
                }
            }
            else
            {
                LogSp(MSG_INFO, "Exporting %s (AIP index %i, not used by NPC/monsters?)",path,index);
            }
        }

		dword titlestringlength = fh->Get<dword>();
		strings title = new char[titlestringlength+1];
		strings title2 = new char[32+1];
		fh->Read(title, titlestringlength, 1);
		for(dword i = 0; i < BlockCount; i++)
	    {
	        titlestringlength = 32;
	        fh->Read(title2, titlestringlength, 1);//on spawn-idle-attack-attacked-killed target-dead
			dword RecordCount = fh->Get<dword>();

            if(lma_export)
            {
                //Log(MSG_INFO, "--%s", title2);
                bool extra_rl=true;
                switch(i)
                {
                    case 0:
                    {
                        extra_rl=false;
                        sprintf(titlegen,"On Spawn");
                    }
                    break;
                    case 1:
                    {
                        sprintf(titlegen,"Idle");
                    }
                    break;
                    case 2:
                    {
                        sprintf(titlegen,"In Combat");
                    }
                    break;
                    case 3:
                    {
                        sprintf(titlegen,"Is attacked");
                    }
                    break;
                    case 4:
                    {
                        sprintf(titlegen,"Killed enemy");
                    }
                    break;
                    case 5:
                    {
                        sprintf(titlegen,"Dead");
                    }
                    break;
                    default:
                    {
                        sprintf(titlegen,"??Unknown??");
                    }
                    break;

                }

                if(extra_rl)
                    LogSp(MSG_INFO, "\t ");

                if(RecordCount==0)
                {
                    LogSp(MSG_INFO, "\t %s :: 0 Block",titlegen);
                }
                else if(RecordCount==1)
                {
                    LogSp(MSG_INFO, "\t %s :: 1 Block",titlegen);
                }
                else
                {
                    LogSp(MSG_INFO, "\t %s :: %i Blocks",titlegen,RecordCount);
                }

                LogSp(MSG_INFO, "\t {");
            }

	    	for(dword j = 0; j < RecordCount; j++)
	        {
	            if(lma_export)
	            {
	                if(j>0)
                        LogSp(MSG_INFO, "\t\t ");
	                LogSp(MSG_INFO, "\t\t Block %i / %i", j+1,RecordCount);
	                LogSp(MSG_INFO, "\t\t {");
	            }

	            CAip* script = new CAip();
				fh->Read(title2, titlestringlength, 1);// 00 ==============================
				script->ConditionCount = fh->Get<dword>();
				if(script->ConditionCount > 0)
	            {
                    if(lma_export)
                    {
                        if(script->ConditionCount==1)
                        {
                            LogSp(MSG_INFO, "\t\t\t IF (1 condition)");
                        }
                        else
                        {
                            LogSp(MSG_INFO, "\t\t\t IF (%i conditions)",script->ConditionCount);
                        }

                        LogSp(MSG_INFO, "\t\t\t {");
                    }

					script->Conditions = new CAip::SAipDatum*[script->ConditionCount];
					for(dword k = 0; k < script->ConditionCount; k++)
	                {
						CAip::SAipDatum* data = new CAip::SAipDatum();
						data->size = fh->Get<dword>();
						data->opcode = fh->Get<dword>() - 0x04000001;
						data->data = new byte[data->size - 8];
						fh->Read(data->data, data->size - 8, 1);
						script->Conditions[k] = data;

                        //LMA: exporting...
                        if(lma_export)
                        {
                            ExportAipData(data->data,data->size,data->opcode);
                        }
                        //LMA: END exporting...

					}

                    if(lma_export)
                    {
                        LogSp(MSG_INFO, "\t\t\t }");
                    }

				}
	            else
	            {
					script->Conditions = NULL;
                    if(lma_export)
                    {
                        LogSp(MSG_INFO, "\t\t\t IF (0 condition)");
                    }

				}
				script->ActionCount = fh->Get<dword>();

				if(script->ActionCount > 0)
	            {
                    if(lma_export)
                    {
                        if(script->ActionCount==1)
                        {
                            LogSp(MSG_INFO, "\t\t\t THEN (1 action)");
                        }
                        else
                        {
                            LogSp(MSG_INFO, "\t\t\t THEN (%i actions)",script->ActionCount);
                        }

                        LogSp(MSG_INFO, "\t\t\t {");
                    }


                    //LMA: In some special cases, we "invert" the LTB (28) and the executeqsdtrigger (30)
                    script->offset_ltb=-1;
                    script->offset_qsd_trigger=-1;

					script->Actions = new CAip::SAipDatum*[script->ActionCount];
					for(dword k = 0; k < script->ActionCount; k++)
	                {
						CAip::SAipDatum* data = new CAip::SAipDatum();
						data->size = fh->Get<dword>();
						data->opcode = fh->Get<dword>() - 0x0B000001;

						data->data = new byte[data->size - 8];
						fh->Read(data->data, data->size - 8, 1);
						script->Actions[k] = data;

						if (data->opcode==28)
						{
						    script->offset_ltb=k;
						}

						if(data->opcode==30)
						{
						    script->offset_qsd_trigger=k;
						}

						//LMA: export.
						if(lma_export)
						{
						    ExportAipDataA(data->data,data->size,data->opcode);
						}

					}

                    if(lma_export)
                    {
                        LogSp(MSG_INFO, "\t\t\t }");
                    }

				}
	            else
	            {
					script->Actions = NULL;
                    if(lma_export)
                    {
                        LogSp(MSG_INFO, "\t\t\t THEN (0 action) ");
                        LogSp(MSG_INFO, "\t\t\t {}");
                    }

				}

				script->AipID=((index*0x10000)+(i*0x100)+j);
				script->minDamage = mindamage;
				script->minTime = mintime;
				script->recordcount[i] = RecordCount;

				//LMA: testing maps.
				//AipList.push_back( script );
				if(AipListMap.find(script->AipID)!=AipListMap.end())
				{
				    Log(MSG_WARNING,"Script ID %u already done!",script->AipID);
				}
				else
				{
				    //AipListMap[script->AipID]=script;
				    AipListMap.insert ( pair<dword,CAip*>(script->AipID,script) );
				}

				//LMA: Timer time.
				if(AipListTimer.find(index)==AipListTimer.end())
				{
				    AipListTimer[index]=script->minTime*1000;
				    //Log(MSG_INFO,"Script %i has timer %i",index,script->minTime);
				}

                if(lma_export)
                {
                    LogSp(MSG_INFO, "\t\t }");
                }


			}

			if (lma_export)
			{
			    if(RecordCount>0)
                    LogSp(MSG_INFO, "\t");
                LogSp(MSG_INFO, "\t }");
            }

		}
    }
    else
        Log( MSG_ERROR, "AIP File: '%s'", path );
    fh->Close();
	delete fh;
	*/

	//PY: This is how it should be written
CRoseFile* fh = new CRoseFile(path, FM_READ | FM_BINARY);
	
    if(fh->IsOpen())
	{ 
        //Log(MSG_LOAD, "Loading %s                              ", path);
		dword BlockCount = fh->Get<dword>();
	    dword mintime = fh->Get<dword>();
		dword mindamage = fh->Get<dword>();
		dword titlestringlength = fh->Get<dword>();
		strings title = new char[titlestringlength+1];
		strings title2 = new char[32+1];
		fh->Read(title, titlestringlength, 1);
		for(dword i = 0; i < BlockCount; i++)
	    {
	        titlestringlength = 32;
	        fh->Read(title2, titlestringlength, 1);//on spawn-idle-attack-attacked-killed target-dead
			dword RecordCount = fh->Get<dword>();

	    	for(dword j = 0; j < RecordCount; j++)
	        {
	            CAip* script = new CAip();
				fh->Read(title2, titlestringlength, 1);// 00 ==============================
				script->ConditionCount = fh->Get<dword>();
				if(script->ConditionCount > 0)
	            {
					script->Conditions = new CAip::SAipDatum*[script->ConditionCount];
					for(dword k = 0; k < script->ConditionCount; k++)
	                {
						CAip::SAipDatum* data = new CAip::SAipDatum();
						data->size = fh->Get<dword>();
						data->opcode = fh->Get<dword>() - 0x04000001;
						data->data = new byte[data->size - 8];
						fh->Read(data->data, data->size - 8, 1);
						script->Conditions[k] = data;
					}
				}
	            else
	            {
					script->Conditions = NULL;
				}
				script->ActionCount = fh->Get<dword>();

				if(script->ActionCount > 0)
	            {
					script->Actions = new CAip::SAipDatum*[script->ActionCount];
					for(dword k = 0; k < script->ActionCount; k++)
	                {
						CAip::SAipDatum* data = new CAip::SAipDatum();
						data->size = fh->Get<dword>();
						data->opcode = fh->Get<dword>() - 0x0B000001;
						data->data = new byte[data->size - 8];
						fh->Read(data->data, data->size - 8, 1);
						script->Actions[k] = data;
					}
				}
	            else
	            {
					script->Actions = NULL;
				}
				script->AInumber = index;
                script->AipID=((index*0x10000)+(i*0x100)+j);
				script->minDamage = mindamage;
				script->minTime = mintime;
				script->recordcount[i] = RecordCount;
				AipList.push_back( script );
			}
		}
    }
    else
        Log( MSG_ERROR, "AIP File: '%s' failed to load properly", path );
    fh->Close();
	delete fh;
}

void CWorldServer::LoadAipData()
{
    CStrStb* stbAip = NULL;
    stbAip = new CStrStb("3DDATA\\STB\\FILE_AI.STB");
	for(dword i = 1; i < stbAip->Rows(); i++)
	{
		if(stbAip->Data(i, 0))
		{
            GServer->ReadAIP(stbAip->Data(i, 0),i);
		}
	}

    //Log(MSG_INFO, "%i aip entrys",AipList.size());
	Log(MSG_INFO, "Finished loading aip data                              ");

	aiCondFunc[0] = &F_AI_COND_000;
	aiCondFunc[1] = &F_AI_COND_001;
	aiCondFunc[2] = &F_AI_COND_002;
	aiCondFunc[3] = &F_AI_COND_003;
	aiCondFunc[4] = &F_AI_COND_004;
	aiCondFunc[5] = &F_AI_COND_005;
	aiCondFunc[6] = &F_AI_COND_006;
	aiCondFunc[7] = &F_AI_COND_007;
	aiCondFunc[8] = &F_AI_COND_008;
	aiCondFunc[9] = &F_AI_COND_009;
	aiCondFunc[10] = &F_AI_COND_010;
	aiCondFunc[11] = &F_AI_COND_011;
	aiCondFunc[12] = &F_AI_COND_012;
	aiCondFunc[13] = &F_AI_COND_013;
	aiCondFunc[14] = &F_AI_COND_014;
	aiCondFunc[15] = &F_AI_COND_015;
	aiCondFunc[16] = &F_AI_COND_016;
	aiCondFunc[17] = &F_AI_COND_017;
	aiCondFunc[18] = &F_AI_COND_018;
	aiCondFunc[19] = &F_AI_COND_019;
	aiCondFunc[20] = &F_AI_COND_020;
	aiCondFunc[21] = &F_AI_COND_021;
	aiCondFunc[22] = &F_AI_COND_022;
	aiCondFunc[23] = &F_AI_COND_023;
	aiCondFunc[24] = &F_AI_COND_024;
	aiCondFunc[25] = &F_AI_COND_025;
	aiCondFunc[26] = &F_AI_COND_026;
	aiCondFunc[27] = &F_AI_COND_027;
	aiCondFunc[28] = &F_AI_COND_028;
	aiCondFunc[29] = &F_AI_COND_029;
	aiCondFunc[30] = &F_AI_COND_030;
	aiCondFunc[31] = &F_AI_COND_031;

	aiActFunc[0] = &F_AI_ACT_000;
	aiActFunc[1] = &F_AI_ACT_001;
	aiActFunc[2] = &F_AI_ACT_002;
	aiActFunc[3] = &F_AI_ACT_003;
	aiActFunc[4] = &F_AI_ACT_004;
	aiActFunc[5] = &F_AI_ACT_005;
	aiActFunc[6] = &F_AI_ACT_006;
	aiActFunc[7] = &F_AI_ACT_007;
	aiActFunc[8] = &F_AI_ACT_008;
	aiActFunc[9] = &F_AI_ACT_009;
	aiActFunc[10] = &F_AI_ACT_010;
	aiActFunc[11] = &F_AI_ACT_011;
	aiActFunc[12] = &F_AI_ACT_012;
	aiActFunc[13] = &F_AI_ACT_013;
	aiActFunc[14] = &F_AI_ACT_014;
	aiActFunc[15] = &F_AI_ACT_015;
	aiActFunc[16] = &F_AI_ACT_016;
	aiActFunc[17] = &F_AI_ACT_017;
	aiActFunc[18] = &F_AI_ACT_018;
	aiActFunc[19] = &F_AI_ACT_019;
	aiActFunc[20] = &F_AI_ACT_020;
	aiActFunc[21] = &F_AI_ACT_021;
	aiActFunc[22] = &F_AI_ACT_022;
	aiActFunc[23] = &F_AI_ACT_023;
	aiActFunc[24] = &F_AI_ACT_024;
	aiActFunc[25] = &F_AI_ACT_025;
	aiActFunc[26] = &F_AI_ACT_026;
	aiActFunc[27] = &F_AI_ACT_027;
	aiActFunc[28] = &F_AI_ACT_028;
	aiActFunc[29] = &F_AI_ACT_029;
	aiActFunc[30] = &F_AI_ACT_030;
	aiActFunc[31] = &F_AI_ACT_031;
	aiActFunc[32] = &F_AI_ACT_032;
	aiActFunc[33] = &F_AI_ACT_033;
	aiActFunc[34] = &F_AI_ACT_034;
	aiActFunc[35] = &F_AI_ACT_035;
	aiActFunc[36] = &F_AI_ACT_036;
	aiActFunc[37] = &F_AI_ACT_037;

	delete stbAip;
}

//LMA: We export AIP Conditions here
void CWorldServer::ExportAipData(byte* dataorg,int size,int opcode)
{
    UINT itemtype=0;
    UINT itemnum=0;


    //hold or attack
    if(opcode==0)
    {
        LogSp(MSG_INFO,"\t\t\t\t\t CDT %.3i: Hold or attack, (NOT CODED)",opcode);
        return;
    }

    //(Damage > iDamage) == true iDamage cRecvOrGive
    if(opcode==1)
    {
        STR_AI_COND_001 * data = (STR_AI_COND_001 *)dataorg;
        if(data->cRecvOrGive == 0)
        {
            LogSp(MSG_INFO,"\t\t\t\t\t CDT %.3i: True IF damagecounter > %u",opcode,data->iDamage);
        }
        else
        {
            LogSp(MSG_INFO,"\t\t\t\t\t CDT %.3i: True IF damagecounter < %u",opcode,data->iDamage);
        }

        return;
    }


    //Check Near (1)
    if(opcode==2)
    {
        STR_AI_COND_002 * data2 = (STR_AI_COND_002 *)dataorg;
        if(data2->btIsAllied == 1)
        {
            LogSp(MSG_INFO,"\t\t\t\t\t CDT %.3i: Check Nb monsters=>%i distance<%i level [%i,%i], (FindChar, NearChar)",opcode,data2->wChrNum,data2->iDistance,data2->nLevelDiff,data2->nLevelDiff2);
        }
        else
        {
            LogSp(MSG_INFO,"\t\t\t\t\t CDT %.3i: Check Nb Players>=%i distance<%i level [%i,%i], (FindChar, NearChar)",opcode,data2->wChrNum,data2->iDistance,data2->nLevelDiff,data2->nLevelDiff2);
        }

        return;
    }

    //Check Distance (1)
    if(opcode==3)
    {
        STR_AI_COND_003 * data = (STR_AI_COND_003 *)dataorg;
        LogSp(MSG_INFO,"\t\t\t\t\t CDT %.3i: Dist current to source > %i",opcode,data->iDistance);
        return;
    }

    //Check Distance (2)
    if(opcode==4)
    {
        STR_AI_COND_004 * data = (STR_AI_COND_004 *)dataorg;
        if(data->cMoreLess == 0)
        {
            LogSp(MSG_INFO,"\t\t\t\t\t CDT %.3i: Dist current to target >= %i",opcode,data->iDistance);
        }
        else
        {
            LogSp(MSG_INFO,"\t\t\t\t\t CDT %.3i: Dist current to target <= %i",opcode,data->iDistance);
        }

        return;
    }


    //Check ability (1)
    if(opcode==5)
    {
        STR_AI_COND_005 * data = (STR_AI_COND_005 *)dataorg;
       char buffer[3];

        if(data->cMoreLess == 0)
        {
            sprintf ( buffer, ">");
        }
        else
        {
            sprintf ( buffer, "<");
        }

        switch(data->cAbType)
        {
            case 0:
                LogSp(MSG_INFO,"\t\t\t\t\t CDT %.3i: Check lvl %s %i",opcode,buffer,data->iDiff);
            break;
            case 1:
                LogSp(MSG_INFO,"\t\t\t\t\t CDT %.3i: Check attack power %s %i",opcode,buffer,data->iDiff);
            break;
            case 2:
                LogSp(MSG_INFO,"\t\t\t\t\t CDT %.3i: Check defense %s %i",opcode,buffer,data->iDiff);
            break;
            case 3:
                LogSp(MSG_INFO,"\t\t\t\t\t CDT %.3i: Check MDEF %s %i",opcode,buffer,data->iDiff);
            break;
            case 4:
                LogSp(MSG_INFO,"\t\t\t\t\t CDT %.3i: Check HP %s %i",opcode,buffer,data->iDiff);
            break;
            default:
                LogSp(MSG_INFO,"\t\t\t\t\t CDT %.3i: Check ?(%i)? %s %i",opcode,data->cAbType,buffer,data->iDiff);
            break;
        }


        return;
    }

    //Check HP
    if(opcode==6)
    {
        STR_AI_COND_006 * data = (STR_AI_COND_006 *)dataorg;
        if(data->cMoreLess == 0)
        {
            LogSp(MSG_INFO,"\t\t\t\t\t CDT %.3i: Check HP >=  %i",opcode,data->wHP);
        }
        else
        {
            LogSp(MSG_INFO,"\t\t\t\t\t CDT %.3i: Check HP <=  %i",opcode,data->wHP);
        }

        return;
    }

    //Random Chance
    if(opcode==7)
    {
        STR_AI_COND_007 * data = (STR_AI_COND_007 *)dataorg;
        LogSp(MSG_INFO,"\t\t\t\t\t CDT %.3i: Random Chance <  %i",opcode,data->cPercent);
        return;
    }


    //Check Near (2)
    if(opcode==8)
    {
        STR_AI_COND_008 * data = (STR_AI_COND_008 *)dataorg;
        if(data->btIsAllied == 1)
        {
            LogSp(MSG_INFO,"\t\t\t\t\t CDT %.3i: Check 1 monster distance<%i level [%u,%u], (FindChar, NearChar)",opcode,data->iDistance,data->nLevelDiff,data->nLevelDiff2);
        }
        else
        {
            LogSp(MSG_INFO,"\t\t\t\t\t CDT %.3i: Check 1 Player distance<%i level [%u,%u], (FindChar, NearChar)",opcode,data->iDistance,data->nLevelDiff,data->nLevelDiff2);
        }

        return;
    }

    //Is there a target?
    if(opcode==9)
    {
        LogSp(MSG_INFO,"\t\t\t\t\t CDT %.3i: Is there a target?",opcode);
        return;
    }

    //Check ability /r target (1)
    if(opcode==10)
    {
        STR_AI_COND_010 * data = (STR_AI_COND_010 *)dataorg;
       char buffer[3];

        if(data->cMoreLess == 0)
        {
            sprintf ( buffer, ">");
        }
        else
        {
            sprintf ( buffer, "<");
        }

        switch(data->cAbType)
        {
            case 0:
                LogSp(MSG_INFO,"\t\t\t\t\t CDT %.3i: Check %s target lvl",opcode,buffer);
            break;
            case 1:
                LogSp(MSG_INFO,"\t\t\t\t\t CDT %.3i: Check %s target attack power",opcode,buffer);
            break;
            case 2:
                LogSp(MSG_INFO,"\t\t\t\t\t CDT %.3i: Check %s target defense",opcode,buffer);
            break;
            case 3:
                LogSp(MSG_INFO,"\t\t\t\t\t CDT %.3i: Check %s target MDEF",opcode,buffer);
            break;
            case 4:
                LogSp(MSG_INFO,"\t\t\t\t\t CDT %.3i: Check %s target HP",opcode,buffer);
            break;
            case 5:
                LogSp(MSG_INFO,"\t\t\t\t\t CDT %.3i: Check %s target Charm (NOT CODED)",opcode,buffer);
            break;
            default:
                LogSp(MSG_INFO,"\t\t\t\t\t CDT %.3i: Check %s target ?(%i)? ",opcode,buffer,data->cAbType);
            break;
        }


        return;
    }

    //Check ability /r target or number (1)
    if(opcode==11)
    {
        STR_AI_COND_011 * data = (STR_AI_COND_011 *)dataorg;
       char buffer[3];

        if(data->cMoreLess == 0)
        {
            sprintf ( buffer, ">=");
        }
        else
        {
            sprintf ( buffer, "<=");
        }

        switch(data->cAbType)
        {
            case 0:
                LogSp(MSG_INFO,"\t\t\t\t\t CDT %.3i: Check %s target lvl (or %i if %i>0)",opcode,buffer,data->iValue,data->iValue);
            break;
            case 1:
                LogSp(MSG_INFO,"\t\t\t\t\t CDT %.3i: Check %s target attack power or (or %i if %i>0)",opcode,buffer,data->iValue,data->iValue);
            break;
            case 2:
                LogSp(MSG_INFO,"\t\t\t\t\t CDT %.3i: Check %s target defense or (or %i if %i>0)",opcode,buffer,data->iValue,data->iValue);
            break;
            case 3:
                LogSp(MSG_INFO,"\t\t\t\t\t CDT %.3i: Check %s target MDEF or (or %i if %i>0)",opcode,buffer,data->iValue,data->iValue);
            break;
            case 4:
                LogSp(MSG_INFO,"\t\t\t\t\t CDT %.3i: Check %s target HP or (or %i if %i>0)",opcode,buffer,data->iValue,data->iValue);
            break;
            default:
                LogSp(MSG_INFO,"\t\t\t\t\t CDT %.3i: Check %s target ?(%i) or (or %i if %i>0)",opcode,buffer,data->cAbType,data->iValue,data->iValue);
            break;
        }


        return;
    }

    //check time
    if(opcode==12)
    {
        STR_AI_COND_012 * data = (STR_AI_COND_012 *)dataorg;
        if (data->cWhen == 1)
        {
            LogSp(MSG_INFO,"\t\t\t\t\t CDT %.3i: True if night in map",opcode);
        }
        else
        {
            LogSp(MSG_INFO,"\t\t\t\t\t CDT %.3i: True if day in map",opcode);
        }

        return;
    }

    //Check Target (1)
    if(opcode==13)
    {
        STR_AI_COND_013 * data = (STR_AI_COND_013 *)dataorg;
        char buffer[20];
        char buffer1[20];
        char buffer2[20];
        if(data->btCheckTarget == 1) //check enemy
        {
            sprintf ( buffer, "enemy");
        }
        else if(data->btCheckTarget == 0) //check self
        {
             sprintf ( buffer, "self");
        }

        if (data->btStatusType == 1) // check for buffs
        {
            sprintf ( buffer1, "buff");
        }
        else  // check for debuffs
        {
            sprintf ( buffer1, "debuff");
        }

        if (data->btHave == 1) //True if HAS buffs
        {
           sprintf ( buffer2, "needs buff");
        }
        else // true if NOT HAS buffs
        {
           sprintf ( buffer2, "NO buffs needed");
        }

        LogSp(MSG_INFO,"\t\t\t\t\t CDT %.3i: Check %s and %s, %s ",opcode,buffer,buffer1,buffer2);

        return;
    }


    //check variable
    if(opcode==14)
    {
        STR_AI_COND_014 * data = (STR_AI_COND_014 *)dataorg;
        char buffer[3];
        switch(data->btOp)
        {
            case 0:
                 sprintf ( buffer, "==");
                 break;
            case 1:
                 sprintf ( buffer, ">");
                 break;
            case 2:
                 sprintf ( buffer, ">=");
                 break;
            case 3:
                 sprintf ( buffer, "<");
                 break;
            case 4:
                 sprintf ( buffer, "<=");
                 break;
            case 10:
                 sprintf ( buffer, "!=");
                 break;
            default:
                 sprintf ( buffer, "??");
                 break;
        }

        if(data->btVarIDX==0)
        {
            LogSp(MSG_INFO,"\t\t\t\t\t CDT %.3i: Check Objvar[%i] (EventID) %s %i",opcode,data->btVarIDX,buffer,data->iValue);
        }
        else
        {
            LogSp(MSG_INFO,"\t\t\t\t\t CDT %.3i: Check Objvar[%i] %s %i",opcode,data->btVarIDX,buffer,data->iValue);
        }

        return;
    }

    //check variable world
    if(opcode==15)
    {
        LogSp(MSG_INFO,"\t\t\t\t\t CDT %.3i: Check WorldVar (NOT CODED)",opcode);
        return;
    }

    //check variable economy
    if(opcode==16)
    {
        LogSp(MSG_INFO,"\t\t\t\t\t CDT %.3i: Check EconomyVar (NOT CODED)",opcode);
        return;
    }

    //select objvar ref
    if(opcode==17)
    {
        STR_AI_COND_017 * data = (STR_AI_COND_017 *)dataorg;
        LogSp(MSG_INFO,"\t\t\t\t\t CDT %.3i: Select NPC %i (%s) for Objvar",opcode,data->iNpcNo,GServer->GetSTLMonsterNameByID(data->iNpcNo));
        return;
    }

    //select distance (summon to summoner)
    if(opcode==18)
    {
        STR_AI_COND_018 * data = (STR_AI_COND_018 *)dataorg;
        char buffer[3];
        switch(data->btOp)
        {
            case 0:
                 sprintf ( buffer, "==");
                 break;
            case 1:
                 sprintf ( buffer, ">");
                 break;
            case 2:
                 sprintf ( buffer, ">=");
                 break;
            case 3:
                 sprintf ( buffer, "<");
                 break;
            case 4:
                 sprintf ( buffer, "<=");
                 break;
            case 10:
                 sprintf ( buffer, "!=");
                 break;
            default:
                 sprintf ( buffer, "??");
                 break;
        }

        LogSp(MSG_INFO,"\t\t\t\t\t CDT %.3i: Check distance summoner to summon %s %i",opcode,buffer,data->iDistance);
        return;
    }

    //check time
    if(opcode==19)
    {
        STR_AI_COND_019 * data = (STR_AI_COND_019 *)dataorg;
        LogSp(MSG_INFO,"\t\t\t\t\t CDT %.3i: Check %i <=time<= %i",opcode,data->ulTime,data->ulEndTime);
        return;
    }

    //Check target /r value
    if(opcode==20)
    {
        STR_AI_COND_020 * data = (STR_AI_COND_020 *)dataorg;
        char buffer[3];
        switch(data->btOp)
        {
            case 0:
                 sprintf ( buffer, "==");
                 break;
            case 1:
                 sprintf ( buffer, ">");
                 break;
            case 2:
                 sprintf ( buffer, ">=");
                 break;
            case 3:
                 sprintf ( buffer, "<");
                 break;
            case 4:
                 sprintf ( buffer, "<=");
                 break;
            case 10:
                 sprintf ( buffer, "!=");
                 break;
            default:
                 sprintf ( buffer, "??");
                 break;
        }

        switch(data->btAbType)
        {
            case 0:
                LogSp(MSG_INFO,"\t\t\t\t\t CDT %.3i: Check target lvl %s %i",opcode,buffer,data->iValue);
            break;
            case 1:
                LogSp(MSG_INFO,"\t\t\t\t\t CDT %.3i: Check target attack power %s %i",opcode,buffer,data->iValue);
            break;
            case 2:
                LogSp(MSG_INFO,"\t\t\t\t\t CDT %.3i: Check target defense %s %i",opcode,buffer,data->iValue);
            break;
            case 3:
                LogSp(MSG_INFO,"\t\t\t\t\t CDT %.3i: Check target MDEF %s %i",opcode,buffer,data->iValue);
            break;
            case 4:
                LogSp(MSG_INFO,"\t\t\t\t\t CDT %.3i: Check target HP %s %i",opcode,buffer,data->iValue);
            break;
            default:
                LogSp(MSG_INFO,"\t\t\t\t\t CDT %.3i: Check target ?(%i)? %s %i",opcode,buffer,data->btAbType,data->iValue);
            break;
        }


        return;
    }

    //test if no owner?
    if(opcode==21)
    {
        LogSp(MSG_INFO,"\t\t\t\t\t CDT %.3i: Check if monster has no owner",opcode);
        return;
    }

    //does my master have a "real" target?
    if(opcode==22)
    {
        LogSp(MSG_INFO,"\t\t\t\t\t CDT %.3i: Check if my master has a real target",opcode);
        return;
    }

    //game map time?
    if(opcode==23)
    {
        LogSp(MSG_INFO,"\t\t\t\t\t CDT %.3i: Game map time (NOT CODED)",opcode);
        return;
    }

    //check time
    if(opcode==24)
    {
        STR_AI_COND_024 * data = (STR_AI_COND_024 *)dataorg;

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

    //Check Weekday Time (5)
    if(opcode==25)
    {
        STR_AI_COND_025 * data = (STR_AI_COND_025 *)dataorg;
        LogSp(MSG_INFO,"\t\t\t\t\t CDT 0%i: Check wDayOfWeek == %i and %.2i:%.2i:00<=time<=%.2i:%.2i:00",opcode,data->btWeekDay,data->btHour1,data->btMin1,data->btHour2,data->btMin2);
        return;
    }

    //check server channel
    if(opcode==26)
    {
        STR_AI_COND_026 * data = (STR_AI_COND_026 *)dataorg;
        LogSp(MSG_INFO,"\t\t\t\t\t CDT %.3i: Check server channel %u, %u (NOT CODED)",opcode,data->nMin,data->nMax);
        return;
    }

    //Check Near Character
    if(opcode==27)
    {
        STR_AI_COND_027 * data = (STR_AI_COND_027 *)dataorg;
        char buffer[3];
        switch(data->btOp)
        {
            case 0:
                 sprintf ( buffer, "==");
                 break;
            case 1:
                 sprintf ( buffer, ">");
                 break;
            case 2:
                 sprintf ( buffer, ">=");
                 break;
            case 3:
                 sprintf ( buffer, "<");
                 break;
            case 4:
                 sprintf ( buffer, "<=");
                 break;
            case 10:
                 sprintf ( buffer, "!=");
                 break;
            default:
                 sprintf ( buffer, "??");
                 break;
        }

        if(data->btIsAllied == 1)
        {
            LogSp(MSG_INFO,"\t\t\t\t\t CDT %.3i: Check Nb monsters %s %i distance<%i level [%i,%i], (FindChar, NearChar)",opcode,buffer,data->wChrNum,data->iDistance,data->nLevelDiff,data->nLevelDiff2);
        }
        else
        {
            LogSp(MSG_INFO,"\t\t\t\t\t CDT %.3i: Check Nb Players %s %i distance<%i level [%i,%i], (FindChar, NearChar)",opcode,buffer,data->wChrNum,data->iDistance,data->nLevelDiff,data->nLevelDiff2);
        }


        return;
    }

    //check variable for this monster
    if(opcode==28)
    {
        STR_AI_COND_028 * data = (STR_AI_COND_028 *)dataorg;
        char buffer[3];
        switch(data->btOp)
        {
            case 0:
                 sprintf ( buffer, "==");
                 break;
            case 1:
                 sprintf ( buffer, ">");
                 break;
            case 2:
                 sprintf ( buffer, ">=");
                 break;
            case 3:
                 sprintf ( buffer, "<");
                 break;
            case 4:
                 sprintf ( buffer, "<=");
                 break;
            case 10:
                 sprintf ( buffer, "!=");
                 break;
            default:
                 sprintf ( buffer, "??");
                 break;
        }

        LogSp(MSG_INFO,"\t\t\t\t\t CDT %.3i: Check monster var[%i] %s %i",opcode,data->nVarIDX,buffer,data->iValue);

        return;
    }

    //Check Target (2)
    if(opcode==29)
    {
        LogSp(MSG_INFO,"\t\t\t\t\t CDT %.3i: Check Target (2) (NOT CODED)",opcode);
        return;
    }

    //Unknown
    //LMA: Timer?
    if(opcode==30)
    {
        STR_AI_COND_030 * data = (STR_AI_COND_030 *)dataorg;
        LogSp(MSG_INFO,"\t\t\t\t\t CDT %.3i: Timer since spawn %u",opcode,data->Timer);
        return;
    }

    //Unknown
    if(opcode==31)
    {
        LogSp(MSG_INFO,"\t\t\t\t\t CDT %.3i: Unknown",opcode);
        return;
    }

    //Really unknown :)
    LogSp(MSG_WARNING,"\t\t\t\t\t CDT %.3i: Impossible to export AIP opcode, size %u",opcode,size-8);


    return;
}


//LMA: We export AIP Actions here
void CWorldServer::ExportAipDataA(byte* dataorg,int size,int opcode)
{
    UINT itemtype=0;
    UINT itemnum=0;


    //Unknown
    if(opcode==0)
    {
        LogSp(MSG_INFO,"\t\t\t\t\t ACT %.3i: Unknown",opcode);
        return;
    }

    //Action
    if(opcode==1)
    {
        STR_AI_ACT_001 * data = (STR_AI_ACT_001 *)dataorg;
        LogSp(MSG_INFO,"\t\t\t\t\t ACT %.3i: Action, (0x781) %i",opcode,data->cAction);
        return;
    }

    //Say LTB string.
    if(opcode==2)
    {
        STR_AI_ACT_002 * data = (STR_AI_ACT_002 *)dataorg;
        LogSp(MSG_INFO,"\t\t\t\t\t ACT %.3i: Say LTB String %i (%s) (NOT CODED but OK since client Side)",opcode,data->iStrID,GServer->Ltbstring[data->iStrID]->LTBstring);
        return;
    }

    //Move
    if(opcode==3)
    {
        STR_AI_ACT_003 * data = (STR_AI_ACT_003 *)dataorg;
        LogSp(MSG_INFO,"\t\t\t\t\t ACT %.3i: Move from current position, distance %i at stance %i",opcode,data->iDistance,data->cSpeed);
        return;
    }

    //Move 2
    if(opcode==4)
    {
        STR_AI_ACT_004 * data = (STR_AI_ACT_004 *)dataorg;
        LogSp(MSG_INFO,"\t\t\t\t\t ACT %.3i: Move from spawn position, distance %i at stance %i",opcode,data->iDistance,data->cSpeed);
        return;
    }

    //Move 3
    if(opcode==5)
    {
        STR_AI_ACT_005 * data = (STR_AI_ACT_005 *)dataorg;
        LogSp(MSG_INFO,"\t\t\t\t\t ACT %.3i: Move from a char's position, distance 2 at stance %i",opcode,data->cSpeed);
        return;
    }

    //attack a player if ok with some stuff...
    if(opcode==6)
    {
        STR_AI_ACT_006 * data = (STR_AI_ACT_006 *)dataorg;
        char buffer[20];
        char buffer1[20];
        if(data->cMoreLess == 0)
        {
            sprintf(buffer,"biggest");
        }
        else
        {
            sprintf(buffer,"lowest");
        }

        switch(data->cAbType)
            {
            case 0: //level
                sprintf(buffer1,"level");
            break;
            case 1: //Attack power
                sprintf(buffer1,"attack power");
            break;
            case 2: //defense
                sprintf(buffer1,"defense");
            break;
            case 3: //Magic Def
                sprintf(buffer1,"MDEF");
            break;
            case 4: // HP
                sprintf(buffer1,"HP");
            break;
            case 5: // Charm
                sprintf(buffer1,"charm");
            break;
        }

        LogSp(MSG_INFO,"\t\t\t\t\t ACT %.3i: Attacks a player within distance %i if player has %s %s",opcode,data->iDistance,buffer, buffer1);
        return;
    }

    //Unknown
    if(opcode==7)
    {
        LogSp(MSG_INFO,"\t\t\t\t\t ACT %.3i: Unknown",opcode);
        return;
    }

    //Move 4
    if(opcode==8)
    {
        STR_AI_ACT_008 * data = (STR_AI_ACT_008 *)dataorg;
        LogSp(MSG_INFO,"\t\t\t\t\t ACT %.3i: Monster moves to %i %% between source and current position at stance %i ",opcode,data->iDistance,data->cSpeed);
        return;
    }

    //Convert to another monster.
    if(opcode==9)
    {
        STR_AI_ACT_009 * data = (STR_AI_ACT_009 *)dataorg;
        LogSp(MSG_INFO,"\t\t\t\t\t ACT %.3i: Converts to monster %i (%s)",opcode,data->wMonster,GServer->GetSTLMonsterNameByID(data->wMonster));
        return;
    }

    //Spawn monsters.
    if(opcode==10)
    {
        STR_AI_ACT_010 * data = (STR_AI_ACT_010 *)dataorg;
        LogSp(MSG_INFO,"\t\t\t\t\t ACT %.3i: Spawn 1 monster %i near me",opcode,data->wMonster,GServer->GetSTLMonsterNameByID(data->wMonster));
        return;
    }

    //Call for backup..
    if(opcode==11)
    {
        STR_AI_ACT_011 * data = (STR_AI_ACT_011 *)dataorg;
        LogSp(MSG_INFO,"\t\t\t\t\t ACT %.3i: Call for backup, range %i max %i monsters",opcode,data->iDistance,data->iNumOfMonster);
        return;
    }

    //Attack.
    if(opcode==12)
    {
        LogSp(MSG_INFO,"\t\t\t\t\t ACT %.3i: Attacks NearChar",opcode);
        return;
    }

    //Attack.
    if(opcode==13)
    {
        STR_AI_ACT_013 * data = (STR_AI_ACT_013 *)dataorg;
        LogSp(MSG_INFO,"\t\t\t\t\t ACT %.3i: Attacks FindChar",opcode);
        return;
    }

   //Unknown
    if(opcode==14)
    {
        LogSp(MSG_INFO,"\t\t\t\t\t ACT %.3i: Unknown",opcode);
        return;
    }

    //retaliates.
    if(opcode==15)
    {
        LogSp(MSG_INFO,"\t\t\t\t\t ACT %.3i: Retaliates against the last player who hit me",opcode);
        return;
    }

    //run away.
    if(opcode==16)
    {
        STR_AI_ACT_016 * data = (STR_AI_ACT_016 *)dataorg;
        LogSp(MSG_INFO,"\t\t\t\t\t ACT %.3i: Runs away, distance %i",opcode,data->iDistance);
        return;
    }

    //drop item.
    if(opcode==17)
    {
        STR_AI_ACT_017 * data17 = (STR_AI_ACT_017 *)dataorg;

        LogSp(MSG_INFO,"\t\t\t\t\t ACT %.3i: Drops 1 item amongst:",opcode);

        for(int k=0;k<6;k++)
        {
            int nItem = data17->items[k];
            if (nItem==0)
                continue;
            if(k==5&&nItem==0xCDCD)
                continue;

            itemtype=gi(nItem,0);
            itemnum=gi(nItem,1);

            LogSp(MSG_INFO,"\t\t\t\t\t\t |-> Item %u : (%i:%i), %s %s",nItem,itemtype,itemnum,GServer->GetSTLItemPrefix(itemtype,itemnum),GServer->GetSTLObjNameByID(itemtype,itemnum));
        }

        return;
    }

   //Unknown
    if(opcode==18)
    {
        LogSp(MSG_INFO,"\t\t\t\t\t ACT %.3i: Unknown",opcode);
        return;
    }

    //Attack (same as 12)
    if(opcode==19)
    {
        LogSp(MSG_INFO,"\t\t\t\t\t ACT %.3i: Attacks NearChar",opcode);
        return;
    }

    //Spawn monster
    if(opcode==20)
    {
        STR_AI_ACT_020 * data = (STR_AI_ACT_020 *)dataorg;

        if(data->btPos == 0)
        {
            LogSp(MSG_INFO,"\t\t\t\t\t ACT %.3i: Spawn monster %i (%s) distance %i ",opcode,data->cMonster,GServer->GetSTLMonsterNameByID(data->cMonster),data->iDistance);
        }
        else if(data->btPos == 1)
        {
            LogSp(MSG_INFO,"\t\t\t\t\t ACT %.3i: Spawn monster %i (%s) distance %i ",opcode,data->cMonster,GServer->GetSTLMonsterNameByID(data->cMonster),data->iDistance);
        }
        else if(data->btPos == 2)
        {
            LogSp(MSG_INFO,"\t\t\t\t\t ACT %.3i: Spawn monster %i (%s) distance %i near target",opcode,data->cMonster,GServer->GetSTLMonsterNameByID(data->cMonster),data->iDistance);
        }
        else
        {
            LogSp(MSG_INFO,"\t\t\t\t\t ACT %.3i: Spawn monster %i (%s) distance %i near ??? (%i)",opcode,data->cMonster,GServer->GetSTLMonsterNameByID(data->cMonster),data->iDistance,data->btPos);
        }

        return;
    }

   //Unknown
    if(opcode==21)
    {
        LogSp(MSG_INFO,"\t\t\t\t\t ACT %.3i: Unknown",opcode);
        return;
    }

   //Unknown
    if(opcode==22)
    {
        LogSp(MSG_INFO,"\t\t\t\t\t ACT %.3i: Unknown",opcode);
        return;
    }

    //ghostseeds and ghosts and player summons commit suicide
    if(opcode==23)
    {
        LogSp(MSG_INFO,"\t\t\t\t\t ACT %.3i: Time to suicide",opcode);
        return;
    }

    //Do Skill
    if(opcode==24)
    {
        STR_AI_ACT_024 * data = (STR_AI_ACT_024 *)dataorg;
        LogSp(MSG_INFO,"\t\t\t\t\t ACT %.3i: Monster does skill %i (%s) ",opcode,data->nSkill,GServer->GetSTLSkillByID(data->nSkill));
        return;
    }

    //Set Variable.
    if(opcode==25)
    {
        STR_AI_ACT_025 * data = (STR_AI_ACT_025 *)dataorg;
        char buffer[3];
        switch(data->btOp)
        {
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
            default:
                sprintf(buffer,"??");
                break;
        }

        if(data->btVarIDX==0)
        {
            LogSp(MSG_INFO,"\t\t\t\t\t ACT %.3i: Set ObjVar[%i] (eventID) %s %i ",opcode,data->btVarIDX,buffer,data->iValue);
        }
        else
        {
            LogSp(MSG_INFO,"\t\t\t\t\t ACT %.3i: Set ObjVar[%i] %s %i ",opcode,data->btVarIDX,buffer,data->iValue);
        }


        return;
    }

    //set variable 2 (world?)
    if(opcode==26)
    {
        LogSp(MSG_INFO,"\t\t\t\t\t ACT %.3i: Set variable (world?) (NOT CODED)",opcode);
        return;
    }

    //set variable 2 (economy?)
    if(opcode==27)
    {
        LogSp(MSG_INFO,"\t\t\t\t\t ACT %.3i: Set variable (economy?) (NOT CODED)",opcode);
        return;
    }

    //shouts / announces LTB String.
    if(opcode==28)
    {
        STR_AI_ACT_028 * data = (STR_AI_ACT_028 *)dataorg;
        switch(data->btMsgType)
        {
            case 0: //whisper to client
                LogSp(MSG_INFO,"\t\t\t\t\t ACT %.3i: Whisper LTB string %i: %s",opcode,data->iStrID,GServer->Ltbstring[data->iStrID]->LTBstring);
            break;
            case 1: //shout to map
            {
                LogSp(MSG_INFO,"\t\t\t\t\t ACT %.3i: Shouts LTB string %i: %s",opcode,data->iStrID,GServer->Ltbstring[data->iStrID]->LTBstring);
            }
            break;
            case 2: //announce to server.
            {
                LogSp(MSG_INFO,"\t\t\t\t\t ACT %.3i: Announces LTB string %i: %s",opcode,data->iStrID,GServer->Ltbstring[data->iStrID]->LTBstring);
            }
            break;
            default:
            {
                //?
                LogSp(MSG_INFO,"\t\t\t\t\t ACT %.3i: Tells(%i)? LTB string %i: %s (NOT CODED)",opcode,data->btMsgType,data->iStrID,GServer->Ltbstring[data->iStrID]->LTBstring);
            }
            break;
        }


        return;
    }

    //moves to my owner location..
    if(opcode==29)
    {
        LogSp(MSG_INFO,"\t\t\t\t\t ACT %.3i: Moves to owner's location",opcode);
        return;
    }

    //Do Quest Trigger.
    if(opcode==30)
    {
        STR_AI_ACT_030 * data = (STR_AI_ACT_030 *)dataorg;
        char* tempName = reinterpret_cast<char*>(&data->szTrigger) - 2;
        dword hash = MakeStrHash(tempName);
        LogSp(MSG_INFO,"\t\t\t\t\t ACT %.3i: Do quest trigger %s, %u",opcode,tempName,hash);
        return;
    }

    //monster attacks owner's target..
    if(opcode==31)
    {
        LogSp(MSG_INFO,"\t\t\t\t\t ACT %.3i: Monster attacks owner's target",opcode);
        return;
    }

    //Set Zone ?
    if(opcode==32)
    {
        STR_AI_ACT_032 * data = (STR_AI_ACT_032 *)dataorg;
        LogSp(MSG_INFO,"\t\t\t\t\t ACT %.3i: Set Pvp to %i in Zone %i",opcode,data->btOnOff,data->nZoneNo);
        return;
    }

    //Set Zone ?
    if(opcode==33)
    {
        LogSp(MSG_INFO,"\t\t\t\t\t ACT %.3i: Set Zone ? (NOT CODED)",opcode);
        return;
    }

    //Gives Item to caller.
    if(opcode==34)
    {
        STR_AI_ACT_034 * data = (STR_AI_ACT_034 *)dataorg;
        char itemtypes[15] = {4,0,0,0,0,0,0,0,0,0,1,2,2,4,3};

        //LMA: exact way.
        itemtype = GServer->gi(data->nItemNum,0);
        itemnum = GServer->gi(data->nItemNum,1);

        int count = data->nCount;
        int durability=100;

        if( itemtypes[itemtype] == 0 )
        {
            durability = GServer->STB_ITEM[itemtype-1].rows[itemnum][29];
        }

        if (durability==0)
        {
            durability=100;
        }

        LogSp(MSG_INFO,"\t\t\t\t\t ACT %.3i: Gives %i item %i::%i (%s %s), durability %i to master",opcode,count,itemtype,itemnum,GServer->GetSTLItemPrefix(itemtype,itemnum),GServer->GetSTLObjNameByID(itemtype,itemnum),durability);
        return;
    }

    //Set Variable.
    if(opcode==35)
    {
        STR_AI_ACT_035 * data = (STR_AI_ACT_035 *)dataorg;
        char buffer[3];
        switch(data->btOp)
        {
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
            default:
                sprintf(buffer,"?(%i)?",data->btOp);
                break;
        }

        LogSp(MSG_INFO,"\t\t\t\t\t ACT %.3i: Set monster var[%i] %s %i ",opcode,data->nVarIDX,buffer,data->iValue);
        return;
    }

    //spawn monster with a master?
    if(opcode==36)
    {
        STR_AI_ACT_036 * data = (STR_AI_ACT_036 *)dataorg;
        LogSp(MSG_INFO,"\t\t\t\t\t ACT %.3i: Spawn monster %i (%s) with master %i near me",opcode,data->nMonster,GServer->GetSTLMonsterNameByID(data->nMonster),data->btMaster);
        return;
    }

    //spawn monster with btmaster?
    if(opcode==37)
    {
        STR_AI_ACT_037 * data = (STR_AI_ACT_037 *)dataorg;
        if(data->nPos == 0) //spawn monster in a circle with radius iDistance around my current position
        {
            LogSp(MSG_INFO,"\t\t\t\t\t ACT %.3i: spawn monster %i (%s) with master %i near me",opcode,data->nMonster,GServer->GetSTLMonsterNameByID(data->nMonster),data->btMaster);
        }
        else if(data->nPos == 1) //spawn monster in a circle with radius iDistance around my destiny position
        {
            LogSp(MSG_INFO,"\t\t\t\t\t ACT %.3i: spawn monster %i (%s) with master %i near master",opcode,data->nMonster,GServer->GetSTLMonsterNameByID(data->nMonster),data->btMaster);
        }
        else if(data->nPos == 2) //spawn monster in a circle with radius iDistance around my target's current position
        {
            LogSp(MSG_INFO,"\t\t\t\t\t ACT %.3i: spawn monster %i (%s) with master %i near target",opcode,data->nMonster,GServer->GetSTLMonsterNameByID(data->nMonster),data->btMaster);
        }
        else
        {
            LogSp(MSG_INFO,"\t\t\t\t\t ACT %.3i: spawn monster %i (%s) with master %i near ?(%i)?",opcode,data->nMonster,GServer->GetSTLMonsterNameByID(data->nMonster),data->btMaster,data->nPos);
        }


        return;
    }

    //Really unknown :)
    LogSp(MSG_WARNING,"\t\t\t\t\t ACT %.3i: Impossible to export AIP opcode, size %i",opcode,size-8);


    return;
}

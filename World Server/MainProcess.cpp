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
#include "worldserver.h"

// Map Process
PVOID MapProcess( PVOID TS )
{
    bool ok_cont=false;
    UINT loopcount=0;
    clock_t time_skill=0;
    bool only_npc=false;    //LMA: AIP is done by NPC even when no player in map.
    bool only_summon=false;  //LMA: test for summons when no one's around (sad is a summon's life ^_^).
    UINT nb_summons_map=0;  //LMA: test for summons when no one's around (sad is a summon's life ^_^).

    //LMA: temp monster used for NPCs.
    fPoint tempPos;
    tempPos.x=0;
    tempPos.y=0;
    tempPos.z=0;
    CMonster* NPCmonster = new (nothrow) CMonster( tempPos, 0, 0, 0, 0  );

    while(GServer->ServerOnline)
    {
        loopcount++;            //geobot: refresh only every 100 cycles
        if (loopcount<100)
        {
            continue;
        }

        loopcount=0;

        pthread_mutex_lock( &GServer->PlayerMutex );
        pthread_mutex_lock( &GServer->MapMutex );

        for(UINT i=0;i<GServer->MapList.Map.size();i++)
        {
            CMap* map = GServer->MapList.Map.at(i);

            //LMA: test for Union.
            only_npc=false;
            only_summon=false;  //LMA: test for summons when no one's around (sad is a summon's life ^_^).
            nb_summons_map=0;
            if( map->PlayerList.size()<1 )
            {
                only_npc=true;    //LMA: AIP is done by NPC even when no player in map.

                //LMA: doing summons too if there are any in map.
                if(map->nb_summons>0)
                {
                    only_summon=true;
                }
                //continue;
            }

            if (!only_npc||only_summon)
            {
                // Player update //------------------------
                for(UINT j=0;j<map->PlayerList.size();j++)
                {
                    CPlayer* player = map->PlayerList.at(j);
                    if(!player->Session->inGame) continue;

                    if(player->IsDead( ))
                    {
                          player->lastRegenTime=0;
                          player->lastShowTime=0;
                          continue;
                     }

                     //Log(MSG_INFO,"Player %s in map %i, position->map %i",player->CharInfo->charname,map->id,player->Position->Map);

                     #ifdef LMA_SPAWNM
                     //LMA: For use with /spawnmonsters gm command (for drop me).
                     if(player->mdeb!=0&&player->mend!=0&&player->playertime!=0)
                     {
                         if(player->playertime < (UINT)GServer->round((clock( ) - player->lastSpawnUpdate)))
                         {
                             CMap* map = GServer->MapList.Index[player->Position->Map];

                             //let's delete the old one :)
                             if(player->last_monstercid!=0)
                             {
                                CMonster* thismon = GServer->GetMonsterByID(player->last_monstercid, player->Position->Map);
                                if (thismon!=NULL)
                                {
                                    thismon->Stats->HP = -1;
                                    map->DeleteMonster( thismon );
                                }

                                GServer->SendPM(player,"Killing monster %u.",player->last_monstercid);
                                player->ClearObject(player->last_monstercid);

                                /*
                                thismon->Stats->HP = -1;
                                BEGINPACKET( pak, 0x799 );
                                ADDWORD    ( pak, thismon->clientid );
                                ADDWORD    ( pak, thismon->clientid );
                                ADDDWORD   ( pak, thismon->thisnpc->hp*thismon->thisnpc->level );
                                ADDDWORD   ( pak, 16 );
                                GServer->SendToVisible( &pak, thismon );
                                map->DeleteMonster( thismon );
                                */
                                player->playertime=1000;
                                player->last_monstercid=0;
                             }
                             else
                             {
                                 if(player->last_monster==0)
                                 {
                                     player->last_monster=player->mdeb;
                                 }
                                 else
                                 {
                                     player->last_monster++;
                                 }

                                 if (player->last_monster<=player->mend)
                                 {
                                     player->lastSpawnUpdate=clock();
                                    fPoint position;
                                    position.x=player->xx;
                                    position.y=player->yy;
                                    position.z=0;

                                    bool is_ok=false;
                                    CMonster* thismonster=NULL;
                                    if(player->last_monster<GServer->maxNPC)
                                    {
                                        if(GServer->NPCData[player->last_monster]->STLId!=0)
                                        {
                                            is_ok=true;
                                            thismonster=map->AddMonster( player->last_monster, position );
                                            if (thismonster==NULL)
                                            {
                                                is_ok=false;
                                            }

                                        }

                                    }

                                    if (is_ok)
                                    {
                                        GServer->SendPM(player,"Spawning monster %u (%u).",player->last_monster,thismonster->clientid);
                                        player->last_monstercid=thismonster->clientid;
                                        player->playertime=2000;
                                    }
                                    else
                                    {
                                        GServer->SendPM(player,"Monster %u does not exist.",player->last_monster);
                                        player->last_monstercid=0;
                                        player->playertime=1000;
                                    }

                                 }
                                 else
                                 {
                                     GServer->SendPM(player,"End spawning from %u to %u.",player->mdeb,player->mend);
                                     player->mdeb=0;
                                     player->mend=0;
                                 }

                             }

                         }

                     }
                     #endif

                     player->RefreshHPMP();         //LMA HP / MP Jumping
                    if(player->UpdateValues( )) //Does nothing except for rides... equals to true if player isn't on the back seat
                        player->UpdatePosition(false);
                    if(player->IsOnBattle( ))
                        player->DoAttack( );

                    //LMA: not used anymore.
                    //player->CheckItems( );

                    player->RefreshBuff( );
                    player->PlayerHeal( );
                    player->Regeneration( );
                    player->CheckPlayerLevelUP( );

                    #ifdef PYCUSTOM
                    //LMA: define PYCUSTOM in the .h to enable custom events. Rebuild your server then.
                    player->CheckPortal( );  //Custom Events
                    player->CheckEvents( );  //Custom Events
                    #endif

                    player->CheckDoubleEquip(); //LMA: Core fix for double weapon and shield
                    player->CheckZulies( );

                    //Fuel handling.
                    if (player->Status->Stance==DRIVING&&(player->last_fuel>0)&&(clock()-player->last_fuel>60000))
                    {
                      //We kill some fuel every now and then :)
                      player->TakeFuel();
                      player->last_fuel=clock();
                    }

                    //LMA: mileage coupon checks.
                    time_t etime=time(NULL);
                    if(player->no_exp&&(etime>=player->timer_no_exp))
                    {
                      BEGINPACKET( pak, 0x702 );
                      ADDSTRING( pak, "[Mileage] Null Xp vanished.");
                      ADDBYTE( pak, 0 );
                      player->client->SendPacket(&pak);
                      player->timer_no_exp=0;
                      player->no_exp=false;
                    }
                    if(player->bonusxp>1&&(etime>=player->timerxp))
                    {
                      BEGINPACKET( pak, 0x702 );
                      ADDSTRING( pak, "[Mileage] Bonus Xp vanished.");
                      ADDBYTE( pak, 0 );
                      player->client->SendPacket(&pak);
                      player->bonusxp=1;
                      player->timerxp=0;
                      player->wait_validation=0;
                    }
                    if(player->bonusddrop>1&&(etime>=player->timerddrop))
                    {
                      BEGINPACKET( pak, 0x702 );
                      ADDSTRING( pak, "[Mileage] Medal of Fortune vanished.");
                      ADDBYTE( pak, 0 );
                      player->client->SendPacket(&pak);
                      player->bonusddrop=1;
                      player->timerddrop=0;
                      player->wait_validation_ddrop=0;
                    }
                    if(player->bonusstatdrop>1&&(etime>=player->timerstatdrop))
                    {
                      BEGINPACKET( pak, 0x702 );
                      ADDSTRING( pak, "[Mileage] Medal of Excellence vanished.");
                      ADDBYTE( pak, 0 );
                      player->client->SendPacket(&pak);
                      player->bonusstatdrop=1;
                      player->timerstatdrop=0;
                      player->wait_validation_statdrop=0;
                    }
                    if(player->bonusgraydrop>0&&(etime>=player->timergraydrop))
                    {
                      BEGINPACKET( pak, 0x702 );
                      ADDSTRING( pak, "[Mileage] Medal of Retrieval vanished.");
                      ADDBYTE( pak, 0 );
                      player->client->SendPacket(&pak);
                      player->bonusgraydrop=0;
                      player->timergraydrop=0;
                      player->wait_validation_graydrop=0;
                    }
                    if(player->Shop->ShopType>0&&(etime>=player->Shop->mil_shop_time))
                    {
                      BEGINPACKET( pak, 0x702 );
                      ADDSTRING( pak, "[Mileage] Mileage shop expired !");
                      ADDBYTE( pak, 0 );
                      player->client->SendPacket(&pak);
                      player->Shop->ShopType=0;
                      player->Shop->mil_shop_time=0;
                    }
                    if(player->Shop->ShopType>0&&(etime>=player->Shop->mil_shop_time))
                    {
                      BEGINPACKET( pak, 0x702 );
                      ADDSTRING( pak, "[Mileage] Mileage shop expired !");
                      ADDBYTE( pak, 0 );
                      player->client->SendPacket(&pak);
                      player->Shop->ShopType=0;
                      player->Shop->mil_shop_time=0;
                    }

                }

                // Monster update //------------------------
                pthread_mutex_lock( &map->MonsterMutex );

                //spawn TD monsters
                unsigned nextmon = map->TDMobList[map->TDNextSpawn];
                if(nextmon != 0)
                {
                    //Log(MSG_DEBUG,"Spawning TD mobs");
                    clock_t etime = clock() - map->lastTDSpawnTime;
                    if(etime >= map->TDMobDelay) // check if elapsed time is greater than spawn delay
                    {
                         //spawn the next monster on the list
                         //Log(MSG_DEBUG,"Spawning TD mob of type %i at position %i in the list for map %i",nextmon, map->TDNextSpawn, map->id);
                         map->AddMonster(nextmon, GServer->WPList[map->id][1].pos, 0, 0, 0, 1, 0, 999 ); //spawn a TD mob with no AI
                         //Log(MSG_DEBUG,"Spawned mob in addmonster");
                         map->TDMobList[map->TDNextSpawn] = 0; //clear the spawn
                         //Log(MSG_DEBUG,"cleared the spawn");
                         map->TDNextSpawn++; //move the pointer
                         //Log(MSG_DEBUG,"moved the pointer");
                         if(map->TDNextSpawn > 100)map->TDNextSpawn = 0;
                         map->lastTDSpawnTime = clock();
                    }
                }
                //TD spawn end

                for(UINT j=0;j<map->MonsterList.size();j++)
                {
                    CMonster* monster = map->MonsterList.at(j);
                    //UINT thistimer = monster->AItimer;
                    clock_t etime = clock() - monster->lastAiUpdate;

                    //LMA: only summon ?
                    if (only_summon&&!monster->IsSummon())
                    {
                        continue;
                    }
                    else if(only_summon)
                    {
                        nb_summons_map++;
                    }

                    //PY handling day only or night only monsters. 
                    
					if(!map->IsNight( ) && monster->Status->nightonly)// if day, delete all night time monsters
                    {
                        //Log( MSG_INFO, "Night Only monster deleted. Type %i", monster->montype);
                        map->DeleteMonster( monster, true, j );
                        continue;
                    }
                    if(map->IsNight() && monster->Status->dayonly)
                    {
                        //Log( MSG_INFO, "Day Only monster deleted. Type %i", monster->montype);
                        map->DeleteMonster( monster, true, j );
                        continue;
                    }

                    //Do TD stuff
                    if(monster->MonAI == 999 && etime >= monster->AITimer) //It's a TD mob
                    {
                        monster->UpdatePosition(monster->stay_still );
                        float distance = GServer->distance( monster->Position->current, monster->Position->destiny );
                        //Log( MSG_INFO, "Found a TD monster %f from target. Waypoint type = %i stance %i speed %i", distance,GServer->WPList[map->id][monster->NextWayPoint].WPType,monster->Stats->stance, monster->Stats->Move_Speed);
                        //Log( MSG_INFO, "monster location X: %f Y: %f",monster->Position->current.x, monster->Position->current.y);
                        //Log( MSG_INFO, "WP location X: %f Y: %f ", GServer->WPList[map->id][monster->NextWayPoint].pos.x,GServer->WPList[map->id][monster->NextWayPoint].pos.y);
                        if(distance < 1) //monster has reached it's destination
                        {
                            switch(GServer->WPList[map->id][monster->NextWayPoint].WPType)
                            {
                                case 1: //Start waypoint
                                case 2: //regular waypoint. Increment waypoint counter. Set detiny to next waypoint
                                {
                                    //Log( MSG_INFO, "Monster reached waypoint %i type: %i", monster->NextWayPoint,GServer->WPList[map->id][monster->NextWayPoint].WPType);
                                    monster->NextWayPoint++;
                                    monster->Position->destiny.x = GServer->WPList[map->id][monster->NextWayPoint].pos.x;
                                    monster->Position->destiny.y = GServer->WPList[map->id][monster->NextWayPoint].pos.y;
                                    BEGINPACKET( pak, 0x797 );
                                	ADDWORD    ( pak, monster->clientid );
                                	ADDWORD    ( pak, 0x0000 );
                                	ADDWORD    ( pak, monster->Stats->Move_Speed ); //speed
                                	ADDFLOAT   ( pak, monster->Position->destiny.x * 100 );
                                	ADDFLOAT   ( pak, monster->Position->destiny.y * 100 );
                                	ADDWORD    ( pak, 0xcdcd );
                                	ADDBYTE    ( pak, monster->Stats->stance );  //should be walking
                                	GServer->SendToVisible(&pak, monster);
                                }
                                break;
                                case 3: //Final waypoint. do stuff. suicide
                                {
                                    //Log( MSG_INFO, "Reached final waypoint");
                                    //monster->Stats->HP = 0;
                                    map->DeleteMonster( monster, true, j );
                                    //Log( MSG_INFO, "monster successfully deleted");
                                    continue;
                                }
                                break;
                                default:
                                    Log( MSG_INFO, "WPType %i not recognized",GServer->WPList[map->id][monster->NextWayPoint].WPType);
                                break;
                            }
                        }
                        else
                        {
                            //send move packet again to try to synchronise the monster movement
                            BEGINPACKET( pak, 0x797 );
                        	ADDWORD    ( pak, monster->clientid );
                        	ADDWORD    ( pak, 0x0000 );
                        	ADDWORD    ( pak, monster->Stats->Move_Speed ); //speed
                        	ADDFLOAT   ( pak, monster->Position->destiny.x * 100 );
                        	ADDFLOAT   ( pak, monster->Position->destiny.y * 100 );
                        	ADDWORD    ( pak, 0xcdcd );
                        	ADDBYTE    ( pak, monster->Stats->stance );  //should be walking
                        	GServer->SendToVisible(&pak, monster);
                        }
                        monster->lastAiUpdate = clock();
                    }
                    //End TD stuff

                    //if(!monster->PlayerInRange( )) continue;
                    if(!monster->UpdateValues( )) continue;
                    monster->UpdatePosition( true );
                    if(monster->IsOnBattle( ))
                    {
                         monster->DoAttack( );   // why was this commented? Monsters were not attacking
                         //monster->DoAi(monster->thisnpc->AI, 2);  //don't use thisnpc->AI any more.
                         if(etime >= monster->AITimer)
                         {
                             monster->DoAi(monster->MonAI, 2);
                             monster->lastAiUpdate = clock();
                         }
                         //Log(MSG_INFO,"Monster type: %i current HP: %i",monster->montype, monster->Stats->HP);
                    }
                    else
                    {
                         //monster->DoAi(monster->thisnpc->AI, 1);
                         if(etime >= monster->AITimer)
                         {
                            monster->DoAi(monster->MonAI, 1);
                            monster->lastAiUpdate = clock();
                         }
                    }
                    monster->RefreshBuff( );
                    if (monster->IsSummon())
                    {
                        monster->SummonUpdate(monster,map, j);
                        continue;
                    }
                    if(monster->IsDead())
                    {
                        if(clock() - monster->DeathDelayTimer > GServer->Config.DeathDelay)
                        {
                            //Log(MSG_DEBUG,"Found dead monster montype %i",monster->montype);
                            monster->OnDie( );  //all this does is give exp
                            //Log(MSG_DEBUG,"back from giving exp");
                            monster->DoAi(monster->MonAI, 5);
                            //Log(MSG_DEBUG,"ran AI");
                            map->DeleteMonster( monster, true, j );
                            //Log(MSG_DEBUG,"deleted monster");
                            continue;
                        }
                        else
                        {
                            //Log(MSG_DEBUG,"Dead monster found. waiting for death delay timer");
                        }
                    }

                }

                //LMA: was there any summons in this map?
                if(only_summon&&nb_summons_map==0)
                {
                    map->nb_summons=0;
                }

            }

            if(only_npc&&!only_summon)
            {
                pthread_mutex_lock( &map->MonsterMutex );
            }

            //LMA: AIP for NPC.
            for(UINT j=0;j<map->NPCList.size();j++)
            {
                CNPC* npc = map->NPCList.at(j);

                //LMA: We don't worry about IFO Objects...
                if(npc->npctype>10000)
                {
                    continue;
                }

                if(npc->thisnpc->AI != 0 && npc->thisnpc->AI != 30)
                {
                    //check every minute. Conditions seem to be based on 6 minute segments
                    //LMA: untrue for some NPCs, special case for UW...
					//PY: NO we don't check every minute. We check whatever interval the AIP file asks for

                    bool is_time_ok=false;

                    // int delay=60000;    //each AIP 60 seconds.
                    //LMA: AIP Timer.
                    //delay = npc->thisnpc->AiTimer;

                    if(npc->thisnpc->AiTimer == 0)
                    {
						//npc->thisnpc->AiTimer = 60000;
                        //Log(MSG_WARNING,"NPC %i hadn't timer, file AI=%i",npc->npctype,npc->thisnpc->AI);
						//PY: If the NPC doesn't have a timer then let's give it one
						CAip* script = NULL;
						for(unsigned k=0; k < GServer->AipList.size(); k++)
						{
							if (GServer->AipList.at(k)->AInumber == npc->thisnpc->AI)
							{
								script = GServer->AipList.at(k);
								break;
							}
						}
						if(script == NULL)
						{
							Log( MSG_WARNING, "Invalid AI script for AI %i Setting AI to 30 (blank)", npc->thisnpc->AI );
							npc->thisnpc->AI = 30;
							continue;
						}
						//Set the timer
						npc->thisnpc->AiTimer = script->minTime;
                    }
					//PY: Any npc reaching this point has a valid timer. It has either been set or removed
					
                    
					//PY MORE Bloody lmame special cases
					//If he had just set the frickin timer correctly like i did above, all this crap would be un-necessary

					//Leum, for Union War (no need to do his stuff always). //PY: NO this is BS
                    //if(npc->npctype==1113&&GServer->ObjVar[1113][1]>0)
                    //{
                        //LogDebug("Doing an update for Leum each 10 seconds since UW is on");
                    //    delay=10000;
                    //}

                    //PY: What the hell is this shit? commenting it out
					//Walls for map 66 (no need to do his stuff always)
                    //if(npc->npctype>=1024&&npc->npctype<=1027&&GServer->ObjVar[1249][2]>0&&GServer->ObjVar[1249][2]<=90)
                    //{
                        //LogDebug("Doing an update for Wall %i each second quest from Hope is on",npc->npctype);
                    //    delay=1000;
                    //}

                    //PY: Seriously??
					//Hope map 66 (no need to do his stuff always)
                    //if(npc->npctype==1249&&GServer->ObjVar[1249][2]>0&&GServer->ObjVar[1249][2]<=90)
                    //{
                        //LogDebug("Doing an update for Hope each 10 seconds quest from Hope is on",npc->npctype);
                    //    delay=10000;
                    //}

                    //PY and more special cases........
					//Williams
                    //if(npc->npctype==1075)
                    //{
                        //Each 5 minutes
                    //    delay=300000;
                    //}

                    //LMA END


                     //if(60000<(UINT)GServer->round((clock( ) - npc->lastAiUpdate)))
                     //if(is_time_ok)
                     //if(delay<(UINT)GServer->round((clock( ) - npc->lastAiUpdate)))
                     //PY NOPE!! How about we do it right
					UINT thistimer = npc->thisnpc->AiTimer * 1000; //this is always set in seconds in AIP
					if(thistimer<(UINT)GServer->round((clock( ) - npc->lastAiUpdate))) //check AIP conditions when the timer calls for it
					{
                         CNPCData* thisnpc = GServer->GetNPCDataByID( npc->npctype );
                         if(thisnpc == NULL)
                         {
                             Log( MSG_WARNING, "Invalid montype %i", npc->npctype );
                             continue;
                         }

                         //LMA: before this temp monster was created and deleted every time, now we only do it once...
                         //new code, we overwrite the temp monster each time rather than creating / deleting him each time.
                        NPCmonster->Position->source = npc->pos;
                        NPCmonster->Position->current = npc->pos;
                        NPCmonster->Position->destiny = npc->pos;
                         NPCmonster->montype=npc->npctype;
                         NPCmonster->Position->Map=map->id;
                         NPCmonster->Position->lastMoveTime = clock( );
                         NPCmonster->SpawnTime = clock( );
                         NPCmonster->lastSighCheck = clock( );
                         NPCmonster->lastLifeUpdate = time(NULL);

                         //old code:
                         //CMonster* NPCmonster = new (nothrow) CMonster( npc->pos, npc->npctype, map->id, 0, 0  );
                         NPCmonster->aip_npctype=npc->npctype;
                         NPCmonster->aip_clientid=npc->clientid;
                         NPCmonster->thisnpc = thisnpc;

                         int lma_previous_eventID = npc->thisnpc->eventid;
                         //Log(MSG_INFO,"XCIDAIBEGIN NPC %i map %i cid %i",npc->npctype,map->id,npc->clientid);

                         NPCmonster->DoAi(NPCmonster->MonAI, 1);
                         //Log(MSG_INFO,"XCIDAIEND NPC %i map %i cid %i",npc->npctype,map->id,npc->clientid);

                        
						 //PY: AAAAAAAAAGGGGGGGGGGHHHHHHHHHH. NO! just NO!!. Stop with the bloody special cases. Fix the damn core code already!!!!!!!
						 //Williams (temple of Oblivion)
                        /*if(npc->npctype == 1075)
                        {
                            //each 5 minutes
                            //saving values for him
                            if(GServer->LastTempleAccess[0]!=GServer->ObjVar[npc->npctype][0]||GServer->LastTempleAccess[1]!=GServer->ObjVar[npc->npctype][1])
                            {
                                GServer->DB->QExecute("UPDATE list_npcs SET eventid=%i, extra_param=%i WHERE type=1075",GServer->ObjVar[1075][0],GServer->ObjVar[1075][1]);
                                /*Log(MSG_WARNING,"Doing an update for Williams each 5 minutes, values changed (%i->%i, %i->%i)",
                                GServer->LastTempleAccess[0],GServer->ObjVar[npc->npctype][0],
                                GServer->LastTempleAccess[1],GServer->ObjVar[npc->npctype][1]);
                            }*/
                            /*else
                            {
                                Log(MSG_WARNING,"Doing an update for Williams each 5 minutes.");
                            }*/
							/*
                            GServer->LastTempleAccess[0]=GServer->ObjVar[npc->npctype][0];
                            GServer->LastTempleAccess[1]=GServer->ObjVar[npc->npctype][1];
                        }*/

                         
						 //PY: I have absolutely no idea what this bit does. I suspuct it's also a load of bollux but I'm leaving it in for now
						 //ToDo Figure this out and tidy it up

						 //LMA: check if eventID changed, if we do it in AIP conditions / actions, it just fails...
                         if (lma_previous_eventID!=NPCmonster->thisnpc->eventid)
                         {
                            //Log(MSG_WARNING,"(1)Event ID not the same NPC %i from %i to %i in map %i, npc->thisnpc->eventid=%i !",npc->npctype,lma_previous_eventID,NPCmonster->thisnpc->eventid,map->id,npc->thisnpc->eventid);
                            LogDebugPriority(3);
                            LogDebug("(1)Event ID not the same NPC %i from %i to %i in map %i, npc->thisnpc->eventid=%i !",npc->npctype,lma_previous_eventID,NPCmonster->thisnpc->eventid,map->id,npc->thisnpc->eventid);
                            LogDebugPriority(4);
                            npc->thisnpc->eventid=NPCmonster->thisnpc->eventid;
                            npc->event=npc->thisnpc->eventid;
                            //LMA: We have to change the event ID here since we didn't send the clientID :(
                            BEGINPACKET( pak, 0x790 );
                            ADDWORD    ( pak, npc->clientid );
                            ADDWORD    ( pak, npc->thisnpc->eventid );
                            GServer->SendToAllInMap(&pak,map->id);
                         }

                        //LMA: We don't clear anymore, too much hassle :(
                        //We declare the temporary monster just once...
                        /*GServer->ClearClientID(NPCmonster->clientid);
                         delete NPCmonster;*/
                         npc->lastAiUpdate = clock();
                     }

                }

                //LMA: Sometimes another NPC does the job for you.
                if(npc->thisnpc->eventid!=GServer->ObjVar[npc->npctype][0])
                {
                    int new_event_id=GServer->ObjVar[npc->npctype][0];
                    LogDebugPriority(3);
                    //Log(MSG_WARNING,"(2)Event ID not the same NPC %i from %i to %i in map %i, npc->thisnpc->eventid=%i !",npc->npctype,npc->thisnpc->eventid,new_event_id,map->id,npc->thisnpc->eventid);
                    LogDebug("(2)Event ID not the same NPC %i from %i to %i in map %i, npc->thisnpc->eventid=%i !",npc->npctype,npc->thisnpc->eventid,new_event_id,map->id,npc->thisnpc->eventid);
                    LogDebugPriority(4);
                    npc->thisnpc->eventid=new_event_id;
                    npc->event=new_event_id;
                    //LMA: We have to change the event ID here since we didn't send the clientID :(
                    BEGINPACKET( pak, 0x790 );
                    ADDWORD    ( pak, npc->clientid );
                    ADDWORD    ( pak, npc->thisnpc->eventid );
                    GServer->SendToAllInMap(&pak,map->id);
                }

            }

            pthread_mutex_unlock( &map->MonsterMutex );
        }

        pthread_mutex_unlock( &GServer->MapMutex );
        pthread_mutex_unlock( &GServer->PlayerMutex );

        #ifdef _WIN32
        Sleep(GServer->Config.MapDelay);
        #else
        usleep(GServer->Config.MapDelay);
        #endif
    }
    pthread_exit( NULL );

    //LMA: we delete the temporary monster.
    GServer->ClearClientID(NPCmonster->clientid);
    delete NPCmonster;
	return 0;
}


// Visibility Process
PVOID VisibilityProcess(PVOID TS)
{
    while(GServer->ServerOnline)
    {
        pthread_mutex_lock( &GServer->PlayerMutex );
        pthread_mutex_lock( &GServer->MapMutex );
        for(UINT i=0;i<GServer->MapList.Map.size();i++)
        {
            CMap* map = GServer->MapList.Map.at(i);
            if( map->PlayerList.size()<1 )
                continue;
            for(UINT j=0;j<map->PlayerList.size();j++)
            {
                CPlayer* player = map->PlayerList.at(j);
                if(!player->Session->inGame)
                {
                    continue;
                }
                if(!player->VisiblityList()) Log(MSG_WARNING, "Visibility False: %u", player->clientid );
                if( GServer->Config.AUTOSAVE == 1 )
                {
                    clock_t etime = clock() - player->lastSaveTime;
                    if( etime >= GServer->Config.SAVETIME*1000 )
                    {
                        player->savedata( );
                        player->lastSaveTime = clock();
                    }
                }
            }
        }
        pthread_mutex_unlock( &GServer->MapMutex );
        pthread_mutex_unlock( &GServer->PlayerMutex );
        #ifdef _WIN32
        Sleep(GServer->Config.VisualDelay);
        #else
        usleep(GServer->Config.VisualDelay);
        #endif
    }
    pthread_exit(NULL);
	return 0;
}

// World Process
PVOID WorldProcess( PVOID TS )
{
    while( GServer->ServerOnline )
    {
        pthread_mutex_lock( &GServer->MapMutex );
        for(UINT i=0;i<GServer->MapList.Map.size();i++)
        {
            CMap* map = GServer->MapList.Map.at(i);
            if( map->PlayerList.size()<1 )
                continue;
            map->UpdateTime( );
            pthread_mutex_lock( &map->DropMutex );
            map->CleanDrops( );
            pthread_mutex_unlock( &map->DropMutex );
            pthread_mutex_lock( &map->MonsterMutex );
            map->RespawnMonster( );
            pthread_mutex_unlock( &map->MonsterMutex );
        }
        pthread_mutex_unlock( &GServer->MapMutex );
        GServer->RefreshFairy( );
        #ifdef _WIN32
        Sleep(GServer->Config.WorldDelay);
        #else
        usleep(GServer->Config.WorldDelay);
        #endif
    }
    pthread_exit(NULL);
	return 0;
}

// Shutdown Server Process
PVOID ShutdownServer(PVOID TS)
{
    int minutes = (int)TS;
    #ifdef _WIN32
    Sleep(minutes*60000);
    #else
    usleep(minutes*60000);
    #endif
    Log( MSG_INFO, "Saving User Information... " );
    GServer->DisconnectAll();
    Log( MSG_INFO, "Waiting Process to ShutDown... " );
    GServer->ServerOnline = false;
    int status,res;
    res = pthread_join( GServer->WorldThread[0], (PVOID*)&status );
    if(res)
    {
        Log( MSG_WARNING, "World thread can't be joined" );
    }
    else
    {
        Log( MSG_INFO, "World thread closed." );
    }
    res = pthread_join( GServer->WorldThread[1], (PVOID*)&status );
    if(res)
    {
        Log( MSG_WARNING, "Visibility thread can't be joined" );
    }
    else
    {
        Log( MSG_INFO, "Visibility thread closed." );
    }
    res = pthread_join( GServer->MapThread[0], (PVOID*)&status );
    if(res)
    {
        Log( MSG_WARNING, "Map thread can't be joined" );
    }
    else
    {
        Log( MSG_INFO, "Map thread closed." );
    }
    Log( MSG_INFO, "All Threads Closed." );
    GServer->isActive = false;
    pthread_exit(NULL);
	return 0;
}

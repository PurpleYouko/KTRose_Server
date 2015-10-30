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
#include "CharServer.h"

//LMA: mask packet?
bool CCharServer::lma_mask (CCharClient* thisclient)
{
    Log(MSG_INFO,"[CS]in lma_mask for %s",thisclient->charname);
    if( thisclient->clanid > 0 )
    {
        Log(MSG_INFO,"[CS] lma_mask, trying to get clan %i",thisclient->clanid );
        CClans *thisclan = GetClanByID(thisclient->clanid);
        if(thisclan!=NULL)
        {
            BEGINPACKET( pak, 0x7e0);
            ADDBYTE    ( pak, 0x71);//0x71, mask?
            ADDWORD    ( pak, thisclan->id);// clan id
            ADDBYTE    ( pak, 0x00);
            ADDBYTE    ( pak, 0x00);
            ADDWORD    ( pak, thisclan->back);//Clan Background
            ADDWORD    ( pak, thisclan->logo);//Clan logo
            ADDBYTE    ( pak, thisclan->grade);//Clan grade
            ADDBYTE    ( pak, 0xcc);// Clan rank (0 = red rokie / 6 = master)
            ADDDWORD    ( pak, thisclan->cp); //Clan Points
            ADDWORD    ( pak, 0x0064); //always 0x64? having before thisclan->cp
            ADDWORD    ( pak, 0x0000); //??
            ADDQWORD   ( pak, 0x000000000000 );   //Clan Money
            ADDDWORD    ( pak, 0xcccccccc);  //Nb of clan members.
            ADDWORD    ( pak, 0xcccc);
            for(int i=0;i<120;i++)
                ADDBYTE ( pak, 0x00);
            ADDDWORD   ( pak, 0x00000000); //Ranking Points
            ADDDWORD   ( pak, 0xcccccccc);  //LMA 139+ (Reward points for clan shop)
            thisclient->SendPacket(&pak);
            Log(MSG_INFO,"[CS]lma_mask sent for %s",thisclient->charname);
        }

    }


     return true;
}

//LMA: Reward points
bool CCharServer::SendRewardPoints (CCharClient* thisclient,long int lma_points)
{
    Log(MSG_INFO,"[CS]in SendRewardPoints for %s",thisclient->charname);
    if( thisclient->clanid > 0 )
    {
        Log(MSG_INFO,"[CS] SendRewardPoints, trying to get clan %i",thisclient->clanid );
        CClans *thisclan = GetClanByID(thisclient->clanid);
        if(thisclan!=NULL)
        {
            thisclient->reward_points=lma_points;          //reward points is handled by world server.

            BEGINPACKET( pak, 0x7e0);
            ADDBYTE    ( pak, 0x33);//0x33 you have invited to clan
            ADDWORD    ( pak, thisclan->id);// clan id
            ADDBYTE    ( pak, 0x00);
            ADDBYTE    ( pak, 0x00);
            ADDWORD    ( pak, thisclan->back);//Clan Background
            ADDWORD    ( pak, thisclan->logo);//Clan logo
            ADDBYTE    ( pak, thisclan->grade);//Clan grade
            ADDBYTE    ( pak, thisclient->clan_rank);// Clan rank (0 = red rokie / 6 = master)
            ADDDWORD    ( pak, thisclan->cp); //Clan Points
            ADDWORD    ( pak, 0x0064); //always 0x64? having before thisclan->cp
            ADDWORD    ( pak, 0x0000); //??
            ADDQWORD   ( pak, 0x000000000000 );   //Clan Money
            ADDDWORD    ( pak, thisclan->ClanMembers.size());  //Nb of clan members.
            //ADDDWORD   ( pak, 0x00000000);
            ADDWORD    ( pak, 0x0000);
            for(int i=0;i<120;i++)
                ADDBYTE ( pak, 0x00);
            ADDDWORD   ( pak, 0x00000000); //Ranking Points
            ADDDWORD   ( pak, thisclient->reward_points );  //LMA 139+ (Reward points for clan shop)
            ADDSTRING  ( pak, thisclan->name);//Clan Name
            ADDBYTE    ( pak, 0x00);

            //Clan slogan
            if (strlen(thisclan->slogan)==0)
            {
                 ADDSTRING  ( pak, thisclan->name);
                 ADDBYTE    ( pak, 0x00);
            }
            else
            {
                ADDSTRING  ( pak, thisclan->slogan);
                ADDBYTE    ( pak, 0x00);
            }

            //only added to packet if there is actually a news inside.
            if (strlen(thisclan->news)!=0)
            {
               ADDSTRING  ( pak, thisclan->news);
               ADDBYTE    ( pak, 0x00);
            }

            thisclient->SendPacket(&pak);
            Log(MSG_INFO,"[CS] Clan packet: 0x7e0, SendRewardPoints %s",thisclan->name);
        }
        else
        {
            Log(MSG_INFO,"[CS] clan ID %i not found in SendRewardPoints",thisclient->clanid );
        }

    }
    return true;
}

bool CCharServer::SendClanPoints (CCharClient* thisclient,long int lma_points)
{
    Log(MSG_INFO,"[CS]in SendClanPoints for %s",thisclient->charname);
    if( thisclient->clanid > 0 )
    {
        Log(MSG_INFO,"[CS] SendClanPoints, trying to get clan %i",thisclient->clanid );
        CClans *thisclan = GetClanByID(thisclient->clanid);
        if(thisclan!=NULL)
        {
            //clan points is handled by world server.
            if(thisclan->cp+lma_points<0)
            {
                thisclan->cp=0;
            }
            else
            {
                thisclan->cp+=lma_points;
            }

            //saving clanpoints.
            DB->QExecute("UPDATE list_clan SET cp=%u WHERE id=%i",thisclan->cp,thisclient->clanid);

            BEGINPACKET( pak, 0x7e0);
            ADDBYTE    ( pak, 0x33);//0x33 you have invited to clan
            ADDWORD    ( pak, thisclan->id);// clan id
            ADDBYTE    ( pak, 0x00);
            ADDBYTE    ( pak, 0x00);
            ADDWORD    ( pak, thisclan->back);//Clan Background
            ADDWORD    ( pak, thisclan->logo);//Clan logo
            ADDBYTE    ( pak, thisclan->grade);//Clan grade
            ADDBYTE    ( pak, thisclient->clan_rank);// Clan rank (0 = red rokie / 6 = master)
            ADDDWORD    ( pak, thisclan->cp); //Clan Points
            ADDWORD    ( pak, 0x0064); //always 0x64? having before thisclan->cp
            ADDWORD    ( pak, 0x0000); //??
            ADDQWORD   ( pak, 0x000000000000 );   //Clan Money
            ADDDWORD    ( pak, thisclan->ClanMembers.size());  //Nb of clan members.
            //ADDDWORD   ( pak, 0x00000000);
            ADDWORD    ( pak, 0x0000);
            for(int i=0;i<120;i++)
                ADDBYTE ( pak, 0x00);
            ADDDWORD   ( pak, 0x00000000); //Ranking Points
            ADDDWORD   ( pak, thisclient->reward_points );  //LMA 139+ (Reward points for clan shop)
            ADDSTRING  ( pak, thisclan->name);//Clan Name
            ADDBYTE    ( pak, 0x00);

            //Clan slogan
            if (strlen(thisclan->slogan)==0)
            {
                 ADDSTRING  ( pak, thisclan->name);
                 ADDBYTE    ( pak, 0x00);
            }
            else
            {
                ADDSTRING  ( pak, thisclan->slogan);
                ADDBYTE    ( pak, 0x00);
            }

            //only added to packet if there is actually a news inside.
            if (strlen(thisclan->news)!=0)
            {
               ADDSTRING  ( pak, thisclan->news);
               ADDBYTE    ( pak, 0x00);
            }

            thisclient->SendPacket(&pak);
            Log(MSG_INFO,"[CS] Clan packet: 0x7e0, SendClanPoints %s",thisclan->name);
        }
        else
        {
            Log(MSG_INFO,"[CS] clan ID %i not found in SendClanPoints",thisclient->clanid );
        }

    }
    return true;
}

//LMA: Saving Clan Grade.
bool CCharServer::SendClanGrade (CCharClient* thisclient,int clan_grade)
{
    Log(MSG_INFO,"[CS]in SendClanGrade for %s",thisclient->charname);
    if(thisclient->clanid <= 0 )
        return true;

    Log(MSG_INFO,"[CS] SendClanGrade, trying to get clan %i",thisclient->clanid );
    CClans *thisclan = GetClanByID(thisclient->clanid);
    if(thisclan!=NULL)
    {
        //clan grade is handled by world server.
        thisclan->grade=clan_grade;

        //saving clan grade.
        DB->QExecute("UPDATE list_clan SET grade=%i WHERE id=%i",thisclan->grade,thisclient->clanid);
        /*
        BEGINPACKET( pak, 0x7e0 );
        ADDBYTE    ( pak, 0x84 );
        ADDWORD    ( pak, thisclient->clanid );
        ADDWORD    ( pak, clan_grade );
        ADDSTRING  ( pak, thisclient->charname );
        ADDBYTE    ( pak, 0x00 );
        SendToClanMembers(thisclient->clanid,&pak);
        */

        Log(MSG_INFO,"[CS] Clan packet: 0x7e0, SendClanGrade %s",thisclan->name);
    }
    else
    {
        Log(MSG_INFO,"[CS] clan ID %i not found in SendClanGrade",thisclient->clanid );
    }


    return true;
}

// Send Clan information
bool CCharServer::SendClanInfo (CCharClient* thisclient)
{
        Log(MSG_INFO,"[CS]in SendClanInfo for %s",thisclient->charname);
    if( thisclient->clanid > 0 )
    {
        Log(MSG_INFO,"[CS] sendclaninfo, trying to get clan %i",thisclient->clanid );
        CClans *thisclan = GetClanByID(thisclient->clanid);
        if(thisclan!=NULL)
        {
            /* Previous version
            BEGINPACKET( pak, 0x7e0);
            ADDBYTE    ( pak, 0x33);//0x33 you have invited to clan
            ADDWORD    ( pak, thisclan->id);// clan id
            ADDBYTE    ( pak, 0x00);//
            ADDBYTE    ( pak, 0x00);
            ADDWORD    ( pak, thisclan->back);//Clan Background
            ADDWORD    ( pak, thisclan->logo);//Clan logo
            ADDBYTE    ( pak, thisclan->grade);//Clan grade
            ADDBYTE    ( pak, thisclient->clan_rank);// Clan rank (0 = red rokie / 6 = master)
            ADDWORD    ( pak, 0x0000 ); // eu adicinei depois do update
            ADDWORD    ( pak, 0x0000 ); // eu adicinei depois do update
            ADDWORD    ( pak, thisclan->cp);//Clan Points
            ADDDWORD   ( pak, 0x00000000);
            ADDDWORD   ( pak, 0x00000000); //Clan found
            ADDBYTE    ( pak, 0x00);
            ADDDWORD   ( pak, 0x00000000);
            ADDWORD    ( pak, 0x0000);
            for(int i=34;i<156;i++)
                ADDBYTE ( pak, 0x00);
            ADDWORD    ( pak, 0x0000);
            ADDBYTE    ( pak, 0x00);
//            ADDDWORD   ( pak, 0x00000000 );
//            ADDBYTE    ( pak, 0x00 );
//            ADDBYTE    ( pak, 0x00);

            //ADDDWORD   ( pak, 0x00000000 );  //LMA 139+ (Reward points for clan shop)
            thisclient->reward_points++;
            ADDDWORD   ( pak, thisclient->reward_points );  //LMA 139+ (Reward points for clan shop)
            ADDSTRING  ( pak, thisclan->name);//Clan Name
            ADDBYTE    ( pak, 0x00);
            ADDSTRING  ( pak, thisclan->slogan);//Clan slogan
            ADDBYTE    ( pak, 0x00);
            ADDSTRING  ( pak, thisclan->news);//Clan news
            ADDBYTE    ( pak, 0x00);
            */

            //New Version
            BEGINPACKET( pak, 0x7e0);
            ADDBYTE    ( pak, 0x33);//0x33 you have invited to clan
            ADDWORD    ( pak, thisclan->id);// clan id
            ADDBYTE    ( pak, 0x00);
            ADDBYTE    ( pak, 0x00);
            ADDWORD    ( pak, thisclan->back);//Clan Background
            ADDWORD    ( pak, thisclan->logo);//Clan logo
            ADDBYTE    ( pak, thisclan->grade);//Clan grade
            ADDBYTE    ( pak, thisclient->clan_rank);// Clan rank (0 = red rokie / 6 = master)
            ADDDWORD    ( pak, thisclan->cp); //Clan Points
            ADDWORD    ( pak, 0x0064); //always 0x64? having before thisclan->cp
            ADDWORD    ( pak, 0x0000); //??
            ADDQWORD   ( pak, 0x000000000000 );   //Clan Money
            ADDDWORD    ( pak, thisclan->ClanMembers.size());  //Nb of clan members.
            //ADDDWORD   ( pak, 0x00000000);
            ADDWORD    ( pak, 0x0000);
            //Skills.
            for(int i=0;i<120;i++)
                ADDBYTE ( pak, 0x00);
            ADDDWORD   ( pak, 0x00000000); //Ranking Points
            ADDDWORD   ( pak, thisclient->reward_points );  //LMA 139+ (Reward points for clan shop)
            ADDSTRING  ( pak, thisclan->name);//Clan Name
            ADDBYTE    ( pak, 0x00);

            //Clan slogan
            if (strlen(thisclan->slogan)==0)
            {
                 ADDSTRING  ( pak, thisclan->name);
                 ADDBYTE    ( pak, 0x00);
            }
            else
            {
                ADDSTRING  ( pak, thisclan->slogan);
                ADDBYTE    ( pak, 0x00);
            }

            //only added to packet if there is actually a news inside.
            if (strlen(thisclan->news)!=0)
            {
               ADDSTRING  ( pak, thisclan->news);
               ADDBYTE    ( pak, 0x00);
            }

            thisclient->SendPacket(&pak);
            Log(MSG_INFO,"[CS] Clan packet: 0x7e0, SendClanInfo %s",thisclan->name);

            //Put the player online in clan
            for(UINT i=0;i<thisclan->ClanMembers.size();i++)
        	{
                CClanMembers*	ClanMember = thisclan->ClanMembers.at( i );;
                CCharClient* otherclient = GetClientByID( ClanMember->id );
                if(otherclient!=NULL)
                    ChangeClanStatus (thisclient, otherclient, thisclient->channel);//send channel here
        	}
        }
        else
        {
            Log(MSG_INFO,"[CS] clan ID %i not found in sendclaninfo",thisclient->clanid );
        }

    }
    return true;
}

// Change player status in clan ((channel) = online/ 0xff = offline)
bool CCharServer::ChangeClanStatus (CCharClient* thisclient, CCharClient* otherclient, int channel)
{
        BEGINPACKET( pak, 0x7e0 );
        ADDBYTE    ( pak, 0x73 );//Change player status

        //ADDBYTE    ( pak, 0x00 );
        //LMA: Nb of people already online in clan?
        //or clan rank?
        /*int nb_online=0;
        CClans* thisclan = (CClans*) GetClanByID( thisclient->clanid );
        if(thisclan!=NULL)
        {
            for(UINT i=0;i<thisclan->ClanMembers.size();i++)
        	{
                CClanMembers*	ClanMember = thisclan->ClanMembers.at( i );;
                CCharClient* otherclient = GetClientByID( ClanMember->id );
                if(otherclient!=NULL)
                    nb_online++;
        	}

        }
        ADDBYTE    ( pak, nb_online );
        */

        //Clan rank version:
        ADDBYTE    ( pak, thisclient->clan_rank);
        //End of test

        ADDBYTE    ( pak, channel );//CANAL
        ADDWORD    ( pak, 0x0000 );
        ADDWORD    ( pak, 0x0000 );
        ADDWORD    ( pak, thisclient->level );
        ADDWORD    ( pak, thisclient->job );
        ADDSTRING  ( pak, thisclient->charname );
        ADDBYTE    ( pak, 0x00 );
        otherclient->SendPacket(&pak);
        Log(MSG_INFO,"[CS] ChangeClanStatus for %s, job %i, level %i, channel %i",thisclient->charname,thisclient->job,thisclient->level,channel);


        return true;
}

// Clan chat
bool CCharServer::pakClanChat ( CCharClient* thisclient, CPacket* P )
{
    if(!thisclient->isLoggedIn)
        return false;
    char* tmp;
	if (P->Buffer[0]=='/')
    {
        char* command = strtok( (char*)&(*P).Buffer[1] , " ");
        if(command==NULL)
            return true;
        if(strcmp(command, "pak")==0)//read cpacket.txt and send it (for test packets)
        {
            int comando = 0;
            if ((tmp = strtok(NULL, " "))==NULL)
                return true;
            comando=atoi(tmp);
            char buffer;
            FILE *packet1 = fopen("cpacket.txt","r");
            BEGINPACKET(pak,comando);
            if(packet1==NULL)
            {
                printf("Error opening packet1.txt for 716!\n");
                return true;
            }
            while((fscanf(packet1,"%c",&buffer))!=EOF)
                    ADDBYTE(pak,buffer);
            fclose(packet1);
    		thisclient->SendPacket( &pak );
    		return true;
        }
    }
    char* text = new (nothrow) char[P->Size-6];
    if(text==NULL)
    {
        Log(MSG_ERROR, "Error allocing memory" );
        return false;
    }
    memcpy( text, &P->Buffer, P->Size-6 );
    CClans* thisclan = (CClans*) GetClanByID ( thisclient->clanid );
    if(thisclan!=NULL)
    {
            BEGINPACKET( pak, 0x787 );
            ADDSTRING  ( pak, thisclient->charname );
            ADDBYTE    ( pak, 0x00 );
            ADDSTRING  ( pak, text );
            ADDBYTE    ( pak, 0x00 );
            SendToClanMembers(thisclan->id,&pak);
    }
    delete []text;
    return true;
}

// Clan manager for all the clan functions
bool CCharServer::pakClanManager ( CCharClient* thisclient, CPacket* P )
{
    if(!thisclient->isLoggedIn) return false;
    MYSQL_RES *result;
	MYSQL_ROW row;
    BYTE action = GETBYTE ((*P),0);
    switch (action)
    {
        case 0x01:
        break;
        case 0x02://invite clan
        {
            char* nick = new (nothrow) char[P->Size-7];
            if(nick==NULL)
            {
                Log(MSG_ERROR, "Error allocing memory" );
                return false;
            }
            memcpy( nick, &P->Buffer[1], P->Size-7 );

            //LMA: no case.
            //if(strcmp(nick,thisclient->charname)==0)
            if(stricmp(nick,thisclient->charname)==0)
            {
                delete []nick;
                return true;
            }

            CCharClient* otherclient = (CCharClient*) GetClientByName( nick );
            if(otherclient!=NULL)
            {
                //LMA: does the other guy have already a clan?
                if(otherclient->clanid!=0)
                {
                    Log(MSG_INFO,"%s tried to invite %s into his clan, but he was already in clanid %i",thisclient->charname,otherclient->charname,otherclient->clanid);
                    delete []nick;
                    break;
                }


                BEGINPACKET( pak, 0x7e0 );
                ADDBYTE    ( pak, 0x0b );//invite action
                ADDSTRING  ( pak, thisclient->charname );
                ADDBYTE    ( pak, 0x0 );
                otherclient->SendPacket( &pak );
                Log(MSG_INFO,"[CS] Clan invite %s",thisclient->charname);
            }
            delete []nick;
        }
        break;
        case 0x03://Kick from clan
        {
            char* nick = new (nothrow) char[P->Size-7];
            if(nick==NULL)
            {
                Log(MSG_ERROR, "Error allocing memory" );
                return false;
            }
            memcpy( nick, &P->Buffer[1], P->Size-7 );

            if(!CheckEscapeMySQL(nick,-1,true))
            {
                Log(MSG_WARNING,"A nick (kick clan) contains incorrect caracter (see warnings above)");
                return false;
            }

           CClans* thisclan = GetClanByID(thisclient->clanid);
           if(thisclan==NULL)
           {
                delete []nick;
                return true;
            }

            //LMA: the name order was wrong... Kicked then kicker.
            BEGINPACKET( pak, 0x7e0 );
	        ADDBYTE    ( pak, 0x81 );//xxx have kicket to yyyy
	        //ADDSTRING  ( pak, thisclient->charname );
	        ADDSTRING  ( pak, nick );
	        ADDBYTE    ( pak, 0x00);
	        //ADDSTRING  ( pak, nick );
	        ADDSTRING  ( pak, thisclient->charname );
	        ADDBYTE    ( pak, 0x00);
            SendToClanMembers(thisclient->clanid,&pak);

            for(UINT i=0;i<thisclan->ClanMembers.size( );i++)
            {
                CClanMembers* ClanMember =  thisclan->ClanMembers.at( i );

                //LMA: no case.
                //if(strcmp(ClanMember->name,nick)==0)
                if(stricmp(ClanMember->name,nick)==0)
                {
                    thisclan->ClanMembers.erase( thisclan->ClanMembers.begin() + i );
                    break;
               }
            }

           CCharClient* otherclient = (CCharClient*) GetClientByName( nick );
           if( otherclient!=NULL )
           {
               otherclient->clanid = 0;
               otherclient->clan_rank=1;
	           RESETPACKET( pak, 0x7e1 );//Update world clan information
	           ADDBYTE    ( pak, 0xfb );//action (kick)
	           ADDSTRING  ( pak, nick );
	           ADDBYTE    ( pak, 0x00 );
 	           //cryptPacket( (char*)&pak, NULL );
 	           CChanels* thischannel = GetChannelByID( otherclient->channel );
 	           if(thischannel!=NULL)
 	           {
        	       send( thischannel->sock, (char*)&pak, pak.Size, 0 );
               }
               pakClanMembers( otherclient );
            }

            if(!DB->QExecute("UPDATE characters SET clanid=0 AND clan_rank=1 WHERE char_name='%s'",nick))
            {
                delete []nick;
                return false;
            }
            Log(MSG_INFO,"[CS] delete (kick) %s from clan by %s",nick,thisclient->charname);
            delete []nick;
        }
        break;
        case 0x04://up rank
        {
            char* nick = new (nothrow) char[P->Size-7];
            if(nick==NULL)
            {
                Log(MSG_ERROR, "Error allocing memory" );
                return false;
            }
            memcpy( nick, &P->Buffer[1], P->Size-7 );
            Log(MSG_INFO,"[CS] clan, %s is trying to up %s",thisclient->charname,nick);

            if(!CheckEscapeMySQL(nick,-1,true))
            {
                Log(MSG_WARNING,"A nick (up rank) contains incorrect caracter (see warnings above)");
                return false;
            }

            int clan_rank=0;
            int level=0;
            int job=0;
            int channel=0xff;

            //LMA: no case.
            //if(strcmp(nick,thisclient->charname)==0)
            if(stricmp(nick,thisclient->charname)==0)
            {
                delete []nick;
                return true;
            }
            CCharClient* otherclient = GetClientByName ( nick );
            if(otherclient!=NULL)
            {
                clan_rank = otherclient->clan_rank;
                if(otherclient->clan_rank<5)
                {
                    otherclient->clan_rank++;
                }
                else
                {
                    delete []nick;
                    return true;
                }
                job = otherclient->job;
                level = otherclient->level;
                channel=otherclient->channel;
	            BEGINPACKET( pak, 0x7e0 );
	            ADDBYTE    ( pak, 0x83 );
	            ADDSTRING  ( pak, nick );
	            ADDBYTE    ( pak, 0x00 );
	            ADDBYTE    ( pak, otherclient->clan_rank );
                otherclient->SendPacket(&pak);
            }
            else
            {
                result = DB->QStore("SELECT clan_rank,classid,level FROM characters WHERE char_name='%s'",nick);
                if(result==NULL)
                {
                    delete []nick;
                    return false;
                }
                if(mysql_num_rows(result)!=1)
                {
                    Log(MSG_WARNING, "Invalid charname: %s" , nick );
                    delete []nick;
                    return false;
                }
	            row = mysql_fetch_row( result );
                clan_rank=atoi(row[0]);
                job = atoi(row[1]);
                level = atoi(row[2]);
                DB->QFree( );
            }
            if(clan_rank<5)
            {
                clan_rank++;
            }
            if(!DB->QExecute("UPDATE characters SET clan_rank=%i WHERE char_name='%s'",clan_rank,nick))
            {
                delete []nick;
                return false;
            }
            BEGINPACKET( pak, 0x7e0 );
            ADDBYTE    ( pak, 0x75 );//
            ADDBYTE    ( pak, clan_rank );
            ADDBYTE    ( pak, channel );//channel
            ADDWORD    ( pak, 0x0000 );
            ADDWORD    ( pak, 0x0000 );
            ADDWORD    ( pak, level ); // level
            ADDWORD    ( pak, job ); // job
            ADDSTRING  ( pak, nick ); // Nick
            ADDBYTE    ( pak, 0x00 );
            SendToClanMembers(thisclient->clanid,&pak);

	        RESETPACKET( pak, 0x7e1 );//update world information
	        ADDBYTE    ( pak, 0xfc );
	        ADDBYTE    ( pak, clan_rank );
	        ADDSTRING  ( pak, nick );
	        ADDBYTE    ( pak, 0x00 );
	        cryptPacket( (char*)&pak, NULL );
 	        CChanels* thischannel = GetChannelByID( channel );
 	        if(thischannel!=NULL)
        	    send( thischannel->sock, (char*)&pak, pak.Size, 0 );
        	delete []nick;
        }
        break;
        case 0x05://down rank
        {
            int clan_rank=0;
            int level=0;
            int job=0;
            int channel=0xff;
            char* nick = new (nothrow) char[P->Size-7];
            if(nick==NULL)
            {
                Log(MSG_ERROR, "Error allocing memory" );
                return false;
            }
            memcpy( nick, &P->Buffer[1], P->Size-7 );
            Log(MSG_INFO,"[CS] clan, %s is trying to down %s",thisclient->charname,nick);

            if(!CheckEscapeMySQL(nick,-1,true))
            {
                Log(MSG_WARNING,"A nick (down rank) contains incorrect caracter (see warnings above)");
                return false;
            }

            //LMA: no case.
            //if(strcmp(nick,thisclient->charname)==0)
            if(stricmp(nick,thisclient->charname)==0)
            {
                delete []nick;
                return true;
            }
            CCharClient* otherclient = GetClientByName( nick );
            if(otherclient!=NULL)
            {
                clan_rank = otherclient->clan_rank;
                if(otherclient->clan_rank>0)
                {
                    otherclient->clan_rank--;
                }
                else
                {
                    delete []nick;
                    return true;
                }
                job = otherclient->job;
                level = otherclient->level;
                channel = otherclient->channel;
	            BEGINPACKET( pak, 0x7e0 );
	            ADDBYTE    ( pak, 0x83 );
	            ADDSTRING  ( pak, nick );
	            ADDBYTE    ( pak, 0x00 );
	            ADDBYTE    ( pak, otherclient->clan_rank );
                otherclient->SendPacket(&pak);
            }
            else
            {                   //    0      1        2
                result = DB->QStore("SELECT clan_rank,classid,level FROM characters WHERE char_name='%s'",nick);
                //Bug
                //if(result==NULL);
                if(result==NULL)
                {
                    delete []nick;
                    return false;
                }
                if(mysql_num_rows(result)!=1)
                {
                    DB->QFree( );
                    Log(MSG_WARNING, "Invalid charname: %s" , nick );
                    delete []nick;
                    return false;
                }
	            row = mysql_fetch_row( result );
                clan_rank=atoi(row[0]);
                job = atoi(row[1]);
                level = atoi(row[2]);
                DB->QFree( );
            }
            if(clan_rank>0)
            {
                clan_rank--;
            }
            if(!DB->QExecute("UPDATE characters SET clan_rank=%i WHERE char_name='%s'",clan_rank,nick))
            {
                delete []nick;
                return false;
            }
            BEGINPACKET( pak, 0x7e0 );
            ADDBYTE    ( pak, 0x75 );
            ADDBYTE    ( pak, clan_rank );
            ADDBYTE    ( pak, channel );// channel
            ADDWORD    ( pak, 0x0000 );
            ADDWORD    ( pak, 0x0000 );
            ADDWORD    ( pak, level ); // level
            ADDWORD    ( pak, job ); // job
            ADDSTRING  ( pak, nick );
            ADDBYTE    ( pak, 0x00 );
            SendToClanMembers(thisclient->clanid,&pak);

	        RESETPACKET( pak, 0x7e1 );//update world information
	        ADDBYTE    ( pak, 0xfc );
	        ADDBYTE    ( pak, clan_rank );
	        ADDSTRING  ( pak, nick );
	        ADDBYTE    ( pak, 0x00 );
	        cryptPacket( (char*)&pak, NULL );
 	        CChanels* thischannel = GetChannelByID( channel );
 	        if(thischannel!=NULL)
        	    send( thischannel->sock, (char*)&pak, pak.Size, 0 );
        	delete []nick;
        }
        break;
        case 0x06://Change news
        {
            if(thisclient->clan_rank<5)
                return true;
            char* news = new (nothrow) char[P->Size-7];
            if(news==NULL)
            {
                Log(MSG_ERROR, "Error allocing memory" );
                return false;
            }
            memcpy( news, &P->Buffer[1], P->Size-7 );
            CClans* thisclan = GetClanByID(thisclient->clanid);
            if(thisclan!=NULL)
            {
                //LMA: checking size.
                if(strlen(news)>200)
                {
                    Log(MSG_WARNING,"Clan ID %u news is too long (%s), %u",thisclan->id,news,strlen(news));
                    return false;
                }

                strcpy(thisclan->news,news);
                BEGINPACKET( pak, 0x7e0 );
                ADDBYTE    ( pak, 0x34 );
                ADDSTRING  ( pak, thisclan->news );
                ADDBYTE    ( pak, 0x00 );
                SendToClanMembers(thisclan->id,&pak);

                /*if(!DB->QExecute("UPDATE list_clan SET news='%s' WHERE id=%i", thisclan->news, thisclan->id))
                     return false;
                     */

                //LMA: escaping (and more).
                char * lma_news = new char[strlen(news) + 1];
                strcpy(lma_news,news);
                char * new_news = new char[strlen(lma_news)*3 +1];
                mysql_real_escape_string(DB->Mysql, new_news,lma_news,strlen(lma_news));

                if(strlen(new_news)>=200)
                {
                    Log(MSG_WARNING,"Clan %i (%s) tried to put a too long news %s (%i>200)",thisclan->id,thisclan->name,new_news,strlen(new_news));
                    delete[] lma_news;
                    delete[] new_news;
                    delete []news;
                    return true;
                }

                if(!DB->QExecute("UPDATE list_clan SET news='%s' WHERE id=%i", new_news, thisclan->id))
                {
                    delete[] lma_news;
                    delete[] new_news;
                    delete []news;
                    return false;
                }

                delete[] lma_news;
                delete[] new_news;
            }
            delete []news;
        }
        break;
        case 0x07://Leave Clan
        {
           if( thisclient->clan_rank == 6 )
                return true;
           CClans* thisclan = GetClanByID( thisclient->clanid );
           if(thisclan==NULL)
                return true;
           BEGINPACKET( pak, 0x7e0 );
	       ADDBYTE( pak, 0x82 );
	       ADDSTRING( pak, thisclient->charname );
	       ADDBYTE( pak, 0x00);
           SendToClanMembers(thisclient->clanid,&pak);

            for(UINT i=0;i<thisclan->ClanMembers.size();i++)
            {
                CClanMembers* ClanMember = thisclan->ClanMembers.at( i );

                //LMA: no case.
                //if(strcmp(ClanMember->name,thisclient->charname)==0)
                if(stricmp(ClanMember->name,thisclient->charname)==0)
                {
                    thisclan->ClanMembers.erase( thisclan->ClanMembers.begin()+i );
                    delete ClanMember;
                    break;
               }
            }
            thisclient->clanid = 0;
            thisclient->clan_rank=1;
            if(!DB->QExecute("UPDATE characters SET clanid=0 AND clan_rank=1 WHERE char_name='%s'",thisclient->charname))
                return false;
	        RESETPACKET( pak, 0x7e1 );//Update world clan information
	        ADDBYTE    ( pak, 0xfb );//action (leave)
	        ADDSTRING  ( pak, thisclient->charname );
	        ADDBYTE    ( pak, 0x00 );
	        cryptPacket( (char*)&pak, NULL );
 	        CChanels* thischannel = GetChannelByID( thisclient->channel );
 	        if(thischannel!=NULL)
        	    send( thischannel->sock, (char*)&pak, pak.Size, 0 );
            Log(MSG_INFO,"[CS] clan, %s leaves clan",thisclient->charname);
        }
        break;
        case 0x08:
        {
            pakClanMembers ( thisclient );
        }
        break;
        case 0x09://Give Master
        {
            int clan_rank=0;
            int level=0;
            int job=0;
            int channel=0xff;
            char* nick = new (nothrow) char[P->Size-7];
            if(nick==NULL)
            {
                Log(MSG_ERROR, "Error allocing memory" );
                return false;
            }
            memcpy( nick, &P->Buffer[1], P->Size-7 );

            if(!CheckEscapeMySQL(nick,-1,true))
            {
                Log(MSG_WARNING,"A nick (give master) contains incorrect caracter (see warnings above)");
                return false;
            }

            //LMA: no case.
            //if(strcmp(nick,thisclient->charname)==0)
            if(stricmp(nick,thisclient->charname)==0)
            {
                delete []nick;
                return true;
            }
            //Convert to master
            CCharClient* otherclient = GetClientByName ( nick );
            if(otherclient!=NULL)
            {
                clan_rank = 6;
                otherclient->clan_rank = clan_rank;
                job = otherclient->job;
                level = otherclient->level;
                channel = otherclient->channel;
	            BEGINPACKET( pak, 0x7e0 );
	            ADDBYTE    ( pak, 0x83 );
	            ADDSTRING  ( pak, nick );
	            ADDBYTE    ( pak, 0x00 );
	            ADDBYTE    ( pak, otherclient->clan_rank );
                otherclient->SendPacket(&pak);
            }
            else
            {                   //   0     1
                result = DB->QStore("SELECT classid,level FROM characters where char_name='%s'",nick);
                if(result==NULL)
                {
                    delete []nick;
                    return false;
                }
                if(mysql_num_rows(result)!=1)
                {
                    DB->QFree( );
                    Log(MSG_WARNING, "Invalid charname: %s" , nick );
                    delete []nick;
                    return false;
                }
	            row = mysql_fetch_row( result );
                job = atoi(row[0]);
                level = atoi(row[1]);
                DB->QFree( );
            }
            if(!DB->QExecute("UPDATE characters SET clan_rank=%i WHERE char_name='%s'",clan_rank,nick))
            {
                delete []nick;
                return false;
            }
            BEGINPACKET( pak, 0x7e0 );
            ADDBYTE    ( pak, 0x75 );
            ADDBYTE    ( pak, clan_rank );
            ADDBYTE    ( pak, channel );//canal
            ADDWORD    ( pak, 0x0000 );
            ADDWORD    ( pak, 0x0000 );
            ADDWORD    ( pak, level ); //level?
            ADDWORD    ( pak, job ); // job?
            ADDSTRING  ( pak, nick );
            ADDBYTE    ( pak, 0x00 );
            SendToClanMembers(thisclient->clanid,&pak);

	        RESETPACKET( pak, 0x7e1 );//update world information
	        ADDBYTE    ( pak, 0xfc );
	        ADDBYTE    ( pak, clan_rank );
	        ADDSTRING  ( pak, nick );
	        ADDBYTE    ( pak, 0x00 );
	        cryptPacket( (char*)&pak, NULL );
 	        CChanels* thischannel = GetChannelByID( channel );
 	        if(thischannel!=NULL)
        	    send( thischannel->sock, (char*)&pak, pak.Size, 0 );

            Log(MSG_INFO,"[CS] clan, %s makes %s master",thisclient->charname,nick);

            //Convert to commander
            clan_rank = 4;
            thisclient->clan_rank=clan_rank;
            job = thisclient->job;
            level = thisclient->level;
            channel = thisclient->channel;
	        RESETPACKET( pak, 0x7e0 );
	        ADDBYTE    ( pak, 0x83 );
	        ADDSTRING  ( pak, thisclient->charname );
	        ADDBYTE    ( pak, 0x00 );
	        ADDBYTE    ( pak, thisclient->clan_rank );
            thisclient->SendPacket(&pak);

            if(!DB->QExecute("UPDATE characters SET clan_rank=%i WHERE char_name='%s'",clan_rank,thisclient->charname))
            {
                delete []nick;
                return false;
            }
            RESETPACKET( pak, 0x7e0 );
            ADDBYTE    ( pak, 0x75 );
            ADDBYTE    ( pak, clan_rank );
            ADDBYTE    ( pak, channel );//canal
            ADDWORD    ( pak, 0x0000 );
            ADDWORD    ( pak, 0x0000 );
            ADDWORD    ( pak, level ); //level?
            ADDWORD    ( pak, job ); // job?
            ADDSTRING  ( pak, thisclient->charname );
            ADDBYTE    ( pak, 0x00 );
            SendToClanMembers(thisclient->clanid,&pak);

	        RESETPACKET( pak, 0x7e1 );//update world information
	        ADDBYTE    ( pak, 0xfc );
	        ADDBYTE    ( pak, clan_rank );
	        ADDSTRING  ( pak, thisclient->charname );
	        ADDBYTE    ( pak, 0x00 );
	        cryptPacket( (char*)&pak, NULL );
 	        thischannel = GetChannelByID( channel );
 	        if(thischannel!=NULL)
        	    send( thischannel->sock, (char*)&pak, pak.Size, 0 );
            delete []nick;
        }
        break;
        case 0x0a://disorganize clan
        {
            if(thisclient->clan_rank<6)
                return true;
            unsigned int clanid = thisclient->clanid;
            CClans* thisclan = GetClanByID(thisclient->clanid);
            if(thisclan==NULL)
                return true;
            if(thisclan->ClanMembers.size()>1)//Can't be deleted if there are members
                return true;
            if(!DB->QExecute("UPDATE characters SET clanid=0 AND clan_rank=1 WHERE clanid=%i",thisclient->clanid))
                return false;
            if(!DB->QExecute("DELETE FROM list_clan WHERE id=%i",thisclient->clanid))
                return false;
            BEGINPACKET( pak, 0x7e0 );
            ADDBYTE    ( pak, 0x51 );
            ADDBYTE    ( pak, 0x00 );
            SendToClanMembers(thisclient->clanid, &pak);
            thisclan->ClanMembers.clear( );
            //delete from clan list
            for(UINT i=0;i<ClanList.size( );i++)
            {
                CClans* clan = ClanList.at( i );
                if(thisclan == clan)
                {
                    ClanList.erase( ClanList.begin() + i );
                    delete clan;
                    break;
                }
            }
	        RESETPACKET( pak, 0x7e1 );//Update world clan information
	        ADDBYTE    ( pak, 0xfd );//action (disorg)
	        ADDWORD    ( pak, clanid );
	        //ADDWORD    ( pak, thisclient->charid );   //W->DW
	        ADDDWORD    ( pak, thisclient->charid );

 	        CChanels* thischannel = GetChannelByID( thisclient->channel );
 	        if(thischannel!=NULL)
 	        {
 	            send( thischannel->sock, (char*)&pak, pak.Size, 0 );
 	        }

            thisclient->clanid = 0;
            thisclient->clan_rank = 1;
            Log(MSG_INFO,"[CS] clan, %s disorg clan",thisclient->charname);
 	        //send( wct
        }
        break;
        case 0x0c://invitation accepted
        {
            char* nick = new (nothrow) char[P->Size-7];
            if(nick==NULL)
            {
                Log(MSG_ERROR, "Error allocing memory" );
                return false;
            }
            memcpy( nick, &P->Buffer[1], P->Size-7 );
            CCharClient* otherclient = (CCharClient*) GetClientByName(nick );
            if(otherclient!=NULL)
            {
                thisclient->clanid = otherclient->clanid;
                thisclient->clan_rank=1;
                if(!DB->QExecute("UPDATE characters SET clanid=%i AND clan_rank=1 WHERE char_name='%s'",thisclient->clanid,thisclient->charname))
                {
                    delete []nick;
                    return false;
                }
                CClans* thisclan = GetClanByID(thisclient->clanid);
                if(thisclan==NULL)
                {
                    delete []nick;
                    return true;
                }
                CClanMembers* newmember = new (nothrow) CClanMembers;
                if(newmember==NULL)
                {
                    Log( MSG_WARNING, "Error allocing memory" );
                    delete []nick;
                    return false;
                }

                newmember->id = thisclient->charid;
                strcpy(newmember->name,thisclient->charname);
                newmember->clan_rank = 1;
                thisclan->ClanMembers.push_back( newmember );
           }
           else
           {
                delete []nick;
                return true;
           }
           SendClanInfo(thisclient);
           pakClanMembers(thisclient);
           BEGINPACKET( pak, 0x7e0 );//xxx have invited to xxx
	       ADDBYTE( pak, 0x61 );
	       ADDSTRING( pak, thisclient->charname );
	       ADDBYTE( pak, 0x00);
	       ADDSTRING( pak, otherclient->charname );
	       ADDBYTE( pak, 0x00);
           SendToClanMembers(thisclient->clanid,&pak);

           RESETPACKET( pak, 0x7e0 );
           ADDBYTE    ( pak, 0x84 );
           ADDWORD    ( pak, thisclient->level );
           ADDWORD    ( pak, thisclient->job );
           ADDSTRING  ( pak, thisclient->charname );
           ADDBYTE    ( pak, 0x00 );
           SendToClanMembers(thisclient->clanid,&pak);

           Log(MSG_INFO,"[CS] clan, %s accepted invitation",otherclient->charname);

	       RESETPACKET( pak, 0x7e1 );//update clan info in world
	       ADDBYTE( pak, 0xfa );
	       //ADDWORD( pak, thisclient->charid );
	       ADDDWORD( pak, thisclient->charid ); //W->DW
	       ADDWORD( pak, thisclient->clanid );
	       cryptPacket( (char*)&pak, NULL );
 	       CChanels* thischannel = GetChannelByID( thisclient->channel );
 	       if(thischannel!=NULL)
        	    send( thischannel->sock, (char*)&pak, pak.Size, 0 );
 	       CChanels* otherchannel = GetChannelByID( otherclient->channel );
 	       if(otherchannel!=NULL && thischannel!=otherchannel)
                send( thischannel->sock, (char*)&pak, pak.Size, 0 );
           delete []nick;
        }
        break;
        case 0x0d://invitation not accepted
        {
            char* nick = new (nothrow) char[P->Size-7];
            if(nick==NULL)
            {
                Log(MSG_ERROR, "Error allocing memory" );
                return false;
            }
            memcpy( nick, &P->Buffer[1], P->Size-7 );
            CCharClient* otherclient = (CCharClient*) GetClientByName(nick );
            if(otherclient!=NULL)
            {
                BEGINPACKET( pak, 0x7e0 );
                ADDBYTE    ( pak, 0x0d );
                ADDSTRING  ( pak, thisclient->charname );
                ADDBYTE    ( pak, 0x00 );
                otherclient->SendPacket(&pak);
                Log(MSG_INFO,"[CS] clan, %s refused invitation",nick);
            }
            delete []nick;
        }
        break;
        case 0x0f:// Update Clan window with Level and Job
        {
            BEGINPACKET( pak, 0x7e0 );
            ADDBYTE    ( pak, 0x84 );
            ADDWORD    ( pak, GETWORD ((*P),1) );
            ADDWORD    ( pak, GETWORD ((*P),3) );
            ADDSTRING  ( pak, thisclient->charname );
            //ADDBYTE    ( pak, thisclient->channel );
            //LMA: not the channel in fact
            ADDBYTE    ( pak, 0x00 );

            SendToClanMembers(thisclient->clanid,&pak);
            Log(MSG_INFO,"[CS] clan, update clan window for %s, %i, %i, channel %i",thisclient->charname,GETWORD ((*P),1),GETWORD ((*P),3),thisclient->channel);
        }
        break;
        case 0xfa://message from worldserver to load the new clan information
        {
           int id = GETWORD((*P),1); // clanid
           /*int charid = GETWORD((*P),3);  // charid
           int clientid = GETWORD((*P),5); // clientid*/
            DWORD charid = GETDWORD((*P),3);  // charid
           int clientid = GETWORD((*P),7); // clientid

           CCharClient* otherclient = (CCharClient*) GetClientByID ( charid );
           if(otherclient==NULL)
               return true;
        	// Load all our clan information
        	            //     0    1  2    3    4   5   6       7
        	result = DB->QStore("SELECT id,logo,back,name,cp,grade,slogan,news,rankingpoints,siglogo FROM list_clan where id=%i",id);
            if(result == NULL) return false;
            if(mysql_num_rows(result)!=1)
            {
                DB->QFree( );
                Log(MSG_WARNING, "Invalid clan: %i" , id );
                return false;
            }
        	row = mysql_fetch_row(result);
        	//clan info
        	CClans* newclan = new CClans;
            assert(newclan);
        	newclan->id = atoi(row[0]);
        	newclan->logo = atoi(row[1]);
        	newclan->back = atoi(row[2]);
        	strncpy(newclan->name,row[3],16);
        	newclan->cp = atoi(row[4]);
        	newclan->grade = atoi(row[5]);
            strncpy(newclan->slogan,row[6],29);
            strcpy(newclan->news,row[7]);
            newclan->rankingpoints=atoi(row[8]);
            newclan->siglogo=atoi(row[9]);
            //DB->QFree( );
            //member info
            CClanMembers* newmember = new CClanMembers;
            newmember->id = otherclient->charid;
            //LMA: since it's clan creator, it's the master ;)
            newmember->clan_rank=6;
            strcpy(newmember->name,otherclient->charname);
            newclan->ClanMembers.push_back( newmember );
        	ClanList.push_back( newclan );
        	DB->QFree( );
            otherclient->clanid = id;
            otherclient->clan_rank = 6;
            //Send new clan information
            BEGINPACKET( pak, 0x7e0);
            ADDBYTE    ( pak, 0x30);
            ADDWORD    ( pak, clientid);
            ADDWORD    ( pak, 0x0100);//
            ADDWORD    ( pak, newclan->back );//clan background
            ADDWORD    ( pak, newclan->logo );//clanlogo
            ADDBYTE    ( pak, 0x01);//clan grade
            ADDBYTE    ( pak, otherclient->clan_rank);// clan rank
            for(int i=0;i<146;i++)//clan skills
                ADDBYTE ( pak, 0x00);
            ADDDWORD    ( pak, 0x00000000 );  //LMA 139+: Reward points
            ADDSTRING  ( pak, newclan->name);//clan name
            ADDBYTE    ( pak, 0x00);
            ADDWORD    ( pak, 0x0000 );
            ADDWORD    ( pak, 0x0000 );
            ADDWORD    ( pak, 0x0000 );
            ADDWORD    ( pak, 0x0000 );
            ADDSTRING  ( pak, newclan->slogan);//clan slogan
            ADDBYTE    ( pak, 0x00);
            otherclient->SendPacket(&pak);
            Log(MSG_INFO,"[CS] Clan load info %s",newclan->name);
        }
        break;
        case 0xfe:
             {
                  //LMA: adding clan points (bogus command)
                  //We have to reload player's information...
                  /*int lma_id = GETWORD((*P),1); // client ID
                  long int lma_points = GETDWORD((*P),3); // clan points (TOTAL)*/
                  DWORD lma_id = GETDWORD((*P),1); // client ID
                  long int lma_points = GETDWORD((*P),5); // clan points (TOTAL)
                CCharClient* otherclient = GetClientByID( lma_id );

                if(otherclient!=NULL)
                {
                     Log(MSG_INFO,"[CS] Forcing clan info for %u, %s, adding clan points %li",lma_id,otherclient->charname,lma_points);
                     //test
                     lma_mask(otherclient);
                     SendClanPoints(otherclient,0);
                     SendClanPoints(otherclient,lma_points);
                }
                else
                {
                    Log(MSG_INFO,"[CS] ERROR Forcing clan info for %u, not found, adding clan points %li!",lma_id,lma_points);
                }

             }
             break;
        case 0xff:
             {
                  //LMA: adding reward points (bogus command)
                  //We have to reload player's information...
                  DWORD lma_id = GETDWORD((*P),1); // client ID
                  long int lma_points = GETDWORD((*P),5); // reward points (TOTAL)
                CCharClient* otherclient = GetClientByID( lma_id );

                if(otherclient!=NULL)
                {
                     Log(MSG_INFO,"[CS] Forcing clan info for %u, %s, total points %li",lma_id,otherclient->charname,lma_points);
                     //test
                     lma_mask(otherclient);
                     SendRewardPoints(otherclient,0);
                     SendRewardPoints(otherclient,lma_points);
                }
                else
                {
                    Log(MSG_INFO,"[CS] ERROR Forcing clan info for %u, not found, total points %li!",lma_id,lma_points);
                }

             }
             break;
        case 0xfb:
            {
                //LMA: Used for Clan Grade upgrade...
                //int lma_id = GETWORD((*P),1); // char ID
                //int clan_grade = GETWORD((*P),3); // clan grade
                DWORD lma_id = GETWORD((*P),1); // char ID
                int clan_grade = GETWORD((*P),5); // clan grade
                CCharClient* otherclient = GetClientByID( lma_id );
                if(otherclient!=NULL)
                {
                    SendClanGrade (otherclient,clan_grade);

                    //TODO: TEST! Is it refreshed?
                    SendRewardPoints(otherclient,0);
                    //SendRewardPoints(otherclient,lma_points);
                    Log(MSG_INFO,"[CS] Forcing clan grade for %u, clan grade %i!",lma_id,clan_grade);
                }
                else
                {
                    Log(MSG_INFO,"[CS] ERROR Forcing clan grade for %u, not found, clan grade %i!",lma_id,clan_grade);
                }

            }
            break;

        default:
            Log( MSG_INFO, "Clan action unknow %i ", action);
        break;
    }
    return true;
}

// Send clan members
bool CCharServer::pakClanMembers ( CCharClient* thisclient )
{
	CClans* thisclan = (CClans*) GetClanByID( thisclient->clanid );
	Log(MSG_INFO,"[CS] in pakClanMembers");
	if(thisclan!=NULL)
	{
        BEGINPACKET( pak, 0x7e0 );
        ADDBYTE    ( pak, 0x72 );//Send clan members
        for(UINT i=0;i<thisclan->ClanMembers.size( );i++)
    	{
            CClanMembers* thismember = thisclan->ClanMembers.at( i );
            if(thismember==NULL)
                continue;
            CCharClient* otherclient = (CCharClient*) GetClientByID (thismember->id);
            if(otherclient!=NULL)
            {
               	Log(MSG_INFO,"[CS] in pakClanMembers online");
                ADDBYTE    ( pak, thismember->clan_rank ); //clan rank
                ADDBYTE    ( pak, otherclient->channel ); //channel (0x01 = channel 1)
                ADDWORD    ( pak, 0x0000 );
                ADDWORD    ( pak, 0x0000 );
                ADDWORD    ( pak, otherclient->level );// Level
                ADDWORD    ( pak, otherclient->job );// Job
                Log(MSG_INFO,"[CS] Clan, pakClanMembers online %s, channel %i, level %i, job %i",otherclient->charname,otherclient->channel,otherclient->level,otherclient->job);
            }
            else
            {
           	    Log(MSG_INFO,"[CS] in pakClanMembers offline");
                ADDBYTE    ( pak, thismember->clan_rank); //clan rank
                ADDBYTE    ( pak, 0xff ); //channel (0xff = offline)
                ADDWORD    ( pak, 0x0000 );
                ADDWORD    ( pak, 0x0000 );
                ADDWORD    ( pak, 0x0000 );// Level
                ADDWORD    ( pak, 0x0000 );// Job
                Log(MSG_INFO,"[CS] Clan, pakClanMembers OFFLINE %s",thismember->name);
            }
            ADDSTRING  ( pak, thismember->name );
            ADDBYTE    ( pak, 0x00 );
        }
        thisclient->SendPacket( &pak );
    }
    else
    {
        	Log(MSG_INFO,"[CS] in pakClanMembers not found");
        BEGINPACKET( pak, 0x7e0 );
        ADDBYTE    ( pak, 0x0f );
        ADDDWORD   ( pak, 0x00000001 );
        thisclient->SendPacket( &pak );
        Log(MSG_INFO,"[CS] Clan, pakClanMembers CLAN NOT FOUND %i",thisclient->clanid);
    }

   	Log(MSG_INFO,"[CS] out pakClanMembers");
    return true;
}

// Logout from clan
bool CCharServer::ClanLogout ( CCharClient* thisclient )
{
    Log(MSG_INFO,"[CS] ClanLogout %s",thisclient->charname);
    CClans* thisclan = (CClans*) GetClanByID(thisclient->clanid);
    if(thisclan!=NULL)
    {
        for(UINT i=0;i<thisclan->ClanMembers.size();i++)
        {
            CClanMembers* thismember = thisclan->ClanMembers.at( i );
            CCharClient* otherclient = (CCharClient*) GetClientByID ( thismember->id );
            if(otherclient!=NULL)
            {
                ChangeClanStatus (thisclient, otherclient, 0xff);//send channel here
            }
        }
    }
    return true;
}

// upload clanmark
bool CCharServer::pakUploadCM ( CCharClient* thisclient, CPacket* P )
{
    CClans* thisclan = GetClanByID( thisclient->clanid );
    if(thisclan==NULL) return true;
    FILE* fh = fopen( "clanmark/clanmark.cnt", "r+" );
    if(fh==NULL)
    {
        Log( MSG_WARNING, "Error opening clanmark counter file" );
        return true;
    }
    rewind(fh);
    unsigned int cmid = 0;// this will be our clanmark id
    unsigned int tcmid = 0;// this will be to update the id
    unsigned int real_logo=0; //LMA: the real logo to be sent to server
    fread( &cmid, 1, 4, fh );
    tcmid = cmid+1;
    rewind(fh);
    fwrite( &tcmid, 1, 4, fh );
    fclose(fh);
    thisclan->siglogo = cmid;
    thisclan->back = 0;
    char filename[30];
    sprintf( filename, "clanmark/%u.cm", cmid );

    //LMA: switched to binary mode
    fh = fopen( filename, "w+b" );
    if(fh==NULL)
    {
        Log( MSG_WARNING, "Error creating clanmark file" );
        return true;
    }
    rewind(fh);
    for(unsigned int i=0;i<P->Size-6;i++)
        fwrite( &P->Buffer[i], 1, 1, fh );
    fclose(fh);

    //LMA: getting real logo ID (more like a signature)
    fh = fopen( filename, "rb" );
    if(fh==NULL)
    {
        Log( MSG_WARNING, "Error opening clanmark for signature" );
        return true;
    }
    fread( &real_logo, 1, 2, fh );
    Log(MSG_INFO,"Signature detected is %u for file %i",real_logo,cmid);
    fclose(fh);

    thisclan->logo = real_logo;

    //LMA: the logo stored server side and the custom are different
    //DB->QExecute("UPDATE list_clan SET logo=%i,back=0 WHERE id=%i",cmid, thisclient->clanid );
    DB->QExecute("UPDATE list_clan SET siglogo=%i,back=0,logo=%i WHERE id=%i",cmid,real_logo,thisclient->clanid);

    //LMA: Downloading back to the client:
    pakDownloadCMNow(thisclient,cmid);

	BEGINPACKET( pak, 0x7e1 );//update clan info in world
	ADDBYTE    ( pak, 0xff );
	ADDWORD    ( pak, thisclient->clanid );

    //ADDDWORD   ( pak, cmid );
    ADDDWORD   ( pak, real_logo );

	cryptPacket( (char*)&pak, NULL );
	for(int i=0;i<ChannelList.size();i++)
        send( ChannelList.at(i)->sock, (char*)&pak, pak.Size, 0 );

    Log(MSG_INFO,"[CS] ClanMark uploaded %i for clan %i",cmid,thisclient->clanid);
    return true;
}

//LMA: special when clan mark upload.
bool CCharServer::pakDownloadCMNow ( CCharClient* thisclient, unsigned int cmid)
{
    Log(MSG_INFO,"[CS] ClanMark download (after upload) %i for clan %i",cmid,thisclient->clanid);
    char filename[30];
    sprintf( filename, "clanmark/%u.cm", cmid );
    FILE* fh = fopen( filename, "rb" );
    if(fh==NULL)
    {
        Log( MSG_WARNING, "Invalid clanmark ID %i", cmid );
        return true;
    }

    //LMA: switched to binary mode and checked if something actually read.
    long data_read=0;
    BEGINPACKET( pak, 0x7e7 );
    ADDDWORD   ( pak, thisclient->clanid );
    while(!feof(fh))
    {
        unsigned char charvalue = '\0';
        data_read=fread( &charvalue, 1 , 1, fh );

        if (data_read>0)
           ADDBYTE( pak, charvalue );
    }
    thisclient->SendPacket( &pak );
    fclose(fh);
    return true;
}

bool CCharServer::pakDownloadCM ( CCharClient* thisclient, CPacket* P )
{
    unsigned int clanid = GETDWORD((*P), 0 );
    //LMA: we use a new field because there is a logo and the logo stored server side
    //MYSQL_RES* result = DB->QStore("SELECT logo FROM list_clan WHERE id=%i",clanid );
    MYSQL_RES* result = DB->QStore("SELECT siglogo FROM list_clan WHERE id=%i",clanid );
    if(result==NULL) return true;
    if(mysql_num_rows(result)!=1) return true;
    MYSQL_ROW row = mysql_fetch_row(result);
    unsigned int cmid = atoi(row[0]);
    DB->QFree( );
    Log(MSG_INFO,"[CS] ClanMark download %i for clan %i",cmid,clanid);
    char filename[30];
    sprintf( filename, "clanmark/%u.cm", cmid );
    FILE* fh = fopen( filename, "rb" );
    if(fh==NULL)
    {
        Log( MSG_WARNING, "Invalid clanmark ID %i", cmid );
        return true;
    }

    //LMA: switched to binary mode and checked if something actually read.
    long data_read=0;
    BEGINPACKET( pak, 0x7e7 );
    ADDDWORD   ( pak, clanid );
    while(!feof(fh))
    {
        unsigned char charvalue = '\0';
        data_read=fread( &charvalue, 1 , 1, fh );

        if (data_read>0)
           ADDBYTE( pak, charvalue );
    }
    thisclient->SendPacket( &pak );
    fclose(fh);
    return true;
}

bool CCharServer::pakClanIconTime ( CCharClient* thisclient, CPacket* P ) //FORMAT UNKNOWN
{
    //LMA: Used when a ClanMark
    //returned nothing when downloaded.
    //d7 07 0b 02 06 1b 1f
    BEGINPACKET( pak, 0x7e8 );
    ADDBYTE    ( pak, 0xd7 );
    ADDBYTE    ( pak, 0x07 );
    ADDBYTE    ( pak, 0x0b );
    ADDBYTE    ( pak, 0x02 );
    ADDBYTE    ( pak, 0x06 );
    ADDBYTE    ( pak, 0x1b );
    ADDBYTE    ( pak, 0x1f );
    thisclient->SendPacket( &pak );


    return true;
}

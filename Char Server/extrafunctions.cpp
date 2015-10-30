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
#include "charserver.h"

// Search client by userid
CCharClient* CCharServer::GetClientByUserID( UINT userid )
{
    for(UINT i=0;i<ClientList.size( );i++)
	{
        CCharClient* client = (CCharClient*) ClientList.at( i );
        if(client->userid==userid && client->accesslevel != 0xffff)
        {
            return client;
        }
	}
    return NULL;
}

// Search the client by char id
CCharClient* CCharServer::GetClientByID( DWORD charid )
{
    for(UINT i=0;i<ClientList.size( );i++)
	{
        CCharClient* client = (CCharClient*) ClientList.at( i );
        if(client->charid==charid)
        {
            return client;
        }
	}
    return NULL;
}

// Search the client by char name
CCharClient* CCharServer::GetClientByName( char *name )
{
    for(UINT i=0;i<ClientList.size( );i++)
	{
        CCharClient* client = (CCharClient*) ClientList.at( i );

        //LMA: no case...
        //if(strcmp(client->charname, name)==0)
        if(stricmp(client->charname, name)==0)
        {
            return client;
        }
	}
    return NULL;
}

// Search the client by char name
CCharClient* CCharServer::GetClientByUserName( char *username )
{
    for(UINT i=0;i<ClientList.size( );i++)
	{
        CCharClient* client = (CCharClient*) ClientList.at( i );

        //LMA: no case...
        //if(strcmp(client->username, username)==0)
        if(stricmp(client->username, username)==0)
        {
            return client;
        }
	}
    return NULL;
}

// Search the clan by clan id
CClans* CCharServer::GetClanByID( int id )
{
    for(UINT i=0;i<ClanList.size();i++)
    {
        CClans* thisclan = ClanList.at( i );
		if (thisclan->id==id)
            return thisclan;
	}
	return NULL;
}

//     Send packet to all the clan members
bool CCharServer::SendToClanMembers( int clanid,CPacket* pak )
{
    CClans* thisclan = GetClanByID(clanid);
    if(thisclan==0)
        return false;
    for(UINT i=0;i<thisclan->ClanMembers.size();i++)
    {
        CClanMembers* thismember = thisclan->ClanMembers.at(i);
        CCharClient* otherclient = (CCharClient*) GetClientByID (thismember->id);
        if(otherclient!=0)
        {
            Log(MSG_INFO,"[CS] Sending intel to clan member %s",otherclient->charname);
            otherclient->SendPacket(pak);
        }
    }
    return true;
}

// Get channel by id
CChanels* CCharServer::GetChannelByID( unsigned int id )
{
    for(unsigned int i=0;i<ChannelList.size();i++)
    {
        CChanels* thischannel = ChannelList.at(i);
        if(thischannel->id==id)
            return thischannel;
    }
    return NULL;
}

// Returns the current time/date
unsigned long int CCharServer::GetServerTime( )
{
	// Get time/date and write it to the beginning of the row
	time_t rawtime;							// For time
	struct tm* timeinfo;					//    "
	time	  ( &rawtime );
	timeinfo  = localtime( &rawtime );
	unsigned long int uCurTime = 0;
	uCurTime += (timeinfo->tm_sec         * 1         );
	uCurTime += (timeinfo->tm_min         * 60        );
	uCurTime += (timeinfo->tm_hour        * 3600      );
	uCurTime += (timeinfo->tm_yday        * 86400     );
	uCurTime += ((timeinfo->tm_year-2000) * 86400*366 );
	return uCurTime;
}

//LMA: Count the number of userid (accounts) in the list.
int CCharServer::GetNbUserID( UINT userid )
{
    int nb_accounts=0;
    for(UINT i=0;i<ClientList.size( );i++)
	{
	    CCharClient* client = (CCharClient*) ClientList.at( i );
        if(client->userid==userid)
        {
            nb_accounts++;
            Log(MSG_INFO,"i=%i / %u , %s",i,ClientList.size( ),client->username);
            /*if (nb_accounts>1)
            {
                //Hack, in all cases.
                return nb_accounts;
            }*/

        }

	}

    return nb_accounts;
}

//LMA: Disconnectiong all avatars from an account
bool CCharServer::DiscoAllAvatars(UINT userid )
{
    CCharClient* otherclient = GetClientByUserID( userid );
    if(otherclient==NULL)
    {
        Log( MSG_WARNING, "Userid '%u' is not online (DiscoAllAvatars)", userid );
        return true;
    }

    otherclient->isLoggedIn = false;
    otherclient->isActive = false;
    BEGINPACKET( pak, 0x502 );
    ADDBYTE    ( pak, 1 );
    ADDDWORD   ( pak, userid );
    //ADDBYTE    ( pak, 0x00 );
    cryptPacket( (char*)&pak, NULL );
    CChanels* thischannel = GetChannelByID( otherclient->channel );
    if(thischannel!=NULL)
    {
        send( thischannel->sock, (char*)&pak, pak.Size, 0 );
    }


    return true;
}

//LMA: The goal is to check a string is made of valid chars and extensions.
bool CCharServer::CheckValidName(CCharClient* thisclient, const char* my_name)
{
    char mychar[65];
    int nb_car=0;

    nb_car=strlen(my_name);
    if (nb_car>=65||nb_car<=0)
    {
        Log(MSG_HACK,"%s tried to create a too long avatar %s (%i)",thisclient->charname,my_name,nb_car);
        return false;
    }

    sprintf (mychar,"%s",my_name);

    //uppercase time.
   for (int k=0;k<nb_car;k++)
    {
        int c=toupper(mychar[k]);
        mychar[k]=c;
    }

    const char* valid_chars="AZERTYUIOPQSDFGHJKLMWXCVBN0123456789";
    int i = strspn (mychar,valid_chars);

    if(i!=nb_car)
    {
        Log(MSG_HACK,"%s tried to create a not correct avatar name %s",thisclient->charname,my_name);
        return false;
    }

    //Now we test some not valid sentences, like GM, ADMIN and so on...
    //the [6] here is the max size of a content, for example ADMIN is 5 so you need to add an extra 1 for NULL, so 6.
    char str[][6] = { "GM" , "ADMIN" , "DEV" };
    int nb_to_check=3;  //nb of extensions to check (see above).
    for (int n=0 ; n<nb_to_check ; n++)
    {
        if(strstr(mychar,str[n])!=NULL)
        {
            Log(MSG_HACK,"%s tried to create an avatar with a reserved extension %s is in %s",thisclient->charname,str[n],my_name);
            return false;
        }

    }


    return true;
}

//LMA: escaping.
bool CCharServer::EscapeMySQL(const char* data,string & mystring,int nb_car_max,bool check_same)
{
    //checking data length
    if(nb_car_max!=-1)
    {
        if (strlen(data)>nb_car_max)
        {
            Log(MSG_WARNING,"Escape:: Data too big (%s) %u>%i",data,strlen(data),nb_car_max);
            return false;
        }
    }

    char * lma_username = new char[strlen(data) + 1];
    strcpy(lma_username,data);
    char * new_username = new char[strlen(data)*3 +1];
    mysql_real_escape_string(DB->Mysql, new_username,lma_username,strlen(lma_username));
    mystring.assign(new_username);
    delete[] lma_username;
    delete[] new_username;

    //Is data escaped the same as the non escaped? Useful for login for example.
    if(check_same&&strcmp(data,mystring.c_str())!=0)
    {
        Log(MSG_WARNING,"Escape:: is different (%s != %s)",data,mystring.c_str());
        return false;
    }

    return true;
}


//LMA: escaping (this version only checks escaped and unescaped versions of a string are the same).
bool CCharServer::CheckEscapeMySQL(const char* data,int nb_car_max,bool check_same)
{
    string mystring;

    //checking data length
    if(nb_car_max!=-1)
    {
        if (strlen(data)>nb_car_max)
        {
            Log(MSG_WARNING,"Escape:: Data too big (%s) %u>%i",data,strlen(data),nb_car_max);
            return false;
        }
    }

    char * lma_username = new char[strlen(data) + 1];
    strcpy(lma_username,data);
    char * new_username = new char[strlen(data)*3 +1];
    mysql_real_escape_string(DB->Mysql, new_username,lma_username,strlen(lma_username));
    mystring.assign(new_username);
    delete[] lma_username;
    delete[] new_username;

    //Is data escaped the same as the non escaped? Useful for login for example.
    if(check_same&&strcmp(data,mystring.c_str())!=0)
    {
        Log(MSG_WARNING,"Escape:: is different (%s != %s)",data,mystring.c_str());
        return false;
    }


    return true;
}

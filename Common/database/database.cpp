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

#include "database.h"
#include "../log.h"

// constructor
CDatabase::CDatabase( char* server , char* username, char* password, char* database, unsigned int port , MYSQL* mysql)
{
    Server = server;
    Username = username;
    Password = password;
    Database = database;
    Port = port;
    Mysql = mysql;
    SQLMutex = PTHREAD_MUTEX_INITIALIZER;
    mysql_init( Mysql );
}

// deconstructor
CDatabase::~CDatabase( )
{
     Log(MSG_INFO,"deconstructor");
}

// disconnect from mysql
void CDatabase::Disconnect( )
{
    Log(MSG_INFO,"Closing mysql");
    mysql_close( Mysql );
}

int CDatabase::Connect( )
{
    Log( MSG_INFO, "Connecting to MySQL" );
    if(!mysql_real_connect( Mysql, Server, Username, Password, Database,Port, NULL, 0 ))
    {
        Log( MSG_FATALERROR, "Error connecting to MySQL server: %s\n", mysql_error( Mysql ) );
        return -1;
    }
    if(!QExecute("SET SESSION interactive_timeout=%i", 2678400))
        Log( MSG_WARNING, "Can't change timeout session");
    else
        Log( MSG_INFO, "SQL timeout set to %i seconds", 2678400);

    return 0;
}

// reconnect to mysql
int CDatabase::Reconnect( )
{
    Log( MSG_QUERY, "DB Reconnect");
    if(!mysql_real_connect( Mysql, Server, Username, Password, Database,Port, NULL, 0 ))
    {
            Log( MSG_FATALERROR, "Error reconnecting to MySQL server: %s\n", mysql_error( Mysql ) );
            return -1;
    }
    if(!QExecute("SET SESSION interactive_timeout=%i", 2678400))
        Log( MSG_WARNING, "Can't change timeout session");
    else
        Log( MSG_INFO, "SQL timeout set to %i seconds", 2678400);

    Log( MSG_QUERY, "DB Reconnect Ok");


    return 0;
}


//LMA: Special Update case, returns nb affected rows.
int CDatabase::QExecuteUpdate( const char *Format,... )
{
    bool Qfail = true;
    //char query[1024];
    char query[80000];
	va_list ap;
    va_start( ap, Format );
	vsprintf( query, Format, ap );
	va_end  ( ap );
    Log( MSG_QUERY, "IN:: ExecU: %s",query );
    pthread_mutex_lock( &SQLMutex );
    while(Qfail)
    {
        if(mysql_query( Mysql, query )!=0)
        {
            Log( MSG_FATALERROR, "Could not execute query: %s", mysql_error( Mysql ) );
            if(Reconnect( )==-1)
            {
                Log( MSG_FATALERROR, "Could not execute query: %s", mysql_error( Mysql ) );
                pthread_mutex_unlock( &SQLMutex );
                return -1;
            }
            else Qfail = false;
        }
        else Qfail = false;
    }
    pthread_mutex_unlock( &SQLMutex );
    //Log( MSG_QUERY, "OUT ExecU");
    return mysql_affected_rows(Mysql);
}

// execute query
bool CDatabase::QExecute( const char *Format,... )
{
    bool Qfail = true;
    //char query[1024];
    //char query[1300];   //LMA: sometimes we need more...
    char query[80000];
	va_list ap;
    va_start( ap, Format );
	vsprintf( query, Format, ap );
	va_end  ( ap );
    Log( MSG_QUERY,"IN QExec:: %s",query );

    int no_err=0;
    no_err=pthread_mutex_lock( &SQLMutex );

    while(Qfail)
    {
        if(mysql_query( Mysql, query )!=0)
        {
            Log( MSG_FATALERROR, "Could not execute query: %s", mysql_error( Mysql ) );
            if(Reconnect( )==-1)
            {
                Log( MSG_FATALERROR, "Could not execute query: %s", mysql_error( Mysql ) );
                pthread_mutex_unlock( &SQLMutex );
                return false;
            }
            else Qfail = false;
        }
        else Qfail = false;
    }
    pthread_mutex_unlock( &SQLMutex );
    //Log( MSG_QUERY,"OUT:: QExec");
    return true;
}

MYSQL_RES* CDatabase::QStore( const char *Format, ...)
{
    bool Qfail = true;
    //char query[1024];
    char query[80000];
	va_list ap;
    va_start( ap, Format );
	vsprintf( query, Format, ap );
	va_end  ( ap );
    result = NULL;
    Log( MSG_QUERY,"IN Qstore:: %s",query );
    pthread_mutex_lock( &SQLMutex );

    if (Mysql==NULL)
    {
      Log(MSG_INFO,"mysql is NULL");
    }

    while(Qfail)
    {
        if(mysql_query( Mysql, query )!=0)
        {
            Log( MSG_FATALERROR, "Could not execute query: %s", mysql_error( Mysql ) );
            if(Reconnect( )==-1)
            {
                Log( MSG_FATALERROR, "Could not execute query: %s", mysql_error( Mysql ) );
                pthread_mutex_unlock( &SQLMutex );
                //return false;
                return NULL;
            }
            else Qfail = false;
        }
        else Qfail = false;
    }
    result = mysql_store_result( Mysql );
    //Log( MSG_QUERY,"OUT Qstore");
    return result;
}

MYSQL_RES* CDatabase::QUse( const char *Format, ...)
{
    bool Qfail = true;
    //char query[1024];
    char query[80000];
	va_list ap;
    va_start( ap, Format );
	vsprintf( query, Format, ap );
	va_end  ( ap );
    result = NULL;
    Log( MSG_QUERY,"IN Quse:: %s",query );
    pthread_mutex_lock( &SQLMutex );
    while(Qfail)
    {
        if(mysql_query( Mysql, query )!=0)
        {
            Log( MSG_FATALERROR, "Could not execute query: %s", mysql_error( Mysql ) );
            if(Reconnect( )==-1)
            {
                Log( MSG_FATALERROR, "Could not execute query: %s", mysql_error( Mysql ) );
                pthread_mutex_unlock( &SQLMutex );
                //return false;
                return NULL;
            }
            else Qfail = false;
        }
        else Qfail = false;
    }
    result = mysql_use_result( Mysql );
    //Log( MSG_QUERY,"OUT Quse");
    return result;
}

void CDatabase::QFree( )
{
    //Log( MSG_QUERY,"IN Qfree");
    if(result!=NULL)
    {

        mysql_free_result( result );
    }
    else
    {
        Log(MSG_QUERY,"Result was NULL");
    }

    result=NULL;
    pthread_mutex_unlock( &SQLMutex );
    //Log( MSG_QUERY,"OUT Qfree");
}

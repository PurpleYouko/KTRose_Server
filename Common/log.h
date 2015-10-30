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
#ifndef __ROSE_LOGGING_FUNCTIONS__
#define __ROSE_LOGGING_FUNCTIONS__

// Log path
//LMA: we add another dir for packets logs.
#ifdef _WIN32
#	define LOG_DIRECTORY "log/"
#	define PLOG_DIRECTORY "plog/"
#endif

#ifdef __unix__
#	define LOG_DIRECTORY "/var/log/roseserver/"
#	define PLOG_DIRECTORY "/var/log/roseserver/p/"
#endif

#define LOG_LOGIN_SERVER		0x01
#define LOG_CHARACTER_SERVER	0x02
#define LOG_WORLD_SERVER     	0x03
#define LOG_SAME_FILE           0x04

// Log File names
extern unsigned char LOG_THISSERVER;
extern bool PRINT_LOG;
#define LOG_FILENAME_LOGINFILE  "loginserver.log"
#define LOG_FILENAME_CHARFILE   "charserver.log"
#define LOG_FILENAME_WORLDFILE  "worldserver.log"
#define LOG_DEFAULT_FILE        "server.log"
#define LOG_FILENAME_ERRORFILE  "errors.log"

#define LOG_LOGINPACKETS "loginpacket.log"
#define LOG_CHARPACKETS  "charpackets.log"
#define LOG_WORLDPACKETS "worldpackets.log"
#define LOG_DEFAULTPACKETS "packets.log"

//LMA: new debug message types, using Windows stack, you can use dbgview.exe
//to get the messages. Simply do something like (quite like Log command):
//DEBUGF("this is a test message %s ver %i","osrose",282);
//Nothing is logged in files using those commands!

//Comment define DEBUGMSG to disable it.
//#define DEBUGMSG	//PY this is preventing compiling in VS 2010 so commented it out
#ifdef DEBUGMSG
    #ifdef _DEBUG
        //gdb eats those messages in debug mode, so we handle them differently.
        //btw, _DEBUG is defined in CB debug project option.
        //this one gives the file and line as well as the msg.
        #define DEBUGF(a...)    do{char buffer[1024]; \
                                    char bufferRem[200]; \
                                   snprintf(buffer,1024, "%s (F:%s, L:%i)",__FILE__, __FUNCTION__,__LINE__); \
                                   snprintf(bufferRem,200, ##a); \
                                   snprintf(buffer,1024,"%s, %s",buffer,bufferRem); \
                                   Log(MSG_DEBUG,"%s",buffer);}while(0)
        //More simple debug output.
        #define DEBUGS(a...)    do{char buffer[200]; \
                                   snprintf(buffer,200, ##a); \
                                   Log(MSG_DEBUG,"%s",buffer);}while(0)
    #else
        //this one gives the file and line as well as the msg.
        #define DEBUGF(a...)    do{char buffer[1024]; \
                                    char bufferRem[200]; \
                                   snprintf(buffer,1024, "%s (F:%s, L:%i)",__FILE__, __FUNCTION__,__LINE__); \
                                   snprintf(bufferRem,200, ##a); \
                                   snprintf(buffer,1024,"%s, %s",buffer,bufferRem); \
                                   OutputDebugString(buffer);}while(0)
        //More simple debug output.
        #define DEBUGS(a...)    do{char buffer[200]; \
                                   snprintf(buffer,200, ##a); \
                                   OutputDebugString(buffer);}while(0)
    #endif
#else
    //if define DEBUGMSG is commented, the commands are trapped but nothing is outputed.
//    #define DEBUGF(a...)
//    #define DEBUGS(a...)
	// PY Still refuses to compile in VS2010 though so I commented these out too
#endif
//LMA end.

// Error Types
enum msg_type {
	MSG_NONE,
	MSG_STATUS,
	MSG_SQL,
	MSG_INFO,
	MSG_NOTICE,
	MSG_WARNING,
	MSG_DEBUG,
	MSG_ERROR,
	MSG_FATALERROR,
	MSG_HACK,
	MSG_LOAD,
	MSG_SDEBUG,
	MSG_GMACTION,
	MSG_START,
	MSG_QUERY,
	MSG_CONSOLE
};

// Logging function
void Log( msg_type flag, const char* Format, ... );


extern FILE *fhSp1;
void LogSp( enum msg_type flag, const char *Format, ... );
void LogHandleSp(int type);
void LogDebugPriority(int priority=3, bool warning=false);
void LogDebug(const char *Format, ... );


#endif

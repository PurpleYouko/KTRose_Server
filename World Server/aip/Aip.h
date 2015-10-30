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

// Props to ExJam for this code :]
class CAip
{
    public:
    	struct SAipDatum
        {
    		int size;
    		int opcode;
    		byte* data;
    	};

    	dword AipID;
    	dword minDamage;
	    dword minTime;
	    byte recordcount[6];

    	dword ConditionCount;
    	SAipDatum** Conditions;

    	dword ActionCount;
    	int offset_ltb;
    	int offset_qsd_trigger;
    	SAipDatum** Actions;
    	int AipVar;
    private:
};
#define AI_FAILURE 8
#define AI_SUCCESS 7
#include "AiConditions.h"
#include "AiActions.h"

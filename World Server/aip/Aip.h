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
    
    	dword AInumber;
        dword AipID;
    	dword minDamage;
	    dword minTime;
	    byte recordcount[6];
    
    	dword ConditionCount;
    	SAipDatum** Conditions;
    
    	dword ActionCount;
    	SAipDatum** Actions;
    	int AipVar;
    private:
};
#define AI_FAILURE 8
#define AI_SUCCESS 7
#include "AiConditions.h"
#include "AiActions.h"

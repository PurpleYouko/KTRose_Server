class CQuestTrigger {
public:
	struct SQuestDatum {
		int size;
		int opcode;
		byte* data;
	};
	
	string TriggerName;
	dword TriggerHash;

	byte CheckNext;

	dword ConditionCount;
	SQuestDatum** Conditions;

	dword ActionCount;
	SQuestDatum** Actions;
private:
};

struct STR_NPCVAR {
	dword iNpcNo;
	word nVarNo;
	word unused;
};

struct STR_ITEM_DATA {
	dword uiItemSN;
	dword iWhere;
	dword iRequestCnt;
	byte btOp;
};

struct STR_QUEST_DATA {
	union {
		dword iType;
		struct {
			word m_wVarNO;
			word m_wVarTYPE;
		};
	};
	word nValue;
	byte btOp;
};

struct STR_ABIL_DATA {
	dword iType;
	int iValue;
	byte btOp;
};

#define QUEST_FAILURE 0
#define QUEST_SUCCESS 1

int OperateValues(byte btOp, int* value1PTR, int value2);

#include "QuestConditions.h"
#include "QuestActions.h"

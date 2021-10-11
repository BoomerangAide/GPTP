#include <SCBW/structures/CUnit.h>

namespace hooks {

u32 getAttackPriorityHook(CUnit* target, CUnit* attacker);	//42160
CUnit* findRandomAttackTargetHook(CUnit* unit);				//42FC0
CUnit* findBestAttackTargetHook(CUnit* unit);				//43080

void injectAttackPriorityHooks();

} //hooks

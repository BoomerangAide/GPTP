#pragma once
#include "../../SCBW/structures/CUnit.h"

namespace hooks {

Bool32 SpiderMine_EnemyProc(CUnit* spiderMine, CUnit* target);	//40EC0
void orders_VultureMine(CUnit* unit);							//63DF0

void injectSpiderMineHooks();

} //hooks

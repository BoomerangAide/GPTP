#pragma once
#include "../../../SCBW/structures/CUnit.h"

namespace hooks {

void orders_Patrol(CUnit* unit);	//780F0

void injectPatrolOrderHook();

} //hooks
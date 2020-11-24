#pragma once
#include "../../../SCBW/structures/CUnit.h"

namespace hooks {

void function00455F20(CUnit* unit, u32 orderUnitType, int x, int y);	//55F20
void orders_RightClickAction(CUnit* unit);								//F6EF0

void injectRightClickOrderHooks();

} //hooks
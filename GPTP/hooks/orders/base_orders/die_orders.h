#pragma once
#include "../../../SCBW/structures/CUnit.h"

namespace hooks {

void orders_SelfDestructing(CUnit* unit);	//75710
void orders_Die(CUnit* unit);				//79480

void injectDieOrdersHook();

} //hooks

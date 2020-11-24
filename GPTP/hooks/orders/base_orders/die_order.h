#pragma once
#include "../../../SCBW/structures/CUnit.h"

namespace hooks {

void orders_Die(CUnit* unit);		//79480

void injectDieOrderHook();

} //hooks

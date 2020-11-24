#pragma once
#include "../../SCBW/structures/CUnit.h"

namespace hooks {

void orders_RescuePassive(CUnit* unit);	//A1EF0

void injectRescuableOrderHook();

} //hooks

#pragma once
#include "../../SCBW/structures/CUnit.h"

namespace hooks {

void orders_RechargeShields2(CUnit* unit);	//93990
void orders_RechargeShields1(CUnit* unit);	//93DD0

void injectShieldRechargeOrdersHooks();

} //hooks
#pragma once
#include "../../SCBW/structures/CUnit.h"

namespace hooks {

void function_00493920(CUnit* unit);		//93920
void orders_RechargeShields2(CUnit* unit);	//93990
void orders_RechargeShields1(CUnit* unit);	//93DD0

void injectShieldRechargeOrdersHooks();

} //hooks

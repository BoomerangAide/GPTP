#pragma once
#include "../../SCBW/structures/CUnit.h"

namespace hooks {

void orders_Burrowed(CUnit* unit);	//E9860
void orders_Burrow(CUnit* unit);	//E9E60
void orders_Unburrow(CUnit* unit);	//EA670

void injectBurrowOrdersHooks();

} //hooks

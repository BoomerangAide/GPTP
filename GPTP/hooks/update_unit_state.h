#pragma once
#include <SCBW/structures/CUnit.h>

namespace hooks {

void updateUnitEnergy(CUnit* unit);		//EB4B0
void updateUnitStateHook(CUnit* unit);	//EC290

void injectUpdateUnitState();

} //hooks

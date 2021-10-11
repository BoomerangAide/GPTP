#pragma once
#include <SCBW/structures/CUnit.h>

namespace hooks {

u16 getUnitMaxEnergyHook(CUnit* const unit);	//91870

void injectUnitMaxEnergyHook();

} //hooks

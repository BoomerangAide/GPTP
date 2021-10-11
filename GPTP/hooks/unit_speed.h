#pragma once
#include "../SCBW/structures/CUnit.h"

namespace hooks {

u32 getModifiedUnitSpeedHook(CUnit* unit, u32 baseSpeed);	//7B5F0
u32 getModifiedUnitTurnSpeedHook(CUnit* unit);				//7B850
u32 getModifiedUnitAccelerationHook(CUnit* unit);			//7B8A0

void injectUnitSpeedHooks();

} //hooks

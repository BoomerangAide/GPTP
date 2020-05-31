#pragma once
#include "../../SCBW/structures/CUnit.h"

namespace hooks {

s32 BTNSCOND_NoNydusExit(CUnit* unit, u16 reqVar, u32 playerId);	//00428340
s32 BTNSCOND_Movement(CUnit* unit, u16 reqVar, u32 playerId);		//004283C0
s32 BTNSCOND_HasScarabs(CUnit* unit, u16 reqVar, u32 playerId);		//004286A0
s32 BTNSCOND_CanBuildUnit(CUnit* unit, u16 reqVar, u32 playerId);	//00428E60

//The injector function that should be called in initialize.cpp
void injectBtnsCondHook();

} //hooks

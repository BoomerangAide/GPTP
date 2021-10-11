#pragma once
#include "../../SCBW/structures/CUnit.h"

namespace hooks {

s32 BTNSCOND_NoNydusExit(CUnit* unit, u16 reqVar, u32 playerId);	//28340
s32 BTNSCOND_Movement(CUnit* unit, u16 reqVar, u32 playerId);		//283C0
s32 BTNSCOND_HasScarabs(CUnit* unit, u16 reqVar, u32 playerId);		//286A0
s32 BTNSCOND_CanBuildUnit(CUnit* unit, u16 reqVar, u32 playerId);	//28E60
s32 BTNSCOND_IsCloaked(CUnit* unit, u16 reqVar, u32 playerId);		//29210
s32 BTNSCOND_CanCloak(CUnit* unit, u16 reqVar, u32 playerId);		//292C0

void injectBtnsCondHook();

} //hooks

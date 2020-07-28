#pragma once
#include "../SCBW/structures/CUnit.h"

namespace hooks {

void CMDACT_RightClick(s16 someX, s16 someY);												//563A0
void function_004564E0(s16 someX, s16 someY);												//564E0
void CMDACT_RightClickOrder(CUnit* unit, s16 x, s16 y, u16 unitId, Bool8 isQueuedOrder);	//C0380

void injectRightClickCMDACT_Hooks();

} //hooks

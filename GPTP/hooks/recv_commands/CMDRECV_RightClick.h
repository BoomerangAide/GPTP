#pragma once
#include "../../SCBW/structures/CUnit.h"

namespace hooks {

void CMDRECV_RightClick_Proc(s16 x, s16 y, CUnit* target, u16 wUnitType, u8 bCommandType);		//560D0

void injectCMDRECV_RightClickHook();

} //hooks

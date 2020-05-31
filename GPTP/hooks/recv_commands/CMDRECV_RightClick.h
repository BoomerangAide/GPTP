#pragma once
#include "../../SCBW/structures/CUnit.h"

namespace hooks {

u32 returnNothing2(CUnit* unit, int x, int y, CUnit** target, u32 unitId);						//55650
u32 returnNormalMove_NormalAttack_Order(CUnit* unit, int x, int y, CUnit** target, u32 unitId);	//55900
u32 returnNormalMove_NoAttack_Order(CUnit* unit, int x, int y, CUnit** target, u32 unitId);		//55E00
u32 returnNoMove_NormalAttack_Order(CUnit* unit, int x, int y, CUnit** target, u32 unitId);		//55890
u32 returnHarvest_Order(CUnit* unit, int x, int y, CUnit** target, u32 unitId);					//559C0
u32 returnHarvestAndRepair_Order(CUnit* unit, int x, int y, CUnit** target, u32 unitId);		//55CC0

void CMDRECV_RightClick_Proc(s16 x, s16 y, CUnit* target, u16 wUnitType, u8 bCommandType);		//560D0

void injectCMDRECV_RightClickHooks();

} //hooks

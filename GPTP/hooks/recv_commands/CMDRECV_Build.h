#pragma once
#include "../../SCBW/structures/CUnit.h"

namespace hooks {

bool CMDRECV_PlaceBuildingAllowed(CUnit* builder, u8 orderId, u16 builtUnitId);					//8DBD0
void cmdRECV_PlaceBuildingOrder(CUnit* builder, u32 builtUnitId, Point16 coords, u32 orderId);	//8DE70
void cmdRECV_PlaceBuildingNormal(s16 x, s16 y, u32 orderId, u32 builtUnitId);					//8E010
void cmdRECV_PlaceBuildingAddon(s16 x, s16 y, u32 orderId, u32 builtUnitId);					//8E0A0
void CMDRECV_Build(u8 orderId, s16 x, s16 y, u16 builtUnitId);									//C23C0

void injectCMDRECV_BuildHooks();

} //hooks

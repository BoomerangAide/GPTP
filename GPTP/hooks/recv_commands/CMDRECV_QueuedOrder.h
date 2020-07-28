#pragma once
#include "../../SCBW/structures/CUnit.h"

namespace hooks {

void queueNewOrder(CUnit* unit, u32 orderId, int x, int y, CUnit* target, u32 unitId,
						Point16 unkPos1, Point16 unkPos2, u32 unkQueuedOrderType);						//74B90
void function_004754F0(CUnit* unit, u32 orderId, int x, int y, CUnit* target, u32 unitId,
							Point16 unkPos1, Point16 unkPos2, Bool32 isQueued, u32 unkQueuedOrderType); //754F0

void injectCMDRECV_QueuedOrderHooks();

} //hooks
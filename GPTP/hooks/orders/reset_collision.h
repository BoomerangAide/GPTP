#pragma once
#include "../../SCBW/structures/CUnit.h"

namespace hooks {

void orders_ResetCollision2(CUnit* unit);	//2E320
void orders_ResetCollision1(CUnit* unit);	//671B0

void injectResetCollisionHooks();

} //hooks
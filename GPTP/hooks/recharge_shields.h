#pragma once
#include "../SCBW/structures/CUnit.h"

namespace hooks {

Bool32 unitCanRechargeShieldsHook(CUnit* target, CUnit* battery);	//93520

void injectRechargeShieldsHooks();

} //hooks

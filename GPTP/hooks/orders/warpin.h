#pragma once
#include "../../SCBW/structures/CUnit.h"

namespace hooks {

void orders_Warpin(CUnit* unit);	//E4C70

void injectWarpinOrderHook();

} //hooks

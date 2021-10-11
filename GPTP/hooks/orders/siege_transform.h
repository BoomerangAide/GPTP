#pragma once
#include "../../SCBW/structures/CUnit.h"

namespace hooks {

void ordersTankMode(CUnit* unit);	//64AE0
void ordersSiegeMode(CUnit* unit);	//64BD0

void injectSiegeTransformHooks();

} //hooks

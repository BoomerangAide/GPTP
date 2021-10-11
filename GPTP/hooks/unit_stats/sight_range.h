#pragma once
#include <SCBW/structures/CUnit.h>

namespace hooks {

u32 getSightRangeHook(CUnit* unit, bool isForSpellCasting);	//E5B40

void injectSightRangeHook();

} //hooks

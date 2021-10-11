#pragma once
#include <SCBW/structures/CUnit.h>

namespace hooks {

u32 getMaxWeaponRangeHook(CUnit* unit, u8 weaponId);	//75870
u8 getSeekRangeHook(CUnit* unit);						//76000

void injectWeaponRangeHooks();

} //hooks

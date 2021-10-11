#pragma once
#include "../../SCBW/structures.h"

namespace hooks {

u32 getModifiedWeaponCooldownHook(CUnit* unit, u8 weaponId);	//75DC0

void injectWeaponCooldownHook();

} //hooks

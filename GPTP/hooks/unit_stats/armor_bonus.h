#pragma once
#include <SCBW/structures/CUnit.h>

namespace hooks {

u8 getArmorBonusHook(CUnit* unit);

void injectArmorBonusHook();

} //hooks

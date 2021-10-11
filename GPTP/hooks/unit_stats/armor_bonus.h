#pragma once
#include <SCBW/structures/CUnit.h>

namespace hooks {

u8 getArmorBonusHook(CUnit* unit);	//53FC0

void injectArmorBonusHook();

} //hooks

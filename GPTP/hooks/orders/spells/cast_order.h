#pragma once
#include "../../../SCBW/structures/CUnit.h"

namespace hooks {

void orders_Spell(CUnit* unit);	//92850

void injectCastOrderHooks();

} //hooks

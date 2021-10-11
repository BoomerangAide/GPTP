#pragma once
#include "../SCBW/structures/CUnit.h"

namespace hooks {

Bool32 attackApplyCooldown(CUnit* unit);	//78B40

void injectAttackAndCooldownHook();

} //hooks
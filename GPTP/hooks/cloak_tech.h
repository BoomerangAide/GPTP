#pragma once
#include "../SCBW/structures/CUnit.h"

namespace hooks {

u8 getCloakingTech(CUnit* unit);	//91A50

void injectCloakingTechHooks();

} //hooks

#pragma once
#include "../../SCBW/structures/CUnit.h"

namespace hooks {

void orders_cloakNearbyUnits(CUnit* unit);	//93A80

void injectCloakNearbyUnitsOrderHook();

} //hooks

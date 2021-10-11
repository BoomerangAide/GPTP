#pragma once
#include "../../../SCBW/structures/CUnit.h"

namespace hooks {

u16 statdata_UnitWireframeUpdate_Fragment(CUnit* unit);

void injectWireframeHook();

} //hooks

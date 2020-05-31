#pragma once
#include "../../../SCBW/structures/CUnit.h"

namespace hooks {

u16 statdata_UnitWireframeUpdate_Fragment(CUnit* unit);

//The injector function that should be called in initialize.cpp
void injectWireframeHook();

} //hooks

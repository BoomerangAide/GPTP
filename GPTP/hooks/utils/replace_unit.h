#pragma once
#include "../../SCBW/structures.h"

namespace hooks {

void replaceUnitWithType(CUnit* unit, u16 newUnitId);	//0049FED0

void injectReplaceUnitWithTypeHook();

} //hooks
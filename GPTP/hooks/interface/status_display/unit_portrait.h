#pragma once
#include "../../../SCBW/structures.h"

namespace hooks {

	u16 setBuildingSelPortrait(CUnit* unit, u16 unitId);	//00426C60

	void injectUnitPortraitHook();

}
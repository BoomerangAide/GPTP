#pragma once
#include "../../../SCBW/structures.h"

namespace hooks {
	
	u16 setBuildingSelPortrait(CUnit* unit, u16 unitId);	//26C60
	void displayUpdatePortraitHack(u16* portraitId, CUnit** unit, u32* unkParam);

	void injectUnitPortraitHooks();

}

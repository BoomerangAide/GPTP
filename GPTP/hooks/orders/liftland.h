#pragma once
#include "../../SCBW/structures/CUnit.h"

namespace hooks {

	void orders_BuildingLand(CUnit* unit);		//64360
	void orders_BuildingLiftoff(CUnit* unit);	//649B0

	void injectLiftLandHooks();

} //hooks

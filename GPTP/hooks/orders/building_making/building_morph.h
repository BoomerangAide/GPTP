#pragma once
#include "../../../SCBW/structures/CUnit.h"

namespace hooks {

	bool isMorphing(CUnit* building);			//5CD00
	void orders_ZergBuildSelf(CUnit* building);	//5D500
	void ZergPlaceBuilding(CUnit* drone);		//5DA40

	void injectBuildingMorphHooks();

} //hooks

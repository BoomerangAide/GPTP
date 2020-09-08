#pragma once
#include "../../../SCBW/structures/CUnit.h"

namespace hooks {

void orders_TerranBuildSelf(CUnit* building);	//67760
void orders_SCVBuild2(CUnit* unit);				//67A70
void orders_SCVBuild(CUnit* unit);				//67FD0

void injectBuildingTerranHook();

} //hooks

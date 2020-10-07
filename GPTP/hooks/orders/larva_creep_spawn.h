#pragma once
#include "../../SCBW/structures/CUnit.h"

namespace hooks {

	void function_0049D660(CUnit* main_building);					//9D660
	bool function_004E8C80(CUnit* unit, CUnit* main_building);		//E8C80
	void orders_StopCreepGrowth(CUnit* unit);						//E95E0
	void orders_InitCreepGrowth(CUnit* unit);						//E96D0	
	void secondaryOrd_SpawningLarva(CUnit* unit);					//EA780
	void secondaryOrd_SpreadCreepSpawningLarva(CUnit* unit);		//EA880

	//The injector function that should be called in initialize.cpp
	void injectLarvaCreepSpawnHooks();

} //hooks

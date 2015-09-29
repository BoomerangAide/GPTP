#pragma once
#include "../../SCBW/structures/CUnit.h"

namespace hooks {

	CUnit* templarMergePartner(CUnit* unit, CUnit** units_list, u32 units_list_length);	//004BFA80

	void orders_CompletingArchonSummon(CUnit* unit);									//00493B10
	void orders_WarpingDarkArchon(CUnit* unit);											//00494690
	void orders_WarpingArchon(CUnit* unit);												//004948B0

	//The injector function that should be called in initialize.cpp
	void injectMergeUnitsHooks();

} //hooks

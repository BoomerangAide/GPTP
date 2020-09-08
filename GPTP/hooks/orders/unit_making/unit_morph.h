#pragma once
#include "../../../SCBW/structures/CUnit.h"

namespace hooks {

	void orders_ZergBirth(CUnit* unit);	//5DD60
	void orders_Morph1(CUnit* unit);	//5DEA0

	void injectUnitMorphHooks();

} //hooks

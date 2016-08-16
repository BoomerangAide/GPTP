#pragma once
#include "../../SCBW/structures/CUnit.h"

namespace hooks {

	bool isInfestableUnit(CUnit* unit);
	bool unitCanInfest(CUnit* unit);

	//infestation order used by infestation victim
	void orderMorphIntoInfested(CUnit* unitInfested);

	//infestation order used by the unit that infest
	void orderInfestTarget(CUnit* unitInfesting);
	
	//The injector function that should be called in initialize.cpp
	void injectInfestationHooks();

} //hooks

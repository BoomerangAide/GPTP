#pragma once
#include "../../SCBW/structures/CUnit.h"

namespace hooks {

	bool isInfestableUnit(CUnit* unit);					//02210
	bool unitCanInfest(CUnit* unit);					//02750

	//infestation order used by infestation victim
	void orderMorphIntoInfested(CUnit* unitInfested);	//EA4C0

	//infestation order used by the unit that infest
	void orderInfestTarget(CUnit* unitInfesting);		//EA290
	
	void injectInfestationHooks();

} //hooks

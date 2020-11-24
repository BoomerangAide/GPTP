#pragma once
#include "../../../SCBW/structures/CUnit.h"

namespace hooks {

	void secondaryOrd_TrainFighter(CUnit* unit);	//66790
	void function_00468420(CUnit* unit);			//68420

	void injectUnitTrainHooks();

} //hooks

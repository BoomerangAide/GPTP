#pragma once
#include "../../SCBW/structures/CUnit.h"

namespace hooks {

	u32 getLarvaeUnitsFromList(CUnit** unitList, CUnit** outputList);	//23190
	void BTNSACT_SelectLarva();											//23930

	void injectSelectLarvaHooks();

}

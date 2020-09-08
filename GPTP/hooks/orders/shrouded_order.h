#pragma once
#include "../../SCBW/structures/CUnit.h"

namespace hooks {

	void orders_Special(CUnit* unit);	//F6FA0

	void injectShroudedOrderHook();

} //hooks
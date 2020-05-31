#pragma once
#include "../../SCBW/structures/CUnit.h"

namespace hooks {

	bool canEnterNydusCanal(CUnit* unit, CUnit* nydusCanal);		//E8C20
	void enterNydusCanal_Effect(CUnit* unit, CUnit* nydusCanal);	//EA180
	void orders_EnterNydusCanal(CUnit* unit);						//EA3E0

	void injectEnterNydusHooks();

} //hooks

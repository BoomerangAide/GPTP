#pragma once
#include "../../../SCBW/structures/CUnit.h"

namespace hooks {

	void orders_Build5(CUnit* nydus_canal); //0x0045DC20

	void injectMakeNydusExitHook();

} //hooks
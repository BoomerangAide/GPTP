#pragma once
#include "../../SCBW/structures/CUnit.h"

namespace hooks {

void function_00466270(CUnit* carrier, CUnit* interceptor);	//66270 (not hooked, used directly)
void orders_Return(CUnit* unit);							//66350

void injectInterceptorReturnOrderHook();

} //hooks

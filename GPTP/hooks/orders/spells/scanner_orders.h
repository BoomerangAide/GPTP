#pragma once
#include "../../../SCBW/structures/CUnit.h"

namespace hooks {

void orders_Scanner(CUnit* unit);		//63D30
void orders_PlaceScanner(CUnit* unit);	//64E40

void injectScannerOrdersHook();

} //hooks
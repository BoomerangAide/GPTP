#pragma once
#include "../../../SCBW/structures/CUnit.h"

namespace hooks {

void ordersIDCases(CUnit* unit);		//EBED0
void secondaryOrdersRoot(CUnit* unit);	//EC170
void mainOrdersRoot(CUnit* unit);		//EC4D0

void injectOrdersRootHooks();

} //hooks

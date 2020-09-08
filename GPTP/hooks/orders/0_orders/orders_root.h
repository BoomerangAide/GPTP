#pragma once
#include "../../../SCBW/structures/CUnit.h"

namespace hooks {

void secondaryOrdersRoot(CUnit* unit);	//EC170
void mainOrdersRoot(CUnit* unit);		//EC4D0

void injectOrdersRootHooks();

} //hooks
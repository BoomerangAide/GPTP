#pragma once
#include "../SCBW/structures/CUnit.h"

namespace hooks {

void setRallyUnit(CUnit* unit, CUnit* target);			//66B40
void orderNewUnitToRally(CUnit* unit, CUnit* factory);	//66F50
void setRallyPosition(CUnit* unit, u16 x, u16 y);		//66910

void injectRallyHooks();

} //hooks

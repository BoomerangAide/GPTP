#pragma once
#include "../../SCBW/structures/CUnit.h"

namespace hooks {

void CMDRECV_BuildingMorph(u16 wUnitType);	//C1910
void CMDRECV_UnitMorph(u16 wUnitType);		//C1990

void injectCMDRECV_MorphHooks();

} //hooks

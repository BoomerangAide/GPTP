#pragma once
#include "../../SCBW/structures/CUnit.h"

namespace hooks {

void CMDRECV_Unburrow();				//C1AC0
void CMDRECV_Burrow(u8 bCommandType);	//C1FA0
void Unburrow_Proc(CUnit* unit);		//E97C0

void injectBurrowTechHooks();

} //hooks

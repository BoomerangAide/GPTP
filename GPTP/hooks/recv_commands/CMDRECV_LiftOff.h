#pragma once
#include "../../SCBW/structures/CUnit.h"

namespace hooks {

void CMDRECV_Liftoff(s16 x, s16 y);	//C1620

void injectCMDRECV_LiftOffHook();

} //hooks

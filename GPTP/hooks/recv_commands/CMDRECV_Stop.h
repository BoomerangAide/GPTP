#pragma once
#include "../../SCBW/structures/CUnit.h"

namespace hooks {

void CMDRECV_ReaverStop();			//C1240
void CMDRECV_CarrierStop();			//C1430
void CMDRECV_Stop(u8 bCommandType);	//C2190

void injectCMDRECV_StopHooks();

} //hooks

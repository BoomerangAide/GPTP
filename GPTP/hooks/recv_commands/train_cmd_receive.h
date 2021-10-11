#pragma once
#include "../../SCBW/structures/CUnit.h"

namespace hooks {

void CMDRECV_TrainFighter();		//C1800
void CMDRECV_Train(u16 wUnitType);	//C1C20

void injectTrainCmdRecvHooks();

} //hooks

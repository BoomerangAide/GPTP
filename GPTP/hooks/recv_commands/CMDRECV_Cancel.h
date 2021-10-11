#pragma once
#include "../../SCBW/structures/CUnit.h"

namespace hooks {

void CMDRECV_CancelNuke();				//BFCD0
void CMDRECV_CancelAddon();				//BFF30
void CMDRECV_CancelUpgrade();			//BFFC0
void CMDRECV_CancelResearch();			//C0070
void CMDRECV_CancelTrain(u16 wUnitID);	//C0100
void CMDRECV_CancelUnitMorph();			//C2EC0
void CMDRECV_CancelConstruction();		//C2EF0

void injectCMDRECV_CancelHooks();

} //hooks

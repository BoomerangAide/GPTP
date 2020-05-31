#pragma once
#include "../../SCBW/structures/CUnit.h"

namespace hooks {

void CMDRECV_Upgrade(u8 bUpgradeType);	//C1910
void CMDRECV_Research(u8 bTechType);	//C1990

void injectCMDRECV_ResearchUpgradeHooks();

} //hooks

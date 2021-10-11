#pragma once
#include "../../SCBW/structures/CUnit.h"

namespace hooks {

void CMDRECV_SaveGameAfter(char* savefileName);							//after C2910
void CMDRECV_LoadGameAfter(char* savefileName, bool loadSuccessful);	//after CF950

void injectCMDRECV_SaveLoadWrappersHooks();

} //hooks

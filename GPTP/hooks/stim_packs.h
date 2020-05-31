#pragma once
#include "../SCBW/structures/CUnit.h"

namespace hooks {

void CMDACT_Stimpack();					//234D0
void useStimPacksAIHook(CUnit* unit);	//554A0

void injectStimPacksHooks();

} //hooks

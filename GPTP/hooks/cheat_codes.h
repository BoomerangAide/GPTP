#pragma once
#include "../SCBW/structures/CUnit.h"

namespace hooks {

Bool32 campaignTypeCheatStrings(char* code_entered, u32* newCheatState);	//B1DC0
void cheatActivation(u32 newCheatState, char* playerName);					//B1FA0

void injectCheatCodesHooks();

} //hooks
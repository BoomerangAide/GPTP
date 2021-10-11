#pragma once
#include "../SCBW/structures/CUnit.h"

namespace hooks {

void CreateInitialMeleeBuildings(u8 raceId, u32 playerId);	//9D7C0
void CreateInitialMeleeUnits();								//9DA40
void CreateInitialOverlord(u8 playerId);					//9D6C0

void injectCreateInitUnitsHooks();

} //hooks

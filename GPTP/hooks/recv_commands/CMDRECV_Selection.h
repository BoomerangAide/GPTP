#pragma once
#include "../../SCBW/structures/CUnit.h"

namespace hooks {

void function_004965D0(u8 selectionGroup, Bool32 unkBoolean);						//965D0
void function_00496940(u32 bGroupSlot);												//96940
bool function_0049AF80(CUnit* unit, u32 playerId, u32 selection_slot);				//9AF80
void CMDRECV_ShiftSelect(u8 packetId, s8 bCount, StoredUnit* selectedCommandUnits);	//C2560
void CMDRECV_Select(u8 packetId, s8 bCount, StoredUnit* selectedCommandUnits);		//C2750
void CMDRECV_Hotkey(u8 packetId, u8 bGroupType, u8 bGroupSlot);						//C2870

void injectCMDRECV_SelectionHooks();

} //hooks

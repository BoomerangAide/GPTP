#pragma once
#include "../../SCBW/structures/CUnit.h"

namespace hooks {

void orders_CloseDoor(CUnit* unit);		//7BC50
void orders_OpenDoor(CUnit* unit);		//7BCD0
void orders_Disabledoodad(CUnit* unit);	//7BD60
void orders_Enabledoodad(CUnit* unit);	//7BE80
void orders_HiddenGun(CUnit* unit);		//7BF80
void orders_HideTrap(CUnit* unit);		//7C0A0
void orders_RevealTrap(CUnit* unit);	//7C1B0

void injectDoodadOrdersHooks();

} //hooks

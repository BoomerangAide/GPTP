#pragma once
#include "../../../SCBW/structures/CUnit.h"

namespace hooks {

void orders_CarrierIgnore2(CUnit* unit);	//66720
void orders_StayInRange(CUnit* unit);		//7C4F0
void orders_Follow(CUnit* unit);			//7C7B0
void orders_Move(CUnit* unit);				//7C950
void orders_MoveToInfestable(CUnit* unit);	//E98E0

void injectMoveOrdersHooks();

} //namespace hooks

#pragma once
#include "../../../SCBW/structures/CUnit.h"

namespace hooks {

void orders_MedicHoldPosition(CUnit* unit);		//64050
void orders_ReaverStop(CUnit* unit);			//654B0
void orders_CarrierStop(CUnit* unit);			//65910
void orders_Guard(CUnit* unit);					//75B90
void orders_PlayerGuard(CUnit* unit);			//774A0
void orders_TurretGuard(CUnit* unit);			//777F0
void orders_HoldPosition2(CUnit* unit);			//78D10
void orders_BunkerGuard(CUnit* unit);			//790A0
void makeToHoldPosition(CUnit* unit);			//EB5B0

void injectStopHoldPosOrdersHooks();

} //namespace hooks

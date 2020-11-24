#pragma once
#include "../../../SCBW/structures/CUnit.h"

namespace hooks {

void orders_NukePaint(CUnit* unit);		//63610
void orders_NukeLaunch(CUnit* unit);	//64730
void orders_NukeTrack(CUnit* unit);		//64D10
void orders_NukeGround(CUnit* unit);	//79200
void orders_NukeUnit(CUnit* unit);		//79410
void orders_NukeTrain(CUnit* unit);		//E6700

void injectNukeOrdersHooks();

} //hooks

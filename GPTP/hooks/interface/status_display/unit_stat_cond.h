#pragma once
#include "../../../SCBW/structures.h"

namespace hooks {

bool UnitStatCond_Never();			//24500
bool UnitStatCond_Powerup();		//24520
//bool UnitStatCond_Selection();	//defined directly in hooks\interface\status_display\unit_stat_selection instead
bool UnitStatCond_Standard();		//24980
bool UnitStatCond_Egg();			//24AC0
bool UnitStatCond_Archon();			//24AF0
bool UnitStatCond_CarrierReaver();	//24B50
bool UnitStatCond_Dropship();		//24F10
bool UnitStatCond_Building();		//25180
bool UnitStatCond_overlord();		//25900

bool function_00424780();			//24780
bool function_00424A10();			//24A10
bool function_00425230();			//25230

void injectUnitStatCondHooks();

}

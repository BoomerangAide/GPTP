#pragma once
#include "../../../SCBW/structures.h"

namespace hooks {

bool UnitStatCond_Never();			//00424500
bool UnitStatCond_Powerup();		//00424520
bool UnitStatCond_Standard();		//00424980
bool UnitStatCond_Egg();			//00424AC0
bool UnitStatCond_Archon();			//00424AF0
bool UnitStatCond_CarrierReaver();	//00424B50
bool UnitStatCond_Dropship();		//00424F10
bool UnitStatCond_Building();		//00425180
bool UnitStatCond_overlord();		//00425900

bool function_00424780();			//00424780
bool function_00424A10();			//00424A10
bool function_00425230();			//00425230

void injectUnitStatCondHooks();

}
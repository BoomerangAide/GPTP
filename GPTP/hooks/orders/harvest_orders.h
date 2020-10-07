#pragma once
#include "../../SCBW/structures/CUnit.h"

namespace hooks {

void orders_Harvest3(CUnit* unit);			//68E80
void orders_Harvest4(CUnit* unit);			//68ED0
void orders_HarvestMinerals2(CUnit* unit);	//68F60
void orders_HarvestGas2(CUnit* unit);		//69000
void orders_ReturnHarvest(CUnit* unit);		//690C0
void orders_HarvestMinerals1(CUnit* unit);	//69240
void orders_Harvest1(CUnit* unit);			//69500
void orders_HarvestMinerals3(CUnit* unit);	//697C0
void orders_HarvestGas3(CUnit* unit);		//69980

void injectHarvestOrdersHooks();

} //hooks

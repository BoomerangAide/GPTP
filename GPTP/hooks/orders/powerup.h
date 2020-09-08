#pragma once
#include "../../SCBW/structures/CUnit.h"

namespace hooks {

bool powerupCanBePickedUpProc(CUnit* worker, CUnit* powerup);	//40930
void killPowerup(CUnit* worker);								//F3C60
void pickupPowerup(CUnit* worker, CUnit* powerup);				//F3CA0
void orders_Powerup2(CUnit* powerup);							//F3E10
void orders_Powerup1(CUnit* powerup);							//F3EA0

void injectPowerupOrderHooks();

} //hooks

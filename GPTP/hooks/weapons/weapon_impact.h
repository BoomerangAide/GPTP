//The header file for the Weapon Impact hook module.
#pragma once
#include <SCBW/structures/CUnit.h>
#include <SCBW/structures/CBullet.h>

namespace hooks {

void weaponImpact(CBullet* bullet);	//8B770 CBullet_Damage

void injectWeaponImpactHook();

} //hooks

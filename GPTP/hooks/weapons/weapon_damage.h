#pragma once
#include "../../SCBW/structures/CUnit.h"

namespace hooks {

void killTargetUnitCheck(CUnit* target, s32 damage, CUnit* attacker, u32 attackingPlayerId, Bool32 bNotify);	//797B0

void weaponDamageHook(s32     damage,
                      CUnit*  target,
                      u8      weaponId,
                      CUnit*  attacker,
                      u8      attackingPlayerId,
                      s8      direction,
                      u8      dmgDivisor);	//79930

void injectWeaponDamageHook();

} //hooks

//Contains hooks that control unit movement speed, acceleration, and turn speed.

#include "unit_speed.h"
#include "../SCBW/enumerations.h"
#include "../SCBW/scbwdata.h"

namespace hooks {

/// Calculates the unit's modified movement speed, factoring in upgrades and status effects.
///
/// @return		The modified speed value.
/// Logically equivalent to GetModifiedUnitSpeed @ 0047B5F0  
u32 getModifiedUnitSpeedHook(CUnit* unit, u32 baseSpeed) {


	u32 speed = baseSpeed;

	int speedModifier = (unit->stimTimer ? 1 : 0) - (unit->ensnareTimer ? 1 : 0)
                      + (unit->status & UnitStatus::SpeedUpgrade ? 1 : 0);

	if (speedModifier > 0) {

		if (unit->id == UnitId::ProtossScout || unit->id == UnitId::Hero_Mojo || unit->id == UnitId::Hero_Artanis)
			speed = 1707;
		else {

			speed += (speed / 2);

			if (speed < 853)
				speed = 853;

		}

	}
	else
	if (speedModifier < 0)
		speed /= 2;

	return speed;

}

;

/// Calculates the unit's acceleration, factoring in upgrades and status effects.
///
/// @return		The modified acceleration value.
/// Logically equivalent to getModifiedUnitAcceleration @ 0047B8A0 
u32 getModifiedUnitAccelerationHook(CUnit* unit) {


	u32 acceleration = flingy_dat::Acceleration[units_dat::Graphic[unit->id]];

	int modifier = (unit->stimTimer ? 1 : 0) - (unit->ensnareTimer ? 1 : 0)
                 + (unit->status & UnitStatus::SpeedUpgrade ? 1 : 0);

	if (modifier > 0)
		acceleration *= 2;
	else 
	if (modifier < 0)
		acceleration = acceleration - (acceleration / 4);

	return acceleration;


}

;

/// Calculates the unit's turn speed, factoring in upgrades and status effects.
///
/// @return		The modified turning speed value.
/// Logically equivalent to getModifiedUnitTurnRadius @ 0047B850 
u32 getModifiedUnitTurnSpeedHook(CUnit* unit) {


	u32 turnSpeed = flingy_dat::TurnSpeed[units_dat::Graphic[unit->id]];

	int modifier = (unit->stimTimer ? 1 : 0) - (unit->ensnareTimer ? 1 : 0)
                 + (unit->status & UnitStatus::SpeedUpgrade ? 1 : 0);

	if (modifier > 0)
		turnSpeed *= 2;
	else
	if (modifier < 0)
		turnSpeed = turnSpeed - (turnSpeed / 4);

	return turnSpeed;

}

;

} //hooks

//Contains hooks that control attack range and seek range (AKA target acquisition range).

#include "weapon_range.h"
#include <SCBW/api.h>

namespace hooks {

/// Returns the modified max range for the weapon, which is assumed to be
/// attached to the given unit.
/// This hook affects the behavior of CUnit::getMaxWeaponRange().
/// Note: Weapon ranges are measured in pixels.
///
/// @param	weapon		The weapons.dat ID of the weapon.
/// @param	unit			The unit that owns the weapon. Use this to check upgrades.
u32 getMaxWeaponRangeHook(CUnit* unit, u8 weaponId) {
	//Default StarCraft behavior

	u32 bonusAmount = 0;

	//Give bonus range to units inside Bunkers
	if (unit->status & UnitStatus::InBuilding)
		bonusAmount = 64;

	switch (unit->id) {
		case UnitId::marine:
			if (scbw::getUpgradeLevel(unit->playerId, UpgradeId::U_238Shells))
				bonusAmount += 32;
			break;
		case UnitId::hydralisk:
			if (scbw::getUpgradeLevel(unit->playerId, UpgradeId::GroovedSpines))
				bonusAmount += 32;
			break;
		case UnitId::dragoon:
			if (scbw::getUpgradeLevel(unit->playerId, UpgradeId::SingularityCharge))
				bonusAmount += 64;
			break;
		case UnitId::fenix_dragoon:
			if (scbw::isBroodWarMode())
				bonusAmount += 64;
			break;
		case UnitId::goliath:
		case UnitId::goliath_turret:
			if (weaponId == WeaponId::HellfireMissilePack
					&& scbw::getUpgradeLevel(unit->playerId, UpgradeId::CharonBooster))
				bonusAmount += 96;
			break;
		case UnitId::alan_schezar:
		case UnitId::alan_schezar_turret:
			if (weaponId == WeaponId::HellfireMissilePack_AlanSchezar
					&& scbw::isBroodWarMode())
				bonusAmount += 96;
			break;
	}

	return weapons_dat::MaxRange[weaponId] + bonusAmount;

}

;

/// Returns the modified seek range (AKA target acquisition range) for the unit.
/// Note: Seek ranges are measured in matrices (1 matrix = 32 pixels).
/// This hook affects the behavior of CUnit::getSeekRange().
/// Equivalent to GetTargetAckRangeUpgrade @ 0x00476000  
u8 getSeekRangeHook(CUnit* unit) {

	u16 unitId = unit->id;
	u32 seekRange;

	//Cloaked ghosts do not voluntarily attack enemy units
	if 
	(
		(		
			unitId == UnitId::TerranGhost ||
			unitId == UnitId::Hero_SarahKerrigan ||
			unitId == UnitId::Hero_AlexeiStukov ||
			unitId == UnitId::Hero_SamirDuran ||
			unitId == UnitId::Hero_InfestedDuran
		) && 
		unit->status & (UnitStatus::Cloaked + UnitStatus::RequiresDetection) &&
		unit->mainOrderId != OrderId::HoldPosition2
	)
		seekRange = 0;
	else
	if(unitId > UnitId::Hero_FenixDragoon)
		seekRange = units_dat::SeekRange[unitId];
	else {

		u8 bonusAmount = 0;

		//Equivalent to original code
		switch (unitId) {

			case UnitId::TerranMarine:
				if (scbw::getUpgradeLevel(unit->playerId, UpgradeId::U_238Shells))
					bonusAmount = 1;
				break;
			case UnitId::ZergHydralisk:
				if (scbw::getUpgradeLevel(unit->playerId, UpgradeId::GroovedSpines))
					bonusAmount = 1;
				break;
			case UnitId::ProtossDragoon:
				if (scbw::getUpgradeLevel(unit->playerId, UpgradeId::SingularityCharge))
					bonusAmount = 2;
				break;
			case UnitId::Hero_FenixDragoon:
				if (scbw::isBroodWarMode())
					bonusAmount = 2;
				break;
			case UnitId::TerranGoliath:
			case UnitId::TerranGoliathTurret:
				if (scbw::getUpgradeLevel(unit->playerId, UpgradeId::CharonBooster))
					bonusAmount = 3;
				break;
			case UnitId::Hero_AlanSchezar:
			case UnitId::Hero_AlanSchezarTurret:
				if (scbw::isBroodWarMode())
					bonusAmount = 3;
				break;
			default:
				bonusAmount = 0;

		}

		seekRange = units_dat::SeekRange[unitId] + bonusAmount;

	}

	return seekRange;

}

;

} //hooks

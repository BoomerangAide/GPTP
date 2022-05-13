#include "armor_bonus.h"
#include <SCBW/api.h>

namespace hooks {

/// Returns the bonus armor for this unit.
///Should be equivalent to getArmorCount @ 00453FC0  
u8 getArmorBonusHook(CUnit* unit) {

	u8 armorUpg = 0;

	if (scbw::isBroodWarMode()) {
		if (unit->id == UnitId::Hero_Torrasque || unit->id == UnitId::ZergUltralisk) {
			if (
				(units_dat::BaseProperty[unit->id] & UnitProperty::Hero) || 
				scbw::getUpgradeLevel(unit->playerId, UpgradeId::ChitinousPlating)
			) 
				armorUpg = 2;
		}
	}

	return armorUpg + scbw::getUpgradeLevel(unit->playerId, units_dat::ArmorUpgrade[unit->id]);
}

}

#include "weapon_armor_tooltip.h"
#include <SCBW/api.h>
#include <Events/Events.h>

char buffer[128];

//Returns the special damage multiplier factor for units that don't use the
//"Damage Factor" property in weapons.dat.
u8 getDamageFactorForTooltip(u8 weaponId, CUnit* unit) {

	//Default StarCraft behavior
	if (
		unit->id == UnitId::TerranFirebat || 
		unit->id == UnitId::Hero_GuiMontag ||
		unit->id == UnitId::ProtossZealot || 
		unit->id == UnitId::Hero_FenixZealot
	)
		return 2;

	if (unit->id == UnitId::TerranValkyrie)
		return 1;

	return weapons_dat::DamageFactor[weaponId];
}

;

//Returns the C-string for the tooltip text of the unit's weapon icon.
//This function is used for weapon icons and special icons.
//Precondition: @p entryStrIndex is a stat_txt.tbl string index.
#ifdef EVENTS_SYSTEM
const char* getDamageTooltipString(u8 weaponId, CUnit* unit, u16 entryStrIndex) {

	char* entryName = (char*)statTxtTbl->getString(entryStrIndex);
	char* damageStr = (char*)statTxtTbl->getString(777);		//"Damage:"
	char* perRocketStr = (char*)statTxtTbl->getString(1301);	//"per rocket"

	const u8 damageFactor = getDamageFactorForTooltip(weaponId, unit);
	const u8 upgradeLevel = scbw::getUpgradeLevel(unit->playerId, weapons_dat::DamageUpgrade[weaponId]);
	const u16 baseDamage = weapons_dat::DamageAmount[weaponId] * damageFactor;
	const u16 bonusDamage = weapons_dat::DamageBonus[weaponId] * damageFactor * upgradeLevel;

	std::vector<int*> events_arg(7);

	events_arg[0] = (int*)unit;
	events_arg[1] = (int*)weaponId;
	events_arg[2] = (int*)&entryName;
	events_arg[3] = (int*)&damageStr;
	events_arg[4] = (int*)&baseDamage;
	events_arg[5] = (int*)&bonusDamage;

	if (weaponId != WeaponId::HaloRockets)
		perRocketStr = NULL;

	EventManager::EventCalled(EventId::STATUS_TOOLTIP_WEAPON, events_arg);

	if (perRocketStr != NULL) {

		if (bonusDamage > 0)
			sprintf_s(buffer, sizeof(buffer), "%s\n%s %d+%d %s",
								entryName, damageStr, baseDamage, bonusDamage, perRocketStr);
		else
			sprintf_s(buffer, sizeof(buffer), "%s\n%s %d %s",
								entryName, damageStr, baseDamage, perRocketStr);
	}
	else {
		if (bonusDamage > 0)
			sprintf_s(buffer, sizeof(buffer), "%s\n%s %d+%d",
								entryName, damageStr, baseDamage, bonusDamage);
		else
			sprintf_s(buffer, sizeof(buffer), "%s\n%s %d",
								entryName, damageStr, baseDamage);
	}

	return buffer;

}
#else
const char* getDamageTooltipString(u8 weaponId, CUnit* unit, u16 entryStrIndex) {

	char* entryName = (char*)statTxtTbl->getString(entryStrIndex);
	char* damageStr = (char*)statTxtTbl->getString(777);		//"Damage:"
	char* perRocketStr = (char*)statTxtTbl->getString(1301);	//"per rocket"

	const u8 damageFactor = getDamageFactorForTooltip(weaponId, unit);
	const u8 upgradeLevel = scbw::getUpgradeLevel(unit->playerId, weapons_dat::DamageUpgrade[weaponId]);
	const u16 baseDamage = weapons_dat::DamageAmount[weaponId] * damageFactor;
	const u16 bonusDamage = weapons_dat::DamageBonus[weaponId] * damageFactor * upgradeLevel;

	if (weaponId == WeaponId::HaloRockets) {

		if (bonusDamage > 0)
			sprintf_s(buffer, sizeof(buffer), "%s\n%s %d+%d %s",
								entryName, damageStr, baseDamage, bonusDamage, perRocketStr);
		else
			sprintf_s(buffer, sizeof(buffer), "%s\n%s %d %s",
								entryName, damageStr, baseDamage, perRocketStr);
	}
	else {
		if (bonusDamage > 0)
			sprintf_s(buffer, sizeof(buffer), "%s\n%s %d+%d",
								entryName, damageStr, baseDamage, bonusDamage);
		else
			sprintf_s(buffer, sizeof(buffer), "%s\n%s %d",
								entryName, damageStr, baseDamage);
	}

	return buffer;

}
#endif

;

namespace hooks {

//Returns the C-string for the tooltip text of the unit's weapon icon.
const char* getWeaponTooltipString(u8 weaponId, CUnit* unit) {
	return getDamageTooltipString(weaponId, unit, weapons_dat::Label[weaponId]);
}

;

//Returns the C-string for the tooltip text of the unit's armor icon.
#ifdef EVENTS_SYSTEM
const char* getArmorTooltipString(CUnit* unit) {
	
	const u16 labelId = upgrades_dat::Label[units_dat::ArmorUpgrade[unit->id]];
	const char *armorUpgradeName = statTxtTbl->getString(labelId);
	const char *armorStr = statTxtTbl->getString(778);						//"Armor:"

	const u8 baseArmor = units_dat::ArmorAmount[unit->id];
	const u8 bonusArmor = unit->getArmorBonus();

	std::vector<int*> events_arg(5);

	events_arg[0] = (int*)unit;
	events_arg[1] = (int*)&armorUpgradeName;
	events_arg[2] = (int*)&armorStr;
	events_arg[3] = (int*)&baseArmor;
	events_arg[4] = (int*)&bonusArmor;

	EventManager::EventCalled(EventId::STATUS_TOOLTIP_ARMOR, events_arg);

	if (bonusArmor > 0)
		sprintf_s(buffer, sizeof(buffer), "%s\n%s %d+%d",
							armorUpgradeName, armorStr, baseArmor, bonusArmor);
	else
		sprintf_s(buffer, sizeof(buffer), "%s\n%s %d",
							armorUpgradeName, armorStr, baseArmor);

	return buffer;

}
#else
const char* getArmorTooltipString(CUnit* unit) {
	
	const u16 labelId = upgrades_dat::Label[units_dat::ArmorUpgrade[unit->id]];
	const char *armorUpgradeName = statTxtTbl->getString(labelId);
	const char *armorStr = statTxtTbl->getString(778);						//"Armor:"

	const u8 baseArmor = units_dat::ArmorAmount[unit->id];
	const u8 bonusArmor = unit->getArmorBonus();

	if (bonusArmor > 0)
		sprintf_s(buffer, sizeof(buffer), "%s\n%s %d+%d",
							armorUpgradeName, armorStr, baseArmor, bonusArmor);
	else
		sprintf_s(buffer, sizeof(buffer), "%s\n%s %d",
							armorUpgradeName, armorStr, baseArmor);

	return buffer;

}
#endif

;

//Returns the C-string for the tooltip text of the plasma shield icon.
#ifdef EVENTS_SYSTEM
const char* getShieldTooltipString(CUnit* unit) {

	u16 labelId = upgrades_dat::Label[UpgradeId::ProtossPlasmaShields];
	char *shieldUpgradeName = (char*)statTxtTbl->getString(labelId);
	char *shieldStr = (char*)statTxtTbl->getString(779);					 //"Shields:"

	const u8 shieldUpgradeLevel = scbw::getUpgradeLevel(unit->playerId, UpgradeId::ProtossPlasmaShields);

	std::vector<int*> events_arg(5);

	u8 shieldBase = 0;

	events_arg[0] = (int*)unit;
	events_arg[1] = (int*)&shieldUpgradeName;
	events_arg[2] = (int*)&shieldStr;
	events_arg[3] = (int*)&shieldBase;
	events_arg[4] = (int*)&shieldUpgradeLevel;

	EventManager::EventCalled(EventId::STATUS_TOOLTIP_SHIELD, events_arg);

	if (shieldUpgradeLevel > 0)
		sprintf_s(buffer, sizeof(buffer), "%s\n%s %d+%d",
							shieldUpgradeName, shieldStr, shieldBase, shieldUpgradeLevel);
	else
		sprintf_s(buffer, sizeof(buffer), "%s\n%s %d",
							shieldUpgradeName, shieldStr, shieldBase);

	return buffer;

}
#else
const char* getShieldTooltipString(CUnit* unit) {

	u16 labelId = upgrades_dat::Label[UpgradeId::ProtossPlasmaShields];
	char *shieldUpgradeName = (char*)statTxtTbl->getString(labelId);
	char *shieldStr = (char*)statTxtTbl->getString(779);					 //"Shields:"

	const u8 shieldUpgradeLevel = scbw::getUpgradeLevel(unit->playerId, UpgradeId::ProtossPlasmaShields);

	if (shieldUpgradeLevel > 0)
		sprintf_s(buffer, sizeof(buffer), "%s\n%s %d+%d",
							shieldUpgradeName, shieldStr, 0, shieldUpgradeLevel);
	else
		sprintf_s(buffer, sizeof(buffer), "%s\n%s %d",
							shieldUpgradeName, shieldStr, 0);

	return buffer;

}
#endif

;

//Returns the C-string for the tooltip text of the Interceptor icon (Carriers),
//Scarab icon (Reavers), Nuclear Missile icon (Nuclear Silos), and Spider Mine
//icon (Vultures).
#ifdef EVENTS_SYSTEM
const char* getSpecialTooltipString(u16 iconUnitId, CUnit* unit) {

	char* return_value = (char*)StringEmpty;
	bool bCancelNormalBehavior = false;
	std::vector<int*> events_override_arg(3);

	events_override_arg[0] = (int*)unit;
	events_override_arg[1] = (int*)iconUnitId;
	events_override_arg[2] = (int*)&return_value;

	EventManager::EventCalling(EventId::STATUS_TOOLTIP_SPECIAL_OVERRIDE, &bCancelNormalBehavior, events_override_arg);

	if (!bCancelNormalBehavior) {

		if (iconUnitId == UnitId::ProtossInterceptor)
			return_value = (char*)getDamageTooltipString(WeaponId::PulseCannon, unit, 791);		//"Interceptors"
		else
		if (iconUnitId == UnitId::ProtossScarab)
			return_value = (char*)getDamageTooltipString(WeaponId::Scarab, unit, 792);			//"Scarabs"
		else
		if (iconUnitId == UnitId::TerranNuclearMissile)
			return_value = (char*)statTxtTbl->getString(793);									//"Nukes"
		else
		if (iconUnitId == UnitId::TerranVultureSpiderMine)
			return_value = (char*)getDamageTooltipString(WeaponId::SpiderMines, unit, 794);		//"Spider Mines"

		std::vector<int*> events_arg(3);

		events_arg[0] = (int*)unit;
		events_arg[1] = (int*)iconUnitId;
		events_arg[2] = (int*)&return_value;

		EventManager::EventCalled(EventId::STATUS_TOOLTIP_SPECIAL, events_arg);

	}

	return return_value;

}
#else
const char* getSpecialTooltipString(u16 iconUnitId, CUnit* unit) {
	
	if (iconUnitId == UnitId::ProtossInterceptor)
		return getDamageTooltipString(WeaponId::PulseCannon, unit, 791);		//"Interceptors"

	if (iconUnitId == UnitId::ProtossScarab)
		return getDamageTooltipString(WeaponId::Scarab, unit, 792);				 //"Scarabs"

	if (iconUnitId == UnitId::TerranNuclearMissile)
		return statTxtTbl->getString(793);										//"Nukes"

	if (iconUnitId == UnitId::TerranVultureSpiderMine)
		return getDamageTooltipString(WeaponId::SpiderMines, unit, 794);		//"Spider Mines"

	//Should never reach here
	return "";

}
#endif

;

} //hooks

#include "update_status_effects.h"
#include <SCBW/api.h>
#include <SCBW/UnitFinder.h>
#include "Events/Events.h"

#ifdef EVENTS_SYSTEM
class irradiateProc : public scbw::UnitFinderCallbackProcInterface {

	private:
		CUnit* irradiatedUnit;

	public:
		irradiateProc(CUnit* irradiatedUnit)
			: irradiatedUnit(irradiatedUnit) {}

	void proc(CUnit* unit) {

		std::vector<int*> events_unit_irradiate_splash_dmg_arg(2);

		//Damage organic units only
		if (!(units_dat::BaseProperty[unit->id] & UnitProperty::Organic))
			return;

		//Don't damage buildings
		if (units_dat::BaseProperty[unit->id] & UnitProperty::Building)
			return;

		//Don't damage larvae, eggs, and lurker eggs
		if (unit->id == UnitId::ZergLarva || unit->id == UnitId::ZergEgg || unit->id == UnitId::ZergLurkerEgg)
			return;

		//Irradiate splash damage does not affect burrowed units
		if (unit != irradiatedUnit && unit->status & UnitStatus::Burrowed)
			return;

		//Check if the unit is within distance, or is inside the same transport
		if (
			irradiatedUnit->status & UnitStatus::InTransport ||
			irradiatedUnit->getDistanceToTarget(unit) <= 32)
		{

			s32 damage = weapons_dat::DamageAmount[WeaponId::Irradiate] * 256 / weapons_dat::Cooldown[WeaponId::Irradiate];

			events_unit_irradiate_splash_dmg_arg[0] = (int*)unit;
			events_unit_irradiate_splash_dmg_arg[1] = (int*)&damage;

			EventManager::EventCalled(EventId::UNIT_IRRADIATE_SPLASH_DAMAGE_PRECALCULATION, events_unit_irradiate_splash_dmg_arg);

			unit->damageWith(damage, WeaponId::Irradiate, irradiatedUnit->irradiatedBy, irradiatedUnit->irradiatePlayerId);

		}
	}

};
#else
class irradiateProc : public scbw::UnitFinderCallbackProcInterface {

	private:
		CUnit* irradiatedUnit;

	public:
		irradiateProc(CUnit* irradiatedUnit)
			: irradiatedUnit(irradiatedUnit) {}

	void proc(CUnit* unit) {
		//Damage organic units only
		if (!(units_dat::BaseProperty[unit->id] & UnitProperty::Organic))
			return;

		//Don't damage buildings
		if (units_dat::BaseProperty[unit->id] & UnitProperty::Building)
			return;

		//Don't damage larvae, eggs, and lurker eggs
		if (unit->id == UnitId::ZergLarva || unit->id == UnitId::ZergEgg || unit->id == UnitId::ZergLurkerEgg)
			return;

		//Irradiate splash damage does not affect burrowed units
		if (unit != irradiatedUnit && unit->status & UnitStatus::Burrowed)
			return;

		//Check if the unit is within distance, or is inside the same transport
		if (
			irradiatedUnit->status & UnitStatus::InTransport ||
			irradiatedUnit->getDistanceToTarget(unit) <= 32)
		{
			const s32 damage = weapons_dat::DamageAmount[WeaponId::Irradiate] * 256 / weapons_dat::Cooldown[WeaponId::Irradiate];
			unit->damageWith(damage, WeaponId::Irradiate, irradiatedUnit->irradiatedBy, irradiatedUnit->irradiatePlayerId);
		}
	}

};
#endif

namespace hooks {

//Applies Irradiate effects for @p unit (which is Irradiated)
void doIrradiateDamage(CUnit* irradiatedUnit) {
	//Default StarCraft behavior

	irradiateProc irradiation = irradiateProc(irradiatedUnit);

	//No splash if burrowed
	if (irradiatedUnit->status & UnitStatus::Burrowed) {
		irradiation.proc(irradiatedUnit);
	}
	//If inside a transport, damage all units loaded within
	else 
	if (irradiatedUnit->status & UnitStatus::InTransport) {

		CUnit* transport = irradiatedUnit->connectedUnit;

		if (transport != NULL) {
			for (int i = 0; i < units_dat::SpaceProvided[transport->id]; ++i) {

				CUnit* loadedUnit = transport->getLoadedUnit(i);

				if (loadedUnit)
					irradiation.proc(loadedUnit);

			}
		}

	}
	//Find and iterate nearby units
	else {
		scbw::UnitFinder unitFinder(irradiatedUnit->getX() - 160,
									irradiatedUnit->getY() - 160,
									irradiatedUnit->getX() + 160,
									irradiatedUnit->getY() + 160);
		unitFinder.forEach(irradiation);
	}

}

;

//Hook function for UpdateStatusEffects() (AKA RestoreAllUnitStats())
//Note: This function is called every 8 ticks (when unit->cycleCounter reaches 8 == 0)

#ifdef EVENTS_SYSTEM
void updateStatusEffects(CUnit* unit) {

	if (unit->stasisTimer != 0) {

		unit->stasisTimer--;

		if (unit->stasisTimer == 0)
			unit->removeStasisField();

	}

	if (unit->stimTimer != 0) {

		unit->stimTimer--;

		if (unit->stimTimer == 0)
			unit->updateSpeed();

	}

	if (unit->ensnareTimer != 0) {

		unit->ensnareTimer--;

		if (unit->ensnareTimer == 0) {
			unit->removeOverlay(ImageId::EnsnareOverlay_Small, ImageId::EnsnareOverlay_Large);
			unit->updateSpeed();
		}

	}

	if (unit->defensiveMatrixTimer != 0) {

		unit->defensiveMatrixTimer--;

		if (unit->defensiveMatrixTimer == 0)
			unit->reduceDefensiveMatrixHp(unit->defensiveMatrixHp);

	}

	if (unit->irradiateTimer != 0) {

		unit->irradiateTimer--;
		doIrradiateDamage(unit);	//assumed to work, not checked in details

		if (unit->irradiateTimer == 0) {
			unit->removeOverlay(ImageId::Irradiate_Small, ImageId::Irradiate_Large);
			unit->irradiatedBy = NULL;
			unit->irradiatePlayerId = 8;
		}

	}

	if (unit->lockdownTimer != 0) {

		unit->lockdownTimer--;

		if (unit->lockdownTimer == 0)
			unit->removeLockdown();

	}

	if (unit->maelstromTimer != 0) {

		unit->maelstromTimer--;

		if (unit->maelstromTimer == 0)
			unit->removeMaelstrom();

	}

	if (unit->plagueTimer != 0) {

		unit->plagueTimer--;

		if (!(unit->status & UnitStatus::Invincible)) {

			s32 damage = weapons_dat::DamageAmount[WeaponId::Plague];
			std::vector<int*> events_unit_plague_dmg_arg(3);
			bool bApplyDamage;

			//randomize?
			__asm {
				PUSHAD
				MOV ECX, damage
				SHL ECX, 0x08
				MOV EAX, 0x6BCA1AF3
				IMUL ECX
				SAR EDX, 0x05
				MOV EAX, EDX
				SHR EAX, 0x1F
				ADD EAX, EDX
				MOV damage, EAX
				POPAD
			}

			bApplyDamage = unit->hitPoints > damage;

			events_unit_plague_dmg_arg[0] = (int*)unit;
			events_unit_plague_dmg_arg[1] = (int*)&damage;
			events_unit_plague_dmg_arg[2] = (int*)&bApplyDamage;

			EventManager::EventCalled(EventId::UNIT_PLAGUE_DAMAGE_PRECALCULATION, events_unit_plague_dmg_arg);

			if (bApplyDamage)
				unit->damageHp(damage,NULL,-1,true);

		}

		if (unit->plagueTimer == 0)
			unit->removeOverlay(ImageId::PlagueOverlay_Small, ImageId::PlagueOverlay_Large);

	}

	if (unit->isUnderStorm != 0)
		unit->isUnderStorm--;

	//hardcoding effect of unreferenced_sub_4F42C0 @ 0x004F42C0
	u8 previousAcidSporeCount = unit->acidSporeCount;
	
	for (int i = 0; i <= 8; ++i) {

		if (unit->acidSporeTime[i] != 0) {

			unit->acidSporeTime[i]--;

			if (unit->acidSporeTime[i] == 0)
				unit->acidSporeCount--;

		}

	}

	if (unit->acidSporeCount != 0) {

		//Calculate the appropriate overlay ID
		u32 acidOverlayId = std::min(3, unit->acidSporeCount / 2)
							+ 4 * scbw::getUnitOverlayAdjustment(unit)
							+ ImageId::AcidSpores_1_Overlay_Small;

		if (!unit->getOverlay(acidOverlayId)) {

			unit->removeOverlay(ImageId::AcidSpores_1_Overlay_Small, ImageId::AcidSpores_6_9_Overlay_Large);

			if (unit->subunit != NULL)
				unit = unit->subunit;

			unit->sprite->createTopOverlay(acidOverlayId);

		}

	}
	else
	if (previousAcidSporeCount != 0) {
		unit->removeOverlay(ImageId::AcidSpores_1_Overlay_Small, ImageId::AcidSpores_6_9_Overlay_Large);
	}

}
#else
void updateStatusEffects(CUnit* unit) {

	if (unit->stasisTimer != 0) {

		unit->stasisTimer--;

		if (unit->stasisTimer == 0)
			unit->removeStasisField();

	}

	if (unit->stimTimer != 0) {

		unit->stimTimer--;

		if (unit->stimTimer == 0)
			unit->updateSpeed();

	}

	if (unit->ensnareTimer != 0) {

		unit->ensnareTimer--;

		if (unit->ensnareTimer == 0) {
			unit->removeOverlay(ImageId::EnsnareOverlay_Small, ImageId::EnsnareOverlay_Large);
			unit->updateSpeed();
		}

	}

	if (unit->defensiveMatrixTimer != 0) {

		unit->defensiveMatrixTimer--;

		if (unit->defensiveMatrixTimer == 0)
			unit->reduceDefensiveMatrixHp(unit->defensiveMatrixHp);

	}

	if (unit->irradiateTimer != 0) {

		unit->irradiateTimer--;
		doIrradiateDamage(unit);	//assumed to work, not checked in details

		if (unit->irradiateTimer == 0) {
			unit->removeOverlay(ImageId::Irradiate_Small, ImageId::Irradiate_Large);
			unit->irradiatedBy = NULL;
			unit->irradiatePlayerId = 8;
		}

	}

	if (unit->lockdownTimer != 0) {

		unit->lockdownTimer--;

		if (unit->lockdownTimer == 0)
			unit->removeLockdown();

	}

	if (unit->maelstromTimer != 0) {

		unit->maelstromTimer--;

		if (unit->maelstromTimer == 0)
			unit->removeMaelstrom();

	}

	if (unit->plagueTimer != 0) {

		unit->plagueTimer--;

		if (!(unit->status & UnitStatus::Invincible)) {

			s32 damage = weapons_dat::DamageAmount[WeaponId::Plague];

			//randomize?
			__asm {
				PUSHAD
				MOV ECX, damage
				SHL ECX, 0x08
				MOV EAX, 0x6BCA1AF3
				IMUL ECX
				SAR EDX, 0x05
				MOV EAX, EDX
				SHR EAX, 0x1F
				ADD EAX, EDX
				MOV damage, EAX
				POPAD
			}

			if (unit->hitPoints > damage)
				unit->damageHp(damage,NULL,-1,true);

		}

		if (unit->plagueTimer == 0)
			unit->removeOverlay(ImageId::PlagueOverlay_Small, ImageId::PlagueOverlay_Large);

	}

	if (unit->isUnderStorm != 0)
		unit->isUnderStorm--;

	//hardcoding effect of unreferenced_sub_4F42C0 @ 0x004F42C0
	u8 previousAcidSporeCount = unit->acidSporeCount;
	
	for (int i = 0; i <= 8; ++i) {

		if (unit->acidSporeTime[i] != 0) {

			unit->acidSporeTime[i]--;

			if (unit->acidSporeTime[i] == 0)
				unit->acidSporeCount--;

		}

	}

	if (unit->acidSporeCount != 0) {

		//Calculate the appropriate overlay ID
		u32 acidOverlayId = std::min(3, unit->acidSporeCount / 2)
							+ 4 * scbw::getUnitOverlayAdjustment(unit)
							+ ImageId::AcidSpores_1_Overlay_Small;

		if (!unit->getOverlay(acidOverlayId)) {

			unit->removeOverlay(ImageId::AcidSpores_1_Overlay_Small, ImageId::AcidSpores_6_9_Overlay_Large);

			if (unit->subunit != NULL)
				unit = unit->subunit;

			unit->sprite->createTopOverlay(acidOverlayId);

		}

	}
	else
	if (previousAcidSporeCount != 0) {
		unit->removeOverlay(ImageId::AcidSpores_1_Overlay_Small, ImageId::AcidSpores_6_9_Overlay_Large);
	}

}
#endif

;

} //hooks

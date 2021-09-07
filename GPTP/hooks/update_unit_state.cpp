#include "update_unit_state.h"
#include "update_status_effects.h"
#include <SCBW/api.h>
#include <algorithm>

namespace {
//Helper function: Returns true if the unit's HP <= 33%.
bool unitHpIsInRedZone(CUnit* unit);				//022C0
void RestoreAllUnitStats(CUnit* unit);				//92F70 (hooked by hooks\update_status_effects.cpp)
void setAllImageGroupFlagsPal11(CSprite* sprite);	//97430
} //unnamed namespace

namespace hooks {

/// This function regenerates energy for spellcasters and drains energy for
/// cloaking units.
/// Equivalent to function sub_4EB4B0 @ 0x004EB4B0
void updateUnitEnergy(CUnit* unit) {

	//was hardcoded instead of function calling in original code
	if (
		unit->isValidCaster() &&				//If the unit is hallucination or not a spellcaster, don't regenerate energy
		(unit->status & UnitStatus::Completed)	//If the unit is not fully constructed, don't regenerate energy
	)
	{

		bool bStopHere = false;

		//Spend energy for cloaked units
		if (
			unit->status & (UnitStatus::Cloaked + UnitStatus::RequiresDetection) &&	//If the unit is cloaked
			!(unit->status & UnitStatus::CloakingForFree)						//...and must consume energy to stay cloaked (i.e. not under an Arbiter)
		)
		{

			u16 cloakingEnergyCost;

			//Equivalent to getCloakingEnergyConsumption @ 0x00491A90
			if (unit->id == UnitId::TerranGhost ||
				unit->id == UnitId::Hero_SarahKerrigan ||
				unit->id == UnitId::Hero_InfestedKerrigan ||
				unit->id == UnitId::Hero_SamirDuran ||
				unit->id == UnitId::Hero_InfestedDuran ||
				unit->id == UnitId::Hero_AlexeiStukov
			)
				cloakingEnergyCost = 10;
			else 
			if (unit->id == UnitId::TerranWraith ||
				unit->id == UnitId::Hero_TomKazansky
			)
				cloakingEnergyCost = 13;
			else
				cloakingEnergyCost = 0;

			if (!(unit->hasEnergy(cloakingEnergyCost)))
			{

				if (unit->secondaryOrderId == OrderId::Cloak)
					unit->setSecondaryOrder(OrderId::Nothing2); //Supposedly, immediately decloaks the unit.

				bStopHere = true;

			}

			if(!bStopHere)	//the function prevent energy consumption if THE GATHERING enabled
				unit->spendUnitEnergy(cloakingEnergyCost);

		}
		else { //EB526

			int maxEnergy;

			if (unit->id == UnitId::ProtossDarkArchon &&
				unit->mainOrderId == OrderId::CompletingArchonSummon &&
				unit->mainOrderState == 0
			)
				maxEnergy = 12800;  //50 * 256; Identical to energy amount on spawn
			else
				maxEnergy = unit->getMaxEnergy();

			if(unit->energy == maxEnergy)
				bStopHere = true;
			else {

				unit->energy += 8;

				//Omitting hardcoded recalculation of maxEnergy

				//Omitting recalculation of maxEnergy through
				//GetDarkArchonEnergy @ 0x00491C00 since it
				//calculate maxEnergy the same way the previous
				//code did, and nothing could have changed the
				//parameters used since the calculation
				if(unit->energy > maxEnergy)
					unit->energy = maxEnergy;

			}

		}

		//If the unit is currently selected, redraw its graphics
		if (!bStopHere && unit->sprite->flags & CSprite_Flags::Selected)
			setAllImageGroupFlagsPal11(unit->sprite);

	}

} 

;

/// Updates unit timers, regenerates hp and shields, and burns down Terran buildings.
/// Identical to function @ 0x004EC290
void updateUnitStateHook(CUnit* unit) {

	//Timers
	if (unit->mainOrderTimer)
		unit->mainOrderTimer--;
	if (unit->groundWeaponCooldown)
		unit->groundWeaponCooldown--;
	if (unit->airWeaponCooldown)
		unit->airWeaponCooldown--;
	if (unit->spellCooldown)
		unit->spellCooldown--;

	//Shield regeneration
	if (units_dat::ShieldsEnabled[unit->id] != 0) {

		s32 maxShields = (s32)(units_dat::MaxShieldPoints[unit->id]) * 256;

		if (unit->shields != maxShields) {

			unit->shields += 7;

			if (unit->shields > maxShields)
				unit->shields = maxShields;

			if (unit->sprite->flags & CSprite_Flags::Selected)  //If the unit is currently selected, redraw its graphics
				setAllImageGroupFlagsPal11(unit->sprite);

		}

	}

	//Supposedly, allows Zerglings to attack after unburrowing without delay.
	if (unit->id == UnitId::zergling || unit->id == UnitId::Hero_DevouringOne)
		if (unit->groundWeaponCooldown == 0)
		  unit->orderQueueTimer = 0;

	//Clear the healing flag every frame
	unit->isBeingHealed = 0;

	//Update unit status effects (stim, maelstrom, plague, etc.)
	if (unit->status & UnitStatus::Completed || !(unit->sprite->flags & CSprite_Flags::Hidden)) {

		unit->cycleCounter++;

		if (unit->cycleCounter >= 8) {
			unit->cycleCounter = 0;
			RestoreAllUnitStats(unit); //use update_status_effects hook if available
		}

	}

	//Only for fully-constructed units and buildings
	if (unit->status & UnitStatus::Completed) {

		bool bStopHere = false;

		//HP regeneration
		if (
			units_dat::BaseProperty[unit->id] & UnitProperty::RegeneratesHP &&
			unit->hitPoints > 0 &&
			unit->hitPoints != units_dat::MaxHitPoints[unit->id]
		)
			unit->setHp(unit->hitPoints + 4);

		//Update unit energy (energy regen/drain)
		//call StarCraft.exe+EB4B0		//sub_4EB4B0
		updateUnitEnergy(unit);

		//Recent order timer
		if (unit->recentOrderTimer != 0)
			unit->recentOrderTimer--;

		//Self-destruct timer
		if (unit->removeTimer != 0) {

			unit->removeTimer--;

			if (unit->removeTimer == 0) {
				unit->remove();
				bStopHere = true;
			}

		}

		//Terran building burn-down
		if (!bStopHere) {

			RaceId::Enum raceId = unit->getRace();

			if(
				raceId != RaceId::Zerg &&
				raceId != RaceId::Protoss &&
				raceId == RaceId::Terran
			)

				//Check if the unit is a grounded or lifted building
				if (	
					unit->status & UnitStatus::GroundedBuilding ||
					units_dat::BaseProperty[unit->id] & UnitProperty::FlyingBuilding)
				{

					//...whose current HP is less or equal to 33% of max HP
					if (unitHpIsInRedZone(unit))
						unit->damageHp(20, NULL, unit->lastAttackingPlayer,true);

				}

		} //if (unit->getRace() == RaceId::Terran)


	} //if (unit->status & UnitStatus::Completed) {

}

;

} //hooks

namespace {

/**** Helper function definitions. Do not modify anything below this! ****/

const u32 Helper_UnitHpIsInRedZone  = 0x004022C0;
bool unitHpIsInRedZone(CUnit* unit) {

  static Bool32 result;

	__asm {
		PUSHAD
		MOV ECX, unit
		CALL Helper_UnitHpIsInRedZone
		MOV result, EAX
		POPAD
	}

	return result != 0;
}

;

//hooked by hooks\update_status_effects.cpp
const u32 Func_RestoreAllUnitStats = 0x00492F70;
void RestoreAllUnitStats(CUnit* unit) {

	__asm {
		PUSHAD
		MOV EAX, unit
		CALL Func_RestoreAllUnitStats
		POPAD
	}

}

;

//Identical to setAllImageGroupFlagsPal11 @ 0x00497430;
void setAllImageGroupFlagsPal11(CSprite* sprite) {

	for(
		CImage* current_image = sprite->images.head; 
		current_image != NULL;
		current_image = current_image->link.next
	)
	{
		if(current_image->paletteType == PaletteType::RLE_HPFLOATDRAW)
			current_image->flags |= CImage_Flags::Redraw;
	}

}

;

}

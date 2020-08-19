#include "btns_cond.h"
#include "../../SCBW/api.h"

namespace hooks {

/// These hooks are a selection of buttons conditions
/// showing how they work, and allowing these cases
/// to be modified for a specific unit or situation.
/// Not many more buttons conditions should need to be hooked,
/// you should be able to use these for anything you need with
/// internal checks for various cases to handle.

s32 BTNSCOND_NoNydusExit(CUnit* unit, u16 reqVar, u32 playerId) {
	return (unit->building.nydusExit == NULL) ? BUTTON_STATE::Enabled : BUTTON_STATE::Invisible;
}

;

s32 BTNSCOND_Movement(CUnit* unit, u16 reqVar, u32 playerId) {

	s32 return_value = BUTTON_STATE::Enabled;

	for(int i = 0;return_value == BUTTON_STATE::Enabled && i < SELECTION_ARRAY_LENGTH;i++)
		if(clientSelectionGroup->unit[i] != NULL && clientSelectionGroup->unit[i]->status & UnitStatus::Burrowed)
			return_value = BUTTON_STATE::Invisible;

	return return_value;

}

;

s32 BTNSCOND_HasScarabs(CUnit* unit, u16 reqVar, u32 playerId) {

	s32 return_value = BUTTON_STATE::Disabled;

	for(int i = 0;return_value == BUTTON_STATE::Disabled && i < SELECTION_ARRAY_LENGTH;i++)
		if(	clientSelectionGroup->unit[i] != NULL && 
			(clientSelectionGroup->unit[i]->id == UnitId::Hero_Warbringer || clientSelectionGroup->unit[i]->id == UnitId::ProtossReaver) &&
			clientSelectionGroup->unit[i]->carrier.inHangarCount != 0
		)
			return_value = BUTTON_STATE::Enabled;

	return return_value;

}
	
;

s32 BTNSCOND_CanBuildUnit(CUnit* unit, u16 reqVar, u32 playerId) {

	s32 return_value = BUTTON_STATE::Invisible;

	if (
		*clientSelectionCount <= 1 ||
		unit->id == UnitId::ZergLarva ||
		unit->id == UnitId::ZergMutalisk ||
		unit->id == UnitId::ZergHydralisk
	)
		return_value = unit->canMakeUnit(reqVar, playerId);

	return return_value;

}
	
;

s32 BTNSCOND_IsCloaked(CUnit* unit, u16 reqVar, u32 playerId) {
	
	s32 returnValue = BUTTON_STATE::Enabled;
	CUnit* unit_from_selection;

	for(int i = 0; i < SELECTION_ARRAY_LENGTH && (returnValue == BUTTON_STATE::Enabled); i++) {

		unit_from_selection = clientSelectionGroup->unit[i];

		if(unit_from_selection != NULL) {

			u8 techId; 

			if (
				unit->id == UnitId::TerranGhost ||
				unit->id == UnitId::Hero_SarahKerrigan ||
				unit->id == UnitId::Hero_AlexeiStukov ||
				unit->id == UnitId::Hero_SamirDuran ||
				unit->id == UnitId::Hero_InfestedDuran ||
				unit->id == UnitId::Hero_InfestedKerrigan
			)
				techId = TechId::PersonnelCloaking;
			else
			if (unit->id == UnitId::TerranWraith || unit->id == UnitId::Hero_TomKazansky)
				techId = TechId::CloakingField;
			else
				techId = TechId::None;

			if (unit_from_selection->canUseTech(techId, playerId) == 1) {

				if (
					unit_from_selection->status & UnitStatus::CloakingForFree &&
					!(unit_from_selection->status & UnitStatus::Burrowed)
				)
					returnValue = BUTTON_STATE::Invisible;
				else
				if (!(unit_from_selection->status & UnitStatus::RequiresDetection))
					returnValue = BUTTON_STATE::Invisible;

			}
			else
				returnValue = BUTTON_STATE::Invisible;

		}

	}

	return returnValue;

}

;

s32 BTNSCOND_CanCloak(CUnit* unit, u16 reqVar, u32 playerId) {
	
	s32 returnValue = BUTTON_STATE::Invisible;
	CUnit* unit_from_selection;

	for(int i = 0; i < SELECTION_ARRAY_LENGTH && (returnValue == BUTTON_STATE::Invisible); i++) {

		unit_from_selection = clientSelectionGroup->unit[i];

		if(unit_from_selection != NULL) {

			u8 techId; 

			if (
				unit->id == UnitId::TerranGhost ||
				unit->id == UnitId::Hero_SarahKerrigan ||
				unit->id == UnitId::Hero_AlexeiStukov ||
				unit->id == UnitId::Hero_SamirDuran ||
				unit->id == UnitId::Hero_InfestedDuran ||
				unit->id == UnitId::Hero_InfestedKerrigan
			)
				techId = TechId::PersonnelCloaking;
			else
			if (unit->id == UnitId::TerranWraith || unit->id == UnitId::Hero_TomKazansky)
				techId = TechId::CloakingField;
			else
				techId = TechId::None;

			if(
				unit_from_selection->canUseTech(techId,playerId) == 1 &&			//must be able to use cloaking tech for more checks to be necessary
				(
					!(unit_from_selection->status & UnitStatus::CloakingForFree) ||	//to cloak, unit should not already be cloaked for free
					unit_from_selection->status & UnitStatus::Burrowed				//if burrowed, cloaking for free, but still allowed to cloak
				) &&
				!(unit_from_selection->status & UnitStatus::RequiresDetection)		//if unit doesn't already require detection
			)
				returnValue = BUTTON_STATE::Enabled;

		}

	}

	return returnValue;

}

;	

} //hooks

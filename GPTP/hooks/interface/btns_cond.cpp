#include "btns_cond.h"
#include "../../SCBW/api.h"

namespace hooks {

/// These hooks are a selection of buttons conditions
/// showing how they work, and allowing these cases
/// to be modified for a specific unit or situation.
/// No more buttons conditions will be hooked, so
/// use these for anything you need.

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

} //hooks

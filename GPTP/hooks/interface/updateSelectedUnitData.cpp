#include "updateSelectedUnitsData.h"
#include <SCBW/api.h>

//Helper functions declaration

namespace {

void updateButtonSet();											//58DE0
Bool32 compareUnitRank(CUnit* current_unit, CUnit* other_unit); //9A350

} //unnamed namespace

namespace hooks {

//Called in sub_458120 if *bCanUpdateSelectedUnitData is not 0,
//Called before calling updateSelectedUnitPortrait, updateCurrentButtonset
//and finally jumping into main data display function sub_458120
void updateSelectedUnitData() {

	CUnit** current_unit_offset = (CUnit**)clientSelectionGroup;
	CUnit* current_unit;
	CUnit* unit_at_selection_top = NULL;

	for(int i = 0; i < SELECTION_ARRAY_LENGTH; i++)
		clientSelectionGroup->unit[i] = activePlayerSelection->unit[i];

	*clientSelectionCount = 0;
	*activePortraitUnit = NULL;

	while(current_unit_offset < clientSelectionGroupEnd) {

		current_unit = *current_unit_offset;

		if(current_unit != NULL) {

			if(compareUnitRank(current_unit,unit_at_selection_top) != 0)
				unit_at_selection_top = current_unit;

			(*clientSelectionCount)++;

		}

		current_unit_offset++;

	}

	*activePortraitUnit = unit_at_selection_top;

	if(*clientSelectionCount == 1) {

		clientSelectionGroup->unit[0] = *activePortraitUnit;

		for(int i = 1; i < SELECTION_ARRAY_LENGTH; i++)
			clientSelectionGroup->unit[i] = NULL;

	}

	updateButtonSet();

}

;

} //namespace hooks

//-------- Helper function definitions. Do NOT modify! --------//

namespace {

const u32 Func_UpdateButtonSet = 0x00458DE0;
void updateButtonSet() {

	__asm {
		PUSHAD
		CALL Func_UpdateButtonSet
		POPAD
	}

}

;

const u32 Func_CompareUnitRank = 0x0049A350;
//Check if current_unit is before other_unit in selection.
//If not, return 0 (I think).
Bool32 compareUnitRank(CUnit* current_unit, CUnit* other_unit) {

	static Bool32 bResult;

	__asm {
		PUSHAD
		MOV EDI, current_unit
		MOV ESI, other_unit
		CALL Func_CompareUnitRank
		MOV bResult, EAX
		POPAD
	}

	return bResult;

}

;

} //unnamed namespace

//End of helper functions
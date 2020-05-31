#pragma once
#include "../../SCBW/structures/CUnit.h"

namespace hooks {

	u32 SortAllUnits(CUnit* unit,CUnit** unit_list,CUnit** units_in_bounds);							//6F0F0
	u32 combineSelectionsLists(CUnit* unit,CUnit** unit_list_1,CUnit** unit_list_2,u32 list_length);	//6F290
	void getSelectedUnitsInBox(Box16* coords);															//6FA40
	void getSelectedUnitsAtPoint(u32 unk1, u32 unk2);													//6FB40
	void selectUnitGroup(u32 selectionGroupNumber);														//96B40

	void injectSelectMod();

}

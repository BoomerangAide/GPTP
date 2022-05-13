#include "status_transport.h"
#include <SCBW/api.h>
#include <Events/Events.h>

//Helper functions declaration

namespace {

void hideDialog(BinDlg* dialog);	//18700
void updateDialog(BinDlg* dialog);	//1C400
void function_004248F0();			//248F0
bool function_00424A10_helper();	//24A10

u32 fxnInteract(BinDlg* dialog, u32 data_struct_offset);

} //unnamed namespace

CUnit*				const unitTable_0059CB58		= (CUnit*)	0x0059CB58;	//array of CUnit structures
u16*				const selectionLoadedUnitvalues = (u16*)	0x006CA78C; //array of 8 struct elements (simplified in 16bits value here)
s32*				const selectionHPvalues			= (s32*)	0x006CA94C;	//array of SELECTION_ARRAY_LENGTH elements
s32*				const selectionShieldvalue		= (s32*)	0x006CA9EC; //pointer on a single element (not array)
Bool8*				const selectionUpdateNeeded		= (Bool8*)	0x006CA9F0;
u16*				const selectionEnergyvalue		= (u16*)	0x006CAC0C; //pointer on a single element

namespace hooks {

//Equivalent to function at 0x00424BA0
#ifdef EVENTS_SYSTEM
void stats_transports_loaded_units_buttons(BinDlg* dialog) {

	BinDlg* current_dialog;
	CUnit* loaded_unit;
	int transport_index;
	int dialog_graphic;

	std::vector<int*> events_unit_transport_cap__less_than_7(2);
	std::vector<int*> events_transport_cargo_occupying_space(2);
	std::vector<int*> events_loaded_unit_dialog_arg(4);

	struct {
		u32 unknown_value;					//[EBP-0x28]	[00]
		u32 always_zero_here;				//[EBP-0x24]	[04]
		u32 not_allocated_4;				//[EBP-0x20]	[08]
		u16 always_14_here;					//[EBP-0x1C]	[0C]
		s16 mouseX;							//[EBP-0x1A]	[0E]
		s16 mouseY;							//[EBP-0x18]	[10]
		u16 not_allocated_3;				//[EBP-0x16]	[12]
		u32 not_allocated_2;				//[EBP-0x14]	[14]
		u32 next_loaded_unit_dialog_index;	//[EBP-0x10]	[18]
		u32 index_ebp_0c;					//[EBP-0x0C]	[1C]
		u32 some_dialog_index;				//[EBP-0x08]	[20]
		u16 not_allocated_1;				//[EBP-0x04]	[24]
		u8 not_allocated_0;					//[EBP-0x02]	[26]
		u8 storage_in_current_4_rooms_space;//[EBP-0x01]	[27]
	} stack_placeholder;

	CUnit* unit = *activePortraitUnit;

	//5 lines maybe unnecessary, but feels safer
	stack_placeholder.not_allocated_0 = 0;
	stack_placeholder.not_allocated_1 = 0;
	stack_placeholder.not_allocated_2 = 0;
	stack_placeholder.not_allocated_3 = 0;
	stack_placeholder.not_allocated_4 = 0;

	if(dialog->controlType == DialogControlTypes::DialogBox)
		current_dialog = dialog->childrenDlg;
	else
		current_dialog = (dialog->parent)->childrenDlg;

	while(current_dialog != NULL && current_dialog->index != 18)
		current_dialog = current_dialog->next;

	while(current_dialog != NULL && current_dialog->index != 32) {
		current_dialog->user = NULL;
		current_dialog = current_dialog->next;
	}

	//Not sure the index necessarily incremented normally between
	//18 and 32, so this is needed
	if(current_dialog != NULL && current_dialog->index == 32)
		current_dialog->user = NULL;

	bool unitSpaceProvidedLowerThan7 = units_dat::SpaceProvided[unit->id] < 7;

	events_unit_transport_cap__less_than_7[0] = (int*)unit;
	events_unit_transport_cap__less_than_7[1] = (int*)&unitSpaceProvidedLowerThan7;

	EventManager::EventCalled(EventId::STATUS_TRANSPORT_CAPACITY_LOWER_THAN_7, events_unit_transport_cap__less_than_7);

	//24BE4
	if(unitSpaceProvidedLowerThan7) {
		stack_placeholder.some_dialog_index = 20;
		stack_placeholder.index_ebp_0c = 22;
		stack_placeholder.next_loaded_unit_dialog_index = 27;
	}
	else {
		stack_placeholder.some_dialog_index = 18;
		stack_placeholder.index_ebp_0c = 21;
		stack_placeholder.next_loaded_unit_dialog_index = 25;
	}

	stack_placeholder.storage_in_current_4_rooms_space = 4;
	transport_index = 0;

	//24C20 (BIG LOOP)
	do {

		if(
			unit->loadedUnit[0].fullValue != 0 &&
			unitTable_0059CB58[unit->loadedUnit[0].innerValues.index].sprite != NULL
		) 
		{

			loaded_unit = (CUnit*)&unitTable_0059CB58[unit->loadedUnit[0].innerValues.index];

			if(
				(loaded_unit->mainOrderId == OrderId::Die && loaded_unit->mainOrderState == 1) ||
				loaded_unit->targetOrderSpecial != unit->loadedUnit[0].innerValues.unitId
			)
				loaded_unit = NULL;

		}
		else
			loaded_unit = NULL;

		transport_index = 0;

		do {

			//24C72 (BIG INNER LOOP)

			if(
				loaded_unit != NULL &&
				units_dat::SpaceRequired[loaded_unit->id] == stack_placeholder.storage_in_current_4_rooms_space
			) 
			{

				int transport_space_for_drawing = stack_placeholder.storage_in_current_4_rooms_space;

				events_transport_cargo_occupying_space[0] = (int*)unit;
				events_transport_cargo_occupying_space[1] = &transport_space_for_drawing;

				EventManager::EventCalled(EventId::STATUS_TRANSPORT_CURRENT_SPACE_OCCUPIED_CHECK, events_transport_cargo_occupying_space);

				if(transport_space_for_drawing == 2) { //24D5A

					if(dialog->controlType == DialogControlTypes::DialogBox)
						current_dialog = dialog->childrenDlg;
					else
						current_dialog = (dialog->parent)->childrenDlg;

					while(current_dialog != NULL && current_dialog->index != stack_placeholder.index_ebp_0c)
						current_dialog = current_dialog->next;

					if(loaded_unit->status & UnitStatus::IsHallucination)
						dialog_graphic = 33;		//cyan borders
					else
					if(units_dat::BaseProperty[loaded_unit->id] & UnitProperty::Hero)
						dialog_graphic = 21;		//white borders
					else
					if(unit->parasiteFlags == 0)
						dialog_graphic = 15;		//blue borders
					else
						dialog_graphic = 27;		//green borders

				}
				else
				if(transport_space_for_drawing == 4) { //24D00

					if(dialog->controlType == DialogControlTypes::DialogBox)
						current_dialog = dialog->childrenDlg;
					else
						current_dialog = (dialog->parent)->childrenDlg;

					while(current_dialog != NULL && current_dialog->index != stack_placeholder.some_dialog_index)
						current_dialog = current_dialog->next;

					if(loaded_unit->status & UnitStatus::IsHallucination)
						dialog_graphic = 35;		//cyan borders
					else
					if(units_dat::BaseProperty[loaded_unit->id] & UnitProperty::Hero)
						dialog_graphic = 23;		//white borders
					else
					if(unit->parasiteFlags == 0)
						dialog_graphic = 17;		//blue borders
					else
						dialog_graphic = 29;		//green borders

				}
				else { //24CA0

					if(dialog->controlType == DialogControlTypes::DialogBox)
						current_dialog = dialog->childrenDlg;
					else
						current_dialog = (dialog->parent)->childrenDlg;

					while(current_dialog != NULL && current_dialog->index != stack_placeholder.next_loaded_unit_dialog_index)
						current_dialog = current_dialog->next;

					if(loaded_unit->status & UnitStatus::IsHallucination)
						dialog_graphic = 31;		//cyan borders
					else
					if(units_dat::BaseProperty[loaded_unit->id] & UnitProperty::Hero)
						dialog_graphic = 19;		//white borders
					else
					if(unit->parasiteFlags == 0)
						dialog_graphic = 13;		//blue borders
					else
						dialog_graphic = 25;		//green borders

				}

				events_loaded_unit_dialog_arg[0] = (int*)&loaded_unit;
				events_loaded_unit_dialog_arg[1] = (int*)transport_index;
				events_loaded_unit_dialog_arg[2] = (int*)&dialog_graphic;
				events_loaded_unit_dialog_arg[3] = (int*)&current_dialog->pszText;	//normally not set here

				EventManager::EventCalled(EventId::STATUS_TRANSPORT_LOADED_UNIT_DIALOG_DATA, events_loaded_unit_dialog_arg);

				//24DB2
				current_dialog->graphic = dialog_graphic;
				current_dialog->unitUser = loaded_unit;

				if(!(current_dialog->flags & BinDlgFlags::Visible)) {

					stack_placeholder.mouseX = mouse->x;
					stack_placeholder.mouseY = mouse->y;
					stack_placeholder.always_14_here = 14;	//set here each time, because maybe changed out of sight
					stack_placeholder.unknown_value = 13;
					stack_placeholder.always_zero_here = 0;	//set here each time, because maybe changed out of sight

					if(
						fxnInteract(current_dialog,(u32)&stack_placeholder) != 0 &&
						!(current_dialog->flags & BinDlgFlags::Unknown0)
					)
					{
						current_dialog->flags |= BinDlgFlags::Unknown0;
						updateDialog(current_dialog);
					}

				}


				//24E05
				if(!(current_dialog->flags & BinDlgFlags::Unknown0)) {
					current_dialog->flags |= BinDlgFlags::Unknown0;
					updateDialog(current_dialog);
				}

				//24E19
				stack_placeholder.some_dialog_index = stack_placeholder.some_dialog_index + stack_placeholder.storage_in_current_4_rooms_space / 4;
				stack_placeholder.index_ebp_0c = stack_placeholder.index_ebp_0c + stack_placeholder.storage_in_current_4_rooms_space / 2;
				stack_placeholder.next_loaded_unit_dialog_index = stack_placeholder.next_loaded_unit_dialog_index + stack_placeholder.storage_in_current_4_rooms_space;

			}

			//24E49
			transport_index++;

			if(
				unit->loadedUnit[transport_index].fullValue != 0 &&
				unitTable_0059CB58[unit->loadedUnit[transport_index].innerValues.index].sprite != NULL
			) 
			{

				loaded_unit = (CUnit*)&unitTable_0059CB58[unit->loadedUnit[transport_index].innerValues.index];

				if(
					(loaded_unit->mainOrderId == OrderId::Die && loaded_unit->mainOrderState == 1) ||
					loaded_unit->targetOrderSpecial != unit->loadedUnit[transport_index].innerValues.unitId
				)
					loaded_unit = NULL;

			}
			else
				loaded_unit = NULL;

		}while(transport_index < 8);

		//24EA6
		stack_placeholder.storage_in_current_4_rooms_space = stack_placeholder.storage_in_current_4_rooms_space / 2;

	}while(stack_placeholder.storage_in_current_4_rooms_space != 0);

	if(dialog->controlType == DialogControlTypes::DialogBox)
		current_dialog = dialog->childrenDlg;
	else
		current_dialog = (dialog->parent)->childrenDlg;

	while(current_dialog != NULL && current_dialog->index != 18)
		current_dialog = current_dialog->next;

	while(current_dialog != NULL) {

		if(current_dialog->user == NULL)
			hideDialog(current_dialog);

		if(current_dialog->index != 32)
			current_dialog = current_dialog->next;
		else
			current_dialog = NULL;

	}

}
#else
void stats_transports_loaded_units_buttons(BinDlg* dialog) {

	BinDlg* current_dialog;
	CUnit* loaded_unit;
	int transport_index;
	int dialog_graphic;

	struct {
		u32 unknown_value;					//[EBP-0x28]	[00]
		u32 always_zero_here;				//[EBP-0x24]	[04]
		u32 not_allocated_4;				//[EBP-0x20]	[08]
		u16 always_14_here;					//[EBP-0x1C]	[0C]
		s16 mouseX;							//[EBP-0x1A]	[0E]
		s16 mouseY;							//[EBP-0x18]	[10]
		u16 not_allocated_3;				//[EBP-0x16]	[12]
		u32 not_allocated_2;				//[EBP-0x14]	[14]
		u32 next_loaded_unit_dialog_index;	//[EBP-0x10]	[18]
		u32 index_ebp_0c;					//[EBP-0x0C]	[1C]
		u32 some_dialog_index;				//[EBP-0x08]	[20]
		u16 not_allocated_1;				//[EBP-0x04]	[24]
		u8 not_allocated_0;					//[EBP-0x02]	[26]
		u8 storage_in_current_4_rooms_space;//[EBP-0x01]	[27]
	} stack_placeholder;

	CUnit* unit = *activePortraitUnit;

	//5 lines maybe unnecessary, but feels safer
	stack_placeholder.not_allocated_0 = 0;
	stack_placeholder.not_allocated_1 = 0;
	stack_placeholder.not_allocated_2 = 0;
	stack_placeholder.not_allocated_3 = 0;
	stack_placeholder.not_allocated_4 = 0;

	if(dialog->controlType == DialogControlTypes::DialogBox)
		current_dialog = dialog->childrenDlg;
	else
		current_dialog = (dialog->parent)->childrenDlg;

	while(current_dialog != NULL && current_dialog->index != 18)
		current_dialog = current_dialog->next;

	while(current_dialog != NULL && current_dialog->index != 32) {
		current_dialog->user = NULL;
		current_dialog = current_dialog->next;
	}

	//Not sure the index necessarily incremented normally between
	//18 and 32, so this is needed
	if(current_dialog != NULL && current_dialog->index == 32)
		current_dialog->user = NULL;

	//24BE4
	if(units_dat::SpaceProvided[unit->id] < 7) {
		stack_placeholder.some_dialog_index = 20;
		stack_placeholder.index_ebp_0c = 22;
		stack_placeholder.next_loaded_unit_dialog_index = 27;
	}
	else {
		stack_placeholder.some_dialog_index = 18;
		stack_placeholder.index_ebp_0c = 21;
		stack_placeholder.next_loaded_unit_dialog_index = 25;
	}

	stack_placeholder.storage_in_current_4_rooms_space = 4;
	transport_index = 0;

	//24C20 (BIG LOOP)
	do {

		if(
			unit->loadedUnit[0].fullValue != 0 &&
			unitTable_0059CB58[unit->loadedUnit[0].innerValues.index].sprite != NULL
		) 
		{

			loaded_unit = (CUnit*)&unitTable_0059CB58[unit->loadedUnit[0].innerValues.index];

			if(
				(loaded_unit->mainOrderId == OrderId::Die && loaded_unit->mainOrderState == 1) ||
				loaded_unit->targetOrderSpecial != unit->loadedUnit[0].innerValues.unitId
			)
				loaded_unit = NULL;

		}
		else
			loaded_unit = NULL;

		transport_index = 0;

		do {

			//24C72 (BIG INNER LOOP)

			if(
				loaded_unit != NULL &&
				units_dat::SpaceRequired[loaded_unit->id] == stack_placeholder.storage_in_current_4_rooms_space
			) 
			{

				if(stack_placeholder.storage_in_current_4_rooms_space == 2) { //24D5A

					if(dialog->controlType == DialogControlTypes::DialogBox)
						current_dialog = dialog->childrenDlg;
					else
						current_dialog = (dialog->parent)->childrenDlg;

					while(current_dialog != NULL && current_dialog->index != stack_placeholder.index_ebp_0c)
						current_dialog = current_dialog->next;

					if(loaded_unit->status & UnitStatus::IsHallucination)
						dialog_graphic = 33;		//cyan borders
					else
					if(units_dat::BaseProperty[loaded_unit->id] & UnitProperty::Hero)
						dialog_graphic = 21;		//white borders
					else
					if(unit->parasiteFlags == 0)
						dialog_graphic = 15;		//blue borders
					else
						dialog_graphic = 27;		//green borders

				}
				else
				if(stack_placeholder.storage_in_current_4_rooms_space == 4) { //24D00

					if(dialog->controlType == DialogControlTypes::DialogBox)
						current_dialog = dialog->childrenDlg;
					else
						current_dialog = (dialog->parent)->childrenDlg;

					while(current_dialog != NULL && current_dialog->index != stack_placeholder.some_dialog_index)
						current_dialog = current_dialog->next;

					if(loaded_unit->status & UnitStatus::IsHallucination)
						dialog_graphic = 35;		//cyan borders
					else
					if(units_dat::BaseProperty[loaded_unit->id] & UnitProperty::Hero)
						dialog_graphic = 23;		//white borders
					else
					if(unit->parasiteFlags == 0)
						dialog_graphic = 17;		//blue borders
					else
						dialog_graphic = 29;		//green borders

				}
				else { //24CA0

					if(dialog->controlType == DialogControlTypes::DialogBox)
						current_dialog = dialog->childrenDlg;
					else
						current_dialog = (dialog->parent)->childrenDlg;

					while(current_dialog != NULL && current_dialog->index != stack_placeholder.next_loaded_unit_dialog_index)
						current_dialog = current_dialog->next;

					if(loaded_unit->status & UnitStatus::IsHallucination)
						dialog_graphic = 31;		//cyan borders
					else
					if(units_dat::BaseProperty[loaded_unit->id] & UnitProperty::Hero)
						dialog_graphic = 19;		//white borders
					else
					if(unit->parasiteFlags == 0)
						dialog_graphic = 13;		//blue borders
					else
						dialog_graphic = 25;		//green borders

				}


				//24DB2
				current_dialog->graphic = dialog_graphic;
				current_dialog->unitUser = loaded_unit;

				if(!(current_dialog->flags & BinDlgFlags::Visible)) {

					stack_placeholder.mouseX = mouse->x;
					stack_placeholder.mouseY = mouse->y;
					stack_placeholder.always_14_here = 14;	//set here each time, because maybe changed out of sight
					stack_placeholder.unknown_value = 13;
					stack_placeholder.always_zero_here = 0;	//set here each time, because maybe changed out of sight

					if(
						fxnInteract(current_dialog,(u32)&stack_placeholder) != 0 &&
						!(current_dialog->flags & BinDlgFlags::Unknown0)
					)
					{
						current_dialog->flags |= BinDlgFlags::Unknown0;
						updateDialog(current_dialog);
					}

				}


				//24E05
				if(!(current_dialog->flags & BinDlgFlags::Unknown0)) {
					current_dialog->flags |= BinDlgFlags::Unknown0;
					updateDialog(current_dialog);
				}

				//24E19
				stack_placeholder.some_dialog_index = stack_placeholder.some_dialog_index + stack_placeholder.storage_in_current_4_rooms_space / 4;
				stack_placeholder.index_ebp_0c = stack_placeholder.index_ebp_0c + stack_placeholder.storage_in_current_4_rooms_space / 2;
				stack_placeholder.next_loaded_unit_dialog_index = stack_placeholder.next_loaded_unit_dialog_index + stack_placeholder.storage_in_current_4_rooms_space;

			}

			//24E49
			transport_index++;

			if(
				unit->loadedUnit[transport_index].fullValue != 0 &&
				unitTable_0059CB58[unit->loadedUnit[transport_index].innerValues.index].sprite != NULL
			) 
			{

				loaded_unit = (CUnit*)&unitTable_0059CB58[unit->loadedUnit[transport_index].innerValues.index];

				if(
					(loaded_unit->mainOrderId == OrderId::Die && loaded_unit->mainOrderState == 1) ||
					loaded_unit->targetOrderSpecial != unit->loadedUnit[transport_index].innerValues.unitId
				)
					loaded_unit = NULL;

			}
			else
				loaded_unit = NULL;

		}while(transport_index < 8);

		//24EA6
		stack_placeholder.storage_in_current_4_rooms_space = stack_placeholder.storage_in_current_4_rooms_space / 2;

	}while(stack_placeholder.storage_in_current_4_rooms_space != 0);

	if(dialog->controlType == DialogControlTypes::DialogBox)
		current_dialog = dialog->childrenDlg;
	else
		current_dialog = (dialog->parent)->childrenDlg;

	while(current_dialog != NULL && current_dialog->index != 18)
		current_dialog = current_dialog->next;

	while(current_dialog != NULL) {

		if(current_dialog->user == NULL)
			hideDialog(current_dialog);

		if(current_dialog->index != 32)
			current_dialog = current_dialog->next;
		else
			current_dialog = NULL;

	}

}
#endif

;

//Optimized compared to function at 0x00424FC0
#ifdef EVENTS_SYSTEM
void stats_transports_cache_for_display(BinDlg* dialog) {

	CUnit* unit = *activePortraitUnit;
	int unitHP, unitEnergy, unitShields;
	std::vector<int*> events_loaded_unit_hp_cache(3);
	std::vector<int*> events_transport_stats_cache(4);

	//original code hardcoded dealing with 4 units twice, probably a given up attempt to optimize half-capacity transports
	for(int index_loaded_unit = 0; index_loaded_unit < 8; index_loaded_unit++) {

		selectionLoadedUnitvalues[index_loaded_unit] = unit->loadedUnit[index_loaded_unit].fullValue;

		if(selectionLoadedUnitvalues[index_loaded_unit] != 0) {

			CUnit* loaded_unit;

			loaded_unit = &unitTable_0059CB58[unit->loadedUnit[index_loaded_unit].innerValues.index];

			if(
				loaded_unit->sprite != NULL &&
				(
					loaded_unit->mainOrderId != OrderId::Die ||
					loaded_unit->mainOrderState != 1
				) &&
				loaded_unit->targetOrderSpecial == unit->loadedUnit[0].innerValues.unitId
			)
				unitHP = loaded_unit->hitPoints;
			else
				unitHP = 0;

			events_loaded_unit_hp_cache[0] = (int*)unit;
			events_loaded_unit_hp_cache[1] = (int*)loaded_unit;
			events_loaded_unit_hp_cache[2] = (int*)&unitHP;

			EventManager::EventCalled(EventId::STATUS_TRANSPORT_LOADEDUNIT_HP_CACHE, events_loaded_unit_hp_cache);

			selectionHPvalues[index_loaded_unit + 1] = unitHP;

		}

	}

	//set to true if incomplete,researching,morphing...
	*selectionUpdateNeeded = function_00424A10_helper();

	unitHP = unit->hitPoints;
	unitEnergy = unit->energy / 256;
	unitShields = unit->shields / 256;

	events_transport_stats_cache[0] = (int*)unit;
	events_transport_stats_cache[1] = (int*)&unitHP;
	events_transport_stats_cache[1] = (int*)&unitEnergy;
	events_transport_stats_cache[1] = (int*)&unitShields;

	EventManager::EventCalled(EventId::STATUS_TRANSPORT_STATS_CACHE, events_transport_stats_cache);

	selectionHPvalues[0] = unitHP;
	*selectionEnergyvalue = unitEnergy;
	*selectionShieldvalue = unitShields;

	function_004248F0();

}
#else
void stats_transports_cache_for_display(BinDlg* dialog) {

	CUnit* unit = *activePortraitUnit;

	//original code hardcoded dealing with 4 units twice, weird
	for(int index_loaded_unit = 0; index_loaded_unit < 8; index_loaded_unit++) {

		selectionLoadedUnitvalues[index_loaded_unit] = unit->loadedUnit[index_loaded_unit].fullValue;

		if(selectionLoadedUnitvalues[index_loaded_unit] != 0) {

			CUnit* loaded_unit;
			int unitHP;

			loaded_unit = &unitTable_0059CB58[unit->loadedUnit[index_loaded_unit].innerValues.index];

			if(
				loaded_unit->sprite != NULL &&
				(
					loaded_unit->mainOrderId != OrderId::Die ||
					loaded_unit->mainOrderState != 1
				) &&
				loaded_unit->targetOrderSpecial == unit->loadedUnit[0].innerValues.unitId
			)
				unitHP = loaded_unit->hitPoints;
			else
				unitHP = 0;

			selectionHPvalues[index_loaded_unit + 1] = unitHP;

		}

	}

	//set to true if incomplete,researching,morphing...
	*selectionUpdateNeeded = function_00424A10_helper();

	selectionHPvalues[0] = unit->hitPoints;
	*selectionEnergyvalue = unit->energy / 256;
	*selectionShieldvalue = unit->shields / 256;

	function_004248F0();

}
#endif

;

} //namespace hooks

//-------- Helper function definitions. Do NOT modify! --------//

namespace {

const u32 Func_HideDialog = 0x00418700;
void hideDialog(BinDlg* dialog) {

	__asm {
		PUSHAD
		MOV ESI, dialog
		CALL Func_HideDialog
		POPAD
	}

}

;

const u32 Func_UpdateDialog	= 0x0041C400;
void updateDialog(BinDlg* dialog) {
	__asm {
		PUSHAD
		MOV EAX, dialog
		CALL Func_UpdateDialog
		POPAD
	}
}

;

const u32 Func_Sub4248F0 = 0x004248F0;
//Update the content of selectionVariableData data for
//Cond functions, with scarabs/interceptors/remove timer
void function_004248F0() {
	__asm {
		PUSHAD
		CALL Func_Sub4248F0
		POPAD
	}
}

;

const u32 Func_Sub424A10 = 0x00424A10;
//Hooked in unit_stat_cond
bool function_00424A10_helper() {
	
	static Bool32 bPreResult;

	__asm {
		PUSHAD
		CALL Func_Sub424A10
		MOV bPreResult, EAX
		POPAD
	}

	return (bPreResult != 0);

}

;

//FROM ANOTHER FUNCTION, TO UPDATE: dev info: in this context this call 4598D0 statbtn_Btn_Interact
u32 fxnInteract(BinDlg* dialog, u32 data_struct_offset) {

	static u32 return_value;
	static u32 fxnInteract;

	fxnInteract = (u32)dialog->fxnInteract;

	__asm {
		PUSHAD
		MOV ESI, dialog
		MOV ECX, dialog
		MOV EDX, data_struct_offset
		CALL fxnInteract
		MOV return_value, EAX
		POPAD
	}

	return return_value;

}

;

} //unnamed namespace

//End of helper functions

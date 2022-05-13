#include "unit_stat_selection.h"
#include <SCBW/api.h>
#include <Events/Events.h>

//Helper functions declaration

namespace {

void showDialog(BinDlg* dialog);		//186A0
void hideDialog(BinDlg* dialog);		//18700
void updateDialog(BinDlg* dialog);		//1C400
void function_00424540();				//24540

void function_004566B0(CUnit* unit);	//566B0
void function_00456730(CUnit* unit);	//56730
void function_004567C0(CUnit* unit);	//567C0
void function_00456D30(BinDlg* dialog);	//56D30

#ifdef EVENTS_SYSTEM
void function_004566B0_event(bool shieldsEnabled, u32 currentShield = 0, u32 maxShield = 0);	//566B0
void function_00456730_event(u32 currentHP, u32 maxHP);											//56730
void function_004567C0_event(u32 currentHP, u32 maxHP, u8 unitWireframeRandomizer);				//567C0
#endif

u16 getLastQueueSlotType(CUnit* unit);	//7B270

} //unnamed namespace

u32* const selectionHPvalues = (u32*)0x006CA94C;
u16* const selectionIDvalues = (u16*)0x006CAD7C;

namespace hooks {

//Equivalent to function @ 0x00424660
#ifdef EVENTS_SYSTEM
bool UnitStatCond_Selection() {

	bool return_value = false;
	bool cancelNormalCheck = false;

	std::vector<int*> events_override_arg(0);
	std::vector<int*> events_unit_require_refresh_arg(2);

	EventManager::EventCalling(EventId::STATUS_SELECTION_UPDATE_COND_OVERRIDE, &cancelNormalCheck, events_override_arg);

	if (!cancelNormalCheck)
	{

		int index = 0;
		CUnit* current_unit = clientSelectionGroup->unit[index];

		while (!return_value && index < SELECTION_ARRAY_LENGTH) {

			if (current_unit != NULL) {

				events_unit_require_refresh_arg[0] = (int*)current_unit;
				events_unit_require_refresh_arg[1] = (int*)&return_value;

				EventManager::EventCalled(EventId::STATUS_SELECTION_UPDATE_COND_UNIT, events_unit_require_refresh_arg);

				if (
					!return_value &&
					(
						current_unit->hitPoints != selectionHPvalues[index] ||
						current_unit->id != selectionIDvalues[index]
					)
				)
					return_value = true;
				else
				{
					index++;
					current_unit = clientSelectionGroup->unit[index];
				}

			}
			else {
				index++;
				current_unit = clientSelectionGroup->unit[index];
			}

		}

	}

	return return_value;

}
#else
bool UnitStatCond_Selection() {

	bool return_value = false;
	int index = 0;
	CUnit* current_unit = clientSelectionGroup->unit[index];

	while(!return_value && index < SELECTION_ARRAY_LENGTH) {

		if(
			current_unit != NULL &&
			(	
				current_unit->hitPoints != selectionHPvalues[index] ||
				current_unit->id != selectionIDvalues[index]
			)
		)
			return_value = true;
		else {
			index++;
			current_unit = clientSelectionGroup->unit[index];
		}

	}

	return return_value;

}
#endif

;

//Equivalent to function @ 0x00425960
#ifdef EVENTS_SYSTEM
void UnitStatAct_Selection(BinDlg* dialog) {

	u8* const u8_0068C1E5 = (u8*)0x0068C1E5;
	BinDlg* current_dialog;
	std::vector<int*> events_override_arg(1);
	std::vector<int*> events_multiselect_unit_arg(7);

	if(*u8_0068C1E5 != 1) {

		if(dialog->controlType != DialogControlTypes::DialogBox)
			current_dialog = dialog;
		else
			current_dialog = dialog->childrenDlg;

		while(current_dialog != NULL) {
			hideDialog(current_dialog);
			current_dialog = current_dialog->next;
		}

		*u8_0068C1E5 = 1;

	}

	//update selection hp and id arrays
	function_00424540();

#pragma warning ( suppress:28182 )
	if(dialog->controlType != DialogControlTypes::DialogBox)
		dialog = dialog->parent;

	current_dialog = dialog->childrenDlg;

	while(current_dialog != NULL && current_dialog->index != 33) //0x21
		current_dialog = current_dialog->next;

	if(current_dialog != NULL) {

		bool cancelNormalDisplay = false;
		events_override_arg[0] = (int*)&current_dialog;

		EventManager::EventCalling(EventId::STATUS_MULTISELECT_OVERRIDE, &cancelNormalDisplay, events_override_arg);

		if(!cancelNormalDisplay) {
			CUnit** current_selection = (CUnit**)clientSelectionGroup;
			CUnit* current_unit;
			int unitIndexInSelection = 0;

			u16 panelGraphic, iconId, itemId, tooltipTypeId;
			char* iconText;

			do {

				current_unit = *current_selection;

				if(current_unit != NULL) {

					panelGraphic = current_dialog->graphic;
					iconId = current_unit->id;
					tooltipTypeId = current_dialog->statUser->tooltipType_06;
					itemId = current_dialog->statUser->id_08;
					iconText = current_dialog->pszText;

					events_multiselect_unit_arg[0] = (int*)&current_unit;
					events_multiselect_unit_arg[1] = (int*)unitIndexInSelection;
					events_multiselect_unit_arg[2] = (int*)&panelGraphic;
					events_multiselect_unit_arg[3] = (int*)&iconId;
					events_multiselect_unit_arg[4] = (int*)&tooltipTypeId;
					events_multiselect_unit_arg[5] = (int*)&itemId;
					events_multiselect_unit_arg[6] = (int*)&iconText;

					EventManager::EventCalled(EventId::STATUS_MULTISELECT_UNIT, events_multiselect_unit_arg);

					current_dialog->graphic = panelGraphic;
					current_dialog->statUser->unkUser_00 = (u32)current_unit;
					current_dialog->statUser->iconId_04 = iconId;
					current_dialog->statUser->tooltipType_06 = tooltipTypeId;
					current_dialog->statUser->id_08 = itemId;

					if(iconText != NULL)
						current_dialog->pszText = iconText;

					showDialog(current_dialog);

					if(!(current_dialog->flags & BinDlgFlags::Unknown0)) {
						current_dialog->flags |= BinDlgFlags::Unknown0;
						updateDialog(current_dialog);
					}

					current_dialog = current_dialog->next;

				}

				current_selection++;
				unitIndexInSelection++;

			}while(current_dialog != NULL && current_selection < clientSelectionGroupEnd);

			while(current_dialog != NULL) {

				hideDialog(current_dialog);

				if(current_dialog->index == 44) //0x2C
					current_dialog = NULL;
				else
					current_dialog = current_dialog->next;

			}

		}

	}

}
#else
void UnitStatAct_Selection(BinDlg* dialog) {

	u8* const u8_0068C1E5 = (u8*)0x0068C1E5;
	BinDlg* current_dialog;

	if(*u8_0068C1E5 != 1) {

		if(dialog->controlType != DialogControlTypes::DialogBox)
			current_dialog = dialog;
		else
			current_dialog = dialog->childrenDlg;

		while(current_dialog != NULL) {
			hideDialog(current_dialog);
			current_dialog = current_dialog->next;
		}

		*u8_0068C1E5 = 1;

	}

	//update selection hp and id arrays
	function_00424540();

#pragma warning ( suppress:28182 )
	if(dialog->controlType != DialogControlTypes::DialogBox)
		dialog = dialog->parent;

	current_dialog = dialog->childrenDlg;

	while(current_dialog != NULL && current_dialog->index != 33) //0x21
		current_dialog = current_dialog->next;

	if(current_dialog != NULL) {

		CUnit** current_selection = (CUnit**)clientSelectionGroup;
		CUnit* current_unit;

		do {

			current_unit = *current_selection;

			if(current_unit != NULL) {

				current_dialog->statUser->unkUser_00 = (u32)current_unit;
				current_dialog->statUser->iconId_04 = current_unit->id;

				showDialog(current_dialog);

				if(!(current_dialog->flags & BinDlgFlags::Unknown0)) {
					current_dialog->flags |= BinDlgFlags::Unknown0;
					updateDialog(current_dialog);
				}

				current_dialog = current_dialog->next;

			}

			current_selection++;

		}while(current_dialog != NULL && current_selection < clientSelectionGroupEnd);

		while(current_dialog != NULL) {

			hideDialog(current_dialog);

			if(current_dialog->index == 44) //0x2C
				current_dialog = NULL;
			else
				current_dialog = current_dialog->next;

		}

	}

}
#endif
;

//Function used by dialog item (fxnUpdate) corresponding 
//to an unit in a multi units selections (Partial)
//The shield not updating while taking damage until you
//reselect is a Starcraft bug indeed, may make testing harder.
//Equivalent to beginning of function @ 0x00456F50
#ifdef EVENTS_SYSTEM
u32 statdata_UnitWireframeSelectUpdate_Part1(BinDlg* dialog) {

	//array type is not reflected here, but was observed in internal
	const s16** const s16_array_0068C1FC = (const s16**)0x0068C1FC;

	std::vector<int*> events_override_arg(3);
	std::vector<int*> events_multiselect_unit_wireframe_unitvalues_update_arg(10);
	std::vector<int*> events_multiselect_unit_wireframe_background_update_arg(2);
	std::vector<int*> events_multiselect_unit_wireframe_icon_update_arg(2);
	bool bReplaceNormalBehavior = false;

	CUnit* unit =  (CUnit*)dialog->statUser->unkUser_00;
	u16 dialogIconId = IconId::Marine;

	events_override_arg[0] = (int*)dialog;
	events_override_arg[1] = (int*)unit;
	events_override_arg[2] = (int*)&dialogIconId;

	EventManager::EventCalling(EventId::STATUS_MULTISELECT_UNIT_WIREFRAME_UPDATE_OVERRIDE, &bReplaceNormalBehavior, events_override_arg);

	if(!bReplaceNormalBehavior){

		bool shieldsEnabled = (units_dat::ShieldsEnabled[unit->id] != 0);
		u32 currentShield = unit->shields / 256;
		u32 maxShield = units_dat::MaxShieldPoints[getLastQueueSlotType(unit)];
		u32 currentHP = (unit->hitPoints + 255) / 256;
		u32 maxHP = units_dat::MaxHitPoints[unit->id] / 256;
		u32 unitStatus = unit->status;
		u32 unitProperties = units_dat::BaseProperty[unit->id];
		u32 unitParasiteFlags = unit->parasiteFlags;
		u32 unitWireframeRandomizer = unit->wireframeRandomizer;

		events_multiselect_unit_wireframe_unitvalues_update_arg[0] = (int*)unit;
		events_multiselect_unit_wireframe_unitvalues_update_arg[1] = (int*)&shieldsEnabled;
		events_multiselect_unit_wireframe_unitvalues_update_arg[2] = (int*)&currentShield;
		events_multiselect_unit_wireframe_unitvalues_update_arg[3] = (int*)&maxShield;
		events_multiselect_unit_wireframe_unitvalues_update_arg[4] = (int*)&currentHP;
		events_multiselect_unit_wireframe_unitvalues_update_arg[5] = (int*)&maxHP;
		events_multiselect_unit_wireframe_unitvalues_update_arg[6] = (int*)&unitStatus;
		events_multiselect_unit_wireframe_unitvalues_update_arg[7] = (int*)&unitProperties;
		events_multiselect_unit_wireframe_unitvalues_update_arg[8] = (int*)&unitParasiteFlags;
		events_multiselect_unit_wireframe_unitvalues_update_arg[9] = (int*)&unitWireframeRandomizer;

		EventManager::EventCalled(EventId::STATUS_MULTISELECT_UNIT_WIREFRAME_UPDATE_UNITVALUES, events_multiselect_unit_wireframe_unitvalues_update_arg);

		if (unitStatus & UnitStatus::IsHallucination)
			dialog->graphic = 0x1F; //(31)
		else
		if(unitProperties & UnitProperty::Hero)
			dialog->graphic = 0x13; //(19)
		else
		if(unitParasiteFlags != 0)
			dialog->graphic = 0x19; //(25)
		else
			dialog->graphic = 0x0D; //(13)

		events_multiselect_unit_wireframe_background_update_arg[0] = (int*)unit;
		events_multiselect_unit_wireframe_background_update_arg[1] = (int*)&dialog->graphic;

		EventManager::EventCalled(EventId::STATUS_MULTISELECT_UNIT_WIREFRAME_BACKGROUND_UPDATE, events_multiselect_unit_wireframe_background_update_arg);

		function_00456D30(dialog);	//possibly render borders of image based on dialog->graphic

		function_004567C0_event(currentHP, maxHP, unitWireframeRandomizer);	//calculations using HP, unit->wireframeRandomizer, possibly for wireframe color

		if (!shieldsEnabled) //calculations using shield, possibly for shield effect on wireframe
			function_004566B0_event(shieldsEnabled);
		else
			function_004566B0_event(shieldsEnabled, currentShield, maxShield);

		function_00456730_event(currentHP, maxHP);	//another calculations using HP, possibly for wireframe color

		if (*s16_array_0068C1FC[0] < 0)
			dialogIconId = (u16)(*s16_array_0068C1FC[0]) - 0x8000;
		else
			dialogIconId = *s16_array_0068C1FC[0];

		//dialogIconId value should be IconId::Hatchery (0x83) at this point)

		if (dialog->statUser->iconId_04 < dialogIconId)
			dialogIconId = dialog->statUser->iconId_04;
		else
			dialogIconId = IconId::Marine;

		events_multiselect_unit_wireframe_icon_update_arg[0] = (int*)unit;
		events_multiselect_unit_wireframe_icon_update_arg[1] = (int*)&dialogIconId;

		EventManager::EventCalled(EventId::STATUS_MULTISELECT_UNIT_WIREFRAME_ICON_UPDATE, events_multiselect_unit_wireframe_icon_update_arg);

		//Normally, from here until the hooked end, there should be complicated computations
		//involving lengths and an advanced data structure around 0068C1FC, call SetRect of User32 (from
		//windows.h), then perform more coordinates/lengths/dimensions calculations before finally
		//calling imageRenderFxn0_0

	}

	return dialogIconId;

}
#else
u32 statdata_UnitWireframeSelectUpdate_Part1(BinDlg* dialog) {

	const s16** const s16_array_0068C1FC = (const s16**)0x0068C1FC;

	CUnit* unit;
	u16 dialogIconId;

	unit = (CUnit*)dialog->statUser->unkUser_00;

	if (unit->status & UnitStatus::IsHallucination)
		dialog->graphic = 0x1F; //(31)
	else
	if(units_dat::BaseProperty[unit->id] & UnitProperty::Hero)
		dialog->graphic = 0x13; //(19)
	else
	if(unit->parasiteFlags != 0)
		dialog->graphic = 0x19; //(25)
	else
		dialog->graphic = 0x0D; //(13)

	function_00456D30(dialog);	//possibly render borders of image based on dialog->graphic
	function_004567C0(unit);	//calculations using HP, unit->wireframeRandomizer, possibly for wireframe color
	function_004566B0(unit);	//calculations using shield, possibly for shield effect on wireframe
	function_00456730(unit);	//another calculations using HP, unit->wireframeRandomizer, possibly for wireframe color

	if (*s16_array_0068C1FC[0] < 0)
		dialogIconId = (u16)(*s16_array_0068C1FC[0]) - 0x8000;
	else
		dialogIconId = *s16_array_0068C1FC[0];

	//dialogIconId value should be IconId::Hatchery (0x83) at this point)

	if (dialog->statUser->iconId_04 < dialogIconId)
		dialogIconId = dialog->statUser->iconId_04;
	else
		dialogIconId = IconId::Marine;

	//Normally, from here until the hooked end, there should be complicated computations
	//involving lengths and an advanced data structure around 0068C1FC, call SetRect of User32 (from
	//windows.h), then perform more coordinates/lengths/dimensions calculations before finally
	//calling imageRenderFxn0_0

	return dialogIconId;

}
#endif

;

//Identical to end of function @ 0x00456F50
void statdata_UnitWireframeSelectUpdate_Part3(BinDlg* dialog) {
	*((u8*)0x0050CE91) = -0x30;
	*((u8*)0x0050CE92) = -0x2F;
	*((u8*)0x0050CE93) = -0x2E;
	*((u8*)0x0050CE94) = -0x2D;
	*((u8*)0x0050CE81) = -0x40;
	*((u8*)0x0050CE82) = -0x3F;
	*((u8*)0x0050CE99) = -0x28;
	*((u8*)0x0050CE9A) = -0x27;
	*((u8*)0x0050CE9B) = -0x26;
	*((u8*)0x0050CE9C) = -0x25;
}

;

} //namespace hooks

//-------- Helper function definitions. Do NOT modify! --------//

namespace {

const u32 Func_ShowDialog = 0x004186A0;
void showDialog(BinDlg* dialog) {

	__asm {
		PUSHAD
		MOV ESI, dialog
		CALL Func_ShowDialog
		POPAD
	}

}

;

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

//Equivalent (though implemented differently) to sub_424540 @ 0x00424540
void function_00424540() {

	CUnit* current_unit;

	for(int i = 0; i < SELECTION_ARRAY_LENGTH; i++) {

		current_unit = clientSelectionGroup->unit[i];

		if(current_unit != NULL) {
			selectionHPvalues[i] = current_unit->hitPoints;
			selectionIDvalues[i] = current_unit->id;
		}
		else {
			selectionHPvalues[i] = 0;
			selectionIDvalues[i] = UnitId::None;
		}

	}

}

;

void function_004566B0(CUnit* unit) {

	static u8*	u8_0050CE81	= (u8*)	0x0050CE81;
	static u8*	u8_0050CE82	= (u8*)	0x0050CE82;
	static u8*	u8_00515280	= (u8*)	0x00515280;
	static u8*	u8_00515281	= (u8*)	0x00515281;
	static u8*	u8_0068C208	= (u8*)	0x0068C208;
	static u8*	u8_0068C20A = (u8*) 0x0068C20A;

	if (units_dat::ShieldsEnabled[unit->id] == 0) {
		*u8_0050CE82 = *u8_0068C20A;
		*u8_0050CE81 = *u8_0068C20A;
	}
	else
	{

		u32 maxShield = units_dat::MaxShieldPoints[getLastQueueSlotType(unit)];

		if(maxShield == 0) {
			*u8_0050CE81 = *u8_0068C20A;
			*u8_0050CE82 = *u8_0068C20A;
		}
		else
		{
			//order and parenthesis really matter!
			u8 shieldLayerId = 2 * ((7 * (unit->shields / 256)) / maxShield); //x7 for Id, x2 to access block of 2 bytes structure hidden behind 2 arrays
			*u8_0050CE81 = u8_0068C208[u8_00515280[shieldLayerId]];
			*u8_0050CE82 = u8_0068C208[u8_00515281[shieldLayerId]];
		}

	}

}

;

void function_00456730(CUnit* unit) {

	static u8*	u8_0050CE99		= (u8*)	0x0050CE99;
	static u8*	u8_0050CE9A		= (u8*)	0x0050CE9A;
	static u8*	u8_0050CE9B		= (u8*)	0x0050CE9B;
	static u8*	u8_0050CE9C		= (u8*)	0x0050CE9C;
	static u8*	u8_00515290		= (u8*)	0x00515290;
	static u8*	u8_00515291		= (u8*)	0x00515291;
	static u8*	u8_00515292		= (u8*)	0x00515292;
	static u8*	u8_00515293		= (u8*)	0x00515293;
	static u8*	u8_0068C208		= (u8*)	0x0068C208;

	u32 currentHP = (unit->hitPoints + 255) / 256;
	u32 id_HP_Layer;
	u32 maxHP = units_dat::MaxHitPoints[unit->id] / 256;

	if (maxHP == 0) {

		maxHP = currentHP;

		if (maxHP == 0)
			maxHP = 1;

	}

	//order and parenthesis really matter!
	id_HP_Layer = 4 * ((5 * currentHP) / maxHP); //x5 for Id, x4 to access block of 4 bytes structure hidden behind 4 arrays

	*u8_0050CE99 = u8_0068C208[u8_00515290[id_HP_Layer]];
	*u8_0050CE9A = u8_0068C208[u8_00515291[id_HP_Layer]];
	*u8_0050CE9B = u8_0068C208[u8_00515292[id_HP_Layer]];
	*u8_0050CE9C = u8_0068C208[u8_00515293[id_HP_Layer]];

}

;

void function_004567C0(CUnit* unit) {

	static u8*	u8_0050CE91		= (u8*)	0x0050CE91;
	static u8*	u8_0050CE92		= (u8*)	0x0050CE92;
	static u8*	u8_0050CE93		= (u8*)	0x0050CE93;
	static u8*	u8_0050CE94		= (u8*)	0x0050CE94;
	static u8*	u8_00515230		= (u8*)	0x00515230;
	static u8*	u8_00515258		= (u8*)	0x00515258;
	static u8*	u8_0068C208		= (u8*)	0x0068C208;
	static u8*	u8_0068C20A		= (u8*)	0x0068C20A;

	u32 currentHP = (unit->hitPoints + 255) / 256;
	u32 maxHP;
	u8* id_array_HP_Layers_ptr;

	*u8_0050CE91 = *u8_0068C20A;
	*u8_0050CE92 = *u8_0068C20A;
	*u8_0050CE93 = *u8_0068C20A;
	*u8_0050CE94 = *u8_0068C20A;

	maxHP = units_dat::MaxHitPoints[unit->id] / 256;

	if (maxHP == 0) {

		maxHP = currentHP;

		if (maxHP == 0)
			maxHP = 1;

	}

	//order and parenthesis really matter!
	if (unit->wireframeRandomizer & 1)
		id_array_HP_Layers_ptr = &u8_00515230[4 * ((9 * currentHP) / maxHP)];
	else
		id_array_HP_Layers_ptr = &u8_00515258[4 * ((9 * currentHP) / maxHP)];

	for (int counter = 0; counter < 4; counter++) {

		u32 inner_index;

		inner_index = unit->wireframeRandomizer;

		__asm {

			PUSHAD

			MOV EAX, inner_index
			MOV ECX, counter

			SHR EAX, CL
			AND EAX, 0x07
			LEA ECX, [EAX+EAX*2]
			MOV EAX, 0x92492493		//magic number trick force me to use raw asm
			IMUL ECX
			ADD EDX, ECX
			SAR EDX, 0x02
			MOV EAX, EDX
			SHR EAX, 0x1F
			ADD EAX, EDX

			MOV inner_index, EAX

			POPAD

		}

		if (u8_0050CE91[inner_index] != *u8_0068C20A) {

			//hopefully, data is always properly initialized, because otherwise, look like an easy infinite loop
			do
			{

				inner_index++;

				if (inner_index == 4)
					inner_index = 0;

			} while (u8_0050CE91[inner_index] != *u8_0068C20A);

		}

		//56880
		u8_0050CE91[inner_index] = u8_0068C208[id_array_HP_Layers_ptr[counter]]; //BORKEN BORKEN BORKEN BROKEN

	}

}

;

#ifdef EVENTS_SYSTEM
void function_004566B0_event(bool shieldsEnabled, u32 currentShield, u32 maxShield) {

	static u8*	u8_0050CE81	= (u8*)	0x0050CE81;
	static u8*	u8_0050CE82	= (u8*)	0x0050CE82;
	static u8*	u8_00515280	= (u8*)	0x00515280;
	static u8*	u8_00515281	= (u8*)	0x00515281;
	static u8*	u8_0068C208	= (u8*)	0x0068C208;
	static u8*	u8_0068C20A	= (u8*) 0x0068C20A;

	if (!shieldsEnabled) {
		*u8_0050CE81 = *u8_0068C20A;
		*u8_0050CE82 = *u8_0068C20A;
	}
	else
	{

		if(maxShield == 0) {
			*u8_0050CE81 = *u8_0068C20A;
			*u8_0050CE82 = *u8_0068C20A;
		}
		else
		{
			//order and parenthesis really matter!
			u8 shieldLayerId = 2 * ((7 * currentShield) / maxShield); //x7 for Id, x2 to access block of 2 bytes structure hidden behind 2 arrays
			*u8_0050CE81 = u8_0068C208[u8_00515280[shieldLayerId]];
			*u8_0050CE82 = u8_0068C208[u8_00515281[shieldLayerId]];
		}

	}

}

;

void function_00456730_event(u32 currentHP, u32 maxHP) {

	static u8*	u8_0050CE99		= (u8*)	0x0050CE99;
	static u8*	u8_0050CE9A		= (u8*)	0x0050CE9A;
	static u8*	u8_0050CE9B		= (u8*)	0x0050CE9B;
	static u8*	u8_0050CE9C		= (u8*)	0x0050CE9C;
	static u8*	u8_00515290		= (u8*)	0x00515290;
	static u8*	u8_00515291		= (u8*)	0x00515291;
	static u8*	u8_00515292		= (u8*)	0x00515292;
	static u8*	u8_00515293		= (u8*)	0x00515293;
	static u8*	u8_0068C208		= (u8*)	0x0068C208;

	u32 id_HP_Layer;

	if (maxHP == 0) {

		maxHP = currentHP;

		if (maxHP == 0)
			maxHP = 1;

	}

	//order and parenthesis really matter!
	id_HP_Layer = 4 * ((5 * currentHP) / maxHP); //x5 for Id, x4 to access block of 4 bytes structure hidden behind 4 arrays

	*u8_0050CE99 = u8_0068C208[u8_00515290[id_HP_Layer]];
	*u8_0050CE9A = u8_0068C208[u8_00515291[id_HP_Layer]];
	*u8_0050CE9B = u8_0068C208[u8_00515292[id_HP_Layer]];
	*u8_0050CE9C = u8_0068C208[u8_00515293[id_HP_Layer]];

}

;

void function_004567C0_event(u32 currentHP, u32 maxHP, u8 unitWireframeRandomizer) {

	static u8*	u8_0050CE91		= (u8*)	0x0050CE91;
	static u8*	u8_0050CE92		= (u8*)	0x0050CE92;
	static u8*	u8_0050CE93		= (u8*)	0x0050CE93;
	static u8*	u8_0050CE94		= (u8*)	0x0050CE94;
	static u8*	u8_00515230		= (u8*)	0x00515230;
	static u8*	u8_00515258		= (u8*)	0x00515258;
	static u8*	u8_0068C208		= (u8*)	0x0068C208;
	static u8*	u8_0068C20A		= (u8*)	0x0068C20A;

	u8* id_array_HP_Layers_ptr;

	*u8_0050CE91 = *u8_0068C20A;
	*u8_0050CE92 = *u8_0068C20A;
	*u8_0050CE93 = *u8_0068C20A;
	*u8_0050CE94 = *u8_0068C20A;

	if (maxHP == 0) {

		maxHP = currentHP;

		if (maxHP == 0)
			maxHP = 1;

	}

	//order and parenthesis really matter!
	if (unitWireframeRandomizer & 1)
		id_array_HP_Layers_ptr = &u8_00515230[4 * ((9 * currentHP) / maxHP)];
	else
		id_array_HP_Layers_ptr = &u8_00515258[4 * ((9 * currentHP) / maxHP)];

	for (int counter = 0; counter < 4; counter++) {

		u32 inner_index;

		inner_index = unitWireframeRandomizer;

		__asm {

			PUSHAD

			MOV EAX, inner_index
			MOV ECX, counter

			SHR EAX, CL
			AND EAX, 0x07
			LEA ECX, [EAX+EAX*2]
			MOV EAX, 0x92492493		//magic number trick force me to use raw asm
			IMUL ECX
			ADD EDX, ECX
			SAR EDX, 0x02
			MOV EAX, EDX
			SHR EAX, 0x1F
			ADD EAX, EDX

			MOV inner_index, EAX

			POPAD

		}

		if (u8_0050CE91[inner_index] != *u8_0068C20A) {

			//hopefully, data is always properly initialized, because otherwise, look like an easy infinite loop
			do
			{

				inner_index++;

				if (inner_index == 4)
					inner_index = 0;

			} while (u8_0050CE91[inner_index] != *u8_0068C20A);

		}

		//56880
		u8_0050CE91[inner_index] = u8_0068C208[id_array_HP_Layers_ptr[counter]];

	}

}

;
#endif

const u32 Func_Sub56D30 = 0x00456D30;
void function_00456D30(BinDlg* dialog) {
	__asm {
		PUSHAD
		MOV EDI, dialog
		CALL Func_Sub56D30
		POPAD
	}
}

;

//Identical to function at 0x0047B270
//EDX = unit
//If the unit is currently mutating into
//a level 2/3 zerg building, return the
//id of the building morphed into, else
//return the normal unit id.
u16 getLastQueueSlotType(CUnit* unit) {

	u16 unitId;

	if(unit->status & UnitStatus::Completed)
		unitId = unit->id;
	else {

		unitId = unit->buildQueue[unit->buildQueueSlot];

		if(
			unitId != UnitId::ZergHive &&
			unitId != UnitId::ZergLair &&
			unitId != UnitId::ZergGreaterSpire &&
			unitId != UnitId::ZergSporeColony &&
			unitId != UnitId::ZergSunkenColony
		)
			unitId = unit->id;
		
	}

	return unitId;

}

;

} //unnamed namespace

//End of helper functions

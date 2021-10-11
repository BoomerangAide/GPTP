#include "unit_stat_selection.h"
#include <SCBW/api.h>

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

} //unnamed namespace

u32* const selectionHPvalues = (u32*)0x006CA94C;
u16* const selectionIDvalues = (u16*)0x006CAD7C;

namespace hooks {

//Equivalent to function @ 0x00424660
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

;

//Equivalent to function @ 0x00425960
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

	while(current_dialog != NULL && current_dialog->index != 33)
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

;

//Function used by dialog item (fxnUpdate) corresponding 
//to an unit in a multi units selections (Partial)
//Equivalent to beginning of function @ 0x00456F50
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

	function_00456D30(dialog);
	function_004567C0(unit);
	function_004566B0(unit);
	function_00456730(unit);

	if (*s16_array_0068C1FC[0] < 0)
		dialogIconId = (u16)(*s16_array_0068C1FC[0]) - 0x8000;
	else
		dialogIconId = *s16_array_0068C1FC[0];

	//dialogIconId value should be IconId::Hatchery (0x83) at this point)

	if (dialog->statUser->iconId_04 < dialogIconId)
		dialogIconId = dialog->statUser->iconId_04;
	else
		dialogIconId = IconId::Marine;

	return dialogIconId;

}

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

const u32 Func_Sub566B0 = 0x004566B0;
void function_004566B0(CUnit* unit) {
	__asm {
		PUSHAD
		MOV EDX, unit
		CALL Func_Sub566B0
		POPAD
	}
}

;

const u32 Func_Sub56730 = 0x00456730;
void function_00456730(CUnit* unit) {
	__asm {
		PUSHAD
		MOV ECX, unit
		CALL Func_Sub56730
		POPAD
	}
}

;

const u32 Func_Sub567C0 = 0x004567C0;
void function_004567C0(CUnit* unit) {
	__asm {
		PUSHAD
		MOV ESI, unit
		CALL Func_Sub567C0
		POPAD
	}
}

;

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

} //unnamed namespace

//End of helper functions

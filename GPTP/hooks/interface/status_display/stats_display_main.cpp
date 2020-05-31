#include "stats_display_main.h"
#include <SCBW/api.h>

//Helper functions declaration

namespace {

void hideDialog(BinDlg* dialog);					//18700
bool UnitStatCond_Selection_Helper();				//24660
void UnitStatAct_Selection_Helper(BinDlg* dialog);	//25960
void function_004568B0();							//568B0
void function_00457FE0();							//57FE0

bool UnitStatCondFunc(u32 funcOffset);
void UnitStatActFunc(u32 funcOffset, BinDlg* dialog);

} //unnamed namespace

namespace hooks {

//Main function when it comes to display the stats
//part at the bottom of the screen
void function_00458120() {

	struct UnitStatFuncArrayEntry {
		u32 unitId;	//unused?
		u32 UnitStatCondFunc;
		u32 UnitStatActFunc;
	};

	UnitStatFuncArrayEntry* const unitStatFuncs	= (UnitStatFuncArrayEntry*)	0x005193A0;

	u8*		const u8_0068C1E5				= (u8*)						0x0068C1E5;
	Bool8*	const bCanUpdateStatDataDialog	= (Bool8*)					0x0068C1F8;

	CUnit* activeUnit = *activePortraitUnit;
	BinDlg* dialog_0068C1F0 = *(BinDlg**)(0x0068C1F0);

	if(activeUnit == NULL) {

		if(*bCanUpdateStatDataDialog == 0)
			*bCanUpdateStatDataDialog = 0;
		else {

			BinDlg* current_dialog;

			if(dialog_0068C1F0->controlType != DialogControlTypes::DialogBox)
				current_dialog = dialog_0068C1F0;
			else
				current_dialog = dialog_0068C1F0->childrenDlg;

			while(current_dialog != NULL) {
				hideDialog(current_dialog);
				current_dialog = current_dialog->next;
			}

			*u8_0068C1E5 = 0;
			*bCanUpdateStatDataDialog = 0;

		}

	}
	else
	if(*clientSelectionCount == 1) {

		bool canDisplay = true;

		if(*bCanUpdateStatDataDialog == 0)
			canDisplay = UnitStatCondFunc(unitStatFuncs[activeUnit->id].UnitStatCondFunc);

		if(!canDisplay)
			*bCanUpdateStatDataDialog = 0;
		else {
			UnitStatActFunc(unitStatFuncs[activeUnit->id].UnitStatActFunc, dialog_0068C1F0);
			function_004568B0();
			function_00457FE0();
			*bCanUpdateStatDataDialog = 0;
		}

	}
	else { //*clientSelectionCount != 1

		bool canDisplay = true;

		if(*bCanUpdateStatDataDialog == 0)
			canDisplay = UnitStatCond_Selection_Helper();

		if(!canDisplay)
			*bCanUpdateStatDataDialog = 0;
		else {
			UnitStatAct_Selection_Helper(dialog_0068C1F0);
			function_00457FE0();
			*bCanUpdateStatDataDialog = 0;
		}

	}

}

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

const u32 Func_Sub424660 = 0x00424660;
//was sub_424660
//return true if the selection display
//should be shown or updated
bool UnitStatCond_Selection_Helper() {

	static Bool32 bPreResult;

	__asm {
		PUSHAD
		CALL Func_Sub424660
		MOV bPreResult, EAX
		POPAD
	}

	return (bPreResult != 0);

}

;

const u32 Func_Sub425960 = 0x00425960;
//set the display when several units are
//selected
void UnitStatAct_Selection_Helper(BinDlg* dialog) {

	__asm {
		PUSHAD
		MOV EAX, dialog
		CALL Func_Sub425960
		POPAD
	}

}

;

const u32 Func_Sub4568B0 = 0x004568B0;
//Take a dialog (not identified at this time),
//make *activePortraitUnit its unitUser, display
//or hide the dialog depending on the unit existing,
//then update or not the dialog depending on its flags.
void function_004568B0() {

	__asm {
		PUSHAD
		CALL Func_Sub4568B0
		POPAD
	}

}

;

const u32 Func_Sub457FE0 = 0x00457FE0;
//Probably check mouse position to update the dialog
//below if it react to the mouse cursor being over it
void function_00457FE0() {

	__asm {
		PUSHAD
		CALL Func_Sub457FE0
		POPAD
	}

}

;

bool UnitStatCondFunc(u32 funcOffset) {

	static Bool32 bPreResult;

	__asm {
		PUSHAD
		CALL funcOffset
		MOV bPreResult, EAX
		POPAD
	}

	return (bPreResult != 0);

}

;

void UnitStatActFunc(u32 funcOffset, BinDlg* dialog) {

	__asm {
		PUSHAD
		MOV ECX, dialog
		CALL funcOffset
		POPAD
	}

}

;

} //unnamed namespace

//End of helper functions
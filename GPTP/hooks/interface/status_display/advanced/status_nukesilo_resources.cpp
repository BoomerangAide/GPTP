#include "status_nukesilo_resources.h"
#include <SCBW/api.h>

//Helper functions declaration

namespace {

void showDialog(BinDlg* dialog);														//186A0
void hideDialog(BinDlg* dialog);														//18700
void function_00418E00(BinDlg* dialog);													//18E00
void updateDialog(BinDlg* dialog);														//1C400

void SC_sprintf_s(char* buffer, int buffer_size, const char* format_string, int value);	//1F1B0
void SC_sprintf_s(char* buffer, int buffer_size, const char* format_string, int value1, 
				  int value2);															//1F1B0

void StatsShieldLevel_Helper(BinDlg* dialog, u32 index);								//25510
void AddTextToDialog(BinDlg* dialog, int index, char* textOffset);						//258B0
void stats_panel_display_Helper(BinDlg* dialog);										//26C60
void DLGsetProgressBarValue(BinDlg* dialog, int index, u32 progress);					//57200
void function_00457250(BinDlg* dialog, int index, u32 unk);								//57250
void function_00457310(BinDlg* dialog);													//57310
u32 function_004669E0(CUnit* unit);														//669E0
bool isAttemptingProtossBuild(CUnit* unit);												//E4C40
bool isConstructingAddon(CUnit* unit);													//E66B0

} //unnamed namespace

const u32 selectionVariableData1Offset = 0x006CA93C; //Since the data type can vary, only the offset is global variable

char**	const	queues_buffers			= (char**)	0x00519F40; //array of buffers for strings in production queues
u32*	const	u32_0068C1E0			= (u32*)	0x0068C1E0;
u8*		const	 u8_0068C1E5			= (u8*)		0x0068C1E5;
char*	const	buffer_resource_amount	= (char*)	0x006CA9F8;

const int TEXTLABELINDEX_UNUSED_TEXT_LINE_4 = -13;
const int TEXTLABELINDEX_UNUSED_TEXT_LINE_3 = -12;
const int TEXTLABELINDEX_RESOURCE_AMOUNT	= -11;
const int TEXTLABELINDEX_UNUSED_TEXT_LINE_1 = -10;

namespace hooks {

//00426190
void stats_resources(BinDlg* dialog) {

	CUnit* unit = *activePortraitUnit;
	BinDlg* current_dialog;
	char* resource_type_text = NULL;

	*(u16*)selectionVariableData1Offset = unit->building.resource.resourceAmount;

	if(*u8_0068C1E5 != 15) {
		function_00457310(dialog);
		function_00457250(dialog,-10,-13);
		*u8_0068C1E5 = 15;
	}

	AddTextToDialog(dialog,TEXTLABELINDEX_UNUSED_TEXT_LINE_1,NULL);
	AddTextToDialog(dialog,TEXTLABELINDEX_UNUSED_TEXT_LINE_3,NULL);
	AddTextToDialog(dialog,TEXTLABELINDEX_UNUSED_TEXT_LINE_4,NULL);

	if(unit->id >= UnitId::ResourceMineralField && unit->id <= UnitId::ResourceMineralFieldType3) {
		SC_sprintf_s(
			buffer_resource_amount,
			260,
			FORMATSTRING_TEXT_SPACE_VALUE,
			(int)statTxtTbl->getString(0x31C), //"Minerals:"
			unit->building.resource.resourceAmount
		);
		AddTextToDialog(dialog,TEXTLABELINDEX_RESOURCE_AMOUNT,buffer_resource_amount);
	}
	else 
	if(unit->building.resource.resourceAmount != 0) {
		SC_sprintf_s(
			buffer_resource_amount,
			260,
			FORMATSTRING_TEXT_SPACE_VALUE,
			(int)statTxtTbl->getString(0x31D), //"Vespene Gas:"
			unit->building.resource.resourceAmount
		);
		AddTextToDialog(dialog,TEXTLABELINDEX_RESOURCE_AMOUNT,buffer_resource_amount);
	}
	else
		AddTextToDialog(
			dialog,
			TEXTLABELINDEX_RESOURCE_AMOUNT,
			(char*)statTxtTbl->getString(0x31E) //"Depleted"
		);

	if(units_dat::ShieldsEnabled[unit->id])
		StatsShieldLevel_Helper(dialog,0);
	else {

		if(dialog->controlType == DialogControlTypes::DialogBox)
			current_dialog = dialog->childrenDlg;
		else
			current_dialog = dialog->parent->childrenDlg;

		while(current_dialog != NULL && current_dialog->index != 9)
			current_dialog = current_dialog->next;

		hideDialog(current_dialog);

	}

}

;

//00426FF0
void stats_nukesilo(BinDlg* dialog) {

	CUnit* unit = *activePortraitUnit;
	BinDlg* current_dialog;

	//buildings id start after disruption web id
	//block use of buildings id, but only normal
	//use case is last id UnitId::None
	if(unit->buildQueue[unit->buildQueueSlot % 5] > UnitId::Spell_DisruptionWeb) { //271D0
		
		if(*(u16*)u8_0068C1E5 != 3) {
			function_00457310(dialog);
			function_00457250(dialog,-20,12);
			*u8_0068C1E5 = 3;
		}

		stats_panel_display_Helper(dialog);
	
		if(dialog->controlType == DialogControlTypes::DialogBox)
			current_dialog = dialog->childrenDlg;
		else
			current_dialog = dialog->parent->childrenDlg;

		while(current_dialog != NULL && current_dialog->index != -20)
			current_dialog = current_dialog->next;

		hideDialog(current_dialog);

		if(dialog->controlType == DialogControlTypes::DialogBox)
			current_dialog = dialog->childrenDlg;
		else
			current_dialog = dialog->parent->childrenDlg;

		while(current_dialog != NULL && current_dialog->index != -21)
			current_dialog = current_dialog->next;

		hideDialog(current_dialog);

	}
	else { 

		CUnit* target = NULL;
		int progress;
		u8 raceId;
		u16 builtUnitId;
		
		//2701E
		if(dialog->controlType == DialogControlTypes::DialogBox)
			current_dialog = dialog->childrenDlg;
		else
			current_dialog = dialog->parent->childrenDlg;

		while(current_dialog != NULL && current_dialog->index != 2)
			current_dialog = current_dialog->next;

		if(*u8_0068C1E5 != 14) {
			function_00457310(dialog);
			*u8_0068C1E5 = 14;
		}

		if(
			unit->mainOrderId == OrderId::BuildTerran ||
			unit->mainOrderId == OrderId::Repair1 ||
			unit->mainOrderId == OrderId::ConstructingBuilding ||
			isAttemptingProtossBuild(unit)
		)
			target = unit->orderTarget.unit;
		else
		if(
			unit->secondaryOrderId == OrderId::Train ||
			unit->secondaryOrderId == OrderId::TrainFighter ||
			isConstructingAddon(unit)
		)
			target = unit->currentBuildUnit;

		if(target != NULL) {
			progress = function_004669E0(target);
			builtUnitId = target->id;
		}
		else { //2713D
			progress = 0;
			builtUnitId = 0;
		}

		//270B4
		DLGsetProgressBarValue(dialog,7,progress);
		SC_sprintf_s(queues_buffers[0],8,FORMATSTRING_VALUE_SPACE,1); //active slot number display

		current_dialog->graphic = 2;
		current_dialog->statUser->unkUser_00 = *u32_0068C1E0;
		current_dialog->statUser->iconId_04 = builtUnitId;
		current_dialog->statUser->unknown_06 = 3;
		current_dialog->statUser->id_08 = builtUnitId;		

		function_00418E00(current_dialog);

		current_dialog->pszText = queues_buffers[0];
		showDialog(current_dialog);

		if(!(current_dialog->flags & BinDlgFlags::Unknown0)) {
			current_dialog->flags |= BinDlgFlags::Unknown0;
			updateDialog(current_dialog);
		}

		if(units_dat::GroupFlags[unit->id].isZerg)
			raceId = RaceId::Zerg;
		else
		if(units_dat::GroupFlags[unit->id].isProtoss)
			raceId = RaceId::Protoss;
		else
		if(units_dat::GroupFlags[unit->id].isTerran)
			raceId = RaceId::Terran;
		else
			raceId = RaceId::Neutral;

		AddTextToDialog(
			dialog,
			-15,
			(char*)statTxtTbl->getString(0x303 + raceId) //Zerg: "Morphing", Terran: "Building", Protoss: "Opening Warp Gate"
		);

	}

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

const u32 Func_Sub418E00 = 0x00418E00;
void function_00418E00(BinDlg* dialog) {

	__asm {
		PUSHAD
		MOV ESI, dialog
		CALL Func_Sub418E00
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

const u32 Func_sprintf_s = 0x0041F1B0;
//Note: buffer_size may be max number of characters * 2, possibly for 16bits text support

void SC_sprintf_s(char* buffer, int buffer_size, const char* format_string, int value1, int value2) {

	__asm {

		PUSHAD

		PUSH value2
		PUSH value1
		PUSH format_string
		MOV ESI, buffer_size
		MOV EDI, buffer

		CALL Func_sprintf_s
		ADD ESP, 0x0C

		POPAD

	}

}

;

void SC_sprintf_s(char* buffer, int buffer_size, const char* format_string, int value) {

	__asm {

		PUSHAD

		PUSH value
		PUSH format_string
		MOV ESI, buffer_size
		MOV EDI, buffer

		CALL Func_sprintf_s
		ADD ESP, 0x08

		POPAD

	}

}

;

const u32 Func_Sub425510 = 0x00425510;
void StatsShieldLevel_Helper(BinDlg* dialog, u32 index) {

	__asm {
		PUSHAD
		MOV EAX, dialog
		PUSH index
		CALL Func_Sub425510
		POPAD
	}

}

;

const u32 Func_AddTextToDialog = 0x004258B0;
void AddTextToDialog(BinDlg* dialog, int index, char* textOffset) {

	__asm {
		PUSHAD
		PUSH textOffset
		MOV ECX, index
		MOV EAX, dialog
		CALL Func_AddTextToDialog
		POPAD
	}

}

;

const u32 Func_Sub426C60 = 0x00426C60;
//Display icons with weapon, shield, armor...
void stats_panel_display_Helper(BinDlg* dialog) {

	__asm {
		PUSHAD
		PUSH dialog
		CALL Func_Sub426C60
		POPAD
	}

}

;

const u32 Func_DLGsetProgressBarValue = 0x00457200;
void DLGsetProgressBarValue(BinDlg* dialog, int index, u32 progress) {

	__asm {
		PUSHAD
		PUSH progress
		MOV ECX, index
		MOV EAX, dialog
		CALL Func_DLGsetProgressBarValue
		POPAD
	}

}

;

const u32 Func_Sub457250 = 0x00457250;
//Can use mouse coordinates, dialog->fxnInteract and
//updateDialog.
//Used by UnitStatAct_Standard, UnitStatAct_Egg, 
//UnitStatAct_Archon and UnitStatAct_Building.
void function_00457250(BinDlg* dialog, int index, u32 unk) {
	__asm {
		PUSHAD
		MOV EAX, dialog
		MOV ECX, index
		PUSH unk
		CALL Func_Sub457250
		POPAD
	}
}

;

const u32 Func_Sub457310 = 0x00457310;
//Hide some parts of the dialog
void function_00457310(BinDlg* dialog) {

	__asm {
		PUSHAD
		MOV EAX, dialog
		CALL Func_Sub457310
		POPAD
	}

}

;

//Identical to function @ 0x004669E0
//Return the progress percentage of a
//building unit
//unit = ESI
u32 function_004669E0(CUnit* unit) {

	u32 return_value;
	u32 timeCost;
	u16 unitId = unit->id;

	if(
		unitId == UnitId::ZergEgg ||
		unitId == UnitId::ZergCocoon ||
		unitId == UnitId::ZergLurkerEgg ||
		unit->isRemorphingBuilding()
	)
		unitId = unit->buildQueue[unit->buildQueueSlot];

	timeCost = units_dat::TimeCost[unitId];

	return_value = (100 * (timeCost - unit->remainingBuildTime) ) / timeCost;

	return return_value;

}

;

//Identical to isAttemptingProtossBuild @ 0x004E4C40
bool isAttemptingProtossBuild(CUnit* unit) {

	bool return_value = true;

	if(
		unit->mainOrderId != OrderId::BuildProtoss1 ||
		!(unit->status & UnitStatus::GroundedBuilding) ||
		unit->orderTarget.unit == NULL ||
		(unit->orderTarget.unit)->status & UnitStatus::Completed
	)
		return_value = false;

	return return_value;

}

;

//Identical to isConstructingAddon @ 0x004E66B0
bool isConstructingAddon(CUnit* unit) {

	bool return_value = true;

	if(
		unit->secondaryOrderId != OrderId::BuildAddon ||
		!(unit->status & UnitStatus::GroundedBuilding) ||
		unit->currentBuildUnit == NULL ||
		unit->currentBuildUnit->status & UnitStatus::Completed
	)
		return_value = false;

	return return_value;

}

;

} //unnamed namespace

//End of helper functions
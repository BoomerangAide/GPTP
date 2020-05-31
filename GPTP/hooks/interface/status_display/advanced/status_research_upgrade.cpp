#include "status_research_upgrade.h"
#include <SCBW/api.h>

//Helper functions declaration

namespace {

void function_00418E00(BinDlg* dialog);													//18E00
void updateDialog(BinDlg* dialog);														//1C400
void AddTextToDialog(BinDlg* dialog, int index, char* textOffset);						//258B0
u32 getUpgradeTimeCost(u8 playerId, u8 upgradeId);										//53F70
void DLGsetProgressBarValue(BinDlg* dialog, int index, u32 progress);					//57200
void function_00457250(BinDlg* dialog, int index, u32 unk);								//57250
void function_00457310(BinDlg* dialog);													//57310

} //unnamed namespace

u32* const	u32_0068C1E0	= (u32*)0x0068C1E0;
u8*	const	u8_0068C1E5		= (u8*)	0x0068C1E5;

namespace hooks {

//Equivalent to function at 0x00426500
void stats_upgrade_in_progress(BinDlg* dialog) {

	CUnit* unit = *activePortraitUnit;

	if(*IS_IN_REPLAY != 0 || unit->playerId == *LOCAL_NATION_ID) {

		u8 raceId;
		u32 upgradeTimeCost;
		char* upgradeText;
		BinDlg* current_dialog;

		if(*u8_0068C1E5 != 8 ){
			function_00457310(dialog);
			function_00457250(dialog,-35,15);
			*u8_0068C1E5 = 8;
		}

		upgradeTimeCost = getUpgradeTimeCost(unit->playerId, unit->building.upgradeType);

		DLGsetProgressBarValue(
			dialog,
			14,
			100 * (upgradeTimeCost - unit->building.upgradeResearchTime) / upgradeTimeCost
		);

		if(dialog->controlType == DialogControlTypes::DialogBox)
			current_dialog = dialog;
		else
			current_dialog = dialog->parent;

		current_dialog = current_dialog->childrenDlg;

		while(current_dialog != NULL && current_dialog->index != 15)
			current_dialog = current_dialog->next;

		current_dialog->graphic = 2;
		current_dialog->statUser->unkUser_00 = *u32_0068C1E0;
		current_dialog->statUser->iconId_04 = upgrades_dat::IconId[unit->building.upgradeType];
		current_dialog->statUser->unknown_06 = 5;
		current_dialog->statUser->id_08 = unit->building.upgradeType;

		function_00418E00(current_dialog);

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

		if(unit->lockdownTimer != 0 || unit->stasisTimer != 0 || unit->maelstromTimer != 0)
			upgradeText = (char*)statTxtTbl->getString(0x33C + raceId); //"Disabled","Disabled" or "Unpowered"
		else
			upgradeText = (char*)statTxtTbl->getString(0x2FD + raceId); //"Evolving","Upgrading" or "Upgrading"

		AddTextToDialog(dialog,-35,upgradeText);

	}

}

;

//Equivalent to function at 0x004266F0
void stats_research_in_progress(BinDlg* dialog) {

	CUnit* unit = *activePortraitUnit;

	if(*IS_IN_REPLAY != 0 || unit->playerId == *LOCAL_NATION_ID) {

		u8 raceId;
		u32 researchTimeCost;
		char* researchText;
		BinDlg* current_dialog;

		if(*u8_0068C1E5 != 7 ){
			function_00457310(dialog);
			function_00457250(dialog,-35,15);
			*u8_0068C1E5 = 7;
		}

		researchTimeCost = techdata_dat::TimeCost[unit->building.techType];

		DLGsetProgressBarValue(
			dialog,
			14,
			100 * (researchTimeCost - unit->building.upgradeResearchTime) / researchTimeCost
		);

		if(dialog->controlType == DialogControlTypes::DialogBox)
			current_dialog = dialog;
		else
			current_dialog = dialog->parent;

		current_dialog = current_dialog->childrenDlg;

		while(current_dialog != NULL && current_dialog->index != 15)
			current_dialog = current_dialog->next;

		current_dialog->graphic = 2;
		current_dialog->statUser->unkUser_00 = *u32_0068C1E0;
		current_dialog->statUser->iconId_04 = techdata_dat::IconId[unit->building.techType];
		current_dialog->statUser->unknown_06 = 4;
		current_dialog->statUser->id_08 = unit->building.techType;

		function_00418E00(current_dialog);

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

		if(unit->lockdownTimer != 0 || unit->stasisTimer != 0 || unit->maelstromTimer != 0)
			researchText = (char*)statTxtTbl->getString(0x33C + raceId); //"Disabled","Disabled" or "Unpowered"
		else
			researchText = (char*)statTxtTbl->getString(0x300 + raceId); //"Evolving,Researching or Developing"

		AddTextToDialog(dialog,-35,researchText);

	}

}

;

} //namespace hooks

//-------- Helper function definitions. Do NOT modify! --------//

namespace {

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

const u32 Func_getUpgradeTimeCost = 0x00453F70;
u32 getUpgradeTimeCost(u8 playerId, u8 upgradeId) {

	static u32 return_value;

	__asm {
		PUSHAD
		MOV AL, playerId
		MOV BL, upgradeId
		CALL Func_getUpgradeTimeCost
		MOV return_value, EAX
		POPAD
	}

	return return_value;

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

} //unnamed namespace

//End of helper functions
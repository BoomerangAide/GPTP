#include "status_buildmorphtrain.h"
#include <SCBW/api.h>

//Helper functions declaration

namespace {

void disableDialog(BinDlg* dialog);														//18640
void function_00418E00(BinDlg* dialog);													//18E00
void updateDialog(BinDlg* dialog);														//1C400
void SC_sprintf_s(char* buffer, int buffer_size, const char* format_string, int value);	//1F1B0
void AddTextToDialog(BinDlg* dialog, int index, char* textOffset);						//258B0
void DLGsetProgressBarValue(BinDlg* dialog, int index, u32 progress);					//57200
void stats_MorphProgress_Helper(BinDlg* dialog, int index, u32 unk);					//57250
void function_00457310(BinDlg* dialog);													//57310
u32 function_004669E0(CUnit* unit);														//669E0
u16 getLastQueueSlotType(CUnit* unit);													//7B270
bool isAttemptingProtossBuild(CUnit* unit);												//E4C40
bool isConstructingAddon(CUnit* unit);													//E66B0

u32 fxnInteract(BinDlg* dialog, u32 data_struct_offset);

} //unnamed namespace

namespace hooks {

//00425F30
void stats_AddOnBuilder(BinDlg* dialog) {

	u32* const	u32_0068C1E0 =	(u32*)	0x0068C1E0; //unknown, maybe not always store the same thing
	u8* const	u8_0068C1E5 =	(u8*)	0x0068C1E5;	//Some kind of ID of current type of display, to not redo memory allocation

	BinDlg* current_dialog;
	CUnit* unit = *activePortraitUnit;
	CUnit* builtUnit;
	int slotUnitId;
	int textId;

	if (*IS_IN_REPLAY != 0 || unit->id == *LOCAL_NATION_ID) {

		if (*u8_0068C1E5 != 9) {
			function_00457310(dialog);
			stats_MorphProgress_Helper(dialog, -40, 17);
			*u8_0068C1E5 = 9;
		}

		if (
			unit->mainOrderId == OrderId::BuildTerran ||
			unit->mainOrderId == OrderId::Repair1 ||
			unit->mainOrderId == OrderId::ConstructingBuilding ||
			isAttemptingProtossBuild(unit)
		)
			builtUnit = unit->orderTarget.unit;
		else
		if (
			unit->secondaryOrderId == OrderId::Train ||
			unit->secondaryOrderId == OrderId::TrainFighter ||
			isConstructingAddon(unit)
		)
			builtUnit = unit->currentBuildUnit;
		else
			builtUnit = NULL;

		DLGsetProgressBarValue(dialog, 16, function_004669E0(builtUnit));

		slotUnitId = getLastQueueSlotType(builtUnit);

		if (dialog->controlType == DialogControlTypes::DialogBox)
			current_dialog = dialog->childrenDlg;
		else
			current_dialog = dialog->parent->childrenDlg;

		while (current_dialog != NULL && current_dialog->index != 17)
			current_dialog = current_dialog->next;

		current_dialog->graphic = 2;
		current_dialog->statUser->unkUser_00 = *u32_0068C1E0;
		current_dialog->statUser->iconId_04 = slotUnitId;
		current_dialog->statUser->unknown_06 = 3;
		current_dialog->statUser->id_08 = slotUnitId;

		if (!(current_dialog->flags & BinDlgFlags::Unknown0)) {
			current_dialog->flags |= BinDlgFlags::Unknown0;
			updateDialog(current_dialog);
		}

		textId = 0x313 + builtUnit->getRace(); //"Mutating", "Adding On", "Opening Warp Rift"
		AddTextToDialog(dialog, -40, (char*)statTxtTbl->getString(textId));

	}

}

//004260C0
void stats_IncompleteUnit(BinDlg* dialog) {

	u8* const u8_0068C1E5 = (u8*)0x0068C1E5;

	CUnit* unit = *activePortraitUnit;
	int textId;

	stats_MorphProgress_Helper(dialog, -30, 13);

	if (*u8_0068C1E5 != 6) {
		function_00457310(dialog);
		*u8_0068C1E5 = 6;
	}

	DLGsetProgressBarValue(dialog, 13, function_004669E0(unit));

	textId = 0x31F + unit->getRace(); //"Mutating" for Zerg, "Under Construction" for Terran, "Opening Warp Rift" for Protoss

	AddTextToDialog(dialog, -30, (char*)statTxtTbl->getString(textId));

}

//004268D0
void stats_QueueProgress(BinDlg* dialog) {

	char**	const	queues_buffers =	(char**)0x00519F40; //array of buffers for strings in production queues
	u32*	const	u32_0068C1C0 =		(u32*)	0x0068C1C0; //unknown, used for empty queue slot case here
	u32*	const	u32_0068C1E0 =		(u32*)	0x0068C1E0; //unknown, maybe not always store the same thing
	u8*		const	u8_0068C1E5 =		(u8*)	0x0068C1E5; //to know if refreshing same info display or new one, probably

	BinDlg* current_dialog;
	CUnit* unit = *activePortraitUnit;

	if (dialog->controlType == DialogControlTypes::DialogBox)
		current_dialog = dialog->childrenDlg;
	else
		current_dialog = dialog->parent->childrenDlg;

	while (current_dialog != NULL && current_dialog->index != 2)
		current_dialog = current_dialog->next;

	if (
		*IS_IN_REPLAY != 0 ||
		unit->playerId == *LOCAL_NATION_ID
	)
	{

		CUnit* builtUnit;
		u32 progress;
		int queueSlot;
		int progressNameTextId;
		char* statusText;

		if (*u8_0068C1E5 != 5) {
			function_00457310(dialog);
			stats_MorphProgress_Helper(dialog, -15, 8);
			*u8_0068C1E5 = 5;
		}

		if (
			unit->mainOrderId == OrderId::BuildTerran ||
			unit->mainOrderId == OrderId::Repair1 ||
			unit->mainOrderId == OrderId::ConstructingBuilding ||
			isAttemptingProtossBuild(unit)
		)
			builtUnit = unit->orderTarget.unit;
		else
		if (
			unit->secondaryOrderId == OrderId::Train ||
			unit->secondaryOrderId == OrderId::TrainFighter ||
			isConstructingAddon(unit)
		)
			builtUnit = unit->currentBuildUnit;
		else
			builtUnit = NULL;

		if (builtUnit != NULL)
			progress = function_004669E0(builtUnit);
		else
			progress = 0;

		DLGsetProgressBarValue(dialog, 7, progress);

		queueSlot = 0;

		while (current_dialog != NULL) {

			if (queueSlot >= 5)
				current_dialog = NULL;
			else {

				char* slot_text;
				int slotUnitId = unit->buildQueue[(unit->buildQueueSlot + queueSlot) % 5];

				if (queueSlot == 0)
					slot_text = (char*)FORMATSTRING_COLORYELLOW_VALUE_COLORCYAN_SPACE;	//queued slot number display (yellow)
				else
					slot_text = (char*)FORMATSTRING_VALUE_SPACE;						//active slot number display (cyan)

				SC_sprintf_s(queues_buffers[queueSlot], 8, slot_text, queueSlot + 1);

				if (queueSlot == 0 || queueSlot >= 4)
					current_dialog->graphic = 2;
				else
					current_dialog->graphic = 4;

				if (slotUnitId != UnitId::None) {

					current_dialog->statUser->unkUser_00 = *u32_0068C1E0;
					current_dialog->statUser->iconId_04 = slotUnitId;
					current_dialog->statUser->unknown_06 = 3;
					current_dialog->statUser->id_08 = slotUnitId;

					function_00418E00(current_dialog);

					current_dialog->pszText = queues_buffers[queueSlot];

				}
				else
				{
					
					current_dialog->statUser->unkUser_00 = *u32_0068C1C0;
					current_dialog->statUser->iconId_04 = queueSlot + 6;
					current_dialog->statUser->unknown_06 = 6;

					disableDialog(current_dialog);

					current_dialog->pszText = NULL;

				}

				if (!(current_dialog->flags & BinDlgFlags::Unknown0)) {
					current_dialog->flags |= BinDlgFlags::Unknown0;
					updateDialog(current_dialog);
				}

				current_dialog = current_dialog->next;
				queueSlot += 1;

			}

		}

		if (
			unit->status & UnitStatus::GroundedBuilding &&
			(
				unit->status & UnitStatus::DoodadStatesThing ||
				unit->lockdownTimer != 0 ||
				unit->stasisTimer != 0 ||
				unit->maelstromTimer != 0
			)
		)
			progressNameTextId = 0x33C; //"<6>Disabled" (<6> means 0x6 byte for red)(Unpowered for Protoss)
		else {

			u32 unitId;

			if (unit->status & UnitStatus::Completed)
				unitId = unit->id;
			else {

				unitId = unit->buildQueue[unit->buildQueueSlot];

				if (
					unitId != UnitId::ZergHive &&
					unitId != UnitId::ZergLair &&
					unitId != UnitId::ZergGreaterSpire &&
					unitId != UnitId::ZergSporeColony &&
					unitId != UnitId::ZergSunkenColony
				)
					unitId = unit->id;

			}

			//note: original code recalculated unitId/builtUnitId, not done here
			//to optimize things

			if (unitId == UnitId::ProtossGateway || unitId == UnitId::ProtossStargate)
				progressNameTextId = 0x303; //"Morphing" ("Building" for Terran," Opening Warp Gate" for Protoss)
			else
				progressNameTextId = 0x306; //"Morphing" ("Building" for Terran and Protoss)

		}

		//Simplified lots of code here, should work
		statusText = (char*)statTxtTbl->getString(progressNameTextId + unit->getRace());

		if (dialog->controlType == DialogControlTypes::DialogBox)
			current_dialog = dialog->childrenDlg;
		else
			current_dialog = dialog->parent->childrenDlg;

		while (current_dialog != NULL && current_dialog->index != -15)
			current_dialog = current_dialog->next;

		current_dialog->pszText = statusText;

		if (!(current_dialog->flags & BinDlgFlags::Visible)) {

			//26BF8

			struct {
				u32 unknown_value;		//[EBP-0x20]	[00]
				u32 always_zero_here;	//[EBP-0x1C]	[04]
				u32 not_allocated_2;	//[EBP-0x18]	[08]
				u16 always_14_here;		//[EBP-0x14]	[0C]
				s16 mouseX;				//[EBP-0x12]	[0E]
				s16 mouseY;				//[EBP-0x10]	[10]
				u32 ignored_3;			//[EBP-0x0C]	[14] //used to store a queueSlot+1 value from earlier
				u16 not_allocated_1;	//[EBP-0x0A]	[16]
				u16 ignored_2;			//[EBP-0x08]	[18] //used to store builtUnitId value from earlier
				u32 ignored_1;			//[EBP-0x04]	[1C] //used to store queueSlot value from earlier
			} stack_placeholder;

			//not allocated on original code, but result
			//is probably more true to the original like
			//this
			stack_placeholder.not_allocated_1 = 0;
			stack_placeholder.not_allocated_2 = 0;

			//Zeroed because ignored, if bugs will have to fill with
			//real values maybe
			stack_placeholder.ignored_1 = 0;
			stack_placeholder.ignored_2 = 0;
			stack_placeholder.ignored_3 = 0;

			stack_placeholder.mouseY = mouse->y;
			stack_placeholder.always_14_here = 14;
			stack_placeholder.unknown_value = 13;
			stack_placeholder.always_zero_here = 0;
			stack_placeholder.mouseX = mouse->x;

			if (
				fxnInteract(current_dialog, (u32)&stack_placeholder) != 0 &&
				!(current_dialog->flags & BinDlgFlags::Unknown0)
			)
			{
				current_dialog->flags |= BinDlgFlags::Unknown0;
				updateDialog(current_dialog);
			}
			

		}

		if(!(current_dialog->flags & BinDlgFlags::Unknown0))
		{
			current_dialog->flags |= BinDlgFlags::Unknown0;
			updateDialog(current_dialog);
		}

	}

}

//00457250
void stats_MorphProgress(BinDlg* dialog, int index, u32 unk) {

	BinDlg* current_dialog;

	if (dialog->controlType == DialogControlTypes::DialogBox)
		current_dialog = dialog->childrenDlg;
	else
		current_dialog = dialog->parent->childrenDlg;

	while (current_dialog != NULL && current_dialog->index != index)
		current_dialog = current_dialog->next;

	while (current_dialog != NULL) {

		if (!(current_dialog->flags & BinDlgFlags::Visible)) {

			struct {
				u32 unknown_value;		//[EBP-0x14]	[00]
				u32 always_zero_here;	//[EBP-0x10]	[04]
				u32 not_allocated_2;	//[EBP-0x0C]	[08]
				u16 always_14_here;		//[EBP-0x08]	[0C]
				s16 mouseX;				//[EBP-0x06]	[0E]
				s16 mouseY;				//[EBP-0x04]	[10]
				u16 not_allocated_1;	//[EBP-0x02]	[12]
			} stack_placeholder;

			//not allocated on original code, but result
			//is probably more true to the original like
			//this
			stack_placeholder.not_allocated_1 = 0;
			stack_placeholder.not_allocated_2 = 0;

			stack_placeholder.mouseY = mouse->y;
			stack_placeholder.always_14_here = 14;
			stack_placeholder.unknown_value = 13;
			stack_placeholder.always_zero_here = 0;
			stack_placeholder.mouseX = mouse->x;

			if (
				fxnInteract(current_dialog, (u32)&stack_placeholder) != 0 &&
				!(current_dialog->flags & BinDlgFlags::Unknown0)
				)
			{
				current_dialog->flags |= BinDlgFlags::Unknown0;
				updateDialog(current_dialog);
			}

		}

		if (current_dialog->index == unk)
			current_dialog = NULL;
		else
			current_dialog = current_dialog->next;

	}

}

;

} //namespace hooks

//-------- Helper function definitions. Do NOT modify! --------//

namespace {

const u32 Func_DisableDialog = 0x00418640;
void disableDialog(BinDlg* dialog) {

	__asm {
		PUSHAD
		MOV ESI, dialog
		CALL Func_DisableDialog
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

const u32 Func_UpdateDialog = 0x0041C400;
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
void stats_MorphProgress_Helper(BinDlg* dialog, int index, u32 unk) {
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

	if (
		unitId == UnitId::ZergEgg ||
		unitId == UnitId::ZergCocoon ||
		unitId == UnitId::ZergLurkerEgg ||
		unit->isRemorphingBuilding()
		)
		unitId = unit->buildQueue[unit->buildQueueSlot];

	timeCost = units_dat::TimeCost[unitId];

	return_value = (100 * (timeCost - unit->remainingBuildTime)) / timeCost;

	return return_value;

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

	if (unit->status & UnitStatus::Completed)
		unitId = unit->id;
	else {

		unitId = unit->buildQueue[unit->buildQueueSlot];

		if (
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

//Identical to isAttemptingProtossBuild @ 0x004E4C40
bool isAttemptingProtossBuild(CUnit* unit) {

	bool return_value = true;

	if (
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

	if (
		unit->secondaryOrderId != OrderId::BuildAddon ||
		!(unit->status & UnitStatus::GroundedBuilding) ||
		unit->currentBuildUnit == NULL ||
		unit->currentBuildUnit->status & UnitStatus::Completed
		)
		return_value = false;

	return return_value;

}

;

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
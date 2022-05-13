#include "status_supply_provider.h"
#include <SCBW/api.h>
#include <Events/Events.h>

//Helper functions declaration

namespace {

void hideDialog(BinDlg* dialog);														//18700

void SC_sprintf_s(char* buffer, int buffer_size, const char* format_string, int value);	//1F1B0
void SC_sprintf_s(char* buffer, int buffer_size, const char* format_string, int value1, 
				  int value2);															//1F1B0

void function_004246D0();																//246D0
void StatsShieldLevel_Helper(BinDlg* dialog, u32 index);								//25510
void StatsArmorLevel_Helper(BinDlg* dialog, u32 index);									//25600
void AddTextToDialog(BinDlg* dialog, int index, char* textOffset);						//258B0
void UnitStatAct_Standard_Helper(BinDlg* dialog);										//26F50
void function_00457250(BinDlg* dialog, int index, u32 unk);								//57250
void function_00457310(BinDlg* dialog);													//57310
u16 getLastQueueSlotType(CUnit* unit);													//7B270

} //unnamed namespace

const u32 selectionVariableData1Offset = 0x006CA93C; //Since the data type can vary, only the offset is global variable
const u32 selectionVariableData2Offset = 0x006CA93E; //Since the data type can vary, only the offset is global variable

u8*	const u8_0068C1E5 = (u8*)0x0068C1E5;

const int TEXTLABELINDEX_MAX_SUPPLY = -13;
const int TEXTLABELINDEX_AVAILABLE_SUPPLY = -12;
const int TEXTLABELINDEX_PROVIDED_SUPPLY = -11;
const int TEXTLABELINDEX_USED_SUPPLY = -10;

namespace hooks {

//00427540
#ifdef EVENTS_SYSTEM
void stats_supply_provider(BinDlg* dialog) {

	CUnit* unit = *activePortraitUnit;

	if(*IS_IN_REPLAY == 0 && unit->playerId != *LOCAL_NATION_ID)
		UnitStatAct_Standard_Helper(dialog);
	else {

		char* BUFFER_1 = (char*)0x006CA9F8;
		char* BUFFER_2 = (char*)0x006CAC78;
		char* BUFFER_3 = (char*)0x006CA668;
		char* BUFFER_4 = (char*)0x006CAB08;

		u8 raceId;
		u16 unitAltType;
		s32 supply_used;
		s32 supply_provided;
		s32 supply_available;
		s32 supply_max;

		//Update selectionVariableDataOffset variables (storing supply available and used)
		function_004246D0();

		if(*u8_0068C1E5 != 12) { //probably clear some cache if a non-supply unit was selected before
			function_00457310(dialog);
			function_00457250(dialog,TEXTLABELINDEX_USED_SUPPLY,-13);	//-13 is (probably) not TEXTLABELINDEX_MAX_SUPPLY
			*u8_0068C1E5 = 12;
		}

		bool cancelDisplay = false;
		std::vector<int*> events_supply_override_arg(1);

		events_supply_override_arg[0] = (int*)unit;

		EventManager::EventCalling(EventId::STATUS_SUPPLY_OVERRIDE, &cancelDisplay, events_supply_override_arg);

		if(!cancelDisplay) {

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

			if(raceId == RaceId::Zerg || raceId == RaceId::Terran || raceId == RaceId::Protoss)
				supply_used = raceSupply[raceId].used[unit->playerId] + 1;	//if only 1 zergling (cost: 0.5 ingame), will display 1 rather than 0 when divided by 2
			else
				supply_used = 0;

			if(supply_used < 0)
				supply_used++;

			char* supplyUsedText = (char*)statTxtTbl->getString(0x335 + raceId); //("Control Used:","Supplies Used:" or "Psi Used:)
			std::vector<int*> events_arg(3);

			supply_used /= 2;

			events_arg[0] = (int*)unit;
			events_arg[1] = (int*)&supplyUsedText;
			events_arg[2] = (int*)&supply_used;

			EventManager::EventCalled(EventId::STATUS_SUPPLY_USED_TEXT, events_arg);

			SC_sprintf_s(
				BUFFER_1,
				260,
				FORMATSTRING_TEXT_SPACE_VALUE,
				(int)supplyUsedText,
				supply_used
			);

			//if only 1 zergling (cost: 0.5 ingame), will display 1 rather than 0 when divided by 2
			supply_provided = units_dat::SupplyProvided[getLastQueueSlotType(unit)] + 1;

			if(supply_provided < 0)
				supply_provided++;

			char* supplyProvidedText = (char*)statTxtTbl->getString(0x32F + raceId); //("Control Provided:","Supplies Provided:" or "Psi Provided:)

			supply_provided /= 2;

			events_arg[0] = (int*)unit;
			events_arg[1] = (int*)&supplyProvidedText;
			events_arg[2] = (int*)&supply_provided;

			EventManager::EventCalled(EventId::STATUS_SUPPLY_PROVIDED_TEXT, events_arg);

			SC_sprintf_s(
				BUFFER_2,
				260,
				FORMATSTRING_TEXT_SPACE_VALUE,
				(int)supplyProvidedText,
				supply_provided
			);

			//if only 1 zergling (cost: 0.5 ingame), will display 1 rather than 0 when divided by 2
			supply_available = scbw::getSuppliesAvailable(unit->playerId,raceId) + 1;

			if(supply_available < 0)
				supply_available++;

			char* supplyTotalText = (char*)statTxtTbl->getString(0x332 + raceId); //("Total Control:","Total Supplies:" or "Total Psi:)

			supply_provided /= 2;

			events_arg[0] = (int*)unit;
			events_arg[1] = (int*)&supplyTotalText;
			events_arg[2] = (int*)&supply_available;

			EventManager::EventCalled(EventId::STATUS_SUPPLY_AVAILABLE_TEXT, events_arg);

			SC_sprintf_s(
				BUFFER_3,
				260,
				FORMATSTRING_TEXT_SPACE_VALUE,
				(int)supplyTotalText,
				supply_available
			);

			unitAltType = getLastQueueSlotType(unit);

			if(units_dat::GroupFlags[unitAltType].isZerg)
				supply_max = raceSupply[RaceId::Zerg].max[unit->playerId];
			else
			if(units_dat::GroupFlags[unitAltType].isTerran)
				supply_max = raceSupply[RaceId::Terran].max[unit->playerId];
			else
			if(units_dat::GroupFlags[unitAltType].isProtoss)
				supply_max = raceSupply[RaceId::Protoss].max[unit->playerId];
			else
				supply_max = 0;

			//if only 1 zergling (cost: 0.5 ingame), will display 1 rather than 0 when divided by 2
			supply_max += 1;

			char* supplyMaxText = (char*)statTxtTbl->getString(0x338 + raceId); //("Control Max:","Supplies Max:","Psi Max:")

			supply_max /= 2;

			events_arg[0] = (int*)unit;
			events_arg[1] = (int*)&supplyMaxText;
			events_arg[2] = (int*)&supply_max;

			EventManager::EventCalled(EventId::STATUS_SUPPLY_AVAILABLE_TEXT, events_arg);

			SC_sprintf_s(
				BUFFER_4,
				260,
				FORMATSTRING_TEXT_SPACE_VALUE,
				(int)supplyMaxText,
				supply_max
			);

			AddTextToDialog(dialog,TEXTLABELINDEX_USED_SUPPLY,BUFFER_1);
			AddTextToDialog(dialog,TEXTLABELINDEX_PROVIDED_SUPPLY,BUFFER_2);
			AddTextToDialog(dialog,TEXTLABELINDEX_AVAILABLE_SUPPLY,BUFFER_3);
			AddTextToDialog(dialog,TEXTLABELINDEX_MAX_SUPPLY,BUFFER_4);

		}

		int index = 0;
		cancelDisplay = false;

		std::vector<int*> events_shieldarmor_override_arg(2);

		events_shieldarmor_override_arg[0] = (int*)unit;
		events_shieldarmor_override_arg[1] = (int*)&index;

		EventManager::EventCalling(EventId::STATUS_DISPLAYING_PANEL_SUPPLY_OR_RESOURCE_SHIELD_OVERRIDE, &cancelDisplay, events_shieldarmor_override_arg);

		if (!cancelDisplay && units_dat::ShieldsEnabled[unit->id]) {
			StatsShieldLevel_Helper(dialog, index);
			index++;
		}

		if (index == 0) {

			cancelDisplay = false;

			EventManager::EventCalling(EventId::STATUS_DISPLAYING_PANEL_SUPPLY_OR_RESOURCE_ARMOR_OVERRIDE, &cancelDisplay, events_shieldarmor_override_arg);

			if (
				!cancelDisplay &&
				(unit->id == UnitId::ZergOverlord || unit->id == UnitId::Hero_Yggdrasill)
			)
			{
				StatsArmorLevel_Helper(dialog, index);
				index++;
			}

		}

		if (index == 0) {
			
			BinDlg* current_dialog;

			if(dialog->controlType == DialogControlTypes::DialogBox)
				current_dialog = dialog;
			else
				current_dialog = dialog->parent;

			current_dialog = current_dialog->childrenDlg;

			while(current_dialog != NULL && current_dialog->index != 9)
				current_dialog = current_dialog->next;

			hideDialog(current_dialog);

		}

	}

} //void stats_supply_provider(BinDlg* dialog)
#else
void stats_supply_provider(BinDlg* dialog) {

	CUnit* unit = *activePortraitUnit;

	if(*IS_IN_REPLAY == 0 && unit->playerId != *LOCAL_NATION_ID)
		UnitStatAct_Standard_Helper(dialog);
	else {

		char* BUFFER_1 = (char*)0x006CA9F8;
		char* BUFFER_2 = (char*)0x006CAC78;
		char* BUFFER_3 = (char*)0x006CA668;
		char* BUFFER_4 = (char*)0x006CAB08;

		u8 raceId;
		u16 unitAltType;
		s32 supply_used;
		s32 supply_provided;
		s32 supply_available;
		s32 supply_max;

		//Update selectionVariableDataOffset variables (storing supply available and used)
		function_004246D0();

		if(*u8_0068C1E5 != 12) { //probably clear some cache if a non-supply unit was selected before
			function_00457310(dialog);
			function_00457250(dialog,TEXTLABELINDEX_USED_SUPPLY,-13);	//-13 is (probably) not TEXTLABELINDEX_MAX_SUPPLY
			*u8_0068C1E5 = 12;
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

		if(raceId == RaceId::Zerg || raceId == RaceId::Terran || raceId == RaceId::Protoss)
			supply_used = raceSupply[raceId].used[unit->playerId] + 1;	//if only 1 zergling (cost: 0.5 ingame), will display 1 rather than 0 when divided by 2
		else
			supply_used = 0;

		if(supply_used < 0)
			supply_used++;

		SC_sprintf_s(
			BUFFER_1,
			260,
			FORMATSTRING_TEXT_SPACE_VALUE,
			(int)statTxtTbl->getString(0x335 + raceId),	//("Control Used:","Supplies Used:" or "Psi Used:)
			supply_used / 2
		);

		//if only 1 zergling (cost: 0.5 ingame), will display 1 rather than 0 when divided by 2
		supply_provided = units_dat::SupplyProvided[getLastQueueSlotType(unit)] + 1;

		if(supply_provided < 0)
			supply_provided++;

		SC_sprintf_s(
			BUFFER_2,
			260,
			FORMATSTRING_TEXT_SPACE_VALUE,
			(int)statTxtTbl->getString(0x32F + raceId),	//("Control Provided:","Supplies Provided:" or "Psi Provided:)
			supply_provided / 2
		);

		//if only 1 zergling (cost: 0.5 ingame), will display 1 rather than 0 when divided by 2
		supply_available = scbw::getSuppliesAvailable(unit->playerId,raceId) + 1;

		if(supply_available < 0)
			supply_available++;

		SC_sprintf_s(
			BUFFER_3,
			260,
			FORMATSTRING_TEXT_SPACE_VALUE,
			(int)statTxtTbl->getString(0x332 + raceId),	//("Total Control:","Total Supplies:" or "Total Psi:)
			supply_available / 2
		);

		unitAltType = getLastQueueSlotType(unit);

		if(units_dat::GroupFlags[unitAltType].isZerg)
			supply_max = raceSupply[RaceId::Zerg].max[unit->playerId];
		else
		if(units_dat::GroupFlags[unitAltType].isTerran)
			supply_max = raceSupply[RaceId::Terran].max[unit->playerId];
		else
		if(units_dat::GroupFlags[unitAltType].isProtoss)
			supply_max = raceSupply[RaceId::Protoss].max[unit->playerId];
		else
			supply_max = 0;

		//if only 1 zergling (cost: 0.5 ingame), will display 1 rather than 0 when divided by 2
		supply_max += 1;

		SC_sprintf_s(
			BUFFER_4,
			260,
			FORMATSTRING_TEXT_SPACE_VALUE,
			(int)statTxtTbl->getString(0x338 + raceId),	//("Control Max:","Supplies Max:","Psi Max:")
			supply_max / 2
		);

		AddTextToDialog(dialog,TEXTLABELINDEX_USED_SUPPLY,BUFFER_1);
		AddTextToDialog(dialog,TEXTLABELINDEX_PROVIDED_SUPPLY,BUFFER_2);
		AddTextToDialog(dialog,TEXTLABELINDEX_AVAILABLE_SUPPLY,BUFFER_3);
		AddTextToDialog(dialog,TEXTLABELINDEX_MAX_SUPPLY,BUFFER_4);

		if (units_dat::ShieldsEnabled[unit->id])
			StatsShieldLevel_Helper(dialog,0);
		else
		if (unit->id == UnitId::ZergOverlord || unit->id == UnitId::Hero_Yggdrasill)
			StatsArmorLevel_Helper(dialog,0);
		else {
			
			BinDlg* current_dialog;

			if(dialog->controlType == DialogControlTypes::DialogBox)
				current_dialog = dialog;
			else
				current_dialog = dialog->parent;

			current_dialog = current_dialog->childrenDlg;

			while(current_dialog != NULL && current_dialog->index != 9)
				current_dialog = current_dialog->next;

			hideDialog(current_dialog);

		}

	}

} //void stats_supply_provider(BinDlg* dialog)
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

//Equivalent to function @ 0x004246D0, only used here in Starcraft
//Update selectionVariableDataOffset variables
void function_004246D0() {

	CUnit* unit = *activePortraitUnit;
	u8 raceId;

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

	*((u32*)selectionVariableData1Offset) =
		scbw::getSuppliesAvailable(unit->playerId,raceId);

	//not recalculating raceId like original code here

	if(raceId == RaceId::Zerg || raceId == RaceId::Terran || raceId == RaceId::Protoss)
		*((u32*)selectionVariableData2Offset) = raceSupply[raceId].used[unit->playerId];
	else
		*((u32*)selectionVariableData2Offset) = 0;

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

const u32 Func_Sub425600 = 0x00425600;
void StatsArmorLevel_Helper(BinDlg* dialog, u32 index) {

	__asm {
		PUSHAD
		MOV EAX, dialog
		PUSH index
		CALL Func_Sub425600
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

const u32 Func_UnitStatAct_Standard = 0x00426F50;
void UnitStatAct_Standard_Helper(BinDlg* dialog) {
	__asm {
		PUSHAD
		MOV ECX, dialog
		CALL Func_UnitStatAct_Standard
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

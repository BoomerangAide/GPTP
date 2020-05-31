#include "status_base_text.h"
#include <SCBW/api.h>

//Helper functions declaration

namespace {

bool unitIsTrainingOrMorphing(CUnit* unit);												//01500
bool unitHpIsInYellowZone(CUnit* unit);													//02270
bool unitHpIsInRedZone(CUnit* unit);													//022C0
BinDlg* getControlFromIndex(BinDlg* dialog, int index);									//18080
void hideDialog(BinDlg* dialog);														//18700

void SC_sprintf_s(char* buffer, int buffer_size, const char* format_string, int value);	//1F1B0
void SC_sprintf_s(char* buffer, int buffer_size, const char* format_string, int value1, 
				  int value2);															//1F1B0
void SC_sprintf_s(char* buffer, int buffer_size, const char* format_string, int value1, 
				  int value2, int value3);												//1F1B0
void SC_sprintf_s(char* buffer, int buffer_size, const char* format_string, int value1, 
				  int value2, int value3, int value4);									//1F1B0
void SC_sprintf_s(char* buffer, int buffer_size, const char* format_string, int value1, 
				  int value2, int value3, int value4, int value5);						//1F1B0

void AddTextToDialog(BinDlg* dialog, int index, char* textOffset);						//258B0
void stats_text_energy_timer_Helper(BinDlg* dialog);									//25A30
char* getUnitRankString(CUnit* unit);													//2F370
u16 getLastQueueSlotType(CUnit* unit);													//7B270

} //unnamed namespace

const int TEXTLABELINDEX_KILLS = -21;
const int TEXTLABELINDEX_UNITSTATUS = -20;
const int TEXTLABELINDEX_ENERGY_TIMER = -8;
const int TEXTLABELINDEX_HP_SHIELDS = -7;

namespace hooks {

//00425A30
void stats_text_energy_timer(BinDlg* dialog) {

	char* TEXT_BUFFER = (char*)0x006CA76C;
	CUnit* unit = *activePortraitUnit;

	if(
		!(units_dat::BaseProperty[unit->id] & UnitProperty::Spellcaster) ||
		unit->status & UnitStatus::IsHallucination ||
		(*IS_IN_REPLAY == 0 && unit->playerId != *LOCAL_NATION_ID)
	)
	{ //25AAA (timer)
		
		if(
			(unit->status & UnitStatus::IsHallucination ||
			unit->id == UnitId::ZergBroodling) &&
			(*IS_IN_REPLAY != 0 || unit->playerId == *LOCAL_NATION_ID)
		)
		{ //25ACF

			int timerMax;
			int currentTimer;

			if(unit->status & UnitStatus::IsHallucination)
				timerMax = 1350;
			else
			if(unit->id == UnitId::ZergBroodling)
				timerMax = 1800;
			else
				timerMax = 0;

			currentTimer = unit->removeTimer / 10;

			if(currentTimer < 0)
				currentTimer++;

			if(timerMax < 0)
				timerMax++;

			SC_sprintf_s(
				TEXT_BUFFER,
				32,
				FORMATSTRING_COLORWHITE_CURRENTVALUE_MAXVALUE,
				currentTimer,
				timerMax
			);

			AddTextToDialog(dialog,TEXTLABELINDEX_ENERGY_TIMER,TEXT_BUFFER);

		}
		else //25B3D
			AddTextToDialog(dialog,TEXTLABELINDEX_ENERGY_TIMER,NULL);

	}
	else
	{ //25A70 (energy)
		SC_sprintf_s(
			TEXT_BUFFER,
			32,
			FORMATSTRING_COLORWHITE_CURRENTVALUE_MAXVALUE,
			unit->energy / 256,
			unit->getMaxEnergy() / 256
		);
		AddTextToDialog(dialog,TEXTLABELINDEX_ENERGY_TIMER,TEXT_BUFFER);
	}

} //void stats_text_energy_timer(BinDlg* dialog)

;

//00425B50
void stats_text_status_ranking(BinDlg* dialog) {

	char* TEXT_BUFFER = (char*)0x006CA818;
	CUnit* unit = *activePortraitUnit;

	if(
		unit->status & UnitStatus::IsHallucination &&
		(*IS_IN_REPLAY != 0 || unit->playerId == *LOCAL_NATION_ID)
	)
		AddTextToDialog(
			dialog,
			TEXTLABELINDEX_UNITSTATUS,
			(char*)statTxtTbl->getString(0x340) //"Hallucination"
		);
	else { //25BC1

		if(
			units_dat::BaseProperty[unit->id] & UnitProperty::Building &&
			(
				unit->status & UnitStatus::DoodadStatesThing ||
				unit->lockdownTimer != 0 ||
				unit->stasisTimer != 0 ||
				unit->maelstromTimer != 0
			) &&
			unit->status & UnitStatus::Completed &&
			!unitIsTrainingOrMorphing(unit) &&
			unit->building.techType == TechId::None &&
			unit->building.upgradeType == UpgradeId::None
		) //disabled building
			AddTextToDialog(
				dialog,
				TEXTLABELINDEX_UNITSTATUS,
				(char*)statTxtTbl->getString(0x33C + unit->getRace()) //"Disabled" (Z),"Disabled" (T),"Unpowered" (P)
			);
		else //25C42
		if(
			unit->acidSporeCount != 0 &&
			unit->status & UnitStatus::Completed
		) 
		{
			SC_sprintf_s(
				TEXT_BUFFER,
				260,
				FORMATSTRING_TEXT_SPACE_VALUE_IN_PARENTHESIS,
				(int)statTxtTbl->getString(0x514), //"Acid Spores"
				unit->acidSporeCount
			);
			AddTextToDialog(dialog,TEXTLABELINDEX_UNITSTATUS,TEXT_BUFFER);
		}
		else
		if(
			unit->isBlind &&
			unit->status & UnitStatus::Completed
		)
			AddTextToDialog(
				dialog,
				TEXTLABELINDEX_UNITSTATUS,
				(char*)statTxtTbl->getString(0x513) //"Blind"
			);
		else
		if(
			unit->parasiteFlags &&
			unit->status & UnitStatus::Completed
		)
			AddTextToDialog(
				dialog,
				TEXTLABELINDEX_UNITSTATUS,
				(char*)statTxtTbl->getString(0x33B) //"Parasite Detected"
			);
		else
		if(unit->canDetect())
			AddTextToDialog(
				dialog,
				TEXTLABELINDEX_UNITSTATUS,
				(char*)statTxtTbl->getString(0x33F) //"Detector"
			);
		else
		if(!(units_dat::BaseProperty[unit->id] & UnitProperty::Building))
			AddTextToDialog(
				dialog,
				TEXTLABELINDEX_UNITSTATUS,
				getUnitRankString(unit)
			);
		else	
			AddTextToDialog(
				dialog,
				TEXTLABELINDEX_UNITSTATUS,
				NULL
			);

	}

} //void stats_text_status_ranking(BinDlg* dialog)

;

//00425DD0
void stats_text_kills(BinDlg* dialog) {

	CUnit* unit = *activePortraitUnit;

	if(
		unit->id == UnitId::ZergScourge ||
		unit->id == UnitId::ZergInfestedTerran ||
		(
			unit->status & UnitStatus::IsHallucination &&
			*IS_IN_REPLAY == 0 &&
			unit->playerId == *LOCAL_NATION_ID
		)
	)
	{ //25E88

		BinDlg* current_dialog;

		if(dialog->controlType == DialogControlTypes::DialogBox)
			current_dialog = dialog;
		else
			current_dialog = dialog->parent;

		current_dialog = current_dialog->childrenDlg;

		while(current_dialog != NULL && current_dialog->index != TEXTLABELINDEX_KILLS)
			current_dialog = current_dialog->next;

		hideDialog(current_dialog);

	}
	else { //25E10

		if(
			unit->hasWeapon() ||
			unit->killCount != 0
		)
		{ //25E3A

			char* BUFFER = (char*)0x006CAC78;

			SC_sprintf_s(
				BUFFER,
				260,
				FORMATSTRING_TEXT_SPACE_VALUE,
				(int)statTxtTbl->getString(0x2FC),	//"Kills:"
				unit->killCount
			);

			AddTextToDialog(dialog,TEXTLABELINDEX_KILLS,BUFFER);

		}
		else //25E25
			hideDialog(getControlFromIndex(dialog,TEXTLABELINDEX_KILLS));

	}


} //void stats_text_kills(BinDlg* dialog)

;

//004263E0
//Note: if MAX HP set in a Staredit counterpart is
//above 0xFFFFFF / 0x64 (16777215/100 => 167772),
//initial HP will be wrong (probably maxHP%167772
//with a minimum of 1)
void stats_text_hp_shield(BinDlg* dialog) {

	char* BUFFER = (char*)0x006CA91C;

	CUnit* unit = *activePortraitUnit;
	int unitMaxHP = units_dat::MaxHitPoints[unit->id] / 256;

	if(unitMaxHP == 0)
		unitMaxHP = (unit->hitPoints + 255) / 256;

	if(
		(unitMaxHP == 0 || unitMaxHP <= 9999) &&
		units_dat::MaxShieldPoints[unit->id] <= 9999 &&
		!(unit->status & UnitStatus::Invincible)
	)
	{ //26438

		u8 hpColor;

		if(unitHpIsInRedZone(unit))
			hpColor = TEXTLABELCOLOR::RED;
		else
		if(unitHpIsInYellowZone(unit))
			hpColor = TEXTLABELCOLOR::YELLOW;
		else
			hpColor = TEXTLABELCOLOR::GREEN;

		if(!units_dat::ShieldsEnabled[unit->id])
			SC_sprintf_s(
				BUFFER,
				32,
				FORMATSTRING_COLOR_CURRENTVALUE_MAXVALUE,
				hpColor,
				(unit->hitPoints + 255) / 256,
				unit->getMaxHpInGame()
			);
		else { //26496
			SC_sprintf_s(
				BUFFER,
				32,
				FORMATSTRING_CURRENTVALUE1_MAXVALUE1_COLOR_CURRENTVALUE2_MAXVALUE2,
				unit->shields / 256,
				units_dat::MaxShieldPoints[getLastQueueSlotType(unit)],
				hpColor,
				(unit->hitPoints + 255) / 256,
				unit->getMaxHpInGame()
			);
		}

	}
	else //264E1
		BUFFER = NULL;

	AddTextToDialog(dialog,TEXTLABELINDEX_HP_SHIELDS,BUFFER);
	stats_text_energy_timer_Helper(dialog);

} //void stats_text_hp_shield(BinDlg* dialog)

;

} //namespace hooks

//-------- Helper function definitions. Do NOT modify! --------//

namespace {

//Identical to unitIsTrainingOrMorphing @ 0x00401500
bool unitIsTrainingOrMorphing(CUnit* unit) {

	bool bReturnValue;

	if(unit->buildQueue[unit->buildQueueSlot % 5] != UnitId::None)
		bReturnValue = true;
	else
	if(
		unit->secondaryOrderId == OrderId::BuildAddon &&
		unit->status & UnitStatus::GroundedBuilding &&
		unit->currentBuildUnit != NULL &&
		!(unit->currentBuildUnit->status & UnitStatus::Completed)
	)
		bReturnValue = true;
	else
	if(
		unit->mainOrderId != OrderId::BuildProtoss1 ||
		!(unit->status & UnitStatus::GroundedBuilding) ||
		unit->orderTarget.unit == NULL
	)
		bReturnValue = false;
	else
	if(!(unit->orderTarget.unit->status & UnitStatus::Completed))
		bReturnValue = true;
	else
		bReturnValue = false;

	return bReturnValue;

}

;

const u32 Func_unitHPbetween33_66_percent = 0x00402270;
bool unitHpIsInYellowZone(CUnit* unit) {

  static Bool32 result;

	__asm {
		PUSHAD
		MOV EDX, unit
		CALL Func_unitHPbetween33_66_percent
		MOV result, EAX
		POPAD
	}

	return result != 0;

}

;

const u32 Helper_UnitHpIsInRedZone  = 0x004022C0;
bool unitHpIsInRedZone(CUnit* unit) {

  static Bool32 result;

	__asm {
		PUSHAD
		MOV ECX, unit
		CALL Helper_UnitHpIsInRedZone
		MOV result, EAX
		POPAD
	}

	return result != 0;

}

;

const u32 Func_getControlFromIndex = 0x00418080;
BinDlg* getControlFromIndex(BinDlg* dialog, int index) {

	static BinDlg* control;

	__asm {
		PUSHAD
		MOV ECX, index
		MOV EAX, dialog
		CALL Func_getControlFromIndex
		MOV control, EAX
		POPAD
	}

	return control;

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

const u32 Func_sprintf_s = 0x0041F1B0;

//Note: buffer_size may be max number of characters * 2, possibly for 16bits text support

void SC_sprintf_s(char* buffer, int buffer_size, const char* format_string, int value1, int value2, int value3, int value4, int value5) {

	__asm {

		PUSHAD

		PUSH value5
		PUSH value4
		PUSH value3
		PUSH value2
		PUSH value1
		PUSH format_string
		MOV ESI, buffer_size
		MOV EDI, buffer
		CALL Func_sprintf_s
		ADD ESP, 0x18

		POPAD

	}

}

;

void SC_sprintf_s(char* buffer, int buffer_size, const char* format_string, int value1, int value2, int value3, int value4) {

	__asm {

		PUSHAD

		PUSH value4
		PUSH value3
		PUSH value2
		PUSH value1
		PUSH format_string
		MOV ESI, buffer_size
		MOV EDI, buffer
		CALL Func_sprintf_s
		ADD ESP, 0x14

		POPAD

	}

}

;

void SC_sprintf_s(char* buffer, int buffer_size, const char* format_string, int value1, int value2, int value3) {

	__asm {

		PUSHAD

		PUSH value3
		PUSH value2
		PUSH value1
		PUSH format_string
		MOV ESI, buffer_size
		MOV EDI, buffer

		CALL Func_sprintf_s
		ADD ESP, 0x10

		POPAD

	}

}

;

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

const u32 Func_stats_text_energy_timer_Helper = 0x00425A30;
void stats_text_energy_timer_Helper(BinDlg* dialog) {
	__asm {
		PUSHAD
		MOV EBX, dialog
		CALL Func_stats_text_energy_timer_Helper
		POPAD
	}
}

;

const u32 Func_getUnitRankString = 0x0042F370;
char* getUnitRankString(CUnit* unit) {

	char* return_value;

	if(
		units_dat::BaseProperty[unit->id] & UnitProperty::NeutralAccessories ||
		unit->id == UnitId::TerranCivilian ||
		unit->id == UnitId::TerranVultureSpiderMine
	)
		return_value = NULL;
	else
	if(unit->id == UnitId::Hero_Gantrithor) {
		if(units_dat::MapStringId[UnitId::Hero_Gantrithor] != 0)
			return_value = NULL;
		else
			return_value = (char*)statTxtTbl->getString(0x23C); //"Tassadar"
	}
	else
	if(
		(
			unit->id == UnitId::TerranBattlecruiser ||
			unit->id == UnitId::Hero_NoradII ||
			unit->id == UnitId::Hero_Hyperion
		)
		&&
			units_dat::BaseProperty[unit->id] & UnitProperty::Hero
		&&
			units_dat::MapStringId[unit->id] == 0
	)
		return_value = (char*)statTxtTbl->getString(0x23B); //"Admiral"
	else
	if(unit->getRace() != RaceId::Terran)
		return_value = NULL;
	else {
		return_value = (char*)statTxtTbl->getString(
							0x516 +							//"Recruit"
							units_dat::Rank[unit->id] + 
							unit->rankIncrease
						);
	}

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

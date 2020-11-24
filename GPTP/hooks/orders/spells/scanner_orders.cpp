#include "scanner_orders.h"
#include <SCBW/api.h>

//helper functions def

namespace {

void fixTargetLocation(Point16* coords, u32 unitId);		//01FA0
void setUnitStatTxtErrorMsg(char* message);					//8CCB0
void displayLastNetErrForPlayer(u32 playerId);				//9E530
void updateUnitStrength(CUnit* unit);						//9FA40
void function_004A01F0(CUnit* unit);						//A01F0
CUnit* createUnit(u32 unitId, int x, int y, u32 playerId);	//A09D0

} //unnamed namespace

namespace hooks {

//Order performed by the scanner unit
//Renamed from unknown function at 00463D30
void orders_Scanner(CUnit* unit) {
	if (unit->orderSignal & 4) {
		unit->orderSignal -= 4;
		unit->remove();
	}
}

;

//Order performed by the Comsat station
void orders_PlaceScanner(CUnit* unit) {

	if (
		*CHEAT_STATE & CheatFlags::TheGathering ||
		unit->energy >= (techdata_dat::EnergyCost[TechId::ScannerSweep] * 256)
	)
	{
		
		Point16 pos;
		CUnit* scannerSpellUnit;

		pos.x = unit->orderTarget.pt.x;
		pos.y = unit->orderTarget.pt.y;

		fixTargetLocation(&pos, UnitId::Spell_ScannerSweep);

		scannerSpellUnit = createUnit(UnitId::Spell_ScannerSweep, pos.x, pos.y, unit->playerId);

		if (scannerSpellUnit != NULL) {

			function_004A01F0(scannerSpellUnit);
			updateUnitStrength(scannerSpellUnit);

			scbw::playSound(SoundId::Terran_bldg_tcsSca00_WAV, unit);

			if (!(*CHEAT_STATE & CheatFlags::TheGathering))
				unit->energy -= (techdata_dat::EnergyCost[TechId::ScannerSweep] * 256);

			unit->orderToIdle();

		}
		else
		{
			displayLastNetErrForPlayer(unit->playerId);
			unit->orderToIdle();
		}

	}
	else
	{

		u8 raceId;
		char* error_msg;

		if (units_dat::GroupFlags[unit->id].isZerg)
			raceId = RaceId::Zerg;
		else
		if (units_dat::GroupFlags[unit->id].isTerran)
			raceId = RaceId::Terran;
		else
		if (units_dat::GroupFlags[unit->id].isProtoss)
			raceId = RaceId::Protoss;
		else
			raceId = RaceId::Neutral;

		error_msg = (char*)statTxtTbl->getString(0x360 + raceId);	//"Not enough energy.<0>"

		if (
			unit->playerId == *LOCAL_NATION_ID &&
			*ACTIVE_PLAYER_ID == *LOCAL_HUMAN_ID
		)
			setUnitStatTxtErrorMsg(error_msg);

		if(unit->orderQueueHead != NULL) {
			unit->userActionFlags |= 1;
			prepareForNextOrder(unit);
		}
		else
		if(unit->pAI != NULL)
			unit->orderComputerCL(OrderId::ComputerAI);
		else
			unit->orderComputerCL(units_dat::ReturnToIdleOrder[unit->id]);

	}

}

;

} //namespace hooks

;

//-------- Helper function definitions. Do NOT modify! --------//

namespace {

const u32 Func_fixTargetLocation = 0x00401FA0;
void fixTargetLocation(Point16* coords, u32 unitId) {

	__asm {
		PUSHAD
		MOV EAX, unitId
		MOV EDX, coords
		CALL Func_fixTargetLocation
		POPAD
	}

}

;

const u32 Func_setUnitStatTxtErrorMsg = 0x0048CCB0;
void setUnitStatTxtErrorMsg(char* message) {

	__asm {
		PUSHAD
		MOV EAX, message
		CALL Func_setUnitStatTxtErrorMsg
		POPAD
	}

}

;

const u32 Func_displayLastNetErrForPlayer = 0x0049E530;
void displayLastNetErrForPlayer(u32 playerId) {
	__asm {
		PUSHAD
		PUSH playerId
		CALL Func_displayLastNetErrForPlayer
		POPAD
	}
}

;

const u32 Func_UpdateUnitStrength = 0x0049FA40;
void updateUnitStrength(CUnit* unit) {

	__asm {
		PUSHAD
		MOV EAX, unit
		CALL Func_UpdateUnitStrength
		POPAD
	}

}

;

const u32 Func_Sub4A01F0 = 0x004A01F0;
void function_004A01F0(CUnit* unit) {

	__asm {
		PUSHAD
		MOV EAX, unit
		CALL Func_Sub4A01F0
		POPAD
	}

}

;

const u32 Func_CreateUnit = 0x004A09D0;
CUnit* createUnit(u32 unitId, int x, int y, u32 playerId) {

	static CUnit* unit_created;

	__asm {
		PUSHAD
		PUSH playerId
		PUSH y
		MOV ECX, unitId
		MOV EAX, x
		CALL Func_CreateUnit
		MOV unit_created, EAX
		POPAD
	}

	return unit_created;

}

;

} //Unnamed namespace

//End of helper functions

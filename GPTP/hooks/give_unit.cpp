#include "give_unit.h"
#include <SCBW/api.h>

//helper functions def

namespace {

	void ApplySpeedUpgradeFromUnitType(CUnit* unit);						//54370
	void function_00463040(CUnit* unit);									//63040
	void function_00476D30(CUnit* unit);									//76D30
	void incrementUnitScores(CUnit* unit, s32 unkScore);					//88BF0
	void incrementUnitScoresEx(CUnit* unit, s32 unkScore, s32 unk2);		//88D50
	void function_004A1E50(CUnit* unit, u32 unk);							//A1E50
	void CreateUnitSelection(CUnit* unit, u32 selectionIndex);				//E6180
	void DestroyUnitSelection(CUnit* unit);									//E6290
	void function_004F3B70(CUnit* powerup);									//F3B70
	
} //unnamed namespace

namespace hooks {

	/// used in completeAddon, sub_4678A0 (related to Terran construction),
	/// GiveAllUnitPropertiesToPlayerAndMakeIdle, itself, sub_4A0080, AI_CreateUnit,
	/// orders_RescuePassive, orders_InfestMine1
	/// Changes would affect all those functions and maybe more
	void GiveUnit(CUnit* unit, u32 playerId, u32 unkScore) {

		if(unit->playerId != playerId) {

			bool isMorphing = false;
			bool isSelected;
			u32 selectionIndex = 0;

			function_004A1E50(unit,0);

			if(unit->isRemorphingBuilding()) {
				unit->status |= UnitStatus::Completed;
				isMorphing = true;
			}

			incrementUnitScores(unit,-1);

			if(unit->status & UnitStatus::Completed)
				incrementUnitScoresEx(unit,0,-1);

			isSelected = (unit->sprite->flags & CSprite_Flags::Selected) == 1;

			if(isSelected) {
				selectionIndex = unit->sprite->selectionIndex;
				DestroyUnitSelection(unit);
			}

			if(unit->player_link.prev != NULL)
				(unit->player_link.prev)->player_link.next = unit->player_link.next;

			if(unit->player_link.next != NULL)
				(unit->player_link.next)->player_link.prev = unit->player_link.prev;

			if(unit == firstPlayerUnit->unit[unit->playerId])
				firstPlayerUnit->unit[unit->playerId] = unit->player_link.next;

			unit->player_link.prev = NULL;
			unit->player_link.next = NULL;
			unit->playerId = playerId;

			//maybe "if" not needed?
			if(isSelected)
				CreateUnitSelection(unit,selectionIndex);

			unit->player_link.prev = NULL;
			unit->player_link.next = firstPlayerUnit->unit[playerId];

			if(firstPlayerUnit->unit[playerId] != NULL)
				(firstPlayerUnit->unit[playerId])->player_link.prev = unit;

			firstPlayerUnit->unit[playerId] = unit;

			incrementUnitScores(unit,1);

			if(unit->status & UnitStatus::Completed)
				incrementUnitScoresEx(unit,unkScore,1);

			if(isMorphing)
				unit->status &= ~UnitStatus::Completed;

			function_00476D30(unit);

			scbw::refreshConsole();

			if(
				units_dat::GroupFlags[unit->id].isZerg != 1 &&
				units_dat::GroupFlags[unit->id].isProtoss != 0 &&
				unit->status & UnitStatus::GroundedBuilding
			)
				*canUpdatePoweredStatus = 1;

			function_00463040(unit);

			if(unit->subunit != NULL && units_dat::BaseProperty[unit->subunit->id] & UnitProperty::Subunit)
				GiveUnit(unit->subunit,playerId,unkScore);

			ApplySpeedUpgradeFromUnitType(unit);

			if(
				units_dat::BaseProperty[unit->id] & UnitProperty::Worker &&
				unit->worker.powerup != NULL &&
				(unit->worker.powerup)->id == UnitId::Powerup_Flag) 
			{
				if((unit->worker.powerup)->playerId == playerId || playerId >= 8)
					function_004F3B70(unit->worker.powerup); //return the flag to initial position?
			}

		}

	}

	;

} //namespace hooks

;

//-------- Helper function definitions. Do NOT modify! --------//

namespace {

	const u32 Func_ApplySpeedUpgradeFromUnitType = 0x00454370;
	void ApplySpeedUpgradeFromUnitType(CUnit* unit) {

		__asm {
			PUSHAD
			MOV ESI, unit
			CALL Func_ApplySpeedUpgradeFromUnitType
			POPAD
		}

	}

	;

	const u32 Func_Sub463040 = 0x00463040;
	void function_00463040(CUnit* unit) {

		__asm {
			PUSHAD
			MOV EAX, unit
			CALL Func_Sub463040
			POPAD
		}

	}

	;

	const u32 Func_Sub476D30 = 0x00476D30;
	void function_00476D30(CUnit* unit) {

		__asm {
			PUSHAD
			MOV EDI, unit
			CALL Func_Sub476D30
			POPAD
		}

	}

	;

	const u32 Func_incrementUnitScores = 0x00488BF0;
	void incrementUnitScores(CUnit* unit, s32 unkScore) {

		__asm {
			PUSHAD
			MOV EAX, unit
			PUSH unkScore
			CALL Func_incrementUnitScores
			POPAD
		}

	}

	;

	const u32 Func_incrementUnitScoresEx = 0x00488D50;
	void incrementUnitScoresEx(CUnit* unit, s32 unkScore, s32 unk2) {

		__asm {
			PUSHAD
			MOV EDI, unit
			MOV ECX, unk2
			PUSH unkScore
			CALL Func_incrementUnitScoresEx
			POPAD
		}

	}

	;

	const u32 Func_Sub4A1E50 = 0x004A1E50;
	void function_004A1E50(CUnit* unit, u32 unk) {

		__asm {
			PUSHAD
			MOV ECX, unit
			MOV EDX, unk
			CALL Func_Sub4A1E50
			POPAD
		}

	}

	;

	const u32 Func_CreateUnitSelection = 0x004E6180;
	void CreateUnitSelection(CUnit* unit, u32 selectionIndex) {

		__asm {
			PUSHAD
			MOV EAX, unit
			PUSH selectionIndex
			CALL Func_CreateUnitSelection
			POPAD
		}

	}

	;

	const u32 Func_DestroyUnitSelection = 0x004E6290;
	void DestroyUnitSelection(CUnit* unit) {

		__asm {
			PUSHAD
			MOV EAX, unit
			CALL Func_DestroyUnitSelection
			POPAD
		}

	}

	;

	const u32 Func_Sub4F3B70 = 0x004F3B70;
	void function_004F3B70(CUnit* powerup) {

		__asm {
			PUSHAD
			PUSH powerup
			CALL Func_Sub4F3B70
			POPAD
		}
	}

	;

} //Unnamed namespace

//End of helper functions

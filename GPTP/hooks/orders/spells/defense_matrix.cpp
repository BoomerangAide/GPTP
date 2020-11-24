#include "defense_matrix.h"
#include <SCBW/api.h>

//helper functions def

namespace {

void CreateDefensiveMatrix_Helper(CUnit* unit);																		//54F90
u32 getSpellStatString(CUnit* target, u32 techId, u8 playerId);														//91E80
bool ordersSpell_Sub_4926D0(CUnit* unit, u32 techId, u16* techEnergyCost, u32 sightRange,u32 error_message_index);	//926D0
void createBottomOverlay(CSprite* sprite, u32 imageId, s32 x, s32 y, u32 direction);								//98D70

} //unnamed namespace

namespace hooks {

void CreateDefensiveMatrix(CUnit* unit) {

	if (
		unit->defensiveMatrixTimer == 0 &&
		!(unit->status & UnitStatus::Burrowed)
	)
	{

		CUnit* current_unit;
		u8 overlaySize;

		if (units_dat::BaseProperty[unit->id] & UnitProperty::MediumOverlay)
			overlaySize = 1;
		else
		if (units_dat::BaseProperty[unit->id] & UnitProperty::LargeOverlay)
			overlaySize = 2;
		else
			overlaySize = 0;

		if (unit->subunit != NULL)
			current_unit = unit->subunit;
		else
			current_unit = unit;

		current_unit->sprite->createTopOverlay(ImageId::DefensiveMatrixFront_Small + overlaySize, 0, 0, 0);
		createBottomOverlay(unit->sprite, ImageId::DefensiveMatrixBack_Small + overlaySize, 0, 0, 0);

	}

	unit->defensiveMatrixHp = 64000;	//(250 * 256)
	unit->defensiveMatrixTimer = 168;

}

;

//Note: unrelated to orders, called by ApplyUnitEffects(unit) at 00492DA0.
//Only other user of CreateDefensiveMatrix(unit) besides the order below.
void ApplyDefensiveMatrix(CUnit* unit) {

	u16 previousMatrixHP;
	u8 previousTimer;

	if (unit->defensiveMatrixTimer != 0) {

		previousTimer = unit->defensiveMatrixTimer;

		if (
			unit->status & UnitStatus::IsHallucination ||
			getSpellStatString(unit, TechId::DefensiveMatrix, unit->playerId) != 0
		)
		{
			unit->defensiveMatrixTimer = 0;
			unit->defensiveMatrixHp = 0;
		}
		else
		{

			previousMatrixHP = unit->defensiveMatrixHp;

			unit->defensiveMatrixTimer = 0;
			unit->defensiveMatrixHp = 0;
			CreateDefensiveMatrix_Helper(unit);

			unit->defensiveMatrixHp = previousMatrixHP;
			unit->defensiveMatrixTimer = previousTimer;

		}

	}

}

;

void orders_DefensiveMatrix(CUnit* unit) {
	
	u16 techCost = 0;
	u32 spellRange = unit->getSightRange(true) * 32;

	if (ordersSpell_Sub_4926D0(unit, TechId::DefensiveMatrix, &techCost, spellRange, 0x370)) //0x370 is "Invalid target.<0>"
	{

		if (!(*CHEAT_STATE & CheatFlags::TheGathering))
			unit->energy -= techCost;

		//Effects on target
		CreateDefensiveMatrix_Helper(unit->orderTarget.unit);

		//Effects (sound and animation) on science vessel casting
		scbw::playSound(SoundId::Terran_VESSEL_TVeDef00_WAV, unit);
		unit->sprite->createOverlay(ImageId::ScienceVesselOverlay_Part2, 0, 0, 0);

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

const u32 Func_CreateDefensiveMatrix = 0x00454F90;
void CreateDefensiveMatrix_Helper(CUnit* unit) {
	__asm {
		PUSHAD
		MOV EDI, unit
		CALL Func_CreateDefensiveMatrix
		POPAD
	}
}

;

const u32 Func_getSpellStatString = 0x00491E80;
u32 getSpellStatString(CUnit* target, u32 techId, u8 playerId) {

	static u32 return_value;

	__asm {
		PUSHAD
		MOV BL, playerId
		PUSH techId
		MOV EAX, target
		CALL Func_getSpellStatString
		MOV return_value, EAX
		POPAD
	}

	return return_value;
}

;

const u32 Func_Sub_4926D0 = 0x004926D0;
bool ordersSpell_Sub_4926D0(CUnit* unit, u32 techId, u16* techEnergyCost, u32 sightRange,u32 error_message_index) {

	static Bool32 bPreResult;

	__asm {
		PUSHAD
		PUSH techEnergyCost
		PUSH sightRange
		PUSH techId
		MOV EAX, error_message_index
		MOV EBX, unit
		CALL Func_Sub_4926D0
		MOV bPreResult, EAX
		POPAD
	}

	return (bPreResult != 0);

}

;

const u32 Func_Sub498D70 = 0x00498D70;
void createBottomOverlay(CSprite* sprite, u32 imageId, s32 x, s32 y, u32 direction) {

	__asm {
		PUSHAD
		MOV EAX, sprite
		MOV ESI, imageId
		PUSH direction
		PUSH y
		PUSH x
		CALL Func_Sub498D70
		POPAD
	}

}

;

} //Unnamed namespace

//End of helper functions

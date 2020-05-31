#include "replace_unit.h"
#include <SCBW/api.h>

//helper functions def

namespace {

void fixTargetLocation(Point16* coords, u32 unitId);							//01FA0
void function_0042D9A0(CUnit* unit);											//2D9A0
void setRepulseAngle(CUnit* unit);												//53300
void removeRepulseTile(CUnit* unit);											//533D0
void function_00469EC0(CUnit* unit, int x, int y);								//69EC0
void function_00469F60(CUnit* unit, int x, int y);								//69F60
void sortUnit(CUnit* unit);														//6A300
void updateUnitOrderFinder(CUnit* unit);										//6A3A0
bool unit_IsStandardAndMovable_Helper(CUnit* unit);								//7B770
void ApplyUnitEffects(CUnit* unit);												//92DA0
void function_0049F6A0(CUnit* unit, u32 unitId);								//9F6A0
void function_0049F7A0(CUnit* unit);											//9F7A0
void function_0049F860(CUnit* unit, u32 unitId);								//9F860
void function_004C3B40(CUnit* unit);											//C3B40

u32 fxnInteract(BinDlg* dialog, u32 data_struct_offset);

} //unnamed namespace

namespace hooks {

//0049FED0
void replaceUnitWithType(CUnit* unit, u16 newUnitId) {

	Bool8* const bCanUpdateSelectedUnitPortrait	= (Bool8*)	0x0068AC74;
	Bool32* const bCanUpdateCurrentButtonSet	= (Bool32*)	0x0068C1B0;
	BinDlg** const someDialogUnknown			= (BinDlg**)0x0068C1E8; //related to MouseOver?
	Bool8* const bCanUpdateStatDataDialog		= (Bool8*)	0x0068C1F8;
	BinDlg** const someDialogUnknownUser		= (BinDlg**)0x0068C1EC; //related to MouseOver? Usually someDialogUnknown->user if not 0 or -1

	Point16 pos;

	if (!(unit->sprite->flags & CSprite_Flags::Hidden)) {

		sortUnit(unit);

		if (unit->status & UnitStatus::GroundedBuilding)
			function_00469EC0(unit, unit->sprite->position.x, unit->sprite->position.y);

		if (unit->status & UnitStatus::InAir)
			removeRepulseTile(unit);

	}

	function_0049F860(unit, newUnitId);

	if (
		(units_dat::BaseProperty[unit->id] & UnitProperty::Building) &&
		(unit->status & (UnitStatus::Cloaked + UnitStatus::RequiresDetection))
	)
	{
		if (unit->secondaryOrderId != OrderId::Nothing2) 
		{
			unit->secondaryOrderId = OrderId::Nothing2;
			unit->secondaryOrderPos.x = 0;
			unit->secondaryOrderPos.y = 0;
			unit->currentBuildUnit = NULL;
			unit->secondaryOrderState = 0;
		}
		unit->status -= (UnitStatus::Cloaked + UnitStatus::RequiresDetection); //already checked they're on
	}

	if(
		unit->sprite->flags & CSprite_Flags::Selected &&
		!unit_IsStandardAndMovable_Helper(unit) &&
		*clientSelectionCount > 1 &&
		newUnitId != UnitId::ZergEgg
	)
	{ 
		function_004C3B40(unit);
		function_0049F7A0(unit);
	}

	*bCanUpdateCurrentButtonSet = 1;
	*bCanUpdateSelectedUnitPortrait = 1;
	*bCanUpdateStatDataDialog = 1;
	*someDialogUnknown = NULL;
	*someDialogUnknownUser = NULL;

	pos.x = unit->sprite->position.x;
	pos.y = unit->sprite->position.y;

	fixTargetLocation(&pos, unit->id);

	if (
		unit->sprite->position.x != pos.x ||
		unit->sprite->position.y != pos.y
	)
		scbw::setUnitPosition(unit, pos.x, pos.y);

	if (!(unit->sprite->flags & CSprite_Flags::Hidden)) {

		updateUnitOrderFinder(unit);

		if (unit->status & UnitStatus::GroundedBuilding) {
			function_00469F60(unit, unit->sprite->position.x, unit->sprite->position.y);
		}

		function_0042D9A0(unit);

		if (unit->status & UnitStatus::InAir)
			setRepulseAngle(unit);

	}

	if (unit->subunit != NULL)
		function_0049F6A0(unit->subunit, units_dat::SubUnit[newUnitId]);

	ApplyUnitEffects(unit);

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

const u32 Func_Sub42D9A0 = 0x0042D9A0;
void function_0042D9A0(CUnit* unit) {
	__asm {
		PUSHAD
		MOV EAX, unit
		CALL Func_Sub42D9A0
		POPAD
	}
}

;

const u32 Func_setRepulseAngle = 0x00453300;
void setRepulseAngle(CUnit* unit) {
	__asm {
		PUSHAD
		MOV ESI, unit
		CALL Func_setRepulseAngle
		POPAD
	}
}

;

const u32 Func_removeRepulseTile = 0x004533D0;
void removeRepulseTile(CUnit * unit) {
	__asm {
		PUSHAD
		MOV EAX, unit
		CALL Func_removeRepulseTile
		POPAD
	}
}

;

const u32 Func_Sub469EC0 = 0x00469EC0;
void function_00469EC0(CUnit* unit, int x, int y) {

	__asm {
		PUSHAD
		MOV EAX, unit
		PUSH y
		PUSH x
		CALL Func_Sub469EC0
		POPAD
	}

}

;

const u32 Func_Sub469F60 = 0x00469F60;
void function_00469F60(CUnit* unit, int x, int y) {
	__asm {
		PUSHAD
		MOV EAX, unit
		PUSH y
		PUSH x
		CALL Func_Sub469F60
		POPAD
	}
}

;

const u32 Func_sortUnit = 0x0046A300;
void sortUnit(CUnit* unit) {
	__asm {
		PUSHAD
		MOV ESI, unit
		CALL Func_sortUnit
		POPAD
	}
}

;

const u32 Func_updateUnitOrderFinder = 0x0046A3A0;
void updateUnitOrderFinder(CUnit* unit) {
	__asm {
		PUSHAD
		MOV ESI, unit
		CALL Func_updateUnitOrderFinder
		POPAD
	}
}

;

const u32 Func_unit_IsStandardAndMovable = 0x0047B770;
bool unit_IsStandardAndMovable_Helper(CUnit * unit) {

	Bool32 return_value_unconverted;

	__asm {
		PUSHAD
		MOV ECX, unit
		CALL Func_unit_IsStandardAndMovable
		MOV return_value_unconverted, EAX
		POPAD
	}

	return (return_value_unconverted != 0);

}

;

const u32 Func_ApplyUnitEffects = 0x00492DA0;
void ApplyUnitEffects(CUnit* unit) {
	__asm {
		PUSHAD
		MOV EAX, unit
		CALL Func_ApplyUnitEffects
		POPAD
	}
}

;

const u32 Func_Sub49F6A0 = 0x0049F6A0;
void function_0049F6A0(CUnit* unit, u32 unitId) {
	__asm {
		PUSHAD
		MOV EDI, unit
		PUSH unitId
		CALL Func_Sub49F6A0
		POPAD
	}
}

;

//possibly recreate selection
const u32 Func_Sub49F7A0 = 0x0049F7A0;
void function_0049F7A0(CUnit* unit) {

	__asm {
		PUSHAD
		MOV EAX, unit
		CALL Func_Sub49F7A0
		POPAD
	}

}

;

//Manipulate selection, score and other things
const u32 Func_Sub49F860 = 0x0049F860;
void function_0049F860(CUnit* unit, u32 unitId) {
	__asm {
		PUSHAD
		MOV EAX, unit
		PUSH unitId
		CALL Func_Sub49F860
		POPAD
	}
}

;

//exclude unit from selection?
const u32 Func_Sub4C3B40 = 0x004C3B40;
void function_004C3B40(CUnit* unit) {

	__asm {
		PUSHAD
		MOV EDX, unit
		CALL Func_Sub4C3B40
		POPAD
	}

}

;

} //Unnamed namespace

//End of helper functions
#include "junkyarddog_order.h"
#include <SCBW/api.h>

//helper functions def
namespace {

bool unitCantSeeCloakedTarget(CUnit* unit, CUnit* target);		//01D60
void fixTargetLocation(Point16* coords, u32 unitId);			//01FA0
void setFlagHoldPosition(CUnit* unit);							//026A0
CUnit* findBestAttackTarget(CUnit* unit);						//43080
void function_004749A0(CUnit* unit, COrder* order, u8 orderId);	//749A0
u32 RandBetween(u32 min, u32 max, u32 someIndex);				//DC550
bool SetMoveTarget_xy(CUnit* unit, int x, int y);				//EB820

} //unnamed namespace

namespace hooks {

void orders_JunkYardDog(CUnit* unit) {

	bool bEndThere = false;
	CUnit* autoTargetUnit;
	
	if (unit->mainOrderState == 0) {

		u32 random_value_1, random_value_2;

		random_value_1 = RandBetween(0, 255, 48);
		random_value_2 = RandBetween(1024, 2048, 48);

		unit->orderTarget.pt.x = unit->sprite->position.x + (angleDistance[random_value_1].x * random_value_2) / 256;
		unit->orderTarget.pt.y = unit->sprite->position.y + (angleDistance[random_value_1].y * random_value_2) / 256;

		fixTargetLocation(&unit->orderTarget.pt, unit->id);
		SetMoveTarget_xy(unit, unit->orderTarget.pt.x, unit->orderTarget.pt.y);
		unit->mainOrderState = 1;
		unit->spellCooldown = 255;
		
	}

	autoTargetUnit = unit->autoTargetUnit;

	if (autoTargetUnit != NULL) {

		u8 autoTargetUnitPlayerId;

		if (autoTargetUnit->playerId != 11)
			autoTargetUnitPlayerId = autoTargetUnit->playerId;
		else
			autoTargetUnitPlayerId = autoTargetUnit->sprite->playerId;

		if (
			!scbw::isAlliedTo(unit->playerId, autoTargetUnitPlayerId) &&
			!(autoTargetUnit->sprite->flags & CSprite_Flags::Hidden) &&
			!((autoTargetUnit->status & UnitStatus::Invincible)) &&
			!unitCantSeeCloakedTarget(unit, autoTargetUnit)
		)
		{

			unit->orderTo(units_dat::AttackUnitOrder[unit->id], autoTargetUnit);
			function_004749A0(unit, NULL, OrderId::JunkYardDog);
			unit->autoTargetUnit = NULL;
			setFlagHoldPosition(unit);

			bEndThere = true;

		}

	}

	//7C317
	if (!bEndThere && unit->mainOrderTimer == 0) {

		unit->mainOrderTimer = 15;

		if (unit->moveTarget.pt != unit->sprite->position) {
			/*Ignored a bugged unit->status & UnitStatus::Unmovable check here*/

			if(unit->spellCooldown == 0 && !(unit->status & UnitStatus::InAir))
				unit->mainOrderState = 0;
			else
			{

				CUnit* newTarget = findBestAttackTarget(unit);

				if (newTarget != NULL) {
					unit->orderTo(units_dat::AttackUnitOrder[unit->id], newTarget);
					unit->performAnotherOrder(OrderId::JunkYardDog, 0, 0, NULL, UnitId::None, NULL);
					setFlagHoldPosition(unit);
				}


			}

		}
		else
			unit->mainOrderState = 0;

	}

}

} //hooks

;

//-------- Helper function definitions. Do NOT modify! --------//

namespace {

const u32 Func_unitCantSeeCloakedTarget = 0x00401D60;
bool unitCantSeeCloakedTarget(CUnit* unit, CUnit* target) {

	static Bool32 bPreResult;

	__asm {
		PUSHAD
		PUSH unit
		MOV EAX, target
		CALL Func_unitCantSeeCloakedTarget
		MOV bPreResult, EAX
		POPAD
	}

	return (bPreResult != 0);

}

;

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

//Identical to function @ 0x004026A0
void setFlagHoldPosition(CUnit* unit) {

	unit->status |= UnitStatus::HoldingPosition;

	if(unit->subunit != NULL)
		unit->subunit->status |= UnitStatus::HoldingPosition;

}

;

const u32 Func_Sub443080 = 0x00443080;
//named according to attack_priority_inject.cpp
CUnit* findBestAttackTarget(CUnit* unit) {

	static CUnit* result;

	__asm {
		PUSHAD
		MOV EAX, unit
		CALL Func_Sub443080
		MOV result, EAX
		POPAD
	}

	return result;

}

;

const u32 Func_Sub4749A0 = 0x004749A0;
void function_004749A0(CUnit* unit, COrder* order, u8 orderId) {

	__asm {
		PUSHAD
		MOV BL, orderId
		MOV ESI, unit
		MOV EDI, order
		CALL Func_Sub4749A0
		POPAD
	}

}

;

const u32 Func_RandBetween = 0x004DC550;
u32 RandBetween(u32 min, u32 max, u32 someIndex) {

	static u32 return_value;

	__asm {
		PUSHAD
		PUSH max
		MOV EAX, someIndex
		MOV EDX, min
		CALL Func_RandBetween
		MOV return_value, EAX
		POPAD
	}

	return return_value;

}

;

const u32 Func_SetMoveTarget_xy = 0x004EB820;
bool SetMoveTarget_xy(CUnit* unit, int x, int y) {

	static Bool32 bPreResult;

	__asm {
		PUSHAD
		MOV ESI, unit
		MOV EBX, x
		MOV EDI, y
		CALL Func_SetMoveTarget_xy
		MOV bPreResult, EAX
		POPAD
	}

	return (bPreResult != 0);

}

;

} //Unnamed namespace

//End of helper functions

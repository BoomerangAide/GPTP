#include "critter_order.h"
#include <SCBW/api.h>

//helper functions def
namespace {

void fixTargetLocation(Point16* coords, u32 unitId);	//01FA0
u32 RandBetween(u32 min, u32 max, u32 someIndex);		//DC550
bool SetMoveTarget_xy(CUnit* unit, int x, int y);		//EB820

} //unnamed namespace

namespace hooks {

void orders_Critter(CUnit* unit) {
	
	if (unit->mainOrderState == 0) {

		u32 random_value_1, random_value_2;

		random_value_1 = RandBetween(0, 255, 48);

		if (unit->id != UnitId::Critter_Kakaru)
			random_value_2 = 32;
		else
			random_value_2 = RandBetween(160, 640, 48);

		unit->orderTarget.pt.x = unit->sprite->position.x + (angleDistance[random_value_1].x * random_value_2) / 256;
		unit->orderTarget.pt.y = unit->sprite->position.y + (angleDistance[random_value_1].y * random_value_2) / 256;

		fixTargetLocation(&unit->orderTarget.pt, unit->id);
		SetMoveTarget_xy(unit, unit->orderTarget.pt.x, unit->orderTarget.pt.y);
		unit->mainOrderState = 1;
		
	}
	else
	if (unit->mainOrderState == 1) {
		
		if (unit->moveTarget.pt == unit->sprite->position) {
			/*Ignored a bugged unit->status & UnitStatus::Unmovable check here*/

			if (unit->id == UnitId::Critter_Kakaru)
				unit->mainOrderState = 0;
			else
			{

				u8 movableState = unit->getMovableState();

				if (movableState == MovableState::UnmovableAtDestination) {
					unit->mainOrderTimer = 75;
					unit->mainOrderState = movableState; //0 or 1
				}
				else
				{

					u32 random_value = RandBetween(0, 75, 47);

					if (random_value < 15)
						unit->mainOrderTimer = 0;
					else
						unit->mainOrderTimer = random_value;

					unit->mainOrderState = 2;

				}

			}

		}

	}
	else
	if (unit->mainOrderState == 2) {
		if (unit->mainOrderTimer == 0)
			unit->mainOrderState = 0;
	}

}

} //hooks

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

#include "burrow_orders.h"
#include <SCBW/api.h>

//helper functions def
namespace {

void unitSetRetreatPoint(CUnit* unit, u8 angleValue);					//011A0
bool getCloserToDestination_sub402BE0(CUnit* unit);						//02BE0
void function_0042D9A0(CUnit* unit);									//2D9A0
void _SetMoveTarget_xy(CUnit* unit);									//6BF00
void function_004749A0(CUnit* unit, COrder* order, u8 orderId);			//749A0
void actUnitReturnToIdle(CUnit* unit);									//75420
Bool32 function_00477160(CUnit* unit, u32 unk1, u32 unk2, u32 unk3);	//77160
void ApplyUnitEffects(CUnit* unit);										//92DA0
void refreshAllVisibleSprites(CSprite* sprite);							//976C0
void refreshCarryableSprite(CSprite* sprite);							//97750
void updateCarryableSpriteFlag(CSprite* sprite);						//981B0
void function_00498C30(CSprite* sprite, u32 direction);					//98C30
Bool32 AI_CastSpellBehaviour(CUnit* unit, u32 unknown);					//A13C0
void function_004E99D0(CUnit* unit);									//E99D0
void function_004E9A30(CUnit* unit);									//E9A30
void setNextWaypoint_Sub4EB290(CUnit* unit);							//EB290
bool SetMoveTarget_xy(CUnit* unit, int x, int y);						//EB820
void CloakProc(CUnit* unit);											//F46F0

} //unnamed namespace

namespace hooks {

void orders_Burrowed(CUnit* unit) {

	if (unit->id == UnitId::ZergLurker)
		actUnitReturnToIdle(unit);
	else
	if(
		unit->id == UnitId::ZergHydralisk &&
		unit->orderQueueHead != NULL &&
		unit->orderQueueHead->orderId == OrderId::Morph1
	)
	{
		unit->orderComputerCL(OrderId::Unburrow);
		function_004749A0(unit, unit->orderQueueHead, OrderId::Morph1);
		actUnitReturnToIdle(unit);
	}
	else
	if(unit->pAI != NULL) {

		if (unit->mainOrderTimer == 0) {

			unit->mainOrderTimer = 7;

			if (AI_CastSpellBehaviour(unit, 0))
				function_00477160(unit, 0, 1, 0);

		}

	}

}

;

void orders_Burrow(CUnit* unit) {
	
	if (unit->mainOrderState <= 3) {

		if (unit->mainOrderState == 0) {
			setNextWaypoint_Sub4EB290(unit);
			unit->mainOrderState = 1;
		}

		if (unit->mainOrderState == 1) {

			if (unit->movementFlags & MovementFlags::Accelerating) {
				if (unit->current_speed2 == 0)
					_SetMoveTarget_xy(unit);
			}
			else
			{

				u8 angle = 12;

				unit->sprite->playIscriptAnim(IscriptAnimation::WalkingToIdle, true);

				if (unit->id == UnitId::ZergDefiler || unit->id == UnitId::Hero_UncleanOne)
					angle = 11;
				else
				if (unit->id == UnitId::ZergDrone)
					angle = 13;

				unitSetRetreatPoint(unit, angle);
				unit->mainOrderState = 2;

			}

		}

		if (unit->mainOrderState == 2) {

			if (getCloserToDestination_sub402BE0(unit)) {

				unit->sprite->playIscriptAnim(IscriptAnimation::Burrow, true);
				scbw::playSound(SoundId::Misc_BurrowDn_wav, unit);
				unit->mainOrderState = 3;

			}

		}

		if (unit->mainOrderState == 3) {

			if (unit->orderSignal & 4) {

				unit->orderSignal -= 4;

				refreshAllVisibleSprites(unit->sprite);
				refreshCarryableSprite(unit->sprite);

				unit->status |= UnitStatus::NoCollide;

				function_004E9A30(unit);

				unit->setSecondaryOrder(OrderId::Cloak);
				unit->visibilityStatus = 0x80000000;
				CloakProc(unit);

				scbw::refreshConsole();

				unit->orderComputerCL(OrderId::Burrowed);
				actUnitReturnToIdle(unit);

			}

		}


	}

}

;

void orders_Unburrow(CUnit* unit){
	
	if (unit->mainOrderState <= 3) {

		if (unit->mainOrderState == 0) {

			unit->sprite->elevationLevel = units_dat::Elevation[unit->id];
			unit->status &= ~UnitStatus::NoCollide;			//disable NoCollide if enabled

			function_0042D9A0(unit);

			unit->sprite->flags &= ~CSprite_Flags::Burrowed;	//disable Burrowed if enabled

			unit->setSecondaryOrder(OrderId::Nothing2);

			unit->sprite->playIscriptAnim(IscriptAnimation::UnBurrow, true);
			scbw::playSound(SoundId::Misc_BurrowUp_wav, unit);

			unit->mainOrderState = 1;

		}
		else
		if (unit->mainOrderState < 3) 
		{

			if (unit->mainOrderState == 1) {
				unit->visibilityStatus = 0x80000000;
				unit->secondaryOrderTimer = 0;
				unit->mainOrderState = 2;
			}

			if (unit->mainOrderState == 2) {

				if (unit->orderSignal & 4) {

					unit->orderSignal -= 4;

					function_004E99D0(unit);
					ApplyUnitEffects(unit);

					updateCarryableSpriteFlag(unit->sprite);
					function_00498C30(unit->sprite, unit->currentDirection1);

					unit->mainOrderState = 3;

				}

			}

		}
		else //unit->mainOrderState == 3
		{

			if (unit->orderQueueHead == NULL)
				unit->orderSimple(units_dat::ReturnToIdleOrder[unit->id], 0);

			actUnitReturnToIdle(unit);
			scbw::refreshConsole();

		}

	}

}

;

} //hooks

;

//-------- Helper function definitions. Do NOT modify! --------//

namespace {

const u32 Func_UnitSetRetreatPoint = 0x004011A0;
//angleValue is a name guess
void unitSetRetreatPoint(CUnit* unit, u8 angleValue) {

	__asm {
		PUSHAD
		MOV AL, angleValue
		MOV EDX, unit
		CALL Func_UnitSetRetreatPoint
		POPAD
	}

}

;

const u32 Func_Sub402BE0_GetCloserToDestination = 0x00402BE0;
//return true when unit reached nextTargetWaypoint either at call
//or after some action inside the function, else return false
//At least that's what I think it does (not sure what I called
//"action inside the function" is, and how it's used to return
//true
bool getCloserToDestination_sub402BE0(CUnit* unit) {

	static Bool32 bResult;

	__asm {
		PUSHAD
		MOV ESI, unit
		CALL Func_Sub402BE0_GetCloserToDestination
		MOV bResult, EAX
		POPAD
	}

	return (bResult == 1);

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

const u32 Func__SetMoveTarget_xy = 0x0046BF00;
void _SetMoveTarget_xy(CUnit * unit) {
	__asm {
		PUSHAD
		MOV EAX, unit
		CALL Func__SetMoveTarget_xy
		POPAD
	}
}

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

const u32 Func_ActUnitReturnToIdle = 0x00475420;
void actUnitReturnToIdle(CUnit* unit) {

	__asm {
		PUSHAD
		MOV EAX, unit
		CALL Func_ActUnitReturnToIdle
		POPAD
	}

}

;

const u32 Func_Sub477160 = 0x00477160;
Bool32 function_00477160(CUnit* unit, u32 unk1, u32 unk2, u32 unk3) {

	__asm {
		PUSHAD
		PUSH unk1
		PUSH unk2
		PUSH unk3
		MOV ECX, unit
		CALL Func_Sub477160
		POPAD
	}

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

const u32 Func_refreshAllVisibleSprites = 0x004976C0;
void refreshAllVisibleSprites(CSprite* sprite) {
	__asm {
		PUSHAD
		MOV EAX, sprite
		CALL Func_refreshAllVisibleSprites
		POPAD
	}
}

;

const u32 Func_refreshCarryableSprite = 0x00497750;
void refreshCarryableSprite(CSprite* sprite) {
	__asm {
		PUSHAD
		MOV EAX, sprite
		CALL Func_refreshCarryableSprite
		POPAD
	}
}

;

const u32 Func_updateCarryableSpriteFlag = 0x004981B0;
void updateCarryableSpriteFlag(CSprite* sprite) {
	__asm {
		PUSHAD
		MOV EAX, sprite
		CALL Func_updateCarryableSpriteFlag
		POPAD
	}
}

;

const u32 Func_Sub498C30 = 0x00498C30;
void function_00498C30(CSprite* sprite, u32 direction) {
	__asm {
		PUSHAD
		MOV EAX, sprite
		MOV EDI, direction
		CALL Func_Sub498C30
		POPAD
	}
}

;

const u32 Func_AI_CastSpellBehaviour = 0x004A13C0;
Bool32 AI_CastSpellBehaviour(CUnit* unit, u32 unknown) {

	static Bool32 result;

	__asm {
		PUSHAD
		MOV EAX, unit
		PUSH unknown
		CALL Func_AI_CastSpellBehaviour
		MOV result, EAX
		POPAD
	}

	return result;

}

;

const u32 Func_Sub4E99D0 = 0x004E99D0;
void function_004E99D0(CUnit* unit) {
	__asm {
		PUSHAD
		MOV EAX, unit
		CALL Func_Sub4E99D0
		POPAD
	}
}

;

const u32 Func_Sub4E9A30 = 0x004E9A30;
void function_004E9A30(CUnit* unit) {
	__asm {
		PUSHAD
		MOV EAX, unit
		CALL Func_Sub4E9A30
		POPAD
	}
}

;

const u32 Func_sub_4EB290 = 0x004EB290;
//Related to path/movement decision
void setNextWaypoint_Sub4EB290(CUnit* unit) {
	__asm {
		PUSHAD
		MOV EAX, unit
		CALL Func_sub_4EB290
		POPAD
	}
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

const u32 Func_CloakProc = 0x004F46F0;
void CloakProc(CUnit * unit) {
	__asm {
		PUSHAD
		MOV ESI, unit
		CALL Func_CloakProc
		POPAD
	}
}

;

} //Unnamed namespace

//End of helper functions

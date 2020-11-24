#include "patrol_order.h"
#include <SCBW/api.h>

//helper functions def

namespace {

bool unitCantSeeCloakedTarget(CUnit* unit, CUnit* target);															//01D60
void setFlagHoldPosition(CUnit* unit);																				//026A0
CUnit* MedicHeal_TargetAcquire(CUnit* medic);																		//422A0
CUnit* findBestAttackTarget(CUnit* unit);																			//43080
void removeOrderFromUnitQueue(CUnit* unit, COrder* order);															//742D0
void unknownSpecialOrder(CUnit* unit, COrder* order, u8 orderId, s16 x, s16 y, CUnit* target, u16 targetUnitId);	//74540
void function_004747E0(CUnit* unit, COrder* order, u8 orderId, CUnit* target);										//747E0
void function_004748E0(CUnit* unit, int x, int y, u8 orderId, COrder* order);										//748E0
void actUnitReturnToIdle(CUnit* unit);																				//75420
bool SetMoveTarget_xy(CUnit* unit, int x, int y);																	//EB820

} //unnamed namespace

namespace hooks {

void orders_Patrol(CUnit* unit) {

	if (unit->mainOrderState == 0) {

		if (unit->orderQueueHead == NULL || unit->orderQueueHead->orderId != OrderId::Patrol) {

			unit->userActionFlags |= 1;

			if (unit->mainOrderId != OrderId::Die) {

				while (
					unit->orderQueueTail != NULL &&
					(
						(orders_dat::CanBeInterrupted[unit->orderQueueHead->orderId] != 0) || 
						(unit->orderQueueTail->orderId == OrderId::Patrol)
					)
				)
					removeOrderFromUnitQueue(unit, unit->orderQueueTail);

				unit->performAnotherOrder(
					OrderId::Patrol,
					unit->orderTarget.pt.x,
					unit->orderTarget.pt.y,
					unit->orderTarget.unit,
					UnitId::None,
					NULL
				);

			}

			prepareForNextOrder(unit);

			unit->performAnotherOrder(OrderId::Patrol, unit->sprite->position.x, unit->sprite->position.y, NULL, UnitId::None, NULL);

		}
		else
		{

			if (SetMoveTarget_xy(unit, unit->orderTarget.pt.x, unit->orderTarget.pt.y))
				unit->mainOrderState = 1;

			if (unit->nextTargetWaypoint != unit->orderTarget.pt) {
				unit->nextTargetWaypoint.x = unit->orderTarget.pt.x;
				unit->nextTargetWaypoint.y = unit->orderTarget.pt.y;
			}

		}

	}

	if (unit->mainOrderState != 0) {

		bool bNotUsingAutoTarget = false;
		CUnit* targetUnit = unit->autoTargetUnit;

		if (targetUnit != NULL) {

			u8 autoTargetPlayerId;

			if (targetUnit->playerId == 11)
				autoTargetPlayerId = targetUnit->sprite->playerId;
			else
				autoTargetPlayerId = targetUnit->playerId;

			if (
				playerAlliance[unit->playerId].flags[autoTargetPlayerId] != 0 ||
				targetUnit->sprite->flags & CSprite_Flags::Hidden ||
				targetUnit->status & UnitStatus::Invincible ||
				unitCantSeeCloakedTarget(unit,targetUnit)
			)
				bNotUsingAutoTarget = true;
			else
			{

				unknownSpecialOrder(
					unit, 
					unit->orderQueueHead, 
					OrderId::Patrol, 
					unit->orderTarget.pt.x, 
					unit->orderTarget.pt.y, 
					unit->orderTarget.unit, 
					UnitId::None
				);

				function_004747E0(unit, unit->orderQueueHead, units_dat::AttackUnitOrder[unit->id], unit->autoTargetUnit);

				unit->autoTargetUnit = NULL;

				actUnitReturnToIdle(unit);
				setFlagHoldPosition(unit);

			}

		}
		else
			bNotUsingAutoTarget = true;

		if (bNotUsingAutoTarget && unit->mainOrderTimer == 0) {

			unit->mainOrderTimer = 15;

			if (unit->id == UnitId::TerranMedic) {

				targetUnit = MedicHeal_TargetAcquire(unit);

				if (targetUnit != NULL && unit->orderTarget.unit == NULL) {

					unknownSpecialOrder(
						unit,
						unit->orderQueueHead,
						OrderId::Patrol,
						unit->orderTarget.pt.x,
						unit->orderTarget.pt.y,
						unit->orderTarget.unit,
						UnitId::None
					);
					function_004747E0(unit, unit->orderQueueHead, OrderId::MedicHeal1, targetUnit);

					unit->autoTargetUnit = NULL;

					actUnitReturnToIdle(unit);
					setFlagHoldPosition(unit);

					bNotUsingAutoTarget = false; //reusing the boolean instead of creating a new one

				}

			}

			if (bNotUsingAutoTarget) { //782AF

				if (unit->moveTarget.pt == unit->sprite->position) { /*Ignored a bugged unit->status & UnitStatus::Unmovable check here*/

					if (unit->getMovableState() == MovableState::UnmovableAtDestination)
						function_004748E0(unit, unit->orderTarget.pt.x, unit->orderTarget.pt.y, OrderId::Patrol, NULL);
					else
						function_004748E0(unit, unit->sprite->position.x, unit->sprite->position.y, OrderId::Patrol, NULL);

					actUnitReturnToIdle(unit);

				}
				else
				if(unit->getSeekRange() != 0)
				{

					targetUnit = findBestAttackTarget(unit);

					if (targetUnit != NULL) {

						unknownSpecialOrder(
							unit,
							unit->orderQueueHead,
							OrderId::Patrol,
							unit->orderTarget.pt.x,
							unit->orderTarget.pt.y,
							unit->orderTarget.unit,
							UnitId::None
						);
						function_004747E0(unit, unit->orderQueueHead, units_dat::AttackUnitOrder[unit->id], targetUnit);

						actUnitReturnToIdle(unit);
						setFlagHoldPosition(unit);

					}

				}

			}

		}
		

	}

}

;

} //namespace hooks

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

const u32 Func_MedicHeal_TargetAcquire = 0x004422A0;
CUnit* MedicHeal_TargetAcquire(CUnit* medic) {

	static CUnit* target;

	__asm {
		PUSHAD
		PUSH medic
		CALL Func_MedicHeal_TargetAcquire
		MOV target, EAX
		POPAD
	}

	return target;
	
}

;

const u32 Func_removeOrderFromUnitQueue = 0x004742D0;
void removeOrderFromUnitQueue(CUnit* unit, COrder* order) {

	__asm {
		PUSHAD
		MOV ECX, unit
		MOV EAX, order
		CALL Func_removeOrderFromUnitQueue
		POPAD
	}

}

;

const u32 Func_unknownSpecialOrder = 0x00474540;
void unknownSpecialOrder(CUnit* unit, COrder* order, u8 orderId, s16 x, s16 y, CUnit* target, u16 targetUnitId) {

	static Point16 pos;
	pos.x = x;pos.y = y;

	__asm {
		PUSHAD
		PUSH target
		PUSH pos
		MOV BL, orderId
		MOVZX EDX, targetUnitId
		MOV ESI, unit
		MOV EDI, order
		CALL Func_unknownSpecialOrder
		POPAD
	}

}

;

//Equivalent to function @ 0x004747E0
//Short version of unknownSpecialOrder call
void function_004747E0(CUnit* unit, COrder* order, u8 orderId, CUnit* target) {
	unknownSpecialOrder(unit, order, orderId, target->sprite->position.x, target->sprite->position.y, target, UnitId::None);
}

;

//Equivalent to function @ 0x004748E0
//Used in hooks\orders\base_orders\patrol_order.cpp, hooks\orders\medic_orders.cpp 
//and hooks\recv_commands\CMDRECV_QueuedOrder.cpp
void function_004748E0(CUnit* unit, int x, int y, u8 orderId, COrder* order) {
	unit->performAnotherOrder(orderId, x, y, NULL, UnitId::None, order);
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

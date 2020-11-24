#include "medic_orders.h"
#include <SCBW/api.h>

//helper functions def

namespace {

CUnit* MedicHeal_TargetAcquire(CUnit* medic);									//422A0
Bool32 AI_OrderHeal(CUnit* unit, CUnit* target);								//63530
u32 doMedicHeal(CUnit* unit, CUnit* target);									//63C40
void removeOrderFromUnitQueue(CUnit* unit, COrder* order);						//742D0
void function_004748E0(CUnit* unit, int x, int y, u8 orderId, COrder* order);	//748E0
void function_00474940(CUnit* unit, CUnit* target, u8 orderId, COrder* order);	//74940
void orderImmediate(CUnit* unit, u8 order);										//74B40
void orderInterrupt(CUnit* unit, CUnit* target, u16 x, u16 y, u32 orderId);		//74C30
void actUnitReturnToIdle(CUnit* unit);											//75420
void unitOrderMoveToTargetUnit(CUnit* unit, CUnit* target);						//79FE0
Bool32 AI_CastSpellBehaviour(CUnit* unit, u32 unknown);							//A13C0
void setNextWaypoint_Sub4EB290(CUnit* unit);									//EB290
bool moveToTarget(CUnit* unit, CUnit* target);									//EB720
bool SetMoveTarget_xy(CUnit* unit, int x, int y);								//EB820
bool function_004EB900(CUnit* unit, CUnit* target);								//EB900

} //unnamed namespace

namespace hooks {

void orders_MedicHeal2(CUnit* unit) {

	if(!(unit->movementFlags & MovementFlags::Accelerating)) {

		CImage* current_image = unit->sprite->images.head;

		while(current_image != NULL) {
			current_image->playIscriptAnim(IscriptAnimation::WalkingToIdle);
			current_image = current_image->link.next;
		}

	}

	actUnitReturnToIdle(unit);

}

;

void orders_HealMove(CUnit* unit) {

	bool bStopThere = false;
	CUnit* target = unit->orderTarget.unit;
	CUnit* oldTarget = unit->orderTarget.unit;

	if(target != NULL) {

		if(AI_OrderHeal(unit,target) != 0) {
			unit->orderComputerCL(OrderId::MedicHeal1);
			unit->orderTarget.unit = target;
			unit->orderTarget.pt.x = target->sprite->position.x;
			unit->orderTarget.pt.y = target->sprite->position.y;
			moveToTarget(unit,target);
			bStopThere = true;
		}
		else { //63801
			unit->orderTarget.pt.x = target->sprite->position.x;
			unit->orderTarget.pt.y = target->sprite->position.y;
		}

	}

	if(!bStopThere) { //63817

		if(unit->recentOrderTimer == 0) {
			if(!SetMoveTarget_xy(unit,unit->orderTarget.pt.x,unit->orderTarget.pt.y))
				bStopThere = true;
		}

		//63842
		if(!bStopThere) {

			if(	unit->moveTarget.pt.x == unit->sprite->position.x &&
				unit->moveTarget.pt.y == unit->sprite->position.y
			)
				unit->orderToIdle();
			else
			if(unit->mainOrderTimer == 0) {

				CUnit* newTarget;
				Point16 oldPos;

				unit->mainOrderTimer = 15;

				newTarget = MedicHeal_TargetAcquire(unit);

				if(newTarget != NULL) {

					oldPos.x = unit->orderTarget.pt.x;
					oldPos.y = unit->orderTarget.pt.y;

					orderInterrupt(
						unit,
						unit->orderTarget.unit,
						unit->orderTarget.pt.x,
						unit->orderTarget.pt.y,
						OrderId::MedicHeal1
					);

					moveToTarget(unit,newTarget);
					unitOrderMoveToTargetUnit(unit,newTarget);

					if(oldTarget != NULL)
						function_00474940(unit,oldTarget,OrderId::HealMove, NULL);
					else
						function_004748E0(unit,oldPos.x,oldPos.y,OrderId::HealMove, NULL);
		
				}

			}


		}

	}

}

;

// Idle/AI order probably
void orders_Medic(CUnit* unit) {

	bool bStopThere = false;

	if(unit->pAI != NULL && AI_CastSpellBehaviour(unit,NULL) != 0)
		bStopThere = true;

	if(!bStopThere) {

		CUnit* target = MedicHeal_TargetAcquire(unit);

		if(target != NULL) {

			unit->userActionFlags |= 1;

			if(unit->mainOrderId != OrderId::Die) {

				bool bEndLoop = unit->orderQueueTail == NULL;

				while(!bEndLoop) {

					if(	!orders_dat::CanBeInterrupted[unit->orderQueueTail->orderId] &&
						unit->orderQueueTail->orderId != OrderId::MedicHeal1)
						bEndLoop = true;
					else {
						removeOrderFromUnitQueue(unit,unit->orderQueueTail);
						bEndLoop = (unit->orderQueueTail == NULL);
					}

				}

				unit->performAnotherOrder(
					OrderId::MedicHeal1,
					0,
					0,
					NULL,
					UnitId::None
				);

			}

			//63991
			prepareForNextOrder(unit);

			unit->orderTarget.unit = target;
			unit->orderTarget.pt.x = target->sprite->position.x;
			unit->orderTarget.pt.y = target->sprite->position.y;

			moveToTarget(unit,target);

		}
		else { //639BD

			if(unit->pAI != NULL) {

				if(*(u8*)((u32)unit->pAI + 8) != 1)
					unit->orderComputerCL(OrderId::ComputerAI);
				else
				if(	unit->sprite->position.x != *(u16*)((u32)unit->pAI + 0x12) ||
					unit->sprite->position.y != *(u16*)((u32)unit->pAI + 0x14)
				)
					unit->orderTo(
						OrderId::ComputerReturn,
						*(u16*)((u32)unit->pAI + 0x12),
						*(u16*)((u32)unit->pAI + 0x14)
					);

			}

		}

	}

}

;

//void orders_MedicHoldPosition(CUnit* unit)
//in base_orders/stopholdpos_orders.cpp

;

void orders_MedicHeal1(CUnit* unit) {

	CUnit* target = unit->orderTarget.unit;
	u32 doMedicHeal_result;

	doMedicHeal_result = doMedicHeal(unit,target);

	if (doMedicHeal_result == 0) {

		setNextWaypoint_Sub4EB290(unit);

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
	else { //641D3

		if(unit->mainOrderState == 0) {

			if(	target != NULL &&
				function_004EB900(unit,target) != 0 &&
				unit->getMovableState() != MovableState::UnmovableAtDestination
			)
			{
				if(unit->isTargetWithinMinRange(target,30)) {
					setNextWaypoint_Sub4EB290(unit);
					unit->mainOrderState = 1;
				}
			}
			else //64216
				unit->orderToIdle();

		}
		else { //6421E

			if(doMedicHeal_result == 1) {

				if(!(unit->orderSignal & 1)) {

					CImage* current_image = unit->sprite->images.head;

					while(current_image != NULL) {
						current_image->playIscriptAnim(IscriptAnimation::SpecialState1);
						current_image = current_image->link.next;
					}

					if(	unit->orderQueueHead == NULL ||
						unit->orderQueueHead->orderId != OrderId::MedicHeal2
					)
						orderImmediate(unit,OrderId::MedicHeal2);

				}

				//6425B
				target = unit->orderTarget.unit;

				if(	target->position.x != unit->nextTargetWaypoint.x ||
					target->position.y != unit->nextTargetWaypoint.y
				)
				{
					unit->nextTargetWaypoint.x = target->position.x;
					unit->nextTargetWaypoint.y = target->position.y;
				}

			}
			else //64283
				unit->mainOrderState = 0;

		}

	}

}

;

} //namespace hooks

;

//-------- Helper function definitions. Do NOT modify! --------//

namespace {

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

const u32 Func_AI_OrderHeal = 0x00463530;
Bool32 AI_OrderHeal(CUnit* unit, CUnit* target) {

	static Bool32 result;

	__asm {
		PUSHAD
		MOV ESI, target
		MOV EBX, unit
		CALL Func_AI_OrderHeal
		MOV result, EAX
		POPAD
	}

	return result;

}

;

const u32 Func_doMedicHeal = 0x00463C40;
u32 doMedicHeal(CUnit* unit, CUnit* target) {

	static u32 return_value;

	__asm {
		PUSHAD
		MOV EAX, unit
		PUSH target
		CALL Func_doMedicHeal
		MOV return_value, EAX
		POPAD
	}

	return return_value;

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

//Used in hooks\orders\base_orders\patrol_order.cpp, hooks\orders\medic_orders.cpp 
//and hooks\recv_commands\CMDRECV_QueuedOrder.cpp
void function_004748E0(CUnit* unit, int x, int y, u8 orderId, COrder* order) {
	unit->performAnotherOrder(orderId, x, y, NULL, UnitId::None, order);
}

;

//Equivalent to function @ 0x00474940, also used in hooks\utils\utils.cpp
void function_00474940(CUnit* unit, CUnit* target, u8 orderId, COrder* order) {
	if (target == NULL)
		unit->performAnotherOrder(orderId, 0, 0, NULL, UnitId::None, order);
	else
		unit->performAnotherOrder(orderId, target->sprite->position.x, target->sprite->position.y, target, UnitId::None, order);
}

;

const u32 Func_OrderImmediate = 0x00474B40;
void orderImmediate(CUnit* unit, u8 order) {

	__asm {
		PUSHAD
		MOV ECX, unit
		MOV AL, order
		CALL Func_OrderImmediate
		POPAD
	}

}

;

const u32 Func_orderInterrupt = 0x00474C30;
void orderInterrupt(CUnit* unit, CUnit* target, u16 x, u16 y, u32 orderId) {

	static Point16 pos;

	pos.x = x;pos.y = y;

	__asm {
		PUSHAD
		MOV ESI, unit
		PUSH target
		PUSH pos
		PUSH orderId
		CALL Func_orderInterrupt
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

const u32 Func_unitOrderMoveToTargetUnit = 0x00479FE0;
void unitOrderMoveToTargetUnit(CUnit* unit, CUnit* target) {

	__asm {
		PUSHAD
		MOV EAX, unit
		MOV ECX, target
		CALL Func_unitOrderMoveToTargetUnit
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

//Related to path/movement decision
const u32 Func_sub_4EB290 = 0x004EB290;
void setNextWaypoint_Sub4EB290(CUnit* unit) {

	__asm {
		PUSHAD
		MOV EAX, unit
		CALL Func_sub_4EB290
		POPAD
	}
}

;

const u32 Func_moveToTarget = 0x004EB720;
bool moveToTarget(CUnit* unit, CUnit* target) {

	static Bool32 bPreResult;

	__asm {
		PUSHAD
		MOV ESI, unit
		MOV EDI, target
		CALL Func_moveToTarget
		MOV bPreResult, EAX
		POPAD
	}

	return (bPreResult != 0);

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

const u32 Func_Sub4EB900 = 0x004EB900;
bool function_004EB900(CUnit* unit, CUnit* target) {

	static Bool32 bPreResult;

	__asm {
		PUSHAD
		MOV EAX, target
		MOV ECX, unit
		CALL Func_Sub4EB900
		MOV bPreResult, EAX
		POPAD
	}

	return (bPreResult != 0);

}

;

} //Unnamed namespace

//End of helper functions

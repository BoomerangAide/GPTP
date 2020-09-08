#include "stopholdpos_orders.h"
#include <SCBW/api.h>

//helper functions def

namespace {

CUnit* MedicHeal_TargetAcquire(CUnit* medic);							//422A0
CUnit* findBestAttackTarget(CUnit* unit);								//43080
u32 doMedicHeal(CUnit* unit, CUnit* target);							//63C40
void function_004655F0(CUnit* unit);									//655F0
void removeOrderFromUnitQueue(CUnit* unit, COrder* order);				//742D0
void orderImmediate(CUnit* unit, u8 order);								//74B40
void function_00476FC0(CUnit* unit, CUnit* target, u32 unk1, u32 unk2);	//76FC0
Bool32 function_004770E0(CUnit* unit);									//770E0
void orders_PlayerGuard_Helper(CUnit* unit);							//774A0
u32 RandBetween(u32 min, u32 max, u32 someIndex);						//DC550
void setNextWaypoint_Sub4EB290(CUnit* unit);							//EB290

} //unnamed namespace

namespace hooks {

void orders_MedicHoldPosition(CUnit* unit) {

	u32 switchValue;

	if(unit->mainOrderState == 0) {

		//change move target destination to current position
		setNextWaypoint_Sub4EB290(unit);

		if(
			unit->nextTargetWaypoint.x != unit->moveTarget.pt.x ||
			unit->nextTargetWaypoint.y != unit->moveTarget.pt.y
		)
		{
			unit->nextTargetWaypoint.x = unit->moveTarget.pt.x;
			unit->nextTargetWaypoint.y = unit->moveTarget.pt.y;
		}

		//6407F:
		unit->status |= UnitStatus::HoldingPosition;

		if(unit->subunit != NULL)
			unit->subunit->status |= UnitStatus::HoldingPosition;

		unit->mainOrderState = 1;
		unit->sprite->playIscriptAnim(IscriptAnimation::WalkingToIdle,true);

	}

	//640AF

	//try healing current target, return a value depending
	//on what happened
	switchValue = doMedicHeal(unit,unit->orderTarget.unit);

	if(switchValue == 3) //640C6
		unit->orderTarget.unit = NULL;

	if(switchValue == 0 || switchValue == 2 || switchValue == 3) //640CD
		unit->sprite->playIscriptAnim(IscriptAnimation::WalkingToIdle,true);

	//640D9, continued into for values 0,2,3, and jumped into for values
	//above 3, thus only switchValue of 1 don't go through it
	if(switchValue != 1) {

		//if timer is 0, seek a target in range
		if(unit->mainOrderTimer == 0) {

			unit->mainOrderTimer = 15;
			unit->orderTarget.unit = MedicHeal_TargetAcquire(unit);

			if(unit->orderTarget.unit != NULL) {
				unit->orderTarget.pt.x = (unit->orderTarget.unit)->sprite->position.x;
				unit->orderTarget.pt.y = (unit->orderTarget.unit)->sprite->position.y;
			}
				
		}

	}
	else { //64109 (switchValue == 1)

		if(!(unit->orderSignal & 1)) {

			CImage* current_image = unit->sprite->images.head;

			while(current_image != NULL) {
				current_image->playIscriptAnim(IscriptAnimation::SpecialState1);
				current_image = current_image->link.next;
			}

			if(
				unit->orderQueueHead == NULL ||
				unit->orderQueueHead->orderId != OrderId::MedicHeal2
			) 	//6413D
				orderImmediate(unit,OrderId::MedicHeal2); //return to idle after heal

		}

		//64146
		if(
			(unit->orderTarget.unit)->sprite->position.x != unit->nextTargetWaypoint.x ||
			(unit->orderTarget.unit)->sprite->position.y != unit->nextTargetWaypoint.y
		)
		{
			unit->nextTargetWaypoint.x = (unit->orderTarget.unit)->sprite->position.x;
			unit->nextTargetWaypoint.y = (unit->orderTarget.unit)->sprite->position.y;
		}

	}

} //void orders_MedicHoldPosition(CUnit* unit)

;

void orders_ReaverStop(CUnit* unit) {

	CUnit* outHangarChild = unit->carrier.outHangarChild;
	Point16 pos;

	//change move target destination to current position
	setNextWaypoint_Sub4EB290(unit);

	if(
		unit->moveTarget.pt.x != unit->nextTargetWaypoint.x ||
		unit->moveTarget.pt.y != unit->nextTargetWaypoint.y
	) 
	{
		unit->nextTargetWaypoint.x = unit->moveTarget.pt.x;
		unit->nextTargetWaypoint.y = unit->moveTarget.pt.y;
	}

	//65500
	while(outHangarChild != NULL) {

		outHangarChild->userActionFlags |= 1;

		if(outHangarChild->mainOrderId != OrderId::Die) {

			bool bNotRemovableOrder = false;

			while(outHangarChild->orderQueueTail != NULL && !bNotRemovableOrder) {

				if(
					!orders_dat::CanBeInterrupted[outHangarChild->orderQueueTail->orderId] &&
					outHangarChild->orderQueueTail->orderId != OrderId::SelfDestrucing
				) 
					bNotRemovableOrder = true;
				else
					removeOrderFromUnitQueue(outHangarChild, outHangarChild->orderQueueTail);

			}

			//65539
			outHangarChild->performAnotherOrder(
				OrderId::SelfDestrucing,
				0,
				0,
				NULL,
				UnitId::None
			);

		}

		//65554
		prepareForNextOrder(outHangarChild);
		outHangarChild = outHangarChild->interceptor.hangar_link.prev;

	} //while(outHangarChild != NULL)

	//65565

	unit->userActionFlags |= 1;

	if(unit->orderTarget.unit != NULL) {
		pos.x = (unit->orderTarget.unit)->sprite->position.x;
		pos.y = (unit->orderTarget.unit)->sprite->position.y;
	}
	else {
		pos.x = 0;
		pos.y = 0;
	}

	//65598
	if(unit->mainOrderId != OrderId::Die) {

		bool bNotRemovableOrder = false;

		while(unit->orderQueueTail != NULL && !bNotRemovableOrder) {

			if(
				!orders_dat::CanBeInterrupted[unit->orderQueueTail->orderId] &&
				unit->orderQueueTail->orderId != OrderId::Reaver
			)
				bNotRemovableOrder = true;
			else
				removeOrderFromUnitQueue(unit, unit->orderQueueTail);

		}

		//655C3
		unit->performAnotherOrder(
			OrderId::Reaver,
			pos.x,
			pos.y,
			unit->orderTarget.unit,
			UnitId::None
		);

	}

	//655DB
	prepareForNextOrder(unit);

} //void orders_ReaverStop(CUnit* unit)

;

//Initially unidentified function 00465910
void orders_CarrierStop(CUnit* unit) {

	//change move target destination to current position
	setNextWaypoint_Sub4EB290(unit);

	if(
		unit->moveTarget.pt.x != unit->nextTargetWaypoint.x ||
		unit->moveTarget.pt.y != unit->nextTargetWaypoint.y
	) 
	{
		unit->nextTargetWaypoint.x = unit->moveTarget.pt.x;
		unit->nextTargetWaypoint.y = unit->moveTarget.pt.y;
	}

	//stop and order to return to the interceptors
	function_004655F0(unit);

	unit->orderTo(OrderId::Carrier, unit->orderTarget.unit);

}

;

void orders_Guard(CUnit* unit) {

	unit->mainOrderTimer = (u8)RandBetween(0,15,29);

	if(playerTable[unit->playerId].type == PlayerType::Computer) {

		//since details about pAI structure are unknown, have to use this
		u8* const pAI_subparameter = (u8*)( ( (int)(unit->pAI) ) + 8 );

		unit->mainOrderId = OrderId::ComputerAI;

		if(unit->pAI == NULL) {

			if(units_dat::BaseProperty[unit->id] & UnitProperty::Hero)
				unit->mainOrderId = OrderId::PlayerGuard;

		}
		else //75BD6
		if( *pAI_subparameter == 1 )
			unit->mainOrderId = OrderId::GuardPost;

	}
	else
		unit->mainOrderId = OrderId::PlayerGuard;

} //void orders_Guard(CUnit* unit)

;

//NOT HOOKED (since most of the action happen in another function)
//Just here to show that the true code is in function_004774A0, though
//a few things happen before
void orders_TurretGuard(CUnit* unit) {

	if(
		unit->subunit->nextTargetWaypoint.x != unit->nextTargetWaypoint.x ||
		unit->subunit->nextTargetWaypoint.y != unit->nextTargetWaypoint.y
	) 
	{
		unit->nextTargetWaypoint.x = unit->subunit->nextTargetWaypoint.x;
		unit->nextTargetWaypoint.y = unit->subunit->nextTargetWaypoint.y;
	}

	orders_PlayerGuard_Helper(unit);

} //void orders_TurretGuard(CUnit* unit)

;

//Initially unidentified function 004774A0
void orders_PlayerGuard(CUnit* unit) {

	//if unit->autoTargetUnit != NULL, attack this target
	//then disable the unit...I think
	//That's why if it return true, nothing else is done
	//since has already attacked
	if(!function_004770E0(unit)) {

		if(unit->mainOrderTimer == 0) {

			unit->mainOrderTimer = 15;

			if(units_dat::BaseProperty[unit->id] & UnitProperty::Subunit) {
				if(
					unit->subunit->nextTargetWaypoint.x != unit->nextTargetWaypoint.x ||
					unit->subunit->nextTargetWaypoint.y != unit->nextTargetWaypoint.y
				) 
				{
					unit->nextTargetWaypoint.x = unit->subunit->nextTargetWaypoint.x;
					unit->nextTargetWaypoint.y = unit->subunit->nextTargetWaypoint.y;
				}
			}

			if(unit->getSeekRange() != 0) {

				CUnit* target = findBestAttackTarget(unit);

				if(target != NULL)
					//attack target I think, also used by function_004770E0
					function_00476FC0(unit, target, 0,1); 

			}

		}

	}

} //void function_004774A0(CUnit* unit)

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

//Equivalent to function @ 004655F0
//Note: function also used by paralysing spells like
//Maelstrom and Lockdown, take it into account if
//using a different code in this use case.
void function_004655F0(CUnit* unit) {

	CUnit* outHangarChild = unit->carrier.outHangarChild;

	while(outHangarChild != NULL) {

		outHangarChild->userActionFlags |= 1;

		if(outHangarChild->mainOrderId != OrderId::Die) {

			bool bNotRemovableOrder = false;

			while(outHangarChild->orderQueueTail != NULL && !bNotRemovableOrder) {

				if(
					!orders_dat::CanBeInterrupted[outHangarChild->orderQueueTail->orderId] &&
					outHangarChild->orderQueueTail->orderId != OrderId::Return
				) 
					bNotRemovableOrder = true;
				else
					removeOrderFromUnitQueue(outHangarChild, outHangarChild->orderQueueTail);

			}

			//6565B
			outHangarChild->performAnotherOrder(
				OrderId::Return,
				0,
				0,
				NULL,
				UnitId::None
			);

		}

		//65671
		prepareForNextOrder(outHangarChild);
		outHangarChild = outHangarChild->interceptor.hangar_link.prev;

	}

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

const u32 Func_Sub476FC0 = 0x00476FC0;
void function_00476FC0(CUnit* unit, CUnit* target, u32 unk1, u32 unk2) {

	__asm {
		PUSHAD
		PUSH unk1
		PUSH unk2
		MOV EAX, target
		MOV EDI, unit
		CALL Func_Sub476FC0
		POPAD
	}

}

;

const u32 Func_Sub4770E0 = 0x004770E0;
Bool32 function_004770E0(CUnit* unit) {

	static Bool32 return_value;

	__asm {
		PUSHAD
		MOV EAX, unit
		CALL Func_Sub4770E0
		MOV return_value, EAX
		POPAD
	}

	return (return_value != 0);

}

;

const u32 Func_Sub4774A0 = 0x004774A0;
void orders_PlayerGuard_Helper(CUnit* unit) {
	__asm {
		PUSHAD
		MOV EAX, unit
		CALL Func_Sub4774A0
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

} //Unnamed namespace

//End of helper functions

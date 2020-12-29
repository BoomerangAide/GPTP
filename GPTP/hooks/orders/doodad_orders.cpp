#include "doodad_orders.h"
#include <SCBW/api.h>

//helper functions def
namespace {

void function_0042D9A0(CUnit* unit);						//2D9A0
CUnit* findRandomAttackTarget(CUnit* unit);					//42FC0
void removeOrderFromUnitQueue(CUnit* unit, COrder* order);	//742D0
void orderImmediate(CUnit* unit, u8 order);					//74B40
void actUnitReturnToIdle(CUnit* unit);						//75420
bool unitIsFloorTrap(CUnit* unit);							//7B070

} //unnamed namespace

namespace hooks {

void orders_CloseDoor(CUnit* unit) {

	if (unit->orderSignal & 1) {

		unit->orderSignal -= 1;
		unit->status &= ~UnitStatus::NoCollide;	//disable if was set
		unit->sprite->elevationLevel = units_dat::Elevation[unit->id];
		unit->userActionFlags |= 1;

		unit->order(units_dat::ReturnToIdleOrder[unit->id], 0, 0, NULL, UnitId::None, true);
		prepareForNextOrder(unit);

		actUnitReturnToIdle(unit);
		function_0042D9A0(unit);

	}

}

;

void orders_OpenDoor(CUnit* unit) {

	if (unit->orderSignal & 1) {

		u32* const u32_006D5BD0 = (u32*)0x006D5BD0;

		unit->orderSignal -= 1;
		unit->status |= UnitStatus::NoCollide;
		unit->sprite->elevationLevel = 1;
		unit->userActionFlags |= 1;

		unit->order(units_dat::ReturnToIdleOrder[unit->id], 0, 0, NULL, UnitId::None, true);
		prepareForNextOrder(unit);

		unit->status |= UnitStatus::DoodadStatesThing;
		actUnitReturnToIdle(unit);

		*u32_006D5BD0 = 1;

	}

}

;

void orders_Disabledoodad(CUnit* unit) {

	if (
		!(unit->status & UnitStatus::DoodadStatesThing) &&
		unit->lockdownTimer == 0 &&
		unit->stasisTimer == 0 &&
		unit->maelstromTimer == 0
	)
	{

		if (!(unit->status & UnitStatus::NoBrkCodeStart)) {

			unit->sprite->playIscriptAnim(IscriptAnimation::Disable, true);

			if (
				unit->id == UnitId::Special_FloorMissileTrap ||
				unit->id == UnitId::Special_FloorGunTrap ||
				unit->id == UnitId::Special_UpperLevelDoor ||
				unit->id == UnitId::Special_RightUpperLevelDoor ||
				unit->id == UnitId::Special_PitDoor ||
				unit->id == UnitId::Special_RightPitDoor
			)
				unit->sprite->elevationLevel = 1;

			if (
				unit->id == UnitId::Special_UpperLevelDoor ||
				unit->id == UnitId::Special_RightUpperLevelDoor ||
				unit->id == UnitId::Special_PitDoor ||
				unit->id == UnitId::Special_RightPitDoor
			)
			{
				scbw::playSound(units_dat::SoundWhatFirst[unit->id], unit);
				orderImmediate(unit, OrderId::OpenDoor);
				actUnitReturnToIdle(unit);
			}
			else
			{

				unit->visibilityStatus = 0x80000000;
				unit->secondaryOrderTimer = 0;
				unit->status |= (UnitStatus::DoodadStatesThing + UnitStatus::Cloaked + UnitStatus::RequiresDetection);

				if (unit->orderQueueHead == NULL)
					unit->orderComputerCL(units_dat::ReturnToIdleOrder[unit->id]);

				actUnitReturnToIdle(unit);

			}

		}

	}
	else
	{

		if (unit->orderQueueHead == NULL)
			unit->orderComputerCL(units_dat::ReturnToIdleOrder[unit->id]);

		actUnitReturnToIdle(unit);

	}

}

;

void orders_Enabledoodad(CUnit* unit) {

	if (
		!(unit->status & UnitStatus::DoodadStatesThing) &&
		unit->lockdownTimer == 0 &&
		unit->stasisTimer == 0 &&
		unit->maelstromTimer == 0
	)
	{

		if (!(unit->status & UnitStatus::NoBrkCodeStart)) {

			unit->sprite->playIscriptAnim(IscriptAnimation::Enable, true);

			if (
				unit->id == UnitId::Special_FloorMissileTrap ||
				unit->id == UnitId::Special_FloorGunTrap ||
				unit->id == UnitId::Special_UpperLevelDoor ||
				unit->id == UnitId::Special_RightUpperLevelDoor ||
				unit->id == UnitId::Special_PitDoor ||
				unit->id == UnitId::Special_RightPitDoor
			)
				unit->sprite->elevationLevel = units_dat::Elevation[unit->id];

			if (
				unit->id == UnitId::Special_UpperLevelDoor ||
				unit->id == UnitId::Special_RightUpperLevelDoor ||
				unit->id == UnitId::Special_PitDoor ||
				unit->id == UnitId::Special_RightPitDoor
			)
			{
				scbw::playSound(units_dat::SoundWhatLast[unit->id], unit);
				orderImmediate(unit, OrderId::CloseDoor);
				actUnitReturnToIdle(unit);
			}
			else
			{

				if (unit->orderQueueHead == NULL)
					unit->orderComputerCL(units_dat::ReturnToIdleOrder[unit->id]);

				actUnitReturnToIdle(unit);

			}

		}

	}
	else
	{

		if (unit->orderQueueHead == NULL)
			unit->orderComputerCL(units_dat::ReturnToIdleOrder[unit->id]);

		actUnitReturnToIdle(unit);

	}

}

;

void orders_HiddenGun(CUnit* unit) {

	if (unit->mainOrderState == 0) {
		
		unit->status |= UnitStatus::NoCollide;

		if (unit->id == UnitId::Special_FloorMissileTrap || unit->id == UnitId::Special_FloorGunTrap)
			unit->sprite->elevationLevel = 0;

		unit->status |= (UnitStatus::Cloaked + UnitStatus::RequiresDetection);
		unit->visibilityStatus = 0x80000000;
		unit->secondaryOrderTimer = 0;
		unit->mainOrderState = 1;

	}
	else
	if (unit->mainOrderState == 1) {

		if (
			!(unit->status & UnitStatus::DoodadStatesThing) &&
			unit->lockdownTimer == 0 &&
			unit->stasisTimer == 0 &&
			unit->maelstromTimer == 0 &&
			unit->mainOrderTimer == 0
		)
		{

			CUnit* attackTarget;

			unit->mainOrderTimer = 15;
			attackTarget = findRandomAttackTarget(unit);

			if (attackTarget != NULL) {

				unit->status &= ~(UnitStatus::NoCollide); //remove status if was present

				if (unitIsFloorTrap(unit))
					unit->sprite->elevationLevel = units_dat::Elevation[unit->id];

				unit->status &= ~(UnitStatus::Cloaked + UnitStatus::RequiresDetection);	//remove those status if were present
				unit->secondaryOrderTimer = 0;

				unit->orderComputerCL(OrderId::RevealTrap);
				unit->issueQueuedOrderTarget(units_dat::AttackUnitOrder[unit->id], attackTarget, false);
				unit->orderSimple(OrderId::HideTrap, false);

			}

		}

	}

}

;

void orders_HideTrap(CUnit* unit) {

	if (unit->mainOrderState == 0) {

		CUnit* attackTarget;
		
		if (
			!(unit->status & UnitStatus::DoodadStatesThing) &&
			unit->lockdownTimer == 0 &&
			unit->stasisTimer == 0 &&
			unit->maelstromTimer == 0 &&
			unit->mainOrderTimer == 0 &&
			(attackTarget = findRandomAttackTarget(unit)) != NULL
		)
		{

			unit->orderTo(units_dat::AttackUnitOrder[unit->id], attackTarget);

			if (unit->mainOrderId != OrderId::Die) {

				while (
					unit->orderQueueTail != NULL &&
					unit->orderQueueTail->orderId == OrderId::HideTrap
				)
					removeOrderFromUnitQueue(unit,unit->orderQueueTail);

				unit->performAnotherOrder(OrderId::HideTrap, 0, 0, NULL, UnitId::None, NULL);

			}

			actUnitReturnToIdle(unit);

		}
		else
		{
			
			CImage* current_image = unit->sprite->images.head;

			while (current_image != NULL) {
				current_image->playIscriptAnim(IscriptAnimation::SpecialState2);
				current_image = current_image->link.next;
			}

			unit->orderSignal &= ~1; //remove 1 if present as flag
			unit->mainOrderState = 1;

		}

	}
	else
	if (unit->mainOrderState == 1) {

		if (unit->orderSignal & 1) {

			unit->orderSignal -= 1;

			if (unit->orderQueueHead == NULL)
				unit->orderComputerCL(units_dat::ReturnToIdleOrder[unit->id]);

			actUnitReturnToIdle(unit);

		}

	}

}

;

void orders_RevealTrap(CUnit* unit) {

	if (unit->mainOrderState == 0) {
			
		CImage* current_image = unit->sprite->images.head;

		while (current_image != NULL) {
			current_image->playIscriptAnim(IscriptAnimation::SpecialState1);
			current_image = current_image->link.next;
		}

		unit->orderSignal &= ~1; //remove 1 if present as flag
		unit->mainOrderState = 1;

	}
	else
	if (unit->mainOrderState == 1) {

		if (unit->orderSignal & 1) {

			unit->orderSignal -= 1;

			if (unit->orderQueueHead == NULL)
				unit->orderComputerCL(units_dat::ReturnToIdleOrder[unit->id]);

			actUnitReturnToIdle(unit);

		}

	}

}

;

} //hooks

;

//-------- Helper function definitions. Do NOT modify! --------//

namespace {

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

const u32 Func_findRandomAttackTarget = 0x00442FC0;
CUnit* findRandomAttackTarget(CUnit* unit) {

	static CUnit* target;

	__asm {
		PUSHAD
		MOV ESI, unit
		CALL Func_findRandomAttackTarget
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

//Logically identical to function at 0x0047B070
bool unitIsFloorTrap(CUnit* unit) {
	return (unit->id == UnitId::Special_FloorMissileTrap || unit->id == UnitId::Special_FloorGunTrap);
}

;

} //Unnamed namespace

//End of helper functions

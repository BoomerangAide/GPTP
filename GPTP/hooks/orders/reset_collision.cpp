#include "reset_collision.h"
#include <SCBW/api.h>

//helper functions def

namespace {

void function_0042D9A0(CUnit* unit);						//2D9A0
void removeOrderFromUnitQueue(CUnit* unit, COrder* order);	//742D0
void actUnitReturnToIdle(CUnit* unit);						//75420
void toggleUnitPath(CUnit* unit);							//E42A0

} //unnamed namespace

namespace hooks {

void orders_ResetCollision2(CUnit* unit) {

	if (
		unit->orderQueueHead != NULL &&
		(
			unit->orderQueueHead->orderId == OrderId::Harvest1 ||
			unit->orderQueueHead->orderId == OrderId::HarvestGas1 ||
			unit->orderQueueHead->orderId == OrderId::HarvestGas2 ||
			unit->orderQueueHead->orderId == OrderId::HarvestGas3 ||
			unit->orderQueueHead->orderId == OrderId::MoveToMinerals ||
			unit->orderQueueHead->orderId == OrderId::HarvestMinerals2 ||
			unit->orderQueueHead->orderId == OrderId::MiningMinerals ||
			unit->orderQueueHead->orderId == OrderId::Harvest4 ||
			unit->orderQueueHead->orderId == OrderId::Harvest3
		)
	)
	{

		if (unit->mainOrderId != OrderId::Die) {

			while (unit->orderQueueTail != NULL && unit->orderQueueTail->orderId == OrderId::ResetCollision2)
				removeOrderFromUnitQueue(unit, unit->orderQueueTail);

			unit->performAnotherOrder(OrderId::ResetCollision2, 0, 0, NULL, UnitId::None, NULL);

		}
		
		actUnitReturnToIdle(unit);

	}
	else
	{

		//remove IsGathering and NoCollide status if set
		unit->status = unit->status & ~(UnitStatus::IsGathering + UnitStatus::NoCollide);

		//seek nearby units, and possibly perform some changes on them using FindBestUnit
		function_0042D9A0(unit);

		if (unit->orderQueueHead == NULL)
			unit->order(units_dat::ReturnToIdleOrder[unit->id], 0, 0, NULL, UnitId::None, false);

		toggleUnitPath(unit);

		unit->movementState = 0;

		if (unit->sprite->elevationLevel >= 12) {
			if (unit->pathingFlags & 1)
				unit->pathingFlags -= 1;
		}
		else //unit->sprite->elevationLevel < 12
		if (!(unit->pathingFlags & 1))
			unit->pathingFlags += 1;

		actUnitReturnToIdle(unit);

	}

}

;

//Used by SCV after construction, and others
void orders_ResetCollision1(CUnit* unit) {

	unit->sprite->elevationLevel = units_dat::Elevation[unit->id];

	//remove status IsGathering and NoCollide if set, and set IsNormal if not set
	unit->status = (unit->status & ~(UnitStatus::IsGathering + UnitStatus::NoCollide)) | UnitStatus::IsNormal;

	//seek nearby units, and possibly perform some changes on them using FindBestUnit
	function_0042D9A0(unit);

	toggleUnitPath(unit);

	unit->movementState = 0;

	if (unit->sprite->elevationLevel >= 12) {
		if (unit->pathingFlags & 1)
			unit->pathingFlags -= 1;
	}
	else //unit->sprite->elevationLevel < 12
	if (!(unit->pathingFlags & 1))
		unit->pathingFlags += 1;

	if (unit->orderQueueHead == NULL)
		unit->order(units_dat::ReturnToIdleOrder[unit->id], 0, 0, NULL, UnitId::None, false);

	actUnitReturnToIdle(unit);

}

;

} //namespace hooks

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

const u32 Func_toggleUnitPath = 0x004E42A0;
void toggleUnitPath(CUnit* unit) {

	__asm {
		PUSHAD
		MOV ESI, unit
		CALL Func_toggleUnitPath
		POPAD
	}

}

;

} //Unnamed namespace

//End of helper functions

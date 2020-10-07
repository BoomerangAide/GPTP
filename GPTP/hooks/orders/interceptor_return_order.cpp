#include "interceptor_return_order.h"
#include <SCBW/api.h>

//Helper functions definitions

namespace {

void hideAndDisableUnit(CUnit* unit);				//E6340
bool SetMoveTarget_xy(CUnit* unit, int x, int y);	//EB820

} //unnamed namespace

namespace hooks {

//Identical to function @ 00466270
//Update the carrier and interceptor when interceptor is going back in
//and hide the interceptor
//Used directly instead of hooking since not used elsewhere
void function_00466270(CUnit* carrier, CUnit* interceptor) {

	if (interceptor->interceptor.hangar_link.next != NULL)
		(interceptor->interceptor.hangar_link.next)->interceptor.hangar_link.prev = interceptor->interceptor.hangar_link.prev;
	else
		carrier->carrier.outHangarChild = interceptor->interceptor.hangar_link.prev;

	if (interceptor->interceptor.hangar_link.prev != NULL)
		(interceptor->interceptor.hangar_link.prev)->interceptor.hangar_link.next = interceptor->interceptor.hangar_link.next;

	carrier->carrier.outHangarCount--;

	interceptor->interceptor.hangar_link.prev = carrier->carrier.inHangarChild;
	interceptor->interceptor.hangar_link.next = NULL;

	if (carrier->carrier.inHangarChild != NULL)
		(carrier->carrier.inHangarChild)->interceptor.hangar_link.next = interceptor;

	carrier->carrier.inHangarChild = interceptor;

	carrier->carrier.inHangarCount++;

	interceptor->interceptor.isOutsideHangar = false;

	hideAndDisableUnit(interceptor);

}

;

//Order performed by interceptor with mainOrderId OrderId::Return
void orders_Return(CUnit* unit) {

	CUnit* carrier = unit->interceptor.parent;

	if (carrier == NULL)
		unit->remove();
	else
	{

		bool bEndThere = false;
		int distance;

		if (unit->mainOrderState == 0) {

			distance =
				scbw::getDistanceFast(
					carrier->sprite->position.x * 256,
					carrier->sprite->position.y * 256,
					unit->halt.x,
					unit->halt.y
				);

			if (distance / 256 <= 60) {

				unit->mainOrderState = 1;

				if (unit->id == UnitId::ProtossInterceptor)
					unit->sprite->elevationLevel = carrier->sprite->elevationLevel - 2;

			}

		}

		distance =
			scbw::getDistanceFast(
				carrier->sprite->position.x * 256,
				carrier->sprite->position.y * 256,
				unit->halt.x,
				unit->halt.y
			);

		if (distance / 256 > 10) {

			SetMoveTarget_xy(unit, carrier->sprite->position.x, carrier->sprite->position.y);

			if (
				unit->nextMovementWaypoint.x != carrier->sprite->position.x ||
				unit->nextMovementWaypoint.y != carrier->sprite->position.y
			)
			{
				unit->nextMovementWaypoint.x = carrier->sprite->position.x;
				unit->nextMovementWaypoint.y = carrier->sprite->position.y;
			}

		}
		else
		{
			function_00466270(carrier, unit);
			unit->orderComputerCL(OrderId::Nothing2);
			scbw::refreshConsole();
		}

	}

}

;

} //hooks

//-------- Helper function definitions. Do NOT modify! --------//

namespace {

const u32 Func_unitDeathSomething_0 = 0x004E6340;
void hideAndDisableUnit(CUnit* unit) {

	__asm {
		PUSHAD
		MOV EAX, unit
		CALL Func_unitDeathSomething_0
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

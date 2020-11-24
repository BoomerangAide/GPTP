#include "cloak_nearby_units_order.h"
#include <SCBW/api.h>

//helper functions def
namespace {

void actUnitReturnToIdle(CUnit* unit);	//75420

} //unnamed namespace

namespace hooks {

void orders_cloakNearbyUnits(CUnit* unit) {

	if (unit->orderQueueHead == NULL) {
		unit->userActionFlags |= 1;
		unit->order(units_dat::ReturnToIdleOrder[UnitId::ProtossArbiter], 0, 0, NULL, UnitId::None, true);
		prepareForNextOrder(unit);
	}

	if (
		!(unit->status & UnitStatus::IsHallucination) &&
		unit->secondaryOrderId != OrderId::CloakNearbyUnits
	) 
	{
		unit->secondaryOrderId = OrderId::CloakNearbyUnits;
		unit->secondaryOrderPos.x = 0;
		unit->secondaryOrderPos.y = 0;
		unit->currentBuildUnit = NULL;
		unit->secondaryOrderState = 0;
	}

	actUnitReturnToIdle(unit);

}

;

} //hooks

;

//-------- Helper function definitions. Do NOT modify! --------//

namespace {

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

} //Unnamed namespace

//End of helper functions

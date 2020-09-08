#include "recharge_shields.h"
#include "../SCBW/api.h"

//-------- Helper function declarations. Do NOT modify! --------//
namespace {

bool isHangarUnit(CUnit* unit);							//01450

} //unnamed namespace

//-------- Actual hook functions --------//

namespace hooks {

/// Decides whether the @p target can recharge shields from the @p battery.
Bool32 unitCanRechargeShieldsHook(CUnit* target, CUnit* battery) {

	Bool32 result;

	//Check target conditions
	if (	target->playerId != battery->playerId ||								//Is not owned by the player
			!(units_dat::ShieldsEnabled[target->id]) ||								//Does not have shields
			!(target->status & UnitStatus::Completed) ||							//Is incomplete?
			!(target->status & UnitStatus::IsBuilding) ||							//Is building (like archon build self)?
			target->status & UnitStatus::GroundedBuilding ||						//Is a building
			target->getRace() != RaceId::Protoss ||									//Check target race
			target->shields >= (units_dat::MaxShieldPoints[target->id] * 256) ||	//Already has max shields
			!(battery->status & UnitStatus::Completed) ||
			battery->energy == 0 ||
			battery->isFrozen() ||
			isHangarUnit(target)
	  )                       
		result = 0;
	else
	if(target->pAI == NULL)
		result = 1;
	else { //Separate check for AI-controlled units
		if (target->mainOrderId == OrderId::RechargeShields1 || target->mainOrderId == OrderId::Pickup4)
			result = 0;
		else
		if (target->orderQueueHead != NULL && target->orderQueueHead->orderId == OrderId::RechargeShields1)
			result = 0;
		else
			result = 1;
	}

	return result;

}

;

} //hooks


//-------- Helper function definitions. Do NOT modify! --------//

namespace {

//Identical to unitIsHangerUnit @ 0x00401450
bool isHangarUnit(CUnit* unit) {
	if(
		unit->id == UnitId::ProtossInterceptor ||
		unit->id == UnitId::ProtossScarab
	)
		return true;
	else
		return false;
}

;

} //unnamed namespace

#include "shield_recharge_orders.h"
#include <SCBW/api.h>

//helper functions def

namespace {

void insertFirstOrder(CUnit* unit, u8 orderId);										//749D0
u8 hasOverlay(CUnit* unit);															//7B720
void rechargeShieldsProc(CUnit* target, CUnit* battery);							//934B0
bool unitCanRechargeShields_Helper(CUnit* target, CUnit* battery);					//93520
void IterateUnitsAtLocationTargetProc_ShieldBattery(CUnit* unit, Box16* coords);	//E8280
void makeToHoldPosition(CUnit* unit);												//EB5B0
bool orderToMoveToTarget(CUnit* unit, CUnit* target);								//EB980

} //unnamed namespace

const int BATTERY_RANGE = 0x80; //(128)

namespace hooks {

//The order process ran by a shield battery attempting the
//recharge shield order
void orders_RechargeShields2(CUnit* unit) {

	CUnit* target = unit->orderTarget.unit;

	if (unit->secondaryOrderId != OrderId::ShieldBattery) {
		unit->secondaryOrderId = OrderId::ShieldBattery;
		unit->secondaryOrderPos.x = 0;
		unit->secondaryOrderPos.y = 0;
		unit->currentBuildUnit = NULL;
		unit->secondaryOrderState = 0;
	}

	if (target != NULL && unitCanRechargeShields_Helper(target, unit))
		target->orderTo(OrderId::Follow, unit);
	else
	{

		Box16 coords;

		coords.left = unit->orderTarget.pt.x - BATTERY_RANGE;
		coords.right = unit->orderTarget.pt.x + BATTERY_RANGE;
		coords.top = unit->orderTarget.pt.y - BATTERY_RANGE;
		coords.bottom = unit->orderTarget.pt.y + BATTERY_RANGE;

		//order all units, near target location,
		//with unitCanRechargeShields returning true,
		//to come and recharge
		IterateUnitsAtLocationTargetProc_ShieldBattery(unit,&coords);

	}

	if (unit->orderQueueHead != NULL) {
		unit->userActionFlags |= 1;
		prepareForNextOrder(unit);
	}
	else
	if(unit->pAI != NULL)
		unit->orderComputerCL(OrderId::ComputerAI);
	else
		unit->orderComputerCL(units_dat::ReturnToIdleOrder[unit->id]);

}

;

//The order process run by a unit when recharging shields
void orders_RechargeShields1(CUnit* unit) {

	CUnit* battery = unit->orderTarget.unit;

	//Skip if the Shield Battery does not exist, has no energy, or is disabled
	//isFrozen code was hardcoded rather than function call in original code
	if (battery == NULL || battery->energy == 0 || battery->isFrozen()) {

		if (unit->orderQueueHead == NULL) {
			unit->userActionFlags |= 1;
			prepareForNextOrder(unit);
		}
		else
		if (unit->pAI != NULL)
			unit->orderComputerCL(OrderId::ComputerAI);
		else
			unit->orderComputerCL(units_dat::ReturnToIdleOrder[unit->id]);

	}
	else 
	if(unit->mainOrderState <= 3)
	{

		bool bStopThere = false;

		if(unit->mainOrderState == 0) {

			if(orderToMoveToTarget(unit,battery))
				unit->mainOrderState = 1;

			bStopThere = true;

		}
		else
		if(unit->mainOrderState == 1) {

			u32 movableState = unit->getMovableState();

			if(movableState == 0) { //not reached destination
				if(!unit->isTargetWithinMinRange(battery, BATTERY_RANGE))
					bStopThere = true;
				else
				{
					makeToHoldPosition(unit);
					unit->mainOrderState = 2;
				}
			}
			else
			if(movableState == 1) { //unit reached destination
				makeToHoldPosition(unit);
				unit->mainOrderState = 2;
			}
			else {					//probably movableState == 2 (== unit unmovable)
				unit->orderToIdle();
				bStopThere = true;
			}

		}

		if(!bStopThere && unit->mainOrderState == 2) {

			u8 overlaysCounter;

			if(battery->secondaryOrderId != OrderId::ShieldBattery)
				battery->setSecondaryOrder(OrderId::ShieldBattery);

			overlaysCounter = hasOverlay(unit);
			unit->sprite->createOverlay(ImageId::RechargeShields_Small + overlaysCounter,0,0,0);

			//the order id is correct, see OrderId.h description
			insertFirstOrder(unit,OrderId::Harvest5);

			unit->mainOrderState = 3;

		}

		if(!bStopThere && unit->mainOrderState == 3) {

			if(battery->orderTarget.unit == NULL)
				battery->orderTarget.unit = unit;

			//do 1 recharge action? (increase unit shield, decrease battery energy)
			rechargeShieldsProc(unit,battery);

			if(
				unit->shields >= units_dat::MaxShieldPoints[unit->id] * 256 ||
				battery->energy == 0
			)
			{

				if(battery->orderTarget.unit == unit)
					battery->orderTarget.unit = NULL;

				unit->orderToIdle();

			}

		} //if(!bStopThere && unit->mainOrderState == 3)

	} //if(unit->mainOrderState <= 3)

}

;

} //namespace hooks

;

//-------- Helper function definitions. Do NOT modify! --------//

namespace {

const u32 Func_InsertFirstOrder = 0x004749D0;
void insertFirstOrder(CUnit* unit, u8 orderId) {

	__asm {
		PUSHAD
		MOV AL, orderId
		MOV ECX, unit
		CALL Func_InsertFirstOrder
		POPAD
	}

}

;

const u32 Func_hasOverlay = 0x0047B720;
/// Return the count of overlays
u8 hasOverlay(CUnit* unit) {

	static u8 result;

	__asm {
		PUSHAD
		MOV EAX, unit
		CALL Func_hasOverlay
		MOV result, AL
		POPAD
	}

	return result;

}

;

//This function is called every frame when a unit recharges shields.
//Equivalent to 004934B0  sub_4934B0
//ECX = unit, EBX = battery
//Not hooked
void rechargeShieldsProc(CUnit* target, CUnit* battery) {

	const s32 maxShields = units_dat::MaxShieldPoints[target->id] * 256;

	s32 shieldGain = 1280, energySpent = 640;

	if (maxShields - target->shields < shieldGain) {

		shieldGain = maxShields - target->shields;

		if(shieldGain < 0)
			shieldGain++;

		energySpent = shieldGain / 2;

	}

	if (energySpent > battery->energy) {
		energySpent = battery->energy;
		shieldGain = energySpent * 2;
	}

	target->shields = target->shields + shieldGain;

	if (target->shields > maxShields)
		target->shields = maxShields;

	if (!scbw::isCheatEnabled(CheatFlags::TheGathering))
		battery->energy -= energySpent;

}

;

const u32 Func_unitCanRechargeShields = 0x00493520;
//hooked in hooks/recharge_shields
bool unitCanRechargeShields_Helper(CUnit* target, CUnit* battery) {

	static Bool32 bPreResult;

	__asm {
		PUSHAD
		MOV EAX, target
		MOV EDI, battery
		CALL Func_unitCanRechargeShields
		MOV bPreResult, EAX
		POPAD
	}

	return (bPreResult != 0);

}

;

const u32 Func_IterateUnitsAtLocationTargetProc = 0x004E8280;
void IterateUnitsAtLocationTargetProc_ShieldBattery(CUnit* unit, Box16* coords) {
	__asm {
		PUSHAD
		PUSH unit
		MOV EAX, coords
		MOV EBX, 0x00493900
		CALL Func_IterateUnitsAtLocationTargetProc
		POPAD
	}
}

;

const u32 Func__moveToTarget = 0x004EB980;
bool orderToMoveToTarget(CUnit* unit, CUnit* target) {

	static Bool32 bPreResult;
  
	__asm {
		PUSHAD
		MOV EAX, target
		MOV ECX, unit
		CALL Func__moveToTarget
		MOV bPreResult, EAX
		POPAD
	}

	return (bPreResult != 0);

}

;

const u32 Func_OrdersHoldPositionSuicidal = 0x004EB5B0;
void makeToHoldPosition(CUnit* unit) {
	__asm {
		PUSHAD
		MOV ESI, unit
		CALL Func_OrdersHoldPositionSuicidal
		POPAD
	}
}

;

} //Unnamed namespace

//End of helper functions

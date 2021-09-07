#include "right_click_returnedOrders.h"
#include <SCBW/api.h>

//Helper functions declaration

namespace {

u32 function_004557C0(CUnit* unit, CUnit* target);														//557C0
u32 returnNormalMove_NormalAttack_Order_Helper(CUnit* unit, int x, int y, CUnit** target, u32 unitId);	//55900
bool isOrderValid(CUnit* unit, CUnit* target, u8 orderId, u32 unk);										//74D90
u32 getSpellStatString(CUnit* target, u32 techId, u8 playerId);											//91E80
bool unitIsActiveTransport(CUnit* unit);																//E6BA0

} //unnamed namespace

namespace hooks {

//0x00455650
u32 returnNothing2(CUnit* unit, int x, int y, CUnit** target, u32 unitId) {
	return OrderId::Nothing2;
}

;

//0x00455900
u32 returnNormalMove_NormalAttack_Order(CUnit* unit, int x, int y, CUnit** target, u32 unitId) {

	u32 returned_order;

	if (unit->status & UnitStatus::GroundedBuilding)
		returned_order = OrderId::Nothing2;
	else
	if (*target == NULL) {
		if (unitId != UnitId::None)
			returned_order = OrderId::Rightclickaction;
		else
			returned_order = OrderId::Move;
	}
	else
	if((*target)->id >= UnitId::Special_ZergBeacon && (*target)->id <= UnitId::Special_ProtossFlagBeacon)
		returned_order = OrderId::Move;
	else
	if(units_dat::BaseProperty[(*target)->id] & UnitProperty::NeutralAccessories)
		returned_order = OrderId::Move;
	else
	if(unit->isTargetEnemy(*target))
		returned_order = OrderId::Attack1;
	else
	if(unitIsActiveTransport(*target) && scbw::canBeEnteredBy(*target,unit))
		returned_order = OrderId::EnterTransport;
	else
	if((*target)->status & UnitStatus::Burrowed)
		returned_order = OrderId::Move;
	else
		returned_order = OrderId::Follow;

	return returned_order;

}

;

//0x00455E00
u32 returnNormalMove_NoAttack_Order(CUnit* unit, int x, int y, CUnit** target, u32 unitId) {

	u32 returned_order;

	if (unit->status & UnitStatus::GroundedBuilding) {
		if (unit->isFactory()) {
			if (*target == NULL)
				returned_order = OrderId::RallyPoint2;
			else
				returned_order = OrderId::RallyPoint1;
		}
		else
			returned_order = OrderId::Nothing2;
	}
	else
	if (*target == NULL) {
		if (
			(unit->id == UnitId::ZergQueen || unit->id == UnitId::Hero_Matriarch) &&
			unitId != UnitId::None
		)
			returned_order = OrderId::Rightclickaction;
		else
			returned_order = OrderId::Move;
	}
	else
	if(unitIsActiveTransport(unit) && isOrderValid(unit,*target,OrderId::Pickup2,0))
		returned_order = OrderId::Pickup2;
	else
	if(unitIsActiveTransport(*target) && scbw::canBeEnteredBy(*target,unit))
		returned_order = OrderId::EnterTransport;
	else
	if((*target)->status & UnitStatus::Burrowed)
		returned_order = OrderId::Move;
	else
	if (
		(unit->id == UnitId::ZergQueen || unit->id == UnitId::Hero_Matriarch) &&
		!((*target)->status & UnitStatus::Invincible) &&
		getSpellStatString(*target,TechId::Infestation,unit->playerId) == 0
	)
		returned_order = OrderId::InfestMine2;
	else
	if(unit->id == UnitId::TerranMedic)
		returned_order = OrderId::HealMove;
	else
		returned_order = OrderId::Follow;

	return returned_order;

}

;

//0x00455890
u32 returnNoMove_NormalAttack_Order(CUnit* unit, int x, int y, CUnit** target, u32 unitId) {

	u32 returned_order;

	if (
		*target == NULL ||
		((*target)->id >= UnitId::Special_ZergBeacon && (*target)->id <= UnitId::Special_ProtossFlagBeacon) ||
		units_dat::BaseProperty[(*target)->id] & UnitProperty::NeutralAccessories ||
		!(unit->isTargetEnemy(*target))
	)
		returned_order = units_dat::ReturnToIdleOrder[unit->id];
	else
		returned_order = OrderId::Attack1;

	return returned_order;

}

;

//0x004559C0
u32 returnHarvest_Order(CUnit* unit, int x, int y, CUnit** target, u32 unitId) {

	u32 returned_order;

	if (*target != NULL) {

		returned_order = function_004557C0(unit, *target);

		if (returned_order == OrderId::None)
			returned_order = returnNormalMove_NormalAttack_Order(unit, x, y, target, unitId);

	}
	else
		returned_order = returnNormalMove_NormalAttack_Order(unit, x, y, target, unitId);

	return returned_order;

}

;		

//0x00455CC0
u32 returnHarvestAndRepair_Order(CUnit* unit, int x, int y, CUnit** target, u32 unitId) {

	u32 returned_order;

	if(*target == NULL)
		returned_order = returnNormalMove_NormalAttack_Order(unit, x, y, target, unitId);
	else {

		returned_order = function_004557C0(unit, *target);

		if (returned_order == OrderId::None) {

			bool targetCannotNeedToBeBuilt = false;
			bool skippingAllLocalChecks = false;

			targetCannotNeedToBeBuilt =
				!((*target)->status & UnitStatus::GroundedBuilding) ||
				(*target)->status & UnitStatus::Completed;

			if(!targetCannotNeedToBeBuilt)
				skippingAllLocalChecks =
					(*target)->playerId != unit->playerId ||
					(*target)->getRace() != RaceId::Terran ||
					units_dat::BaseProperty[(*target)->id] & UnitProperty::Addon ||
					((*target)->connectedUnit != NULL && (*target)->connectedUnit->orderTarget.unit == *target);

			if (!targetCannotNeedToBeBuilt && !skippingAllLocalChecks)
				returned_order = OrderId::ConstructingBuilding;
			else
			if (targetCannotNeedToBeBuilt) { //55D5D

				if (playerAlliance[unit->playerId].flags[(*target)->playerId] < 1)
					skippingAllLocalChecks = true;
				else
				if(
					!(units_dat::BaseProperty[(*target)->id] & UnitProperty::Building) &&
					unitIsActiveTransport(*target) &&
					scbw::canBeEnteredBy(*target,unit)
				)
					returned_order = OrderId::EnterTransport;
				else
				if(isOrderValid(unit,*target,OrderId::Repair1,0))
					returned_order = OrderId::Repair1;
				else
				if(unitIsActiveTransport(*target) && scbw::canBeEnteredBy(*target,unit))
					returned_order = OrderId::EnterTransport;
				else
					skippingAllLocalChecks=true;

			}

			if(skippingAllLocalChecks)
				returned_order = returnNormalMove_NormalAttack_Order(unit, x, y, target, unitId);
		}

	}

	return returned_order;

}

;		

} //namespace hooks

//-------- Helper function definitions. Do NOT modify! --------//

namespace {

const u32 Func_Sub557C0 = 0x004557C0;
u32 function_004557C0(CUnit* unit, CUnit* target) {

	static u32 return_value;

	__asm {
		PUSHAD
		MOV EDI, target
		MOV ESI, unit
		CALL Func_Sub557C0
		MOV return_value, EAX
		POPAD
	}

	return return_value;

}

;

const u32 Func_returnNormalMove_NormalAttack_Order = 0x00455900;
u32 returnNormalMove_NormalAttack_Order_Helper(CUnit* unit, int x, int y, CUnit** target, u32 unitId){

	static u32 return_value;

	__asm {
		PUSHAD
		PUSH unitId
		PUSH target
		PUSH y
		MOV EDX, x
		MOV ECX, unit
		CALL Func_returnNormalMove_NormalAttack_Order
		MOV return_value, EAX
		POPAD
	}

	return return_value;

}

;

const u32 Func_isOrderValid = 0x00474D90;
bool isOrderValid(CUnit* unit, CUnit* target, u8 orderId, u32 unk) {

	static Bool32 bPreResult;

	__asm {
		PUSHAD
		PUSH unk
		MOV EBX, unit
		MOV ESI, target
		MOV AL, orderId
		CALL Func_isOrderValid
		MOV bPreResult, EAX
		POPAD
	}

	return (bPreResult != 0);

}

;

const u32 Func_getSpellStatString = 0x00491E80;
u32 getSpellStatString(CUnit* target, u32 techId, u8 playerId) {

	static u32 return_value;

	__asm {
		PUSHAD
		MOV BL, playerId
		PUSH techId
		MOV EAX, target
		CALL Func_getSpellStatString
		MOV return_value, EAX
		POPAD
	}

	return return_value;
}

;

//Equivalent to code unitIsActiveTransport @ 0x004E6BA0
bool unitIsActiveTransport(CUnit* unit) {

	bool result = false;

	if(!(unit->status & UnitStatus::IsHallucination)) {

		if(
			unit->id != UnitId::ZergOverlord ||
			UpgradesSc->currentLevel[unit->playerId][UpgradeId::VentralSacs] != 0
		)
		{
			if(units_dat::SpaceProvided[unit->id] != 0)
				result = true;
		}


	}

	return result;

}

;

} //unnamed namespace

//End of helper functions

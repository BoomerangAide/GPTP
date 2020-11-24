#include "CMDRECV_RightClick.h"
#include <SCBW/api.h>

//Helper functions declaration

namespace {

u32 returnNothing2_Helper(CUnit* unit, int x, int y, CUnit** target, u32 unitId);						//55650
u32 function_004557C0(CUnit* unit, CUnit* target);														//557C0
u32 returnNormalMove_NormalAttack_Order_Helper(CUnit* unit, int x, int y, CUnit** target, u32 unitId);	//55900
u32 returnNormalMove_NoAttack_Order_Helper(CUnit* unit, int x, int y, CUnit** target, u32 unitId);		//55E00
u32 returnNoMove_NormalAttack_Order_Helper(CUnit* unit, int x, int y, CUnit** target, u32 unitId);		//55890
u32 returnHarvest_Order_Helper(CUnit* unit, int x, int y, CUnit** target, u32 unitId);					//559C0
u32 returnHarvestAndRepair_Order_Helper(CUnit* unit, int x, int y, CUnit** target, u32 unitId);			//55CC0

Bool32 OrderAllowed(CUnit* unit, u16 order, u32 nationID);												//6DC20
bool isOrderValid(CUnit* unit, CUnit* target, u8 orderId, u32 unk);										//74D90
void function_004756B0(CUnit* unit, u8 orderId, int y, int x, u32 unitId, u32 bCommandType);			//756B0
void function_004756E0(CUnit* unit, CUnit* target, u32 orderId, u32 bCommandType);						//756E0
CUnit* function_00487FD0(int x, int y, u32 unitId);														//87FD0
u32 getSpellStatString(CUnit* target, u32 techId, u8 playerId);											//91E80
void function_0049A500(CUnit* unit,	u8* array_of_data);													//9A500
void function_0049A8C0(u8* array_of_data,Bool32 bCanBeObstructed);										//9A8C0
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

//Function 004560D0 called by 004C2370  CMDRECV_RightClick
//Do the actual work after the initial function
//confirmed x and y are not out of map and extracted
//the target unit
void CMDRECV_RightClick_Proc(s16 x, s16 y, CUnit* target, u16 wUnitType, u8 bCommandType) {

	u8* const u8_0068C300 = (u8*) 0x0068C300;

	u8 array_of_data[0x28];
	/*[EBP-0x28]*/ s16* saved_x = (s16*)&array_of_data[0];				
	/*[EBP-0x26]*/ s16* saved_y = (s16*)&array_of_data[2];				
	/*[EBP-0x24]*/ s16* saved_x2 = (s16*)&array_of_data[4];			
	/*[EBP-0x22]*/ s16* saved_y2 = (s16*)&array_of_data[6];			
					 										
	/*[EBP-0x0C]*/ Bool32* isQueued = (Bool32*)&array_of_data[0x1C];	
	/*[EBP-0x08]*/ u32* tileIndex_orderId = (u32*)&array_of_data[0x20];
	/*[EBP-0x04]*/ CUnit** saved_target = (CUnit**)&array_of_data[0x24];	

	CUnit* current_unit;

	*saved_x = x;
	*saved_y = y;

	function_0049A8C0(array_of_data, 1);

	if (target != NULL)
		wUnitType = UnitId::None;
	else
	if (wUnitType != UnitId::None) {

		int mapX, mapY;

		if (y < 0)
			mapY = y + 31;
		else
			mapY = y;

		mapY /= 32;

		if (x < 0)
			mapX = x + 31;
		else
			mapX = x;

		mapX /= 32;

		*tileIndex_orderId = mapY * (*mapTileSize).width + mapX;

		if (((*activeTileArray)[*tileIndex_orderId].visibilityFlags & (1 << *ACTIVE_NATION_ID)) == 0) {
			target = function_00487FD0(x, y, wUnitType);
			wUnitType = UnitId::None;
		}

	}

	//56160
	*selectionIndexStart = 0;
	current_unit = getActivePlayerNextSelection();

	while (current_unit != NULL) {

		u8 rightClickAction;
		u32 orderId;
		bool jumpNearLoopEnd = false;
		bool jumpToActionFlagSet = false;

		if (current_unit->id == UnitId::ZergLurker && current_unit->status & UnitStatus::Burrowed)
			rightClickAction = RightClickActions::NoMove_NormalAttack;
		else
		{

			rightClickAction = units_dat::RightClickAction[current_unit->id];

			if (
				rightClickAction == RightClickActions::NoCommand_AutoAttack &&
				current_unit->status & UnitStatus::GroundedBuilding &&
				current_unit->isFactory()
			)
				rightClickAction = RightClickActions::NormalMove_NoAttack;

		}

		//561B2
		if (
			current_unit->status & UnitStatus::IsHallucination &&
			(rightClickAction == RightClickActions::Harvest || rightClickAction == RightClickActions::HarvestAndRepair)
		)
			rightClickAction = RightClickActions::NormalMove_NormalAttack;

		*saved_target = target;
		*u8_0068C300 = OrderId::Nothing2;

		//was initially a switch using values from array at 0x005153FC as called functions
		if (rightClickAction == RightClickActions::NoCommand_AutoAttack || rightClickAction == RightClickActions::Unknown)
			orderId = returnNothing2_Helper(current_unit, x, y, saved_target, wUnitType);
		else
		if(rightClickAction == RightClickActions::NormalMove_NormalAttack)
			orderId = returnNormalMove_NormalAttack_Order_Helper(current_unit, x, y, saved_target, wUnitType);
		else
		if(rightClickAction == RightClickActions::NormalMove_NoAttack)
			orderId = returnNormalMove_NoAttack_Order_Helper(current_unit, x, y, saved_target, wUnitType);
		else
		if(rightClickAction == RightClickActions::NoMove_NormalAttack)
			orderId = returnNoMove_NormalAttack_Order_Helper(current_unit, x, y, saved_target, wUnitType);
		else
		if(rightClickAction == RightClickActions::Harvest)
			orderId = returnHarvest_Order_Helper(current_unit, x, y, saved_target, wUnitType);
		else
		if(rightClickAction == RightClickActions::HarvestAndRepair)
			orderId = returnHarvestAndRepair_Order_Helper(current_unit, x, y, saved_target, wUnitType);
		else
			orderId = returnNothing2_Helper(current_unit, x, y, saved_target, wUnitType); //added to avoid orderId not initialized message

		*tileIndex_orderId = orderId;

		if (orderId != OrderId::Nothing2) {

			if (orderId == OrderId::RallyPoint1) {

				if (current_unit->isFactory()) {

					CUnit* orderTarget;

					if (target != NULL)
						orderTarget = target;
					else
						orderTarget = current_unit;

					current_unit->rally.unit = orderTarget;
					current_unit->rally.pt.x = orderTarget->sprite->position.x;
					current_unit->rally.pt.y = orderTarget->sprite->position.y;
					jumpNearLoopEnd = true;

				}
				else
					jumpNearLoopEnd = true;

			}

			//5623A
			if (!jumpNearLoopEnd && !(jumpNearLoopEnd = (current_unit == target)) && orderId == OrderId::RallyPoint2) {

				if (current_unit->isFactory()) {
					current_unit->rally.unit = NULL;
					current_unit->rally.pt.x = x;
					current_unit->rally.pt.y = y;
				}
				
				jumpNearLoopEnd = true;

			}

			//56273
			if (!jumpNearLoopEnd) {

				if (orderId == OrderId::Attack1) {

					if (current_unit->subunit != NULL)
						function_004756E0(current_unit->subunit, *saved_target, units_dat::AttackUnitOrder[current_unit->subunit->id], bCommandType != 0);
					
					*tileIndex_orderId = units_dat::AttackUnitOrder[current_unit->id];
					orderId = *tileIndex_orderId;

				}

				//562B2
				if (OrderAllowed(current_unit, orderId, *ACTIVE_NATION_ID)) {

					if (*saved_target != NULL) {

						function_004756E0(current_unit, *saved_target, *tileIndex_orderId, bCommandType != 0);

						if (*u8_0068C300 == OrderId::Nothing2)
							jumpToActionFlagSet = true;
						else
						{
							function_004756E0(current_unit, *saved_target, *u8_0068C300, bCommandType != 0);
							jumpToActionFlagSet = true;
						}

					}
					else //56301
					{

						*saved_y = y;
						*saved_x = x;

						if (wUnitType == UnitId::None)
							function_0049A500(current_unit, array_of_data);
						else
						{
							*saved_x2 = *saved_x;
							*saved_y2 = *saved_y;
						}

						function_004756B0(current_unit, *tileIndex_orderId, *saved_y2, *saved_x2, wUnitType, bCommandType != 0);

						if (*u8_0068C300 == OrderId::Nothing2)
							jumpToActionFlagSet = true;
						else {
							function_004756B0(current_unit, *u8_0068C300, *saved_y2, *saved_x2, wUnitType, bCommandType != 0);
							jumpToActionFlagSet = true;
						}

					}
				}

			}

		}

		if (jumpToActionFlagSet)
			current_unit->userActionFlags |= 2;

		//56384 (NearLoopEnd)
		current_unit = getActivePlayerNextSelection();

	}

}

;

} //namespace hooks

//-------- Helper function definitions. Do NOT modify! --------//

namespace {

const u32 Func_returnNothing2 = 0x00455650;
u32 returnNothing2_Helper(CUnit* unit, int x, int y, CUnit** target, u32 unitId) {

	static u32 return_value;

	__asm {
		PUSHAD
		PUSH unitId
		PUSH target
		PUSH y
		MOV EDX, x
		MOV ECX, unit
		CALL Func_returnNothing2
		MOV return_value, EAX
		POPAD
	}

	return return_value;

}

;

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

const u32 Func_returnNormalMove_NoAttack_Order = 0x00455E00;
u32 returnNormalMove_NoAttack_Order_Helper(CUnit* unit, int x, int y, CUnit** target, u32 unitId){

	static u32 return_value;

	__asm {
		PUSHAD
		PUSH unitId
		PUSH target
		PUSH y
		MOV EDX, x
		MOV ECX, unit
		CALL Func_returnNormalMove_NoAttack_Order
		MOV return_value, EAX
		POPAD
	}

	return return_value;

}

;

const u32 Func_returnNoMove_NormalAttack_Order = 0x00455890;
u32 returnNoMove_NormalAttack_Order_Helper(CUnit* unit, int x, int y, CUnit** target, u32 unitId){

	static u32 return_value;

	__asm {
		PUSHAD
		PUSH unitId
		PUSH target
		PUSH y
		MOV EDX, x
		MOV ECX, unit
		CALL Func_returnNoMove_NormalAttack_Order
		MOV return_value, EAX
		POPAD
	}

	return return_value;

}

;

const u32 Func_returnHarvest_Order = 0x004559C0;
u32 returnHarvest_Order_Helper(CUnit* unit, int x, int y, CUnit** target, u32 unitId){

	static u32 return_value;

	__asm {
		PUSHAD
		PUSH unitId
		PUSH target
		PUSH y
		MOV EDX, x
		MOV ECX, unit
		CALL Func_returnHarvest_Order
		MOV return_value, EAX
		POPAD
	}

	return return_value;

}

;

const u32 Func_returnHarvestAndRepair_Order = 0x00455CC0;
u32 returnHarvestAndRepair_Order_Helper(CUnit* unit, int x, int y, CUnit** target, u32 unitId){

	static u32 return_value;

	__asm {
		PUSHAD
		PUSH unitId
		PUSH target
		PUSH y
		MOV EDX, x
		MOV ECX, unit
		CALL Func_returnHarvestAndRepair_Order
		MOV return_value, EAX
		POPAD
	}

	return return_value;

}

;

const u32 Func_OrderAllowed = 0x0046DC20;
Bool32 OrderAllowed(CUnit* unit, u16 order, u32 nationID) {

	static Bool32 bResult;

	__asm {
		PUSHAD
		MOV BX, order
		MOV EAX, unit
		PUSH nationID
		CALL Func_OrderAllowed
		MOV bResult, EAX
		POPAD
	}

	return bResult;

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

const u32 Func_Sub4756B0 = 0x004756B0;
//Prepare arguments then call function_004754F0 hooked in CMDRECV_QueuedOrder
void function_004756B0(CUnit* unit, u8 orderId, int y, int x, u32 unitId, u32 bCommandType) {

	__asm {
		PUSHAD
		MOV EBX, unitId
		MOV DL, orderId
		MOV ESI, unit
		PUSH y
		PUSH x
		PUSH bCommandType
		CALL Func_Sub4756B0
		POPAD
	}

}

;

const u32 Func_Sub4756E0 = 0x004756E0;
//Prepare arguments then call function_004754F0 hooked in CMDRECV_QueuedOrder
void function_004756E0(CUnit* unit, CUnit* target, u32 orderId, u32 bCommandType) {

	__asm {
		PUSHAD
		MOV EDX, target
		MOV ESI, unit
		PUSH orderId
		PUSH bCommandType
		CALL Func_Sub4756E0
		POPAD
	}

}

;

const u32 Func_Sub487FD0 = 0x00487FD0;
//return an unit from a location
CUnit* function_00487FD0(int x, int y, u32 unitId) {

	static CUnit* return_value;

	__asm {
		PUSHAD
		PUSH unitId
		MOV EAX, y
		MOV ECX, x
		CALL Func_Sub487FD0
		MOV return_value, EAX
		POPAD
	}

	return return_value;

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

const u32 Func_Sub49A500 = 0x0049A500;
void function_0049A500(CUnit* unit,	u8* array_of_data) {

	__asm {
		PUSHAD
		PUSH array_of_data
		PUSH unit
		CALL Func_Sub49A500
		POPAD
	}

}

;

const u32 Func_Sub49A8C0 = 0x0049A8C0;
void function_0049A8C0(u8* array_of_data, Bool32 bCanBeObstructed) {

	__asm {
		PUSHAD
		PUSH bCanBeObstructed
		MOV EDI, array_of_data
		CALL Func_Sub49A8C0
		POPAD
	}	

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

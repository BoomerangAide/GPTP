#include "move_orders.h"
#include <SCBW/api.h>

//helper functions def

namespace {

bool isInfestableUnit(CUnit* unit);											//02210
void orders_Reaver_Helper(CUnit* unit);										//665D0
void orders_Carrier_Helper(CUnit* unit);									//666A0
void function_00474A20(CUnit* unit, u8 orderId, int x, int y);				//74A20
bool isTargetWithinMinMovementRange(CUnit* unit, CUnit* target, u32 range);	//763D0
bool isUnitRescuable(CUnit* unit);											//7B490
u32 get_statTxt_Str_0(CUnit* unit, u32 playerId, char* message);			//8EF30
bool unitCanRechargeShields_Helper(CUnit* battery, CUnit* target);			//93520
void function_00494BB0(CUnit* unit,int x, int y);							//94BB0
bool isTargetVisible(CUnit* unit, CUnit* target);							//E5E30
bool canEnterNydusCanal_Helper(CUnit* unit, CUnit* nydusCanal);				//E8C20
void setNextWaypoint_Sub4EB290(CUnit* unit);								//EB290
void makeToHoldPosition_Helper(CUnit* unit);								//EB5B0
bool moveToTarget(CUnit* unit, CUnit* target);								//EB720
bool SetMoveTarget_xy(CUnit* unit, int x, int y);							//EB820
bool function_004EB900(CUnit* unit, CUnit* target);							//EB900
bool orderToMoveToTarget(CUnit* unit, CUnit* target);						//EB980

} //unnamed namespace

namespace hooks {

//Function 00466720, used by OrderId::CarrierIgnore2
void orders_CarrierIgnore2(CUnit* unit) {

	bool bStopThere = false;

	if (unit->mainOrderState == 0) {

		bStopThere = !(SetMoveTarget_xy(unit, unit->orderTarget.pt.x, unit->orderTarget.pt.y));

		if (
			unit->nextTargetWaypoint.x != unit->orderTarget.pt.x ||
			unit->nextTargetWaypoint.y != unit->orderTarget.pt.y
		)
		{
			unit->nextTargetWaypoint.x = unit->orderTarget.pt.x;
			unit->nextTargetWaypoint.y = unit->orderTarget.pt.y;
		}

		if (!bStopThere)
			unit->mainOrderState = 1;

	}

	if (!bStopThere) {

		if (unit->id == UnitId::ProtossReaver || unit->id == UnitId::Hero_Warbringer) {
			orders_Reaver_Helper(unit);
			if (unit->orderTarget.unit != NULL)
				unit->mainOrderId = OrderId::ReaverFight;
		}
		else
			orders_Carrier_Helper(unit);

	}

}

;

//Function used by OrderId::StayinRange
//Seen used by Siege Tank
void orders_StayInRange(CUnit* unit) {
	
	bool bGoliathSpecialCode = false;
	CUnit* target = unit->orderTarget.unit;

	if (target != NULL) {

		unit->orderTarget.pt.x = target->sprite->position.x;
		unit->orderTarget.pt.y = target->sprite->position.y;

		if (unit->canAttackTarget(target, true)) {

			if (unit->mainOrderState == 0) {

				bool bDontMoveToTarget = false;

				//not checking if target is NULL like original

				if (isTargetVisible(unit, target)) {

					CUnit* current_unit;
					u8 weaponId;

					if (unit->subunit != NULL && units_dat::BaseProperty[unit->subunit->id] & UnitProperty::Subunit)
						current_unit = unit->subunit;
					else
						current_unit = unit;

					if (target->status & UnitStatus::InAir)
						weaponId = units_dat::AirWeapon[current_unit->id];
					else
					if(current_unit->id == UnitId::ZergLurker && !(current_unit->status & UnitStatus::Burrowed))
						weaponId = WeaponId::None;
					else
						weaponId = units_dat::GroundWeapon[current_unit->id];

					if (weaponId < WeaponId::None) {

						u32 range = weapons_dat::MinRange[weaponId];

						//Note: target should NOT be within minRange (like too close of a sieged tank)
						//0 range means no min range
						if (range == 0 || !(current_unit->isTargetWithinMinRange(target, range))) {

							bDontMoveToTarget =
								isTargetWithinMinMovementRange(
									current_unit, 
									target,
									unit->getMaxWeaponRange(weaponId)
								);

						}

					}

				}

				if (bDontMoveToTarget || moveToTarget(unit,target)) {
					unit->mainOrderState = 1;
					bGoliathSpecialCode = (unit->id == UnitId::TerranGoliath || unit->id == UnitId::Hero_AlanSchezar);
				}

			} //if (unit->mainOrderState == 0)
			else
			{

				bool bDontMoveToTarget = false;

				//not checking if target is NULL like original

				if (isTargetVisible(unit, target)) {

					CUnit* current_unit;
					u8 weaponId;

					if (unit->subunit != NULL && units_dat::BaseProperty[unit->subunit->id] & UnitProperty::Subunit)
						current_unit = unit->subunit;
					else
						current_unit = unit;

					if (target->status & UnitStatus::InAir)
						weaponId = units_dat::AirWeapon[current_unit->id];
					else
					if(current_unit->id == UnitId::ZergLurker && !(current_unit->status & UnitStatus::Burrowed))
						weaponId = WeaponId::None;
					else
						weaponId = units_dat::GroundWeapon[current_unit->id];

					if (weaponId < WeaponId::None) {

						u32 range = weapons_dat::MinRange[weaponId];

						//Note: target should NOT be within minRange (like too close of a sieged tank)
						//0 range means no min range
						if (range == 0 || !(current_unit->isTargetWithinMinRange(target, range))) {

							bDontMoveToTarget =
								isTargetWithinMinMovementRange(
									current_unit, 
									target,
									unit->getMaxWeaponRange(weaponId)
								);
							
							if (bDontMoveToTarget)
								makeToHoldPosition_Helper(unit);							

						}

					}

				}

				if (!bDontMoveToTarget && function_004EB900(unit, target))
					bGoliathSpecialCode = (unit->id == UnitId::TerranGoliath || unit->id == UnitId::Hero_AlanSchezar);
				
			} //if (unit->mainOrderState != 0)

		} //if (unit->canAttackTarget(target, true))
		else
		{

			if (!(unit->status & UnitStatus::HoldingPosition) && unit->pAI == NULL)
				function_00474A20(unit, OrderId::Move, unit->orderTarget.pt.x, unit->orderTarget.pt.y);

			unit->orderToIdle();

		}

		if (bGoliathSpecialCode && !(unit->movementFlags & MovementFlags::Accelerating)) 
		{

			s32 angle =
				scbw::getAngle(
					unit->sprite->position.x,
					unit->sprite->position.y,
					unit->orderTarget.pt.x,
					unit->orderTarget.pt.y
				);

			angle = ((s8)angle) - unit->currentDirection1;

			if (angle >= 128 || angle <= -128)
				angle = 128;

			if (angle < 0)
				angle = -angle;

			if (angle >= 32)
				function_00494BB0(unit, unit->orderTarget.pt.x, unit->orderTarget.pt.y);

		}

	}
	else //if (target == NULL)
	{

		setNextWaypoint_Sub4EB290(unit);

		if (
			unit->nextTargetWaypoint.x != unit->moveTarget.pt.x ||
			unit->nextTargetWaypoint.y != unit->moveTarget.pt.y
		)
		{
			unit->nextTargetWaypoint.x = unit->moveTarget.pt.x;
			unit->nextTargetWaypoint.y = unit->moveTarget.pt.y;
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

}

;

//Function 0047C7B0, used by OrderId::Follow
void orders_Follow(CUnit* unit) {
	
	CUnit* target = unit->orderTarget.unit;

	if (target == NULL) {

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
	else
	{

		bool bEndThere = false;

		if (unit->pAI != NULL) {

			s32 hpValue = units_dat::MaxHitPoints[unit->id] / 256;

			if (hpValue == 0)
				hpValue = ((u32)unit->hitPoints + 255) / 256;

			if (hpValue == 0)
				hpValue = 1;

			if ( ( ( (u32)unit->hitPoints + 255 ) / 256 ) == hpValue ) {
				unit->orderToIdle();
				bEndThere = true;
			}

		}

		if (!bEndThere) {

			if (unit->mainOrderState == 0) {

				if (orderToMoveToTarget(unit, target)) {

					if (
						target->id == UnitId::ZergNydusCanal &&
						canEnterNydusCanal_Helper(unit, target)
					)
					{
						unit->orderTo(OrderId::Enternyduscanal, target);
						bEndThere = true;
					}
					else
					if (
						target->id == UnitId::ProtossShieldBattery &&
						unitCanRechargeShields_Helper(target, unit)
					)
					{
						unit->orderTo(OrderId::RechargeShields1, target);
						bEndThere = true;
					}

					if (!bEndThere)
						unit->mainOrderState = 1;

				}
				else
					bEndThere = true;

			}

			if (!bEndThere) {

				if (unit->mainOrderTimer == 0) {

					unit->mainOrderTimer = 7;

					if (
						unit->isTargetEnemy(target) &&
						unit->canAttackTarget(target,true)
					) 
						unit->orderComputerCL(OrderId::Guard);
					else
					{

						if (unit->status & UnitStatus::InAir || unit->hasPathToUnit(target)) {

							if (
								isUnitRescuable(target) &&
								!unit->isTargetWithinMinRange(target, 1)
							)
								orderToMoveToTarget(unit, target);
							else
							if(!unit->isTargetWithinMinRange(target, 96))
								function_004EB900(unit,target);
							else
							if(
								unit->status & UnitStatus::GroundedBuilding ||
								unit->orderQueueHead != NULL
							)
								unit->orderToIdle();
							else
								makeToHoldPosition_Helper(unit);

						}
						else
							makeToHoldPosition_Helper(unit);

					}

				}

			}

		}

	} //if (target != NULL) 


}

;

//Function 0047C950, used by OrderId::Move and OrderId::CarrierIgnore1
void orders_Move(CUnit* unit) {

	if (unit->mainOrderState == 0) {
		
		CUnit* target = unit->orderTarget.unit;
		bool bMoveFunctionReturn;

		if (target != NULL)
			bMoveFunctionReturn = orderToMoveToTarget(unit, target);
		else
		{

			bMoveFunctionReturn = SetMoveTarget_xy(unit, unit->orderTarget.pt.x, unit->orderTarget.pt.y);

			if (
				unit->nextTargetWaypoint.x != unit->orderTarget.pt.x ||
				unit->nextTargetWaypoint.y != unit->orderTarget.pt.y
			)
			{
				unit->nextTargetWaypoint.x = unit->orderTarget.pt.x;
				unit->nextTargetWaypoint.y = unit->orderTarget.pt.y;
			}

		}

		if (bMoveFunctionReturn)
			unit->mainOrderState = 1;

	}
	else
	if (unit->mainOrderState == 1) {

		if (
			unit->moveTarget.pt.x == unit->sprite->position.x &&
			unit->moveTarget.pt.y == unit->sprite->position.y
			//ignore bugged UnitStatus::Unmovable check
		)
		{
			unit->orderToIdle();
			if (unit->pAI != NULL)
				unit->autoTargetUnit = NULL;
		}

	}

}

;

//Function 004E98E0, used by OrderId::InfestMine2
void orders_MoveToInfestable(CUnit* unit) {
	
	CUnit* target = unit->orderTarget.unit;

	if (unit->mainOrderState == 0) {

		if (target == NULL)
			unit->orderToIdle();
		else
		if (!isInfestableUnit(target)) {
			get_statTxt_Str_0(unit, unit->playerId, (char*)statTxtTbl->getString(0x366)); //"Must target severely damaged Terran Command Center."
			unit->orderToIdle();
		}
		else
		if(orderToMoveToTarget(unit,target))
			unit->mainOrderState = 1;

	}
	else 
	if (unit->mainOrderState == 1) {

		if (
			unit->moveTarget.pt.x != unit->sprite->position.x ||
			unit->moveTarget.pt.y != unit->sprite->position.y
			//ignore bugged UnitStatus::Unmovable check
		)
		{
			if (target != NULL)
				function_004EB900(unit, target);
		}
		else
		{

			if (
				target != NULL &&
				isInfestableUnit(target) &&
				!(unit->status & UnitStatus::Unmovable)
			)
				unit->orderTo(OrderId::InfestMine4, target);
			else
				unit->orderToIdle();

		}

	}

}

;

} //namespace hooks

;

//-------- Helper function definitions. Do NOT modify! --------//

namespace {

const u32 Func_IsInfestable = 0x00402210;
bool isInfestableUnit(CUnit* unit) {

  static Bool32 bPreResult;
  
  __asm {
    PUSHAD
    MOV EDX, unit
    CALL Func_IsInfestable
    MOV bPreResult, EAX
    POPAD
  }

  return (bPreResult != 0);

}

;

const u32 Func_orders_Reaver = 0x004665D0;
//Hooked in hooks\orders\base_orders\attack_orders
void orders_Reaver_Helper(CUnit* unit) {
	__asm {
		PUSHAD
		MOV EDI, unit
		CALL Func_orders_Reaver
		POPAD
	}
}

;

const u32 Func_orders_Carrier = 0x004666A0;
//Hooked in hooks\orders\base_orders\attack_orders
void orders_Carrier_Helper(CUnit* unit) {
	__asm {
		PUSHAD
		MOV EDI, unit
		CALL Func_orders_Carrier
		POPAD
	}
}

;

const u32 Func_Sub474A20 = 0x00474A20;
void function_00474A20(CUnit* unit, u8 orderId, int x, int y) {
	__asm {
		PUSHAD
		MOV ECX, y
		MOV EDX, x
		PUSH unit
		MOVZX EAX, orderId
		CALL Func_Sub474A20
		POPAD
	}
}

;

const u32 Func_isTargetWithinMinMovementRange = 0x004763D0;
bool isTargetWithinMinMovementRange(CUnit* unit, CUnit* target, u32 range) {

	static Bool32 bPreResult;

	__asm {
		PUSHAD
		MOV EDI, unit
		MOV EAX, target
		MOV ECX, range
		CALL Func_isTargetWithinMinMovementRange
		MOV bPreResult, EAX
		POPAD
	}

	return (bPreResult != 0);

}

;

//Logically equivalent to function @ 0047B490
bool isUnitRescuable(CUnit* unit) {
	if (
		playerTable[unit->playerId].type == PlayerType::Rescuable ||
		playerTable[unit->playerId].type == PlayerType::Unknown0
	)
		return true;
	else
		return false;
}

;

const u32 Func_get_statTxt_Str_0 = 0x0048EF30;
u32 get_statTxt_Str_0(CUnit* unit, u32 playerId, char* message) {

	static u32 return_value;

	__asm {
		PUSHAD
		MOV EAX, message
		MOV ECX, playerId
		MOV EDX, unit
		CALL Func_get_statTxt_Str_0
		MOV EAX, return_value
		POPAD
	}

	return return_value;

}

;

const u32 Func_unitCanRechargeShields = 0x00493520;
//Hooked in hooks\recharge_shields
bool unitCanRechargeShields_Helper(CUnit* battery, CUnit* target) {

	static Bool32 bPreResult;

	__asm {
		PUSHAD
		MOV EDI, battery
		MOV EAX, target
		CALL Func_unitCanRechargeShields
		MOV bPreResult, EAX
		POPAD
	}

	return (bPreResult != 0);

}

;

const u32 Func_Sub494BB0 = 0x00494BB0;
void function_00494BB0(CUnit* unit,int x, int y) {

	__asm {
		PUSHAD
		MOV ECX, y
		MOV EDX, x
		MOV EAX, unit
		CALL Func_Sub494BB0
		POPAD
	}

}

;

const u32 Func_isTargetVisible = 0x004E5E30;
bool isTargetVisible(CUnit* unit, CUnit* target) {

	static Bool32 bPreResult;

	__asm {
		PUSHAD
		MOV ESI, unit
		MOV EAX, target
		CALL Func_isTargetVisible
		MOV bPreResult, EAX
		POPAD
	}

	return (bPreResult != 0);
	
}

;

const u32 Func_Sub4E8C20 = 0x004E8C20;
//hooked in hooks\orders\enter_nydus
bool canEnterNydusCanal_Helper(CUnit* unit, CUnit* nydusCanal) {

	static Bool32 bPreResult;

	__asm {
		PUSHAD
		MOV EAX, unit
		MOV EDX, nydusCanal
		CALL Func_Sub4E8C20
		MOV bPreResult, EAX
		POPAD
	}

	return (bPreResult != 0);

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

const u32 Func_OrdersHoldPositionSuicidal = 0x004EB5B0;
//Hooked in hooks\orders\base_orders\stopholdpos_orders
void makeToHoldPosition_Helper(CUnit* unit) {

	__asm {
		PUSHAD
		MOV ESI, unit
		CALL Func_OrdersHoldPositionSuicidal
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

	return bPreResult != 0;

}

;

} //Unnamed namespace

//End of helper functions

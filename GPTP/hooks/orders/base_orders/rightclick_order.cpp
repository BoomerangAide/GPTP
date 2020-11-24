#include "rightclick_order.h"
#include <SCBW/api.h>

//helper functions def

namespace {

u32 returnNothing2_Helper(CUnit* unit, int x, int y, CUnit** target, u32 unitId);						//55650
u32 returnNormalMove_NormalAttack_Order_Helper(CUnit* unit, int x, int y, CUnit** target, u32 unitId);	//55900
u32 returnNormalMove_NoAttack_Order_Helper(CUnit* unit, int x, int y, CUnit** target, u32 unitId);		//55E00
u32 returnNoMove_NormalAttack_Order_Helper(CUnit* unit, int x, int y, CUnit** target, u32 unitId);		//55890
u32 returnHarvest_Order_Helper(CUnit* unit, int x, int y, CUnit** target, u32 unitId);					//559C0
u32 returnHarvestAndRepair_Order_Helper(CUnit* unit, int x, int y, CUnit** target, u32 unitId);			//55CC0

void function_00455F20_Helper(CUnit* unit, u32 orderUnitType, int x, int y);			//55F20
u8 function_00455760(CUnit* unit);														//55760
void setRallyPosition_Helper(CUnit* unit, u16 x, u16 y);								//66910
void setRallyUnit_Helper(CUnit* unit, CUnit* target);									//66B40
Bool32 OrderAllowed(CUnit* unit, u16 order, u32 nationID);								//6DC20
void function_00474A20(CUnit* unit, u8 orderId, int x, int y);							//74A20
void function_00474A70(CUnit* unit, CUnit* target, u8 orderId);							//74A70
CUnit* IterateUnitsAtLocationTargetProc_FindClickedUnit(u32 unitType, Box16* coords);	//E8280
void setNextWaypoint_Sub4EB290(CUnit* unit);											//EB290
bool SetMoveTarget_xy(CUnit* unit, int x, int y);										//EB820

} //unnamed namespace

namespace hooks {

//Used by orders_RightClickAction and contains a significant amount
//of the useful code
void function00455F20(CUnit* unit, u32 orderUnitType, int x, int y) {

	Point16* const	point16_006509CC	= (Point16*)0x006509CC;
	u8* const		u8_0068C300			= (u8*)0x0068C300;

	CUnit* target;
	Box16 clickedUnitFindingAreaBox;
	u8 rightClickAction;
	u32 orderId;

	rightClickAction = function_00455760(unit);

	point16_006509CC->x = x;
	point16_006509CC->y = y;

	clickedUnitFindingAreaBox.bottom = y + 96;
	clickedUnitFindingAreaBox.left = x - 96;
	clickedUnitFindingAreaBox.right = x + 96;
	clickedUnitFindingAreaBox.top = y - 96;

	target = IterateUnitsAtLocationTargetProc_FindClickedUnit(orderUnitType, &clickedUnitFindingAreaBox);

	//was initially a switch using values from array at 0x005153FC as called functions
	if (rightClickAction == RightClickActions::NoCommand_AutoAttack || rightClickAction == RightClickActions::Unknown)
		orderId = returnNothing2_Helper(unit, x, y, &target, UnitId::None);
	else
	if(rightClickAction == RightClickActions::NormalMove_NormalAttack)
		orderId = returnNormalMove_NormalAttack_Order_Helper(unit, x, y, &target, UnitId::None);
	else
	if(rightClickAction == RightClickActions::NormalMove_NoAttack)
		orderId = returnNormalMove_NoAttack_Order_Helper(unit, x, y, &target, UnitId::None);
	else
	if(rightClickAction == RightClickActions::NoMove_NormalAttack)
		orderId = returnNoMove_NormalAttack_Order_Helper(unit, x, y, &target, UnitId::None);
	else
	if(rightClickAction == RightClickActions::Harvest)
		orderId = returnHarvest_Order_Helper(unit, x, y, &target, UnitId::None);
	else
	if(rightClickAction == RightClickActions::HarvestAndRepair)
		orderId = returnHarvestAndRepair_Order_Helper(unit, x, y, &target, UnitId::None);
	else
		orderId = returnNothing2_Helper(unit, x, y, &target, UnitId::None); //added to avoid orderId not initialized message

	if (orderId != OrderId::Nothing2) {

		if (orderId == OrderId::Attack1)
			orderId = units_dat::AttackUnitOrder[unit->id];

		if (OrderAllowed(unit, orderId, unit->playerId)) {

			if (orderId == OrderId::RallyPoint1) {
				if (unit->isFactory())
					setRallyUnit_Helper(unit, target);
			}
			else
			if (orderId == OrderId::RallyPoint2) { //5606C
				if (unit->isFactory())
					setRallyPosition_Helper(unit, x, y);
			}
			else
			if (target != NULL) {
				function_00474A70(unit, target, orderId);
				unit->orderToIdle();
			}
			else
			{
				function_00474A20(unit, orderId, x, y);
				unit->orderToIdle();
			}

		}

	}
	else
	{

		setNextWaypoint_Sub4EB290(unit);

		if (unit->nextMovementWaypoint != unit->moveTarget.pt) {
			unit->nextMovementWaypoint.x = unit->moveTarget.pt.x;
			unit->nextMovementWaypoint.y = unit->moveTarget.pt.y;
		}

		if(unit->orderQueueHead != NULL) {
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

//Some units don't get an order directly in CMDRECV_RightClick or around,
//and go through here instead.
//Possibly for under fog of war?
void orders_RightClickAction(CUnit* unit) {

	if (unit->mainOrderState == 0) {

		if (SetMoveTarget_xy(unit, unit->orderTarget.pt.x, unit->orderTarget.pt.y))
			unit->mainOrderState = 1;

		if (unit->nextMovementWaypoint != unit->orderTarget.pt) {
			unit->nextMovementWaypoint.x = unit->orderTarget.pt.x;
			unit->nextMovementWaypoint.y = unit->orderTarget.pt.y;
		}

	}
	else
	if (unit->mainOrderState == 1) {

		int x, y;
		int tileIndex;

		x = (s16)unit->orderTarget.pt.x;

		if (x < 0)
			x = (x + 31) / 32;
		else
			x = x / 32;

		y = (s16)unit->orderTarget.pt.y;

		if (y < 0)
			y = (y + 31) / 32;
		else
			y = y / 32;

		tileIndex = y * mapTileSize->width + x;

		if (((*activeTileArray)[tileIndex].visibilityFlags & (1 < unit->playerId)) == 0)
			function_00455F20_Helper(unit, unit->orderUnitType, unit->orderTarget.pt.x, unit->orderTarget.pt.y);

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

const u32 Func_Sub55F20 = 0x00455F20;
void function_00455F20_Helper(CUnit* unit, u32 orderUnitType, int x, int y) {
	__asm {
		PUSHAD
		MOV EAX, unit
		MOV EDI, y
		PUSH orderUnitType
		PUSH x
		CALL Func_Sub55F20
		POPAD
	}
}

;

//Logically Identical to function at 00455760
u8 function_00455760(CUnit* unit) {
	
	u8 result;

	if (unit == NULL)
		result = RightClickActions::NoCommand_AutoAttack;
	else {

		if (unit->id == UnitId::ZergLurker && unit->status & UnitStatus::Burrowed)
			result = RightClickActions::NoMove_NormalAttack;
		else
		{

			if (
				units_dat::RightClickAction[unit->id] != RightClickActions::NoCommand_AutoAttack ||
				!(unit->status & UnitStatus::GroundedBuilding) ||
				!(unit->isFactory())
			)
				result = units_dat::RightClickAction[unit->id];
			else
				result = RightClickActions::NormalMove_NoAttack;

		}

		if (
			unit->status & UnitStatus::IsHallucination &&
			(result == RightClickActions::Harvest || result == RightClickActions::HarvestAndRepair)
		) 
			result = RightClickActions::NormalMove_NormalAttack;

	}

	return result;

}

;

const u32 Func_setRallyPosition = 0x00466910;
//Hooked in hooks\rally_point
void setRallyPosition_Helper(CUnit* unit, u16 x, u16 y) {
	__asm {
		PUSHAD
		MOV EAX, unit
		PUSH y
		PUSH x
		CALL Func_setRallyPosition
		POPAD
	}
}

;

const u32 Func_setRallyUnit = 0x00466B40;
//Hooked in hooks\rally_point
void setRallyUnit_Helper(CUnit* unit, CUnit* target) {
	__asm {
		PUSHAD
		MOV ECX, unit
		MOV EAX, target
		CALL Func_setRallyUnit
		POPAD
	}
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

const u32 Func_Sub474A70 = 0x00474A70;
void function_00474A70(CUnit* unit, CUnit* target, u8 orderId) {

	__asm {
		PUSHAD
		MOV CL, orderId
		MOV EDX, unit
		MOV EAX, target
		CALL Func_Sub474A70
		POPAD
	}

}

;

const u32 Func_IterateUnitsAtLocationTargetProc = 0x004E8280;
//hardcoding the unit finding function used in this case
CUnit* IterateUnitsAtLocationTargetProc_FindClickedUnit(u32 unitType, Box16* coords) {

	static CUnit* return_value;

	__asm {
		PUSHAD
		PUSH unitType
		MOV EAX, coords
		MOV EBX, 0x00487A90
		CALL Func_IterateUnitsAtLocationTargetProc
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

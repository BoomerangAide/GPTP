#include "shrouded_order.h"
#include <SCBW/api.h>

//helper functions def

namespace {

void function_00474A70(CUnit* unit, CUnit* target, u8 orderId);	//74A70
CUnit* function_00487FD0(int x, int y, u32 unitId);				//87FD0
bool SetMoveTarget_xy(CUnit* unit, int x, int y);				//EB820

} //unnamed namespace

namespace hooks {

//004F6FA0  orders_Special
//Order performed with a target under the fog of war apparently.
//Can perform various actions depending on mainOrderId, possible values:
//OrderId::Attack2, OrderId::InfestMine3, OrderId::Repair2, OrderId::CarrierAttack2, OrderId::ReaverAttack2,
//OrderId::Harvest2, OrderId::FireYamatoGun2 
//NOTE: Could not test it, didn't ran into case where it's used, hopefully it's working
void orders_Special(CUnit* unit) {

	if (unit->mainOrderState == 0) {

		if (unit->status & UnitStatus::IsBuilding) {

			bool resultSetMoveTarget_xy = SetMoveTarget_xy(unit, unit->orderTarget.pt.x, unit->orderTarget.pt.y);

			if (
				unit->orderTarget.pt.x != unit->nextTargetWaypoint.x ||
				unit->orderTarget.pt.y != unit->nextTargetWaypoint.y
			)
			{
				unit->nextTargetWaypoint.x = unit->orderTarget.pt.x;
				unit->nextTargetWaypoint.y = unit->orderTarget.pt.y;
			}

			if (resultSetMoveTarget_xy)
				unit->mainOrderState = 1;

		}
		else
			unit->mainOrderState = 1;
		
	}
	else
	if(unit->mainOrderState == 1)
	{

		int x, y;

		if ((s16)unit->orderTarget.pt.x < 0)
			x = (unit->orderTarget.pt.x + 31) / 32;
		else
			x = unit->orderTarget.pt.x / 32;

		if ((s16)unit->orderTarget.pt.y < 0)
			y = (unit->orderTarget.pt.y + 31) / 32;
		else
			y = unit->orderTarget.pt.y / 32;

		if (( (*activeTileArray)[y * mapTileSize->width + x].visibilityFlags & (1 << unit->playerId) ) == 0) {

			CUnit* target;
			
			target = function_00487FD0(unit->orderTarget.pt.x, unit->orderTarget.pt.y, unit->orderUnitType);

			if (target != NULL) {

				u8 obscuredOrder = orders_dat::ObscuredOrder[unit->mainOrderId];

				if (obscuredOrder == OrderId::Attack1) {

					if (unit->subunit->isSubunit()) {
						function_00474A70(unit->subunit, target, units_dat::AttackUnitOrder[unit->subunit->id]);
						unit->subunit->orderToIdle();
					}

					function_00474A70(unit, target, units_dat::AttackUnitOrder[unit->id]);

				}
				else
					function_00474A70(unit, target, obscuredOrder);

			}

			unit->orderToIdle();

		}

	}

}

;

} //namespace hooks

;

//-------- Helper function definitions. Do NOT modify! --------//

namespace {

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
#include "spidermine.h"
#include <SCBW/api.h>

//helper functions def
namespace {

void fixTargetLocation(Point16* coords, u32 unitId);					//01FA0
void function_00495400(CUnit* unit, CUnit* target);						//95400
void turnGraphic(CUnit* unit, s32 direction);							//95F20
void function_0049B1E0(CUnit* unit);									//9B1E0
void displayLastNetErrForPlayer(u32 playerId);							//9E530
void updateUnitStrength(CUnit* unit);									//9FA40
void function_004A01F0(CUnit* unit);									//A01F0
CUnit* createUnit(u32 unitId, int x, int y, u32 playerId);				//A09D0
CUnit* AI_BestUnit_InBox(CUnit* unit, int range, u32 Func_UnitMatch);	//E87E0
void function_004E99D0(CUnit* unit);									//E99D0
void function_004E9A30(CUnit* unit);									//E9A30
void setNextWaypoint_Sub4EB290(CUnit* unit);							//EB290
void makeToHoldPosition_Helper(CUnit* unit);							//EB5B0
bool moveToTarget(CUnit* unit, CUnit* target);							//EB720
bool SetMoveTarget_xy(CUnit* unit, int x, int y);						//EB820

} //unnamed namespace

namespace hooks {

const int SPIDERMINE_BURROW_TIME = 0x3C;	//60
const int SPIDERMINE_SEEKING_RANGE = 0x240;	//576 (2.25*256)
const int SPIDERMINE_DETONATE_RANGE = 0x1E; //30

//Check used by AI_BestUnit_InBox to evaluate if target is
//a valid target (returning 1) or not (returning 0)
Bool32 SpiderMine_EnemyProc(CUnit* spiderMine, CUnit* target) {

	Bool32 return_value = 1;

	//Don't attack friendly / allied units
	if (!spiderMine->isTargetEnemy(target))
		return_value = 0;
	else
	//Don't attack invincible units
	if (target->status & UnitStatus::Invincible)
		return_value = 0;
	else
	//Don't attack air units / buildings
	if (target->status & (UnitStatus::InAir | UnitStatus::GroundedBuilding))
		return_value = 0;
	else
	//Don't attack hovering units
	if (units_dat::MovementFlags[target->id] == MovementFlags::HoverUnit)
		return_value = 0;

	return return_value;

}

;

//Order performed by the spider mine
void orders_VultureMine(CUnit* unit) {

	if(unit->mainOrderState == 0) {
		unit->groundWeaponCooldown = SPIDERMINE_BURROW_TIME;
		unit->mainOrderState = 1;
	}

	if(unit->mainOrderState == 1) {
		
		if(unit->groundWeaponCooldown == 0) {
			unit->sprite->playIscriptAnim(IscriptAnimation::Burrow,true);
			unit->status |= UnitStatus::NoCollide;
			unit->mainOrderState = 2;
		}

	}

	if(unit->mainOrderState == 2) {

		if(unit->orderSignal & 4) {

			unit->orderSignal -= 4;

			if(!(unit->status & UnitStatus::IsHallucination)) {
				function_004E9A30(unit); //related to movement
				function_0049B1E0(unit); //related to cloaking/stealth
			}

			unit->mainOrderState = 3;

		}

	}

	if(unit->mainOrderState == 3) {

		CUnit* target;

		//hardcoding function @ 00441270 SpiderMine_AcquireTarget
		if (unit->status & UnitStatus::CanNotAttack)
			target = NULL;
		else
			target = AI_BestUnit_InBox(unit, 32 * unit->getSeekRange(), 0x00440EC0);
		//end of hardcoding

		if(target != NULL) {

			unit->orderTarget.unit = target;

			unit->sprite->playIscriptAnim(IscriptAnimation::UnBurrow,true);

			//disable burrowed flag if still on
			unit->sprite->flags &= ~CSprite_Flags::Burrowed;

			unit->setSecondaryOrder(OrderId::Nothing2);

			unit->mainOrderState = 4;

		}

	}

	if(unit->mainOrderState == 4) {

		if(unit->orderSignal & 4) {

			unit->orderSignal -= 4;

			unit->sprite->elevationLevel = units_dat::Elevation[unit->id];
			
			//disabled NoCollide flag if was still on
			unit->status &= ~UnitStatus::NoCollide;

			if(unit->orderTarget.unit != NULL) {

				//probably trigger movement/acceleration of unit
				//toward the target
				function_00495400(unit,unit->orderTarget.unit);

				unit->mainOrderState = 5;

			}
			else
				unit->mainOrderState = 1;

		}

	}
	else //unlike the previous states, going directly from state 4 to 5 in the same frame is impossible
	if(unit->mainOrderState == 5) {

		bool bIsCompletelyInRange = false;
		bool bEndThere = false;

		function_004E99D0(unit); //related to movement, similar to function_004E9A30

		if(unit->orderTarget.unit != NULL) {

			if(unit->isTargetWithinMinRange(unit->orderTarget.unit,SPIDERMINE_SEEKING_RANGE)) {

				//probably trigger movement/acceleration of unit
				//toward the target
				function_00495400(unit,unit->orderTarget.unit);

				if(unit->isTargetWithinMinRange(unit->orderTarget.unit,SPIDERMINE_DETONATE_RANGE))
					bIsCompletelyInRange = true; //jump to 63F22
				else
				if(unit->getMovableState() != MovableState::UnmovableAtDestination)
					bEndThere = true;

			}

		}

		if(!bEndThere && !bIsCompletelyInRange) { //63F14
			setNextWaypoint_Sub4EB290(unit);
			unit->mainOrderState = 1;
		}

		if(bIsCompletelyInRange) { //63F22

			CImage* current_image;

			unit->orderTarget.unit = NULL;
			unit->orderTarget.pt.x = unit->sprite->position.x;
			unit->orderTarget.pt.y = unit->sprite->position.y;

			current_image = unit->sprite->images.head;

			while(current_image != NULL) {
				current_image->playIscriptAnim(IscriptAnimation::SpecialState1);
				current_image = current_image->link.next;
			}

			unit->mainOrderState = 6;
			
		}

	}

	//unlike state 5 with state 4, state 6 can be chained from
	//state 5, so no 'else' here
	if( (unit->mainOrderState == 6) && (unit->orderSignal & 1) )
		unit->remove();
	
}

;

//Order of the Vulture to place a mine
void orders_PlaceMine(CUnit* unit) {

	if(unit->mainOrderState == 0) {

		CUnit* target = unit->orderTarget.unit;

		if (target != NULL) {

			if (moveToTarget(unit, target)) {

				if (unit->nextMovementWaypoint != target->sprite->position) {
					unit->nextMovementWaypoint.x = target->sprite->position.x;
					unit->nextMovementWaypoint.y = target->sprite->position.y;
				}

				unit->orderTarget.pt.x = target->sprite->position.x;
				unit->orderTarget.pt.y = target->sprite->position.y;

				unit->mainOrderState = 1;

			}
			else
			if (unit->nextMovementWaypoint != unit->sprite->position) {
				unit->nextMovementWaypoint.x = unit->sprite->position.x;
				unit->nextMovementWaypoint.y = unit->sprite->position.y;
			}

		}
		else
		{

			if (SetMoveTarget_xy(unit, unit->orderTarget.pt.x, unit->orderTarget.pt.y)) {

				if (unit->nextMovementWaypoint != unit->orderTarget.pt) {
					unit->nextMovementWaypoint.x = unit->orderTarget.pt.x;
					unit->nextMovementWaypoint.y = unit->orderTarget.pt.y;
				}

				unit->mainOrderState = 1;

			}
			else
			if (unit->nextMovementWaypoint != unit->orderTarget.pt) {
				unit->nextMovementWaypoint.x = unit->orderTarget.pt.x;
				unit->nextMovementWaypoint.y = unit->orderTarget.pt.y;
			}

		}

	}
	else
	{

		if (
			!unit->canUseTech(TechId::SpiderMines,unit->playerId) ||
			(
				unit->sprite->position == unit->moveTarget.pt &&
				unit->status & UnitStatus::Unmovable
			)
		)
		{
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
		else
		{

			int distance = scbw::getDistanceFast(
				unit->halt.x,
				unit->halt.y,
				unit->orderTarget.pt.x * 256,
				unit->orderTarget.pt.y * 256
			);

			if (((int)(distance / 256)) > 20) {
				if (unit->pAI != NULL && unit->getMovableState() != MovableState::NotReachedDestination)
					unit->orderToIdle();
			}
			else
			{

				Point16 mine_position;
				CUnit* spiderMine;

				makeToHoldPosition_Helper(unit);

				mine_position.x = unit->orderTarget.pt.x;
				mine_position.y = unit->orderTarget.pt.y;
				fixTargetLocation(&mine_position, UnitId::TerranVultureSpiderMine);

				spiderMine = createUnit(UnitId::TerranVultureSpiderMine, mine_position.x, mine_position.y, unit->playerId);

				if (spiderMine == NULL) {
					displayLastNetErrForPlayer(unit->playerId);
					unit->orderToIdle();
				}
				else
				{

					//update various stuff (set hp, set shield...) not finished on Create
					function_004A01F0(spiderMine);
					updateUnitStrength(spiderMine);

					turnGraphic(spiderMine, unit->currentDirection1);

					unit->vulture.spiderMineCount -= 1;

					unit->orderToIdle();

				}

			}

		}

	}

}

;

} //hooks

;

//-------- Helper function definitions. Do NOT modify! --------//

namespace {

const u32 Func_fixTargetLocation = 0x00401FA0;
void fixTargetLocation(Point16* coords, u32 unitId) {

	__asm {
		PUSHAD
		MOV EAX, unitId
		MOV EDX, coords
		CALL Func_fixTargetLocation
		POPAD
	}

}

;

const u32 Func_Sub495400 = 0x00495400;
void function_00495400(CUnit* unit, CUnit* target) {
	__asm {
		PUSHAD
		MOV EDI, unit
		MOV EAX, target
		CALL Func_Sub495400
		POPAD
	}
}

;

const u32 Func_turnGraphic = 0x00495F20;
void turnGraphic(CUnit* unit, s32 direction) {
	__asm {
		PUSHAD
		MOV EAX, unit
		MOV EBX, direction
		CALL Func_turnGraphic
		POPAD
	}
}

;

const u32 Func_Sub49B1E0 = 0x0049B1E0;
void function_0049B1E0(CUnit* unit) {
	__asm {
		PUSHAD
		MOV EDI, unit
		CALL Func_Sub49B1E0
		POPAD
	}
}

;

const u32 Func_displayLastNetErrForPlayer = 0x0049E530;
void displayLastNetErrForPlayer(u32 playerId) {
	__asm {
		PUSHAD
		PUSH playerId
		CALL Func_displayLastNetErrForPlayer
		POPAD
	}
}

;

const u32 Func_UpdateUnitStrength = 0x0049FA40;
void updateUnitStrength(CUnit* unit) {

	__asm {
		PUSHAD
		MOV EAX, unit
		CALL Func_UpdateUnitStrength
		POPAD
	}

}

;

const u32 Func_Sub4A01F0 = 0x004A01F0;
void function_004A01F0(CUnit* unit) {

	__asm {
		PUSHAD
		MOV EAX, unit
		CALL Func_Sub4A01F0
		POPAD
	}

}

;

const u32 Func_CreateUnit = 0x004A09D0;
CUnit* createUnit(u32 unitId, int x, int y, u32 playerId) {

	static CUnit* unit_created;

	__asm {
		PUSHAD
		PUSH playerId
		PUSH y
		MOV ECX, unitId
		MOV EAX, x
		CALL Func_CreateUnit
		MOV unit_created, EAX
		POPAD
	}

	return unit_created;

}

;

const u32 Func_AI_BestUnit_InBox = 0x004E87E0;
CUnit* AI_BestUnit_InBox(CUnit* unit, int range, u32 Func_UnitMatch) {

	static CUnit* result;

	__asm {
		PUSHAD
		MOV ECX, unit
		MOV EAX, range
		PUSH unit
		PUSH Func_UnitMatch
		CALL Func_AI_BestUnit_InBox
		MOV result, EAX
		POPAD
	}

	return result;

}

;

const u32 Func_Sub4E99D0 = 0x004E99D0;
void function_004E99D0(CUnit* unit) {
	__asm {
		PUSHAD
		MOV EAX, unit
		CALL Func_Sub4E99D0
		POPAD
	}
}

;

const u32 Func_Sub4E9A30 = 0x004E9A30;
void function_004E9A30(CUnit* unit) {
	__asm {
		PUSHAD
		MOV EAX, unit
		CALL Func_Sub4E9A30
		POPAD
	}
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

} //Unnamed namespace

//End of helper functions

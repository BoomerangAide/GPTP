#include "spidermine.h"
#include <SCBW/api.h>

//helper functions def
namespace {
	void function_00495400(CUnit* unit, CUnit* target);						//95400
	void function_0049B1E0(CUnit* unit);									//9B1E0
	CUnit* AI_BestUnit_InBox(CUnit* unit, int range, u32 Func_UnitMatch);	//E87E0
	void function_004E99D0(CUnit* unit);									//E99D0
	void function_004E9A30(CUnit* unit);									//E9A30
	void setNextWaypoint_Sub4EB290(CUnit* unit);							//EB290
} //unnamed namespace

namespace hooks {

const int SPIDERMINE_BURROW_TIME = 0x3C;
const int SPIDERMINE_SEEKING_RANGE = 0x240;
const int SPIDERMINE_DETONATE_RANGE = 0x1E;

//Check used by AI_BestUnit_InBox to evaluate if target is
//a valid target (returnning 1) or not (returning 0
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
				if(unit->getMovableState() != 2) //unit not unmovable
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

} //hooks

;

//-------- Helper function definitions. Do NOT modify! --------//

namespace {

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

} //Unnamed namespace

//End of helper functions

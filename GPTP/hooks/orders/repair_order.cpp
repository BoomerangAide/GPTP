#include "repair_order.h"
#include <SCBW/api.h>
#include <Events/Events.h>

//helper functions def

namespace {

bool isUnitPositions2Equal(CUnit* unit);																	//02160
void getRepairInfo(CUnit* repairTarget, u32* repairCostMineral, u32* repairCostGas, s16* unkRepairInfo);	//66D10
bool verifyResources(u32 mineralCost, u32 gasCost, u32 playerId);											//66EE0
bool ord_repair_subtract(CUnit* worker, CUnit* target);														//66FB0
void unitOrderMoveToTargetUnit(CUnit* unit, CUnit* target);													//79FE0
void refreshLayer3And4();																					//8D9A0
bool function_0048DDA0();																					//8DDA0
void function_0048E310();																					//8E310
void function_00494BB0(CUnit* unit,int x, int y);															//94BB0
bool function_004EB900(CUnit* unit, CUnit* target);															//EB900
bool orderToMoveToTarget(CUnit* unit, CUnit* target);														//EB980

} //unnamed namespace

namespace hooks {

#ifdef EVENTS_SYSTEM
void orders_Repair1(CUnit* unit) {

	bool jump_to_676C4 = false; //handle various early invalid cases
	bool bEndThere = false;

	CUnit* target = unit->orderTarget.unit;

	if(target == NULL)
		jump_to_676C4 = true;
	else
	if(target->hitPoints >= units_dat::MaxHitPoints[target->id])
		jump_to_676C4 = true;
	else
	if(target->stasisTimer != 0)
		jump_to_676C4 = true;
	else
	if(unit->pAI != NULL && target->status & UnitStatus::IsBuilding && target->orderTarget.unit != unit)
		jump_to_676C4 = true;
	else
	if(target->status & UnitStatus::InTransport)
		jump_to_676C4 = true;
	else
	if(!(units_dat::BaseProperty[target->id] & UnitProperty::Mechanical))
		jump_to_676C4 = true;
	else
	if(!(target->status & UnitStatus::Completed))
		jump_to_676C4 = true;

	if(!jump_to_676C4) {

		if(
			units_dat::GroupFlags[target->id].isZerg || 
			units_dat::GroupFlags[target->id].isProtoss ||
			!(units_dat::GroupFlags[target->id].isTerran)
		)
		{
			unit->sprite->playIscriptAnim(IscriptAnimation::WalkingToIdle,true);
			unit->orderToIdle();
			bEndThere = true;
		}

	}

	if(!bEndThere) {

		bool jump_to_default_switch;

		if(!jump_to_676C4) {

			jump_to_default_switch = ((unit->mainOrderState > 8) || (unit->mainOrderState >= 2 && unit->mainOrderState <= 5));

			if(!jump_to_default_switch && unit->mainOrderState == 0) {

				s16 unkRepairInfo = 0;
				u32 repairCostMinerals = 0, repairCostGas = 0;

				getRepairInfo(target,&repairCostMinerals,&repairCostGas,&unkRepairInfo);

				verifyResources(repairCostMinerals,repairCostGas,unit->playerId);

				unit->worker.repairResourceLossTimer = 0;
				unit->mainOrderState = 1; //continue directly into this case

			} //if(!jump_to_default_switch && unit->mainOrderState == 0)

			if(!jump_to_default_switch && unit->mainOrderState == 1) {

				if(!(target->status & UnitStatus::Completed)) {

					unit->orderTo(OrderId::ConstructingBuilding,target);

					if(*IS_PLACING_BUILDING) {

						if(!function_0048DDA0()) {
							refreshLayer3And4();
							function_0048E310();
						}

					}

					bEndThere = true;

				}
				else {

					if(orderToMoveToTarget(unit,target)) {
						unitOrderMoveToTargetUnit(unit,target);
						unit->mainOrderState = 6; //don't continue directly into this case
					}

					bEndThere = true;

				}

			} //if(!jump_to_default_switch && unit->mainOrderState == 1)

			if(!bEndThere && !jump_to_default_switch && unit->mainOrderState == 6) {

				if(unit->getMovableState() == MovableState::NotReachedDestination) {
					function_004EB900(unit,target); //may cause unit to move or become idle
					bEndThere = true;
				}
				else {

					if(!(unit->isTargetWithinMinRange(target, 5))) {
						unit->mainOrderState = 1; //don't continue directly into this case
						bEndThere = true;
					}
					else {
						function_00494BB0(unit,target->sprite->position.x,target->sprite->position.y);
						unitOrderMoveToTargetUnit(unit,target);
						unit->mainOrderState = 7; //don't continue directly into this case
						jump_to_default_switch = true;
					}

				}

			} //if(!bEndThere && !jump_to_default_switch && unit->mainOrderState == 6)

			if(!bEndThere && !jump_to_default_switch && unit->mainOrderState == 7) {

				if(!(unit->isTargetWithinMinRange(target, 5))) {
					unit->mainOrderState = 1;
					bEndThere = true;
				}
				else {

					function_00494BB0(unit,target->sprite->position.x,target->sprite->position.y);
					unitOrderMoveToTargetUnit(unit,target);

					if(!isUnitPositions2Equal(unit)) {

						s32 angleReturned, currentAngle;
						u8 weaponId;

						angleReturned = 
							scbw::getAngle(
								unit->sprite->position.x,
								unit->sprite->position.y,
								unit->nextTargetWaypoint.x,
								unit->nextTargetWaypoint.y
							);

						currentAngle = unit->currentDirection1 - angleReturned;

						if(currentAngle < 0)
							currentAngle += 256;

						if(currentAngle > 128)
							currentAngle = 256 - currentAngle;
				
						weaponId = unit->getGroundWeapon();

						if(currentAngle > weapons_dat::AttackAngle[weaponId])
							jump_to_default_switch = true;
						else {
							unit->sprite->playIscriptAnim(IscriptAnimation::AlmostBuilt,true);
							unit->mainOrderState = 8; //don't continue directly into this case
							jump_to_default_switch = true;
						}

					} //if(!isUnitPositions2Equal(unit))
					else {
						unit->sprite->playIscriptAnim(IscriptAnimation::AlmostBuilt,true);
						unit->mainOrderState = 8; //don't continue directly into this case
						jump_to_default_switch = true;
					}

				} //if(unit->isTargetWithinMinRange(target, 5))

			} //if(!bEndThere && !jump_to_default_switch && unit->mainOrderState == 7)

			if(!bEndThere && !jump_to_default_switch && unit->mainOrderState == 8) {

				function_00494BB0(unit,target->sprite->position.x,target->sprite->position.y);
				unitOrderMoveToTargetUnit(unit,target);

				if(unit->isTargetWithinMinRange(target,5) && target->mainOrderId != OrderId::Liftoff)
					jump_to_default_switch = true;
				else {

					CImage* current_image = unit->sprite->images.head;

					while(current_image != NULL) {
						current_image->playIscriptAnim(IscriptAnimation::GndAttkToIdle);
						current_image = current_image->link.next;
					}

					unit->mainOrderState = 1; //don't continue directly into this case
					jump_to_default_switch = true;

				}

			} //if(!bEndThere && !jump_to_default_switch && unit->mainOrderState == 8)

			if(!bEndThere) { //jump_to_default_switch CASE

				bool bDoRepair = false;

				if(unit->worker.repairResourceLossTimer == 0) {

					unit->worker.repairResourceLossTimer--;
					bDoRepair = ord_repair_subtract(unit,target);

					if(!bDoRepair) {
							
						CImage* current_image = unit->sprite->images.head;

						while(current_image != NULL) {
							current_image->playIscriptAnim(IscriptAnimation::WalkingToIdle);
							current_image = current_image->link.next;
						}

						unit->orderToIdle();
						bEndThere = true;

					}

				}
				else {
					unit->worker.repairResourceLossTimer--;
					bDoRepair = true;
				}

				if(bDoRepair) { // //67692

					std::vector<int*> events_building_repaired_hp_add_arg(4);
					s32 newHp;

					if (*CHEAT_STATE & CheatFlags::OperationCwal)
						newHp = target->hitPoints + target->buildRepairHpGain * 16;
					else
						newHp = target->hitPoints + target->buildRepairHpGain;

					events_building_repaired_hp_add_arg[0] = (int*)target;
					events_building_repaired_hp_add_arg[1] = (int*)unit;
					events_building_repaired_hp_add_arg[2] = (int*)target->hitPoints;
					events_building_repaired_hp_add_arg[3] = (int*)&newHp;

					EventManager::EventCalled(EventId::BUILDING_TERRAN_REPAIRED_HP_ADD, events_building_repaired_hp_add_arg);

					//the function can fix overHP
					target->setHp(newHp);

					if(target->hitPoints < units_dat::MaxHitPoints[target->id])
						bEndThere = true;
					else
						jump_to_676C4 = true;

				}

			}

		} //if(!jump_to_676C4)

		if(!bEndThere) { //jump_to_676C4 CASE

			CImage* current_image = unit->sprite->images.head;

			while(current_image != NULL) {
				current_image->playIscriptAnim(IscriptAnimation::WalkingToIdle);
				current_image = current_image->link.next;
			}

			if(unit->orderQueueHead != NULL) {
				unit->userActionFlags |= 1;
				prepareForNextOrder(unit);
			}
			else { //676FE

				u8 orderId;

				if(unit->pAI == NULL)
					orderId = units_dat::ReturnToIdleOrder[unit->id];
				else
					orderId = OrderId::ComputerAI;

				unit->orderComputerCL(orderId);

			}

			//6771B

			if(target != NULL && !(target->status & UnitStatus::InTransport))
				target->connectedUnit = NULL;

		}

	} //if(!bEndThere)

} //void orders_Repair1(CUnit* unit)
#else
void orders_Repair1(CUnit* unit) {

	bool jump_to_676C4 = false; //handle various early invalid cases
	bool bEndThere = false;

	CUnit* target = unit->orderTarget.unit;

	if(target == NULL)
		jump_to_676C4 = true;
	else
	if(target->hitPoints >= units_dat::MaxHitPoints[target->id])
		jump_to_676C4 = true;
	else
	if(target->stasisTimer != 0)
		jump_to_676C4 = true;
	else
	if(unit->pAI != NULL && target->status & UnitStatus::IsBuilding && target->orderTarget.unit != unit)
		jump_to_676C4 = true;
	else
	if(target->status & UnitStatus::InTransport)
		jump_to_676C4 = true;
	else
	if(!(units_dat::BaseProperty[target->id] & UnitProperty::Mechanical))
		jump_to_676C4 = true;
	else
	if(!(target->status & UnitStatus::Completed))
		jump_to_676C4 = true;

	if(!jump_to_676C4) {

		if(
			units_dat::GroupFlags[target->id].isZerg || 
			units_dat::GroupFlags[target->id].isProtoss ||
			!(units_dat::GroupFlags[target->id].isTerran)
		)
		{
			unit->sprite->playIscriptAnim(IscriptAnimation::WalkingToIdle,true);
			unit->orderToIdle();
			bEndThere = true;
		}

	}

	if(!bEndThere) {

		bool jump_to_default_switch;

		if(!jump_to_676C4) {

			jump_to_default_switch = ((unit->mainOrderState > 8) || (unit->mainOrderState >= 2 && unit->mainOrderState <= 5));

			if(!jump_to_default_switch && unit->mainOrderState == 0) {

				s16 unkRepairInfo = 0;
				u32 repairCostMinerals = 0, repairCostGas = 0;

				getRepairInfo(target,&repairCostMinerals,&repairCostGas,&unkRepairInfo);

				verifyResources(repairCostMinerals,repairCostGas,unit->playerId);

				unit->worker.repairResourceLossTimer = 0;
				unit->mainOrderState = 1; //continue directly into this case

			} //if(!jump_to_default_switch && unit->mainOrderState == 0)

			if(!jump_to_default_switch && unit->mainOrderState == 1) {

				if(!(target->status & UnitStatus::Completed)) {

					unit->orderTo(OrderId::ConstructingBuilding,target);

					if(*IS_PLACING_BUILDING) {

						if(!function_0048DDA0()) {
							refreshLayer3And4();
							function_0048E310();
						}

					}

					bEndThere = true;

				}
				else {

					if(orderToMoveToTarget(unit,target)) {
						unitOrderMoveToTargetUnit(unit,target);
						unit->mainOrderState = 6; //don't continue directly into this case
					}

					bEndThere = true;

				}

			} //if(!jump_to_default_switch && unit->mainOrderState == 1)

			if(!bEndThere && !jump_to_default_switch && unit->mainOrderState == 6) {

				if(unit->getMovableState() == MovableState::NotReachedDestination) {
					function_004EB900(unit,target); //may cause unit to move or become idle
					bEndThere = true;
				}
				else {

					if(!(unit->isTargetWithinMinRange(target, 5))) {
						unit->mainOrderState = 1; //don't continue directly into this case
						bEndThere = true;
					}
					else {
						function_00494BB0(unit,target->sprite->position.x,target->sprite->position.y);
						unitOrderMoveToTargetUnit(unit,target);
						unit->mainOrderState = 7; //don't continue directly into this case
						jump_to_default_switch = true;
					}

				}

			} //if(!bEndThere && !jump_to_default_switch && unit->mainOrderState == 6)

			if(!bEndThere && !jump_to_default_switch && unit->mainOrderState == 7) {

				if(!(unit->isTargetWithinMinRange(target, 5))) {
					unit->mainOrderState = 1;
					bEndThere = true;
				}
				else {

					function_00494BB0(unit,target->sprite->position.x,target->sprite->position.y);
					unitOrderMoveToTargetUnit(unit,target);

					if(!isUnitPositions2Equal(unit)) {

						s32 angleReturned, currentAngle;
						u8 weaponId;

						angleReturned = 
							scbw::getAngle(
								unit->sprite->position.x,
								unit->sprite->position.y,
								unit->nextTargetWaypoint.x,
								unit->nextTargetWaypoint.y
							);

						currentAngle = unit->currentDirection1 - angleReturned;

						if(currentAngle < 0)
							currentAngle += 256;

						if(currentAngle > 128)
							currentAngle = 256 - currentAngle;
				
						weaponId = unit->getGroundWeapon();

						if(currentAngle > weapons_dat::AttackAngle[weaponId])
							jump_to_default_switch = true;
						else {
							unit->sprite->playIscriptAnim(IscriptAnimation::AlmostBuilt,true);
							unit->mainOrderState = 8; //don't continue directly into this case
							jump_to_default_switch = true;
						}

					} //if(!isUnitPositions2Equal(unit))
					else {
						unit->sprite->playIscriptAnim(IscriptAnimation::AlmostBuilt,true);
						unit->mainOrderState = 8; //don't continue directly into this case
						jump_to_default_switch = true;
					}

				} //if(unit->isTargetWithinMinRange(target, 5))

			} //if(!bEndThere && !jump_to_default_switch && unit->mainOrderState == 7)

			if(!bEndThere && !jump_to_default_switch && unit->mainOrderState == 8) {

				function_00494BB0(unit,target->sprite->position.x,target->sprite->position.y);
				unitOrderMoveToTargetUnit(unit,target);

				if(unit->isTargetWithinMinRange(target,5) && target->mainOrderId != OrderId::Liftoff)
					jump_to_default_switch = true;
				else {

					CImage* current_image = unit->sprite->images.head;

					while(current_image != NULL) {
						current_image->playIscriptAnim(IscriptAnimation::GndAttkToIdle);
						current_image = current_image->link.next;
					}

					unit->mainOrderState = 1; //don't continue directly into this case
					jump_to_default_switch = true;

				}

			} //if(!bEndThere && !jump_to_default_switch && unit->mainOrderState == 8)

			if(!bEndThere) { //jump_to_default_switch CASE

				bool bDoRepair = false;

				if(unit->worker.repairResourceLossTimer == 0) {

					unit->worker.repairResourceLossTimer--;
					bDoRepair = ord_repair_subtract(unit,target);

					if(!bDoRepair) {
							
						CImage* current_image = unit->sprite->images.head;

						while(current_image != NULL) {
							current_image->playIscriptAnim(IscriptAnimation::WalkingToIdle);
							current_image = current_image->link.next;
						}

						unit->orderToIdle();
						bEndThere = true;

					}

				}
				else {
					unit->worker.repairResourceLossTimer--;
					bDoRepair = true;
				}

				if(bDoRepair) { // //67692

					u32 hpGain = target->buildRepairHpGain;

					if(*CHEAT_STATE & CheatFlags::OperationCwal)
						hpGain *= 16;

					//the function can fix overHP
					target->setHp(target->hitPoints + hpGain);

					if(target->hitPoints < units_dat::MaxHitPoints[target->id])
						bEndThere = true;
					else
						jump_to_676C4 = true;

				}

			}

		} //if(!jump_to_676C4)

		if(!bEndThere) { //jump_to_676C4 CASE

			CImage* current_image = unit->sprite->images.head;

			while(current_image != NULL) {
				current_image->playIscriptAnim(IscriptAnimation::WalkingToIdle);
				current_image = current_image->link.next;
			}

			if(unit->orderQueueHead != NULL) {
				unit->userActionFlags |= 1;
				prepareForNextOrder(unit);
			}
			else { //676FE

				u8 orderId;

				if(unit->pAI == NULL)
					orderId = units_dat::ReturnToIdleOrder[unit->id];
				else
					orderId = OrderId::ComputerAI;

				unit->orderComputerCL(orderId);

			}

			//6771B

			if(target != NULL && !(target->status & UnitStatus::InTransport))
				target->connectedUnit = NULL;

		}

	} //if(!bEndThere)

} //void orders_Repair1(CUnit* unit)
#endif

;

} //namespace hooks

;

//-------- Helper function definitions. Do NOT modify! --------//

namespace {

//Equivalent to function @ 0x00402160
bool isUnitPositions2Equal(CUnit* unit) {

	return (
		unit->nextTargetWaypoint.x == unit->position.x &&
		unit->nextTargetWaypoint.y == unit->position.y
	);

}

;

const u32 Func_getRepairInfo = 0x00466D10;
//unkRepairInfo may be related to unit->worker.repairResourceLossTimer
void getRepairInfo(CUnit* repairTarget, u32* repairCostMineral, u32* repairCostGas, s16* unkRepairInfo) {

	__asm {
		PUSHAD
		MOV EAX, repairTarget
		PUSH unkRepairInfo
		PUSH repairCostGas
		PUSH repairCostMineral
		CALL Func_getRepairInfo
		POPAD
	}

}

;

const u32 Func_ord_repair_subtract = 0x00466FB0;
bool ord_repair_subtract(CUnit* worker, CUnit* target) {

	static Bool32 bPreResult;

	__asm {
		PUSHAD
		MOV ESI, worker
		PUSH target
		CALL Func_ord_repair_subtract
		MOV bPreResult, EAX
		POPAD
	}

	return (bPreResult != 0);

}

;

const u32 Func_verifyResources = 0x00466EE0;
//display an error message and return false if not enough resources
bool verifyResources(u32 mineralCost, u32 gasCost, u32 playerId) {

	Bool32 bPreResult;

	__asm {
		PUSHAD
		MOV EAX, mineralCost
		PUSH gasCost
		MOV ECX, playerId
		CALL Func_verifyResources
		MOV bPreResult, EAX
		POPAD
	}

	return (bPreResult != 0);

}

;

const u32 Func_unitOrderMoveToTargetUnit = 0x00479FE0;
void unitOrderMoveToTargetUnit(CUnit* unit, CUnit* target) {

	__asm {
		PUSHAD
		MOV EAX, unit
		MOV ECX, target
		CALL Func_unitOrderMoveToTargetUnit
		POPAD
	}

}

;

const u32 Func_RefreshLayer3And4 = 0x0048D9A0;
void refreshLayer3And4() {

	__asm {
		PUSHAD
		CALL Func_RefreshLayer3And4
		POPAD
	}

}

;

const u32 Func_Sub48DDA0 = 0x0048DDA0;
bool function_0048DDA0() {

	Bool32 pre_return_value;

	__asm {
		PUSHAD
		CALL Func_Sub48DDA0
		MOV pre_return_value, EAX
		POPAD
	}

	return (pre_return_value != 0);

}

;

const u32 Func_Sub48E310 = 0x0048E310;
void function_0048E310() {

	__asm {
		PUSHAD
		CALL Func_Sub48E310
		POPAD
	}

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

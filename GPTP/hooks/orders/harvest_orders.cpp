#include "harvest_orders.h"
#include <SCBW/api.h>
#include <logger.h>

//helper functions def
namespace {

bool unitIsActiveResourceDepot(CUnit* unit);										//01CF0
bool getCloserToDestination_sub402BE0(CUnit* unit);									//02BE0
bool unitTypeIsHatcheryType(CUnit* unit);											//02C20
CUnit* FindBestReturnCargoTarget(CUnit* unit);										//41310
CUnit* harvestNextNearestResources(CUnit* unit, Point16 pos);						//42F70
CUnit* harvestNextNearestResourcesEx(CUnit* unit, Point16 pos, CUnit* resource);	//432D0
void function_004686D0(CUnit* unit, CUnit* target);									//686D0
bool unit_isGeyserUnitEx(CUnit* unit, CUnit* target);								//68930
CUnit* function_00468970(CUnit* unit);												//68970
void function_00468AA0(CUnit* unit);												//68AA0
bool function_00468B00(CUnit* unit, CUnit* target, Point16* pos, s8* angle);		//68B00
bool function_00468C70(CUnit* resource, CUnit* unit);								//68C70
void function_00468D10(CUnit* unit, CUnit* target);									//68D10
void function_00468E40(CUnit* unit, CUnit* target);									//68E40
void transferResourceToWorker_Helper(CUnit* worker, CUnit* resource);				//696D0
void removeOrderFromUnitQueue(CUnit* unit, COrder* order);							//742D0
void function_00474400(CUnit* unit, u8 orderId);									//74400
void function_00474760(CUnit* unit, COrder* order, u8 orderId);						//74760
void function_00474A70(CUnit* unit, CUnit* target, u8 orderId);						//74A70
void function_00474AF0(CUnit* unit, CUnit* target, u8 orderId, Point16 pos);		//74AF0	
void orderImmediate(CUnit* unit, u8 order);											//74B40
void actUnitReturnToIdle(CUnit* unit);												//75420
void function_00494BB0(CUnit* unit,int x, int y);									//94BB0
void turnGraphic(CUnit* unit, s32 direction);										//95F20
void showAndEnableUnit(CUnit* unit);												//E6490
void function_004E92B0(CUnit* unit, CUnit* target);									//E92B0
void setNextWaypoint_Sub4EB290(CUnit* unit);										//EB290
void makeToHoldPosition_Helper(CUnit* unit);										//EB5B0
bool orderToMoveToTarget(CUnit* unit, CUnit* target);								//EB980
void function_004F39A0(CUnit* unit);												//F39A0

} //unnamed namespace

namespace hooks {

void orders_Harvest3(CUnit* unit) {

	if (unit->worker.isCarryingSomething) 
	{

		if (unit->worker.harvestTarget != NULL) {

			CUnit* oldHarvestTarget = unit->worker.harvestTarget;
			CUnit* r_function_00468970;

			unit->worker.harvestTarget = NULL;
			unit->worker.isCarryingSomething = false;

			oldHarvestTarget->building.resource.gatherQueueCount -= 1;

			r_function_00468970 = function_00468970(oldHarvestTarget);

			if (r_function_00468970 != NULL)
				function_00468C70(oldHarvestTarget, r_function_00468970);

		}
		else
			unit->worker.isCarryingSomething = false;

	}

	actUnitReturnToIdle(unit);
	
}

;

void orders_Harvest4(CUnit* unit) {
	
	if (unit->worker.harvestTarget != NULL)
		function_004686D0(unit, unit->worker.harvestTarget);

	actUnitReturnToIdle(unit);

}

;

void orders_HarvestMinerals2(CUnit* unit) {

	CUnit* target = unit->orderTarget.unit;

	if (
		target == NULL ||
		target->id < UnitId::ResourceMineralField ||
		target->id > UnitId::ResourceMineralFieldType3
	)
	{

		unit->userActionFlags |= 1;

		if (unit->mainOrderId != OrderId::Die) {
			
			while (
				unit->orderQueueTail != NULL &&
				(
					orders_dat::CanBeInterrupted[unit->orderQueueTail->orderId] != 0 ||
					unit->orderQueueTail->orderId == OrderId::MoveToMinerals
				)
			)
				removeOrderFromUnitQueue(unit, unit->orderQueueTail);

			unit->performAnotherOrder(OrderId::MoveToMinerals, 0, 0, NULL, UnitId::None);

		}

		prepareForNextOrder(unit);

	}
	else
	if(unit->mainOrderState == 0)
		function_00468D10(unit,target);
	
}

;

//Function 00469000 used by OrderId::HarvestGas2
void orders_HarvestGas2(CUnit* unit) {
	
	CUnit* target = unit->orderTarget.unit;

	if (
		target == NULL ||
		!(target->status & UnitStatus::Completed) ||
		(target->id != UnitId::TerranRefinery && target->id != UnitId::ProtossAssimilator && target->id != UnitId::ZergExtractor) ||
		target->playerId != unit->playerId ||
		(unit->resourceType != ResourceCarried::None && !(unit->resourceType & ResourceCarried::GasOrMineral))
	)
	{

		setNextWaypoint_Sub4EB290(unit);

		if (unit->nextTargetWaypoint != unit->moveTarget.pt) {
			unit->nextTargetWaypoint.x = unit->moveTarget.pt.x;
			unit->nextTargetWaypoint.y = unit->moveTarget.pt.y;
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
	else
	if (unit->mainOrderState == 0) {
		function_00468D10(unit, target);
	}

}

;

//Function 004690C0 used by OrderId::ReturnGas and OrderId::ReturnMinerals
void orders_ReturnHarvest(CUnit* unit){

	if (
		unit->resourceType != ResourceCarried::None &&
		unit->resourceType & ResourceCarried::GasOrMineral
	)
	{

		CUnit* target = unit->orderTarget.unit;

		function_00468AA0(unit);

		if (target == NULL || !unitIsActiveResourceDepot(target)) {
			unit->mainOrderState = 0;
			target = NULL;
		}

		if (unit->mainOrderState == 0) {

			bool bEndThere = false;

			if (target == NULL) {

				target = FindBestReturnCargoTarget(unit);

				if (target == NULL) {

					if (unit->pAI != NULL)
						unit->orderComputerCL(OrderId::ComputerAI);
					else
						unit->orderQueueTimer = 75;

					bEndThere = true;

				}

			}

			if (!bEndThere) {

				unit->orderTarget.unit = target;

				if (orderToMoveToTarget(unit, target))
					unit->mainOrderState = 1;
				
			}


		}
		else
		if (unit->mainOrderState == 1) {

			int movableState = unit->getMovableState();

			if (movableState != 0) {		//0 = unit not at destination

				if (movableState == 2) {	//2 = unit unmovable
					unit->orderTarget.unit = NULL;
					unit->mainOrderState = 0;
				}
				else
				{

					u8 orderId;

					if (unit->pAI != NULL)
						orderId = OrderId::ComputerAI;
					else
					if(unit->resourceType != ResourceCarried::Vespene)
						orderId = OrderId::MoveToMinerals;
					else
						orderId = OrderId::HarvestGas1;

					if (unit->worker.targetResource.unit != NULL)
						function_00474A70(unit, unit->worker.targetResource.unit, orderId);
					else
						orderImmediate(unit, orderId);

					unit->orderToIdle();

					function_004F39A0(unit);

					if (unitTypeIsHatcheryType(target))
						function_004E92B0(unit, target);

				}

			}

		}

	}
	else //carry nothing, or carry a non-resource powerup
	{

		setNextWaypoint_Sub4EB290(unit);

		if (unit->nextTargetWaypoint != unit->moveTarget.pt) {
			unit->nextTargetWaypoint.x = unit->moveTarget.pt.x;
			unit->nextTargetWaypoint.y = unit->moveTarget.pt.y;
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

//Used by OrderId::MoveToMinerals
void orders_HarvestMinerals1(CUnit* unit) {

	CUnit* target = unit->orderTarget.unit;

	if (
		unit->resourceType == ResourceCarried::None ||
		unit->resourceType & ResourceCarried::GasOrMineral
	)
	{

		function_00468AA0(unit);

		if (
			target == NULL ||
			target->id < UnitId::ResourceMineralField ||
			target->id > UnitId::ResourceMineralFieldType3
		)
		{

			Point16 somePos;
			CUnit* nearestMineralField;

			if (
				unit->worker.targetResource.pt.x != 0 ||
				unit->worker.targetResource.pt.y != 0
			)
			{
				somePos = unit->worker.targetResource.pt;
				unit->worker.targetResource.pt.x = 0;
				unit->worker.targetResource.pt.y = 0;
			}
			else
			{
				somePos.x = unit->sprite->position.x;
				somePos.y = unit->sprite->position.y;
			}

			nearestMineralField = harvestNextNearestResources(unit, somePos);

			if (nearestMineralField == NULL) {

				setNextWaypoint_Sub4EB290(unit);

				if (unit->nextTargetWaypoint != unit->moveTarget.pt) {
					unit->nextTargetWaypoint.x = unit->moveTarget.pt.x;
					unit->nextTargetWaypoint.y = unit->moveTarget.pt.y;
				}

				unit->orderToIdle();

			}
			else
			{

				unit->orderTarget.unit = nearestMineralField;
				target = nearestMineralField;
				unit->orderTarget.pt.x = nearestMineralField->sprite->position.x;
				unit->orderTarget.pt.y = nearestMineralField->sprite->position.y;
				unit->mainOrderState = 0;

			}

		}

		if (unit->mainOrderState == 0) {
			if (orderToMoveToTarget(unit, unit->orderTarget.unit)) {
				unit->worker.targetResource.pt = unit->orderTarget.pt;
				unit->worker.targetResource.unit = unit->orderTarget.unit;
				unit->mainOrderState = 1;
			}
		}
		else
		if (unit->mainOrderState == 1) {
			
			if (
				unit->isTargetWithinMinRange(target, 10) ||
				unit->getMovableState() != 0					//0= "not reached destination"
			)
			{

				bool bEndThere = false;

				if (target->building.resource.gatherQueueCount != 0) {

					CUnit* nearestMineralField = harvestNextNearestResourcesEx(unit, unit->sprite->position, NULL);

					if (nearestMineralField != NULL && nearestMineralField != target) {

						if (orderToMoveToTarget(unit, nearestMineralField))
						{
							unit->worker.targetResource.unit = nearestMineralField;
							unit->worker.targetResource.pt.x = nearestMineralField->sprite->position.x;
							unit->worker.targetResource.pt.y = nearestMineralField->sprite->position.y;
							unit->orderTarget.unit = nearestMineralField;
							unit->orderTarget.pt.x = nearestMineralField->sprite->position.x;
							unit->orderTarget.pt.y = nearestMineralField->sprite->position.y;
						}

					}
					else
						bEndThere = true;

				}

				if (!bEndThere)
					unit->mainOrderState = 3;

			}

		}
		
		if (unit->mainOrderState == 3) {
			
			int movableState = unit->getMovableState();

			if (movableState != 0) {

				if (movableState != 2) { //unit can move
					if (unit->resourceType & ResourceCarried::Minerals)
						unit->orderComputerCL(OrderId::ReturnMinerals);
					else
					{
						function_00494BB0(unit, unit->orderTarget.pt.x, unit->orderTarget.pt.y);
						function_00474A70(unit, target, OrderId::HarvestMinerals2);
						unit->orderToIdle();
					}
				}
				else
				{

					CUnit* nearestMineralField = harvestNextNearestResources(unit, unit->sprite->position);

					if (nearestMineralField == NULL)
						unit->orderToIdle();
					else
					if (orderToMoveToTarget(unit, nearestMineralField)) {
						unit->worker.targetResource.unit = nearestMineralField;
						unit->worker.targetResource.pt.x = nearestMineralField->sprite->position.x;
						unit->worker.targetResource.pt.y = nearestMineralField->sprite->position.y;
						unit->orderTarget.unit = nearestMineralField;
						unit->orderTarget.pt.x = nearestMineralField->sprite->position.x;
						unit->orderTarget.pt.y = nearestMineralField->sprite->position.y;
					}

				}

			}

		}

	}
	else
	{

		setNextWaypoint_Sub4EB290(unit);

		if (unit->nextTargetWaypoint != unit->moveTarget.pt) {
			unit->nextTargetWaypoint.x = unit->moveTarget.pt.x;
			unit->nextTargetWaypoint.y = unit->moveTarget.pt.y;
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

//Function 00469500 used by OrderId::Harvest1 and OrderId::HarvestGas1
void orders_Harvest1(CUnit* unit) {

	CUnit* target = unit->orderTarget.unit;
	
	if (
		unit->resourceType != ResourceCarried::None &&
		!(unit->resourceType & ResourceCarried::GasOrMineral)
	)
	{

		setNextWaypoint_Sub4EB290(unit);

		if (unit->nextTargetWaypoint != unit->moveTarget.pt) {
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
	else
	{

		function_00468AA0(unit);

		if (unit->mainOrderState == 3) {

			int movableState = unit->getMovableState();

			if (movableState != 0) { //0 is "not reached destination"

				if (target == NULL || !unit_isGeyserUnitEx(unit, target)) {
					makeToHoldPosition_Helper(unit);
					unit->orderToIdle();
				}
				else
				if(movableState == 2)
					unit->mainOrderState = 0;
				else
				if(unit->resourceType & ResourceCarried::Vespene)
					unit->orderComputerCL(OrderId::ReturnGas);
				else
				{
					function_00474A70(unit, target, OrderId::HarvestGas2);
					unit->orderToIdle();
				}

			}

		}
		else
		if (unit->mainOrderState == 0) {

			if (target == NULL) {
				makeToHoldPosition_Helper(unit);
				unit->orderToIdle();
			}
			else
			if (
				target->id >= UnitId::ResourceMineralField &&
				target->id <= UnitId::ResourceMineralFieldType3
			)
			{
				function_00474AF0(unit, target, OrderId::MoveToMinerals, unit->orderTarget.pt);
				unit->orderToIdle();
			}
			else
			if (orderToMoveToTarget(unit, target)) {
				unit->worker.targetResource.pt.x = unit->orderTarget.pt.x;
				unit->worker.targetResource.pt.y = unit->orderTarget.pt.y;
				unit->worker.targetResource.unit = target;
				unit->mainOrderState = 3;
			}

		}

	}

}

;

//Function 004697C0 used by OrderId::MiningMinerals
void orders_HarvestMinerals3(CUnit* unit) {
	
	CUnit* target = unit->orderTarget.unit;

	if (
		target == NULL ||
		target->id < UnitId::ResourceMineralField ||
		target->id > UnitId::ResourceMineralFieldType3
	)
	{
		
		if (unit->worker.isCarryingSomething) {

			if (unit->worker.harvestTarget != NULL) {

				CUnit* r_function_00468970;

				target = unit->worker.harvestTarget;

				unit->worker.harvestTarget = NULL;
				unit->worker.isCarryingSomething = false;
				target->building.resource.gatherQueueCount -= 1;

				r_function_00468970 = function_00468970(target);

				if (r_function_00468970 != NULL)
					function_00468C70(target, r_function_00468970);

			}
			else
				unit->worker.isCarryingSomething = false;

		}

		function_00474400(unit, OrderId::Harvest3);

		unit->userActionFlags |= 1;

		if (unit->mainOrderId != OrderId::Die) {

			while (
				unit->orderQueueTail != NULL &&
				(
					orders_dat::CanBeInterrupted[unit->orderQueueTail->orderId] != 0 ||
					unit->orderQueueTail->orderId == OrderId::MoveToMinerals
				)
			)
				removeOrderFromUnitQueue(unit, unit->orderQueueTail);

			unit->performAnotherOrder(OrderId::MoveToMinerals, 0, 0, NULL, UnitId::None);

		}
			
		prepareForNextOrder(unit);

	}
	else
	{

		function_00468AA0(unit);

		if (unit->mainOrderState == 0) {
			if (getCloserToDestination_sub402BE0(unit)) {
				unit->orderTarget.pt.x = (20 * angleDistance[unit->currentDirection1].x) / 256 + unit->sprite->position.x;
				unit->orderTarget.pt.y = (20 * angleDistance[unit->currentDirection1].y) / 256 + unit->sprite->position.y;
				unit->mainOrderState = 4;
			}
		}
		
		if (unit->mainOrderState == 4) {
			unit->sprite->playIscriptAnim(IscriptAnimation::AlmostBuilt, true);
			unit->mainOrderTimer = 75;
			unit->mainOrderState = 5;
		}
		else
		if (unit->mainOrderState == 5) {

			if (unit->mainOrderTimer == 0) {

				unit->sprite->playIscriptAnim(IscriptAnimation::GndAttkToIdle, true);
				transferResourceToWorker_Helper(unit, target);
				function_00468E40(unit, target);
				function_00474400(unit, OrderId::Harvest3);

				if (unit->resourceType & ResourceCarried::Minerals)
					unit->orderComputerCL(OrderId::ReturnMinerals);
				else
					unit->orderComputerCL(OrderId::MoveToMinerals);

			}

		}

	}

}

;

void orders_HarvestGas3(CUnit* unit) {

	CUnit* target = unit->orderTarget.unit;

	if (
		unit->sprite->flags & CSprite_Flags::Hidden ||
		(
			target != NULL &&
			unit_isGeyserUnitEx(unit,target) &&
			(unit->resourceType == ResourceCarried::None || unit->resourceType & ResourceCarried::GasOrMineral)
		)
	) 
	{

		function_00468AA0(unit);

		if (unit->mainOrderState == 0) {
			scbw::prepareUnitMove(unit, true);
			unit->mainOrderState = 5;
			unit->mainOrderTimer = 37;
		}
		else
		if (unit->mainOrderState == 5) {
			
			if (unit->mainOrderTimer == 0) {

				Point16 pos;
				s8 angle = unit->currentDirection1;

				if (!function_00468B00(unit, target, &pos, &angle))
					unit->mainOrderTimer = 18;
				else
				{

					turnGraphic(unit, angle);
					scbw::setUnitPosition(unit, pos.x, pos.y);
					showAndEnableUnit(unit);

					if (target != NULL)
						transferResourceToWorker_Helper(unit, target);

					if (unit->worker.isCarryingSomething)
						function_00468E40(unit, unit->worker.harvestTarget);

					function_00474400(unit, OrderId::ResetCollision2);

					if (unit->orderQueueHead == NULL) {

						if (unit->resourceType & ResourceCarried::Vespene)
							unit->orderSimple(OrderId::ReturnGas, false);
						else
							unit->orderSimple(units_dat::ReturnToIdleOrder[unit->id], false);

					}

					function_00474760(unit, unit->orderQueueHead, OrderId::ResetCollision2);
					actUnitReturnToIdle(unit);

				}

			}

		}
		
	}
	else
	{ //699B7
		
		setNextWaypoint_Sub4EB290(unit);

		if (unit->nextTargetWaypoint != unit->moveTarget.pt) {
			unit->nextTargetWaypoint.x = unit->moveTarget.pt.x;
			unit->nextTargetWaypoint.y = unit->moveTarget.pt.y;
		}

		actUnitReturnToIdle(unit);

		if (unit->worker.isCarryingSomething)
			function_00468E40(unit, unit->worker.harvestTarget);

	}

}

;

} //hooks

;

//-------- Helper function definitions. Do NOT modify! --------//

namespace {

const u32 Func_unitIsActiveResourceDepot = 0x00401CF0;
bool unitIsActiveResourceDepot(CUnit* unit) {

	static Bool32 bPreResult;

	__asm {
		PUSHAD
		MOV EDX, unit
		CALL Func_unitIsActiveResourceDepot
		MOV bPreResult, EAX
		POPAD
	}

	return (bPreResult != 0);

}

;

const u32 Func_Sub402BE0_GetCloserToDestination = 0x00402BE0;
//return true when unit reached nextTargetWaypoint either at call
//or after some action inside the function, else return false
//At least that's what I think it does (not sure what I called
//"action inside the function" is, and how it's used to return
//true
bool getCloserToDestination_sub402BE0(CUnit* unit) {

	static Bool32 bResult;

	__asm {
		PUSHAD
		MOV ESI, unit
		CALL Func_Sub402BE0_GetCloserToDestination
		MOV bResult, EAX
		POPAD
	}

	return (bResult == 1);

}

;

const u32 Func_unitTypeIsHatcheryType = 0x00402C20;
bool unitTypeIsHatcheryType(CUnit* unit) {

	static Bool32 bPreResult;

	__asm {
		PUSHAD
		MOV EAX, unit
		CALL Func_unitTypeIsHatcheryType
		MOV bPreResult, EAX
		POPAD
	}

	return (bPreResult != 0);

}

;

const u32 Func_FindBestReturnCargoTarget = 0x00441310;
CUnit* FindBestReturnCargoTarget(CUnit* unit) {

	static CUnit* found_unit;

	__asm {
		PUSHAD
		MOV EAX, unit
		CALL Func_FindBestReturnCargoTarget
		MOV found_unit, EAX
		POPAD
	}

	return found_unit;

}

;

const u32 Func_harvestNextNearestResources = 0x00442F70;
CUnit* harvestNextNearestResources(CUnit* unit, Point16 pos) {

	static CUnit* found_unit;

	__asm {
		PUSHAD
		PUSH pos
		MOV ECX, unit
		CALL Func_harvestNextNearestResources
		MOV found_unit, EAX
		POPAD
	}

	return found_unit;

}

;

const u32 Func_harvestNextNearestResourcesEx = 0x004432D0;
CUnit* harvestNextNearestResourcesEx(CUnit* unit, Point16 pos, CUnit* resource) {

	static CUnit* found_unit;

	__asm {
		PUSHAD
		PUSH pos
		PUSH unit
		MOV EAX, resource
		CALL Func_harvestNextNearestResourcesEx
		MOV found_unit, EAX
		POPAD
	}

	return found_unit;

}

;

const u32 Func_Sub468970 = 0x00468970;
CUnit* function_00468970(CUnit* unit) {

	static CUnit* returned_unit;

	__asm {
		PUSHAD
		MOV EBX, unit
		CALL Func_Sub468970
		MOV returned_unit, EAX
		POPAD
	}

	return returned_unit;

}

;

const u32 Func_Sub4686D0 = 0x004686D0;
void function_004686D0(CUnit* unit, CUnit* target) {
	__asm {
		PUSHAD
		MOV EAX, unit
		PUSH target
		CALL Func_Sub4686D0
		POPAD
	}
}

;

//Logically identical to function @ 0x00468930
bool unit_isGeyserUnitEx(CUnit* unit, CUnit* target) {
	return (
		(target->status & UnitStatus::Completed) &&
		(
			target->id == UnitId::TerranRefinery ||
			target->id == UnitId::ProtossAssimilator ||
			target->id == UnitId::ZergExtractor
		) &&
		target->playerId == unit->playerId
	);
}

;

const u32 Func_Sub468AA0 = 0x00468AA0;
//May order the unit to perform ResetCollision2 in different ways, or have no effect.
void function_00468AA0(CUnit* unit) {
	__asm {
		PUSHAD
		MOV EAX, unit
		CALL Func_Sub468AA0
		POPAD
	}
}

;

const u32 Func_Sub468B00 = 0x00468B00;
bool function_00468B00(CUnit* unit, CUnit* target, Point16* pos, s8* angle) {

	static Bool32 bPreResult;

	__asm {
		PUSHAD
		PUSH angle
		PUSH pos
		PUSH target
		PUSH unit
		CALL Func_Sub468B00
		MOV bPreResult, EAX
		POPAD
	}

	return (bPreResult != 0);

}

;

const u32 Func_Sub468C70 = 0x00468C70;
bool function_00468C70(CUnit* resource, CUnit* unit) {

	static Bool32 bPreResult;

	__asm {
		PUSHAD
		MOV EAX, unit
		MOV EDI, resource
		CALL Func_Sub468C70
		MOV bPreResult, EAX
		POPAD
	}

	return (bPreResult != 0);

}

;

const u32 Func_Sub468D10 = 0x00468D10;
//Try to make worker harvest (through successful call to 00468C70) or join queue for resource
void function_00468D10(CUnit* unit, CUnit* target) {
	__asm {
		PUSHAD
		MOV ECX, unit
		MOV EAX, target
		CALL Func_Sub468D10
		POPAD
	}
}

;

const u32 Func_Sub468E40 = 0x00468E40;
//Probably try to remove worker from target resource queue, then make it harvest.
//Maybe more than just removing from queue to actually decrease the queue size,
//yet not necessarily harvest after that.
void function_00468E40(CUnit* unit, CUnit* target) {
	__asm {
		PUSHAD
		MOV ECX, target
		MOV EAX, unit
		CALL Func_Sub468E40
		POPAD
	}
}

;

const u32 Func_Sub4696D0 = 0x004696D0;
//Hooked in hooks\harvest*
void transferResourceToWorker_Helper(CUnit* worker, CUnit* resource) {
	__asm {
		PUSHAD
		MOV ECX, worker
		MOV EAX, resource
		CALL Func_Sub4696D0
		POPAD
	}
}

;

const u32 Func_removeOrderFromUnitQueue = 0x004742D0;
void removeOrderFromUnitQueue(CUnit* unit, COrder* order) {

	__asm {
		PUSHAD
		MOV ECX, unit
		MOV EAX, order
		CALL Func_removeOrderFromUnitQueue
		POPAD
	}

}

;

const u32 Func_Sub474400 = 0x00474400;
void function_00474400(CUnit* unit, u8 orderId) {
	__asm {
		PUSHAD
		MOV ECX, unit
		MOV DL, orderId
		CALL Func_Sub474400
		POPAD
	}
}

;

const u32 Func_Sub474760 = 0x00474760;
void function_00474760(CUnit* unit, COrder* order, u8 orderId) {

	__asm {
		PUSHAD
		MOV ESI, unit
		MOV EDI, order
		MOV BL, orderId
		CALL Func_Sub474760
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

const u32 Func_Sub474AF0 = 0x00474AF0;
//May order the unit to perform in 2 different ways
void function_00474AF0(CUnit* unit, CUnit* target, u8 orderId, Point16 pos) {
	__asm {
		PUSHAD
		PUSH target
		PUSH pos
		MOV ECX, unit
		MOV AL, orderId
		CALL Func_Sub474AF0
		POPAD
	}
}

;

const u32 Func_OrderImmediate = 0x00474B40;
void orderImmediate(CUnit* unit, u8 order) {

	__asm {
		PUSHAD
		MOV ECX, unit
		MOV AL, order
		CALL Func_OrderImmediate
		POPAD
	}

}

;

const u32 Func_ActUnitReturnToIdle = 0x00475420;
void actUnitReturnToIdle(CUnit* unit) {

	__asm {
		PUSHAD
		MOV EAX, unit
		CALL Func_ActUnitReturnToIdle
		POPAD
	}

}

;

const u32 Func_Sub494BB0 = 0x00494BB0;
void function_00494BB0(CUnit* unit, int x, int y) {

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

const u32 Func_InitUnitTrapDoodad = 0x004E6490;
void showAndEnableUnit(CUnit* unit) {

	__asm {
		PUSHAD
		MOV EDI, unit
		CALL Func_InitUnitTrapDoodad
		POPAD
	}

}

;

const u32 Func_Sub4E92B0 = 0x004E92B0;
void function_004E92B0(CUnit* unit, CUnit* target) {
	__asm {
		PUSHAD
		PUSH unit
		MOV EAX, target
		CALL Func_Sub4E92B0
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

//Hooked in hooks\orders\base_orders\stopholdpos_orders
const u32 Func_OrdersHoldPositionSuicidal = 0x004EB5B0;
void makeToHoldPosition_Helper(CUnit* unit) {

	__asm {
		PUSHAD
		MOV ESI, unit
		CALL Func_OrdersHoldPositionSuicidal
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

	return bPreResult != 0;

}

;

const u32 Func_Sub4F39A0 = 0x004F39A0;
void function_004F39A0(CUnit* unit) {
	__asm {
		PUSHAD
		MOV ESI, unit
		CALL Func_Sub4F39A0
		POPAD
	}
}

;

} //Unnamed namespace

//End of helper functions

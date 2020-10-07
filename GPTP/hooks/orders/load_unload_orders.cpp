#include "load_unload_orders.h"
#include <SCBW/api.h>

//helper functions def

namespace {

bool isDistanceGreaterThanHaltDistance(CUnit* unit, int x, int y, u32 distance);									//01240
Bool32 function_00438E70(CUnit* unit, int x, int y);																//38E70
void function_0043DF30(CUnit* unit);																				//3DF30
void removeOrderFromUnitQueue(CUnit* unit, COrder* order);															//742D0
void unknownSpecialOrder(CUnit* unit, COrder* order, u8 orderId, s16 x, s16 y, CUnit* target, u16 targetUnitId);	//74540
void function_00474A70(CUnit* unit, CUnit* target, u8 orderId);														//74A70
void orderImmediate(CUnit* unit, u8 order);																			//74B40
void _doOrder(CUnit* unit, u32 orderId, int x, int y, bool stopPreviousOrders);										//74C70
Bool32 function_00477160(CUnit* unit, u32 unk1, u32 unk2, u32 unk3);												//77160
void function_00494BB0(CUnit* unit,int x, int y);																	//94BB0
bool unitIsActiveTransport(CUnit* unit);																			//E6BA0
CUnit* getLoadedUnitFromIndex(CUnit* transport, int index);															//E6C40
Bool32 getFirstLoadedWorker(CUnit* transport);																		//E6D90
Bool32 function_004E6EF0(CUnit* unit, CUnit** unitTargetOffset);													//E6EF0
u32 function_004E76C0(CUnit* transport, CUnit* loaded_unit, Point16* pos);											//E76C0
void loadUnitProc(CUnit* unit, CUnit* unitToLoad);																	//E78E0	(Hooked)
Bool32 unloadUnitProc(CUnit* unit);																					//E7F70 (Hooked)
void makeToHoldPosition_Helper(CUnit* unit);																		//EB5B0
bool SetMoveTarget_xy(CUnit* unit, int x, int y);																	//EB820
bool function_004EB900(CUnit* unit, CUnit* target);																	//EB900
bool orderToMoveToTarget(CUnit* unit, CUnit* target);																//EB980

} //unnamed namespace

namespace hooks {

	//unit is a transport (overlord,shuttle,dropship)
	//this is the initial pickup order
	void orders_Pickup1(CUnit* unit) {

		CUnit* unitToLoad = unit->orderTarget.unit;

		if(	unitToLoad != NULL &&
			scbw::canBeEnteredBy(unit,unitToLoad)
		) 
		{
			function_00474A70(unit,unitToLoad,OrderId::Pickup2);
			prepareForNextOrder(unit);
		}
		else
		if(unit->pAI != NULL) {

			Point16 pos = {0,0};

			unit->userActionFlags |= 1;

			if(unit->mainOrderId != OrderId::Die) {

				bool bStopLoop = (unit->orderQueueTail == NULL);

				while(!bStopLoop) {

					u8 orderId = (u8)unit->orderQueueTail->orderId;

					if(
						orders_dat::CanBeInterrupted[orderId] == 0 &&
						orderId != OrderId::ComputerAI
					)
						bStopLoop = true;

					if(!bStopLoop) {
						removeOrderFromUnitQueue(unit,unit->orderQueueTail);
						bStopLoop = (unit->orderQueueTail == NULL);
					}

				}

				unit->performAnotherOrder(OrderId::ComputerAI,0,0,NULL,UnitId::None);

			}

			prepareForNextOrder(unit);

		}

	}

	;

	//unit is a non-mobile transport (bunker)
	void orders_Pickup3_0(CUnit* unit) {

		CUnit* unitToLoad = unit->orderTarget.unit;

		if(unitToLoad == NULL) {

			if(unit->orderQueueHead != NULL) {
				unit->userActionFlags |= 1;
				prepareForNextOrder(unit);
			}
			else {
				if(unit->pAI != NULL)
					unit->orderComputerCL(OrderId::ComputerAI);
				else
					unit->orderComputerCL(units_dat::ReturnToIdleOrder[unit->id]);
			}

		}
		else { //E73F4

			if(scbw::canBeEnteredBy(unit,unitToLoad))
				unitToLoad->orderTo(OrderId::EnterTransport,unit);

			unit->orderToIdle();

		}

	}

	;

	//unit is a transport (overlord,shuttle,dropship)
	//this is used after orders_Pickup1
	void orders_Pickup2(CUnit* unit) {

		CUnit* unitToLoad = unit->orderTarget.unit;

		if(unitToLoad == NULL) {

			if(unit->orderQueueHead != NULL) {
				unit->userActionFlags |= 1;
				prepareForNextOrder(unit);
			}
			else {
				if(unit->pAI != NULL)
					unit->orderComputerCL(OrderId::ComputerAI);
				else
					unit->orderComputerCL(units_dat::ReturnToIdleOrder[unit->id]);
			}

		}
		else { //E761F

			if(
				unitToLoad->pAI == NULL ||
				unitToLoad->mainOrderId == OrderId::Pickup4
			) 
			{ //E762F

				if(unit->mainOrderState == 0) {

					if(scbw::canBeEnteredBy(unit,unitToLoad)) {

						if(unit->mainOrderTimer == 0) {

							unit->mainOrderTimer = 7;

							if(!unit->isTargetWithinMinRange(unitToLoad,1))
								orderToMoveToTarget(unit,unitToLoad);
							else {

								makeToHoldPosition_Helper(unit);

								if(
									unitToLoad->mainOrderId != OrderId::EnterTransport &&
									unitToLoad->mainOrderId != OrderId::Pickup4
								)
									unitToLoad->orderTo(OrderId::EnterTransport,unit);

								if(unit->pAI != NULL)
									unit->mainOrderState = 1;
								else {
									function_00474A70(unit,unitToLoad,OrderId::Pickup1);
									prepareForNextOrder(unit);
								}

							}


						}

					}
					else
						unit->orderToIdle();

				}

			}
			else //E7642
				unit->orderToIdle();

		}

	}

	;

	//unit is a transport (overlord,shuttle,dropship)
	//a big part of the code could not be tested
	//since different ground height test never
	//occured during testing
	void orders_MoveUnload(CUnit* unit) {

		u32 distance;

		if(
			unit->mainOrderState == 0 &&
			unit->orderTarget.unit != NULL
		) 
		{
			unit->mainOrderState = 1;
			unit->orderTarget.pt.x = (unit->orderTarget.unit)->sprite->position.x;
			unit->orderTarget.pt.y = (unit->orderTarget.unit)->sprite->position.y;
			unit->orderTarget.unit = NULL;
		}

		//E7739
		distance = scbw::getDistanceFast(
			unit->orderTarget.pt.x * 256,
			unit->orderTarget.pt.y * 256,
			unit->halt.x,
			unit->halt.y
		);

		distance &= 0xFFFFFF00;

		if(distance <= 0x1000) {

			if(unit->orderQueueHead != NULL) {
				unknownSpecialOrder(unit,unit->orderQueueHead,OrderId::Unload,0,0,NULL,UnitId::None);
				prepareForNextOrder(unit);
			}
			else {
				unit->performAnotherOrder(OrderId::Unload,0,0,NULL,UnitId::None);
				prepareForNextOrder(unit);
			}

		}
		else { //E77B4

			SetMoveTarget_xy(unit,unit->orderTarget.pt.x,unit->orderTarget.pt.y);

			if(
				unit->orderTarget.pt.x != unit->nextTargetWaypoint.x ||
				unit->orderTarget.pt.y != unit->nextTargetWaypoint.y
			)
			{
				unit->nextTargetWaypoint.x = unit->orderTarget.pt.x;
				unit->nextTargetWaypoint.y = unit->orderTarget.pt.y;
			}

			if(unit->pAI != NULL) {

				u32 groundHeightAtPxPos1 =
					scbw::getGroundHeightAtPos(
						unit->sprite->position.x,
						unit->sprite->position.y
						);

				if(groundHeightAtPxPos1 !=
					scbw::getGroundHeightAtPos(
					unit->orderTarget.pt.x,
					unit->orderTarget.pt.y
					))
				{
					
					distance = scbw::getDistanceFast(
						unit->orderTarget.pt.x * 256,
						unit->orderTarget.pt.y * 256,
						unit->halt.x,
						unit->halt.y
					);

					distance &= 0xFFFFFF00;

					if(
						distance <= 0xCA000 &&
						function_00438E70(unit,unit->orderTarget.pt.x,unit->orderTarget.pt.y) != 0 &&
						getFirstLoadedWorker(unit) == 0
					)
					{

						int index = 0;
						bool bStopLoop = false;
						bool bStopAll = false;
						bool bIsDistanceGreaterThanHaltDistance =
							isDistanceGreaterThanHaltDistance(
								unit,
								unit->orderTarget.pt.x,
								unit->orderTarget.pt.y,
								512
							);
						Point16 pos = {0,0}; //unsure about this start value

						while(!bStopLoop) {

							CUnit* unitToUnload = getLoadedUnitFromIndex(unit,index);

							if(unitToUnload != NULL) {

								if(unitToUnload->id == UnitId::ProtossReaver && bIsDistanceGreaterThanHaltDistance)
									bStopLoop = true;
								else 
								if(function_004E76C0(unit,unitToUnload,&pos) == 0) {
									bStopAll = true;
									bStopLoop = true;
								}



							}

							//E78BC
							if(!bStopAll) {
								index++;
								bStopLoop = (index < 8);
							}

						} //while(!bStopLoop)

						if(!bStopAll) {
							orderImmediate(unit,OrderId::Unload);
							prepareForNextOrder(unit);
						}

					}

				}

			}

		}

	} //void orders_MoveUnload(CUnit* unit)

	;

	//unit is an unit to pick up by transport
	//COULD NOT BE TESTED (tried bunker and all
	//and nothing called this function)
	void orders_Pickup4_0(CUnit* unit) {

		if(unit->playerId == 11) {

			unit->userActionFlags |= 1;

			if(unit->status & UnitStatus::InTransport)
				unit->order(OrderId::Nothing2,0,0,NULL,UnitId::None,1);
			else
				unit->order(units_dat::ReturnToIdleOrder[unit->id],0,0,NULL,UnitId::None,1);

			prepareForNextOrder(unit);

		}
		else 
		if(	!(unit->status & UnitStatus::InTransport) &&
			function_00477160(unit,1,0,0) == 0
		) 
		{ //E7BEB

			CUnit* target = unit->orderTarget.unit;
			bool jump_to_E7C2B = false;
			bool jump_to_E7C37 = false;
			bool jump_to_E7C44 = false;
			bool bEndThere = false;

			if(target == NULL) {

				if(function_004E6EF0(unit,&target) == 0) {
					unit->orderToIdle();
					bEndThere = true;
				}
				else
				if(target != NULL)
					jump_to_E7C2B = true;

			}

			if(!bEndThere) {

				if(!jump_to_E7C2B) { //E7C1B

					if(!unitIsActiveTransport(target) ||
						target->orderTarget.unit != unit
					)
						jump_to_E7C37 = true;
					else
						jump_to_E7C2B = true;

				}

				if(jump_to_E7C2B) {
					if(scbw::canBeEnteredBy(target,unit))
						jump_to_E7C44 = true;
					else
						jump_to_E7C37 = true;
				}

				if(jump_to_E7C37)
					unit->orderTarget.unit = NULL;

				if(jump_to_E7C44) {

					if(unit->isTargetWithinMinRange(target,1)) { //E7C52

						loadUnitProc(target,unit);
						target->orderTo(OrderId::MoveUnload,unit->orderTarget.pt.x,unit->orderTarget.pt.y);

						if(target->pAI != NULL)
							_doOrder(
								target,
								OrderId::Move,
								target->sprite->position.x,
								target->sprite->position.y,
								false
							);


					}
					else { //E7C94

						function_00474A70(target,unit,OrderId::Pickup2);
						prepareForNextOrder(target);

						if(function_00438E70(unit,target->sprite->position.x,target->sprite->position.y) != 0) {
							function_00494BB0(unit,target->sprite->position.x,target->sprite->position.y);
							function_004EB900(unit,target);
						}

					}

				}

			}

		}

	}

	;

	//unit is an unit aiming to enter a transport
	void orders_EnterTransport(CUnit* unit) {

		CUnit* target = unit->orderTarget.unit;

		if(
			target != NULL &&
			!(target->status & UnitStatus::IsHallucination) &&
			(target->id != UnitId::ZergOverlord ||
			 UpgradesSc->currentLevel[unit->playerId][UpgradeId::VentralSacs] != 0) &&
			units_dat::SpaceProvided[target->id] != 0 &&
			scbw::canBeEnteredBy(target,unit)
		) 
		{ //E7D51

			if(
				unit->mainOrderState == 0 &&
				target->mainOrderId != OrderId::Pickup2
			)
			{

				if(target->status & UnitStatus::IsBuilding) {
					function_00474A70(target,unit,OrderId::Pickup2);
					prepareForNextOrder(target);
				}

				unit->mainOrderState = 1;

			}

			if(	target->sprite->position.x != unit->nextTargetWaypoint.x ||
				target->sprite->position.y != unit->nextTargetWaypoint.y)
			{
				unit->nextTargetWaypoint.x = target->sprite->position.x;
				unit->nextTargetWaypoint.y = target->sprite->position.y;
			}

			function_004EB900(unit,target);

			if(unit->isTargetWithinMinRange(target,1))
				loadUnitProc(target,unit);

		}
		else {

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

	void orders_Unload(CUnit* unit) {

		static CUnit* unitTable_0059CB58 = (CUnit*)(0x0059CB58);	//array of CUnit structures

		if(
			unit->status & UnitStatus::GroundedBuilding ||
			unit->mainOrderTimer == 0
		)
		{

			int counter = 0;
			u32 result_E7F70 = 0;
			bool bEndLoop = false;
			bool bFoundUnitToUnload = false;

			StoredUnit* loadedUnitOffset;

			bool jump_to_E8191 = false;
			bool jump_to_E8195 = false;
			bool jump_to_E81AD = false;

			while(!bEndLoop) {

				loadedUnitOffset = &unit->loadedUnit[counter];

				if(	loadedUnitOffset->fullValue != 0  &&
					unitTable_0059CB58[loadedUnitOffset->innerValues.index].sprite != NULL
				)
				{

					CUnit* unitToUnload = &(unitTable_0059CB58[loadedUnitOffset->innerValues.index]);

					if(unitToUnload->mainOrderId != 0 || unitToUnload->mainOrderState != 1) {
						
						if(loadedUnitOffset->innerValues.unitId == unitToUnload->targetOrderSpecial) {

							bFoundUnitToUnload = true;
							result_E7F70 = unloadUnitProc(unitToUnload);

							if(!(unit->status & UnitStatus::GroundedBuilding))
								bEndLoop = true;

						}

					}

				}

				if(!bEndLoop) {

					counter++;

					if(counter >= 8)
						bEndLoop = true;

				}

			} //while(!bEndLoop)

			if(!bFoundUnitToUnload)
				jump_to_E8191 = true;

			if(!jump_to_E8191) {

				if(result_E7F70 != 0)
					jump_to_E8195 = true;
				else
				if(
					unit->pAI == NULL ||
					unit->status & UnitStatus::GroundedBuilding
				)
					jump_to_E81AD = true;
				else
					function_0043DF30(unit);

			}

			if(jump_to_E8191) {
				if(result_E7F70 == 0)
					jump_to_E81AD = true;
				else
					jump_to_E8195 = true;
			}

			if(jump_to_E8195) {
				if(
					unit->status & UnitStatus::GroundedBuilding ||
					!unit->hasLoadedUnit()
				)
				jump_to_E81AD = true;
			}

			if(jump_to_E81AD) {

				if(unit->orderQueueHead != NULL) {
					unit->userActionFlags |= 1;
					prepareForNextOrder(unit);
				}
				else 
				if(unit->pAI != NULL) { //E81DE

					unit->userActionFlags |= 1;

					if(unit->mainOrderId == OrderId::Die)
						prepareForNextOrder(unit);
					else {

						bEndLoop = (unit->orderQueueTail == NULL);

						while(!bEndLoop) {

							if(
								orders_dat::CanBeInterrupted[(unit->orderQueueTail)->orderId] ||
								unit->orderQueueTail->orderId == OrderId::ComputerAI
							)
							{
								removeOrderFromUnitQueue(unit,unit->orderQueueTail);
							}
							else
								bEndLoop = true;

							if(!bEndLoop)
								bEndLoop = (unit->orderQueueTail == NULL);

						}

						unit->performAnotherOrder(OrderId::ComputerAI,0,0,NULL,UnitId::None);
						prepareForNextOrder(unit);

					}

				}
				else { //E823A

					unit->userActionFlags |= 1;
					unit->order(
						units_dat::ReturnToIdleOrder[unit->id],
						0,
						0,
						NULL,
						UnitId::None,
						true
					);
					prepareForNextOrder(unit);

				}

			}

		}

	}

} //namespace hooks

;

//-------- Helper function definitions. Do NOT modify! --------//

namespace {

const u32 Func_isDistanceGreaterThanHaltDistance = 0x00401240;
bool isDistanceGreaterThanHaltDistance(CUnit* unit, int x, int y, u32 distance) {

	static Bool32 bPreResult;

	__asm {
		PUSHAD
		PUSH x
		PUSH distance
		MOV EAX, y
		MOV ECX, unit
		CALL Func_isDistanceGreaterThanHaltDistance
		MOV bPreResult, EAX
		POPAD
	}

	return (bPreResult != 0);

}

;

const u32 Func_sub_438E70 = 0x00438E70;
Bool32 function_00438E70(CUnit* unit, int x, int y) {

	static Bool32 result;

	__asm {
		PUSHAD
		PUSH y
		PUSH x
		MOV EAX, unit
		CALL Func_sub_438E70
		MOV result, EAX
		POPAD
	}

	return result;

}

;

const u32 Func_Sub43DF30 = 0x0043DF30;
void function_0043DF30(CUnit* unit) {

	__asm {
		PUSHAD
		PUSH unit
		CALL Func_Sub43DF30
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

const u32 Func_unknownSpecialOrder = 0x00474540;
void unknownSpecialOrder(CUnit* unit, COrder* order, u8 orderId, s16 x, s16 y, CUnit* target, u16 targetUnitId) {

	static Point16 pos;
	pos.x = x;pos.y = y;

	__asm {
		PUSHAD
		PUSH target
		PUSH pos
		MOV BL, orderId
		MOVZX EDX, targetUnitId
		MOV ESI, unit
		MOV EDI, order
		CALL Func_unknownSpecialOrder
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

const u32 Func__doOrder = 0x00474C70;
void _doOrder(CUnit* unit, u32 orderId, int x, int y, bool stopPreviousOrders) {

	Bool32 bStopPreviousOrders = stopPreviousOrders ? 1 : 0;

	__asm {
		PUSHAD
		PUSH bStopPreviousOrders
		PUSH y
		PUSH x
		PUSH orderId
		PUSH unit
		CALL Func__doOrder
		POPAD
	}

}

;

const u32 Func_Sub477160 = 0x00477160;
Bool32 function_00477160(CUnit* unit, u32 unk1, u32 unk2, u32 unk3) {

	__asm {
		PUSHAD
		PUSH unk1
		PUSH unk2
		PUSH unk3
		MOV ECX, unit
		CALL Func_Sub477160
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

const u32 Func_getLoadedUnitFromIndex = 0x004E6C40;
CUnit* getLoadedUnitFromIndex(CUnit* transport, int index) {

	static CUnit* return_value;

	__asm {
		PUSHAD
		MOV ECX, index
		MOV EAX, transport
		CALL Func_getLoadedUnitFromIndex
		MOV return_value, EAX
		POPAD
	}

	return return_value;

}

;

const u32 Func_getFirstLoadedWorker = 0x004E6D90;
Bool32 getFirstLoadedWorker(CUnit* transport) {

	static Bool32 return_value;

	__asm {
		PUSHAD
		MOV EAX, transport
		CALL Func_getFirstLoadedWorker
		MOV return_value, EAX
		POPAD
	}

	return return_value;

}

;

const u32 Func_Sub4E6EF0 = 0x004E6EF0;
Bool32 function_004E6EF0(CUnit* unit, CUnit** unitTargetOffset) {

	Bool32 result;

	__asm {
		PUSHAD
		PUSH unitTargetOffset
		MOV EBX, unit
		CALL Func_Sub4E6EF0
		MOV result, EAX
		POPAD
	}

	return result;

}

;

const u32 Func_Sub4E76C0 = 0x004E76C0;
u32 function_004E76C0(CUnit* transport, CUnit* loaded_unit, Point16* pos) {

	static u32 result;

	__asm {
		PUSHAD
		MOV EAX, transport
		MOV ESI, loaded_unit
		PUSH pos
		CALL Func_Sub4E76C0
		MOV result, EAX
		POPAD
	}

	return result;

}

;

const u32 Func_Sub4E78E0 = 0x004E78E0;
void loadUnitProc(CUnit* unit, CUnit* unitToLoad) {

	__asm {
		PUSHAD
		MOV EAX, unit
		MOV ECX, unitToLoad
		CALL Func_Sub4E78E0
		POPAD
	}

}

;

const u32 Func_Sub4E7F70 = 0x004E7F70;
Bool32 unloadUnitProc(CUnit* unit) {

	static Bool32 return_value;

	__asm {
		PUSHAD
		MOV EAX, unit
		CALL Func_Sub4E7F70
		MOV return_value, EAX
		POPAD
	}

	return return_value;

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

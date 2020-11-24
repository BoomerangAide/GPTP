#include "CMDRECV_QueuedOrder.h"
#include <SCBW/api.h>

//helper functions def

namespace {

void function_004748E0(CUnit* unit, int x, int y, u8 orderId, COrder* order);				//748E0
void function_00474940(CUnit* unit, CUnit* target, u8 orderId, COrder* order);				//74940
void queueNewOrder_Helper(CUnit* unit, u32 orderId, int x, int y, CUnit* target, u32 unitId,
						Point16 unkPos1, Point16 unkPos2, u32 unkQueuedOrderType);			//74B90
void function_00475470(CUnit* unit, u32 orderId, int x, int y, CUnit* target, u32 unitId,
						Point16 unkPos1, Point16 unkPos2, u32 unkQueuedOrderType);			//75470
void showStatTxtToPlayer(char* text, u32 playerId);											//8CF00

} //unnamed namespace

namespace hooks {

void queueNewOrder(CUnit* unit, u32 orderId, int x, int y, CUnit* target, u32 unitId,
						Point16 unkPos1, Point16 unkPos2, u32 unkQueuedOrderType)
{

	//omitting the test <=4, probably bug case thus no performance impact
	
	if (unkQueuedOrderType == 0)
		unit->performAnotherOrder(orderId, 0, 0, NULL, UnitId::None); //if not ignored, target would be the unkOrder value
	else
	if (unkQueuedOrderType == 1)
		function_00474940(unit,target,orderId, NULL);
	else
	if (unkQueuedOrderType == 2)
		unit->performAnotherOrder(orderId,x,y,NULL,unitId, NULL);
	else
	if (unkQueuedOrderType == 3)
		function_004748E0(unit,x,y,orderId, NULL);
	else
	if (unkQueuedOrderType == 4)
		unit->performAnotherOrder(orderId, unkPos2.x, unkPos2.y, *(CUnit**)&unkPos1, UnitId::None, NULL);

}

;

//Used by siege mode, burrow, stop, right click and others
void function_004754F0(CUnit* unit, u32 orderId, int x, int y, CUnit* target, u32 unitId,
							Point16 unkPos1, Point16 unkPos2, Bool32 isQueued, u32 unkQueuedOrderType)
{

	const u32* u32_00641698 = (u32*) 0x00641698; //seems to be used in timers...?

	bool bNotQueuedOrder;

	if (
		isQueued != 0 &&
		orders_dat::CanBeQueued[(u8)orderId] != 0
	)
	{

		if (unit->mainOrderId > OrderId::Medic || unit->mainOrderId < OrderId::Guard)
			bNotQueuedOrder = false;
		else
		if( //original code used an array of 0 and 1 indexed by orderId, used to make a switch between 2 possibilities
			unit->mainOrderId == OrderId::Guard || unit->mainOrderId == OrderId::PlayerGuard || unit->mainOrderId == OrderId::Nothing2 ||
			unit->mainOrderId == OrderId::Pickup1 || unit->mainOrderId == OrderId::Patrol || unit->mainOrderId == OrderId::Medic
		)
			bNotQueuedOrder = true;
		else
			bNotQueuedOrder = false;

	}
	else
		bNotQueuedOrder = true;

	if (!bNotQueuedOrder) { //7552F

		if (unit->orderQueueCount >= 8) {
			showStatTxtToPlayer(
				(char*)statTxtTbl->getString(0x367), //"Unit's waypoint list is full.<0>" 
				unit->playerId
			);
		}
		else
		if (*u32_00641698 >= 1800) {
			showStatTxtToPlayer(
				(char*)statTxtTbl->getString(0x369), //"Running low on orders, your last order was not processed.<0>"
				unit->playerId
			);
		}
		else
			queueNewOrder_Helper(unit, orderId, x, y, target, unitId, unkPos1, unkPos2, unkQueuedOrderType);
		
	}
	else					//755D2
		function_00475470(unit, orderId, x, y, target, unitId, unkPos1, unkPos2, unkQueuedOrderType);

}

;

} //namespace hooks

;

//-------- Helper function definitions. Do NOT modify! --------//

namespace {

//Equivalent to function @ 0x004748E0
//Used in hooks\orders\base_orders\patrol_order.cpp, hooks\orders\medic_orders.cpp 
//and hooks\recv_commands\CMDRECV_QueuedOrder.cpp
void function_004748E0(CUnit* unit, int x, int y, u8 orderId, COrder* order) {
	unit->performAnotherOrder(orderId, x, y, NULL, UnitId::None, order);
}

;

//Equivalent to function @ 0x00474940, also used in hooks\orders\medic_orders.cpp
void function_00474940(CUnit* unit, CUnit* target, u8 orderId, COrder* order) {
	if (target == NULL)
		unit->performAnotherOrder(orderId, 0, 0, NULL, UnitId::None, order);
	else
		unit->performAnotherOrder(orderId, target->sprite->position.x, target->sprite->position.y, target, UnitId::None, order);
}

;

const u32 Func_queueNewOrder = 0x00474B90;
void queueNewOrder_Helper(CUnit* unit, u32 orderId, int x, int y, CUnit* target, u32 unitId,
						Point16 unkPos1, Point16 unkPos2, u32 unkQueuedOrderType) 
{

	__asm {
		PUSHAD
		PUSH unkPos1
		PUSH unkPos2
		PUSH unitId
		PUSH target
		PUSH orderId
		PUSH unit
		MOV EDX, x
		MOV ECX, y
		MOV EAX, unkQueuedOrderType
		CALL Func_queueNewOrder
		POPAD
	}

}

;

const u32 Func_Sub475470 = 0x00475470;
void function_00475470(CUnit* unit, u32 orderId, int x, int y, CUnit* target, u32 unitId,
						Point16 unkPos1, Point16 unkPos2, u32 unkQueuedOrderType)
{

	__asm {
		PUSHAD
		PUSH unkPos1
		PUSH unkPos2
		PUSH unitId
		PUSH target
		PUSH unit
		MOV EDX, x
		MOV ECX, orderId
		MOV EAX, y
		MOV EDI, unkQueuedOrderType
		CALL Func_Sub475470
		POPAD
	}


}

;

const u32 Func_showStatTxtToPlayer = 0x0048CF00;
void showStatTxtToPlayer(char* text, u32 playerId) {
	__asm {
		PUSHAD
		MOV ECX, playerId
		MOV EAX, text
		CALL Func_showStatTxtToPlayer
		POPAD
	}
}
;

} //Unnamed namespace

//End of helper functions

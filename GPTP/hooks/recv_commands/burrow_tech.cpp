#include "burrow_tech.h"
#include <SCBW/api.h>

//Helper functions declaration

namespace {

void removeOrderFromUnitQueue(CUnit* unit, COrder* order);												//742D0
void actUnitReturnToIdle(CUnit* unit);																	//75420
void function_004754F0(CUnit* unit, u32 orderId, int x, int y, CUnit* target, u32 unitId,
						Point16 unkPos1, Point16 unkPos2, Bool32 isQueued, u32 unkQueuedOrderType);		//754F0
void function_004E97C0(CUnit* unit);																	//E97C0

} //unnamed namespace

namespace hooks {

	void CMDRECV_Unburrow() {

		CUnit* selected_unit;

		*selectionIndexStart = 0;
		selected_unit = getActivePlayerNextSelection();

		while(selected_unit != NULL) {

			if(
				units_dat::BaseProperty[selected_unit->id] & UnitProperty::Burrowable &&
				selected_unit->canUseTech(TechId::Burrowing,*ACTIVE_NATION_ID) == 1
			) 
				function_004E97C0(selected_unit);

			selected_unit = getActivePlayerNextSelection();

		}

	}

	;

	void CMDRECV_Burrow(u8 bCommandType) {

		CUnit* selected_unit;

		*selectionIndexStart = 0;
		selected_unit = getActivePlayerNextSelection();

		while(selected_unit != NULL) {

			if(
				units_dat::BaseProperty[selected_unit->id] & UnitProperty::Burrowable &&
				selected_unit->canUseTech(TechId::Burrowing,*ACTIVE_NATION_ID) == 1 &&
				!(selected_unit->status & UnitStatus::Burrowed) &&
				selected_unit->mainOrderId != OrderId::Burrow
			)
			{

				if(bCommandType != 0)
					bCommandType = 1;
				else
					bCommandType = 0;

				function_004754F0(
					selected_unit,
					OrderId::Burrow,
					0,
					0,
					NULL,
					UnitId::None,
					*(Point16*)(0x006D5C24),
					*(Point16*)(0x006D5C20),
					bCommandType,
					1
				);

			}

			selected_unit = getActivePlayerNextSelection();

		}

	}

	;

	//was sub_4E97C0
	void Unburrow_Proc(CUnit* unit) {

		if(	unit->mainOrderId == OrderId::Burrow ||
			unit->mainOrderId == OrderId::Burrowed ||
			unit->id == UnitId::ZergLurker
			) 
		{

			if(unit->id == UnitId::ZergLurker)
				unit->status |= UnitStatus::CanNotReceiveOrders;

			unit->userActionFlags |= 1;

			if(unit->mainOrderId != OrderId::Die) {

				bool bStopLoop = false;

				while(unit->orderQueueTail != NULL && !bStopLoop) {
					if(
						!orders_dat::CanBeInterrupted[unit->orderQueueTail->orderId] &&
						unit->orderQueueTail->orderId != OrderId::Unburrow
					)
						bStopLoop = true;
					else
						removeOrderFromUnitQueue(unit,unit->orderQueueTail);
				}

				unit->performAnotherOrder(OrderId::Unburrow,0,0,NULL,UnitId::None);

			}

			//E9846
			prepareForNextOrder(unit);
			actUnitReturnToIdle(unit);

		}

	}

	;

} //namespace hooks

//-------- Helper function definitions. Do NOT modify! --------//

namespace {

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

	const u32 Func_Sub_4E97C0 = 0x004E97C0;
	void function_004E97C0(CUnit* unit) {

		__asm {
			PUSHAD
			MOV EAX, unit
			CALL Func_Sub_4E97C0
			POPAD
		}

	}

	;
	
	const u32 Func_Sub_4754F0 = 0x004754F0;
	void function_004754F0(CUnit* unit, u32 orderId, int x, int y, CUnit* target, u32 unitId,
							Point16 unkPos1, Point16 unkPos2, Bool32 isQueued, u32 unkQueuedOrderType) 
	{

		__asm {
			PUSHAD
			MOV EAX, y
			MOV EDX, x
			MOV EBX, unitId
			MOV ESI, unit
			PUSH unkPos1
			PUSH unkPos2
			PUSH target
			PUSH orderId
			PUSH isQueued
			PUSH unkQueuedOrderType
			CALL Func_Sub_4754F0
			POPAD
		}

	}

	;

} //unnamed namespace

//End of helper functions

#include "CMDRECV_SiegeTank.h"
#include <SCBW/api.h>

//Helper functions declaration

namespace {

void function_004754F0(CUnit* unit, u32 orderId, int x, int y, CUnit* target, u32 unitId,
						Point16 unkPos1, Point16 unkPos2, Bool32 isQueued, u32 unkQueuedOrderType);	//754F0

} //unnamed namespace

namespace hooks {

void CMDRECV_Siege(u8 bCommandType) {

	CUnit* current_selection;

	*selectionIndexStart = 0;
	current_selection = getActivePlayerNextSelection();

	while(current_selection != NULL) {

		if(
			(current_selection->id == UnitId::TerranSiegeTankTankMode ||
			current_selection->id == UnitId::Hero_EdmundDukeTankMode) &&
			current_selection->mainOrderId != OrderId::SiegeMode
		)
		{
			if(current_selection->canUseTech(TechId::TankSiegeMode,*ACTIVE_NATION_ID) == 1) {

				if(bCommandType != 0)
					bCommandType = 1;
				else
					bCommandType = 0;

				function_004754F0(
					current_selection,
					OrderId::SiegeMode,
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
		}

		current_selection = getActivePlayerNextSelection();

	}

}

;

void CMDRECV_Unsiege(u8 bCommandType) {

	CUnit* current_selection;

	*selectionIndexStart = 0;
	current_selection = getActivePlayerNextSelection();

	while(current_selection != NULL) {

		if(
			(current_selection->id == UnitId::TerranSiegeTankSiegeMode ||
			current_selection->id == UnitId::Hero_EdmundDukeSiegeMode) &&
			current_selection->mainOrderId != OrderId::TankMode
		)
		{
			if(current_selection->canUseTech(TechId::TankSiegeMode,*ACTIVE_NATION_ID) == 1) {

				if(bCommandType != 0)
					bCommandType = 1;
				else
					bCommandType = 0;

				function_004754F0(
					current_selection,
					OrderId::TankMode,
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
		}

		current_selection = getActivePlayerNextSelection();

	}

}

;

} //namespace hooks

//-------- Helper function definitions. Do NOT modify! --------//

namespace {

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

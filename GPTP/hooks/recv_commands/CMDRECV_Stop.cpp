#include "CMDRECV_Stop.h"
#include <SCBW/api.h>

//Helper functions declaration

namespace {

void parseOrdersDatReqs();																			//6D450
int parseRequirementOpcodes(CUnit* unit, u32 orderId, u32 playerId, u32 address, u32 req_offset);	//6D610
void removeOrderFromUnitQueue(CUnit* unit, COrder* order);											//742D0
void function_004754F0_Helper(CUnit* unit, u32 orderId, int x, int y, CUnit* target, u32 unitId,
						Point16 unkPos1, Point16 unkPos2, Bool32 isQueued, u32 unkQueuedOrderType);	//754F0

} //unnamed namespace

namespace hooks {

//The functions seems similar but there are some differences,
//like the u16_ offset, the function to perform the order in
//CMDRECV_Stop, and the orders id

void CMDRECV_ReaverStop() {

	u32* const u32_0066FF60 = (u32*)0x0066FF60;

	CUnit* current_unit;

	*selectionIndexStart = 0;

	current_unit = getActivePlayerNextSelection();

	while(current_unit != NULL) {

		*u32_0066FF60 = 0x00;

		if(orders_dat::RequirementsOffset[OrderId::ReaverStop] == 0xFFFF)
			parseOrdersDatReqs();

		if(current_unit->playerId != *ACTIVE_NATION_ID)
			*u32_0066FF60 = 0x01;
		else
		if(!(current_unit->status & UnitStatus::Completed))
			*u32_0066FF60 = 0x14;
		else
		if(	current_unit->status & UnitStatus::DoodadStatesThing ||
			current_unit->lockdownTimer != 0 ||
			current_unit->stasisTimer != 0 ||
			current_unit->maelstromTimer != 0
		)
			*u32_0066FF60 = 0x0A;
		else
		if( current_unit->status & UnitStatus::Burrowed &&
			current_unit->id != UnitId::ZergLurker
		)
			*u32_0066FF60 = 0x0B;
		else
		if(	current_unit->id == UnitId::TerranSCV &&
			current_unit->mainOrderId == OrderId::ConstructingBuilding
		)
			*u32_0066FF60 = 0x08;
		else
		if((current_unit->id == UnitId::TerranGhost ||
			current_unit->id == UnitId::Hero_SarahKerrigan ||
			current_unit->id == UnitId::Hero_AlexeiStukov ||
			current_unit->id == UnitId::Hero_SamirDuran ||
			current_unit->id == UnitId::Hero_InfestedDuran) &&
			current_unit->mainOrderId == OrderId::NukeTrack
		)
			*u32_0066FF60 = 0x08;
		else
		if(	current_unit->id == UnitId::ProtossArchon &&
			current_unit->mainOrderId == OrderId::CompletingArchonSummon
		)
			*u32_0066FF60 = 0x08;
		else
		if(	current_unit->id == UnitId::ProtossDarkArchon &&
			current_unit->mainOrderId == OrderId::CompletingArchonSummon
		)
			*u32_0066FF60 = 0x08;
		else
		if(orders_dat::RequirementsOffset[OrderId::ReaverStop] == 0)
			*u32_0066FF60 = 0x17;
		else
		if(parseRequirementOpcodes(
			current_unit,
			OrderId::ReaverStop,
			*ACTIVE_NATION_ID,
			0x00514CF8,
			orders_dat::RequirementsOffset[OrderId::ReaverStop]
		) == 1) {

			current_unit->userActionFlags |= 1;

			if(current_unit->mainOrderId != OrderId::Die) {

				bool bContinueLoop = true;

				while(bContinueLoop) {

					if(current_unit->orderQueueTail == NULL)
						bContinueLoop = false;
					else
					if(	orders_dat::CanBeInterrupted[current_unit->orderQueueTail->orderId] ||
						current_unit->orderQueueTail->orderId == OrderId::ReaverStop
					)
						removeOrderFromUnitQueue(current_unit,current_unit->orderQueueTail);
					else
						bContinueLoop = false;

				}

				current_unit->performAnotherOrder(OrderId::ReaverStop,0,0,NULL,UnitId::None);

			}

			prepareForNextOrder(current_unit);

		}

		current_unit = getActivePlayerNextSelection();

	}

}

;

void CMDRECV_CarrierStop() {

	CUnit* current_unit;
	static u32* u32_0066FF60 = (u32*)0x0066FF60;

	*selectionIndexStart = 0;

	current_unit = getActivePlayerNextSelection();

	while(current_unit != NULL) {

		*u32_0066FF60 = 0x00;

		if(orders_dat::RequirementsOffset[OrderId::CarrierStop] == 0xFFFF)
			parseOrdersDatReqs();

		if(current_unit->playerId != *ACTIVE_NATION_ID)
			*u32_0066FF60 = 0x01;
		else
		if(!(current_unit->status & UnitStatus::Completed))
			*u32_0066FF60 = 0x14;
		else
		if(	current_unit->status & UnitStatus::DoodadStatesThing ||
			current_unit->lockdownTimer != 0 ||
			current_unit->stasisTimer != 0 ||
			current_unit->maelstromTimer != 0
		)
			*u32_0066FF60 = 0x0A;
		else
		if( current_unit->status & UnitStatus::Burrowed &&
			current_unit->id != UnitId::ZergLurker
		)
			*u32_0066FF60 = 0x0B;
		else
		if(	current_unit->id == UnitId::TerranSCV &&
			current_unit->mainOrderId == OrderId::ConstructingBuilding
		)
			*u32_0066FF60 = 0x08;
		else
		if((current_unit->id == UnitId::TerranGhost ||
			current_unit->id == UnitId::Hero_SarahKerrigan ||
			current_unit->id == UnitId::Hero_AlexeiStukov ||
			current_unit->id == UnitId::Hero_SamirDuran ||
			current_unit->id == UnitId::Hero_InfestedDuran) &&
			current_unit->mainOrderId == OrderId::NukeTrack
		)
			*u32_0066FF60 = 0x08;
		else
		if(	current_unit->id == UnitId::ProtossArchon &&
			current_unit->mainOrderId == OrderId::CompletingArchonSummon
		)
			*u32_0066FF60 = 0x08;
		else
		if(	current_unit->id == UnitId::ProtossDarkArchon &&
			current_unit->mainOrderId == OrderId::CompletingArchonSummon
		)
			*u32_0066FF60 = 0x08;
		else
		if(orders_dat::RequirementsOffset[OrderId::CarrierStop] == 0)
			*u32_0066FF60 = 0x17;
		else
		if(parseRequirementOpcodes(
			current_unit,
			OrderId::CarrierStop,
			*ACTIVE_NATION_ID,
			0x00514CF8,
			orders_dat::RequirementsOffset[OrderId::CarrierStop]
		) == 1) {

			current_unit->userActionFlags |= 1;

			if(current_unit->mainOrderId != OrderId::Die) {

				bool bContinueLoop = true;

				while(bContinueLoop) {

					if(current_unit->orderQueueTail == NULL)
						bContinueLoop = false;
					else
					if(	orders_dat::CanBeInterrupted[current_unit->orderQueueTail->orderId] ||
						current_unit->orderQueueTail->orderId == OrderId::CarrierStop
					)
						removeOrderFromUnitQueue(current_unit,current_unit->orderQueueTail);
					else
						bContinueLoop = false;

				}

				current_unit->performAnotherOrder(OrderId::CarrierStop,0,0,NULL,UnitId::None);

			}

			prepareForNextOrder(current_unit);

		}

		current_unit = getActivePlayerNextSelection();

	}

}

;

void CMDRECV_Stop(u8 bCommandType) {

	CUnit* current_unit;
	static u32* u32_0066FF60 = (u32*)0x0066FF60;

	*selectionIndexStart = 0;
	current_unit = getActivePlayerNextSelection();

	while(current_unit != NULL) {

		*u32_0066FF60 = 0x00;

		if(orders_dat::RequirementsOffset[OrderId::Stop] == 0xFFFF)
			parseOrdersDatReqs();

		if(current_unit->playerId != *ACTIVE_NATION_ID)
			*u32_0066FF60 = 0x01;
		else
		if(!(current_unit->status & UnitStatus::Completed))
			*u32_0066FF60 = 0x14;
		else
		if(	current_unit->status & UnitStatus::DoodadStatesThing ||
			current_unit->lockdownTimer != 0 ||
			current_unit->stasisTimer != 0 ||
			current_unit->maelstromTimer != 0
		)
			*u32_0066FF60 = 0x0A;
		else
		if( current_unit->status & UnitStatus::Burrowed &&
			current_unit->id != UnitId::ZergLurker
		)
			*u32_0066FF60 = 0x0B;
		else
		if((current_unit->id == UnitId::TerranGhost ||
			current_unit->id == UnitId::Hero_SarahKerrigan ||
			current_unit->id == UnitId::Hero_AlexeiStukov ||
			current_unit->id == UnitId::Hero_SamirDuran ||
			current_unit->id == UnitId::Hero_InfestedDuran) &&
			current_unit->mainOrderId == OrderId::NukeTrack
		)
			*u32_0066FF60 = 0x08;
		else
		if(	current_unit->id == UnitId::ProtossArchon &&
			current_unit->mainOrderId == OrderId::CompletingArchonSummon
		)
			*u32_0066FF60 = 0x08;
		else
		if(	current_unit->id == UnitId::ProtossDarkArchon &&
			current_unit->mainOrderId == OrderId::CompletingArchonSummon
		)
			*u32_0066FF60 = 0x08;
		else
		if(orders_dat::RequirementsOffset[OrderId::Stop] == 0)
			*u32_0066FF60 = 0x17;
		else
		if(
			parseRequirementOpcodes(
				current_unit,
				OrderId::Stop,
				*ACTIVE_NATION_ID,
				0x00514CF8,
				orders_dat::RequirementsOffset[OrderId::Stop]
			) == 1
		)
			function_004754F0_Helper(
				current_unit,
				OrderId::Stop,
				0,
				0,
				NULL,
				UnitId::None,
				*(Point16*)(0x006D5C24),
				*(Point16*)(0x006D5C20),
				bCommandType != 0,
				1
			);

		current_unit = getActivePlayerNextSelection();

	}

}

;

} //namespace hooks

//-------- Helper function definitions. Do NOT modify! --------//

namespace {

const u32 Func_parseOrdersDatReqs = 0x0046D450;
void parseOrdersDatReqs() {

	__asm {
		PUSHAD
		CALL Func_parseOrdersDatReqs
		POPAD
	}

}

;

const u32 Func_parseRequirementOpcodes = 0x0046D610;
int parseRequirementOpcodes(CUnit* unit, u32 orderId, u32 playerId, u32 address, u32 req_offset) {

	static int return_value;

	__asm {
		PUSHAD
		MOV EAX, req_offset
		MOV ESI, unit
		PUSH address
		PUSH playerId
		PUSH orderId
		CALL Func_parseRequirementOpcodes
		MOV return_value, EAX
		POPAD
	}

	return return_value;

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

const u32 Func_Sub_4754F0 = 0x004754F0;
void function_004754F0_Helper(CUnit* unit, u32 orderId, int x, int y, CUnit* target, u32 unitId,
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

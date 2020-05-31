#include "CMDRECV_LiftOff.h"
#include <SCBW/api.h>

//Helper functions declaration

namespace {

bool unitIsTrainingOrMorphing(CUnit* unit);															//01500
bool unitIsGhost(CUnit* unit);																		//01570
void parseOrdersDatReqs();																			//6D450
int parseRequirementOpcodes(CUnit* unit, u32 orderId, u32 playerId, u32 address, u32 req_offset);	//6D610
void _doOrder(CUnit* unit, u32 orderId, int x, int y, bool stopPreviousOrders);						//74C70

} //unnamed namespace

namespace hooks {

void CMDRECV_Liftoff(s16 x, s16 y) {

	u16* const maxBoxRightValue =	(u16*)0x00628450;	//should usually be mapTileSize->width * 32
	u16* const maxBoxBottomValue =	(u16*)0x006284B4;	//should usually be mapTileSize->height * 32
	u16* const u16_00665610 =		(u16*)0x00665610;
	u32* const u32_0066FF60 =		(u32*)0x0066FF60;

	CUnit* unit;
	
	*selectionIndexStart = 0;

	unit = getActivePlayerNextSelection();

	if (
		unit != NULL &&
		getActivePlayerNextSelection() == NULL &&
		!unitIsTrainingOrMorphing(unit) &&
		unit->building.techType == TechId::None &&
		unit->building.upgradeType == UpgradeId::None &&
		unit->status & UnitStatus::GroundedBuilding &&
		units_dat::BaseProperty[unit->id] & UnitProperty::FlyingBuilding
	)
	{

		*u32_0066FF60 = 0;

		if (*u16_00665610 == 0xFFFF)
			parseOrdersDatReqs();

		if (unit->playerId != *ACTIVE_NATION_ID)
			*u32_0066FF60 = 0x01;
		else
		if(!(unit->status & UnitStatus::Completed))
			*u32_0066FF60 = 0x14;
		else
		if (
			unit->status & UnitStatus::DoodadStatesThing ||
			unit->lockdownTimer != 0 ||
			unit->stasisTimer != 0 ||
			unit->maelstromTimer != 0
		)
			*u32_0066FF60 = 0x0A;
		else
		if(unit->status & UnitStatus::Burrowed && unit->id != UnitId::ZergLurker)
			*u32_0066FF60 = 0x0B;
		else
		if(
			(unit->id == UnitId::TerranSCV && unit->mainOrderId == OrderId::ConstructingBuilding) ||
			(unitIsGhost(unit) && unit->mainOrderId == OrderId::NukeTrack) ||
			(unit->id == UnitId::ProtossArchon && unit->mainOrderId == OrderId::CompletingArchonSummon) ||
			(unit->id == UnitId::ProtossDarkArchon && unit->mainOrderId == OrderId::CompletingArchonSummon)
		)
			*u32_0066FF60 = 0x08;
		else
		if(*u16_00665610 == 0)
			*u32_0066FF60 = 0x17;
		else
		if(
			parseRequirementOpcodes(unit,OrderId::BuildingLiftoff,*ACTIVE_NATION_ID,0x00514CF8,*u16_00665610) &&
			x < *maxBoxRightValue && y < *maxBoxBottomValue
		)
		{
			unit->orderTo(OrderId::BuildingLiftoff, x, y);
			_doOrder(unit, units_dat::ReturnToIdleOrder[unit->id], x, y - 42, false);
		}

	}

}

;

} //namespace hooks

//-------- Helper function definitions. Do NOT modify! --------//

namespace {

//Identical to unitIsTrainingOrMorphing @ 0x00401500
bool unitIsTrainingOrMorphing(CUnit* unit) {

	bool bReturnValue;

	if(unit->buildQueue[unit->buildQueueSlot % 5] != UnitId::None)
		bReturnValue = true;
	else
	if(
		unit->secondaryOrderId == OrderId::BuildAddon &&
		unit->status & UnitStatus::GroundedBuilding &&
		unit->currentBuildUnit != NULL &&
		!(unit->currentBuildUnit->status & UnitStatus::Completed)
		)
		bReturnValue = true;
	else
	if(
		unit->mainOrderId != OrderId::BuildProtoss1 ||
		!(unit->status & UnitStatus::GroundedBuilding) ||
		unit->orderTarget.unit == NULL
		)
		bReturnValue = false;
	else
	if(!(unit->orderTarget.unit->status & UnitStatus::Completed))
		bReturnValue = true;
	else
		bReturnValue = false;

	return bReturnValue;

}

;

//Identical to unitIsGhost @ 0x00401570
bool unitIsGhost(CUnit* unit) {

	bool return_value;
	
	if (
		unit->id == UnitId::TerranGhost ||
		unit->id == UnitId::Hero_SarahKerrigan ||
		unit->id == UnitId::Hero_AlexeiStukov ||
		unit->id == UnitId::Hero_SamirDuran ||
		unit->id == UnitId::Hero_InfestedDuran
	)
		return_value = true;
	else
		return_value = false;

	return return_value;

}

;

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

} //unnamed namespace

//End of helper functions
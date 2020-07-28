#include "right_click_CMDACT.h"
#include "../SCBW/api.h"

//helper functions def
namespace {

bool unitIsOwnedByCurrentPlayer(CUnit* unit); 													//01170
Bool32 function_00455660(CUnit* unit);															//55660
Bool32 function_004556D0(CUnit* unit);															//556D0
Bool32 function_00455A00(CUnit* unit);															//55A00
GuiOverlay* function_00456490(CUnit* unit, int x, int y);										//56490
CUnit* function_0046F3A0(u32 unk1, u32 unk2);													//6F3A0
void QueueGameCommand(u8* params, u32 param_length);											//85BD0
void GroundAttackInit(int x, int y);															//88660
void PlayWorkerActionSound(CUnit* unit);														//8F4D0
void getMinimapCursorPos(u32* x, u32* y);														//A3D70
void CMDACT_RightClickOrder_Helper(CUnit* unit, s32 x, s32 y, u32 unitId, Bool32 isQueuedOrder);//C0380
Bool32 _STrans442(int x, int y);																//D1140
u8 getRightClickActionOrder(CUnit* unit);														//E5EA0

} //unnamed namespace

#pragma pack(1)

struct RightClickPacket {
	u8 packetId;
	s16 x;
	s16 y;
	u16 wTargetID;
	u16 wUnitType;
	u8 bCommandType;
};

C_ASSERT(sizeof(RightClickPacket) == 10);

namespace hooks {

//Right click on minimap
void CMDACT_RightClick(s16 someX, s16 someY) {

	if (
		*activePortraitUnit != NULL &&
		(*activePortraitUnit)->playerId == *LOCAL_NATION_ID
	)
	{

		int i = 0;
		bool loopBreakCondition = false;

		while (i < SELECTION_ARRAY_LENGTH && !loopBreakCondition) {
			if (
				clientSelectionGroup->unit[i] != NULL &&
				function_004556D0(clientSelectionGroup->unit[i]) != 0
			)
				loopBreakCondition = true;
			else
				i++;
		}

		if (loopBreakCondition && i < SELECTION_ARRAY_LENGTH) {

			i = 0;
			loopBreakCondition = false;

			while (i < SELECTION_ARRAY_LENGTH && !loopBreakCondition) {
				if (
					clientSelectionGroup->unit[i] != NULL &&
					function_00455660(clientSelectionGroup->unit[i]) != 0
				)
					loopBreakCondition = true;
				else
					i++;
			}

			if (loopBreakCondition && i < SELECTION_ARRAY_LENGTH) {

				u32 x = someX;
				u32 y = someY;
				RightClickPacket packet;

				//modify x and y
				getMinimapCursorPos(&x, &y);

				packet.packetId = 0x14; //Right Click
				packet.x = x;
				packet.y = y;
				packet.wTargetID = 0;
				packet.wUnitType = UnitId::None;
				packet.bCommandType = (*IS_HOLDING_SHIFT);

				QueueGameCommand((u8*)&packet, 10);

				PlayWorkerActionSound(*activePortraitUnit);
				GroundAttackInit(x, y);

			}

		}

	}

}

;

//Function that call CMDACT_RightClickOrder, and filter what selections
//are allowed to accept a right click
void function_004564E0(s16 someX, s16 someY) {

	if (_STrans442(someX, someY) == 0) {

		const CUnit** unit_0059724C = (const CUnit**)0x0059724C;
		u8 playerId;

		if (*unit_0059724C != NULL)
			playerId = (*unit_0059724C)->playerId;
		else
			playerId = 0xFF;

		if (
			playerId == *LOCAL_NATION_ID &&
			*activePortraitUnit != NULL &&
			unitIsOwnedByCurrentPlayer(*activePortraitUnit)
		)
		{

			int i = 0;
			bool loop_stop_reached = false;

			//Note: this is where eggs and scv constructing are excluded for right-clicking
			do {

				CUnit* current_unit = clientSelectionGroup->unit[i];

				if (current_unit != NULL && function_004556D0(current_unit) != 0)
					loop_stop_reached = true;
				else
					i++;

			} while (!loop_stop_reached && i < SELECTION_ARRAY_LENGTH);

			if (loop_stop_reached) {

				int x = someX + *MoveToX;
				int y = someY + *MoveToY;
				CUnit* clicked_unit = function_0046F3A0(y, x);

				if (
					*clientSelectionCount != 1 ||
					*activePortraitUnit != clicked_unit ||
					(
						clicked_unit != NULL && 
						clicked_unit->status & UnitStatus::GroundedBuilding &&
						clicked_unit->isFactory()
					)
				)
				{

					//this filter eggs, but not scv (if previous filter was removed/changed to let it reach here)
					if (clicked_unit == NULL) {

						i = 0;
						loop_stop_reached = false;

						do {

							CUnit* current_unit = clientSelectionGroup->unit[i];

							if (current_unit != NULL && function_00455660(current_unit) != 0)
								loop_stop_reached = true;
							else
								i++;

						} while (!loop_stop_reached && i < SELECTION_ARRAY_LENGTH);

					}

					//note: always doing that if clicked_unit != NULL because set to true before
					if (loop_stop_reached) {

						u16 unitId;
						GuiOverlay* unknown = function_00456490(clicked_unit, x, y);

						if (unknown != 0)
							unitId = unknown->id;
						else
							unitId = UnitId::None;

						CMDACT_RightClickOrder_Helper(clicked_unit, x, y, unitId, *IS_HOLDING_SHIFT);

						if (function_00455A00(clicked_unit) != 0)
							PlayWorkerActionSound(*activePortraitUnit);

					}

				}

			}

		}

	}

}

;

//Right click on main screen
void CMDACT_RightClickOrder(CUnit* unit, s16 x, s16 y, u16 unitId, Bool8 isQueuedOrder) {

	RightClickPacket packet;
	StoredUnit stored_unit;

	packet.packetId = 0x14;  //Right Click
	packet.x = x;
	packet.y = y;

	if (unit != NULL) {

		int index;

		//return index of unit in unitTable plus one (unit is in unitTable[index - 1])
		//(unit - unitTable) / sizeof(CUnitLayout or CUnit) doesn't give the same result
		__asm {
			PUSHAD
			MOV ECX, unit
			SUB ECX, unitTable
			MOV EAX, 0x86186187
			MUL ECX
			SUB ECX, EDX
			SHR ECX, 1
			ADD ECX, EDX
			SHR ECX, 0x8
			INC ECX
			MOV index, ECX
			POPAD
		}

		if (index > UNIT_ARRAY_LENGTH)
			stored_unit.fullValue = 0;
		else {
			stored_unit.innerValues.index = index;
			stored_unit.innerValues.unitId = unit->targetOrderSpecial;
		}

	}
	else
		stored_unit.fullValue = 0;

	packet.wTargetID = stored_unit.fullValue;
	packet.wUnitType = unitId;
	packet.bCommandType = isQueuedOrder;

	QueueGameCommand((u8*)&packet, 10);
	
}

;

} //hooks

;

//-------- Helper function definitions. Do NOT modify! --------//

namespace {

//Identical to 0x00401170
bool unitIsOwnedByCurrentPlayer(CUnit* unit) {

	bool return_value;

	if(*IS_IN_REPLAY != 0 || unit->playerId != *LOCAL_NATION_ID)
		return_value = false;
	else
		return_value = true;

	return return_value;

}

;

//Identical to 0x00455660
//2 functions here are the only users of this
Bool32 function_00455660(CUnit* unit) {

	Bool32 result;

	u8 rightClickAction;

	if (unit->id == UnitId::ZergLurker && unit->status & UnitStatus::Burrowed)
		rightClickAction = RightClickActions::NoMove_NormalAttack;
	else {

		rightClickAction = units_dat::RightClickAction[unit->id];

		if (
			rightClickAction == RightClickActions::NoCommand_AutoAttack &&
			unit->status & UnitStatus::GroundedBuilding &&
			unit->isFactory()
		)
			rightClickAction = RightClickActions::NormalMove_NoAttack;

	}

	if (
		rightClickAction == RightClickActions::NormalMove_NormalAttack ||
		rightClickAction == RightClickActions::NormalMove_NoAttack ||
		rightClickAction == RightClickActions::Harvest ||
		rightClickAction == RightClickActions::HarvestAndRepair ||
		rightClickAction == RightClickActions::Unknown
	)
		result = 1;
	else  //RightClickActions::NoCommand_AutoAttack or RightClickActions::NoMove_NormalAttack or invalid value
		result = 0;

	return result;

}

;

//Identical to 0x004556D0
//2 functions here are the only users of this
Bool32 function_004556D0(CUnit* unit) {

	Bool32 result;

	if (
		unit->status & UnitStatus::DoodadStatesThing ||
		unit->lockdownTimer != 0 ||
		unit->stasisTimer != 0 ||
		unit->maelstromTimer != 0 ||
		(unit->status & UnitStatus::Burrowed && unit->id != UnitId::ZergLurker) ||
		(unit->id == UnitId::TerranSCV && unit->mainOrderId == OrderId::ConstructingBuilding) ||
		(
			(
				unit->id == UnitId::TerranGhost || unit->id == UnitId::Hero_SarahKerrigan || unit->id == UnitId::Hero_AlexeiStukov || 
				unit->id == UnitId::Hero_SamirDuran || unit->id == UnitId::Hero_InfestedDuran
			) &&
			unit->mainOrderId == OrderId::NukeTrack
		) ||
		(unit->id == UnitId::ProtossArchon && unit->mainOrderId == OrderId::CompletingArchonSummon) ||
		getRightClickActionOrder(unit) == RightClickActions::NoCommand_AutoAttack
	)
		result = 0;
	else
		result = 1;

	return result;

}

;

const u32 Func_sub_455A00 = 0x00455A00;
Bool32 function_00455A00(CUnit* unit) {

	static Bool32 result;

	__asm {
		PUSHAD
		PUSH unit
		CALL Func_sub_455A00
		MOV result, EAX
		POPAD
	}

	return result;

}

;

const u32 Func_sub_456490 = 0x00456490;
//return type was chosen because element at 08 is 16 bits,
//could have been UnknownUserStruct or something else too.
GuiOverlay* function_00456490(CUnit* unit, int x, int y) {

	GuiOverlay* return_value;

	__asm {
		PUSHAD
		MOV EAX, x
		MOV EDI, y
		PUSH unit
		CALL Func_sub_456490
		MOV return_value, EAX
		POPAD
	}

	return return_value;

}

;

const u32 Func_function_0046F3A0 = 0x0046F3A0;
CUnit* function_0046F3A0(u32 unk1, u32 unk2) {

	static CUnit* returned_unit;

	__asm {
		PUSHAD
		PUSH unk1
		PUSH unk2
		CALL Func_function_0046F3A0
		MOV returned_unit, EAX
		POPAD
	}

	return returned_unit;

}

;

const u32 Func_QueueGameCommand = 0x00485BD0;
void QueueGameCommand(u8* params, u32 param_length) {

	__asm {
		PUSHAD
		MOV EDX, param_length
		MOV ECX, params
		CALL Func_QueueGameCommand
		POPAD
	}

}

;

const u32 Func_GroundAttackInit = 0x00488660;
void GroundAttackInit(int x, int y) {
	__asm {
		PUSHAD
		MOV EAX, y
		MOV ECX, x
		CALL Func_GroundAttackInit
		POPAD
	}
}

;

const u32 Func_PlayWorkerActionSound = 0x0048F4D0;
void PlayWorkerActionSound(CUnit * unit) {
	__asm {
		PUSHAD
		MOV EAX, unit
		CALL Func_PlayWorkerActionSound
		POPAD
	}
}

;

const u32 Func_getMinimapCursorPos = 0x004A3D70;
//use pointers to directly modify x and y within the function
void getMinimapCursorPos(u32* x, u32* y) {
	__asm {
		PUSHAD
		MOV EAX, x
		MOV ECX, y
		CALL Func_getMinimapCursorPos
		POPAD
	}
}

;

const u32 Func_CMDACT_RightClickOrder = 0x004C0380;
void CMDACT_RightClickOrder_Helper(CUnit* unit, s32 x, s32 y, u32 unitId, Bool32 isQueuedOrder) {
	__asm {
		PUSHAD
		MOV ESI, unit
		PUSH isQueuedOrder
		PUSH unitId
		PUSH y
		PUSH x
		CALL Func_CMDACT_RightClickOrder
		POPAD
	}
}

;

const u32 Func__STrans442 = 0x004D1140;
Bool32 _STrans442(int x, int y) {

	static Bool32 result;

	__asm {
		PUSHAD
		MOV ECX, x
		MOV EAX, y
		CALL Func__STrans442
		MOV result, EAX
		POPAD
	}

	return result;

}

;

//Identical to getRightClickActionOrder @ 0x004E5EA0;
u8 getRightClickActionOrder(CUnit* unit) {

	u8 result;

	if( unit->id == UnitId::ZergLurker && unit->status & UnitStatus::Burrowed )
		result = RightClickActions::NoMove_NormalAttack;
	else {

		u8 temp_result = units_dat::RightClickAction[unit->id];

		if(
			temp_result == RightClickActions::NoCommand_AutoAttack &&
			unit->status & UnitStatus::GroundedBuilding &&
			unit->isFactory()
		)
			result = RightClickActions::NormalMove_NoAttack;
		else
			result = temp_result;

	}

	return result;

}

;

} //Unnamed namespace

//End of helper functions

#pragma pack()
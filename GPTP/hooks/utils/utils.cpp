#include "utils.h"
#include <SCBW/api.h>

//Those functions are less likely to be useful (especially if
//modified alone against as a part of a larger modification),
//or may cause more trouble if modified the wrong way, so they
//are put here a bit on the side.

//helper functions def

namespace {

void* SMemAlloc(u32 amount, u32 unkFilename, u32 unkLine, u32 defaultValue);	//1006A (based on Storm.dll definition)
void SetCallbackTimer(BinDlg* dialog, u32 unk1, u32 unk2, u32 unk3);			//16090
void function_0049A7F0(CUnit* unit);											//9A7F0

u32 fxnInteract(BinDlg* dialog, u32 data_struct_offset);

} //unnamed namespace

namespace hooks {

//00419D20
void InitializeDialog(BinDlg* dialog, void* dlgFunc) {

	struct {
		u32 unknown_value;		//[EBP-0x14]	[00]
		u32 always_zero_here;	//[EBP-0x10]	[04]
		u32 not_allocated_2;	//[EBP-0x0C]	[08]
		u16 always_14_here;		//[EBP-0x08]	[0C]
		s16 mouseX;				//[EBP-0x06]	[0E]
		s16 mouseY;				//[EBP-0x04]	[10]
		u16 not_allocated_1;	//[EBP-0x02]	[12]
	} stack_placeholder;

	//not allocated on original code, but result
	//is probably more true to the original like
	//this
	stack_placeholder.not_allocated_1 = 0;
	stack_placeholder.not_allocated_2 = 0;

	dialog->flags |= (BinDlgFlags::NoClickSound + BinDlgFlags::VerticalAlignmentBottom);
	dialog->onInitDialog0x46.unk4a = 0;

	if(dlgFunc != NULL)
		dialog->fxnInteract = dlgFunc;

	dialog->onInitDialog0x36.responseAreaBitArray = (u32*)
		SMemAlloc(
		dialog->onInitDialog0x36.responseAreaWidth * dialog->onInitDialog0x36.responseAreaHeight,
			0x00505F98,
			481, //0x000001E1
			0
		);

	dialog->index = 0;

	stack_placeholder.mouseX = mouse->x;
	stack_placeholder.always_14_here = 14;	//0x000E
	stack_placeholder.unknown_value = 7;	//0x00000007
	stack_placeholder.always_zero_here = 0;
	stack_placeholder.mouseY = mouse->y;

	fxnInteract(dialog, (u32)&stack_placeholder);

	dialog->bounds.right += dialog->bounds.left;
	dialog->bounds.bottom += dialog->bounds.top;

	stack_placeholder.mouseY = mouse->y;
	stack_placeholder.always_14_here = 14;	//0x000E
	stack_placeholder.unknown_value = 10;	//0x0000000A
	stack_placeholder.always_zero_here = 0;
	stack_placeholder.mouseX = mouse->x;

	fxnInteract(dialog, (u32)&stack_placeholder);

	stack_placeholder.mouseX = mouse->x;
	stack_placeholder.always_14_here = 14;	//0x000E
	stack_placeholder.unknown_value = 0;
	stack_placeholder.always_zero_here = 0;
	stack_placeholder.mouseY = mouse->y;
	
	fxnInteract(dialog, (u32)&stack_placeholder);

	SetCallbackTimer(dialog,12,100,0);

	//equivalent to "if(dialog->flags & BinDlgFlags::VerticalAlignmentBottom) dialog->flags -= BinDlgFlags::VerticalAlignmentBottom;"
	dialog->flags &= ~BinDlgFlags::VerticalAlignmentBottom;

}

;

//0047B770
bool unit_IsStandardAndMovable(CUnit* unit) {

	bool return_value;

	if(
		units_dat::BaseProperty[unit->id] & UnitProperty::Building ||
		units_dat::BaseProperty[unit->id] & UnitProperty::NeutralAccessories ||
		unit->status & UnitStatus::DoodadStatesThing ||
		unit->lockdownTimer != 0 ||
		unit->stasisTimer != 0 ||
		unit->maelstromTimer != 0 ||
		(
			unit->id >= UnitId::Special_FloorMissileTrap &&
			unit->id <= UnitId::Special_RightWallFlameTrap
		) ||
		unit->id == UnitId::TerranVultureSpiderMine ||
		unit->id == UnitId::ZergEgg ||
		unit->id == UnitId::Critter_Rhynadon ||
		unit->id == UnitId::Critter_Bengalaas ||
		unit->id == UnitId::Critter_Ragnasaur ||
		unit->id == UnitId::Critter_Kakaru ||
		unit->id == UnitId::Critter_Scantid ||
		unit->id == UnitId::Critter_Ursadon ||
		unit->id == UnitId::Spell_DarkSwarm ||
		unit->id == UnitId::Spell_DisruptionWeb
	)
		return_value = false;
	else
		return_value = true;

	return return_value;

} //bool unit_IsStandardAndMovable(CUnit* unit)

;

//0049A850
CUnit* getActivePlayerFirstSelection() {

	CUnit* unit;

	if(*selectionIndexStart >= 12)
		unit = NULL;
	else {

		s32 playerId = *ACTIVE_PLAYER_ID;
		u8 index = *selectionIndexStart;
		bool bStopLoop = false;

		while(!bStopLoop) {

			unit = playersSelections->unit[playerId][index];

			if(unit == NULL)
				bStopLoop = true;
			else
			if(
				unit->sprite != NULL &&
				(unit->mainOrderId != OrderId::Die || unit->mainOrderState != 1)
			) 
					bStopLoop = true;

			if(!bStopLoop)
				function_0049A7F0(unit); //remove the unit from playersSelections->unit[playerId][index] among other things

		}

		*selectionIndexStart = *selectionIndexStart + 1;
		
	}

	return unit;

}

;

} //namespace hooks

;

//-------- Helper function definitions. Do NOT modify! --------//

namespace {

//Risk of memory leaks or memory corruption or stuff if such a thing is
//used the wrong way or in the wrong place!
const u32 Func_SMemAlloc = 0x0041006A;
void* SMemAlloc(u32 amount, u32 unkFilename, u32 unkLine, u32 defaultValue) {

	static void* pointer_on_allocated_memory;

	__asm {
		PUSHAD
		PUSH defaultValue		//probably what to fill the memory with
		PUSH unkLine			//unknown use, seems to use hardcoded value
		PUSH unkFilename		//unknown use, seems to use hardcoded adress (of some text?)
		PUSH amount				//amount of memory to allocate
		CALL Func_SMemAlloc
		MOV pointer_on_allocated_memory, EAX
		POPAD
	}

	return pointer_on_allocated_memory;

}

;

//Likely to call SMemAlloc!
const u32 Func_SetCallbackTimer = 0x00416090;
void SetCallbackTimer(BinDlg* dialog, u32 unk1, u32 unk2, u32 unk3) {

	__asm {
		PUSHAD
		PUSH unk3
		PUSH unk2
		MOV EAX, unk1
		MOV ECX, dialog
		CALL Func_SetCallbackTimer
		POPAD
	}

}

;

const u32 Func_Sub49A7F0 = 0x0049A7F0;
void function_0049A7F0(CUnit* unit) {

	__asm {
		PUSHAD
		MOV EDI, unit
		CALL Func_Sub49A7F0
		POPAD
	}

}

;

u32 fxnInteract(BinDlg* dialog, u32 data_struct_offset) {

	static u32 return_value;
	static u32 fxnInteract;

	fxnInteract = (u32)dialog->fxnInteract;

	__asm {
		PUSHAD
		MOV ESI, dialog					//possibly useless, but just in case
		MOV ECX, dialog
		MOV EDX, data_struct_offset
		CALL fxnInteract
		MOV return_value, EAX
		POPAD
	}

	return return_value;

}

;

} //Unnamed namespace

//End of helper functions

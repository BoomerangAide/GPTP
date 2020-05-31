//Injector source file for the CMDRECV Siege Tank hooks module.
#include "CMDRECV_Selection.h"
#include <hook_tools.h>

namespace {

void __declspec(naked) function_004965D0Wrapper() {

	static u32 unkBoolean;
	static u8 selectionGroup;
	
	__asm {
		PUSH EBP
		MOV EBP, ESP
		MOV ECX, [EBP+0x08]
		MOV unkBoolean, ECX
		MOV selectionGroup, AL
		PUSHAD
	}
	
	hooks::function_004965D0(selectionGroup,unkBoolean);
	
	__asm {
		POPAD
		MOV ESP, EBP
		POP EBP
		RETN 4
	}
	
}
	
;

void __declspec(naked) function_00496940Wrapper() {

	static u32 bGroupSlot;
	
	__asm {
		PUSH EBP
		MOV EBP, ESP
		MOVZX EAX, byte ptr [EBP+0x08]
		MOV bGroupSlot, EAX
		PUSHAD
	}
	
	hooks::function_00496940(bGroupSlot);
	
	__asm {
		POPAD
		POP EBP
		RETN 4
	}
	
}
	
;

void __declspec(naked) function_0049AF80Wrapper() {

	static CUnit* unit;
	static u32 playerId;
	static u32 selection_slot;
	Bool32 return_value;

	__asm {
		MOV unit, ESI
		MOV playerId, EDI
		MOV selection_slot, EBX
		PUSHAD
	}

	if (hooks::function_0049AF80(unit, playerId, selection_slot))
		return_value = 1;
	else
		return_value = 0;

	__asm {
		POPAD
		MOV EAX, return_value
		RETN
	}

}

;

//Note: using EDX (through DL) because it is overwritten by
//the original code without being read
void __declspec(naked) CMDRECV_ShiftSelectWrapper() {

	static u8 packetId;
	static s8 bCount;
	static StoredUnit* selectedCommandUnits;

	__asm {
		PUSH EBP
		MOV EBP, ESP
		MOV EAX, [EBP+0x08]
		MOV DL, [EAX]
		MOV packetId, DL
		MOV DL, [EAX+1]
		MOV bCount, DL
		ADD EAX, 2
		MOV selectedCommandUnits, EAX
		PUSHAD
	}

	hooks::CMDRECV_ShiftSelect(packetId,bCount,selectedCommandUnits);

	__asm {
		POPAD
		MOV ESP, EBP
		POP EBP
		RETN 4
	}

}

;

//Note: using EDX (through DL) because it is overwritten by
//the original code without being read
void __declspec(naked) CMDRECV_SelectWrapper() {

	static u8 packetId;
	static s8 bCount;
	static StoredUnit* selectedCommandUnits;

	__asm {
		PUSH EBP
		MOV EBP, ESP
		MOV EAX, [EBP+0x08]
		MOV DL, [EAX]
		MOV packetId, DL
		MOV DL, [EAX+1]
		MOV bCount, DL
		ADD EAX, 2
		MOV selectedCommandUnits, EAX
		PUSHAD
	}

	hooks::CMDRECV_Select(packetId,bCount,selectedCommandUnits);

	__asm {
		POPAD
		MOV ESP, EBP
		POP EBP
		RETN 4
	}

}

;

void __declspec(naked) CMDRECV_HotkeyWrapper() {

	static u8 packetId;
	static u8 bGroupType;
	static u8 bGroupSlot;

	__asm {
		MOV AL, [ECX+2]
		MOV bGroupSlot, AL
		MOV AL, [ECX+1]
		MOV bGroupType, AL
		MOV AL, [ECX]
		MOV packetId, AL
		PUSHAD
	}

	hooks::CMDRECV_Hotkey(packetId,bGroupType,bGroupSlot);

	__asm {
		POPAD
		RETN
	}

}

;

}//unnamed namespace

namespace hooks {

void injectCMDRECV_SelectionHooks() {
	jmpPatch(function_004965D0Wrapper,	0x004965D0, 1);
	jmpPatch(function_00496940Wrapper,	0x00496940, 1);
	jmpPatch(function_0049AF80Wrapper,	0x0049AF80, 2);
	jmpPatch(CMDRECV_ShiftSelectWrapper,0x004C2560, 1);
	jmpPatch(CMDRECV_SelectWrapper,		0x004C2750, 1);
	jmpPatch(CMDRECV_HotkeyWrapper,		0x004C2870, 0);
}

} //hooks
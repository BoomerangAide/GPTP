#include "rally_point.h"
#include "../hook_tools.h"

namespace {

///
/// Main functions wrappers
///

void __declspec(naked) orderNewUnitToRallyWrapper() {

	static CUnit* unit;
	static CUnit* factory;

	__asm {
		MOV unit, EAX
		MOV factory, ECX
		PUSHAD
	}

	hooks::orderNewUnitToRally(unit, factory);

	__asm {
		POPAD
		RETN
	}

}

;

void __declspec(naked) setRallyPositionWrapper() {

	static CUnit* unit;
	static u16 x;
	static u16 y;

	__asm {

		PUSH EBP
		MOV EBP, ESP

		MOV unit, EAX

		MOV CX, [EBP+0x08]
		MOV x, CX

		MOV DX, [EBP+0x0C]
		MOV y, DX

		PUSHAD

	}

	hooks::setRallyPosition(unit, x, y);

	__asm {
		POPAD
		POP EBP
		RETN 0x08
	}

}

;

void __declspec(naked) setRallyUnitWrapper() {

	static CUnit* unit;
	static CUnit* target;

	__asm {
		MOV unit, ECX
		MOV target, EAX
		PUSHAD
	}

	hooks::setRallyUnit(unit, target);

	__asm {
		POPAD
		RETN
	}
}

;

///
/// Additional wrappers for uses of the logic that
/// would be hardcoded instead of using the functions
///

//Replace hardcoded execution at 0x00456256
//Jump to 0x00456384 because the code equivalent
//to the function is followed by a jmp to that
//position

void __declspec(naked) setRallyPositionWrapper_Jmp1() {

	static const u32 Hook_SetRallyGenericJmpBack1 = 0x00456384;

	static CUnit* unit;
	static u16 x;
	static u16 y;

	__asm {
		MOV unit, EDI
		MOV x, BX
		MOV y, SI
		PUSHAD
	}

	hooks::setRallyPosition(unit, x, y);

	__asm {
		POPAD
		JMP Hook_SetRallyGenericJmpBack1
	}
}

;

//Replace hardcoded execution at 0x0049ACEF
//Jump to 0x0049AD77 because the code equivalent
//to the function is followed by a jmp to that
//position

void __declspec(naked) setRallyPositionWrapper_Jmp2() {

	static const u32 Hook_SetRallyGenericJmpBack2 = 0x0049AD77;

	static CUnit* unit;
	static u16 x;
	static u16 y;

	__asm {
		MOV unit, ESI
		MOV x, CX
		MOV y, DX
		PUSHAD
	}

	hooks::setRallyPosition(unit, x, y);

	__asm {
		POPAD
		JMP Hook_SetRallyGenericJmpBack2
	}

}

;

//Replace hardcoded execution at 0x0045620D
//Jump to 0x00456384 because the code equivalent
//to the function is followed by a jmp to that
//position

void __declspec(naked) setRallyUnitWrapper_Jmp1() {

	static const u32 Hook_SetRallyGenericJmpBack1 =	0x00456384;

	static CUnit* unit;
	static CUnit* target;

	__asm {
		MOV unit, EDI
		MOV target, EAX
		PUSHAD
	}

	hooks::setRallyUnit(unit, target);

	__asm {
		POPAD
		JMP Hook_SetRallyGenericJmpBack1
	}

}

;

//Replace hardcoded execution at 0x0049ACB0
//Jump to 0x0049AD77 because the code equivalent
//to the function is followed by a jmp to that
//position

void __declspec(naked) setRallyUnitWrapper_Jmp2() {

	static const u32 Hook_SetRallyGenericJmpBack2 =	0x0049AD77;

	static CUnit* unit;
	static CUnit* target;

	__asm {
		MOV unit, ESI
		MOV target, EBX
		PUSHAD
	}

	hooks::setRallyUnit(unit, target);

	__asm {
		POPAD
		JMP Hook_SetRallyGenericJmpBack2
	}
}

;

} //unnamed namespace

namespace hooks {

void injectRallyHooks() {
	
	jmpPatch(setRallyPositionWrapper,		0x00466910, 2);		//this is the actual function
	jmpPatch(setRallyUnitWrapper,			0x00466B40, 1);		//this is the actual function	
	jmpPatch(orderNewUnitToRallyWrapper,	0x00466F50, 0);		//this is the actual function
 
	jmpPatch(setRallyPositionWrapper_Jmp1,	0x00456256, 19);	//24 - 5 of jump //CMDRECV_RightClick hook should be disabled
	jmpPatch(setRallyPositionWrapper_Jmp2,	0x0049ACEF, 19);	//24 - 5 of jump //receive_command() from hooks\recv_commands\receive_command() hook should be disabled

	jmpPatch(setRallyUnitWrapper_Jmp1,		0x0045620D, 35);	//40 - 5 of jump //CMDRECV_RightClick hook should be disabled
	jmpPatch(setRallyUnitWrapper_Jmp2,		0x0049ACB0, 37);	//42 - 5 of jump //receive_command() from hooks\recv_commands\receive_command() hook should be disabled

}

} //hooks

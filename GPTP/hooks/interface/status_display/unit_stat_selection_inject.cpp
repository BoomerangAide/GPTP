#include "unit_stat_selection.h"
#include <hook_tools.h>

namespace {

void __declspec(naked) UnitStatCond_Selection_Wrapper() {

	static Bool32 result;

	__asm PUSHAD

	if(hooks::UnitStatCond_Selection())
		result = 1;
	else
		result = 0;

	__asm {
		POPAD
		MOV EAX, result
		RETN
	}

}

;

void __declspec(naked) UnitStatAct_Selection_Wrapper() {

	static BinDlg* dialog;

	__asm {
		MOV dialog, EAX
		PUSHAD
	}

	hooks::UnitStatAct_Selection(dialog);

	__asm {
		POPAD
		RETN
	}

}

;

const u32 JumpBackPoint_00456FD1 = 0x00456FD1;
void __declspec(naked) statdata_UnitWireframeSelectUpdate_Part1_Wrapper() {

	static BinDlg* dialog;
	static u32 return_value;

	__asm {
		PUSH EBP
		MOV EBP, ESP
		SUB ESP, 0x18
		MOV dialog, ECX
		PUSHAD
	}

	return_value = hooks::statdata_UnitWireframeSelectUpdate_Part1(dialog);

	__asm {
		POPAD
		PUSH EBX
		PUSH ESI
		PUSH EDI
		MOV ECX, return_value
		MOV EAX, 0x0068C1FC
		MOV EAX, [EAX]
		MOV EDI, dialog
		JMP JumpBackPoint_00456FD1
	}

}

;

void __declspec(naked) statdata_UnitWireframeSelectUpdate_Part3_Wrapper() {

	static BinDlg* dialog;

	__asm {
		MOV dialog, EDI
		POP EDI
		POP ESI
		POP EBX
		PUSHAD
	}

	hooks::statdata_UnitWireframeSelectUpdate_Part3(dialog);

	__asm {
		POPAD
		MOV ESP, EBP
		POP EBP
		RETN 8
	}

}

;

}//unnamed namespace

namespace hooks {

void injectUnitStatSelectionHooks() {

	jmpPatch(UnitStatCond_Selection_Wrapper,					0x00424660, 2);
	jmpPatch(UnitStatAct_Selection_Wrapper,						0x00425960, 7);

	//those 2 below must be hooked together, or neither should be hooked
	jmpPatch(statdata_UnitWireframeSelectUpdate_Part1_Wrapper,	0x00456F50, 1);
	jmpPatch(statdata_UnitWireframeSelectUpdate_Part3_Wrapper,	0x00457046, 5);

}

} //hooks
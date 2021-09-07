//Injector source file for the CMDRECV Right Click hooks module.
#include "CMDRECV_RightClick.h"
#include <hook_tools.h>

namespace {

void __declspec(naked) CMDRECV_RightClick_Proc_Wrapper() {

	static s16 x;
	static s16 y;
	static CUnit* target;
	static u16 wUnitType;
	static u8 bCommandType;

	__asm {
		PUSH EBP
		MOV EBP, ESP
		MOV EAX, [EBP+0x08]
		MOV x, AX
		MOV EAX, [EBP+0x0C]
		MOV y, AX
		MOV EAX, [EBP+0x10]
		MOV target, EAX
		MOV EAX, [EBP+0x14]
		MOV wUnitType, AX
		MOV EAX, [EBP+0x18]
		MOV bCommandType, AL
		PUSHAD
	}

	hooks::CMDRECV_RightClick_Proc(x, y, target, wUnitType, bCommandType);

	__asm {
		POPAD
		MOV ESP, EBP
		POP EBP
		RETN 0x14
	}

}

;

}//unnamed namespace

namespace hooks {

void injectCMDRECV_RightClickHook() {
	jmpPatch(CMDRECV_RightClick_Proc_Wrapper, 0x004560D0, 1);
}
	
} //hooks

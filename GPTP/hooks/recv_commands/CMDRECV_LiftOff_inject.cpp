//Injector source file for the CMDRECV Lift Off hook module.
#include "CMDRECV_LiftOff.h"
#include <hook_tools.h>

namespace {

void __declspec(naked) CMDRECV_Liftoff_Wrapper() {

	static s16 x;
	static s16 y;

	__asm {
		PUSH EBP
		MOV EBP, ESP
		MOV EAX, [EBP+0x08]
		MOV CX, WORD PTR [EAX+0x1]
		MOV x, CX
		MOV CX, WORD PTR [EAX+0x3]
		MOV y, CX
		PUSHAD
	}

	hooks::CMDRECV_Liftoff(x, y);

	__asm {
		POPAD
		POP EBP
		RETN 4
	}

}

;

;

}//unnamed namespace

namespace hooks {
	void injectCMDRECV_LiftOffHook() {
		jmpPatch(CMDRECV_Liftoff_Wrapper, 0x004C1620, 7);
	}
} //hooks
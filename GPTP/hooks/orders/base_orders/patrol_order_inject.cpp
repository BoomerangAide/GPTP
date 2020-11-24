//Injector source file for the Patrol Order hook module.
#include "patrol_order.h"
#include <hook_tools.h>

namespace {

void __declspec(naked) orders_PatrolWrapper() {

	static CUnit* unit;

	__asm {
		PUSH EBP
		MOV EBP, ESP
		MOV unit, EAX
		PUSHAD
	}

	hooks::orders_Patrol(unit);

	__asm {
		POPAD
		MOV ESP, EBP
		POP EBP
		RETN
	}
		
}

;

}//unnamed namespace

namespace hooks {

void injectPatrolOrderHook() {
	jmpPatch(orders_PatrolWrapper,	0x004780F0, 1);
}

} //hooks
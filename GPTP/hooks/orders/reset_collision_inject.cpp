#include "reset_collision.h"
#include <hook_tools.h>

namespace {

	void __declspec(naked) orders_ResetCollision2Wrapper() {

		static CUnit* unit;

		__asm {
			PUSH EBP
			MOV EBP, ESP
			MOV unit, EAX
			PUSHAD
		}

		hooks::orders_ResetCollision2(unit);

		__asm {
			POPAD
			MOV ESP, EBP
			POP EBP
			RETN
		}

	}

	;

	void __declspec(naked) orders_ResetCollision1Wrapper() {

		static CUnit* unit;

		__asm {
			PUSH EBP
			MOV EBP, ESP
			MOV unit, EAX
			PUSHAD
		}

		hooks::orders_ResetCollision1(unit);

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

void injectResetCollisionHooks() {
	jmpPatch(orders_ResetCollision2Wrapper,	0x0042E320, 1);
	jmpPatch(orders_ResetCollision1Wrapper,	0x004671B0, 1);
}

} //hooks
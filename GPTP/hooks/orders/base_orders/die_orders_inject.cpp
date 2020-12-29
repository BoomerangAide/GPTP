//Injector source file for the Die Orders hook module.
#include "die_orders.h"
#include <hook_tools.h>

namespace {

void __declspec(naked) orders_SelfDestructingWrapper() {

	static CUnit* unit;

	__asm {
		PUSH EBP
		MOV EBP, ESP
		MOV unit, EAX
		PUSHAD
	}

	hooks::orders_SelfDestructing(unit);

	__asm {
		POPAD
		MOV ESP, EBP
		POP EBP
		RETN
	}
		

}

void __declspec(naked) orders_DieWrapper() {

	static CUnit* unit;

	__asm {
		PUSH EBP
		MOV EBP, ESP

		MOV EAX, [EBP+0x08]
		MOV unit, EAX

		PUSHAD
	}

	hooks::orders_Die(unit);

	__asm {
		POPAD
		POP EBP
		RETN 4
	}
		

}

}//unnamed namespace

namespace hooks {

void injectDieOrdersHook() {
	jmpPatch(orders_SelfDestructingWrapper, 0x00475710, 1);
	jmpPatch(orders_DieWrapper,				0x00479480, 2);
}

} //hooks

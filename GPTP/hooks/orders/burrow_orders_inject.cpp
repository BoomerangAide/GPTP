#include "burrow_orders.h"
#include "../../hook_tools.h"

namespace {

void __declspec(naked) orders_BurrowedWrapper() {

	static CUnit* unit;

	__asm {
		MOV unit, EAX
		PUSHAD
	}

	hooks::orders_Burrowed(unit);

	__asm {
		POPAD
		RETN
	}

}

;

void __declspec(naked) orders_BurrowWrapper() {

	static CUnit* unit;

	__asm {
		MOV unit, EAX
		PUSHAD
	}

	hooks::orders_Burrow(unit);

	__asm {
		POPAD
		RETN
	}

}

;

void __declspec(naked) orders_UnburrowWrapper() {

	static CUnit* unit;

	__asm {
		MOV unit, EAX
		PUSHAD
	}

	hooks::orders_Unburrow(unit);

	__asm {
		POPAD
		RETN
	}

}

;

} //unnamed namespace

namespace hooks {

void injectBurrowOrdersHooks() {
	jmpPatch(orders_BurrowedWrapper,	0x004E9860, 2);
	jmpPatch(orders_BurrowWrapper,		0x004E9E60, 2);
	jmpPatch(orders_UnburrowWrapper,	0x004EA670, 2);
}

} //hooks

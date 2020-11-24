#include "rescuable_order.h"
#include "../../hook_tools.h"

namespace {

void __declspec(naked) orders_RescuePassiveWrapper() {

	static CUnit* unit;

	__asm {
		PUSH EBP
		MOV EBP, ESP
		MOV unit, EAX
		PUSHAD
	}

	hooks::orders_RescuePassive(unit);

	__asm {
		POPAD
		MOV ESP, EBP
		POP EBP
		RETN
	}

}

;

} //unnamed namespace

namespace hooks {

void injectRescuableOrderHook() {
	jmpPatch(orders_RescuePassiveWrapper,	0x004A1EF0, 1);
}

} //hooks

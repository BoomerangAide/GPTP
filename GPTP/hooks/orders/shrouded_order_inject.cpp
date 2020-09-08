#include "shrouded_order.h"
#include <hook_tools.h>

namespace {

	void __declspec(naked) orders_SpecialWrapper() {

		static CUnit* unit;

		__asm {
			PUSH EBP
			MOV EBP, ESP
			MOV unit, EAX
			PUSHAD
		}

		hooks::orders_Special(unit);

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

void injectShroudedOrderHook() {
	jmpPatch(orders_SpecialWrapper,	0x004F6FA0,	1);
}

} //hooks
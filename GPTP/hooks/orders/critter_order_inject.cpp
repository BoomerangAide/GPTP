#include "critter_order.h"
#include "../../hook_tools.h"

namespace {

void __declspec(naked) orders_CritterWrapper() {

	static CUnit* unit;

	__asm {
		MOV unit, EAX
		PUSHAD
	}

	hooks::orders_Critter(unit);

	__asm {
		POPAD
		RETN
	}

}

;

} //unnamed namespace

namespace hooks {

void injectCritterOrderHook() {
	jmpPatch(orders_CritterWrapper,	0x0047C3C0, 3);
}

} //hooks

#include "warpin.h"
#include "../../hook_tools.h"

namespace {

void __declspec(naked) orders_WarpinWrapper() {

	static CUnit* unit;

	__asm {
		MOV unit, EDI
		PUSHAD
	}

	hooks::orders_Warpin(unit);

	__asm {
		POPAD
		RETN
	}

}

;

} //unnamed namespace

namespace hooks {

void injectWarpinOrderHook() {
	jmpPatch(orders_WarpinWrapper,	0x004E4C70, 2);
}

} //hooks

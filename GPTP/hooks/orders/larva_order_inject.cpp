#include "larva_order.h"
#include "../../hook_tools.h"

namespace {

void __declspec(naked) orders_LarvaWrapper() {

	static CUnit* unit;

	__asm {
		PUSH EBP
		MOV EBP, ESP
		MOV unit, EAX
		PUSHAD
	}

	hooks::orders_Larva(unit);

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

void injectLarvaOrderHook() {
	jmpPatch(orders_LarvaWrapper,	0x004E9F90, 1);
}

} //hooks

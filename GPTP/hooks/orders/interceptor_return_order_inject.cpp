#include "interceptor_return_order.h"
#include <hook_tools.h>

namespace {

void __declspec(naked) orders_ReturnWrapper() {

	static CUnit* unit;

	__asm {
		MOV unit, EAX
		PUSHAD
	}

	hooks::orders_Return(unit);

	__asm {
		POPAD
		RETN
	}

}

;

} //unnamed namespace

namespace hooks {

void injectInterceptorReturnOrderHook() {
	jmpPatch(orders_ReturnWrapper,	0x00466350, 4);
}

}

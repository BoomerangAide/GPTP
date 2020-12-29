#include "junkyarddog_order.h"
#include "../../hook_tools.h"

namespace {

void __declspec(naked) orders_JunkYardDogWrapper() {

	static CUnit* unit;

	__asm {
		PUSH EBP
		MOV EBP, ESP
		MOV unit, EAX
		PUSHAD
	}

	hooks::orders_JunkYardDog(unit);

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

void injectJunkYardDogOrderHook() {
	jmpPatch(orders_JunkYardDogWrapper,	0x0047C210, 1);
}

} //hooks

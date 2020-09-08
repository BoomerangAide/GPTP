#include "recharge_shields.h"
#include "../hook_tools.h"

namespace {

void __declspec(naked) unitCanRechargeShieldsWrapper() {

	static CUnit* target;
	static CUnit* battery;
	static Bool32 result;

	__asm {
		MOV battery, EDI
		MOV target, EAX
		PUSHAD
	}

	result = hooks::unitCanRechargeShieldsHook(target, battery);

	__asm {
		POPAD
		MOV EAX, result
		RETN
	}

}

;

} //unnamed namespace

namespace hooks {

void injectRechargeShieldsHooks() {
	jmpPatch(unitCanRechargeShieldsWrapper, 0x00493520, 1);
}

} //hooks

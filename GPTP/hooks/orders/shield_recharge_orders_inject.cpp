#include "shield_recharge_orders.h"
#include <hook_tools.h>

namespace {

void __declspec(naked) orders_RechargeShields2Wrapper() {

	static CUnit* unit;

	__asm {
		PUSH EBP
		MOV EBP, ESP
		MOV unit, EAX
		PUSHAD
	}

	hooks::orders_RechargeShields2(unit);

	__asm {
		POPAD
		MOV ESP, EBP
		POP EBP
		RETN
	}

}

;

void __declspec(naked) orders_RechargeShields1Wrapper() {

	static CUnit* unit;

	__asm {
		MOV unit, EDI
		PUSHAD
	}

	hooks::orders_RechargeShields1(unit);

	__asm {
		POPAD
		RETN
	}

}

;

}//unnamed namespace

namespace hooks {

void injectShieldRechargeOrdersHooks() {
	jmpPatch(orders_RechargeShields2Wrapper,	0x00493990, 1);
	jmpPatch(orders_RechargeShields1Wrapper,	0x00493DD0, 1);
}

} //hooks
//Injector source file for the Stop/HoldPosition Orders hook module.
#include "stopholdpos_orders.h"
#include <hook_tools.h>

namespace {

void __declspec(naked) orders_MedicHoldPositionWrapper() {

	static CUnit* unit;

	__asm {
		MOV unit, ESI
		PUSHAD
	}

	hooks::orders_MedicHoldPosition(unit);

	__asm {
		POPAD
		RETN
	}

}

;

void __declspec(naked) orders_ReaverStopWrapper() {

	static CUnit* unit;

	__asm {
		PUSH EBP
		MOV EBP, ESP
		MOV EAX, [EBP+0x08]
		MOV unit, EAX
		PUSHAD
	}

	hooks::orders_ReaverStop(unit);

	__asm {
		POPAD
		MOV ESP, EBP
		POP EBP
		RETN 4
	}

}

;

void __declspec(naked) orders_CarrierStopWrapper() {

	static CUnit* unit;

	__asm {
		MOV unit, EAX
		PUSHAD
	}

	hooks::orders_CarrierStop(unit);

	__asm {
		POPAD
		RETN
	}

}

;

void __declspec(naked) orders_GuardWrapper() {

	static CUnit* unit;

	__asm {
		MOV unit, ESI
		PUSHAD
	}

	hooks::orders_Guard(unit);

	__asm {
		POPAD
		RETN
	}

}

;

void __declspec(naked) function_004774A0_Wrapper() {

	static CUnit* unit;

	__asm {
		MOV unit, EAX
		PUSHAD
	}

	hooks::orders_PlayerGuard(unit);

	__asm {
		POPAD
		RETN
	}

}

;
	
void __declspec(naked) orders_HoldPosition2Wrapper() {

	static CUnit* unit;

	__asm {
		MOV unit, EAX
		PUSHAD
	}

	hooks::orders_HoldPosition2(unit);

	__asm {
		POPAD
		RETN
	}

}

;

void __declspec(naked) makeToHoldPositionWrapper() {

	static CUnit* unit;

	__asm {
		MOV unit, ESI
		PUSHAD
	}

	hooks::makeToHoldPosition(unit);

	__asm {
		POPAD
		RETN
	}

}

;

}//unnamed namespace

namespace hooks {

void injectStopHoldPosOrdersHooks() {
	jmpPatch(orders_MedicHoldPositionWrapper,	0x00464050, 0);
	jmpPatch(orders_ReaverStopWrapper,			0x004654B0, 1);
	jmpPatch(orders_CarrierStopWrapper,			0x00465910, 3);
	jmpPatch(orders_GuardWrapper,				0x00475B90, 2);
	jmpPatch(function_004774A0_Wrapper,			0x004774A0, 3);
	jmpPatch(orders_HoldPosition2Wrapper,		0x00478D10, 1);
	jmpPatch(makeToHoldPositionWrapper,			0x004EB5B0, 2);	
}

} //hooks

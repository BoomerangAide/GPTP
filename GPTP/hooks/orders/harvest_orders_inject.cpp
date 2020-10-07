#include "harvest_orders.h"
#include "../../hook_tools.h"

namespace {

void __declspec(naked) orders_Harvest3Wrapper() {

	static CUnit* unit;

	__asm {
		MOV unit, ESI
		PUSHAD
	}

	hooks::orders_Harvest3(unit);

	__asm {
		POPAD
		RETN
	}

}

;

void __declspec(naked) orders_Harvest4Wrapper() {

	static CUnit* unit;

	__asm {
		MOV unit, EAX
		PUSHAD
	}

	hooks::orders_Harvest4(unit);

	__asm {
		POPAD
		RETN
	}

}

;

void __declspec(naked) orders_HarvestMinerals2Wrapper() {

	static CUnit* unit;

	__asm {
		PUSH EBP
		MOV EBP, ESP
		MOV unit, EAX
		PUSHAD
	}

	hooks::orders_HarvestMinerals2(unit);

	__asm {
		POPAD
		MOV ESP, EBP
		POP EBP
		RETN
	}

}

;

void __declspec(naked) orders_HarvestGas2Wrapper() {

	static CUnit* unit;

	__asm {
		MOV unit, EAX
		PUSHAD
	}

	hooks:: orders_HarvestGas2(unit);

	__asm {
		POPAD
		RETN
	}

}

;

void __declspec(naked) orders_ReturnHarvestWrapper() {

	static CUnit* unit;

	__asm {
		MOV unit, EAX
		PUSHAD
	}

	hooks::orders_ReturnHarvest(unit);

	__asm {
		POPAD
		RETN
	}

}

;

void __declspec(naked) orders_HarvestMinerals1Wrapper() {

	static CUnit* unit;

	__asm {
		PUSH EBP
		MOV EBP, ESP
		MOV unit, EAX
		PUSHAD
	}

	hooks::orders_HarvestMinerals1(unit);

	__asm {
		POPAD
		MOV ESP, EBP
		POP EBP
		RETN
	}

}

;

void __declspec(naked) orders_Harvest1Wrapper() {

	static CUnit* unit;

	__asm {
		MOV unit, EAX
		PUSHAD
	}

	hooks::orders_Harvest1(unit);

	__asm {
		POPAD
		RETN
	}

}

;

void __declspec(naked) orders_HarvestMinerals3Wrapper() {

	static CUnit* unit;

	__asm {
		PUSH EBP
		MOV EBP, ESP
		MOV unit, EAX
		PUSHAD
	}

	hooks::orders_HarvestMinerals3(unit);

	__asm {
		POPAD
		MOV ESP, EBP
		POP EBP
		RETN
	}

}

;

void __declspec(naked) orders_HarvestGas3Wrapper() {

	static CUnit* unit;

	__asm {
		PUSH EBP
		MOV EBP, ESP
		MOV unit, EAX
		PUSHAD
	}

	hooks::orders_HarvestGas3(unit);

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

void injectHarvestOrdersHooks() {
	jmpPatch(orders_Harvest3Wrapper,			0x00468E80, 1);
	jmpPatch(orders_Harvest4Wrapper,			0x00468ED0, 1);
	jmpPatch(orders_HarvestMinerals2Wrapper,	0x00468F60, 1);
	jmpPatch(orders_HarvestGas2Wrapper,			0x00469000, 1);
	jmpPatch(orders_ReturnHarvestWrapper,		0x004690C0, 4);
	jmpPatch(orders_HarvestMinerals1Wrapper,	0x00469240, 0);
	jmpPatch(orders_Harvest1Wrapper,			0x00469500, 4);
	jmpPatch(orders_HarvestMinerals3Wrapper,	0x004697C0, 1);
	jmpPatch(orders_HarvestGas3Wrapper,			0x00469980, 1);
}

} //hooks

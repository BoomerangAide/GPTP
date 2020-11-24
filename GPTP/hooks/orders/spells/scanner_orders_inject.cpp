//Injector source file for the Scanner Spell Orders hook module.
#include "scanner_orders.h"
#include <hook_tools.h>

namespace {

void __declspec(naked) orders_ScannerWrapper() {

	static CUnit* unit;

	__asm {
		MOV unit, eax
		PUSHAD
	}

	hooks::orders_Scanner(unit);

	__asm {
		POPAD
		RETN
	}

}

;

void __declspec(naked) orders_PlaceScannerWrapper() {

	static CUnit* unit;

	__asm {
		PUSH EBP
		MOV EBP, ESP
		MOV unit, eax
		PUSHAD
	}

	hooks::orders_PlaceScanner(unit);

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

void injectScannerOrdersHook() {
	jmpPatch(orders_ScannerWrapper,			0x00463D30, 1);
	jmpPatch(orders_PlaceScannerWrapper,	0x00464E40, 0);
}

} //hooks
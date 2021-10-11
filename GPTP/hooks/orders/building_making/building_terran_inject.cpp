#include "building_terran.h"
#include <hook_tools.h>

namespace {

void __declspec(naked) orders_TerranBuildSelfWrapper() {

	static CUnit* unit;

	__asm {
		MOV unit, EAX
		PUSHAD
	}

	hooks::orders_TerranBuildSelf(unit);

	__asm {
		POPAD
		RETN
	}

}

;

void __declspec(naked) orders_SCVBuild2Wrapper() {

	static CUnit* unit;

	__asm {
		PUSH EBP
		MOV EBP, ESP
		MOV unit, EAX
		PUSHAD
	}

	hooks::orders_SCVBuild2(unit);

	__asm {
		POPAD
		MOV ESP, EBP
		POP EBP
		RETN
	}

}
	
;

void __declspec(naked) orders_SCVBuildWrapper() {

	static CUnit* unit;

	__asm {

		PUSH EBP
		MOV EBP, ESP

		MOV EAX, [EBP+0x08]
		MOV unit, EAX

		PUSHAD

	}

	hooks::orders_SCVBuild(unit);

	__asm {
		POPAD
		POP EBP
		RETN 4
	}

}

;

}//unnamed namespace

namespace hooks {

void injectBuildingTerranHook() {
	jmpPatch(orders_TerranBuildSelfWrapper, 0x00467760, 2);
	jmpPatch(orders_SCVBuild2Wrapper,		0x00467A70, 1);
	jmpPatch(orders_SCVBuildWrapper,		0x00467FD0, 0);
}

} //hooks

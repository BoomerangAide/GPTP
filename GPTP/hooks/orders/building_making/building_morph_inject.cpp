#include "building_morph.h"
#include <hook_tools.h>

namespace {

void __declspec(naked) orders_isMorphingWrapper() {

	static CUnit* building;
	static Bool32 result;

	__asm {
		MOV building, EAX
		PUSHAD
	}

	result = hooks::isMorphing(building) ? 1 : 0;

	__asm {
		POPAD
		MOV EAX, result
		RETN
	}

}

;

void __declspec(naked) orders_ZergBuildSelfWrapper() {

	static CUnit* building;

	__asm {
		MOV building, EAX
		PUSHAD
	}

	hooks::orders_ZergBuildSelf(building);

	__asm {
		POPAD
		RETN
	}

}

;

void __declspec(naked) ZergPlaceBuildingWrapper() {

	static CUnit* unit;

	__asm {
		PUSH EBP
		MOV EBP, ESP
		MOV EAX, [EBP+0x08]
		MOV unit, EAX
		PUSHAD
	}

	hooks::ZergPlaceBuilding(unit);

	__asm {
		POPAD
		MOv ESP, EBp
		POP EBP
		RETN 4
	}

}

;

} //unnamed namespace

namespace hooks {

	void injectBuildingMorphHooks() {
		jmpPatch(orders_isMorphingWrapper,		0x0045CD00, 2);
		jmpPatch(orders_ZergBuildSelfWrapper,	0x0045D500, 0);
		jmpPatch(ZergPlaceBuildingWrapper,		0x0045DA40, 1);
	}

} //hooks

#include "unit_portrait.h"
#include <hook_tools.h>

namespace {

void __declspec(naked) setBuildingSelPortrait_Wrapper() {

	static u16 return_value;
	static u16 unitId;
	static CUnit* unit;

	__asm {
		MOV unit, EDX
		MOV unitId, AX
		PUSHAD
	}

	return_value = hooks::setBuildingSelPortrait(unit, unitId);

	__asm {
		POPAD
		MOV AX, return_value
		RETN
	}

}

;

}//unnamed namespace

namespace hooks {

void injectUnitPortraitHook() {
	jmpPatch(setBuildingSelPortrait_Wrapper, 0x0045E320, 6);
}

} //hooks
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
	
void __declspec(naked) displayUpdatePortraitHack_Wrapper() {

	static const u32 JUMPBACK_POINT_DISPLAYUPDATEPORTRAITHACK = 0x0045EA40;

	static u16 portraitId;
	static CUnit* unit;
	static u32 unkParam;

	__asm {

		MOV portraitId, AX

		MOV EAX, [EBP+0x08]
		MOV unit, EAX

		MOV EAX, [EBP+0x0C]
		MOV unkParam, EAX

		PUSHAD

	}

	hooks::displayUpdatePortraitHack(&portraitId, &unit, &unkParam);

	__asm {

		POPAD

		MOV EAX, unit
		MOV [EBP+0x08], EAX

		MOV EAX, unkParam
		MOV [EBP+0x0C], EAX

		PUSH EBX
		PUSH ESI
		PUSH EDI
		MOVZX EBX, portraitId
		MOV EAX, [0x006D5CA4]

		JMP JUMPBACK_POINT_DISPLAYUPDATEPORTRAITHACK

	}

}

;

}//unnamed namespace

namespace hooks {

void injectUnitPortraitHooks() {
	jmpPatch(setBuildingSelPortrait_Wrapper,	0x0045E320, 6);
	//jmpPatch(displayUpdatePortraitHack_Wrapper,	0x0045EA36, 0);
}

} //hooks

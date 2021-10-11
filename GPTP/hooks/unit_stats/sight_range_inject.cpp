#include "sight_range.h"
#include <hook_tools.h>

namespace {

void __declspec(naked) getSightRangeWrapper() {

	static CUnit* unit;
	static Bool32 isForSpellCasting;
	static u32 sightRange;

	__asm {

		PUSH EBP
		MOV EBP, ESP

		MOV unit, EDX

		MOV EAX, [EBP+0x08]
		MOV isForSpellCasting, EAX

		PUSHAD

	}

	sightRange = hooks::getSightRangeHook(unit, isForSpellCasting != 0);

	__asm {
		POPAD
		MOV EAX, sightRange
		POP EBP
		RETN 4
	}

}

;

} //unnamed namespace

namespace hooks {

void injectSightRangeHook() {
	jmpPatch(getSightRangeWrapper, 0x004E5B40, 4);
}

} //hooks

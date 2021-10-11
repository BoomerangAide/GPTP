#include "weapon_range.h"
#include <hook_tools.h>

namespace {

void __declspec(naked) getMaxWeaponRangeWrapper() {

	static CUnit* unit;
	static u8 weaponId;
	static u32 maxWeaponRange;

	__asm {
		MOV unit, EAX
		MOV weaponId, BL
		PUSHAD
	}

	maxWeaponRange = hooks::getMaxWeaponRangeHook(unit, weaponId);

	__asm {
		POPAD
		MOV EAX, maxWeaponRange;
		RETN
	}

}

;

void __declspec(naked) getSeekRangeWrapper() {

	static CUnit* unit;
	static u8 seekRange;

	__asm {
		MOV unit, EDX
		PUSHAD
	}

	seekRange = hooks::getSeekRangeHook(unit);

	__asm {
		POPAD
		MOVZX EAX, seekRange
		RETN
	}

}

;

} //unnamed namespace

namespace hooks {

void injectWeaponRangeHooks() {
	jmpPatch(getMaxWeaponRangeWrapper,	0x00475870, 1);
	jmpPatch(getSeekRangeWrapper,		0x00476000, 3);
}

} //hooks

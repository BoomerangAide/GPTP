#include "armor_bonus.h"
#include <hook_tools.h>

namespace {

void __declspec(naked) getArmorBonusWrapper() {

	static CUnit* unit;
	static u8 armor;

	__asm {
		MOV unit, EAX
		PUSHAD
	}

	armor = hooks::getArmorBonusHook(unit);

	__asm {
		POPAD
		MOVZX EAX, armor
		RETN
	}
}
	
;

} //unnamed namespace

namespace hooks {

void injectArmorBonusHook() {
	jmpPatch(getArmorBonusWrapper, 0x00453FC0, 1);
}

} //hooks

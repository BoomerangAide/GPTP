//Injector source file for the Attack and Cooldown hook module.
#include "attack_and_cooldown.h"
#include <hook_tools.h>

namespace {

void __declspec(naked) attackApplyCooldownsWrapper() {

	static CUnit* unit;
	static Bool32 result;

	__asm {
		PUSH EBP
		MOV EBP, ESP
		MOV unit, EAX
		PUSHAD
	}

	result = hooks::attackApplyCooldown(unit);

	__asm {
		POPAD
		MOV EAX, result
		MOV ESP, EBP
		POP EBP
		RETN
	}

}

;

}//unnamed namespace

namespace hooks {

void injectAttackAndCooldownHook() {
	jmpPatch(attackApplyCooldownsWrapper, 0x00478B40, 1);
}

} //hooks
#include "bunker_hooks.h"
#include "../hook_tools.h"

namespace {

void __declspec(naked) createBunkerAttackThingyWrapper() {

	static CUnit* unit;

	__asm {
		MOV unit, EAX
		PUSHAD
	}

	hooks::createBunkerAttackThingyHook(unit);

	__asm {
		POPAD
		RETN
	}
}

;

} //unnamed namespace

namespace hooks {

void injectBunkerHooks() {
  jmpPatch(createBunkerAttackThingyWrapper,		0x00477FD0, 3);
}

} //hooks

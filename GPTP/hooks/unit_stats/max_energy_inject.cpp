#include "max_energy.h"
#include <hook_tools.h>

namespace {

void __declspec(naked) getUnitMaxEnergyWrapper() {

	static CUnit* unit;
	static u16 maxEnergy;

	__asm {
		MOV unit, EAX
		PUSHAD
	}

	maxEnergy = hooks::getUnitMaxEnergyHook(unit);

	__asm {
		POPAD
		MOVZX EAX, maxEnergy
		RETN
	}

}

;

} //unnamed namespace

namespace hooks {

void injectUnitMaxEnergyHook() {
	jmpPatch(getUnitMaxEnergyWrapper, 0x00491870, 7);
}

} //hooks

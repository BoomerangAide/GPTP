//Injector source file for the Siege Transformation hook module.
#include "siege_transform.h"
#include <hook_tools.h>

void __declspec(naked) unitTransformIntoTankModeOrder() {

	static CUnit* unitTransforming; 

	__asm {
		MOV unitTransforming, EAX
		PUSHAD
	}

	hooks::ordersTankMode(unitTransforming);

	__asm {
		POPAD
		RETN
	}

}

;

void __declspec(naked) unitTransformIntoSiegeModeOrder() {

	static CUnit* unitTransforming; 

	__asm {
		MOV unitTransforming, EDI
		PUSHAD
	}

	hooks::ordersSiegeMode(unitTransforming);

	__asm {
		POPAD
		RETN
	}

}

;

namespace hooks {

void injectSiegeTransformHooks() {
	jmpPatch(unitTransformIntoTankModeOrder,	0x00464AE0, 1);
	jmpPatch(unitTransformIntoSiegeModeOrder,	0x00464BD0, 1);
}

}

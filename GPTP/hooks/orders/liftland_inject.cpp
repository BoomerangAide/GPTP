//Injector source file for the Lift/Land hook module.
#include "liftland.h"
#include <hook_tools.h>

namespace {

	void __declspec(naked) orders_BuildingLandWrapper() {

		static CUnit* unit;

		__asm {
			PUSH EBP
			MOV EBP, ESP
			MOV unit, EAX
			PUSHAD
		}

		hooks::orders_BuildingLand(unit);

		__asm {
			POPAD
			MOV ESP, EBP
			POP EBP
			RET
		}

	}

	;

	void __declspec(naked) orders_BuildingLiftoffWrapper() {

		static CUnit* unit;

		__asm {
			PUSH EBP
			MOV EBP, ESP
			MOV unit, ESI
			PUSHAD
		}

		hooks::orders_BuildingLiftoff(unit);

		__asm {
			POPAD
			MOV ESP, EBP
			POP EBP
			RET
		}

	}

	;

} //unnamed namespace

namespace hooks {

	void injectLiftLandHooks() {
		jmpPatch(orders_BuildingLandWrapper,	0x00464360, 1);
		jmpPatch(orders_BuildingLiftoffWrapper,	0x004649B0, 1);
	}

}

#include "cloak_nearby_units_order.h"
#include "../../hook_tools.h"

namespace {

void __declspec(naked) orders_cloakNearbyUnitsWrapper() {

	static CUnit* unit;

	__asm {
		PUSH EBP
		MOV EBP, ESP
		MOV unit, ESI
		PUSHAD
	}

	hooks::orders_cloakNearbyUnits(unit);

	__asm {
		POPAD
		MOV ESP, EBP
		POP EBP
		RETN
	}

}

;

} //unnamed namespace

namespace hooks {

void injectCloakNearbyUnitsOrderHook() {
	jmpPatch(orders_cloakNearbyUnitsWrapper, 0x00493A80, 1);
}

} //hooks

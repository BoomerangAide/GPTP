#include "doodad_orders.h"
#include "../../hook_tools.h"

namespace {

void __declspec(naked) orders_CloseDoorWrapper() {

	static CUnit* unit;

	__asm {
		PUSH EBP
		MOV EBP, ESP
		MOV unit, ESI
		PUSHAD
	}

	hooks::orders_CloseDoor(unit);

	__asm {
		POPAD
		MOV ESP, EBP
		POP EBP
		RETN
	}

}

;

void __declspec(naked) orders_OpenDoorWrapper() {

	static CUnit* unit;

	__asm {
		PUSH EBP
		MOV EBP, ESP
		MOV unit, ESI
		PUSHAD
	}

	hooks::orders_OpenDoor(unit);

	__asm {
		POPAD
		MOV ESP, EBP
		POP EBP
		RETN
	}

}

;

void __declspec(naked) orders_DisabledoodadWrapper() {

	static CUnit* unit;

	__asm {
		MOV unit, EAX
		PUSHAD
	}

	hooks::orders_Disabledoodad(unit);

	__asm {
		POPAD
		RETN
	}

}

;

void __declspec(naked) orders_EnabledoodadWrapper() {

	static CUnit* unit;

	__asm {
		MOV unit, EAX
		PUSHAD
	}

	hooks::orders_Enabledoodad(unit);

	__asm {
		POPAD
		RETN
	}

}

;

void __declspec(naked) orders_HiddenGunWrapper() {

	static CUnit* unit;

	__asm {
		MOV unit, EAX
		PUSHAD
	}

	hooks::orders_HiddenGun(unit);

	__asm {
		POPAD
		RETN
	}

}

;

void __declspec(naked) orders_HideTrapWrapper() {

	static CUnit* unit;

	__asm {
		PUSH EBP
		MOV EBP, ESP
		MOV unit, EAX
		PUSHAD
	}

	hooks::orders_HideTrap(unit);

	__asm {
		POPAD
		MOV ESP, EBP
		POP EBP
		RETN
	}

}

;

void __declspec(naked) orders_RevealTrapWrapper() {

	static CUnit* unit;

	__asm {
		MOV unit, EAX
		PUSHAD
	}

	hooks::orders_RevealTrap(unit);

	__asm {
		POPAD
		RETN
	}

}

;

} //unnamed namespace

namespace hooks {

void injectDoodadOrdersHooks() {
	jmpPatch(orders_CloseDoorWrapper,		0x0047BC50, 1);
	jmpPatch(orders_OpenDoorWrapper,		0x0047BCD0, 1);
	jmpPatch(orders_DisabledoodadWrapper,	0x0047BD60, 4);
	jmpPatch(orders_EnabledoodadWrapper,	0x0047BE80, 4);
	jmpPatch(orders_HiddenGunWrapper,		0x0047BF80, 2);
	jmpPatch(orders_HideTrapWrapper,		0x0047C0A0, 1);
	jmpPatch(orders_RevealTrapWrapper,		0x0047C1B0, 2);
}

} //hooks

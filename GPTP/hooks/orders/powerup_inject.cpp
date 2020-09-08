#include "powerup.h"
#include "../../hook_tools.h"

namespace {

void __declspec(naked) powerupCanBePickedUpProcWrapper() {

	static CUnit* powerup;
	static CUnit* worker;
	static Bool32 result;

	__asm {
		MOV worker, ECX
		MOV powerup, EDX
		PUSHAD
	}

	if (hooks::powerupCanBePickedUpProc(worker, powerup))
		result = 1;
	else
		result = 0;

	__asm {
		POPAD
		MOV EAX, result
		RETN
	}

}

;

void __declspec(naked) killPowerupWrapper() {

	static CUnit* worker;

	__asm {
		MOV worker, ESI
		PUSHAD
	}

	hooks::killPowerup(worker);

	__asm {
		POPAD
		RETN
	}

}

;

void __declspec(naked) pickupPowerupWrapper() {

	static CUnit* powerup;
	static CUnit* worker;

	__asm {

		PUSH EBP
		MOV EBP, ESP

		MOV EAX, [EBP+0x08]
		MOV worker, EAX

		MOV EAX, [EBP+0x0C]
		MOV powerup, EAX

		PUSHAD

	}

	hooks::pickupPowerup(worker, powerup);

	__asm {
		POPAD
		MOV ESP, EBP
		POP EBP
		RETN 0x8
	}

}

;

void __declspec(naked) orders_Powerup2Wrapper() {

	static CUnit* powerup;

	__asm {
		PUSH EBP
		MOV EBP, ESP
		MOV powerup, EAX
		PUSHAD
	}

	hooks::orders_Powerup2(powerup);

	__asm {
		POPAD
		MOV ESP, EBP
		POP EBP
		RETN
	}

}

;

void __declspec(naked) orders_Powerup1Wrapper() {

	static CUnit* powerup;

	__asm {
		PUSH EBP
		MOV EBP, ESP
		MOV powerup, EAX
		PUSHAD
	}

	hooks::orders_Powerup1(powerup);

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

void injectPowerupOrderHooks() {
	jmpPatch(powerupCanBePickedUpProcWrapper,	0x00440930, 7);
	jmpPatch(killPowerupWrapper,				0x004F3C60, 1);
	jmpPatch(pickupPowerupWrapper,				0x004F3CA0, 1);
	jmpPatch(orders_Powerup2Wrapper,			0x004F3E10, 1);
	jmpPatch(orders_Powerup1Wrapper,			0x004F3EA0, 1);
}

} //hooks

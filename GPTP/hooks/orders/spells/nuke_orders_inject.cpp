#include "nuke_orders.h"
#include <hook_tools.h>

namespace {

void __declspec(naked) orders_NukePaintWrapper() {

	static CUnit* unit;

	__asm {
		MOV unit, EAX
		PUSHAD
	}

	hooks::orders_NukePaint(unit);

	__asm {
		POPAD
		RETN
	}

}

;

void __declspec(naked) orders_NukeLaunchWrapper() {

	static CUnit* unit;

	__asm {
		MOV unit, EAX
		PUSHAD
	}

	hooks::orders_NukeLaunch(unit);

	__asm {
		POPAD
		RETN
	}

}

;

void __declspec(naked) orders_NukeTrackWrapper() {

	static CUnit* unit;

	__asm {
		PUSH EBP
		MOV EBP, ESP
		MOV unit, EBX
		PUSHAD
	}

	hooks::orders_NukeTrack(unit);

	__asm {
		POPAD
		MOV ESP, EBP
		POP EBP
		RETN
	}

}

;

void __declspec(naked) orders_NukeGroundWrapper() {

	static CUnit* unit;

	__asm {
		PUSH EBP
		MOV EBP, ESP
		MOV EAX, [EBP+0x08]
		MOV unit, EAX
		PUSHAD
	}

	hooks::orders_NukeGround(unit);

	__asm {
		POPAD
		POP EBP
		RETN 4
	}

}

;

void __declspec(naked) orders_NukeUnitWrapper() {

	static CUnit* unit;

	__asm {
		MOV unit, EAX
		PUSHAD
	}

	hooks::orders_NukeUnit(unit);

	__asm {
		POPAD
		RETN
	}

}

;

void __declspec(naked) orders_NukeTrainWrapper() {

	static CUnit* unit;

	__asm {
		MOV unit, EAX
		PUSHAD
	}

	hooks::orders_NukeTrain(unit);

	__asm {
		POPAD
		RETN
	}

}

}//unnamed namespace

namespace hooks {

void injectNukeOrdersHooks() {
	jmpPatch(orders_NukePaintWrapper,	0x00463610, 1);
	jmpPatch(orders_NukeLaunchWrapper,	0x00464730, 1);
	jmpPatch(orders_NukeTrackWrapper,	0x00464D10, 2);
	jmpPatch(orders_NukeGroundWrapper,	0x00479200, 0);
	jmpPatch(orders_NukeUnitWrapper,	0x00479410, 1);
	jmpPatch(orders_NukeTrainWrapper,	0x004E6700, 2);
}

} //hooks

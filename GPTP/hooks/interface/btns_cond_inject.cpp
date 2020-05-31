#include "btns_cond.h"
#include <hook_tools.h>

namespace {

void __declspec(naked) BTNSCOND_NoNydusExit_Wrapper() {

	static CUnit* unit;
	static u16 reqVar;
	static u32 playerId;

	static s32 button_state;

	__asm {

		PUSH EBP
		MOV EBP, ESP

		MOV EAX, [EBP+0x08]
		MOV unit, EAX

		MOV reqVar, CX
		MOV playerId, EDX

		PUSHAD

	}

	button_state = hooks::BTNSCOND_NoNydusExit(unit, reqVar, playerId);

	__asm {
		POPAD
		MOV EAX, button_state
		POP EBP
		RETN 4
	}

}

;

void __declspec(naked) BTNSCOND_Movement_Wrapper() {

	static CUnit* unit;
	static u16 reqVar;
	static u32 playerId;

	static s32 button_state;

	__asm {

		PUSH EBP
		MOV EBP, ESP

		MOV EAX, [EBP+0x08]
		MOV unit, EAX

		MOV reqVar, CX
		MOV playerId, EDX

		PUSHAD

	}

	button_state = hooks::BTNSCOND_Movement(unit, reqVar, playerId);

	__asm {
		POPAD
		MOV EAX, button_state
		POP EBP
		RETN 4
	}

}

;

void __declspec(naked) BTNSCOND_HasScarabs_Wrapper() {

	static CUnit* unit;
	static u16 reqVar;
	static u32 playerId;

	static s32 button_state;

	__asm {

		PUSH EBP
		MOV EBP, ESP

		MOV EAX, [EBP+0x08]
		MOV unit, EAX

		MOV reqVar, CX
		MOV playerId, EDX

		PUSHAD

	}

	button_state = hooks::BTNSCOND_HasScarabs(unit, reqVar, playerId);

	__asm {
		POPAD
		MOV EAX, button_state
		POP EBP
		RETN 4
	}

}

;

void __declspec(naked) BTNSCOND_CanBuildUnit_Wrapper() {

	static CUnit* unit;
	static u16 reqVar;
	static u32 playerId;

	static s32 button_state;

	__asm {

		PUSH EBP
		MOV EBP, ESP

		MOV EAX, [EBP+0x08]
		MOV unit, EAX

		MOV reqVar, CX
		MOV playerId, EDX

		PUSHAD

	}

	button_state = hooks::BTNSCOND_CanBuildUnit(unit, reqVar, playerId);

	__asm {
		POPAD
		MOV EAX, button_state
		POP EBP
		RETN 4
	}

}

;

}; //unnamed namespace

namespace hooks {

void injectBtnsCondHook() {
	jmpPatch(BTNSCOND_NoNydusExit_Wrapper,	0x00428340, 1);
	jmpPatch(BTNSCOND_Movement_Wrapper,		0x004283C0, 0);
	jmpPatch(BTNSCOND_HasScarabs_Wrapper,	0x004286A0, 0);
	jmpPatch(BTNSCOND_CanBuildUnit_Wrapper,	0x00428E60, 0);
}

}

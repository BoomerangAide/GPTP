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
	
void __declspec(naked) BTNSCOND_IsCloaked_Wrapper() {

	static s32 button_state;

	static u32 playerId;
	static CUnit* unit;
	static u16 reqVar;

	__asm {

		PUSH EBP
		MOV EBP, ESP

		MOV playerId, EDX

		MOV EBX, [EBP+0x08]
		MOV unit, EBX

		MOV reqVar, CX 

		PUSHAD

	}

	button_state = hooks::BTNSCOND_IsCloaked(unit,reqVar,playerId);

	__asm {
		POPAD
		MOV EAX, button_state
		MOV ESP, EBP
		POP EBP
		RETN 4
	}

}

;

void __declspec(naked) BTNSCOND_CanCloak_Wrapper() {

	static s32 button_state;

	static u32 playerId;
	static CUnit* unit;
	static u16 reqVar;

	__asm {

		PUSH EBP
		MOV EBP, ESP

		MOV playerId, EDX

		MOV EBX, [EBP+0x08]
		MOV unit, EBX

		MOV reqVar, CX 

		PUSHAD

	}

	button_state = hooks::BTNSCOND_CanCloak(unit,reqVar,playerId);

	__asm {
		POPAD
		MOV EAX, button_state
		MOV ESP, EBP
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
	jmpPatch(BTNSCOND_IsCloaked_Wrapper,	0x00429210, 1);
	jmpPatch(BTNSCOND_CanCloak_Wrapper,		0x004292C0, 1);	
}

}

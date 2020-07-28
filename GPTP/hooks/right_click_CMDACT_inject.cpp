#include "right_click_CMDACT.h"
#include "../hook_tools.h"

namespace {

void __declspec(naked) CMDACT_RightClickWrapper() {

	static s16 someX;
	static s16 someY;

	__asm {

		PUSH EBP
		MOV EBP, ESP

		XOR ECX, ECX

		MOV CX, [EAX+0x0E]
		MOV someX, CX

		MOV CX, [EAX+0x10]
		MOV someY, CX

		PUSHAD

	}

	hooks::CMDACT_RightClick(someX,someY);

	__asm {
		POPAD
		MOV ESP, EBP
		POP EBP
		RETN
	}

}

;

void __declspec(naked) sub_4564E0_Wrapper() {

	static s16 someX;
	static s16 someY;

	__asm {

		PUSH EBP
		MOV EBP, ESP

		MOV AX, [ECX+0x0E]
		MOV someX, AX

		MOV AX, [ECX+0x10]
		MOV someY, AX

		PUSHAD

	}

	hooks::function_004564E0(someX,someY);

	__asm {
		POPAD
		MOV ESP, EBP
		POP EBP
		RETN
	}

}

;

void __declspec(naked) CMDACT_RightClickOrderWrapper() {

	static CUnit* unit;
	static s16 x;
	static s16 y;
	static u16 unitId;
	static Bool8 isQueuedOrder;

	__asm {

		PUSH EBP
		MOV EBP, ESP

		MOV unit, ESI

		MOV AX, [EBP+0x08]
		MOV x, AX

		MOV AX, [EBP+0x0C]
		MOV y, AX

		MOV AX, [EBP+0x10]
		MOV unitId, AX

		MOV AL, [EBP+0x14]
		MOV isQueuedOrder, AL

		PUSHAD

	}

	hooks::CMDACT_RightClickOrder(unit, x, y, unitId, isQueuedOrder);

	__asm {
		POPAD
		MOV ESP, EBP
		POP EBP
		RETN 0x10
	}

}

;

} //unnamed namespace

namespace hooks {

void injectRightClickCMDACT_Hooks() {
	jmpPatch(CMDACT_RightClickWrapper,		0x004563A0, 1);
	jmpPatch(sub_4564E0_Wrapper,			0x004564E0, 0);
	jmpPatch(CMDACT_RightClickOrderWrapper,	0x004C0380, 1);
}

} //hooks

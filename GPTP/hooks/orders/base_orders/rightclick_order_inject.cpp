//Injector source file for the Right Click Order hooks module.
#include "rightclick_order.h"
#include <hook_tools.h>

namespace {

void __declspec(naked) function00455F20_Wrapper() {

	static CUnit* unit;
	static u32 orderUnitType;
	static int x, y;

	__asm {

		PUSH EBP
		MOV EBP, ESP

		MOV unit, EAX

		MOVZX EAX, WORD PTR [EBP+0x0C]
		MOV orderUnitType, EAX

		MOV EAX, [EBP+0x08]
		MOV x, EAX

		MOV y, EDI

		PUSHAD
	}

	hooks::function00455F20(unit,orderUnitType,x,y);

	__asm {
		POPAD
		MOV ESP, EBP
		POP EBP
		RETN 0x8
	}
		

}

;

void __declspec(naked) orders_RightClickActionWrapper() {

	static CUnit* unit;

	__asm {
		PUSH EBP
		MOV EBP, ESP
		MOV unit, EAX
		PUSHAD
	}

	hooks::orders_RightClickAction(unit);

	__asm {
		POPAD
		MOV ESP, EBP
		POP EBP
		RETN
	}
		

}

;

}//unnamed namespace

namespace hooks {

void injectRightClickOrderHooks() {
	jmpPatch(function00455F20_Wrapper,			0x00455F20, 2);
	jmpPatch(orders_RightClickActionWrapper,	0x004F6EF0, 2);
}

} //hooks
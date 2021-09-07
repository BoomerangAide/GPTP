//Injector source file for the right click returned orders hooks module.
#include "right_click_returnedOrders.h"
#include <hook_tools.h>

namespace {

void __declspec(naked) returnNothing2_Wrapper() {

	static int x;
	static int y;
	static CUnit** target;
	static CUnit* unit;
	static u32 unitId;

	static u32 return_value;

	__asm {

		PUSH EBP
		MOV EBP, ESP

		MOV EAX, [EBP+0x10]
		MOV unitId, EAX
		MOV EAX, [EBP+0x0C]
		MOV target, EAX
		MOV EAX, [EBP+0x08]
		MOV y, EAX

		MOV x, EDX
		MOV unit, ECX

		PUSHAD

	}

	return_value = hooks::returnNothing2(unit, x, y, target, unitId);

	__asm {
		POPAD
		MOV EAX, return_value
		POP EBP
		RETN 0xC
	}

}

;

void __declspec(naked) returnNormalMove_NormalAttack_Order_Wrapper() {

	static int x;
	static int y;
	static CUnit** target;
	static CUnit* unit;
	static u32 unitId;

	static u32 return_value;

	__asm {

		PUSH EBP
		MOV EBP, ESP

		MOV EAX, [EBP+0x10]
		MOV unitId, EAX
		MOV EAX, [EBP+0x0C]
		MOV target, EAX
		MOV EAX, [EBP+0x08]
		MOV y, EAX

		MOV x, EDX
		MOV unit, ECX

		PUSHAD

	}

	return_value = hooks::returnNormalMove_NormalAttack_Order(unit, x, y, target, unitId);

	__asm {
		POPAD
		MOV EAX, return_value
		POP EBP
		RETN 0xC
	}

}

;

void __declspec(naked) returnNormalMove_NoAttack_Order_Wrapper() {

	static int x;
	static int y;
	static CUnit** target;
	static CUnit* unit;
	static u32 unitId;

	static u32 return_value;

	__asm {

		PUSH EBP
		MOV EBP, ESP

		MOV EAX, [EBP+0x10]
		MOV unitId, EAX
		MOV EAX, [EBP+0x0C]
		MOV target, EAX
		MOV EAX, [EBP+0x08]
		MOV y, EAX

		MOV x, EDX
		MOV unit, ECX

		PUSHAD

	}

	return_value = hooks::returnNormalMove_NoAttack_Order(unit, x, y, target, unitId);

	__asm {
		POPAD
		MOV EAX, return_value
		POP EBP
		RETN 0xC
	}

}

;

void __declspec(naked) returnNoMove_NormalAttack_Order_Wrapper() {

	static int x;
	static int y;
	static CUnit** target;
	static CUnit* unit;
	static u32 unitId;

	static u32 return_value;

	__asm {

		PUSH EBP
		MOV EBP, ESP

		MOV EAX, [EBP+0x10]
		MOV unitId, EAX
		MOV EAX, [EBP+0x0C]
		MOV target, EAX
		MOV EAX, [EBP+0x08]
		MOV y, EAX

		MOV x, EDX
		MOV unit, ECX

		PUSHAD

	}

	return_value = hooks::returnNoMove_NormalAttack_Order(unit, x, y, target, unitId);

	__asm {
		POPAD
		MOV EAX, return_value
		POP EBP
		RETN 0xC
	}

}

;

void __declspec(naked) returnHarvest_Order_Wrapper() {

	static int x;
	static int y;
	static CUnit** target;
	static CUnit* unit;
	static u32 unitId;

	static u32 return_value;

	__asm {

		PUSH EBP
		MOV EBP, ESP

		MOV EAX, [EBP+0x10]
		MOV unitId, EAX
		MOV EAX, [EBP+0x0C]
		MOV target, EAX
		MOV EAX, [EBP+0x08]
		MOV y, EAX

		MOV x, EDX
		MOV unit, ECX

		PUSHAD

	}

	return_value = hooks::returnHarvest_Order(unit, x, y, target, unitId);

	__asm {
		POPAD
		MOV EAX, return_value
		POP EBP
		RETN 0xC
	}

}

;

void __declspec(naked) returnHarvestAndRepair_Order_Wrapper() {

	static int x;
	static int y;
	static CUnit** target;
	static CUnit* unit;
	static u32 unitId;

	static u32 return_value;

	__asm {

		PUSH EBP
		MOV EBP, ESP

		MOV EAX, [EBP+0x10]
		MOV unitId, EAX
		MOV EAX, [EBP+0x0C]
		MOV target, EAX
		MOV EAX, [EBP+0x08]
		MOV y, EAX

		MOV x, EDX
		MOV unit, ECX

		PUSHAD

	}

	return_value = hooks::returnHarvestAndRepair_Order(unit, x, y, target, unitId);

	__asm {
		POPAD
		MOV EAX, return_value
		POP EBP
		RETN 0xC
	}

}

;

}//unnamed namespace

namespace hooks {

void injectRightClickReturnedOrdersHooks() {
	jmpPatch(returnNothing2_Wrapper,						0x00455650, 0);
	jmpPatch(returnNormalMove_NormalAttack_Order_Wrapper,	0x00455900, 1);
	jmpPatch(returnNormalMove_NoAttack_Order_Wrapper,		0x00455E00, 1);
	jmpPatch(returnNoMove_NormalAttack_Order_Wrapper,		0x00455890, 1);
	jmpPatch(returnHarvest_Order_Wrapper,					0x004559C0, 1);
	jmpPatch(returnHarvestAndRepair_Order_Wrapper,			0x00455CC0, 2);
}

} //hooks
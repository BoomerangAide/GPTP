//Injector source file for the Move Orders hook module.
#include "move_orders.h"
#include <hook_tools.h>

namespace {

	void __declspec(naked) orders_CarrierIgnore2Wrapper() {

		static CUnit* unit;

		__asm {
			MOV unit, EAX
			PUSHAD
		}

		hooks::orders_CarrierIgnore2(unit);

		__asm {
			POPAD
			RETN
		}

	}

	;

	void __declspec(naked) orders_StayInRangeWrapper() {

		static CUnit* unit;

		__asm {
			PUSH EBP
			MOV EBP, ESP
			MOV unit, EAX
			PUSHAD
		}

		hooks::orders_StayInRange(unit);

		__asm {
			POPAD
			MOV ESP, EBP
			POP EBP
			RETN
		}

	}

	;

	void __declspec(naked) orders_FollowWrapper() {

		static CUnit* unit;

		__asm {
			MOV unit, EAX
			PUSHAD
		}

		hooks::orders_Follow(unit);

		__asm {
			POPAD
			RETN
		}

	}

	;

	void __declspec(naked) orders_MoveWrapper() {

		static CUnit* unit;

		__asm {
			MOV unit, EAX
			PUSHAD
		}

		hooks::orders_Move(unit);

		__asm {
			POPAD
			RETN
		}

	}

	;

	void __declspec(naked) orders_MoveToInfestableWrapper() {

		static CUnit* unit;

		__asm {
			MOV unit, EAX
			PUSHAD
		}

		hooks::orders_MoveToInfestable(unit);

		__asm {
			POPAD
			RETN
		}

	}

	;

}//unnamed namespace

namespace hooks {

	void injectMoveOrdersHooks() {
		jmpPatch(orders_CarrierIgnore2Wrapper,		0x00466720, 1);
		jmpPatch(orders_StayInRangeWrapper,			0x0047C4F0, 0);
		jmpPatch(orders_FollowWrapper,				0x0047C7B0, 2);
		jmpPatch(orders_MoveWrapper,				0x0047C950, 2);
		jmpPatch(orders_MoveToInfestableWrapper,	0x004E98E0, 2);
	}

} //hooks

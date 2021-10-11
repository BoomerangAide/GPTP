#include "merge_units.h"
#include <hook_tools.h>

namespace {

void __declspec(naked) templarMergePartnerWrapper() {

	static CUnit* unit;
	static CUnit** units_list;
	static u32 units_list_length;

	static CUnit* nearestPartner;

	__asm {
		PUSH EBP
		MOV EBP, ESP
		SUB ESP, 0x08
		MOV units_list_length, EAX
		MOV EAX, [EBP+0x0C]
		MOV units_list, EAX
		MOV EAX, [EBP+0x08]
		MOV unit, EAX
		PUSHAD
	}

	nearestPartner = hooks::templarMergePartner(unit, units_list, units_list_length);

	__asm {
		POPAD
		MOV EAX, nearestPartner
		MOV ESP, EBP
		POP EBP
		RETN 0x0008
	}

}
	
;

void __declspec(naked) orders_CompletingArchonSummonWrapper() {

	static CUnit* unit;

	__asm {
		PUSH EBP
		MOV EBP, ESP
		SUB ESP, 0x08
		MOV unit, ESI
		PUSHAD
	}

	hooks::orders_CompletingArchonSummon(unit);

	__asm {
		POPAD
		MOV ESP, EBP
		POP EBP
		RET
	}

}

;

void __declspec(naked) orders_WarpingDarkArchonWrapper() {

	static CUnit* unit;

	__asm {
		MOV unit, EAX
		PUSHAD
	}

	hooks::orders_WarpingDarkArchon(unit);

	__asm {
		POPAD
		RET
	}

}

;

void __declspec(naked) orders_WarpingArchonWrapper() {

	static CUnit* unit;

	__asm {
		MOV unit, EAX
		PUSHAD
	}

	hooks::orders_WarpingArchon(unit);

	__asm {
		POPAD
		RET
	}

}

;

} //unnamed namespace

namespace hooks {

	void injectMergeUnitsHooks() {
		jmpPatch(templarMergePartnerWrapper,			0x004BFA80, 1);
		jmpPatch(orders_CompletingArchonSummonWrapper,	0x00493B10, 1);
		jmpPatch(orders_WarpingDarkArchonWrapper,		0x00494690, 2);
		jmpPatch(orders_WarpingArchonWrapper,			0x004948B0, 2);	
	}

}

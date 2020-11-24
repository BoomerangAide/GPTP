//Injector source file for the Defensive Matrix Spell order hook module.
#include "defense_matrix.h"
#include <hook_tools.h>

namespace {

void __declspec(naked) CreateDefensiveMatrixWrapper() {

	static CUnit* unit;

	__asm {
		MOV unit, EDI
		PUSHAD
	}

	hooks::CreateDefensiveMatrix(unit);

	__asm {
		POPAD
		RETN
	}

}

;

void __declspec(naked) ApplyDefensiveMatrixWrapper() {

	static CUnit* unit;

	__asm {
		PUSH EBP
		MOV EBP, ESP
		MOV unit, EAX
		PUSHAD
	}

	hooks::ApplyDefensiveMatrix(unit);

	__asm {
		POPAD
		MOV ESP, EBP
		POP EBP
		RETN
	}

}

;

void __declspec(naked) orders_DefensiveMatrixWrapper() {

	static CUnit* unit;

	__asm {
		PUSH EBP
		MOV EBP, ESP
		MOV EAX, [EBP+0x08]
		MOV unit, EAX
		PUSHAD
	}

	hooks::orders_DefensiveMatrix(unit);

	__asm {
		POPAD
		MOV ESP, EBP
		POP EBP
		RETN 0x4
	}

}

;

}//unnamed namespace

namespace hooks {

void injectDefensiveMatrixHooks() {
	jmpPatch(CreateDefensiveMatrixWrapper,	0x00454F90, 1);
	jmpPatch(ApplyDefensiveMatrixWrapper,	0x00455010, 0);
	jmpPatch(orders_DefensiveMatrixWrapper,	0x004550A0, 0);
}

} //hooks
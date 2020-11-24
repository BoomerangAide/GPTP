#include "spidermine.h"
#include "../../hook_tools.h"

namespace {

void __declspec(naked) SpiderMine_EnemyProcWrapper() {

	static CUnit* spiderMine;
	static CUnit* target;
	static Bool32 result;

	__asm {
		MOV spiderMine, EDX
		MOV target, ECX
		PUSHAD
	}

	result = hooks::SpiderMine_EnemyProc(spiderMine, target);

	__asm {
		POPAD
		MOV EAX, result
		RETN
	}

}

;

void __declspec(naked) orders_VultureMineWrapper() {

	static CUnit* unit;

	__asm {
		MOV unit, EAX
		PUSHAD
	}

	hooks::orders_VultureMine(unit);

	__asm {
		POPAD
		RETN
	}

}

;
	
void __declspec(naked) orders_PlaceMineWrapper() {

	static CUnit* unit;

	__asm {
		PUSH EBP
		MOV EBP, ESP
		MOV unit, EAX
		PUSHAD
	}

	hooks::orders_PlaceMine(unit);

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

void injectSpiderMineHooks() {
	jmpPatch(SpiderMine_EnemyProcWrapper,	0x00440EC0, 0);
	jmpPatch(orders_VultureMineWrapper,		0x00463DF0, 2);
	jmpPatch(orders_PlaceMineWrapper,		0x00464FD0, 0);
}

} //hooks

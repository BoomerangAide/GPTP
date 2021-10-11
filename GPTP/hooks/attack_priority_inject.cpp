#include "attack_priority.h"
#include <hook_tools.h>

namespace {

void __declspec(naked) getAttackPriorityWrapper() {

	static CUnit* target, *attacker;
	static u32 result;

	__asm {
		MOV target, EDI
		MOV attacker, EBX
		PUSHAD
	}

	result = hooks::getAttackPriorityHook(target, attacker);

	__asm {
		POPAD
		MOV EAX, result
		RETN
	}

}

;

void __declspec(naked) findBestAttackTargetWrapper() {

	static CUnit* unit;
	static CUnit* target;

	__asm {
		PUSH EBP
		MOV EBP, ESP
		MOV unit, EAX
		PUSHAD
	}

	target = hooks::findBestAttackTargetHook(unit);

	__asm {
		POPAD
		MOV EAX, target
		MOV ESP, EBP
		POP EBP
		RETN
	}

}

;

void __declspec(naked) findRandomAttackTargetWrapper() {

	static CUnit* unit;
	static CUnit* target;

	__asm {
		PUSH EBP
		MOV EBP, ESP
		MOV unit, ESI
		PUSHAD
	}

	target = hooks::findRandomAttackTargetHook(unit);

	__asm {
		POPAD
		MOV EAX, target
		MOV ESP, EBP
		POP EBP
		RETN
	}
}

;

} //unnamed namespace

namespace hooks {

void injectAttackPriorityHooks() {
	jmpPatch(getAttackPriorityWrapper,		0x00442160, 3);
	jmpPatch(findRandomAttackTargetWrapper,	0x00442FC0,	1);
	jmpPatch(findBestAttackTargetWrapper,	0x00443080, 1);
}

} //hooks

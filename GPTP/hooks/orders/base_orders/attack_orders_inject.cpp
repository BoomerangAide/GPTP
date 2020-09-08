//Injector source file for the Attack Orders hook module.
#include "attack_orders.h"
#include <hook_tools.h>

namespace {

void __declspec(naked) orders_ReaverAttack1Wrapper() {

	static CUnit* unit;

	__asm {
		MOV unit, EAX
		PUSHAD
	}

	hooks::orders_ReaverAttack1(unit);

	__asm {
		POPAD
		RETN
	}

}

;

void __declspec(naked) orders_CarrierAttack1Wrapper() {

	static CUnit* unit;

	__asm {
		PUSH EBP
		MOV EBP, ESP
		MOV unit, EAX
		PUSHAD
	}

	hooks::orders_CarrierAttack1(unit);

	__asm {
		POPAD
		MOV ESP, EBP
		POP EBP
		RETN
	}

}

;

void __declspec(naked) orders_StrafeUnit2Wrapper() {

	static CUnit* unit;

	__asm {
		MOV unit, EAX
		PUSHAD
	}

	hooks::orders_StrafeUnit2(unit);

	__asm {
		POPAD
		RETN
	}

}

;

void __declspec(naked) orders_StrafeUnitWrapper() {

	static CUnit* unit;

	__asm {
		PUSH EBP
		MOV EBP, ESP
		MOV unit, EAX
		PUSHAD
	}

	hooks::orders_StrafeUnit(unit);

	__asm {
		POPAD
		MOV ESP, EBP
		POP EBP
		RETN
	}

}

;

void __declspec(naked) orders_ReaverWrapper() {

	static CUnit* unit;

	__asm {
		MOV unit, EDI
		PUSHAD
	}

	hooks::orders_Reaver(unit);

	__asm {
		POPAD
		RETN
	}

}

;

void __declspec(naked) orders_CarrierWrapper() {

	static CUnit* unit;

	__asm {
		MOV unit, EDI
		PUSHAD
	}

	hooks::orders_Carrier(unit);

	__asm {
		POPAD
		RETN
	}

}

;

void __declspec(naked) orders_TurretAttackWrapper() {

	static CUnit* unit;

	__asm {
		PUSH EBP
		MOV EBP, ESP
		MOV unit, EAX
		PUSHAD
	}

	hooks::orders_TurretAttack(unit);

	__asm {
		POPAD
		MOV ESP, EBP
		POP EBP
		RETN
	}

}

;

void __declspec(naked) orders_AttackFixedRangeWrapper() {

	static CUnit* unit;

	__asm {
		PUSH EBP
		MOV EBP, ESP
		MOV unit, EAX
		PUSHAD
	}

	hooks::orders_AttackFixedRange(unit);

	__asm {
		POPAD
		MOV ESP, EBP
		POP EBP
		RETN
	}

}

;

void __declspec(naked) orders_SapUnitWrapper() {

	static CUnit* unit;

	__asm {
		MOV unit, EAX
		PUSHAD
	}

	hooks::orders_SapUnit(unit);

	__asm {
		POPAD
		RETN
	}

}

;

void __declspec(naked) orders_SapLocationWrapper() {

	static CUnit* unit;

	__asm {
		MOV unit, EAX
		PUSHAD
	}

	hooks::orders_SapLocation(unit);

	__asm {
		POPAD
		RETN
	}

}

;

void __declspec(naked) orders_AttackMoveEPWrapper() {

	static CUnit* unit;

	__asm {
		PUSH EBP
		MOV EBP, ESP
		MOV unit, EAX
		PUSHAD
	}

	hooks::orders_AttackMoveEP(unit);

	__asm {
		POPAD
		MOV ESP, EBP
		POP EBP
		RETN
	}

}

;

void __declspec(naked) orders_AttackMoveWrapper() {

	static CUnit* unit;

	__asm {
		MOV unit, ESI
		PUSHAD
	}

	hooks::orders_AttackMove(unit);

	__asm {
		POPAD
		RETN
	}

}

;

void __declspec(naked) orders_TowerAttackWrapper() {

	static CUnit* unit;

	__asm {
		MOV unit, EAX
		PUSHAD
	}

	hooks::orders_TowerAttack(unit);

	__asm {
		POPAD
		RETN
	}

}

;

void __declspec(naked) orders_AttackUnitWrapper() {
	
	static CUnit* unit;

	__asm {
		MOV unit, ESI
		PUSHAD
	}

	hooks::orders_AttackUnit(unit);

	__asm {
		POPAD
		RETN
	}

}

;

}//unnamed namespace

namespace hooks {

void injectAttackOrdersHooks() {
	jmpPatch(orders_ReaverAttack1Wrapper,		0x00465690, 2);
	jmpPatch(orders_CarrierAttack1Wrapper,		0x00465950, 0);
	jmpPatch(orders_StrafeUnit2Wrapper,			0x00465E00, 2);
	jmpPatch(orders_StrafeUnitWrapper,			0x00465F60, 0);
	jmpPatch(orders_ReaverWrapper,				0x004665D0, 3);
	jmpPatch(orders_CarrierWrapper,				0x004666A0, 0);
	jmpPatch(orders_TurretAttackWrapper,		0x00477980, 1);
	jmpPatch(orders_AttackFixedRangeWrapper,	0x00477D00, 0);
	jmpPatch(orders_SapUnitWrapper,				0x004788E0, 2);
	jmpPatch(orders_SapLocationWrapper,			0x00478A40, 3);
	jmpPatch(orders_AttackMoveEPWrapper,		0x00478DE0, 1);
	jmpPatch(orders_AttackMoveWrapper,			0x00479040, 0);
	jmpPatch(orders_TowerAttackWrapper,			0x00479150, 4);
	jmpPatch(orders_AttackUnitWrapper,			0x00479BD0, 2);
}

} //hooks

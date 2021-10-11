#include "unit_stat_cond.h"
#include <hook_tools.h>

namespace {

void __declspec(naked) UnitStatCond_Never_Wrapper() {

	static Bool32 bResult;

	__asm PUSHAD

	bResult = hooks::UnitStatCond_Never() ? 1 : 0;

	__asm {
		POPAD
		MOV EAX, bResult
		RETN
	}

}

;

void __declspec(naked) UnitStatCond_Powerup_Wrapper() {

	static Bool32 bResult;

	__asm PUSHAD

	bResult = hooks::UnitStatCond_Powerup() ? 1 : 0;

	__asm {
		POPAD
		MOV EAX, bResult
		RETN
	}

}

;

void __declspec(naked) UnitStatCond_Standard_Wrapper() {

	static Bool32 bResult;

	__asm PUSHAD

	bResult = hooks::UnitStatCond_Standard() ? 1 : 0;

	__asm {
		POPAD
		MOV EAX, bResult
		RETN
	}

}

;

void __declspec(naked) UnitStatCond_Egg_Wrapper() {

	static Bool32 bResult;

	__asm PUSHAD

	bResult = hooks::UnitStatCond_Egg() ? 1 : 0;

	__asm {
		POPAD
		MOV EAX, bResult
		RETN
	}

}

;

void __declspec(naked) UnitStatCond_Archon_Wrapper() {

	static Bool32 bResult;

	__asm PUSHAD

	bResult = hooks::UnitStatCond_Archon() ? 1 : 0;

	__asm {
		POPAD
		MOV EAX, bResult
		RETN
	}

}

;

void __declspec(naked) UnitStatCond_CarrierReaver_Wrapper() {

	static Bool32 bResult;

	__asm PUSHAD

	bResult = hooks::UnitStatCond_CarrierReaver() ? 1 : 0;

	__asm {
		POPAD
		MOV EAX, bResult
		RETN
	}

}

;

void __declspec(naked) UnitStatCond_Dropship_Wrapper() {

	static Bool32 bResult;

	__asm PUSHAD

	bResult = hooks::UnitStatCond_Dropship() ? 1 : 0;

	__asm {
		POPAD
		MOV EAX, bResult
		RETN
	}

}

;

void __declspec(naked) UnitStatCond_Building_Wrapper() {

	static Bool32 bResult;

	__asm PUSHAD

	bResult = hooks::UnitStatCond_Building() ? 1 : 0;

	__asm {
		POPAD
		MOV EAX, bResult
		RETN
	}

}

;

void __declspec(naked) UnitStatCond_overlord_Wrapper() {

	static Bool32 bResult;

	__asm PUSHAD

	bResult = hooks::UnitStatCond_overlord() ? 1 : 0;

	__asm {
		POPAD
		MOV EAX, bResult
		RETN
	}

}

;

void __declspec(naked) function_00424780_Wrapper() {

	static Bool32 bResult;

	__asm {
		PUSH EBP
		MOV EBP, ESP
		PUSHAD
	}

	bResult = hooks::function_00424780() ? 1 : 0;

	__asm {
		POPAD
		MOV EAX, bResult
		MOV ESP, EBP
		POP EBP
		RETN
	}

}

;

void __declspec(naked) function_00424A10_Wrapper() {

	static Bool32 bResult;

	__asm PUSHAD

	bResult = hooks::function_00424A10() ? 1 : 0;

	__asm {
		POPAD
		MOV EAX, bResult
		RETN
	}

}

;

void __declspec(naked) function_00425230_Wrapper() {

	static Bool32 bResult;

	__asm {
		PUSH EBP
		MOV EBP, ESP
		PUSHAD
	}

	bResult = hooks::function_00425230() ? 1 : 0;

	__asm {
		POPAD
		MOV EAX, bResult
		MOV ESP, EBP
		POP EBP
		RETN
	}

}

;

}//unnamed namespace

namespace hooks {

void injectUnitStatCondHooks() {
	jmpPatch(UnitStatCond_Never_Wrapper,		0x00424500, 0);
	jmpPatch(UnitStatCond_Powerup_Wrapper,		0x00424520, 1);
	jmpPatch(UnitStatCond_Standard_Wrapper,		0x00424980, 0);
	jmpPatch(UnitStatCond_Egg_Wrapper,			0x00424AC0, 0);
	jmpPatch(UnitStatCond_Archon_Wrapper,		0x00424AF0, 0);
	jmpPatch(UnitStatCond_CarrierReaver_Wrapper,0x00424B50, 0);
	jmpPatch(UnitStatCond_Dropship_Wrapper,		0x00424F10, 0);
	jmpPatch(UnitStatCond_Building_Wrapper,		0x00425180, 0);
	jmpPatch(UnitStatCond_overlord_Wrapper,		0x00425900, 0);
	jmpPatch(function_00424780_Wrapper,			0x00424780, 0);
	jmpPatch(function_00424A10_Wrapper,			0x00424A10, 0);
	jmpPatch(function_00425230_Wrapper,			0x00425230, 4);
}

} //hooks

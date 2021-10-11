#include "larva_creep_spawn.h"
#include <hook_tools.h>

namespace {

void __declspec(naked) function_0049D660_Wrapper() {

	static CUnit* main_building;

	__asm {
		PUSH EBP
		MOV EBP, ESP
		MOV main_building, ESI
		PUSHAD
	}

	hooks::function_0049D660(main_building);

	__asm {
		POPAD
		MOV ESP, EBP
		POP EBP
		RET
	}

}

;

void __declspec(naked) function_004E8C80_Wrapper(){

	static CUnit* main_building;
	static CUnit* unit;
	static bool bResult;

	__asm {
		MOV unit, ECX
		MOV main_building, EDX
		PUSHAD
	}

	bResult = hooks::function_004E8C80(unit,main_building);

	if(bResult)
		__asm {
			POPAD
			MOV EAX, 0x00000001
			RET
		}
	else
		__asm {
			POPAD
			XOR EAX, EAX
			RET
		}

}

;
	
void __declspec(naked) orders_StopCreepGrowth_Wrapper(){

	static CUnit* unit;

	__asm {
		PUSH EBP
		MOV EBP, ESP
		MOV unit, EAX
		PUSHAD
	}

	hooks::orders_StopCreepGrowth(unit);

	__asm {
		POPAD
		MOV ESP, EBP
		POP EBP
		RET
	}

}

;

void __declspec(naked) orders_InitCreepGrowth_Wrapper(){

	static CUnit* unit;

	__asm {
		PUSH EBP
		MOV EBP, ESP
		MOV unit, EAX
		PUSHAD
	}

	hooks::orders_InitCreepGrowth(unit);

	__asm {
		POPAD
		MOV ESP, EBP
		POP EBP
		RET
	}

}

;

void __declspec(naked) secondaryOrd_SpawningLarva_Wrapper(){

	static CUnit* unit;

	__asm {
		PUSH EBP
		MOV EBP, ESP
		MOV unit, ESI
		PUSHAD
	}

	hooks::secondaryOrd_SpawningLarva(unit);

	__asm {
		POPAD
		MOV ESP, EBP
		POP EBP
		RET
	}

}

;

void __declspec(naked) secondaryOrd_SpreadCreepSpawningLarva_Wrapper(){

	static CUnit* unit;

	__asm {
		MOV unit, EAX
		PUSHAD
	}

	hooks::secondaryOrd_SpreadCreepSpawningLarva(unit);

	__asm {
		POPAD
		RET
	}

}

;

} //unnamed namespace

namespace hooks {

	void injectLarvaCreepSpawnHooks() {
		jmpPatch(function_0049D660_Wrapper,						0x0049D660, 1);
		jmpPatch(function_004E8C80_Wrapper,						0x004E8C80, 0);
		jmpPatch(orders_StopCreepGrowth_Wrapper,				0x004E95E0, 1);
		jmpPatch(orders_InitCreepGrowth_Wrapper,				0x004E96D0, 1);		
		jmpPatch(secondaryOrd_SpawningLarva_Wrapper,			0x004EA780, 1);
		jmpPatch(secondaryOrd_SpreadCreepSpawningLarva_Wrapper,	0x004EA880, 3);	
	}

}

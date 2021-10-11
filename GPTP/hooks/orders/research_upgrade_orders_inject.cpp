//Injector source file for the Enter Nydus Order hook module.
#include "research_upgrade_orders.h"
#include <hook_tools.h>

namespace {

void __declspec(naked) orders_UpgradeWrapper() {

	static CUnit* unit;

	__asm {
		PUSH EBP
		MOV EBP, ESP
		MOV unit, EAX
		PUSHAD
	}

	hooks::orders_Upgrade(unit);

	__asm {
		POPAD
		MOV ESP, EBP
		POP EBP
		RETN
	}

}

;

void __declspec(naked) orders_ResearchTechWrapper() {

	static CUnit* unit;

	__asm {
		PUSH EBP
		MOV EBP, ESP
		MOV unit, EAX
		PUSHAD
	}

	hooks::orders_ResearchTech(unit);

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

	void injectResearchUpgradeOrdersHooks() {
		jmpPatch(orders_UpgradeWrapper,			0x004546A0, 1);
		jmpPatch(orders_ResearchTechWrapper,	0x004548B0, 1);
	}

} //hooks

#include "orders_root.h"
#include <hook_tools.h>

namespace {

void __declspec(naked) secondaryOrdersRootWrapper() {

	static CUnit* unit;

	__asm {
		MOV unit, EAX
		PUSHAD
	}

	hooks::secondaryOrdersRoot(unit);

	__asm {
		POPAD
		RETN
	}

}

;

void __declspec(naked) mainOrdersRoottWrapper() {

	static CUnit* unit;

	__asm {
		MOV unit, EAX
		PUSHAD
	}

	hooks::mainOrdersRoot(unit);

	__asm {
		POPAD
		RETN
	}

}

;

}//unnamed namespace

namespace hooks {

void injectOrdersRootHooks() {
	jmpPatch(secondaryOrdersRootWrapper,	0x004EC170,	1);
	jmpPatch(mainOrdersRoottWrapper,		0x004EC4D0,	3);
}

} //hooks
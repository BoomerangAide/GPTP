#include "updateSelectedUnitsData.h"
#include <hook_tools.h>

namespace {

void __declspec(naked) updateSelectedUnitData_Wrapper() {

	__asm PUSHAD

	hooks::updateSelectedUnitData();

	__asm {
		POPAD
		RETN
	}

}

;

}//unnamed namespace

namespace hooks {

	void injectUpdateSelectedUnitsDataHook() {
		jmpPatch(updateSelectedUnitData_Wrapper, 0x004C38B0, 3);
	}

}
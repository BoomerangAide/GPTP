//Injector source file for the Utils hooks module.
#include "replace_unit.h"
#include <hook_tools.h>

namespace {

void __declspec(naked) replaceUnitWithTypeWrapper() {

	static CUnit* unit;
	static u32 newUnitId;

	__asm {
		PUSH EBP
		MOV EBP, ESP
		MOV unit, EAX
		MOV EAX, [EBP+0x08]
		MOV newUnitId, EAX
		PUSHAD
	}

	hooks::replaceUnitWithType(unit, newUnitId);

	__asm {
		POPAD
		MOV ESP, EBP
		POP EBP
		RETN 4
	}


}

}//unnamed namespace

namespace hooks {

void injectReplaceUnitWithTypeHook() {
	jmpPatch(replaceUnitWithTypeWrapper,			0x0049FED0, 1);
}

} //hooks
//Injector source file for the Tech Target Check hook module.
#include "tech_target_check.h"
#include <hook_tools.h>

namespace {

void __declspec(naked) getTechUseErrorMessageWrapper() {

	static CUnit* target;
	static u8 castingPlayer;
	static u16 techId;
	static u32 errorMessage;

	__asm {
		PUSH EBP
		MOV EBP, ESP
		MOV target, EAX
		MOV castingPlayer, BL
		MOV EAX, [EBP+0x08]
		MOV techId, AX
		PUSHAD
	}

	errorMessage = hooks::getTechUseErrorMessageHook(target, castingPlayer, techId);

	__asm {
		POPAD
		MOV EAX, errorMessage
		POP EBP
		RETN 4
	}

}

;

} //Unnamed namespace

namespace hooks {

void injectTechTargetCheckHooks() {
	jmpPatch(getTechUseErrorMessageWrapper, 0x00491E80, 4);
}

} //hooks

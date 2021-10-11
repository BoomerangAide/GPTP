//Injector source file for the Cheat Codes hooks module.
#include "cheat_codes.h"
#include <hook_tools.h>

namespace {

void __declspec(naked) campaignTypeCheatStringsWrapper() {

	static char* textPassed;
	static u32* newCheatState;
	static Bool32 result;

	__asm {
		PUSH EBP
		MOV EBP, ESP
		MOV textPassed, EDI
		MOV newCheatState, EBX
		PUSHAD
	}

	result = hooks::campaignTypeCheatStrings(textPassed, newCheatState);

	__asm {
		POPAD
		MOV EAX, result
		MOV ESP, EBP
		POP EBP
		RETN
	}

}

;

void __declspec(naked) cheatActivationWrapper() {

	static u32 newCheatState;
	static char* playerName;

	__asm {

		PUSH EBP
		MOV EBP, ESP

		MOV newCheatState, EAX

		MOV EAX, [EBP+0x08]
		MOV playerName, EAX

		PUSHAD

	}

	hooks::cheatActivation(newCheatState, playerName);

	__asm {
		POPAD
		MOV ESP, EBP
		POP EBP
		RETN 4
	}

}

;

}//unnamed namespace

namespace hooks {

	void injectCheatCodesHooks() {
		jmpPatch(campaignTypeCheatStringsWrapper,	0x004B1DC0, 4);
		jmpPatch(cheatActivationWrapper,			0x004B1FA0, 4);
	}

} //hooks
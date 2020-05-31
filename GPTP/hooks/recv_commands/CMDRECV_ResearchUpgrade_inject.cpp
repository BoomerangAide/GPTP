//Injector source file for the CMDRECV Research and Upgrade hooks module.
#include "CMDRECV_ResearchUpgrade.h"
#include <hook_tools.h>

namespace {

void __declspec(naked) CMDRECV_UpgradeWrapper() {

	static u8 bUpgradeType;

	__asm {
		PUSH EBP
		MOV EBP, ESP
		MOV EAX, [EBP+0x08]
		MOV AL, [EAX+0x01]
		MOV bUpgradeType, AL
		PUSHAD
	}

	hooks::CMDRECV_Upgrade(bUpgradeType);

	__asm {
		POPAD
		POP EBP
		RETN 4
	}

}

;

void __declspec(naked) CMDRECV_ResearchWrapper() {

	static u8 bTechType;

	__asm {
		PUSH EBP
		MOV EBP, ESP
		MOV EAX, [EBP+0x08]
		MOV AL, [EAX+0x01]
		MOV bTechType, AL
		PUSHAD
	}

	hooks::CMDRECV_Research(bTechType);

	__asm {
		POPAD
		POP EBP
		RETN 4
	}

}

;

}//unnamed namespace

namespace hooks {

	void injectCMDRECV_ResearchUpgradeHooks() {
		jmpPatch(CMDRECV_UpgradeWrapper,	0x004C1B20, 6);
		jmpPatch(CMDRECV_ResearchWrapper,	0x004C1BA0, 6);
	}

} //hooks
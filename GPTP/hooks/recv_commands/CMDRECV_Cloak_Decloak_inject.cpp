#include "CMDRECV_Cloak_Decloak.h"
#include <hook_tools.h>

namespace {

void __declspec(naked) CMDRECV_DecloakWrapper() {

	__asm PUSHAD

	hooks::CMDRECV_Decloak();

	__asm {
		POPAD
		RETN
	}

}

;

void __declspec(naked) CMRECV_CloakWrapper() {

	__asm PUSHAD

	hooks::CMDRECV_Cloak();

	__asm {
		POPAD
		RETN
	}

}

;

} //unnamed namespace

namespace hooks {

void CMDRECV_DecloakWrapper() {
	jmpPatch(CMDRECV_DecloakWrapper,0x004C0660, 3);
	jmpPatch(CMRECV_CloakWrapper,	0x004C0720, 3);
}

} //hooks

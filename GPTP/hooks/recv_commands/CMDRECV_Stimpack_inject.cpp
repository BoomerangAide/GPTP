//Injector source file for the CMDRECV Stimpack hook module.
#include "CMDRECV_Stimpack.h"
#include <hook_tools.h>

namespace {

void __declspec(naked) CMDRECV_Stimpack_Wrapper() {

	__asm {
		PUSHAD
	}

	hooks::CMDRECV_Stimpack();

	__asm {
		POPAD
		RETN
	}

}

;

;

}//unnamed namespace

namespace hooks {
	void injectCMDRECV_StimpackHook() {
		jmpPatch(CMDRECV_Stimpack_Wrapper, 0x004C2F30, 3);
	}
} //hooks
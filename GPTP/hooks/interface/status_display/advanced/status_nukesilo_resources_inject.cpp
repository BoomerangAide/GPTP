#include "status_nukesilo_resources.h"
#include <hook_tools.h>

namespace {

void __declspec(naked) stats_resources_Wrapper() {

	static BinDlg* dialog;

	__asm {
		MOV dialog, EAX
		PUSHAD
	}

	hooks::stats_resources(dialog);

	__asm {
		POPAD
		RETN
	}

}

;

void __declspec(naked) stats_nukesilo_Wrapper() {

	static BinDlg* dialog;

	__asm {

		PUSH EBP
		MOV EBP, ESP

		MOV EAX, [EBP+0x08]
		MOV dialog, EAX

		PUSHAD

	}

	hooks::stats_nukesilo(dialog);

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

void injectStatusNukeSilo_Resources_Hooks() {
	jmpPatch(stats_resources_Wrapper,	0x00426190, 1);
	jmpPatch(stats_nukesilo_Wrapper,	0x00426FF0, 3);
}

} //hooks
#include "stats_panel_display.h"
#include <hook_tools.h>

namespace {

void __declspec(naked) stats_panel_display_Wrapper() {

	static BinDlg* dialog;

	__asm {
		PUSH EBP
		MOV EBP, ESP
		MOV EAX, [EBP+0x08]
		MOV dialog, EAX
		PUSHAD
	}

	hooks::stats_panel_display(dialog);

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

void injectStatsPanelDisplayHook() {
	jmpPatch(stats_panel_display_Wrapper, 0x00426C60, 1);
}

} //hooks

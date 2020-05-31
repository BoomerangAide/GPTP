#include "status_research_upgrade.h"
#include <hook_tools.h>

namespace {

void __declspec(naked) stats_upgrade_in_progress_Wrapper() {

	static BinDlg* dialog;

	__asm {

		PUSH EBP
		MOV EBP, ESP

		MOV EAX, [EBP+0x08]
		MOV dialog, EAX

		PUSHAD

	}

	hooks::stats_upgrade_in_progress(dialog);

	__asm {
		POPAD
		POP EBP
		RETN 4
	}

}

;

void __declspec(naked) stats_research_in_progress_Wrapper() {

	static BinDlg* dialog;

	__asm {

		PUSH EBP
		MOV EBP, ESP

		MOV EAX, [EBP+0x08]
		MOV dialog, EAX

		PUSHAD

	}

	hooks::stats_research_in_progress(dialog);

	__asm {
		POPAD
		POP EBP
		RETN 4
	}

}

;

}//unnamed namespace

namespace hooks {

void injectStatusResearchUpgradeHooks() {
	jmpPatch(stats_upgrade_in_progress_Wrapper,	0x00426500, 3);
	jmpPatch(stats_research_in_progress_Wrapper,0x004266F0, 3);
}

} //hooks
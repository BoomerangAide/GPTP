#include "status_base_text.h"
#include <hook_tools.h>

namespace {

void __declspec(naked) stats_text_energy_timer_Wrapper() {

	static BinDlg* dialog;

	__asm {
		MOV dialog, EBX
		PUSHAD
	}

	hooks::stats_text_energy_timer(dialog);

	__asm {
		POPAD
		RETN
	}

}

;

void __declspec(naked) stats_text_status_ranking_Wrapper() {

	static BinDlg* dialog;

	__asm {

		PUSH EBP
		MOV EBP, ESP

		MOV EAX, [EBP+0x08]
		MOV dialog, EAX

		PUSHAD

	}

	hooks::stats_text_status_ranking(dialog);

	__asm {
		POPAD
		POP EBP
		RETN 4
	}

}

;

void __declspec(naked) stats_text_kills_Wrapper() {

	static BinDlg* dialog;

	__asm {
		MOV dialog, EBX
		PUSHAD
	}

	hooks::stats_text_kills(dialog);

	__asm {
		POPAD
		RETN
	}

}

;

void __declspec(naked) stats_text_hp_shield_Wrapper() {

	static BinDlg* dialog;

	__asm {

		PUSH EBP
		MOV EBP, ESP

		MOV EAX, [EBP+0x08]
		MOV dialog, EAX

		PUSHAD

	}

	hooks::stats_text_hp_shield(dialog);

	__asm {
		POPAD
		POP EBP
		RETN 4
	}

}

;

}//unnamed namespace

namespace hooks {

void injectStatusBaseTextHooks() {
	jmpPatch(stats_text_energy_timer_Wrapper,	0x00425A30, 2);
	jmpPatch(stats_text_status_ranking_Wrapper,	0x00425B50, 0);
	jmpPatch(stats_text_kills_Wrapper,			0x00425DD0, 2);
	jmpPatch(stats_text_hp_shield_Wrapper,		0x004263E0, 0);
}

} //hooks
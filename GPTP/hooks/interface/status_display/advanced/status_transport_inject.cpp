#include "status_transport.h"
#include <hook_tools.h>

namespace {

void __declspec(naked) stats_transports_loaded_units_buttons_Wrapper() {

	static BinDlg* dialog;

	__asm {

		PUSH EBP
		MOV EBP, ESP

		MOV dialog, EAX

		PUSHAD

	}

	hooks::stats_transports_loaded_units_buttons(dialog);

	__asm {
		POPAD
		MOV ESP, EBP
		POP EBP
		RETN
	}

}

;

void __declspec(naked) stats_transports_cache_for_display_Wrapper() {

	static BinDlg* dialog;

	__asm {

		PUSH EBP
		MOV EBP, ESP

		MOV dialog, EAX

		PUSHAD

	}

	hooks::stats_transports_cache_for_display(dialog);

	__asm {
		POPAD
		MOV ESP, EBP
		POP EBP
		RETN
	}

}

;

}//unnamed namespace

namespace hooks {

void injectStatusTransportHooks() {
	jmpPatch(stats_transports_loaded_units_buttons_Wrapper,	0x00424BA0, 1);
	jmpPatch(stats_transports_cache_for_display_Wrapper,	0x00424FC0, 3);
}

} //hooks
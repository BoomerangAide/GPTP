#include "status_supply_provider.h"
#include <hook_tools.h>

namespace {

void __declspec(naked) stats_supply_provider_Wrapper() {

	static BinDlg* dialog;

	__asm {

		PUSH EBP
		MOV EBP, ESP

		MOV dialog, EAX

		PUSHAD

	}

	hooks::stats_supply_provider(dialog);

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

void injectStatusSupplyProviderHook() {
	jmpPatch(stats_supply_provider_Wrapper,	0x00427540, 1);
}

} //hooks
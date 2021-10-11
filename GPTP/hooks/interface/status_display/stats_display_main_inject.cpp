#include "stats_display_main.h"
#include <hook_tools.h>

namespace {

void __declspec(naked) function_00458120_Wrapper() {

	__asm PUSHAD

	hooks::function_00458120();

	__asm {
		POPAD
		RETN
	}

}
	
;

}//unnamed namespace

namespace hooks {

void injectStatsDisplayMainHook() {
	jmpPatch(function_00458120_Wrapper, 0x00458120, 0);
}

} //hooks

#include "weapon_armor_tooltip.h"
#include <hook_tools.h>
#include <SCBW/scbwdata.h>

namespace {


const u32 Func_CreateContextHelp = 0x00481510;
//Identical to function @ 0x00457350
void createContextHelpFromDialog(BinDlg* dialog, const char *text) {

	u32 x = dialog->bounds.right, y = dialog->bounds.bottom;

	if (dialog->controlType != 0) {
		x += dialog->parent->bounds.left;
		y += dialog->parent->bounds.top;
	}

	__asm {
		PUSHAD
		PUSH text
		PUSH y
		MOV EAX, x
		CALL Func_CreateContextHelp
		POPAD
	}

}

//Replaces function @ 0x004576A0
void __stdcall statdatalUserInteractWrapper(BinDlg *consoleIcon) {

	GuiOverlay *gui = (GuiOverlay*) consoleIcon->user;

	switch (gui->overlayType) {
		case 0:
			createContextHelpFromDialog(consoleIcon,
				hooks::getWeaponTooltipString((u8) gui->id, *activePortraitUnit));
			break;
		case 1:
			createContextHelpFromDialog(consoleIcon,
				hooks::getArmorTooltipString(*activePortraitUnit));
			break;
		case 2:
			createContextHelpFromDialog(consoleIcon,
				hooks::getShieldTooltipString(*activePortraitUnit));
			break;
		case 3:
			createContextHelpFromDialog(consoleIcon,
				hooks::getSpecialTooltipString(gui->id, *activePortraitUnit));
			break;
	}
}

} //unnamed namespace


namespace hooks {

void injectUnitTooltipHook() {
	callPatch(&statdatalUserInteractWrapper, 0x00457D4B);
}

} //hooks

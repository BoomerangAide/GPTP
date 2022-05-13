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

;

//Replaces function @ 0x004576A0 by replacing the call to it
void __stdcall statdatalUserInteractWrapper(BinDlg *consoleIcon) {

	UnknownUserStruct* iconData = (UnknownUserStruct*) consoleIcon->user;

	switch (iconData->tooltipType_06) {
		case PanelTooltipTypes::WeaponPanelTooltip:
			createContextHelpFromDialog(consoleIcon,
				hooks::getWeaponTooltipString((u8) iconData->id_08, *activePortraitUnit));
			break;
		case PanelTooltipTypes::ArmorPanelTooltip:
			createContextHelpFromDialog(consoleIcon,
				hooks::getArmorTooltipString(*activePortraitUnit));
			break;
		case PanelTooltipTypes::ShieldPanelTooltip:
			createContextHelpFromDialog(consoleIcon,
				hooks::getShieldTooltipString(*activePortraitUnit));
			break;
		case PanelTooltipTypes::SpecialPanelTooltip:
			createContextHelpFromDialog(consoleIcon,
				hooks::getSpecialTooltipString(iconData->id_08, *activePortraitUnit));
			break;
	}
}

;

} //unnamed namespace


namespace hooks {

void injectUnitTooltipHook() {
	callPatch(&statdatalUserInteractWrapper, 0x00457D4B);
}

} //hooks

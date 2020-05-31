#include "unit_stat_act.h"
#include <hook_tools.h>

namespace {

void __declspec(naked) UnitStatAct_Default_Wrapper() {

	static BinDlg* dialog;

	__asm {
		MOV dialog, ECX
		PUSHAD
	}

	hooks::UnitStatAct_Default(dialog);

	__asm {
		POPAD
		RETN
	}

}

;

void __declspec(naked) UnitStatAct_Powerup_Wrapper() {

	static BinDlg* dialog;

	__asm {
		MOV dialog, ECX
		PUSHAD
	}

	hooks::UnitStatAct_Powerup(dialog);

	__asm {
		POPAD
		RETN
	}

}

;

void __declspec(naked) UnitStatAct_Standard_Wrapper() {

	static BinDlg* dialog;

	__asm {
		MOV dialog, ECX
		PUSHAD
	}

	hooks::UnitStatAct_Standard(dialog);

	__asm {
		POPAD
		RETN
	}

}

;

void __declspec(naked) UnitStatAct_Egg_Wrapper() {

	static BinDlg* dialog;

	__asm {
		MOV dialog, ECX
		PUSHAD
	}

	hooks::UnitStatAct_Egg(dialog);

	__asm {
		POPAD
		RETN
	}

}

;

void __declspec(naked) UnitStatAct_Archon_Wrapper() {

	static BinDlg* dialog;

	__asm {
		MOV dialog, ECX
		PUSHAD
	}

	hooks::UnitStatAct_Archon(dialog);

	__asm {
		POPAD
		RETN
	}

}

;

void __declspec(naked) UnitStatAct_CarrierReaver_Wrapper() {

	static BinDlg* dialog;

	__asm {
		MOV dialog, ECX
		PUSHAD
	}

	hooks::UnitStatAct_CarrierReaver(dialog);

	__asm {
		POPAD
		RETN
	}

}

;

void __declspec(naked) UnitStatAct_Building_Wrapper() {

	static BinDlg* dialog;

	__asm {
		MOV dialog, ECX
		PUSHAD
	}

	hooks::UnitStatAct_Building(dialog);

	__asm {
		POPAD
		RETN
	}

}

;

void __declspec(naked) UnitStatAct_Dropship_Wrapper() {

	static BinDlg* dialog;

	__asm {
		MOV dialog, ECX
		PUSHAD
	}

	hooks::UnitStatAct_Dropship(dialog);

	__asm {
		POPAD
		RETN
	}

}

;

void __declspec(naked) UnitStatAct_Overlord_Wrapper() {

	static BinDlg* dialog;

	__asm {
		MOV dialog, ECX
		PUSHAD
	}

	hooks::UnitStatAct_Overlord(dialog);

	__asm {
		POPAD
		RETN
	}

}

;

}//unnamed namespace

namespace hooks {

void injectUnitStatActHooks() {
	jmpPatch(UnitStatAct_Default_Wrapper,		0x00425EE0, 4);
	jmpPatch(UnitStatAct_Powerup_Wrapper,		0x00426EE0, 2);
	jmpPatch(UnitStatAct_Standard_Wrapper,		0x00426F50, 0);
	jmpPatch(UnitStatAct_Egg_Wrapper,			0x00427260, 0);
	jmpPatch(UnitStatAct_Archon_Wrapper,		0x004273E0, 0);
	jmpPatch(UnitStatAct_CarrierReaver_Wrapper,	0x004274A0, 0);
	jmpPatch(UnitStatAct_Building_Wrapper,		0x00427890, 3);
	jmpPatch(UnitStatAct_Dropship_Wrapper,		0x00427C90, 2);
	jmpPatch(UnitStatAct_Overlord_Wrapper,		0x00427D30, 0);
}

} //hooks
#include "panels_generators.h"
#include <hook_tools.h>

namespace {

void __declspec(naked) StatsNukesCount_Wrapper() {

	static BinDlg* dialog;
	static u32 index;

	__asm {

		PUSH EBP
		MOV EBP, ESP

		MOV dialog, EAX

		MOV EAX, [EBP+0x08]
		MOV index, EAX

		PUSHAD

	}

	hooks::StatsNukesCount(dialog, index);

	__asm {
		POPAD
		MOV ESP, EBP
		POP EBP
		RETN 4
	}

}

;

void __declspec(naked) StatHangerCount_Wrapper() {

	static BinDlg* dialog;
	static u32 index;

	__asm {

		PUSH EBP
		MOV EBP, ESP

		MOV dialog, EBX

		MOV EAX, [EBP+0x08]
		MOV index, EAX

		PUSHAD

	}

	hooks::StatHangerCount(dialog, index);

	__asm {
		POPAD
		MOV ESP, EBP
		POP EBP
		RETN 4
	}

}

;

void __declspec(naked) StatsShieldLevel_Wrapper() {

	static BinDlg* dialog;
	static u32 index;

	__asm {

		PUSH EBP
		MOV EBP, ESP

		MOV dialog, EAX

		MOV EAX, [EBP+0x08]
		MOV index, EAX

		PUSHAD

	}

	hooks::StatsShieldLevel(dialog, index);

	__asm {
		POPAD
		MOV ESP, EBP
		POP EBP
		RETN 4
	}

}

;

void __declspec(naked) StatsArmorLevel_Wrapper() {

	static BinDlg* dialog;
	static u32 index;

	__asm {

		PUSH EBP
		MOV EBP, ESP

		MOV dialog, EAX

		MOV EAX, [EBP+0x08]
		MOV index, EAX

		PUSHAD

	}

	hooks::StatsArmorLevel(dialog, index);

	__asm {
		POPAD
		MOV ESP, EBP
		POP EBP
		RETN 4
	}

}

;

void __declspec(naked) StatsWeaponLevel_Wrapper() {

	static BinDlg* dialog;
	static u32 index;
	static u32 weaponId;

	__asm {

		PUSH EBP
		MOV EBP, ESP

		MOV dialog, EAX

		MOV EAX, [EBP+0x08]
		MOV index, EAX

		MOV EAX, [EBP+0x0C]
		MOV weaponId, EAX

		PUSHAD

	}

	hooks::StatsWeaponLevel(dialog, index, weaponId);

	__asm {
		POPAD
		MOV ESP, EBP
		POP EBP
		RETN 8
	}

}

;

void __declspec(naked) StatSpidermineCount_Wrapper() {

	static BinDlg* dialog;
	static u32 index;
	static Bool32 result;

	__asm {

		PUSH EBP
		MOV EBP, ESP

		MOV dialog, EAX

		MOV EAX, [EBP+0x08]
		MOV index, EAX

		PUSHAD

	}

	result = hooks::StatSpidermineCount(dialog, index);

	__asm {
		POPAD
		MOV EAX, result
		MOV ESP, EBP
		POP EBP
		RETN 4
	}

}

;

}//unnamed namespace

namespace hooks {

void injectPanelsGeneratorsHook() {
	jmpPatch(StatsNukesCount_Wrapper,		0x00425310, 5);
	jmpPatch(StatHangerCount_Wrapper,		0x004253D0, 5);
	jmpPatch(StatsShieldLevel_Wrapper,		0x00425510, 1);
	jmpPatch(StatsArmorLevel_Wrapper,		0x00425600, 1);
	jmpPatch(StatsWeaponLevel_Wrapper,		0x00425790, 1);
	jmpPatch(StatSpidermineCount_Wrapper,	0x00426300, 0);
}

} //hooks

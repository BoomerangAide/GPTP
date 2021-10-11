#include "status_buildmorphtrain.h"
#include <hook_tools.h>

namespace {

void __declspec(naked) stats_AddOnBuilder_Wrapper() {

	static BinDlg* dialog;

	__asm {
		MOV dialog, EDI
		PUSHAD
	}

	hooks::stats_AddOnBuilder(dialog);

	__asm {
		POPAD
		RETN
	}

}

;

void __declspec(naked) stats_IncompleteUnit_Wrapper() {

	static BinDlg* dialog;

	__asm {
		MOV dialog, EDI
		PUSHAD
	}

	hooks::stats_IncompleteUnit(dialog);

	__asm {
		POPAD
		RETN
	}

}

;

void __declspec(naked) stats_QueueProgress_Wrapper() {

	static BinDlg* dialog;

	__asm {
		PUSH EBP
		MOV EBP, ESP
		MOV EAX, [EBP+0x08]
		MOV dialog, EAX
		PUSHAD
	}

	hooks::stats_QueueProgress(dialog);

	__asm {
		POPAD
		MOV ESP, EBP
		POP EBP
		RETN 4
	}

}

;

void __declspec(naked) stats_MorphProgress_Wrapper() {

	static BinDlg* dialog;
	static int index;
	static u32 unk;

	__asm {

		PUSH EBP
		MOV EBP, ESP

		MOV dialog, EAX
		MOV index, ECX

		MOV EAX, [EBP+0x08]
		MOV unk, EAX

		PUSHAD

	}

	hooks::stats_MorphProgress(dialog,index,unk);

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

void injectStatusBuildMorphTrain_Hooks() {
	jmpPatch(stats_AddOnBuilder_Wrapper,	0x00425F30, 2);
	jmpPatch(stats_IncompleteUnit_Wrapper,	0x004260C0, 2);
	jmpPatch(stats_QueueProgress_Wrapper,	0x004268D0, 1);
	jmpPatch(stats_MorphProgress_Wrapper,	0x00457250, 1);
}

} //hooks

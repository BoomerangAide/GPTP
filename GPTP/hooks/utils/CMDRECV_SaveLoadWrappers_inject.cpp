//Injector source file for the CMDRECV Save Load Wrappers hooks module.
#include "CMDRECV_SaveLoadWrappers.h"
#include <hook_tools.h>

namespace {

static const void* Func_CMDRECV_SaveGame = (void*)0x004C2910;
static const void* Func_CMDRECV_LoadGame = (void*)0x004CF950;

static const DWORD injectCMDRECV_SaveGame_wrapper = (DWORD)0x004866F9;
static const void* returnCMDRECV_SaveGame_wrapper = (void*)0x004866FE;

static const DWORD injectCMDRECV_LoadGame_wrapper_1 = (DWORD)0x00486748;
static const DWORD injectCMDRECV_LoadGame_wrapper_2 = (DWORD)0x004B6516;
static const DWORD injectCMDRECV_LoadGame_wrapper_3 = (DWORD)0x004C1CAF;
static const DWORD injectCMDRECV_LoadGame_wrapper_4 = (DWORD)0x004D419A;
static const void* returnCMDRECV_LoadGame_wrapper_1 = (void*)0x0048674D;
static const void* returnCMDRECV_LoadGame_wrapper_2 = (void*)0x004B651B;
static const void* returnCMDRECV_LoadGame_wrapper_3 = (void*)0x004C1CB4;
static const void* returnCMDRECV_LoadGame_wrapper_4 = (void*)0x004D419F;


void __declspec(naked) CMDRECV_SaveGameWrapper() {

	static char* savefileName;

	__asm {
		PUSH EDI
		LEA EDI, [EAX+0x05]
		MOV savefileName, EDI
		POP EDI
		CALL Func_CMDRECV_SaveGame
		PUSHAD
	}

	hooks::CMDRECV_SaveGameAfter(savefileName);

	__asm {
		POPAD
		JMP returnCMDRECV_SaveGame_wrapper
	}

}

;

void __declspec(naked) CMDRECV_LoadGameWrapper1() {

	static char* savefileName;
	static Bool32 loadSuccess;

	__asm {
		MOV savefileName, EBX
		CALL Func_CMDRECV_LoadGame
		MOV loadSuccess, EAX
		PUSHAD
	}

	hooks::CMDRECV_LoadGameAfter(savefileName, loadSuccess == 1);

	__asm {
		POPAD
		JMP returnCMDRECV_LoadGame_wrapper_1
	}

}

;

void __declspec(naked) CMDRECV_LoadGameWrapper2() {

	static char* savefileName;
	static Bool32 loadSuccess;

	__asm {
		MOV savefileName, EBX
		CALL Func_CMDRECV_LoadGame
		MOV loadSuccess, EAX
		PUSHAD
	}

	hooks::CMDRECV_LoadGameAfter(savefileName, loadSuccess == 1);

	__asm {
		POPAD
		JMP returnCMDRECV_LoadGame_wrapper_2
	}

}

;

void __declspec(naked) CMDRECV_LoadGameWrapper3() {

	static char* savefileName;
	static Bool32 loadSuccess;

	__asm {
		MOV savefileName, EBX
		CALL Func_CMDRECV_LoadGame
		MOV loadSuccess, EAX
		PUSHAD
	}

	hooks::CMDRECV_LoadGameAfter(savefileName, loadSuccess == 1);

	__asm {
		POPAD
		JMP returnCMDRECV_LoadGame_wrapper_3
	}

}

;

void __declspec(naked) CMDRECV_LoadGameWrapper4() {

	static char* savefileName;
	static Bool32 loadSuccess;

	__asm {
		MOV savefileName, EBX
		CALL Func_CMDRECV_LoadGame
		MOV loadSuccess, EAX
		PUSHAD
	}

	hooks::CMDRECV_LoadGameAfter(savefileName, loadSuccess == 1);

	__asm {
		POPAD
		JMP returnCMDRECV_LoadGame_wrapper_4
	}

}

;

}//unnamed namespace

namespace hooks {

	void injectCMDRECV_SaveLoadWrappersHooks() {
		jmpPatch(CMDRECV_LoadGameWrapper1,	injectCMDRECV_LoadGame_wrapper_1,	0);
		jmpPatch(CMDRECV_LoadGameWrapper2,	injectCMDRECV_LoadGame_wrapper_2,	0);
		jmpPatch(CMDRECV_LoadGameWrapper3,	injectCMDRECV_LoadGame_wrapper_3,	0);
		jmpPatch(CMDRECV_LoadGameWrapper4,	injectCMDRECV_LoadGame_wrapper_4,	0);
		jmpPatch(CMDRECV_SaveGameWrapper,	injectCMDRECV_SaveGame_wrapper,		0);
	}

} //hooks
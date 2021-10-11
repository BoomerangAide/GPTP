#include "detector.h"
#include "../hook_tools.h"

namespace {

//Inject with jmpPatch()
void __declspec(naked) unitCanDetectWrapper() {

	static CUnit* unit;
	static u32 result;

	__asm {
		MOV unit, EAX
		PUSHAD
	}

	result = hooks::unitCanDetectHook(unit) ? 1 : 0;

	__asm {
		POPAD
		MOV EAX, result
		RETN
	}

}

;

void __declspec(naked) getCloakedTargetVisibilityWrapper() {

	static DetectorCheckParam* param;
	static CUnit* detector;

	__asm {
		MOV detector, ECX
		MOV param, EDX
		PUSHAD
	}

	hooks::getCloakedTargetVisibility(detector,param);

	__asm {
		POPAD
		XOR EAX, EAX
		RETN
	}

	//changes of param are not moved in something in
	//this asm bloc because it was a pointer directly
	//on Starcraft data from the start

}

;

} //unnamed namespace

namespace hooks {

void injectDetectorHooks() {
  jmpPatch(unitCanDetectWrapper,				0x00403430, 7);
  jmpPatch(getCloakedTargetVisibilityWrapper,	0x004408A0, 9);
}

} //hooks

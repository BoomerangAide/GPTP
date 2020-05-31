#include "wireframe.h"
#include <hook_tools.h>

namespace {

void __declspec(naked) statdata_UnitWireframeUpdate_FragmentWrapper() {

	static const u32 JUMPBACK_POINT_WIREFRAME = 0x00456955;

	static CUnit* unit;
	static u16 unitId;

	__asm {
		MOV unit, ESI
		PUSHAD
	}

	unitId = hooks::statdata_UnitWireframeUpdate_Fragment(unit);

	__asm {
		POPAD
		MOVZX ECX, unitId
		JMP JUMPBACK_POINT_WIREFRAME
	}

}

}; //unnamed namespace

namespace hooks {

void injectWireframeHook() {
	jmpPatch(statdata_UnitWireframeUpdate_FragmentWrapper, 0x00456911, 2);
}

;

} //hooks

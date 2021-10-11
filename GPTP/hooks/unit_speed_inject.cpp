#include "unit_speed.h"
#include "../hook_tools.h"

namespace {

void __declspec(naked) getModifiedUnitSpeedWrapper() {

	static CUnit* unit;
	static u32 speed;

	__asm {
		MOV unit, EDX
		MOV speed, EAX
		PUSHAD
	}

	speed = hooks::getModifiedUnitSpeedHook(unit, speed);

	__asm {
		POPAD
		MOV EAX, speed
		RETN
	}

}

;

void __declspec(naked) getModifiedUnitTurnSpeedWrapper() {

	static CUnit* unit;
	static u32 turnSpeed;

	__asm {
		MOV unit, ECX
		PUSHAD
	}

	turnSpeed = hooks::getModifiedUnitTurnSpeedHook(unit);

	__asm {
		POPAD
		MOV EAX, turnSpeed
		RETN
	}
}

;

void __declspec(naked) getModifiedUnitAccelerationWrapper() {

	static CUnit* unit;
	static u32 acceleration;

	__asm {
		MOV unit, ECX
		PUSHAD
	}

	acceleration = hooks::getModifiedUnitAccelerationHook(unit);

	__asm {
		POPAD
		MOV EAX, acceleration
		RETN
	}

}

;

} //unnamed namespace

namespace hooks {

void injectUnitSpeedHooks() {
	jmpPatch(getModifiedUnitSpeedWrapper,         0x0047B5F0, 2);
	jmpPatch(getModifiedUnitTurnSpeedWrapper,     0x0047B850, 6);
	jmpPatch(getModifiedUnitAccelerationWrapper,  0x0047B8A0, 6);
}

} //hooks

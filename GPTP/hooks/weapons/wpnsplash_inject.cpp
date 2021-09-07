//Injector source file for theeapon Splash hooks module.
#include "wpnsplash.h"
#include <hook_tools.h>

namespace {

void __declspec(naked) function0048ADB0_Wrapper() {

	static CBullet* bullet;
	static CUnit* unit;
	static u32 splashRadius;
	static Bool32 return_value;

	__asm {

		PUSH EBP
		MOV EBP, ESP

		MOV EAX, [EBP+0x0C]
		MOV splashRadius, EAX

		MOV EAX, [EBP+0x08]
		MOV bullet, EAX

		MOV unit, ECX

		PUSHAD

	}

	return_value = hooks::function_0048ADB0(bullet, splashRadius, unit);

	__asm {
		POPAD
		MOV EAX, return_value
		POP EBP
		RETN 0x8
	}

}

;

void __declspec(naked) function0048B570_Wrapper() {

	static CBullet* bullet;
	static CUnit* unit;
	static Bool32 return_value;

	__asm {

		PUSH EBP
		MOV EBP, ESP

		MOV unit, ESI
		MOV bullet, EDI

		PUSHAD

	}

	return_value = hooks::function_0048B570(bullet, unit);

	__asm {
		POPAD
		MOV EAX, return_value
		MOV ESP, EBP
		POP EBP
		RETN
	}

}

;

void __declspec(naked) AirSplashProcWrapper() {

	static CBullet* bullet;
	static CUnit* unit;
	static Bool32 return_value;

	__asm {
		MOV unit, ECX
		MOV bullet, EDX
		PUSHAD
	}

	return_value = hooks::AirSplashProc(bullet, unit);

	__asm {
		POPAD
		MOV EAX, return_value
		RETN
	}

}

;

void __declspec(naked) LurkerSplashProcWrapper() {

	static CBullet* bullet;
	static CUnit* unit;
	static Bool32 return_value;

	__asm {
		MOV unit, ECX
		MOV bullet, EDX
		PUSHAD
	}

	return_value = hooks::LurkerSplashProc(bullet, unit);

	__asm {
		POPAD
		MOV EAX, return_value
		RETN
	}

}

;

void __declspec(naked) AirSplashProc2Wrapper() {

	static CBullet* bullet;
	static CUnit* unit;
	static Bool32 return_value;

	__asm {
		MOV unit, ECX
		MOV bullet, EDX
		PUSHAD
	}

	return_value = hooks::AirSplashProc2(bullet, unit);

	__asm {
		POPAD
		MOV EAX, return_value
		RETN
	}

}

;

void __declspec(naked) SplashProcWrapper() {

	static CBullet* bullet;
	static CUnit* unit;
	static Bool32 return_value;

	__asm {
		MOV unit, ECX
		MOV bullet, EDX
		PUSHAD
	}

	return_value = hooks::SplashProc(bullet, unit);

	__asm {
		POPAD
		MOV EAX, return_value
		RETN
	}

}

;

}//unnamed namespace

namespace hooks {

	void injectWpnSplashHooks() {
		jmpPatch(function0048ADB0_Wrapper,	0x0048ADB0, 1);
		jmpPatch(AirSplashProcWrapper,		0x0048B150, 0);
		jmpPatch(function0048B570_Wrapper,	0x0048B570, 2);
		jmpPatch(LurkerSplashProcWrapper,	0x0048B680, 0);
		jmpPatch(AirSplashProc2Wrapper,		0x0048B730, 1);
		jmpPatch(SplashProcWrapper,			0x0048B750, 1);
	}

} //hooks

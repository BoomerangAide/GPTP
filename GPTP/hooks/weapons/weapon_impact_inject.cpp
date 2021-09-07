//Injector source file for the Weapon Impact hook module.
#include "weapon_impact.h"
#include <hook_tools.h>

namespace {

void __declspec(naked) WeaponImpactWrapper() {

	static CBullet* bullet;

	__asm {

		PUSH EBP
		MOV EBP, ESP

		MOV EAX, [EBP+0x08]
		MOV bullet, EAX

		PUSHAD

	}

	hooks::weaponImpact(bullet);

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

void injectWeaponImpactHook() {
	jmpPatch(WeaponImpactWrapper, 0x0048B770, 1);
}

} //hooks

#include "weapon_damage.h"
#include "../../hook_tools.h"

namespace {

void __declspec(naked) killTargetUnitCheck_Wrapper() {

	static CUnit* target;
	static CUnit* attacker;
	static s32 damage;
	static u32 attackingPlayerId;
	static Bool32 bNotify;

	__asm {
		PUSH EBP
		MOV EBP, ESP
		MOV damage, EAX
		MOV target, ECX
		MOV EAX, [EBP+0x08]
		MOV attacker, EAX
		MOV EAX, [EBP+0x0C]
		MOV attackingPlayerId, EAX
		MOV EAX, [EBP+0x10]
		MOV bNotify, EAX
		PUSHAD
	}

	hooks::killTargetUnitCheck(target, damage, attacker, attackingPlayerId, bNotify);

	__asm {
		POPAD
		POP EBP
		RETN 0xC
	}
}

;

void __declspec(naked) weaponDamageWrapper() {

	static CUnit* target;
	static CUnit* attacker;
	static s32 damage;
	static s32 direction;
	static u32 weaponId;
	static u32 damageDivisor;
	static u32 attackingPlayerId;

	__asm {
		PUSH EBP
		MOV EBP, ESP
		MOV damage, EAX
		MOV target, EDI
		MOV EAX, [EBP+0x08]
		MOV weaponId, EAX
		MOV EAX, [EBP+0x0C]
		MOV damageDivisor, EAX
		MOV EAX, [EBP+0x10]
		MOV direction, EAX
		MOV EAX, [EBP+0x14]
		MOV attacker, EAX
		MOV EAX, [EBP+0x18]
		MOV attackingPlayerId, EAX
		PUSHAD
	}

	hooks::weaponDamageHook(damage, target, weaponId, attacker, attackingPlayerId,
							direction, damageDivisor);

	__asm {
		POPAD
		POP EBP
		RETN 0x14
	}
}

;

} //unnamed namespace

namespace hooks {

void injectWeaponDamageHook() {
	jmpPatch(killTargetUnitCheck_Wrapper,	0x004797B0, 0);
	jmpPatch(weaponDamageWrapper,			0x00479930, 1);
}

} //hooks

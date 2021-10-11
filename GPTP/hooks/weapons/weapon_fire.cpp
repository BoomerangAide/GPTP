//Source file for the Weapon Fire hook module.
//This hook controls how weapons are fired.
#include "weapon_fire.h"
#include <SCBW/api.h>


//-------- Helper function declarations. Do NOT modify! ---------//

namespace {
void getWeaponHitPos(CUnit* unit, int* x, int* y);													//762C0
void createBullet(CUnit* source, u32 weaponId, int x, int y, u32 attackingPlayer, u32 direction);	//8C260
} //unnamed namespace


//-------- Actual hook functions --------//

namespace hooks {

//Fires a weapon from the @p unit.
//This hook affects the following iscript opcodes: attackwith, attack, castspell
//This also affects CUnit::fireWeapon().
void fireWeaponHook(CUnit* unit, u8 weaponId) {

	int x = 0, y = 0;
	bool bStopThere = false;

	if (weapons_dat::Behavior[weaponId] == WeaponBehavior::AppearOnTargetUnit) {

		if (unit->orderTarget.unit == NULL)
			bStopThere = true;
		else
			getWeaponHitPos(unit, &x, &y);

	}
	else 
	if (weapons_dat::Behavior[weaponId] == WeaponBehavior::AppearOnTargetSite) {
		x = unit->orderTarget.pt.x;
		y = unit->orderTarget.pt.y;
	}
	else {

		int forwardOffset = weapons_dat::ForwardOffset[weaponId];

		x = unit->sprite->position.x + (angleDistance[unit->currentDirection1].x * forwardOffset) / 256;
		y = unit->sprite->position.y + (angleDistance[unit->currentDirection1].y * forwardOffset) / 256 - weapons_dat::VerticalOffset[weaponId];

		//x = unit->getX() + scbw::getPolarX(forwardOffset, unit->currentDirection1);
		//y = unit->getY() + scbw::getPolarY(forwardOffset, unit->currentDirection1) - weapons_dat::VerticalOffset[weaponId];

	}

	if (!bStopThere && weapons_dat::FlingyId[weaponId] != 0)
		createBullet(unit, weaponId, x, y, unit->playerId, unit->currentDirection1);
}

;

} //hooks


//-------- Helper function definitions. Do NOT modify! --------//

namespace {

const u32 Func_getWeaponHitPos = 0x004762C0;
//Set x and y to values rather than having a return value
void getWeaponHitPos(CUnit* unit, int* x, int* y) {
	__asm {
		PUSHAD
		PUSH y
		PUSH x
		PUSH unit
		CALL Func_getWeaponHitPos
		POPAD
	}
}

;

const u32 Func_CreateBullet = 0x0048C260;
void createBullet(CUnit* source, u32 weaponId, int x, int y, u32 attackingPlayer, u32 direction) {

	__asm {
		PUSHAD
		PUSH direction
		PUSH attackingPlayer
		PUSH y
		PUSH x
		MOV EAX, source
		MOVZX ECX, weaponId
		CALL Func_CreateBullet
		POPAD
	}

}

;

} //unnamed namespace

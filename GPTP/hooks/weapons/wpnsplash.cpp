//The source file for the Weapon Splash hooks module.
#include "wpnsplash.h"
#include <SCBW/api.h>

//Helper functions definitions

namespace {

void WeaponBulletHit_Helper(CBullet* bullet, CUnit* target, u32 damageDivisor);     //79AE0
Bool32 function_0048ADB0_Helper(CBullet* bullet, u32 splashRadius, CUnit* unit);	//8ADB0
Bool32 function_0048B570_Helper(CBullet* bullet, CUnit* unit);						//8B570

} //unnamed namespace

struct UnitPair {
	CUnit* unit1;
	CUnit* unit2;
};

namespace hooks {

//Note: all those functions are called by FindBestUnit, or another
//of those featured functions that must report to FindBestUnit. 
//FindBestUnit make a list of units, then get each of them to go through
//a function like those, which is why there's some return value.

//All functions use this one, directly or through another
//Left out some potential invalid values fixes through weird scaling, that
//may also have been just weird and useless conversions.
Bool32 function_0048ADB0(CBullet* bullet, u32 splashRadius, CUnit* unit) {

	int xMin, yMin, xMax, yMax;
	int finalX, finalY;

	xMin = unit->sprite->position.x - units_dat::UnitBounds[unit->id].left;
	xMax = unit->sprite->position.x + units_dat::UnitBounds[unit->id].right;
	yMin = unit->sprite->position.y - units_dat::UnitBounds[unit->id].top;
	yMax = unit->sprite->position.y + units_dat::UnitBounds[unit->id].bottom;

	finalX = bullet->sprite->position.x - (xMax + 1);

	if (finalX < 0) {

		finalX = xMin - bullet->sprite->position.x;

		if (finalX < 0)
			finalX = 0;

	}

	finalY = bullet->sprite->position.y - (yMax + 1);

	if (finalY < 0) {

		finalY = yMin - bullet->sprite->position.y;

		if (finalY < 0)
			finalY = 0;

	}

	if (scbw::getDistanceFast(0, 0, finalX, finalY) <= splashRadius)
		return 1;
	else
		return 0;

}

;

Bool32 AirSplashProc(CBullet* bullet, CUnit* unit) {

	static CUnit** units_in_range = (CUnit**)0x0064EEE0;
	static u32* u32_0064DEB4 = (u32*)0x0064DEB4;

	if (
		unit != bullet->sourceUnit &&
		(unit->playerId != bullet->srcPlayerId || unit == bullet->attackTarget.unit) &&
		scbw::canWeaponTargetUnit(bullet->weaponType, unit, unit)
	)
	{

		if (function_0048ADB0_Helper(bullet, weapons_dat::OuterSplashRadius[bullet->weaponType], unit) != 0) {

			if (
				(function_0048ADB0_Helper(bullet, weapons_dat::InnerSplashRadius[bullet->weaponType], unit) != 0) &&
				unit == bullet->attackTarget.unit
			)
			{
				units_in_range[0] = unit;
				*u32_0064DEB4 = 1;
				return 1;
			}
			else
			{
				units_in_range[*u32_0064DEB4] = unit;
				*u32_0064DEB4 = *u32_0064DEB4 + 1;
				return 0;
			}

		}
		else
			return 0;

	}
	else
		return 0;

}

;

//Function used by the 2 "small ones" (last 2)
Bool32 function_0048B570(CBullet* bullet, CUnit* unit) {

	static u32* u32_0064DEB8 = (u32*)0x0064DEB8;

	if (unit == bullet->sourceUnit && bullet->weaponType != WeaponId::PsiStorm)
		return 0;
	else
	{

		u8 explosionType = weapons_dat::ExplosionType[bullet->weaponType];

		if (
			(explosionType == WeaponEffect::SplashEnemy || explosionType == WeaponEffect::SplashAir) &&
			unit->playerId == bullet->srcPlayerId &&
			unit != bullet->attackTarget.unit
		)
			return 0;
		else
		if(!scbw::canWeaponTargetUnit(bullet->weaponType,unit,unit))
			return 0;
		else
		{

			u32 splashRadius = weapons_dat::OuterSplashRadius[bullet->weaponType];

			if (function_0048ADB0_Helper(bullet, splashRadius, unit) == 0)
				return 0;
			else
			{
				
				if (bullet->weaponType == WeaponId::PsiStorm) {
					if (unit->isUnderStorm != 0)
						return 0;
					else
						unit->isUnderStorm = 1;
				}

				if (*u32_0064DEB8 != 0) { //may mean not anti-air splash

					splashRadius = weapons_dat::InnerSplashRadius[bullet->weaponType];

					if (function_0048ADB0_Helper(bullet, splashRadius, unit) == 0) {
						
						if (unit->status & UnitStatus::Burrowed)
							return 0;
						else
						{

							splashRadius = weapons_dat::MediumSplashRadius[bullet->weaponType];

							if (function_0048ADB0_Helper(bullet, splashRadius, unit) == 0) {
								WeaponBulletHit_Helper(bullet, unit, 4);
								return 0;
							}
							else
							{
								WeaponBulletHit_Helper(bullet, unit, 2);
								return 0;
							}

						}

					}
					else {
						WeaponBulletHit_Helper(bullet, unit, 1);
						return 0;
					}

				}
				else					//may mean anti-air splash
				{

					if (
						unit->status & UnitStatus::Burrowed ||
						unit == bullet->attackTarget.unit ||
						unit->id == UnitId::ProtossInterceptor
					)
						return 0;
					else
					{

						splashRadius = weapons_dat::MediumSplashRadius[bullet->weaponType];

						if (function_0048ADB0_Helper(bullet, splashRadius, unit) == 0) {
							WeaponBulletHit_Helper(bullet, unit, 4);
							return 0;
						}
						else
						{
							WeaponBulletHit_Helper(bullet, unit, 2);
							return 0;
						}

					}

				}

			}

		}

	}

}

;

Bool32 LurkerSplashProc(CBullet* bullet, CUnit* unit) {

	static UnitPair* units_pairs_array_0064DEC8 = (UnitPair*) 0x0064DEC8;
	static u32*	u32_0064DEA8 = (u32*)0x0064DEA8;
	static u32*	u32_0064EEC8 = (u32*)0x0064EEC8;

	CUnit* attacker = bullet->sourceUnit;

	if (
		unit != attacker &&
		(unit->playerId != bullet->srcPlayerId || unit == bullet->attackTarget.unit) &&
		scbw::canWeaponTargetUnit(bullet->weaponType, unit, unit)
	)
	{

		if (function_0048ADB0_Helper(bullet, weapons_dat::InnerSplashRadius[bullet->weaponType], unit) != 0) {

			int index_in_units_pairs = 0; //different way from original

			if (attacker == NULL) {
				WeaponBulletHit_Helper(bullet, unit, 1);
				return 0;
			}

			while (
				index_in_units_pairs < 512 &&
				(
					units_pairs_array_0064DEC8[index_in_units_pairs].unit1 != attacker ||
					units_pairs_array_0064DEC8[index_in_units_pairs].unit2 != unit
				)
			)
			{
				index_in_units_pairs++;
			}

			if (index_in_units_pairs < 512)
				return 0;
			else
			{

				if (*u32_0064DEA8 != 16) {
					units_pairs_array_0064DEC8[*u32_0064EEC8 * 16 + *u32_0064DEA8].unit1 = attacker;
					units_pairs_array_0064DEC8[*u32_0064EEC8 * 16 + *u32_0064DEA8].unit2 = unit;	//equivalent to using 0064DECC
					*u32_0064DEA8 = *u32_0064DEA8 + 1;
				}

				WeaponBulletHit_Helper(bullet, unit, 1);
				return 0;

			}

		}
		else
			return 0;

	}
	else
		return 0;

}

;

Bool32 AirSplashProc2(CBullet* bullet, CUnit* unit) {

	static u32* u32_0064DEB8 = (u32*)0x0064DEB8;

	*u32_0064DEB8 = 0;	//maybe say it's air

	return function_0048B570_Helper(bullet, unit);

}

;

Bool32 SplashProc(CBullet* bullet, CUnit* unit) {

	static u32* u32_0064DEB8 = (u32*)0x0064DEB8;

	*u32_0064DEB8 = 1;	//maybe say it's ground

	return function_0048B570_Helper(bullet, unit);

}

;

} //hooks

//-------- Helper function definitions. Do NOT modify! --------//

namespace {

const u32 Func_WeaponBulletHit = 0x00479AE0;
void WeaponBulletHit_Helper(CBullet* bullet, CUnit* target, u32 damageDivisor) {
	__asm {
		PUSHAD
		PUSH damageDivisor
		MOV EDX, bullet
		MOV EAX, target
		CALL Func_WeaponBulletHit
		POPAD
	}
}

;

const u32 Func_0048ADB0 = 0x0048ADB0;
Bool32 function_0048ADB0_Helper(CBullet* bullet, u32 splashRadius, CUnit* unit) {

	static Bool32 result;

	__asm {
		PUSHAD
		PUSH splashRadius
		PUSH bullet
		MOV ECX, unit
		CALL Func_0048ADB0
		MOV result, EAX
		POPAD
	}

	return result;

}

;

const u32 Func_0048B570 = 0x0048B570;
Bool32 function_0048B570_Helper(CBullet* bullet, CUnit* unit) {

	static Bool32 result;

	__asm {
		PUSHAD
		MOV EDI, bullet
		MOV ESI, unit
		CALL Func_0048B570
		MOV result, EAX
		POPAD
	}

	return result;

}

;

} //Unnamed namespace

//End of helper functions

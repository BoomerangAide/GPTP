#include "attack_and_cooldown.h"
#include <SCBW/api.h>

//helper functions def

namespace {

u32 function_00430D30(CUnit* unit, CUnit* target);		//30D30
u32 getUpgradedWpnCooldown(CUnit* unit, u8 weaponId);	//75DC0
void function_00477510(CUnit* unit);					//77510
void applyBunkerWeaponAnimationHelper(CUnit* unit);		//77FD0
u32 RandomizeShort(u32 unknown_index);					//DC4A0
bool isUnitVisible(CUnit* unit);						//E5DB0
	
} //unnamed namespace

namespace hooks {

Bool32 attackApplyCooldown(CUnit* unit) {

	CUnit* target = unit->orderTarget.unit;
	Bool32 return_value;

	if (target == NULL)
		return_value = 0;
	else {

		function_00477510(unit);

		if (
			!isUnitVisible(unit) || //more like "can unit see its target (possibly include target location if no unit target)"
			!unit->canAttackTarget(target, true)
		)
			return_value = 0;
		else
		{

			unit->orderTarget.pt.x = target->sprite->position.x;
			unit->orderTarget.pt.y = target->sprite->position.y;

			if (unit->subunit != NULL && units_dat::BaseProperty[unit->subunit->id] & UnitProperty::Subunit)
				return_value = 1;
			else
			{

				u8 weaponId;
				u8 cooldown;
				bool targetInAir = target->status & UnitStatus::InAir;

				if (targetInAir) {
					weaponId = units_dat::AirWeapon[unit->id];
					cooldown = unit->airWeaponCooldown;
				}
				else {
					weaponId = unit->getGroundWeapon();
					cooldown = unit->groundWeaponCooldown;
				}

				if (cooldown != 0) {

					if (unit->orderQueueTimer > cooldown - 1)
						unit->orderQueueTimer = cooldown - 1;

					return_value = 1;

				}
				else
				if (unit->movementFlags & MovementFlags::StartingAttack)
					return_value = 1;
				else
				{

					u32 distance = function_00430D30(unit, target);
					u32 range_check = weapons_dat::MinRange[weaponId];

					if (
						(range_check != 0 && distance < range_check) ||
						distance > unit->getMaxWeaponRange(weaponId)
					)
						return_value = 0;
					else
					{

						s32 angle = scbw::getAngle(
							unit->sprite->position.x,
							unit->sprite->position.y,
							unit->orderTarget.pt.x,
							unit->orderTarget.pt.y
						);

						angle = unit->currentDirection1 - angle;

						if (angle < 0)
							angle = angle + 256;

						if (angle > 128)
							angle = 256 - angle;

						if (angle > weapons_dat::AttackAngle[weaponId]) {
							if (unit->status & UnitStatus::CanTurnAroundToAttack) {
								unit->orderQueueTimer = 0;
								return_value = 1;
							}
							else
								return_value = 0;
						}
						else
						if (unit->status & UnitStatus::CanNotAttack)
							return_value = 0;
						else
						{

							static u32* u32_0064DEB0 = (u32*)0x0064DEB0;

							s8 random_value;
							IscriptAnimation::Enum iscript_anim;
							CImage* current_image;

							if (unit->status & UnitStatus::InBuilding)
								applyBunkerWeaponAnimationHelper(unit);

							unit->movementFlags |= MovementFlags::StartingAttack;

							*u32_0064DEB0 = 0;

							cooldown = getUpgradedWpnCooldown(unit, weaponId);

							random_value = (u8)RandomizeShort(13);
							random_value = (random_value & 3) - 1; //value between -1 and 2

							unit->groundWeaponCooldown = cooldown + random_value;
							unit->airWeaponCooldown = cooldown + random_value;

							current_image = unit->sprite->images.head;

							if (targetInAir)
								iscript_anim = IscriptAnimation::AirAttkRpt;
							else
								iscript_anim = IscriptAnimation::GndAttkRpt;

							while (current_image != NULL) {
								current_image->playIscriptAnim(iscript_anim);
								current_image = current_image->link.next;
							}

							return_value = 1;

						}

					}

				}

			}

		}

	}

	return return_value;

}

;

} //namespace hooks

;

//-------- Helper function definitions. Do NOT modify! --------//

namespace {

const u32 Func_00430D30 = 0x00430D30;
u32 function_00430D30(CUnit* unit, CUnit* target) {

	static u32 distance;

	__asm {
		PUSHAD
		MOV ECX, target
		MOV EAX, unit
		CALL Func_00430D30
		MOV distance, EAX
		POPAD
	}

	return distance;

}

;

const u32 Func_getUpgradedWpnCooldown = 0x00475DC0;
u32 getUpgradedWpnCooldown(CUnit* unit, u8 weaponId) {

	static u32 return_value;

	__asm {
		PUSHAD
		MOV AL, weaponId
		MOV ESI, unit
		CALL Func_getUpgradedWpnCooldown
		MOV return_value, EAX
		POPAD
	}

	return return_value;

}

;

const u32 Func_Sub477510 = 0x00477510;
void function_00477510(CUnit* unit) {

	__asm {
		PUSHAD
		PUSH unit
		CALL Func_Sub477510
		POPAD
	}

}

;

const u32 Func_applyBunkerWeaponAnimation = 0x00477FD0;
void applyBunkerWeaponAnimationHelper(CUnit* unit) {
	__asm {
		PUSHAD
		MOV EAX, unit
		CALL Func_applyBunkerWeaponAnimation
		POPAD
	}
}

;

const u32 Func_RandomizeShort = 0x004DC4A0;
u32 RandomizeShort(u32 unknown_index) {

	static u32 return_value;

	__asm {
		PUSHAD
		MOV EAX, unknown_index
		CALL Func_RandomizeShort
		MOV return_value, EAX
		POPAD
	}

	return return_value;

}

;

const u32 Func_isUnitVisible = 0x004E5DB0;
bool isUnitVisible(CUnit* unit) {

	static Bool32 bPreResult;

	__asm {
		PUSHAD
		MOV ESI, unit
		CALL Func_isUnitVisible
		MOV bPreResult ,EAX
		POPAD
	}

	return (bPreResult != 0);

}

;

} //Unnamed namespace

//End of helper functions

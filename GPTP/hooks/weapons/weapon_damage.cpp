#include "weapon_damage.h"
#include "../../SCBW/api.h"

namespace {

u16 getUnitStrength(CUnit* unit, bool useGroundStrength);				//31800
void incrementUnitKillCount(CUnit* unit);								//759C0
void function_004795D0(CUnit* target, CUnit* attacker, Bool32 bNotify);	//795D0
void incrementUnitDeathScores(CUnit* unit, u8 player);					//88AF0
void doAttackNotifyEvent(CUnit* unit);									//8F230
void setAllImageGroupFlagsPal11(CSprite* sprite);						//97430
void updateUnitDamageOverlay(CUnit* unit);								//E6090
void createShieldOverlay(CUnit* unit, u32 attackDirection);				//E6140
bool unitIsActiveTransport(CUnit* unit);								//E6BA0
CUnit* getLoadedUnitFromIndex(CUnit* transport, int index);				//E6C40

} //unnamed namespace

/// Definition of damage factors (explosive, concussive, etc.)
struct {
	s32 damageType;
	s32 unitSizeFactor[4];	 //Order: {independent, small, medium, large}
}

//Equivalent to array at 0x00515B88
const damageFactor[5] = {
	{0, 0, 0, 0, 0},		//Independent
	{1, 0, 128, 192, 256},	//Explosive
	{2, 0, 256, 128, 64},	//Concussive
	{3, 0, 256, 256, 256},	//Normal
	{4, 0, 256, 256, 256}	//IgnoreArmor
};

namespace hooks {

// Hooks into the CUnit::damageHp() function.
// 004797B0  killTargetUnitCheck 
void killTargetUnitCheck(CUnit* target, s32 damage, CUnit* attacker, u32 attackingPlayerId, Bool32 bNotify) {

	if (target->hitPoints != 0) {
		
		if (target->sprite->flags & CSprite_Flags::Selected)
			setAllImageGroupFlagsPal11(target->sprite);

		if (attacker != NULL)
			function_004795D0(target, attacker, bNotify);

		if (*CHEAT_STATE & CheatFlags::PowerOverwhelming && playerTable[attackingPlayerId].type != PlayerType::Human)
			damage = 0;

		if (damage >= target->hitPoints) { //killing target

			if (unitIsActiveTransport(target)) {

				CUnit* loaded_unit;

				for (int i = 0; i < 8; i++) {

					loaded_unit = getLoadedUnitFromIndex(target, i);

					if (
						loaded_unit != NULL && 
						loaded_unit->sprite != NULL &&
						loaded_unit->mainOrderId != OrderId::Die
					)
					{
						loaded_unit->remove();
						incrementUnitDeathScores(loaded_unit, attackingPlayerId);
					}

				}

			}

			target->hitPoints = 0;
			target->remove();
			incrementUnitDeathScores(target, attackingPlayerId);

			if (attacker != NULL) {

				u8 targetPlayerId;

				if (target->playerId != 11)
					targetPlayerId = target->playerId;
				else
					targetPlayerId = target->sprite->playerId;

				if (!scbw::isAlliedTo(attacker->playerId, targetPlayerId))
					incrementUnitKillCount(attacker);

			}

		}
		else //target surviving
		{

			target->hitPoints -= damage;
			target->airStrength = getUnitStrength(target, false);
			target->groundStrength = getUnitStrength(target, true);

			if (
				target->status & UnitStatus::Completed &&
				images_dat::DamageOverlay[target->sprite->mainGraphic->id] != 0 //not sure if 0 or NULL contextually
			)
				updateUnitDamageOverlay(target);

			if (attacker != NULL && target->playerId != attacker->playerId && bNotify)
				doAttackNotifyEvent(target);

		}

	}

}

;

// Hooks into the CUnit::damageWith() function.
// 00479930 DoWpnDamage
void weaponDamageHook(	s32		damage,
						CUnit*	target,
						u8		weaponId,
						CUnit*	attacker,
						u8		attackingPlayerId,
						s8		direction,
						u8		dmgDivisor
						) 
{

	//the unit must neither be already dead nor invincible
	if (target->hitPoints != 0 && !(target->status & UnitStatus::Invincible)) {

		u8 damageType;
		s32 shieldReduceAmount = 0;
		u32 damageFactorResult;

		if (scbw::isCheatEnabled(CheatFlags::PowerOverwhelming)	&&			//If Power Overwhelming is enabled
			playerTable[attackingPlayerId].type != PlayerType::Human)		//and the attacker is not a human player
			damage = 0;

		if (target->status & UnitStatus::IsHallucination)
			damage *= 2;

		damage = damage / dmgDivisor + (target->acidSporeCount * 256);

		if (damage < 128)
			damage = 128;

		//Reduce Defensive Matrix
		if (target->defensiveMatrixHp != 0) {

			s32 d_matrix_reduceAmount;

			if(damage <= target->defensiveMatrixHp)
				d_matrix_reduceAmount = damage;						//partial damage to matrix shield
			else
				d_matrix_reduceAmount = target->defensiveMatrixHp;	//destroy all hp of matrix shield

			damage -= d_matrix_reduceAmount;
			target->reduceDefensiveMatrixHp(d_matrix_reduceAmount);

		}

		damageType = weapons_dat::DamageType[weaponId];

		if (units_dat::ShieldsEnabled[target->id] && target->shields >= 256) {

			if (damageType != DamageType::IgnoreArmor) {

				s32 shieldArmor = scbw::getUpgradeLevel(target->playerId, UpgradeId::ProtossPlasmaShields) * 256;

				if (shieldArmor >= damage)
					damage = 128;
				else
					damage -= shieldArmor;

			}

			//799F7
			if (damage <= target->shields)
				shieldReduceAmount = damage;
			else
				shieldReduceAmount = target->shields;

			damage -= shieldReduceAmount;

			/*Shield not reduced yet because shield don't get protected by non-shield armor calculation below*/

		}

		//79A01
		if (damageType != DamageType::IgnoreArmor) {

			s32 armorTotal = target->getArmor() * 256;

			if (damage <= armorTotal)
				damage = 0;
			else
				damage -= armorTotal;

		}

		//79A38
		damageFactorResult = damageFactor[damageType].unitSizeFactor[units_dat::SizeType[target->id]];
		damage = (damageFactorResult * damage) / 256; //256 being the max damage factor, "damageFactorResult/256" is the multiplier

		if (shieldReduceAmount == 0 && damage < 128)
			damage = 128;

		//hooked killTargetUnitCheck, damage is reduced to 0 in here when Power Overwhelming is ON
		//Shields still take impact as seen in this function though
		target->damageHp(damage, attacker, attackingPlayerId, (weaponId != WeaponId::Irradiate));

		if (shieldReduceAmount != 0) {

			target->shields -= shieldReduceAmount;

			if (damageType != DamageType::Independent && target->shields != 0)
				createShieldOverlay(target, direction);

		}

		//Update unit strength data (?)
		target->airStrength = getUnitStrength(target, false);
		target->groundStrength = getUnitStrength(target, true);

	}

}

;

} //hooks

namespace {

/**** Definitions of helper functions. Do NOT modify anything below! ****/

//Somehow related to AI stuff; details unknown.
const u32 Func_GetUnitStrength = 0x00431800;
u16 getUnitStrength(CUnit* unit, bool useGroundStrength) {

	static u16 strength;
	static Bool32 useGroundStrength_;

	useGroundStrength_ = useGroundStrength ? 1 : 0;

	__asm {
		PUSHAD
		PUSH useGroundStrength_
		MOV EAX, unit
		CALL Func_GetUnitStrength
		MOV strength, AX
		POPAD
	}

	return strength;

}

;

//identical to incrementUnitKillCount @ 0x004759C0
void incrementUnitKillCount(CUnit* unit) {

	CUnit* temp_unit = unit;

	if(temp_unit->killCount < 255)
		temp_unit->killCount++;

	while(
		temp_unit->id == UnitId::ProtossInterceptor &&
		temp_unit->interceptor.parent != NULL
	) {

		temp_unit = temp_unit->interceptor.parent;

		if(temp_unit->killCount < 255)
			temp_unit->killCount++;

	}
		
}

;

const u32 Func_004795D0 = 0x004795D0;
void function_004795D0(CUnit* target, CUnit* attacker, Bool32 bNotify) {
	__asm {
		PUSHAD
		MOV EBX, target
		PUSH bNotify
		PUSH attacker
		CALL Func_004795D0
		POPAD
	}
}

;

const u32 Func_IncrementUnitDeathScores = 0x00488AF0;
void incrementUnitDeathScores(CUnit* unit, u8 player) {

	__asm {
		PUSHAD
		MOV EDI, unit
		MOVZX EDX, player
		CALL Func_IncrementUnitDeathScores
		POPAD
	}

}

;

const u32 Func_doAttackNotifyEvent = 0x0048F230;
void doAttackNotifyEvent(CUnit* unit) {
	__asm {
		PUSHAD
		PUSH unit
		CALL Func_doAttackNotifyEvent
		POPAD
	}
}

;

//Identical to setAllImageGroupFlagsPal11 @ 0x00497430;
void setAllImageGroupFlagsPal11(CSprite* sprite) {

	for(
		CImage* current_image = sprite->images.head; 
		current_image != NULL;
		current_image = current_image->link.next
	)
	{
		if(current_image->paletteType == PaletteType::RLE_HPFLOATDRAW)
			current_image->flags |= CImage_Flags::Redraw;
	}

}

;

const u32 Func_UpdateUnitDamageOverlay = 0x004E6090;
void updateUnitDamageOverlay(CUnit* unit) {
	__asm {
		PUSHAD
		MOV ESI, unit
		CALL Func_UpdateUnitDamageOverlay
		POPAD
	}
}

;

//Creates the Plasma Shield flickering effect.
//Identical to function @ 0x004E6140
void createShieldOverlay(CUnit* unit, u32 attackDirection) {

	const LO_Header* shield_lo = shieldOverlays[unit->sprite->mainGraphic->id];

	u32 frameAngle = (attackDirection - 124) / 8 % 32;

	Point8 offset = shield_lo->getOffset(unit->sprite->mainGraphic->direction, frameAngle);

	unit->sprite->createOverlay(ImageId::ShieldOverlay, offset.x, offset.y, frameAngle);

}

;

//Equivalent to code unitIsActiveTransport @ 0x004E6BA0
bool unitIsActiveTransport(CUnit* unit) {

	bool result = false;

	if(!(unit->status & UnitStatus::IsHallucination)) {

		if(
			unit->id != UnitId::ZergOverlord ||
			UpgradesSc->currentLevel[unit->playerId][UpgradeId::VentralSacs] != 0
		)
		{
			if(units_dat::SpaceProvided[unit->id] != 0)
				result = true;
		}


	}

	return result;

}

;

const u32 Func_getLoadedUnitFromIndex = 0x004E6C40;
CUnit* getLoadedUnitFromIndex(CUnit* transport, int index) {

	static CUnit* return_value;

	__asm {
		PUSHAD
		MOV ECX, index
		MOV EAX, transport
		CALL Func_getLoadedUnitFromIndex
		MOV return_value, EAX
		POPAD
	}

	return return_value;

}

;

} //unnamed namespace

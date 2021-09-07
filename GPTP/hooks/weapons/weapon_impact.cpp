//The source file for the Weapon Impact hook module.
#include "weapon_impact.h"
#include <SCBW/api.h>

//Helper functions definitions

namespace {

void FatalError_localversion();														//215D0
void function_0043F320(CUnit* attacker, CUnit* target, u32 unk1, u32 unk2);			//3F320
void IrradiateHit_Helper(CUnit* attacker, CUnit* target, u32 attackingPlayerId);	//54E00
void OpticalFlareHit_Helper(CUnit* target, u32 attackingPlayerId, CBullet* bullet);	//55170
void RestoreHit_Helper(CUnit* target, CBullet* bullet);								//55230
void LockdownHit_Helper(CUnit* target, u32 previousLockdownTimer, CBullet* bullet);	//55380
void function_004795D0(CUnit* target, CUnit* attacker, Bool32 bNotify);				//795D0
void WeaponBulletHit_Helper(CBullet* bullet, CUnit* target, u32 damageDivisor);		//79AE0
void EMPShockwaveHit_Helper(CUnit* attacker, int x, int y, CBullet* bullet);		//92BA0
u32 RandomizeShort(u32 unknown_index);												//DC4A0
CUnit* FindBestUnit(Box16* coords, u32 searchProc, CBullet* bullet);				//E8830
void CorrosiveAcidHit_Helper(u32 attackingPlayerId, int x, int y, CBullet* bullet);	//F4770
void ConsumeHit_Helper(CUnit* attacker, CUnit* target);								//F47B0
void EnsnareHit_Helper(CUnit* attacker, int x, int y, CBullet* bullet);				//F4830
void DarkSwarmHit_Helper(int x, int y, u32 attackingPlayerId, CBullet* bullet);		//F48A0
void BroodlingHit_Helper(CUnit* attacker, CUnit* target, CBullet* bullet);			//F4940
void PlagueHit_Helper(CUnit* attacker, int x, int y, CBullet* bullet);				//F4B30
void MaelstromHit_Helper(CUnit* attacker, int x, int y, CBullet* bullet);			//F6850
void StasisFieldHit_Helper(CUnit* attacker, int x, int y, CBullet* bullet);			//F6A80
void DisruptionWebHit_Helper(u32 attackingPlayerId, int x, int y, CBullet* bullet);	//F6AF0

} //unnamed namespace

namespace hooks {

//Was 0048B770  CBullet_Damage
//The function calling all wpnspellhit functions and managing
//stuff before these calls
//Also handle splash damage logic
void weaponImpact(CBullet* bullet) {

	u32 explosionType = weapons_dat::ExplosionType[bullet->weaponType];

	if (
		explosionType > WeaponEffect::SplashAir ||
		explosionType == WeaponEffect::MindControl ||
		explosionType == WeaponEffect::Feedback ||
		explosionType == WeaponEffect::Unknown_Crash
	)
		FatalError_localversion();
	else
	if (explosionType == WeaponEffect::NormalHit) {

		CUnit* target = bullet->attackTarget.unit;

		if (target != NULL) {

			if (bullet->hitFlags & 1)	//target miss
			{
				
				if (target->pAI != NULL && bullet->sourceUnit != NULL)
					function_004795D0(target, bullet->sourceUnit, 1);

			}
			else						//target hit
			{

				int damage_divisor = 1;

				//staying true to the original, easily crashing code
				if (weapons_dat::Behavior[bullet->weaponType] == WeaponBehavior::Bounce) {

					s8 pre_counter = 2 - bullet->remainingBounces;

					if (pre_counter != 0) {

						u32 counter = (u32)(u8)pre_counter;	//original code would be "MOVZX counter, pre_counter"

						do {
							damage_divisor *= 3;
							counter--;
						} while (counter != 0);

					}

				}

				WeaponBulletHit_Helper(bullet, target, damage_divisor);

			}

		}

	}
	else
	if (explosionType == WeaponEffect::YamatoGun) {
		if (bullet->attackTarget.unit != NULL)
			WeaponBulletHit_Helper(bullet, bullet->attackTarget.unit, 1);
	}
	else
	if (explosionType == WeaponEffect::SplashAir) {

		static const u32 Func_AirSplashProc = 0x0048B150;
		static const u32 Func_AirSplashProc2 = 0x0048B730;

		//probably count the amount of targets in range of AOE
		static u32* u32_0064DEB4;
		static CUnit** units_in_range = (CUnit**)0x0064EEE0;

		CUnit* targetFound;
		Box16 area_of_effect;
		u16 splash_radius;

		splash_radius = weapons_dat::OuterSplashRadius[bullet->weaponType];
		*u32_0064DEB4 = 0;

		area_of_effect.left = bullet->sprite->position.x - splash_radius;
		area_of_effect.top = bullet->sprite->position.y - splash_radius;
		area_of_effect.right = bullet->sprite->position.x + splash_radius;
		area_of_effect.bottom = bullet->sprite->position.y + splash_radius;

		//note: *u32_0064DEB4 get updated in there
		targetFound = FindBestUnit(&area_of_effect, Func_AirSplashProc, bullet);

		if (*u32_0064DEB4 != 0) {

			u32 index_unit_main_damage;

			if (*u32_0064DEB4 == 1)
				index_unit_main_damage = 0;
			else
			{
				u32 random_value = RandomizeShort(58);
				index_unit_main_damage = random_value % *u32_0064DEB4;
			}

			WeaponBulletHit_Helper(bullet, units_in_range[index_unit_main_damage], 1);

		}

		//return value unused, function is used to iterate effect on targets directly
		FindBestUnit(&area_of_effect, Func_AirSplashProc2, bullet);
		
	}
	else
	if (
		explosionType == WeaponEffect::SplashRadial ||
		explosionType == WeaponEffect::SplashEnemy ||
		explosionType == WeaponEffect::NuclearMissile
	) 
	{

		//note: those names were switched in my doc, but I fixed it
		//due to the context obviousness

		static const u32 Func_LurkerSplashProc	=	0x0048B680;
		static const u32 Func_SplashProc		=	0x0048B750;

		Box16 area_of_effect;
		u32 splash_radius;

		splash_radius = weapons_dat::OuterSplashRadius[bullet->weaponType];

		area_of_effect.left = bullet->sprite->position.x - splash_radius;
		area_of_effect.top = bullet->sprite->position.y - splash_radius;
		area_of_effect.right = bullet->sprite->position.x + splash_radius;
		area_of_effect.bottom = bullet->sprite->position.y + splash_radius;

		//return value unused, function is used to iterate effect on targets directly
		if (bullet->weaponType == WeaponId::SubterraneanSpines)
			FindBestUnit(&area_of_effect, Func_LurkerSplashProc, bullet);
		else
			FindBestUnit(&area_of_effect, Func_SplashProc, bullet);

	}
	else
	if (explosionType == WeaponEffect::Lockdown) {

		CUnit* target = bullet->attackTarget.unit;

		if (target != NULL && !(target->isDead()))
			LockdownHit_Helper(target, 131, bullet);

	}
	else
	if (explosionType == WeaponEffect::StasisField) {

		if (bullet->attackTarget.pt.x != 0 && bullet->attackTarget.pt.y != 0)
			StasisFieldHit_Helper(bullet->sourceUnit, bullet->attackTarget.pt.x, bullet->attackTarget.pt.y, bullet);

	}
	else
	if (explosionType == WeaponEffect::Parasite) {

		CUnit* target = bullet->attackTarget.unit;

		if (target != NULL && !(target->isDead())) {

			scbw::playSound(SoundId::Zerg_Queen_ZQuTag01_WAV, target);
			target->parasiteFlags |= (1 << bullet->srcPlayerId);
			scbw::refreshConsole();

			if (bullet->sourceUnit != NULL)
				function_0043F320(bullet->sourceUnit, target, 0, 0);

		}

	}
	else
	if (explosionType == WeaponEffect::Broodlings) {

		CUnit* target = bullet->attackTarget.unit;

		if (bullet->sourceUnit != NULL && target != NULL && !(target->isDead())) {
			function_0043F320(bullet->sourceUnit, target, 0, 1);
			BroodlingHit_Helper(bullet->sourceUnit, target, bullet);
		}

	}
	else
	if (explosionType == WeaponEffect::EmpShockwave) {
		EMPShockwaveHit_Helper(bullet->sourceUnit, bullet->attackTarget.pt.x, bullet->attackTarget.pt.y, bullet);
	}
	else
	if (explosionType == WeaponEffect::Restoration) {

		CUnit* target = bullet->attackTarget.unit;

		if (target != NULL && !(target->isDead())) {
			scbw::playSound(SoundId::Terran_Medic_TmedCure_wav, target);
			RestoreHit_Helper(target, bullet);
		}

	}
	else
	if (explosionType == WeaponEffect::Irradiate) {

		CUnit* target = bullet->attackTarget.unit;

		if (target != NULL && !(target->isDead())) {
			scbw::playSound(SoundId::Terran_VESSEL_TVeIrr00_WAV, target);
			IrradiateHit_Helper(bullet->sourceUnit, target, bullet->srcPlayerId);
		}

	}
	else
	if (explosionType == WeaponEffect::Ensnare) {
		EnsnareHit_Helper(bullet->sourceUnit, bullet->attackTarget.pt.x, bullet->attackTarget.pt.y, bullet);
	}
	else
	if (explosionType == WeaponEffect::Plague) {
		PlagueHit_Helper(bullet->sourceUnit, bullet->attackTarget.pt.x, bullet->attackTarget.pt.y, bullet);
	}
	else
	if (explosionType == WeaponEffect::Consume) {

		CUnit* target = bullet->attackTarget.unit;

		if (bullet->sourceUnit != NULL && target != NULL && !(target->isDead()))
			ConsumeHit_Helper(bullet->sourceUnit, target);

	}
	else
	if (explosionType == WeaponEffect::DisruptionWeb) {
		DisruptionWebHit_Helper(bullet->srcPlayerId, bullet->attackTarget.pt.x, bullet->attackTarget.pt.y, bullet);
	}
	else
	if (explosionType == WeaponEffect::CorrosiveAcid) {

		CUnit* target = bullet->attackTarget.unit;

		//bullet->hitFlags & 1 means shot missing its target
		if (target != NULL && !(bullet->hitFlags & 1)) {

			WeaponBulletHit_Helper(bullet, target, 1);

			//bullet->hitFlags & 2 means attacker is a hallucination
			if(!(bullet->hitFlags & 2))
				CorrosiveAcidHit_Helper(bullet->srcPlayerId, bullet->attackTarget.pt.x, bullet->attackTarget.pt.y, bullet);

		}

	}
	else
	if (explosionType == WeaponEffect::Maelstrom) {

		if (bullet->attackTarget.pt.x != 0 && bullet->attackTarget.pt.y != 0)
			MaelstromHit_Helper(bullet->sourceUnit, bullet->attackTarget.pt.x, bullet->attackTarget.pt.y, bullet);

	}
	else
	if (explosionType == WeaponEffect::OpticalFlare) {

		CUnit* target = bullet->attackTarget.unit;

		if (target != NULL && !(target->isDead()))
			OpticalFlareHit_Helper(target, bullet->srcPlayerId, bullet);

	}

	//Note: WeaponEffect::None end the function without doing anything

}

;

} //hooks

//-------- Helper function definitions. Do NOT modify! --------//

//Note: due to wpnspellhit hooks having access to bullets, the
//data is being passed in those helpers too, even if the function
//would not normally use the bullet data and/or may overwrite it.
//
//If bullet is not passed, it means another function than weaponImpact 
//use the same function, without providing access to "bullet" in the
//same conditions.

namespace {

const u32 Func_FatalError = 0x004215D0;
//the add esp part is dangerous, and is different in other parts of code,
//hence the "localversion" so that it doesn't get copied around carelessly.
//And abusing that to hardcode the pushed value too, this also would change
//when called from somewhere else.
void FatalError_localversion() {

	static const u32 u32_0050434C = 0x0050434C;

	__asm {
		PUSHAD
		PUSH u32_0050434C
		CALL Func_FatalError
		ADD ESP, 4
		POPAD
	}

}

;

const u32 Func_0043F320 = 0x0043F320;
//Was called ParasiteHit, but not really about that.
//Seems to be AI recalculations upon queens spells impact or something.
void function_0043F320(CUnit* attacker, CUnit* target, u32 unk1, u32 unk2) {
	__asm {
		PUSHAD
		MOV EDX, attacker
		MOV ECX, target
		PUSH unk1
		PUSH unk2
		CALL Func_0043F320
		POPAD
	}
}

;

const u32 Func_IrradiateHit = 0x00454E00;
void IrradiateHit_Helper(CUnit* attacker, CUnit* target, u32 attackingPlayerId) {
	__asm {
		PUSHAD
		MOV EDI, target
		PUSH attackingPlayerId
		PUSH attacker
		CALL Func_IrradiateHit
		POPAD
	}
}

;

const u32 Func_OpticalFlareHit = 0x00455170;
void OpticalFlareHit_Helper(CUnit* target, u32 attackingPlayerId, CBullet* bullet) {
	__asm {
		PUSHAD
		MOV EBX, bullet
		MOV EAX, target
		PUSH attackingPlayerId
		CALL Func_OpticalFlareHit
		POPAD
	}
}

;

const u32 Func_RestoreHit = 0x00455230;
void RestoreHit_Helper(CUnit* target, CBullet* bullet) {
	__asm {
		PUSHAD
		MOV EAX, bullet
		MOV EBX, target
		CALL Func_RestoreHit
		POPAD
	}
}

;

const u32 Func_LockdownHit = 0x00455380;
void LockdownHit_Helper(CUnit* target, u32 previousLockdownTimer, CBullet* bullet) {
	__asm {
		PUSHAD
		PUSH previousLockdownTimer
		MOV EDI, target
		MOV EBX, bullet
		CALL Func_LockdownHit
		POPAD
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

const u32 Func_EMPShockwaveHit = 0x00492BA0;
void EMPShockwaveHit_Helper(CUnit * attacker, int x, int y, CBullet * bullet) {
	__asm {
		PUSHAD
		MOV EBX, bullet
		MOV ECX, x
		MOV EAX, y
		PUSH attacker
		CALL Func_EMPShockwaveHit
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

const u32 Func_FindBestUnit = 0x004E8830;
CUnit* FindBestUnit(Box16* coords, u32 searchProc, CBullet* bullet) {

	static CUnit* unit;

	__asm {
		PUSHAD
		MOV EAX, coords
		MOV EDI, searchProc
		MOV EBX, bullet
		CALL Func_FindBestUnit
		MOV unit, EAX
		POPAD
	}

	return unit;

}

;

const u32 Func_CorrosiveAcidHit = 0x004F4770;
void CorrosiveAcidHit_Helper(u32 attackingPlayerId, int x, int y, CBullet* bullet) {
	__asm {
		PUSHAD
		MOV EBX, bullet
		MOV EAX, y
		MOV ECX, x
		PUSH attackingPlayerId
		CALL Func_CorrosiveAcidHit
		POPAD
	}
}

;

const u32 Func_ConsumeHit = 0x004F47B0;
void ConsumeHit_Helper(CUnit* attacker, CUnit* target) {
	__asm {
		PUSHAD
		MOV ESI, attacker
		MOV EAX, target
		CALL Func_ConsumeHit
		POPAD
	}
}

;

const u32 Func_EnsnareHit = 0x004F4830;
void EnsnareHit_Helper(CUnit* attacker, int x, int y, CBullet* bullet) {
	__asm {
		PUSHAD
		MOV EBX, bullet
		MOV EAX, y
		MOV ECX, x
		PUSH attacker
		CALL Func_EnsnareHit
		POPAD
	}
}

;

const u32 Func_DarkSwarmHit = 0x004F48A0;
void DarkSwarmHit_Helper(int x, int y, u32 attackingPlayerId, CBullet* bullet) {
	__asm {
		PUSHAD
		MOV EBX, bullet
		PUSH attackingPlayerId
		PUSH y
		PUSH x
		CALL Func_DarkSwarmHit
		POPAD
	}
}

;

const u32 Func_BroodlingHit = 0x004F4940;
void BroodlingHit_Helper(CUnit* attacker, CUnit* target, CBullet* bullet) {
	__asm {
		PUSHAD
		MOV EBX, bullet
		PUSH target
		PUSH attacker
		CALL Func_BroodlingHit
		POPAD
	}
}

;

const u32 Func_PlagueHit = 0x004F4B30;
void PlagueHit_Helper(CUnit* attacker, int x, int y, CBullet* bullet) {
	__asm {
		PUSHAD
		MOV EBX, bullet
		MOV EAX, y
		MOV ECX, x
		PUSH attacker
		CALL Func_PlagueHit
		POPAD
	}
}

;

const u32 Func_MaelstromHit = 0x004F6850;
void MaelstromHit_Helper(CUnit* attacker, int x, int y, CBullet* bullet) {
	__asm {
		PUSHAD
		MOV EBX, bullet
		MOV EAX, y
		PUSH attacker
		PUSH x
		CALL Func_MaelstromHit
		POPAD
	}
}

;

const u32 Func_StasisFieldHit = 0x004F6A80;
void StasisFieldHit_Helper(CUnit* attacker, int x, int y, CBullet* bullet) {
	__asm {
		PUSHAD
		MOV EBX, bullet
		MOV EAX, x
		MOV ECX, y
		PUSH attacker
		CALL Func_StasisFieldHit
		POPAD
	}
}

;

const u32 Func_DisruptionWebHit = 0x004F6AF0;
void DisruptionWebHit_Helper(u32 attackingPlayerId, int x, int y, CBullet* bullet) {
	__asm {
		PUSHAD
		MOV EBX, bullet
		PUSH attackingPlayerId
		PUSH y
		PUSH x
		CALL Func_DisruptionWebHit
		POPAD
	}
}

;

} //Unnamed namespace

//End of helper functions

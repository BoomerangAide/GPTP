//The header file for the Weapon/Spell hit hook module.
#pragma once
#include <SCBW/structures/CUnit.h>
#include <SCBW/structures/CBullet.h>

namespace hooks {

	void IrradiateHit(CUnit* attacker, CUnit* target, u8 attackingPlayerId);		//54E00
	void OpticalFlareHit(CUnit* target, u32 attackingPlayerId, CBullet* bullet);	//55170
	void RestoreHit(CUnit* target, CBullet* bullet);								//55230
	void LockdownHit(CUnit* target, u8 previousLockdownTimer, CBullet* bullet);		//55380
	void WeaponBulletHit(CBullet* bullet, CUnit* target, u32 damageDivisor);		//79AE0
	void MeleeAttackHit(CUnit* attacker);											//79B40
	void EMPShockwaveHit(CUnit* attacker, int x, int y, CBullet* bullet);			//92BA0
	void CorrosiveAcidHit(u32 attackingPlayerId, int x, int y, CBullet* bullet);	//F4770
	void ConsumeHit(CUnit* attacker, CUnit* target);								//F47B0
	void EnsnareHit(CUnit* attacker, int x, int y, CBullet* bullet);				//F4830
	void DarkSwarmHit(int x, int y, u32 attackingPlayerId, CBullet* bullet);		//F48A0
	void BroodlingHit(CUnit* attacker, CUnit* target, CBullet* bullet);				//F4940
	void PlagueHit(CUnit* attacker, int x, int y, CBullet* bullet);					//F4B30
	void MaelstromHit(CUnit* attacker, int x, int y, CBullet* bullet);				//F6850
	void MindControlHit(CUnit* attacker, CUnit* target);							//F6910
	void StasisFieldHit(CUnit* attacker, int x, int y, CBullet* bullet);			//F6A80
	void DisruptionWebHit(u32 attackingPlayerId, int x, int y, CBullet* bullet);	//F6AF0

	//The injector function for this hook module.
	void injectWpnSpellHitHooks();

} //hooks

//Hooks could be confirmed thanks to 0048B770 CBullet_Damage that call almost all of them
//Hit Features not supported by these hooks may be hardcoded in that function (like
//the Parasite Hit effect)

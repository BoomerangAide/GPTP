//The source file for the Weapon/Spell Hit hook module.
#include "wpnspellhit.h"
#include <SCBW/api.h>

//Helper functions definitions

namespace {

	void fixTargetLocation(Point16* coords, u32 unitId);										//01FA0
	bool function_0042D810(int x, int y, u32 unitId);											//2D810
	CUnit** getAllUnitsInBounds(Box16* coords);													//2FF80
	void incrementUnitKillCount(CUnit* unit);													//759C0
	u32 getUnitDamageBonus(CUnit* unit, u8 weaponId);											//75EC0
	void attackOverlayAndNotify(CUnit* attacker, CUnit* target, u32 weaponType, u32 direction);	//79730
	bool setThingyVisibilityFlags(CThingy* thingy);												//878F0
	CThingy* createThingy(u32 spriteId, s16 x, s16 y, u32 playerId);							//88210
	void incrementUnitDeathScores(CUnit* unit, u8 player);										//88AF0
	u32 GetUnitBulletDamage(CBullet* bullet, CUnit* target);									//8ACD0
	u32 getSpellStatString(CUnit* target, u32 techId, u8 playerId);								//91E80
	void function_492CC0(CUnit* unit);															//92CC0
	void setAllImageGroupFlagsPal11(CSprite* sprite);											//97430
	void displayLastNetErrForPlayer(u32 playerId);												//9E530
	void updateUnitStrength(CUnit* unit);														//9FA40
	void function_004A01F0(CUnit* unit);														//A01F0
	CUnit* CreateUnit(u32 unitId, int x, int y, u32 playerId);									//A09D0
	void AI_TrainingUnit(CUnit* unit_creator, CUnit* created_unit);								//A2830
	void IterateUnitsAtLocationTargetProc_CorrosiveAcid(u32 attackingPlayerId,Box16* coords);	//E8280
	void IterateUnitsAtLocationTargetProc_Ensnare(CUnit* attacker,Box16* coords);				//E8280
	void IterateUnitsAtLocationTargetProc_Plague(CUnit* attacker,Box16* coords);				//E8280
	void IterateUnitsAtLocationTargetProc_Maelstrom(CUnit* attacker,Box16* coords);				//E8280
	void IterateUnitsAtLocationTargetProc_StasisField(CUnit* attacker,Box16* coords);			//E8280

} //unnamed namespace

namespace hooks {

	//no bullet because this function can be called in another context without bullet
	void IrradiateHit(CUnit* attacker, CUnit* target, u8 attackingPlayerId) {

		//if not already irradiated and not burrowed, add the irradiate overlay
		if(target->irradiateTimer == 0 && !(target->status & UnitStatus::Burrowed)) {

			u32 irradiateImageId;
			CUnit* unitWithOverlay;

			if(units_dat::BaseProperty[target->id] & UnitProperty::MediumOverlay)
				irradiateImageId = ImageId::Irradiate_Medium;
			else
			if(units_dat::BaseProperty[target->id] & UnitProperty::LargeOverlay)
				irradiateImageId = ImageId::Irradiate_Large;
			else
				irradiateImageId = ImageId::Irradiate_Small;

			if(target->subunit != NULL)
				unitWithOverlay = target->subunit;
			else
				unitWithOverlay = target;

			(unitWithOverlay->sprite)->createTopOverlay(irradiateImageId,0,0,0);

		}

		target->irradiateTimer = weapons_dat::Cooldown[WeaponId::Irradiate];
		target->irradiatedBy = attacker;
		target->irradiatePlayerId = attackingPlayerId;	

	} //void IrradiateHit(CUnit* attacker, CUnit* target, u8 attackingPlayerId)

	;

	//bullet is not normally part of the function, so if the function is called
	//in an unexpected way, it may not contain the correct value.
	void OpticalFlareHit(CUnit* target, u32 attackingPlayerId, CBullet* bullet) {

		//can take the value 100 (0x64), thus not Bool32
		static u32*		const u32_0051CE98 =					(u32*)		0x0051CE98;

		if(target->status & UnitStatus::IsHallucination)
			target->remove();
		else {

			u32 overlayImageId;
			CUnit* overlayTargetUnit;

			//blindness is applied for a specific player
			//just like parasite, but due to how it's used,
			//a blind unit is still blind for all players
			target->isBlind |= (u8)(1 << attackingPlayerId);

			//playing the impact sound
			scbw::playSound(SoundId::Terran_Medic_Miopia1_wav,target);

			//create impact overlay

			if(units_dat::BaseProperty[target->id] & UnitProperty::MediumOverlay)
				overlayImageId = ImageId::OpticalFlareHit_Medium;
			else
			if(units_dat::BaseProperty[target->id] & UnitProperty::LargeOverlay)
				overlayImageId = ImageId::OpticalFlareHit_Large;
			else
				overlayImageId = ImageId::OpticalFlareHit_Small;

			if(target->subunit != NULL)
				overlayTargetUnit = target->subunit;
			else
				overlayTargetUnit = target;

			(overlayTargetUnit->sprite)->createTopOverlay(overlayImageId,0,0,0);

			//was hardcoded in original code
			scbw::refreshConsole();

			//cause some functions to be skipped in Game Loop
			//possibly delay fog of war reappearance following
			//loss of sight, or cause it?
			*u32_0051CE98 = 1;

		}

	} //void OpticalFlareHit(CUnit* target, u8 attackingPlayerId)

	;

	//bullet is not normally part of the function, so if the function is called
	//in an unexpected way, it may not contain the correct value.
	void RestoreHit(CUnit* target, CBullet* bullet) {

		if(target->status & UnitStatus::IsHallucination)
			target->remove();
		else {

			//create impact overlay

			u32 overlayImageId;
			CUnit* overlayTargetUnit;

			if(units_dat::BaseProperty[target->id] & UnitProperty::MediumOverlay)
				overlayImageId = ImageId::RestorationHit_Medium;
			else
			if(units_dat::BaseProperty[target->id] & UnitProperty::LargeOverlay)
				overlayImageId = ImageId::RestorationHit_Large;
			else
				overlayImageId = ImageId::RestorationHit_Small;

			if(target->subunit != NULL)
				overlayTargetUnit = target->subunit;
			else
				overlayTargetUnit = target;

			(overlayTargetUnit->sprite)->createTopOverlay(overlayImageId,0,0,0);

			//remove status effects and corresponding overlays
			//timers set to 0 twice reflect the original code

			target->parasiteFlags = 0;
			target->isBlind = 0;

			if(target->ensnareTimer != 0) {

				target->ensnareTimer = 0;
				target->removeOverlay(ImageId::EnsnareOverlay_Small,ImageId::EnsnareOverlay_Large);
				target->ensnareTimer = 0;

				//specific update following ensnare removal
				target->updateSpeed();

			}

			if(target->plagueTimer != 0) {
				target->plagueTimer = 0;
				target->removeOverlay(ImageId::PlagueOverlay_Small,ImageId::PlagueOverlay_Large);
				target->plagueTimer = 0;
			}

			if(target->irradiateTimer != 0) {
				target->irradiateTimer = 0;
				target->removeOverlay(ImageId::Irradiate_Small,ImageId::Irradiate_Large);
				target->irradiateTimer = 0;

				//specific updates following irradiate removal
				target->irradiatedBy = NULL;
				target->irradiatePlayerId = 8;

			}

			if(target->lockdownTimer != 0)
				target->removeLockdown();

			if(target->maelstromTimer != 0)
				target->removeMaelstrom();

			if(target->acidSporeCount != 0)
				target->removeAcidSpores();

			//was hardcoded in original code
			scbw::refreshConsole();

		}

	} //void RestoreHit(CUnit* target)

	;

	//bullet is not normally part of the function, so if the function is called
	//in an unexpected way, it may not contain the correct value.
	//...
	//the timer is reset before this call 2 times out of 3 (with
	//previousLockdownTimer holding the pre-reset value), and
	//the 3rd case, previousLockdownTimer is set to 131 (0x83)
	void LockdownHit(CUnit* target, u8 previousLockdownTimer, CBullet* bullet) {

		//add lockdown overlay if no ongoing lockdown already
		if(target->lockdownTimer == 0) {

			u32 overlayImageId;
			CUnit* overlayTargetUnit;

			if(units_dat::BaseProperty[target->id] & UnitProperty::MediumOverlay)
				overlayImageId = ImageId::LockdownField_Medium;
			else
			if(units_dat::BaseProperty[target->id] & UnitProperty::LargeOverlay)
				overlayImageId = ImageId::LockdownField_Large;
			else
				overlayImageId = ImageId::LockdownField_Small;

			if(target->subunit != NULL)
				overlayTargetUnit = target->subunit;
			else
				overlayTargetUnit = target;

			(overlayTargetUnit->sprite)->createTopOverlay(overlayImageId,0,0,0);

		}

		//this added to the fact that target->lockdownTimer is set to 0 before
		//this function probably prevent lockdown to reset the duration of a 
		//previous lock
		if(previousLockdownTimer > target->lockdownTimer)
			target->lockdownTimer = previousLockdownTimer;

		//should stop movements/orders/scripts of the unit, give it the status 
		//DoodadStatesThing, may have an effect on selection, and perform
		//scbw::refreshConsole() code at the end
		function_492CC0(target);

	} //void LockdownHit(CUnit* target, u32 previousLockdownTimer)

	;

	//was called "Shot" instead of "Hit", changed due to the context
	void WeaponBulletHit(CBullet* bullet, CUnit* target, u32 damageDivisor) {

		if(bullet->hitFlags & 2)	//bullet doing no damage (shot from hallucination)
			attackOverlayAndNotify(bullet->sourceUnit,target,bullet->weaponType,bullet->direction1); //no damage, but still ask for retaliation
		else {

			u32 base_damage = GetUnitBulletDamage(bullet,target);

			target->damageWith(
				base_damage,
				bullet->weaponType,
				bullet->sourceUnit,
				bullet->srcPlayerId,
				bullet->direction1,
				damageDivisor
			);

		}

	} //void WeaponBulletHit(CBullet* bullet, CUnit* target, u32 hitFlags)

	;

	//original name was ISCRIPT_AttackMelee @ 00479B40
	//Changed due to the context
	void MeleeAttackHit(CUnit* attacker) {

		CUnit* target = attacker->orderTarget.unit;

		if(target != NULL) {

			u8 weaponId;

			//Lurker can only attack while burrowed even if
			//given a melee attack according to this code
			if(
				attacker->id != UnitId::ZergLurker ||
				attacker->status & UnitStatus::Burrowed
			)
				weaponId = units_dat::GroundWeapon[attacker->id];
			else
				weaponId = WeaponId::None;

			if(attacker->status & UnitStatus::IsHallucination)
				attackOverlayAndNotify(attacker,target,weaponId,attacker->currentDirection1); //no damage, but still ask for retaliation
			else {

				u32 damage = getUnitDamageBonus(attacker,weaponId);

				damage = ((u16)damage + weapons_dat::DamageAmount[weaponId]) * 256;

				target->damageWith(
					damage,
					weaponId,
					attacker,
					attacker->playerId,
					attacker->currentDirection1,
					1
				);

			}

		}

	} //void MeleeAttackHit(CUnit* attacker)

	;

	//bullet is not normally part of the function, so if the function is called
	//in an unexpected way, it may not contain the correct value.
	//...
	//In the original code, the parameters are gotten from CBullet as:
	//attacker from bullet->sourceUnit
	//x and y from bullet->sprite->position
	void EMPShockwaveHit(CUnit* attacker, int x, int y, CBullet* bullet) {

		static u16* const maxBoxRightValue =			(u16*) 0x00628450;	//should usually be mapTileSize->width * 32
		static u16* const maxBoxBottomValue =			(u16*) 0x006284B4;	//should usually be mapTileSize->height * 32

		Box16 area_of_effect;

		CUnit** unitsInAreaOfEffect;
		CUnit* current_unit;

		//define the base area of effect
		area_of_effect.left = (u16)x -  weapons_dat::InnerSplashRadius[WeaponId::EMP_Shockwave];
		area_of_effect.right = (u16)x +  weapons_dat::InnerSplashRadius[WeaponId::EMP_Shockwave];
		area_of_effect.top = (u16)y -  weapons_dat::InnerSplashRadius[WeaponId::EMP_Shockwave];
		area_of_effect.bottom = (u16)y +  weapons_dat::InnerSplashRadius[WeaponId::EMP_Shockwave];

		//check and fix effect if beyond width of map
		if(area_of_effect.left < 0)
			area_of_effect.left = 0;
		else
		if(area_of_effect.right > *maxBoxRightValue)
			area_of_effect.right = *maxBoxRightValue;

		//check and fix effect if beyond height of map
		if(area_of_effect.top < 0)
			area_of_effect.top = 0;
		else
		if(area_of_effect.bottom > *maxBoxBottomValue)
			area_of_effect.bottom = *maxBoxBottomValue;

		//find all units in area of effect and pick the first
		unitsInAreaOfEffect = getAllUnitsInBounds(&area_of_effect);
		current_unit = *unitsInAreaOfEffect;

		while(current_unit != NULL) {

			if(
				current_unit != attacker &&				//EMP doesn't affect the attacker
				(
					attacker == NULL ||					//EMP doesn't affect the attacker
					current_unit != attacker->subunit	//subunit
				)
			)
			{

				if(current_unit->status & UnitStatus::IsHallucination)
					current_unit->remove();
				else
				if(current_unit->stasisTimer == 0) { //don't work against units in stasis

					current_unit->energy = 0;
					current_unit->shields = 0;

					if(
						current_unit->sprite->flags & CSprite_Flags::Selected &&
						current_unit->sprite != NULL /*I'm following original code, though this order of tests doesn't make sense*/
					) 
						setAllImageGroupFlagsPal11(current_unit->sprite); //redraw the unit

				}

			}

			unitsInAreaOfEffect++;					//go on next unit of the list (or null)
			current_unit = *unitsInAreaOfEffect;

		} //while(current_unit != NULL)

		//reload the previous temporary unit list from before the call to getAllUnitsInBounds
		*tempUnitsListCurrentArrayCount = tempUnitsListArraysCountsListLastIndex[*tempUnitsListArraysCountsListLastIndex];
		*tempUnitsListArraysCountsListLastIndex = *tempUnitsListArraysCountsListLastIndex - 1;

	} //void EMPShockwaveHit(CUnit* attacker, int x, int y)

	;

	//bullet is not normally part of the function, so if the function is called
	//in an unexpected way, it may not contain the correct value.
	void CorrosiveAcidHit(u32 attackingPlayerId, int x, int y, CBullet* bullet) {

		Box16 coords;

		coords.left = x - 64;
		coords.bottom = y + 64;
		coords.top = y - 64;
		coords.right = x + 64;

		IterateUnitsAtLocationTargetProc_CorrosiveAcid(attackingPlayerId,&coords);
		
	} //void CorrosiveAcidHit(u32 attackingPlayerId, int x, int y, CBullet* bullet)

	;

	//no bullet because the code normally calling this function lose the 
	//easily available information (and getting it the hard way seems overkill)
	void ConsumeHit(CUnit* attacker, CUnit* target) {

		if(
			target != NULL && 
			!(target->status & UnitStatus::Invincible) &&
			getSpellStatString(target,TechId::Consume,attacker->playerId) == 0 //returning a value != 0 means there was an error
		) 
		{

			incrementUnitDeathScores(target,attacker->playerId);
			target->remove();

			if(!(target->status & UnitStatus::IsHallucination)) {

				u16 energyMax = attacker->getMaxEnergy();

				if((energyMax - attacker->energy) < 12800) //12800 is 50 energy ingame
					attacker->energy = energyMax;
				else
					attacker->energy += 12800;

			}

		}

	} //void ConsumeHit(CUnit* attacker, CUnit* target)

	;

	//bullet is not normally part of the function, so if the function is called
	//in an unexpected way, it may not contain the correct value.
	void EnsnareHit(CUnit* attacker, int x, int y, CBullet* bullet) {

		Box16 coords;
		CThingy* ensnare_effect_sprite;

		ensnare_effect_sprite = createThingy(SpriteId::Ensnare,x,y,0);

		if(ensnare_effect_sprite != NULL) {
			ensnare_effect_sprite->sprite->elevationLevel = 19; //0x13
			setThingyVisibilityFlags(ensnare_effect_sprite);
		}

		coords.top = y - 64;
		coords.right = x + 64;
		coords.left = x - 64;
		coords.bottom = y + 64;
		
		IterateUnitsAtLocationTargetProc_Ensnare(attacker,&coords);

	} //void EnsnareHit(CUnit* attacker, int x, int y, CBullet* bullet)
		
	;

	//bullet is not normally part of the function, so if the function is called
	//in an unexpected way, it may not contain the correct value.
	void DarkSwarmHit(int x, int y, u32 attackingPlayerId, CBullet* bullet) {

		Point16 coords;
		CUnit* dark_swarm_unit;

		coords.x = x;
		coords.y = y;

		//adjust location where Dark Swarm will be created
		//if necessary (by modifying coords)
		fixTargetLocation(&coords,UnitId::Spell_DarkSwarm);

		//the dark swarm is created for player 11 (neutral), not for the owner
		dark_swarm_unit = CreateUnit(UnitId::Spell_DarkSwarm,coords.x,coords.y,11);

		if(dark_swarm_unit == NULL)
			displayLastNetErrForPlayer(attackingPlayerId);
		else {

			dark_swarm_unit->status |= UnitStatus::NoCollide;
			dark_swarm_unit->sprite->elevationLevel = 11;	//0x0B

			//update various stuff (set hp, set shield...) not finished on CreateUnit
			function_004A01F0(dark_swarm_unit);
			updateUnitStrength(dark_swarm_unit);

			if(
				dark_swarm_unit->removeTimer == 0 ||
				dark_swarm_unit->removeTimer > 900
			)
				dark_swarm_unit->removeTimer = 900;	//0x0384

		}

	} //void DarkSwarmHit(int x, int y, u32 attackingPlayerId, CBullet* bullet)

	;

	//bullet is not normally part of the function, so if the function is called
	//in an unexpected way, it may not contain the correct value.
	void BroodlingHit(CUnit* attacker, CUnit* target, CBullet* bullet) {

		if(!(target->status & UnitStatus::IsHallucination)) {

			int current_x = target->sprite->position.x - 2;
			int current_y = target->sprite->position.y - 2;
			u32 broodlings_to_spawn = 2;

			do {

				bool jump_to_end_of_loop = false; //F4A82

				//check various conditions before attempting to create a broodling
				if(function_0042D810(current_x,current_y,UnitId::ZergBroodling)) {

					Point16 inPos,outPos;
					Box16 moveArea;

					inPos.y = current_y;
					moveArea.top = current_y - 32;
					moveArea.bottom = current_y + 32;
					moveArea.left = current_x - 32;
					inPos.x = current_x;
					moveArea.right = current_x + 32;

					//if impossible to avoid a collision even within moveArea,
					//skip the broodling creation code for the current position
					jump_to_end_of_loop = !scbw::checkUnitCollisionPos(attacker,&inPos,&outPos,&moveArea,false,0);

					if(!jump_to_end_of_loop) {
						current_y = outPos.y;
						current_x = outPos.x;
					}

				}

				if(!jump_to_end_of_loop) { //F49E2

					Point16 pos;
					CUnit* created_broodling;

					pos.x = current_x;
					pos.y = current_y;

					//adjust the position to welcome the unit...I think
					fixTargetLocation(&pos,UnitId::ZergBroodling);

					//create the broodling
					created_broodling = CreateUnit(UnitId::ZergBroodling,pos.x,pos.y,attacker->playerId);

					if(created_broodling == NULL) {
						displayLastNetErrForPlayer(attacker->playerId);
						jump_to_end_of_loop = true;
					}
					else { //F4A26

						s32 timerBeforeDisappearance;

						//update various stuff (set hp, set shield...) not finished on CreateUnit
						function_004A01F0(created_broodling);
						updateUnitStrength(created_broodling);

						if(created_broodling->status & UnitStatus::IsHallucination)
							timerBeforeDisappearance = 1350;	//probably old unremoved code
						else
						if(created_broodling->id == UnitId::ZergBroodling)
							timerBeforeDisappearance = 1800;	//0x708
						else
							timerBeforeDisappearance = 0;

						if(
							created_broodling->removeTimer == 0 ||
							created_broodling->removeTimer > timerBeforeDisappearance
						)
							created_broodling->removeTimer = timerBeforeDisappearance;

						//AI stuff (may establish relationship between queen and broodling and/or
						//be about supplies management, or something else?
						if(attacker->pAI != NULL)
							AI_TrainingUnit(attacker,created_broodling);

					}

				} //if(!jump_to_end_of_loop)

				//F4A82 (end of loop)
				current_y += 4;
				current_x += 4;
				broodlings_to_spawn--;

			}while(broodlings_to_spawn > 0);

		} //if(!(target->status & UnitStatus::IsHallucination))

		//F4A9F

		//give the target death as a kill to the queen
		incrementUnitDeathScores(attacker,attacker->playerId);

		//kill the target
		target->remove();

		if(attacker->killCount < 255)
			attacker->killCount++;

		//code for the case where interceptors would be firing
		//the broodling-generating weapon, to give +1 kill to the
		//carrier owning the interceptor making the kill
		if(
			attacker->id == UnitId::ProtossInterceptor &&
			attacker->interceptor.parent != NULL
		)
			incrementUnitKillCount(attacker->interceptor.parent);

	} //void BroodlingHit(CUnit* attacker, CUnit* target, CBullet* bullet)

	;

	//bullet is not normally part of the function, so if the function is called
	//in an unexpected way, it may not contain the correct value.
	void PlagueHit(CUnit* attacker, int x, int y, CBullet* bullet) {

		Box16 coords;

		coords.left = x - 64;
		coords.bottom = y + 64;
		coords.top = y - 64;
		coords.right = x + 64;

		IterateUnitsAtLocationTargetProc_Plague(attacker,&coords);

	} //void PlagueHit(CUnit* attacker, int x, int y, CBullet* bullet)

	;

	//bullet is not normally part of the function, so if the function is called
	//in an unexpected way, it may not contain the correct value.
	void MaelstromHit(CUnit* attacker, int x, int y, CBullet* bullet) {

		Box16 coords;
		CThingy* maelstrom_effect_sprite = createThingy(SpriteId::Maelstrom_Hit,x,y,0);

		if(maelstrom_effect_sprite != NULL) {
			maelstrom_effect_sprite->sprite->elevationLevel = 17; //0x11
			setThingyVisibilityFlags(maelstrom_effect_sprite);
		}

		scbw::playSound(SoundId::Protoss_Darchon_Parahit_wav,attacker);

		coords.right = x + 48;
		coords.left = x - 48;
		coords.top = y - 48;
		coords.bottom = y + 48;

		IterateUnitsAtLocationTargetProc_Maelstrom(attacker,&coords);

	} //void MaelstromHit(CUnit* attacker, int x, int y, CBullet* bullet)

	;

	//no bullet because this function is called in another context without bullet
	void MindControlHit(CUnit* attacker, CUnit* target) {

		if(target->status & UnitStatus::IsHallucination)
			target->remove();
		else {

			target->giveTo(attacker->playerId);

			if(target->id == UnitId::ProtossDarkArchon)
				target->energy = 0;

		}

	} //void MindControlHit(CUnit* attacker, CUnit* target)

	;

	//bullet is not normally part of the function, so if the function is called
	//in an unexpected way, it may not contain the correct value.
	void StasisFieldHit(CUnit* attacker, int x, int y, CBullet* bullet) {

		Box16 coords;
		CThingy* stasisfield_effect_sprite = createThingy(SpriteId::Stasis_Field_Hit,x,y,0);

		if(stasisfield_effect_sprite != NULL) {
			stasisfield_effect_sprite->sprite->elevationLevel = 17; //0x11
			setThingyVisibilityFlags(stasisfield_effect_sprite);
		}

		coords.top = y - 48;
		coords.right = x + 48;
		coords.left = x - 48;
		coords.bottom = y + 48;

		IterateUnitsAtLocationTargetProc_StasisField(attacker,&coords);

	} //void StasisFieldHit(CUnit* attacker, int x, int y, CBullet* bullet)

	;

	//bullet is not normally part of the function, so if the function is called
	//in an unexpected way, it may not contain the correct value.
	void DisruptionWebHit(u32 attackingPlayerId, int x, int y, CBullet* bullet) {

		Point16 coords;
		CUnit* disruption_web_unit;

		coords.x = x;
		coords.y = y;

		//adjust location where Disruption Web will be created
		//if necessary (by modifying coords)
		fixTargetLocation(&coords,UnitId::Spell_DisruptionWeb);

		//create the Disruption Web unit for neutral player 11
		disruption_web_unit = CreateUnit(UnitId::Spell_DisruptionWeb,coords.x,coords.y,11);

		if(disruption_web_unit == NULL)
			displayLastNetErrForPlayer(attackingPlayerId);
		else {

			disruption_web_unit->status |= UnitStatus::NoCollide;
			disruption_web_unit->sprite->elevationLevel = 11; //0x0B

			//update various stuff (set hp, set shield...) not finished on CreateUnit
			function_004A01F0(disruption_web_unit);
			updateUnitStrength(disruption_web_unit);

			if(
				disruption_web_unit->removeTimer == 0 ||
				disruption_web_unit->removeTimer > 360
			)
				disruption_web_unit->removeTimer = 360; //0x0168

		}

	} //void DisruptionWebHit(u32 attackingPlayerId, int x, int y, CBullet* bullet)

	;

} //hooks

//-------- Helper function definitions. Do NOT modify! --------//

namespace {

	const u32 Func_fixTargetLocation = 0x00401FA0;
	void fixTargetLocation(Point16* coords, u32 unitId) {

		__asm {
			PUSHAD
			MOV EAX, unitId
			MOV EDX, coords
			CALL Func_fixTargetLocation
			POPAD
		}

	}

	;

	const u32 Func_Sub42D810 = 0x0042D810;
	bool function_0042D810(int x, int y, u32 unitId) {

		static Bool32 bPreReturnValue;

		__asm {
			PUSHAD
			MOV EAX, x
			MOV EDI, y
			MOV EBX, unitId
			CALL Func_Sub42D810
			MOV bPreReturnValue, EAX
			POPAD
		}

		return (bPreReturnValue != 0);

	}

	;

	const u32 Func_GetAllUnitsInBounds = 0x0042FF80;
	CUnit** getAllUnitsInBounds(Box16* coords) {

		static CUnit** units_in_bounds;

		__asm {
			PUSHAD
			MOV EAX, coords
			CALL Func_GetAllUnitsInBounds
			MOV units_in_bounds, EAX
			POPAD
		}

		return units_in_bounds;

	}

	;

	const u32 Func_getUnitDamageBonus = 0x00475EC0;
	u32 getUnitDamageBonus(CUnit* unit, u8 weaponId) {

		static u32 damageBonus;

		__asm {
			PUSHAD
			MOV EAX, unit
			MOV DL, weaponId
			CALL Func_getUnitDamageBonus
			MOV damageBonus, EAX
			POPAD
		}

		return damageBonus;

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

	const u32 Func_attackOverlayAndNotify = 0x00479730;
	void attackOverlayAndNotify(CUnit* attacker, CUnit* target, u32 weaponType, u32 direction) {

		__asm {
			PUSHAD
			MOV ESI, attacker
			PUSH direction
			MOV EAX, target
			PUSH weaponType
			CALL Func_attackOverlayAndNotify
			POPAD
		}

	}

	;

	const u32 Func_SetThingyVisibilityFlags = 0x004878F0;
	bool setThingyVisibilityFlags(CThingy* thingy) {

		static Bool32 bPreResult;

		__asm {
			PUSHAD
			MOV ESI, thingy
			CALL Func_SetThingyVisibilityFlags
			MOV bPreResult, EAX
			POPAD
		}

		return (bPreResult != 0);

	}

	;

	//original referenced name was replaceSprite (but this one is probably
	//more accurate since it does create something rather than replacing)
	const u32 Func_CreateThingy = 0x00488210;
	CThingy* createThingy(u32 spriteId, s16 x, s16 y, u32 playerId) {

		static CThingy* thingy;
		s32 x_ = x;

		__asm {
			PUSHAD
			PUSH playerId
			MOVSX EDI, y
			PUSH x_
			PUSH spriteId
			CALL Func_CreateThingy
			MOV thingy, EAX
			POPAD
		}

		return thingy;

	}

	;

	const u32 Func_IncrementUnitDeathScores = 0x00488AF0;
	void incrementUnitDeathScores(CUnit* unit, u8 playerId) {

	  __asm {
		PUSHAD
		MOV EDI, unit
		MOVZX EDX, playerId
		CALL Func_IncrementUnitDeathScores
		POPAD
	  }

	}

	;

	const u32 Func_GetUnitBulletDamage = 0x0048ACD0;
	u32 GetUnitBulletDamage(CBullet* bullet, CUnit* target) {

		static u32 return_value;

		__asm {
			PUSHAD
			MOV EAX, target
			MOV EDX, bullet
			CALL Func_GetUnitBulletDamage
			MOV return_value, EAX
			POPAD
		}

		return return_value;

	}

	;

	const u32 Func_getSpellStatString = 0x00491E80;
	u32 getSpellStatString(CUnit* target, u32 techId, u8 playerId) {

		static u32 return_value;

		__asm {
			PUSHAD
			MOV BL, playerId
			PUSH techId
			MOV EAX, target
			CALL Func_getSpellStatString
			MOV return_value, EAX
			POPAD
		}

		return return_value;
	}

	;

	const u32 Func_Sub492CC0 = 0x00492CC0;
	void function_492CC0(CUnit* unit) {

		__asm {
			PUSHAD
			MOV EAX, unit
			CALL Func_Sub492CC0
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

	const u32 Func_Sub4A01F0 = 0x004A01F0;
	void function_004A01F0(CUnit* unit) {

		__asm {
			PUSHAD
			MOV EAX, unit
			CALL Func_Sub4A01F0
			POPAD
		}

	}

	;

	const u32 Func_CreateUnit = 0x004A09D0;
	CUnit* CreateUnit(u32 unitId, int x, int y, u32 playerId) {

		static CUnit* unit_created;

		__asm {
			PUSHAD
			PUSH playerId
			PUSH y
			MOV ECX, unitId
			MOV EAX, x
			CALL Func_CreateUnit
			MOV unit_created, EAX
			POPAD
		}

		return unit_created;

	}

	;

	const u32 Func_displayLastNetErrForPlayer = 0x0049E530;
	void displayLastNetErrForPlayer(u32 playerId) {
		__asm {
			PUSHAD
			PUSH playerId
			CALL Func_displayLastNetErrForPlayer
			POPAD
		}
	}

	;

	const u32 Func_UpdateUnitStrength = 0x0049FA40;
	void updateUnitStrength(CUnit* unit) {

		__asm {
			PUSHAD
			MOV EAX, unit
			CALL Func_UpdateUnitStrength
			POPAD
		}

	}

	;

	const u32 Func_AI_TrainingUnit = 0x004A2830;
	void AI_TrainingUnit(CUnit* unit_creator, CUnit* created_unit) {
		__asm {
			PUSHAD
			MOV EAX, created_unit
			MOV ECX, unit_creator
			CALL Func_AI_TrainingUnit
			POPAD
		}
	}

	;

	const u32 Func_IterateUnitsAtLocationTargetProc = 0x004E8280;
	void IterateUnitsAtLocationTargetProc_CorrosiveAcid(u32 attackingPlayerId,Box16* coords) {

		const u32 CorrosiveAcidProc_function = 0x004F4670;

		__asm {
			PUSHAD
			PUSH attackingPlayerId
			MOV EAX, coords
			MOV EBX, CorrosiveAcidProc_function
			CALL Func_IterateUnitsAtLocationTargetProc
			POPAD
		}

	}

	void IterateUnitsAtLocationTargetProc_Ensnare(CUnit* attacker,Box16* coords) {

		const u32 EnsnareProc_function = 0x004F46C0;

		__asm {
			PUSHAD
			PUSH attacker
			MOV EAX, coords
			MOV EBX, EnsnareProc_function
			CALL Func_IterateUnitsAtLocationTargetProc
			POPAD
		}

	}
	void IterateUnitsAtLocationTargetProc_Plague(CUnit* attacker,Box16* coords) {

		const u32 PlagueProc_function = 0x004F4AF0;

		__asm {
			PUSHAD
			PUSH attacker
			MOV EAX, coords
			MOV EBX, PlagueProc_function
			CALL Func_IterateUnitsAtLocationTargetProc
			POPAD
		}

	}

	void IterateUnitsAtLocationTargetProc_Maelstrom(CUnit* attacker,Box16* coords) {

		const u32 MaelstromProc_function = 0x004F6760;

		__asm {
			PUSHAD
			PUSH attacker
			MOV EAX, coords
			MOV EBX, MaelstromProc_function
			CALL Func_IterateUnitsAtLocationTargetProc
			POPAD
		}

	}


	void IterateUnitsAtLocationTargetProc_StasisField(CUnit* attacker,Box16* coords) {

		const u32 StasisFieldProc_function = 0x004F68D0;

		__asm {
			PUSHAD
			PUSH attacker
			MOV EAX, coords
			MOV EBX, StasisFieldProc_function
			CALL Func_IterateUnitsAtLocationTargetProc
			POPAD
		}

	}


	;

} //Unnamed namespace

//End of helper functions

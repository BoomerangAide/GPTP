#include "recall_spell.h"
#include <SCBW/api.h>

//helper functions def

namespace {

	bool setThingyVisibilityFlags(CThingy* thingy);									//878F0
	CThingy* createThingy(u32 spriteId, s16 x, s16 y, u32 playerId);				//88210
	void setUnitStatTxtErrorMsg(char* message);										//8CCB0
	void playUnitSFX(u32 soundId, u16 x, u16 y, u32 unk1, u32 unk2);				//8EC10
	u32 RandomizeShort(u32 unknown_index);											//DC4A0
	u32 IterateUnitsAtLocationTargetProc_RecallTargets(CUnit* unit, Box16* coords);	//E8280
	void function_004EBAE0(CUnit* unit, s32 x, s32 y);								//EBAE0

} //unnamed namespace

namespace hooks {

	//Originally known as sub_4942D0
	//Function performed on each target of Recall
	//by IterateUnitsAtLocationTargetProc_RecallTargets
	//that was validated throug spells_Recall_ActionOnTargetInRange
	//Do the actual unit movement
	void spells_Recall_ActionOnValidTargetForRecall(CUnit* target, CUnit* caster) {

		static Bool32* const bAtLeast1UnitWasRecalled = (Bool32*)(0x0063FF50);

		Point16 previous_pos, new_pos;

		previous_pos.x = target->sprite->position.x;
		previous_pos.y = target->sprite->position.y;

		//identical code to setUnitPosition() but for some reason, is
		//using another function to do exactly the same thing
		function_004EBAE0(target,caster->sprite->position.x,caster->sprite->position.y);

		if(!scbw::checkUnitCollisionPos(target,&(caster->sprite->position),&new_pos,NULL,false,0))
			//collision detected, cancel the movement
			scbw::setUnitPosition(target,previous_pos.x,previous_pos.y);
		else {

			CThingy* recall_effect_sprite;

			//finish the teleporting (and cut the recalled unit from a possible unit targeted by an order)
			scbw::prepareUnitMove(target,false);
			target->orderTarget.unit = NULL;
			scbw::setUnitPosition(target,new_pos.x,new_pos.y);
			scbw::refreshUnitAfterMove(target);

			//stop some currently active orders
			if(target->id != UnitId::ZergCocoon)
				target->orderComputerCL(units_dat::ReturnToIdleOrder[target->id]);
			
			//create the Recall animation over the unit
			recall_effect_sprite = createThingy(SpriteId::Recall_Field,new_pos.x,new_pos.y,0);
			if(recall_effect_sprite != NULL) {
				recall_effect_sprite->sprite->elevationLevel = target->sprite->elevationLevel + 1;
				setThingyVisibilityFlags(recall_effect_sprite);
			}

			//disconnect a nuke from a ghost using it
			if(
				target->id == UnitId::TerranGhost ||
				target->id == UnitId::Hero_SarahKerrigan ||
				target->id == UnitId::Hero_AlexeiStukov ||
				target->id == UnitId::Hero_SamirDuran ||
				target->id == UnitId::Hero_InfestedDuran
			) 
			{

				if(
					target->connectedUnit != NULL &&
					target->connectedUnit->id == UnitId::TerranNuclearMissile
				) 
				{
					(target->connectedUnit)->connectedUnit = NULL;
					target->connectedUnit = NULL;
				}

			}

			*bAtLeast1UnitWasRecalled = 1;

		}

	} //void spells_Recall_ActionOnValidTargetForRecall(CUnit* target, CUnit* caster)

	;

	//Originally known as sub_494400
	//Function performed on each target of Recall
	//by IterateUnitsAtLocationTargetProc_RecallTargets
	//to choose if the unit can have the Recall effect
	//cast on it or no, and if yes, calling another function
	//to do the Recall effect.
	void spells_Recall_ActionOnTargetInRange(CUnit* target, CUnit* caster) {

		if(
			!(target->sprite->flags & CSprite_Flags::Hidden) &&
			target->playerId == caster->playerId &&
			!(units_dat::BaseProperty[target->id] & UnitProperty::Building) &&
			!(target->status & UnitStatus::Invincible) &&
			!(target->status & UnitStatus::Burrowed) &&
			target->id != UnitId::ZergLarva &&
			target->id != UnitId::ZergEgg &&
			target->id != UnitId::ZergLurkerEgg &&
			!(target->status & UnitStatus::IsHallucination) &&
			target != caster
		)
			spells_Recall_ActionOnValidTargetForRecall(target,caster);

	}

	;

	void orders_Recall(CUnit* unit) {

		static Bool32* const bAtLeast1UnitWasRecalled = (Bool32*)(0x0063FF50);

		//unit has started casting Recall (== went through the other condition)
		if(unit->mainOrderState == 1) {

			//execute the recall effect when timer reach 0
			if(unit->mainOrderTimer == 0) {

				Box16 spell_location;

				spell_location.left = unit->orderTarget.pt.x - 64;
				spell_location.top = unit->orderTarget.pt.y - 64;
				spell_location.right = unit->orderTarget.pt.x + 64;
				spell_location.bottom = unit->orderTarget.pt.y + 64;

				*bAtLeast1UnitWasRecalled = 0;

				//apply the recal effect on each valid target found in spell_location
				IterateUnitsAtLocationTargetProc_RecallTargets(unit,&spell_location);

				if(*bAtLeast1UnitWasRecalled != 0) {

					//original code added (RandomizeShort(0x12) & 1) to the base sound
					//id Protoss_ARBITER_PAbFol02_WAV then called the playSound function
					//using that value, but since there are only 2 sounds, it's easier to 
					//see what sound is used that way.
					//The compiler may make it back like the original anyway.

					if((RandomizeShort(0x12) & 1) == 0)
						scbw::playSound(SoundId::Protoss_ARBITER_PAbFol02_WAV,unit);
					else
						scbw::playSound(SoundId::Protoss_ARBITER_PAbFol03_WAV,unit);
					
				}

				unit->orderToIdle();

			}

		}
		else { //94505

			if(
				!scbw::isCheatEnabled(CheatFlags::TheGathering) &&
				unit->energy < techdata_dat::EnergyCost[TechId::Recall] * 256
			) 
			{

				u8 raceId;
				char* message;

				if(units_dat::GroupFlags[unit->id].isZerg)
					raceId = RaceId::Zerg;
				else
				if(units_dat::GroupFlags[unit->id].isTerran)
					raceId = RaceId::Terran;
				else
				if(units_dat::GroupFlags[unit->id].isProtoss)
					raceId = RaceId::Protoss;
				else
					raceId = RaceId::Neutral;

				message = (char*)statTxtTbl->getString(864 + raceId);	//"Not enough energy" text

				if(
					message != NULL &&
					unit->playerId == *LOCAL_NATION_ID &&
					*ACTIVE_PLAYER_ID == *LOCAL_HUMAN_ID
				)
					setUnitStatTxtErrorMsg(message);

				unit->orderToIdle();

			}
			else { //945B9

				CThingy* recall_effect_sprite;
				u32 random_value;

				if(!scbw::isCheatEnabled(CheatFlags::TheGathering))
					unit->energy -= techdata_dat::EnergyCost[TechId::Recall] * 256;

				if(unit->orderTarget.unit != NULL) {
					unit->orderTarget.pt.x = unit->orderTarget.unit->sprite->position.x;
					unit->orderTarget.pt.y = unit->orderTarget.unit->sprite->position.y;
				}

				recall_effect_sprite =
					createThingy(
						SpriteId::Recall_Field,
						unit->orderTarget.pt.x,
						unit->orderTarget.pt.y,
						0
						);

				if(recall_effect_sprite != NULL) {
					recall_effect_sprite->sprite->elevationLevel = unit->sprite->elevationLevel + 1;
					setThingyVisibilityFlags(recall_effect_sprite);
				}

				//94617
				if(*IS_IN_GAME_LOOP) {

					//hardcoded (not using a function) RNG use, like
					//the original code do

					static u32* const u32_0051C654 = (u32*)0x0051C654;
					static u32* const u32_0051CA18 = (u32*)0x0051CA18;

					*u32_0051C654 = *u32_0051C654 + 1;
					*u32_0051CA18 = *u32_0051CA18 + 1;
					*lastRandomNumber = (*lastRandomNumber * 0x015A4E35) + 1;

					random_value = ((*lastRandomNumber) / 65536) & 0x7FFF;

				}
				else
					random_value = 0;

				//original code added (random_value & 1) to the base sound
				//id Protoss_ARBITER_PAbFol00_WAV then called the playSound function
				//using that value, but since there are only 2 sounds, it's easier to 
				//see what sound is used that way.
				//The compiler may make it back like the original anyway.

				if((random_value & 1) == 0)
					playUnitSFX(
						SoundId::Protoss_ARBITER_PAbFol00_WAV,
						unit->orderTarget.pt.x,
						unit->orderTarget.pt.y,
						1,
						0
					);
				else
					playUnitSFX(
						SoundId::Protoss_ARBITER_PAbFol01_WAV,
						unit->orderTarget.pt.x,
						unit->orderTarget.pt.y,
						1,
						0
					);

				unit->mainOrderTimer = 22;
				unit->mainOrderState = 1;

			}

		}

	} //void orders_Recall(CUnit* unit)

	;

} //namespace hooks

;

//-------- Helper function definitions. Do NOT modify! --------//

namespace {

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

	const u32 Func_playUnitSFX = 0x0048EC10;
	void playUnitSFX(u32 soundId, u16 x, u16 y, u32 unk1, u32 unk2) {

		static Point16 pos;

		pos.x = x;pos.y = y;

		__asm {
			PUSHAD
			PUSH unk2
			PUSH unk1
			PUSH pos
			MOV EBX, soundId
			CALL Func_playUnitSFX
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

	const u32 Func_IterateUnitsAtLocationTargetProc = 0x004E8280;
	//hardcoding the recall targets selection function
	u32 IterateUnitsAtLocationTargetProc_RecallTargets(CUnit* unit, Box16* coords) {

		static u32 return_value;

		__asm {
			PUSHAD
			PUSH unit
			MOV EAX, coords
			MOV EBX, 0x00494400
			CALL Func_IterateUnitsAtLocationTargetProc
			MOV return_value, EAX
			POPAD
		}

		return return_value;

	}

	;

	const u32 Func_setUnitStatTxtErrorMsg = 0x0048CCB0;
	void setUnitStatTxtErrorMsg(char* message) {

		__asm {
			PUSHAD
			MOV EAX, message
			CALL Func_setUnitStatTxtErrorMsg
			POPAD
		}

	}

	;

	const u32 Func_Sub4EBAE0 = 0x004EBAE0;
	void function_004EBAE0(CUnit* unit, s32 x, s32 y) {

		__asm {
			PUSHAD
			MOV EDX, unit
			MOV EAX, x
			MOV ECX, y
			CALL Func_Sub4EBAE0
			POPAD
		}

	}

	;

} //Unnamed namespace

//End of helper functions

#include "cast_order.h"
#include <SCBW/api.h>

//helper functions def

namespace {

u32 getUpgradedWpnCooldown(CUnit* unit, u8 weaponId);				//75DC0
bool function_00476640(CUnit* unit, u32 weaponId);					//76640
void setUnitStatTxtErrorMsg(char* message);							//8CCB0
u32 get_statTxt_Str_0(CUnit* unit, u32 playerId, char* message);	//8EF30
bool canCastSpell_0(CUnit* unit);									//92140
u32 function_00492620(CUnit* unit, Bool32 wasMainOrderStateZero);	//92620
u32 RandomizeShort(u32 unknown_index);								//DC4A0
void setNextWaypoint_Sub4EB290(CUnit* unit);						//EB290

} //unnamed namespace

namespace hooks {

//Used by: 
//OrderId::FireYamatoGun1, OrderId::MagnaPulse (lockdown),
//OrderId::DarkSwarm, OrderId::CastParasite, OrderId::SummonBroodlings,
//OrderId::EmpShockwave, OrderId::PsiStorm, OrderId::Irradiate,
//OrderId::Plague, OrderId::Consume, OrderId::Ensnare,
//OrderId::StasisField, OrderId::Restoration, OrderId::CastDisruptionWeb,
//OrderId::CastOpticalFlare, OrderId::CastMaelstrom	
void orders_Spell(CUnit* unit) {

	s32 spellCost;
	u8 orderWeaponId = orders_dat::OrderWeaponId[unit->mainOrderId];
	CUnit* target = unit->orderTarget.unit;

	if(unit->mainOrderState == 2) {

		if(unit->orderSignal & 2) {
			unit->orderSignal -= 2;
			unit->orderToIdle();
		}
		else { //92886

			if(target != NULL) {

				unit->orderTarget.pt.x = target->sprite->position.x;
				unit->orderTarget.pt.y = target->sprite->position.y;

				if(
					target->sprite->position.x != unit->nextTargetWaypoint.x ||
					target->sprite->position.y != unit->nextTargetWaypoint.y
				)
				{ //928C1
					unit->nextTargetWaypoint.x = target->sprite->position.x;
					unit->nextTargetWaypoint.y = target->sprite->position.y;
				}

			}

		}

	}
	else { //928CF

		if(!canCastSpell_0(unit)) {

			char* message = (char*)statTxtTbl->getString(876); //Invalid Target
			
			//do the actual transmission corresponding to the message
			get_statTxt_Str_0(unit,unit->playerId,message);

			//make the unit stop since the order failed
			setNextWaypoint_Sub4EB290(unit);
			unit->orderToIdle();

		}
		else { //9291A

			u8 techUsed = orders_dat::TechUsed[unit->mainOrderId];

			if(techUsed >= TechId::None)
				spellCost = 0;
			else
				spellCost = techdata_dat::EnergyCost[techUsed] * 256;

			if(
				!(*CHEAT_STATE & CheatFlags::TheGathering) &&
				unit->energy < spellCost
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

				message = (char*)statTxtTbl->getString(864 + raceId); //Not Enough Energy

				if(message != NULL) {

					//display an error message only for the owner
					//of the unit
					if(	*LOCAL_NATION_ID == unit->playerId &&
						*ACTIVE_PLAYER_ID == *LOCAL_HUMAN_ID
					)
						setUnitStatTxtErrorMsg(message);

				}

				unit->orderToIdle();

			}
			else { //929C2

				u32 result_00492620;

				//may initiate movement from unit depending on
				//mainOrderState
				if(unit->mainOrderState == 0)
					result_00492620 = function_00492620(unit,1);
				else
					result_00492620 = function_00492620(unit,0);

				if(unit->mainOrderState == 0)
					unit->mainOrderState = 1;

				if(result_00492620 == 0) {

					if(unit->getMovableState() == MovableState::UnmovableAtDestination)
						unit->orderToIdle();

				}
				else { //92A01

					if(
						unit->spellCooldown == 0 &&
						function_00476640(unit,orderWeaponId) && //is ready to fire weapon (angle,range...) maybe?
						!(unit->movementFlags & MovementFlags::Accelerating) &&
						unit->getMovableState() != MovableState::NotReachedDestination
					) 
					{ //92A38

						bool bStopHere = false;

						//in case it was modified by a previous function,
						//relinking
						target = unit->orderTarget.unit;

						if(unit->pAI != NULL && target != NULL) {
							
							if(orderWeaponId <= WeaponId::Maelstrom) { //Maelstrom is the last spell-related weapon

								//array bound to the function
								const u8* const array_00492B44 = (u8*)(0x00492B44);

								//this code would not work for weapons id of NuclearMissile, YamatoGun 
								//and weapons id before those, they must be excluded somewhere else
								//Maybe the switch and array use could be removed in a future version
								//since enough elements use default value (switch 6)
								u32 switch_value = array_00492B44[orderWeaponId - 0x20];

								if(switch_value != 6) {

									//no idea why irradiate is so important
									u32 analyzed_target_status = target->irradiateTimer;

									if(switch_value == 2)
										analyzed_target_status |= target->parasiteFlags;

									if(switch_value == 3)
										analyzed_target_status |= target->ensnareTimer;

									if(switch_value == 4)
										analyzed_target_status |= target->plagueTimer;

									if(switch_value == 5)
										analyzed_target_status |= target->isBlind;

									if(switch_value == 0 && unit->isFrozen())
										analyzed_target_status |= 1;

									if(analyzed_target_status != 0) {
										unit->orderToIdle();
										bStopHere = true;
									}

								}

							} //if(orderWeaponId <= WeaponId::Maelstrom)

						} //if(unit->pAI != NULL && target != NULL)

						if(!bStopHere) { //92AA3

							u8 cooldown;
							u32 random_value;

							unit->spendUnitEnergy(spellCost);
							cooldown = getUpgradedWpnCooldown(unit,orderWeaponId);

							//a complicated way to add between -1 and 2 to the cooldown
							random_value = RandomizeShort(49);
							cooldown += ( ((u8)random_value) & 3 ) - 1;

							unit->spellCooldown = cooldown;

							//disable flag 2 if enabled, else has no effect
							unit->orderSignal &= ~2;	//&= -3

							unit->movementFlags |= MovementFlags::StartingAttack;

							if(unit->sprite->images.head != NULL) {

								CImage* current_image = unit->sprite->images.head;
								IscriptAnimation::Enum anim = (IscriptAnimation::Enum)orders_dat::OrderIscriptAnim[unit->mainOrderId];

								while(current_image != NULL) {
									current_image->playIscriptAnim(anim);
									current_image = current_image->link.next;
								}

							}

							unit->mainOrderState = 2;

							if(unit->pAI != NULL)
								AIScriptController[unit->playerId].spellcasterTimer = 1;

						}

					}

				} //if(result_00492620 != 0)

			} //got enough energy

		} //canCastSpell_0(unit)

	} //unit->mainOrderState != 2

} //void orders_Spell(CUnit* unit)

;

} //namespace hooks

//-------- Helper function definitions. Do NOT modify! --------//

namespace {

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

const u32 Func_Sub476640 = 0x00476640;
bool function_00476640(CUnit* unit, u32 weaponId) {

	static Bool32 pre_result;

	__asm {
		PUSHAD
		PUSH weaponId
		MOV EAX, unit
		CALL Func_Sub476640
		MOV pre_result, EAX
		POPAD
	}

	return (pre_result != 0);

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

const u32 Func_get_statTxt_Str_0 = 0x0048EF30;
u32 get_statTxt_Str_0(CUnit* unit, u32 playerId, char* message) {

	static u32 return_value;

	__asm {
		PUSHAD
		MOV EAX, message
		MOV ECX, playerId
		MOV EDX, unit
		CALL Func_get_statTxt_Str_0
		MOV EAX, return_value
		POPAD
	}

	return return_value;

}

;

const u32 Func_canCastSpell_0 = 0x00492140;
bool canCastSpell_0(CUnit* unit) {

	static Bool32 bPreResult;

	__asm {
		PUSHAD
		MOV EDI, unit
		CALL Func_canCastSpell_0
		MOV bPreResult, EAX
		POPAD
	}

	//0 means "can cast spell"
	return (bPreResult == 0);

}

;

const u32 Func_Sub492620 = 0x00492620;
u32 function_00492620(CUnit* unit, Bool32 wasMainOrderStateZero) {

	static u32 return_value;

	__asm {
		PUSHAD
		PUSH wasMainOrderStateZero
		MOV EAX, unit
		CALL Func_Sub492620
		MOV return_value, EAX
		POPAD
	}

	return return_value;

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

//Related to path/movement decision
const u32 Func_sub_4EB290 = 0x004EB290;
void setNextWaypoint_Sub4EB290(CUnit* unit) {

	__asm {
		PUSHAD
		MOV EAX, unit
		CALL Func_sub_4EB290
		POPAD
	}
}


;

} //Unnamed namespace

//End of helper functions

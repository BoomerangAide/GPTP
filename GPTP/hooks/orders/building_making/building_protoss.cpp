#include "building_protoss.h"
#include <SCBW/api.h>

//helper functions def

namespace {

bool isDistanceGreaterThanHaltDistance(CUnit* unit, int x, int y, u32 distance);		//01240
void function_0042D9A0(CUnit* unit);													//2D9A0
void AI_TrainingOverlord(CUnit* main_unit, CUnit* created_unit);						//35770
bool function_00467030(CUnit* unit);													//67030
CUnit* function_004678A0(CUnit* unit, u32 unitId);										//678A0
void createBottomOverlay(CSprite* sprite, u32 imageId, s32 x, s32 y, u32 direction);	//98D70
void replaceSpriteImages(CSprite* sprite, u32 imageId, u32 imageDirection);				//99BB0
void displayLastNetErrForPlayer(u32 playerId);											//9E530
void updateUnitStrength(CUnit* unit);													//9FA40
void function_004A01F0(CUnit* unit);													//A01F0
CUnit* createUnit(u32 unitId, int x, int y, u32 playerId);								//A09D0
void iscript_OpcodeCases(CImage* image, u32 offset_, u32 unk1, u32 unk2);				//D74C0
void function_004D8500(CImage* image);													//D8500
void makeToHoldPosition_Helper(CUnit* unit);											//EB5B0
bool function_004EB9C0(CUnit* unit, int x, int y);										//EB9C0

} //unnamed namespace

namespace hooks {

	void orders_ProbeBuild(CUnit* unit) {

		if(unit->pAI != NULL)
			unit->status |= UnitStatus::IsGathering;

		if(unit->mainOrderState == 0) {
			if(function_004EB9C0(unit,unit->orderTarget.pt.x,unit->orderTarget.pt.y))
				unit->mainOrderState = 1;
		}
		else
		if(unit->mainOrderState == 1) {

			u32 distance,distance_wanted;

			distance = scbw::getDistanceFast(unit->halt.x,unit->halt.y,unit->orderTarget.pt.x * 256,unit->orderTarget.pt.y * 256);

			if(unit->buildQueue[unit->buildQueueSlot] == UnitId::ProtossAssimilator)
				distance_wanted = 88;
			else
				distance_wanted = 70;

			if(distance / 256 > distance_wanted) {
				
				if(unit->getMovableState() != MovableState::NotReachedDestination)
					function_004EB9C0(unit,unit->orderTarget.pt.x,unit->orderTarget.pt.y);

			}
			else { //E4DA9
				makeToHoldPosition_Helper(unit);
				unit->mainOrderState = 2;
			}

		}
		else
		if(unit->mainOrderState == 2) {

			if(!(unit->movementFlags & MovementFlags::Accelerating)) {

				u32 distance_wanted;
				bool isBuildingAssimilator = (unit->buildQueue[unit->buildQueueSlot] == UnitId::ProtossAssimilator);

				if(isBuildingAssimilator)
					distance_wanted = 88;
				else
					distance_wanted = 70;

				if(!isDistanceGreaterThanHaltDistance(unit, unit->orderTarget.pt.x, unit->orderTarget.pt.y, distance_wanted)) {

					if(unit->getMovableState() != MovableState::NotReachedDestination)
						function_004EB9C0(unit,unit->orderTarget.pt.x,unit->orderTarget.pt.y);

				}
				else {

					if(function_00467030(unit)) {

						CUnit* builtUnit;

						if(isBuildingAssimilator)
							builtUnit = function_004678A0(unit,UnitId::ProtossAssimilator);
						else
							builtUnit = createUnit(
											unit->buildQueue[unit->buildQueueSlot],
											unit->orderTarget.pt.x,
											unit->orderTarget.pt.y,
											unit->playerId
							);

						unit->buildQueue[unit->buildQueueSlot] = UnitId::None;

						if(builtUnit == NULL) {
							displayLastNetErrForPlayer(unit->playerId);
							unit->orderToIdle();
						}
						else { //E4E63

							replaceSpriteImages(builtUnit->sprite,ImageId::WarpAnchor,0);

							if(isBuildingAssimilator)
								createBottomOverlay(builtUnit->sprite,ImageId::VespeneGeyser,0,0,0);

							unit->orderTarget.unit = builtUnit;

							AI_TrainingOverlord(unit,builtUnit);

							builtUnit->orderComputerCL(OrderId::BuildSelf2);

							scbw::playSound(SoundId::Protoss_SHUTTLE_pshBld00_wav,unit);
							unit->mainOrderState = 3;

						}

					}
					else
						unit->orderToIdle();

				}

			}

		}
		else
		if(unit->mainOrderState == 3) {

			CUnit* target = unit->orderTarget.unit;
			CImage* current_image;

			if(	target == NULL ||
				target->id != UnitId::ProtossAssimilator ||
				unit->orderQueueHead != NULL
			)
				unit->orderToIdle();
			else
				unit->orderTo(OrderId::HarvestGas2,target);

			current_image = unit->sprite->images.head;

			while(current_image != NULL) {
				current_image->playIscriptAnim(IscriptAnimation::WalkingToIdle);
				current_image = current_image->link.next;
			}

		}

	}

	;

	//Protoss building self-building
	void orders_BuildSelf2(CUnit* building) {

		bool jump_to_E50EE = false;

		if(building->mainOrderState > 3)
			jump_to_E50EE = true;
		else
		if(building->mainOrderState == 0) {

			if(building->remainingBuildTime == 0) {

				CImage* current_image = building->sprite->images.head;

				while(current_image != NULL) {

					if(current_image->flags & CImage_Flags::Flag04) {

						const u32* u32_006D1200 = (u32*)0x006D1200;

						current_image->animation = IscriptAnimation::SpecialState1;
						current_image->iscriptOffset = *(u16*)(*u32_006D1200 + current_image->iscriptHeaderOffset + 0x22);
						current_image->wait = 0;
						current_image->unknown14 = 0;
						iscript_OpcodeCases(current_image,(u32)&current_image->iscriptHeaderOffset,0,0);

					}

					current_image = current_image->link.next;

				}

				scbw::playSound(SoundId::Protoss_SHUTTLE_pshBld03_wav,building);
				building->mainOrderState = 1;
				jump_to_E50EE = true;

			}
			else
				jump_to_E50EE = true;

		}
		else
		if(building->mainOrderState == 1) {

			if(building->orderSignal & 1) {

				building->orderSignal -= 1;

				replaceSpriteImages(
					building->sprite,
					sprites_dat::ImageId[building->sprite->spriteId],
					0
				);

				function_004D8500(building->sprite->mainGraphic);

				building->mainOrderState = 2;

			}

		}
		else
		if(building->mainOrderState == 2) {

			if(building->orderSignal & 1) {

				CImage* current_image;

				building->orderSignal -= 1;

				replaceSpriteImages(
					building->sprite,
					sprites_dat::ImageId[building->sprite->spriteId],
					0
				);

				current_image = building->sprite->images.head;

				while(current_image != NULL) {

					if(current_image->flags & CImage_Flags::Flag04) {

						const u32* u32_006D1200 = (u32*)0x006D1200;

						current_image->animation = IscriptAnimation::WarpIn;
						current_image->iscriptOffset = *(u16*)(*u32_006D1200 + current_image->iscriptHeaderOffset + 0x32);
						current_image->wait = 0;
						current_image->unknown14 = 0;
						iscript_OpcodeCases(current_image,(u32)&current_image->iscriptHeaderOffset,0,0);

					}

					current_image = current_image->link.next;

				}

				building->mainOrderState = 3;

			}

		}
		else
		if(building->mainOrderState == 3) {

			if(building->orderSignal & 1) {

				building->orderSignal -= 1;

				function_004A01F0(building);
				updateUnitStrength(building);
				function_0042D9A0(building);

				if(building->status & UnitStatus::DoodadStatesThing) {

					CImage* current_image = building->sprite->images.head;

					while(current_image != NULL) {

						if(current_image->flags & CImage_Flags::Flag04) {

							const u32* u32_006D1200 = (u32*)0x006D1200;

							current_image->animation = IscriptAnimation::Disable;
							current_image->iscriptOffset = *(u16*)(*u32_006D1200 + current_image->iscriptHeaderOffset + 0x38);
							current_image->wait = 0;
							current_image->unknown14 = 0;
							iscript_OpcodeCases(current_image,(u32)&current_image->iscriptHeaderOffset,0,0);

						}

						current_image = current_image->link.next;

					}

					jump_to_E50EE = true;

				}
				else
					jump_to_E50EE = true;

			}

		}
		
		if(jump_to_E50EE) {

			u32 hpGain, shieldGain;

			if(building->remainingBuildTime != 0) {
				if(*CHEAT_STATE & CheatFlags::OperationCwal) {
					if(building->remainingBuildTime > 16)
						building->remainingBuildTime -= 16;
					else
						building->remainingBuildTime = 0;
				}
				else
					building->remainingBuildTime--;
			}

			if(*CHEAT_STATE & CheatFlags::OperationCwal)
				hpGain = building->buildRepairHpGain * 16;
			else
				hpGain = building->buildRepairHpGain;

			building->setHp(building->hitPoints + hpGain);

			if(*CHEAT_STATE & CheatFlags::OperationCwal)
				shieldGain = building->shieldGain * 16;
			else
				shieldGain = building->shieldGain;

			if(building->shields + shieldGain >= (u32)units_dat::MaxShieldPoints[building->id] * 256)
				building->shields = units_dat::MaxShieldPoints[building->id] * 256;
			else
				building->shields += shieldGain;

		}

	}

	;

} //namespace hooks

;

//-------- Helper function definitions. Do NOT modify! --------//

namespace {

const u32 Func_isDistanceGreaterThanHaltDistance = 0x00401240;
bool isDistanceGreaterThanHaltDistance(CUnit* unit, int x, int y, u32 distance) {

	static Bool32 bPreResult;

	__asm {
		PUSHAD
		PUSH x
		PUSH distance
		MOV EAX, y
		MOV ECX, unit
		CALL Func_isDistanceGreaterThanHaltDistance
		MOV bPreResult, EAX
		POPAD
	}

	return (bPreResult != 0);

}

;

const u32 Func_Sub42D9A0 = 0x0042D9A0;
void function_0042D9A0(CUnit* unit) {

	__asm {
		PUSHAD
		MOV EAX, unit
		CALL Func_Sub42D9A0
		POPAD
	}

}

;

const u32 Func_AI_TrainingOverlord = 0x00435770;
void AI_TrainingOverlord(CUnit* main_unit, CUnit* created_unit) {

	__asm {
		PUSHAD
		MOV EAX, created_unit
		MOV ECX, main_unit
		CALL Func_AI_TrainingOverlord
		POPAD
	}

}

;

const u32 Func_Sub467030 = 0x00467030;
bool function_00467030(CUnit* unit) {

	static Bool32 bPreResult;

	__asm {
		PUSHAD
		MOV EAX, unit
		CALL Func_Sub467030
		MOV bPreResult, EAX
		POPAD
	}

	return (bPreResult != 0);

}

;

const u32 Func_Sub4678A0 = 0x004678A0;
CUnit* function_004678A0(CUnit* unit, u32 unitId) {

	static CUnit* newUnit;

	__asm {
		PUSHAD
		MOV EAX, unit
		PUSH unitId
		CALL Func_Sub4678A0
		MOV newUnit, EAX
		POPAD
	}

	return newUnit;

}

;

const u32 Func_Sub498D70 = 0x00498D70;
void createBottomOverlay(CSprite* sprite, u32 imageId, s32 x, s32 y, u32 direction) {

	__asm {
		PUSHAD
		MOV EAX, sprite
		MOV ESI, imageId
		PUSH direction
		PUSH y
		PUSH x
		CALL Func_Sub498D70
		POPAD
	}

}

;

const u32 Func_Sub499BB0 = 0x00499BB0;
void replaceSpriteImages(CSprite *sprite, u32 imageId, u32 imageDirection) {
	__asm {
		PUSHAD
		PUSH imageDirection
		PUSH imageId
		MOV EAX, sprite
		CALL Func_Sub499BB0
		POPAD
	}
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

const u32 Func_iscript_OpcodeCases = 0x004D74C0;
void iscript_OpcodeCases(CImage* image, u32 offset_, u32 unk1, u32 unk2) {

	__asm {
		PUSHAD
		PUSH unk1
		PUSH unk2
		PUSH offset_
		MOV ECX, image
		CALL Func_iscript_OpcodeCases
		POPAD
	}

}

;

const u32 Func_Sub4D8500 = 0x004D8500;
void function_004D8500(CImage* image) {

	__asm {
		PUSHAD
		MOV EAX, image
		CALL Func_Sub4D8500
		POPAD
	}

}

;

const u32 Func_CreateUnit = 0x004A09D0;
CUnit* createUnit(u32 unitId, int x, int y, u32 playerId) {

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

const u32 Func_OrdersHoldPositionSuicidal = 0x004EB5B0;
//Hooked in hooks\orders\base_orders\stopholdpos_orders
void makeToHoldPosition_Helper(CUnit* unit) {

	__asm {
		PUSHAD
		MOV ESI, unit
		CALL Func_OrdersHoldPositionSuicidal
		POPAD
	}

}

;

const u32 Func_Sub4EB9C0 = 0x004EB9C0;
bool function_004EB9C0(CUnit* unit, int x, int y) {

	static Bool32 bPreResult;

	__asm {
		PUSHAD
		MOV EDX, unit
		MOV ECX, x
		MOV EAX, y
		CALL Func_Sub4EB9C0
		MOV bPreResult, EAX
		POPAD
	}

	return (bPreResult != 0);

}

;

} //Unnamed namespace

//End of helper functions

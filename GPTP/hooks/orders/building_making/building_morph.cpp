#include "building_morph.h"
#include <SCBW/api.h>
#include <Events/Events.h>

//helper functions def

namespace {

void refundBuildingCost(u16 unitId, u8 playerId);					//2CE70
void function_00433FE0(CUnit* unit);								//33FE0
bool isValidMorph(u16 buildingId);									//5CC60
void updateNewUnitVision(u16 unitId, int x, int y);					//5CE90
void zergPlaceBuildingCntd(CUnit* unit);							//5D410
void function_0047D770(CUnit* unit);								//7D770
void function_0047DE40(CSprite* sprite, u32 unitId, int x, int y);	//7DE40
bool setThingyVisibilityFlags(CThingy* thingy);						//878F0
CThingy* createThingy(u32 spriteId, s16 x, s16 y, u32 playerId);	//88210
void incrementUnitScoresEx(CUnit* building, s32 unk1, s32 unk2);	//88D50
void playBuildingCompleteSound(CUnit* building);					//8F310
void refreshSpriteData(CSprite* sprite);							//983A0
void displayLastNetErrForPlayer(u32 playerId);						//9E530
void updateUnitStrength(CUnit* unit);								//9FA40
void replaceUnitWithType(CUnit* unit, u16 newUnitId);				//9FED0
void function_004A01F0(CUnit* unit);								//A01F0
CUnit* createUnit(u32 unitId, int x, int y, u32 playerId);			//A09D0

} //unnamed namespace

namespace hooks {

bool isMorphing(CUnit* building) {

	bool bIsMorphing;

	if(!(building->status & UnitStatus::Completed))
			//was hardcoded instead of using isValidMorph in original code
		bIsMorphing = isValidMorph(building->buildQueue[building->buildQueueSlot]);
	else
		bIsMorphing = false;

	return bIsMorphing;

}

;

#ifdef EVENTS_SYSTEM
void orders_ZergBuildSelf(CUnit* building) {

	const int OPCWAL_PROGRESS_INCREASE = 16;

	u16 buildingId;
	bool jump_to_5D56C = false;
	bool jump_to_5D718 = false;
	bool jump_to_5D720 = false;

	buildingId = building->buildQueue[building->buildQueueSlot];

	if(
		buildingId != UnitId::ZergHive &&
		buildingId != UnitId::ZergLair &&
		buildingId != UnitId::ZergGreaterSpire &&
		buildingId != UnitId::ZergSporeColony &&
		buildingId != UnitId::ZergSunkenColony
	)
		buildingId = building->id;

	if(building->mainOrderState == 0) {

		if(building->remainingBuildTime < (units_dat::TimeCost[buildingId] * 3 / 4))
			building->mainOrderState = 1;

		jump_to_5D56C = true;

	}
	else
	if(building->mainOrderState > 6)
		jump_to_5D56C = true;

	if(jump_to_5D56C) { //5D56C

		jump_to_5D56C = false;

		if(building->remainingBuildTime == 0)
			jump_to_5D720 = true;
		else
		if(!scbw::isCheatEnabled(CheatFlags::OperationCwal))
			jump_to_5D718 = true;
		else {

			if(building->remainingBuildTime >= OPCWAL_PROGRESS_INCREASE)
				building->remainingBuildTime -= OPCWAL_PROGRESS_INCREASE;
			else
				building->remainingBuildTime = 0;

			jump_to_5D720 = true;

		}

	}
	else
	if(building->mainOrderState == 1) { //5D5AE

		if(building->id == UnitId::ZergExtractor) {

			CImage* overlay = building->sprite->getOverlay(ImageId::VespeneGeyser2);

			if(overlay != NULL)
				overlay->free();

		}

		//5D5D0
		building->sprite->playIscriptAnim(IscriptAnimation::SpecialState1,true);
		building->mainOrderState = 2;

		jump_to_5D56C = true;

	}
	else
	if(building->mainOrderState == 2) { //5D5E2

		if(building->remainingBuildTime < units_dat::TimeCost[buildingId] / 2)
			building->mainOrderState = 3;

		jump_to_5D56C = true;

	}
	else
	if(building->mainOrderState == 3) { //5D603

		building->sprite->playIscriptAnim(IscriptAnimation::SpecialState2,true);
		building->mainOrderState = 4;

		jump_to_5D56C = true;

	}
	else
	if(building->mainOrderState == 4) { //5D618

		if(building->remainingBuildTime == 0)
			building->mainOrderState = 5;

		jump_to_5D56C = true;

	}
	else
	if(building->mainOrderState == 5) { //5D62F

		building->sprite->playIscriptAnim(IscriptAnimation::AlmostBuilt,true);
		building->mainOrderState = 6;
		playBuildingCompleteSound(building);

		jump_to_5D56C = true;

	}
	else
	if(building->mainOrderState == 6) { //5D64B

		if(building->orderSignal & 4) {

			s32 hpAfterMorph;
			CImage* current_image;

			building->orderSignal -= 4;

			if(isValidMorph(buildingId)) {

				std::vector<int*> events_building_morph_hp_set_arg(3);
				s32 oldHp = building->hitPoints;

				events_building_morph_hp_set_arg[0] = (int*)building;
				events_building_morph_hp_set_arg[1] = (int*)oldHp;

				//update score
				building->status |= UnitStatus::Completed;
				incrementUnitScoresEx(building,-1,0);

				//do the building change
				building->status -= UnitStatus::Completed;
				replaceUnitWithType(building,buildingId);

				//newHp = newHpMax - oldHpMax + oldHp
				hpAfterMorph =
					units_dat::MaxHitPoints[buildingId] -
					units_dat::MaxHitPoints[building->previousUnitType] +
					oldHp;

				if(hpAfterMorph < 256)
					hpAfterMorph = 256;

				events_building_morph_hp_set_arg[2] = (int*)&hpAfterMorph;

				EventManager::EventCalled(EventId::BUILDING_MORPH_HP_SET, events_building_morph_hp_set_arg);

				building->setHp(hpAfterMorph);
				building->remainingBuildTime = 0;

			}

			//5d6cd

			//update various stuff (set hp, set shield...) not finished on Morph
			function_004A01F0(building);
			updateUnitStrength(building);

			//relate to building placement / taking room around?
			function_0047D770(building);

			current_image = building->sprite->images.head;

			while(current_image != NULL) {
				current_image->playIscriptAnim(IscriptAnimation::AlmostBuilt);
				current_image = current_image->link.next;
			}

			updateNewUnitVision(buildingId, building->sprite->position.x, building->sprite->position.y);

		}

	}

	if(jump_to_5D56C) { //5D56C

		jump_to_5D56C = false;

		if(building->remainingBuildTime == 0)
			jump_to_5D720 = true;
		else
		if(!scbw::isCheatEnabled(CheatFlags::OperationCwal))
			jump_to_5D718 = true;
		else {

			if(building->remainingBuildTime >= OPCWAL_PROGRESS_INCREASE)
				building->remainingBuildTime -= OPCWAL_PROGRESS_INCREASE;
			else
				building->remainingBuildTime = 0;

			jump_to_5D720 = true;

		}

	}

	if(jump_to_5D718) {
		jump_to_5D718 = false;
		building->remainingBuildTime--;
		jump_to_5D720 = true;
	}

	if(jump_to_5D720) {

		jump_to_5D720 = false;

		if(
			buildingId != UnitId::ZergHive &&
			buildingId != UnitId::ZergLair &&
			buildingId != UnitId::ZergGreaterSpire &&
			buildingId != UnitId::ZergSporeColony &&
			buildingId != UnitId::ZergSunkenColony
		)
		{

			std::vector<int*> events_building_morph_hp_add_arg(3);
			s32 newHp;

			if(scbw::isCheatEnabled(CheatFlags::OperationCwal))
				newHp = building->hitPoints + building->buildRepairHpGain * OPCWAL_PROGRESS_INCREASE;
			else
				newHp = building->hitPoints + building->buildRepairHpGain;

			events_building_morph_hp_add_arg[0] = (int*)building;
			events_building_morph_hp_add_arg[1] = (int*)building->hitPoints;
			events_building_morph_hp_add_arg[2] = (int*)&newHp;

			EventManager::EventCalled(EventId::BUILDING_MORPH_HP_ADD, events_building_morph_hp_add_arg);

			building->setHp(newHp);

		}

	}

} //void orders_ZergBuildSelf(CUnit* building)
#else
void orders_ZergBuildSelf(CUnit* building) {

	const int OPCWAL_PROGRESS_INCREASE = 16;

	u16 buildingId;
	bool jump_to_5D56C = false;
	bool jump_to_5D718 = false;
	bool jump_to_5D720 = false;

	buildingId = building->buildQueue[building->buildQueueSlot];

	if(
		buildingId != UnitId::ZergHive &&
		buildingId != UnitId::ZergLair &&
		buildingId != UnitId::ZergGreaterSpire &&
		buildingId != UnitId::ZergSporeColony &&
		buildingId != UnitId::ZergSunkenColony
	)
		buildingId = building->id;

	if(building->mainOrderState == 0) {

		if(building->remainingBuildTime < (units_dat::TimeCost[buildingId] * 3 / 4))
			building->mainOrderState = 1;

		jump_to_5D56C = true;

	}
	else
	if(building->mainOrderState > 6)
		jump_to_5D56C = true;

	if(jump_to_5D56C) { //5D56C

		jump_to_5D56C = false;

		if(building->remainingBuildTime == 0)
			jump_to_5D720 = true;
		else
		if(!scbw::isCheatEnabled(CheatFlags::OperationCwal))
			jump_to_5D718 = true;
		else {

			if(building->remainingBuildTime >= OPCWAL_PROGRESS_INCREASE)
				building->remainingBuildTime -= OPCWAL_PROGRESS_INCREASE;
			else
				building->remainingBuildTime = 0;

			jump_to_5D720 = true;

		}

	}
	else
	if(building->mainOrderState == 1) { //5D5AE

		if(building->id == UnitId::ZergExtractor) {

			CImage* overlay = building->sprite->getOverlay(ImageId::VespeneGeyser2);

			if(overlay != NULL)
				overlay->free();

		}

		//5D5D0
		building->sprite->playIscriptAnim(IscriptAnimation::SpecialState1,true);
		building->mainOrderState = 2;

		jump_to_5D56C = true;

	}
	else
	if(building->mainOrderState == 2) { //5D5E2

		if(building->remainingBuildTime < units_dat::TimeCost[buildingId] / 2)
			building->mainOrderState = 3;

		jump_to_5D56C = true;

	}
	else
	if(building->mainOrderState == 3) { //5D603

		building->sprite->playIscriptAnim(IscriptAnimation::SpecialState2,true);
		building->mainOrderState = 4;

		jump_to_5D56C = true;

	}
	else
	if(building->mainOrderState == 4) { //5D618

		if(building->remainingBuildTime == 0)
			building->mainOrderState = 5;

		jump_to_5D56C = true;

	}
	else
	if(building->mainOrderState == 5) { //5D62F

		building->sprite->playIscriptAnim(IscriptAnimation::AlmostBuilt,true);
		building->mainOrderState = 6;
		playBuildingCompleteSound(building);

		jump_to_5D56C = true;

	}
	else
	if(building->mainOrderState == 6) { //5D64B

		if(building->orderSignal & 4) {

			s32 hpAfterMorph;
			CImage* current_image;

			building->orderSignal -= 4;

			if(isValidMorph(buildingId)) {

				//save hp before some functions modify them
				hpAfterMorph = building->hitPoints;

				//update score
				building->status |= UnitStatus::Completed;
				incrementUnitScoresEx(building,-1,0);

				//do the building change
				building->status -= UnitStatus::Completed;
				replaceUnitWithType(building,buildingId);

				//newHp = newHpMax - oldHpMax + oldHp
				hpAfterMorph =
					units_dat::MaxHitPoints[buildingId] -
					units_dat::MaxHitPoints[building->previousUnitType] +
					hpAfterMorph;

				if(hpAfterMorph < 256)
					hpAfterMorph = 256;

				building->setHp(hpAfterMorph);
				building->remainingBuildTime = 0;

			}

			//5d6cd

			//update various stuff (set hp, set shield...) not finished on Morph
			function_004A01F0(building);
			updateUnitStrength(building);

			//relate to building placement / taking room around?
			function_0047D770(building);

			current_image = building->sprite->images.head;

			while(current_image != NULL) {
				current_image->playIscriptAnim(IscriptAnimation::AlmostBuilt);
				current_image = current_image->link.next;
			}

			updateNewUnitVision(buildingId, building->sprite->position.x, building->sprite->position.y);

		}

	}

	if(jump_to_5D56C) { //5D56C

		jump_to_5D56C = false;

		if(building->remainingBuildTime == 0)
			jump_to_5D720 = true;
		else
		if(!scbw::isCheatEnabled(CheatFlags::OperationCwal))
			jump_to_5D718 = true;
		else {

			if(building->remainingBuildTime >= OPCWAL_PROGRESS_INCREASE)
				building->remainingBuildTime -= OPCWAL_PROGRESS_INCREASE;
			else
				building->remainingBuildTime = 0;

			jump_to_5D720 = true;

		}

	}

	if(jump_to_5D718) {
		jump_to_5D718 = false;
		building->remainingBuildTime--;
		jump_to_5D720 = true;
	}

	if(jump_to_5D720) {

		jump_to_5D720 = false;

		if(
			buildingId != UnitId::ZergHive &&
			buildingId != UnitId::ZergLair &&
			buildingId != UnitId::ZergGreaterSpire &&
			buildingId != UnitId::ZergSporeColony &&
			buildingId != UnitId::ZergSunkenColony
		)
		{

			s32 hpGain = building->buildRepairHpGain;

			if(scbw::isCheatEnabled(CheatFlags::OperationCwal))
				hpGain *= OPCWAL_PROGRESS_INCREASE;

			building->setHp(building->hitPoints + hpGain);

		}

	}

} //void orders_ZergBuildSelf(CUnit* building)
#endif

;

#ifdef EVENTS_SYSTEM
void ZergPlaceBuilding(CUnit* unit) {

	bool bStopThere = false;

	if(!(unit->status & UnitStatus::Completed)) {

		u16 builtUnitId = unit->buildQueue[unit->buildQueueSlot];

		//original code was hardcoded instead of using isValidMorph
		if(isValidMorph(builtUnitId)) {
			zergPlaceBuildingCntd(unit);
			bStopThere = true;
		}

	}

	if(!bStopThere) {

		static u32* const scoreUnitsOwned = (u32*)	0x00581E44;
		static u32* const scoreUnitsTotal = (u32*)	0x00581ED4;

		std::vector<int*> events_unit_drone_hp_set_arg(5);
		u32 oldId;
		s32 oldHp, newHp;

		oldHp = unit->hitPoints;
		oldId = unit->id;

		refundBuildingCost(unit->id,unit->playerId);

		scoreUnitsTotal[unit->playerId] = scoreUnitsTotal[unit->playerId] - units_dat::BuildScore[UnitId::ZergDrone];
		scoreUnitsOwned[unit->playerId] = scoreUnitsOwned[unit->playerId] - 1;

		if(unit->id == UnitId::ZergExtractor) {

			CUnit* drone = createUnit(UnitId::ZergDrone,unit->getX(),unit->getY(),unit->playerId);

			if(drone == NULL) {
				displayLastNetErrForPlayer(unit->playerId);
				unit->remove();
			}
			else {	//5DB1E

				//update various stuff (set hp, set shield...) not finished after createUnit
				function_004A01F0(drone);
				updateUnitStrength(drone);

				newHp = unit->previousHp * 256;

				events_unit_drone_hp_set_arg[0] = (int*)unit;
				events_unit_drone_hp_set_arg[1] = (int*)drone;
				events_unit_drone_hp_set_arg[2] = (int*)oldId;
				events_unit_drone_hp_set_arg[3] = (int*)oldHp;
				events_unit_drone_hp_set_arg[4] = (int*)&newHp;

				EventManager::EventCalled(EventId::BUILDING_DRONE_SPAWNED_FROM_CANCELED_BUILDING_HP_SET, events_unit_drone_hp_set_arg);

				drone->setHp(newHp);

				unit->remove();

			}

		}
		else {	//5DB45

			CThingy* thingy = createThingy(SpriteId::Zerg_Building_Spawn_Small,unit->getX(),unit->getY(),0);

			if(thingy != NULL) {
				thingy->sprite->elevationLevel = unit->sprite->elevationLevel + 1;
				setThingyVisibilityFlags(thingy);
			}

			scbw::playSound(SoundId::Misc_ZBldgPlc_wav_1,unit);
			replaceUnitWithType(unit,UnitId::ZergDrone);

			//probably AI related
			function_00433FE0(unit);

			//update various stuff (set hp, set shield...) not finished after unit change
			function_004A01F0(unit);
			updateUnitStrength(unit);

			if(
				unit->sprite->images.tail->paletteType == PaletteType::RLE_SHADOW &&
				unit->sprite->images.tail->verticalOffset != 7
			)
			{
				unit->sprite->images.tail->flags |= CImage_Flags::Redraw;
				unit->sprite->images.tail->verticalOffset = 7;
			}

			refreshSpriteData(unit->sprite);

			unit->orderComputerCL(OrderId::ResetCollision1);

			unit->order(
				units_dat::ReturnToIdleOrder[unit->id],
				0,
				0,
				NULL,
				UnitId::None,
				false
			);

			newHp = unit->previousHp * 256;

			events_unit_drone_hp_set_arg[0] = (int*)unit;
			events_unit_drone_hp_set_arg[1] = (int*)unit;
			events_unit_drone_hp_set_arg[2] = (int*)oldId;
			events_unit_drone_hp_set_arg[3] = (int*)oldHp;
			events_unit_drone_hp_set_arg[4] = (int*)&newHp;

			EventManager::EventCalled(EventId::BUILDING_DRONE_SPAWNED_FROM_CANCELED_BUILDING_HP_SET, events_unit_drone_hp_set_arg);

			unit->setHp(unit->previousHp * 256);

			//???
			function_0047DE40(
				unit->sprite,
				unit->id,
				unit->getX(),
				unit->getY()
			);


		}


	}

}
#else
void ZergPlaceBuilding(CUnit* unit) {

	bool bStopThere = false;

	if(!(unit->status & UnitStatus::Completed)) {

		u16 builtUnitId = unit->buildQueue[unit->buildQueueSlot];

		//original code was hardcoded instead of using isValidMorph
		if(isValidMorph(builtUnitId)) {
			zergPlaceBuildingCntd(unit);
			bStopThere = true;
		}

	}

	if(!bStopThere) {

		static u32* const scoreUnitsOwned = (u32*)	0x00581E44;
		static u32* const scoreUnitsTotal = (u32*)	0x00581ED4;

		refundBuildingCost(unit->id,unit->playerId);

		scoreUnitsTotal[unit->playerId] = scoreUnitsTotal[unit->playerId] - units_dat::BuildScore[UnitId::ZergDrone];
		scoreUnitsOwned[unit->playerId] = scoreUnitsOwned[unit->playerId] - 1;

		if(unit->id == UnitId::ZergExtractor) {

			CUnit* drone = createUnit(UnitId::ZergDrone,unit->getX(),unit->getY(),unit->playerId);

			if(drone == NULL) {
				displayLastNetErrForPlayer(unit->playerId);
				unit->remove();
			}
			else {	//5DB1E

				//update various stuff (set hp, set shield...) not finished after createUnit
				function_004A01F0(drone);
				updateUnitStrength(drone);

				drone->setHp(unit->previousHp * 256);

				unit->remove();

			}

		}
		else {	//5DB45

			CThingy* thingy = createThingy(SpriteId::Zerg_Building_Spawn_Small,unit->getX(),unit->getY(),0);

			if(thingy != NULL) {
				thingy->sprite->elevationLevel = unit->sprite->elevationLevel + 1;
				setThingyVisibilityFlags(thingy);
			}

			scbw::playSound(SoundId::Misc_ZBldgPlc_wav_1,unit);
			replaceUnitWithType(unit,UnitId::ZergDrone);

			//probably AI related
			function_00433FE0(unit);

			//update various stuff (set hp, set shield...) not finished after unit change
			function_004A01F0(unit);
			updateUnitStrength(unit);

			if(
				unit->sprite->images.tail->paletteType == PaletteType::RLE_SHADOW &&
				unit->sprite->images.tail->verticalOffset != 7
			)
			{
				unit->sprite->images.tail->flags |= CImage_Flags::Redraw;
				unit->sprite->images.tail->verticalOffset = 7;
			}

			refreshSpriteData(unit->sprite);

			unit->orderComputerCL(OrderId::ResetCollision1);

			unit->order(
				units_dat::ReturnToIdleOrder[unit->id],
				0,
				0,
				NULL,
				UnitId::None,
				false
			);

			unit->setHp(unit->previousHp * 256);

			//???
			function_0047DE40(
				unit->sprite,
				unit->id,
				unit->getX(),
				unit->getY()
			);


		}


	}

}
#endif

;

} //namespace hooks

;

//-------- Helper function definitions. Do NOT modify! --------//

namespace {

const u32 Func_refundBuildingCost = 0x0042CE70;
void refundBuildingCost(u16 unitId, u8 playerId) {

	__asm {
		PUSHAD
		MOV AX, unitId
		MOV CL, playerId
		CALL Func_refundBuildingCost
		POPAD
	}

}

;

const u32 Func_Sub433FE0 = 0x00433FE0;
void function_00433FE0(CUnit* unit) {

	__asm {
		PUSHAD
		MOV EAX, unit
		CALL Func_Sub433FE0
		POPAD
	}

}

;

//Identical to isValidMorph @ 0x0045CC60;
bool isValidMorph(u16 buildingId) {

	bool result;

	if(
		buildingId == UnitId::ZergHive ||
		buildingId == UnitId::ZergLair ||
		buildingId == UnitId::ZergGreaterSpire ||
		buildingId == UnitId::ZergSporeColony ||
		buildingId == UnitId::ZergSunkenColony
	)
		result = true;
	else
		result = false;

	return result;

}

;

const u32 Func_updateNewUnitVision = 0x0045CE90;
void updateNewUnitVision(u16 unitId, int x, int y) {

	__asm {
		PUSHAD
		MOVZX ECX, unitId
		PUSH y
		PUSH x
		CALL Func_updateNewUnitVision
		POPAD
	}

}

;

const u32 Func_ZergPlaceBuildingCntd = 0x0045D410;
void zergPlaceBuildingCntd(CUnit* unit) {

	__asm {
		PUSHAD
		MOV EAX, unit
		CALL Func_ZergPlaceBuildingCntd
		POPAD
	}

}

;

const u32 Func_Sub47D770 = 0x0047D770;
void function_0047D770(CUnit* unit) {

	__asm {
		PUSHAD
		PUSH unit
		CALL Func_Sub47D770
		POPAD
	}

}

;

const u32 Func_Sub47DE40 = 0x0047DE40;
void function_0047DE40(CSprite* sprite, u32 unitId, int x, int y) {

	__asm {
		PUSHAD
		MOV ECX, y
		MOV EDX, x
		MOV ESI, sprite
		PUSH unitId
		CALL Func_Sub47DE40
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

const u32 Func_incrementUnitScoresEx = 0x00488D50;
//unk1 is ECX, unk2 is pushed value
void incrementUnitScoresEx(CUnit* building, s32 unk1, s32 unk2) {

	__asm {
		PUSHAD
		MOV EDI, building
		MOV ECX, unk1
		PUSH unk2
		CALL Func_incrementUnitScoresEx
		POPAD
	}

}

;

const u32 Func_PlayBuildingCompleteSound = 0x0048F310;
void playBuildingCompleteSound(CUnit* building) {

	__asm {
		PUSHAD
		MOV EAX, building
		CALL Func_PlayBuildingCompleteSound
		POPAD
	}

}

;

const u32 Func_refreshSpriteData = 0x004983A0;
void refreshSpriteData(CSprite* sprite) {

	__asm {
		PUSHAD
		MOV EAX, sprite
		CALL Func_refreshSpriteData
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

const u32 Func_ReplaceUnitWithType = 0x0049FED0;
void replaceUnitWithType(CUnit* unit, u16 newUnitId) {

	u32 newUnitId_ = newUnitId;

  __asm {
    PUSHAD
    PUSH newUnitId_
    MOV EAX, unit
    CALL Func_ReplaceUnitWithType
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

} //Unnamed namespace

//End of helper functions

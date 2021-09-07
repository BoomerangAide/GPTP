#include "create_init_units.h"
#include <SCBW/api.h>

//helper functions def

namespace {

Bool32 spreadsCreep(u32 unitId, Bool32 defaultReturn);					//13870
void function_004148F0(int x, int y, u32 unitId);						//148F0
void CreateBuildingLarva(CUnit* main_building);							//9D660
Bool32 function_0049EC30(CUnit* unit);									//9EC30
void updateUnitStrength(CUnit* unit);									//9FA40
void function_004A01F0(CUnit* unit);									//A01F0
CUnit* createUnit(u32 unitId, int x, int y, u32 playerId);				//A09D0
CUnit* FindBestUnit(Box16* coords, u32 searchProc, CBullet* bullet);	//E8830
	
} //unnamed namespace

namespace hooks {

void CreateInitialMeleeBuildings(u8 raceId, u32 playerId) {

	const u32 Func_Sub49D640 = 0x0049D640;

	u8 buildingId;
	s32 buildingSizeX, buildingSizeY;
	Box16 searchBox; 
	CUnit* created_unit;

	if(raceId == RaceId::Zerg)
		buildingId = UnitId::ZergHatchery;
	else
	if(raceId == RaceId::Terran)
		buildingId = UnitId::TerranCommandCenter;
	else
	if(raceId == RaceId::Protoss)
		buildingId = UnitId::ProtossNexus;
	else
		buildingId = UnitId::None;

	buildingSizeX = (s16)units_dat::BuildingDimensions[buildingId].x;
	buildingSizeY = (s16)units_dat::BuildingDimensions[buildingId].y;

	if(buildingSizeX < 0)
		buildingSizeX++;
	if(buildingSizeY < 0)
		buildingSizeY++;

	searchBox.left = (startPositions[playerId].x - (s16)(buildingSizeX / 2)) & 0x0000FFE0;
	searchBox.top = (startPositions[playerId].y - (s16)(buildingSizeY / 2)) & 0x0000FFE0;
	searchBox.bottom = buildingSizeY / 2 + ((startPositions[playerId].y - (s16)(buildingSizeY / 2)) & 0x0000FFE0)  + buildingSizeY / 2 - 1;
	searchBox.right = buildingSizeX / 2 + ((startPositions[playerId].x - (s16)(buildingSizeX / 2)) & 0x0000FFE0) + buildingSizeX / 2 - 1;

	//proc an effect on each valid target, without caring about returning a value
	FindBestUnit(&searchBox,Func_Sub49D640, NULL);

	created_unit = 
		createUnit(
			buildingId,
			((startPositions[playerId].x - (buildingSizeX / 2)) & 0x0000FFE0) + buildingSizeX / 2,
			((startPositions[playerId].y - (buildingSizeY / 2)) & 0x0000FFE0)  + buildingSizeY / 2,
			playerId
		);

	if(created_unit != NULL) {

		function_004A01F0(created_unit);

		if(function_0049EC30(created_unit) != 0)
			updateUnitStrength(created_unit);

		if(	spreadsCreep(created_unit->id,1) ||
			units_dat::BaseProperty[created_unit->id] & UnitProperty::CreepBuilding
		)
		{
			function_004148F0(
				created_unit->sprite->position.x,
				created_unit->sprite->position.y,
				created_unit->id
			);
			CreateBuildingLarva(created_unit);
			CreateBuildingLarva(created_unit);
		}

	}

}

;

void CreateInitialMeleeUnits() {

	u32 playerId;
	u8* raceIdOffset;

	playerId = 8;
	raceIdOffset = (u8*)0x0057F009;

	while(raceIdOffset != (u8*)0x0057EEE9) {

		raceIdOffset -= 0x24;
		playerId--;

		if(*(raceIdOffset-1) == 2 || *(raceIdOffset-1) == 1) {

			//0 is use map settings,1 is Workers, 2 is Workers + Main Building
			u8 startingUnits = (u8)(*(u16*)(0x00596871));

			static u8* victory_conditions = (u8*)0x0059686D;
			static u8* tournament_mode = (u8*)0x00596877;
			static u8* user_select_slots = (u8*)0x0059BDA8;

			if(
				*victory_conditions == 0 && //check triggers for conditions
				startingUnits == 0 &&		//use map units
				*tournament_mode == 0 &&	//tournament mode off
				playerId < 8 &&
				(user_select_slots[playerId]) != 0	//race selected instead of using map settings?
			)
				startingUnits = 2;

			if(startingUnits == 2) {

				CreateInitialMeleeBuildings(*raceIdOffset,playerId);

				if(*raceIdOffset == RaceId::Zerg)
					CreateInitialOverlord(playerId);

			}

			if(startingUnits == 1 || startingUnits == 2) {

				u8 unitId;
				CUnit* created_unit;

				if(*raceIdOffset == RaceId::Zerg)
					unitId = UnitId::ZergDrone;
				else
				if(*raceIdOffset == RaceId::Terran)
					unitId = UnitId::TerranSCV;
				else
				if(*raceIdOffset == RaceId::Protoss)
					unitId = UnitId::ProtossProbe;
				else
					unitId = UnitId::None;

				created_unit = createUnit(unitId,startPositions[playerId].x,startPositions[playerId].y,playerId);

				if(created_unit != NULL) {

					function_004A01F0(created_unit);

					if(function_0049EC30(created_unit) != 0)
						updateUnitStrength(created_unit);

				}

				if(*raceIdOffset == RaceId::Zerg)
					unitId = UnitId::ZergDrone;
				else
				if(*raceIdOffset == RaceId::Terran)
					unitId = UnitId::TerranSCV;
				else
				if(*raceIdOffset == RaceId::Protoss)
					unitId = UnitId::ProtossProbe;
				else
					unitId = UnitId::None;

				created_unit = createUnit(unitId,startPositions[playerId].x,startPositions[playerId].y,playerId);

				if(created_unit != NULL) {

					function_004A01F0(created_unit);

					if(function_0049EC30(created_unit) != 0)
						updateUnitStrength(created_unit);

				}

				if(*raceIdOffset == RaceId::Zerg)
					unitId = UnitId::ZergDrone;
				else
				if(*raceIdOffset == RaceId::Terran)
					unitId = UnitId::TerranSCV;
				else
				if(*raceIdOffset == RaceId::Protoss)
					unitId = UnitId::ProtossProbe;
				else
					unitId = UnitId::None;

				created_unit = createUnit(unitId,startPositions[playerId].x,startPositions[playerId].y,playerId);

				if(created_unit != NULL) {

					function_004A01F0(created_unit);

					if(function_0049EC30(created_unit) != 0)
						updateUnitStrength(created_unit);

				}

				if(*raceIdOffset == RaceId::Zerg)
					unitId = UnitId::ZergDrone;
				else
				if(*raceIdOffset == RaceId::Terran)
					unitId = UnitId::TerranSCV;
				else
				if(*raceIdOffset == RaceId::Protoss)
					unitId = UnitId::ProtossProbe;
				else
					unitId = UnitId::None;

				created_unit = createUnit(unitId,startPositions[playerId].x,startPositions[playerId].y,playerId);

				if(created_unit != NULL) {

					function_004A01F0(created_unit);

					if(function_0049EC30(created_unit) != 0)
						updateUnitStrength(created_unit);

				}

			}

		}

	}

}

;

void CreateInitialOverlord(u8 playerId) {

	static u16* const maxBoxRightValue =	(u16*)		0x00628450;
	static u16* const maxBoxBottomValue =	(u16*)		0x006284B4;

	int overlordStartX, overlordStartY;
	CUnit* created_unit;

	if(startPositions[playerId].x < *maxBoxRightValue / 2)
		overlordStartX = startPositions[playerId].x + units_dat::UnitBounds[UnitId::ZergHatchery].right + units_dat::UnitBounds[UnitId::ZergHatchery].left + 1;
	else
		overlordStartX = startPositions[playerId].x - (units_dat::UnitBounds[UnitId::ZergHatchery].right + units_dat::UnitBounds[UnitId::ZergHatchery].left + 1);

	if(startPositions[playerId].y < *maxBoxBottomValue / 2)
		overlordStartY = startPositions[playerId].y + units_dat::UnitBounds[UnitId::ZergHatchery].bottom + units_dat::UnitBounds[UnitId::ZergHatchery].top + 1;
	else
		overlordStartY = startPositions[playerId].y - (units_dat::UnitBounds[UnitId::ZergHatchery].bottom + units_dat::UnitBounds[UnitId::ZergHatchery].top + 1);

	created_unit = createUnit(UnitId::ZergOverlord,overlordStartX,overlordStartY,playerId);

	if(created_unit != NULL) {

		function_004A01F0(created_unit);

		if(function_0049EC30(created_unit) != 0)
			updateUnitStrength(created_unit);

	}

}

;

} //namespace hooks

;

//-------- Helper function definitions. Do NOT modify! --------//

namespace {

const u32 Func_spreadsCreep = 0x00413870;
Bool32 spreadsCreep(u32 unitId, Bool32 defaultReturn) {

	Bool32 bReturnValue;

	__asm {
		PUSHAD
		MOV EAX, unitId
		PUSH defaultReturn
		CALL Func_spreadsCreep
		MOV bReturnValue, EAX
		POPAD
	}

	return bReturnValue;

}

;

const u32 Func_Sub4148F0 = 0x004148F0;
void function_004148F0(int x, int y, u32 unitId) {

	__asm {
		PUSHAD
		PUSH y
		PUSH x
		PUSH unitId
		CALL Func_Sub4148F0
		POPAD
	}

}

;

const u32 Func_Sub49D660 = 0x0049D660;
void CreateBuildingLarva(CUnit* main_building) {

	__asm {
		PUSHAD
		MOV ESI, main_building
		CALL Func_Sub49D660
		POPAD
	}

}

;

const u32 Func_Sub49EC30 = 0x0049EC30;
Bool32 function_0049EC30(CUnit* unit) {

	Bool32 bResult;

	__asm {
		PUSHAD
		MOV ESI, unit
		CALL Func_Sub49EC30
		MOV bResult, EAX
		POPAD
	}

	return bResult;

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

} //Unnamed namespace

//End of helper functions

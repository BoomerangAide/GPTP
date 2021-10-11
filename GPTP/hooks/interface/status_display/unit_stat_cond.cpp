#include "unit_stat_cond.h"
#include <SCBW/api.h>

//Helper functions declaration

namespace {

bool unitIsTrainingOrMorphing(CUnit* unit);	//01500
bool function_00424780_helper();			//24780
bool UnitStatCond_Standard_helper();		//24980
bool function_00424A10_helper();			//24A10
bool UnitStatCond_Dropship_helper();		//24F10
bool function_00425230_helper();			//25230

} //unnamed namespace

StoredUnit* const selectionLoadedUnitvalues = (StoredUnit*)0x006CA78C; //array of 8 struct elements

const u32 selectionVariableData1Offset = 0x006CA93C;		//Since the data type can vary, only the offset is global variable
const u32 selectionVariableData2Offset = 0x006CA93E;		//Since the data type can vary, only the offset is global variable

s32* const selectionHPvalues		= (s32*)	0x006CA94C;	//array of SELECTION_ARRAY_LENGTH elements
s32* const selectionShieldvalue		= (s32*)	0x006CA9EC; //pointer on a single element (not array)
Bool8* const selectionUpdateNeeded	= (Bool8*)	0x006CA9F0;
u16* const selectionEnergyvalue		= (u16*)	0x006CAC0C; //pointer on a single element
u16* const selectionIDvalues		= (u16*)	0x006CAD7C;	//array of SELECTION_ARRAY_LENGTH elements

namespace hooks {

//0x00424500
//Possibly used by units turrets, nukes, spells...
//Probably Status Function 1 in FireGraft
bool UnitStatCond_Never() {
	return false;
}

;

//0x00424520
//Possibly used by spider mines, powerups, Floor armed traps...
//Probably Status Function 9 in FireGraft
bool UnitStatCond_Powerup() {
	return ( (*activePortraitUnit)->hitPoints != selectionHPvalues[0] );
}

;

//0x00424980
//Used by most units, sometimes directly after the checks
//made by other functions
//Probably Status Function 2 in FireGraft
bool UnitStatCond_Standard() {

	bool return_value;
	CUnit* unit = *activePortraitUnit;

	if(
		unit->hitPoints != selectionHPvalues[0] ||
		unit->shields / 256 != *selectionShieldvalue
		//displayed shields are actual shields divided by 256 and
		//selectionShieldvalue seems to store the displayed value
	)
		return_value = true;
	else
	if(*IS_IN_REPLAY == 0 && unit->playerId != *LOCAL_NATION_ID)
		return_value = false;
	else
	if(unit->energy != *selectionEnergyvalue)
		return_value = true;
	else
		return_value = function_00424780_helper();

	return return_value;
	
}

;

//0x00424AC0
//Used by ZergEgg,ZergCocoon and ZergLurkerEgg.
//Probably Status Function 7 in FireGraft
bool UnitStatCond_Egg() {
	
	bool return_value;

	if(*IS_IN_REPLAY != 0 || (*activePortraitUnit)->playerId == *LOCAL_NATION_ID)
		return_value = true;
	else
		return_value = UnitStatCond_Standard_helper();

	return return_value;

}

;

//0x00424AF0
//Used by ProtossDarkArchon and ProtossArchon (not heroic Archon)
//Probably Status Function 6 in FireGraft
bool UnitStatCond_Archon() {

	bool return_value;

	if((*activePortraitUnit)->mainOrderId == OrderId::CompletingArchonSummon)
		return_value = true;
	else
		return_value = UnitStatCond_Standard_helper();

	return return_value;
	
}

;

//0x00424B50
//Used by Carrier,Reaver, and their heroic versions
//Probably Status Function 5 in FireGraft
bool UnitStatCond_CarrierReaver() {

	bool return_value;

	if(
		( *IS_IN_REPLAY != 0 || (*activePortraitUnit)->playerId == *LOCAL_NATION_ID ) &&
		(*activePortraitUnit)->buildQueue[(*activePortraitUnit)->buildQueueSlot % 5] <= UnitId::Spell_DisruptionWeb
		//Buildings ids come after UnitId::Spell_DisruptionWeb
	)
		return_value = true;
	else
		return_value = UnitStatCond_Standard_helper();

	return return_value;
	
}

;

//0x00424F10
//Used (directly) by TerranDropship,ProtossShuttle and TerranBunker
//Logically equivalent to original function
//Probably Status Function 4 in FireGraft
bool UnitStatCond_Dropship() {

	const CUnit* unitTable_0059CB58 = (CUnit*)(0x0059CB58);	//array of CUnit structures

	bool return_value = false;
	CUnit* transport = *activePortraitUnit;

	if(*IS_IN_REPLAY == 0 && transport->playerId != *LOCAL_NATION_ID)
		return_value = UnitStatCond_Standard_helper();
	else {

		for(int i = 0; i < 8 && !return_value; i++) {

			if(selectionLoadedUnitvalues[i].fullValue != transport->loadedUnit[i].fullValue)
				return_value = true;
			else
			if(selectionLoadedUnitvalues[i].fullValue != 0) {
				
				CUnit* loaded_unit = (CUnit*)&unitTable_0059CB58[transport->loadedUnit[i].innerValues.index];

				if(
					loaded_unit->sprite != NULL &&
					(
						loaded_unit->mainOrderId != OrderId::Die || 
						loaded_unit->mainOrderState != 1
					) &&
					loaded_unit->targetOrderSpecial == selectionLoadedUnitvalues[i].innerValues.unitId &&
					loaded_unit->hitPoints != selectionHPvalues[i+1]
					 //i+1 because the transport is at 0 then come the loaded units
				)
					return_value = true;

			}
			
		}

		if(!return_value && *selectionUpdateNeeded != 0)
			return_value = true;
		else
			return_value = UnitStatCond_Standard_helper();

	}

	return return_value;
	
}

;

//0x00425180
//Used by buildings and maybe others...
//Probably Status Function 3 in FireGraft
bool UnitStatCond_Building() {

	bool return_value;
	CUnit* building = *activePortraitUnit;

	if(
		building->hitPoints != selectionHPvalues[0] ||
		building->shields / 256 != *selectionShieldvalue
		//displayed shields are actual shields divided by 256 and
		//selectionShieldvalue seems to store the displayed value
	)
		return_value = true;
	else
	if(*IS_IN_REPLAY == 0 && building->playerId != *LOCAL_NATION_ID)
		return_value = false;
	else
	if(
		building->energy / 256 != *selectionEnergyvalue ||
		function_00424A10_helper() ||
		function_00424780_helper()
	)
		return_value = true;
	else
		return_value = *selectionUpdateNeeded != 0;

	return return_value;
	
}

;

//0x00425900
//Used by Overlord/Yggdrasill
//Probably Status Function 8 in FireGraft
bool UnitStatCond_overlord() {

	bool return_value;
	CUnit* unit = *activePortraitUnit;

	if(
		unit->hitPoints != selectionHPvalues[0] ||
		unit->shields / 256 != *selectionShieldvalue
		//displayed shields are actual shields divided by 256 and
		//selectionShieldvalue seems to store the displayed value
	)
		return_value = true;
	else
	if(unit->hasLoadedUnit())
		return_value = UnitStatCond_Dropship_helper();
	else
	if(
		units_dat::SupplyProvided[unit->id] == 0 ||
		!function_00425230_helper()
	)
		return_value = false;
	else
		return_value = true;

	return return_value;
	
}

;

//Needed because called by UnitStatCond_Standard
//and required to handle all cases properly
bool function_00424780() {
	
	bool return_value = false;
	CUnit* unit = *activePortraitUnit;

	//note: those checks on units with hangars are to handle
	//hallucinated versions of those units created by the
	//map editor (thus possibly having things in hangar unlike
	//the normal hallucinations)

	if(
		unit->id == UnitId::ProtossCarrier ||
		unit->id == UnitId::Hero_Gantrithor
	)
		return_value = 
			*(u8*)selectionVariableData1Offset != 
			(unit->carrier.inHangarCount + unit->carrier.outHangarCount);
	else
	if(
		unit->id == UnitId::ProtossReaver ||
		unit->id == UnitId::Hero_Warbringer
	)
		return_value = 
			*(u8*)selectionVariableData1Offset != unit->carrier.inHangarCount;

	if(
		unit->status & UnitStatus::IsHallucination ||
		unit->id == UnitId::ZergBroodling
	)
	{

		if(!return_value) {

			int timer;

			if((s16)unit->removeTimer < 0)
				timer = (unit->removeTimer / 10) + 1;
			else
				timer = unit->removeTimer / 10;

			return_value = ( *(u16*)selectionVariableData2Offset != timer );

		}

	}
	else
	if(units_dat::SupplyProvided[unit->id] != 0)
	{
		
		u8 raceId;
		u8 playerId;
		u32 supplies;

		if(units_dat::GroupFlags[unit->id].isZerg)
			raceId = RaceId::Zerg;
		else
		if(units_dat::GroupFlags[unit->id].isProtoss)
			raceId = RaceId::Protoss;
		else
		if(units_dat::GroupFlags[unit->id].isTerran)
			raceId = RaceId::Terran;
		else
			raceId = RaceId::Neutral;

		playerId = unit->playerId;

		supplies = scbw::getSuppliesAvailable(playerId,raceId);

		if(supplies != *(u16*)selectionVariableData1Offset)
			return_value = true;
		else {
			supplies = scbw::getSuppliesUsed(playerId,raceId);
			return_value = (supplies != *(u16*)selectionVariableData2Offset);
		}

	}
	else
	if(units_dat::BaseProperty[unit->id] & UnitProperty::ResourceContainer)
		return_value = 
			(*(u16*)selectionVariableData1Offset != unit->building.resource.resourceAmount);
	else
		return_value = (*(u32*)selectionVariableData1Offset != 0);

	return return_value;

}

;

//Needed because called by UnitStatCond_Building
//and required to handle all cases properly
bool function_00424A10() {

	bool return_value;
	CUnit* unit = *activePortraitUnit;

	if(*IS_IN_REPLAY == 0 && unit->playerId != *LOCAL_NATION_ID)
		return_value = false;
	else
	if(
		(
			!(unit->status & UnitStatus::IsHallucination) &&
			(
				unit->id != UnitId::ZergOverlord ||
				UpgradesSc->currentLevel[unit->playerId][UpgradeId::VentralSacs] != 0
			)
		) &&
		units_dat::SpaceProvided[unit->id] != 0
	)
		return_value = !(unit->status & UnitStatus::Completed);
	else
	if(!(unit->status & UnitStatus::GroundedBuilding))
		return_value = (
			!(unit->status & UnitStatus::Completed) ||
			unitIsTrainingOrMorphing(unit)
			);
	else
	if(
		!(unit->status & UnitStatus::Completed) ||
		unitIsTrainingOrMorphing(unit) ||
		unit->building.techType != TechId::None ||
		unit->building.upgradeType != UpgradeId::None
	)
		return_value = true;
	else
		return_value = false;

	return return_value;
	
}

;

//Needed because called by UnitStatCond_overlord
//and required to handle all cases properly
bool function_00425230() {

	bool return_value;
	CUnit* unit = *activePortraitUnit;

	if(*IS_IN_REPLAY == 0 && unit->playerId != *LOCAL_NATION_ID)
		return_value = UnitStatCond_Standard_helper();
	else {

		u8 raceId;
		u8 playerId;
		u32 supplies;

		if(units_dat::GroupFlags[unit->id].isZerg)
			raceId = RaceId::Zerg;
		else
		if(units_dat::GroupFlags[unit->id].isProtoss)
			raceId = RaceId::Protoss;
		else
		if(units_dat::GroupFlags[unit->id].isTerran)
			raceId = RaceId::Terran;
		else
			raceId = RaceId::Neutral;

		playerId = unit->playerId;

		supplies = scbw::getSuppliesAvailable(playerId,raceId);

		if(supplies != *(u16*)selectionVariableData1Offset)
			return_value = true;
		else {

			 //was hardcoded instead of function call in original code
			supplies = scbw::getSuppliesUsed(playerId,raceId);

			return_value = (supplies != *(u16*)selectionVariableData2Offset);

		}

	}

	return return_value;
	
}

;

} //namespace hooks

//-------- Helper function definitions. Do NOT modify! --------//

namespace {

//Identical to unitIsTrainingOrMorphing @ 0x00401500
bool unitIsTrainingOrMorphing(CUnit* unit) {

	bool bReturnValue;

	if(unit->buildQueue[unit->buildQueueSlot % 5] != UnitId::None)
		bReturnValue = true;
	else
	if(
		unit->secondaryOrderId == OrderId::BuildAddon &&
		unit->status & UnitStatus::GroundedBuilding &&
		unit->currentBuildUnit != NULL &&
		!(unit->currentBuildUnit->status & UnitStatus::Completed)
	)
		bReturnValue = true;
	else
	if(
		unit->mainOrderId != OrderId::BuildProtoss1 ||
		!(unit->status & UnitStatus::GroundedBuilding) ||
		unit->orderTarget.unit == NULL
	)
		bReturnValue = false;
	else
	if(!(unit->orderTarget.unit->status & UnitStatus::Completed))
		bReturnValue = true;
	else
		bReturnValue = false;

	return bReturnValue;

}

;

const u32 Func_Sub424780 = 0x00424780;
//required by UnitStatCond_Building and UnitStatCond_Standard
bool function_00424780_helper() {
	
	static Bool32 bPreResult;

	__asm {
		PUSHAD
		CALL Func_Sub424780
		MOV bPreResult, EAX
		POPAD
	}

	return (bPreResult != 0);

}

;

const u32 Func_UnitStatCond_Standard = 0x00424980;
//required by most of other Cond functions
bool UnitStatCond_Standard_helper() {
	
	static Bool32 bPreResult;

	__asm {
		PUSHAD
		CALL Func_UnitStatCond_Standard
		MOV bPreResult, EAX
		POPAD
	}

	return (bPreResult != 0);

}
;

const u32 Func_Sub424A10 = 0x00424A10;
//required by UnitStatCond_Building
bool function_00424A10_helper() {
	
	static Bool32 bPreResult;

	__asm {
		PUSHAD
		CALL Func_Sub424A10
		MOV bPreResult, EAX
		POPAD
	}

	return (bPreResult != 0);

}

;

const u32 Func_UnitStatCond_Dropship = 0x00424F10;
//required by UnitStatCond_overlord
bool UnitStatCond_Dropship_helper() {

	static Bool32 bPreResult;

	__asm {
		PUSHAD
		CALL Func_UnitStatCond_Dropship
		MOV bPreResult, EAX
		POPAD
	}

	return (bPreResult != 0);

}

;

const u32 Func_Sub425230 = 0x00425230;
//required by UnitStatCond_overlord
bool function_00425230_helper() {

	static Bool32 bPreResult;

	__asm {
		PUSHAD
		CALL Func_Sub425230
		MOV bPreResult, EAX
		POPAD
	}

	return (bPreResult != 0);

}

;

} //unnamed namespace

//End of helper functions

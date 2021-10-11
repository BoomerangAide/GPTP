#include "larva_creep_spawn.h"
#include <SCBW/api.h>

//Helper functions definitions

namespace {

	u8 function_00414680(u32 unitId, int x, int y);								//14680
	void AI_TrainingOverlord(CUnit* main_building, CUnit* larva);				//35770
	void removeOrderFromUnitQueue(CUnit* unit, COrder* order);					//742D0
	void updateUnitStrength(CUnit* unit);										//9FA40
	void function_004A01F0(CUnit* unit);										//A01F0
	CUnit* CreateUnit(u32 unitId, int x, int y, u32 playerId);					//A09D0
	u32 IterateUnitsAtLocationTargetProc_LarvaCount(CUnit* unit, Box16* coords);//E8280
	void function_004E8DA0(CUnit* unit);										//E8DA0
	bool function_004E8E10(CUnit* unit, Point32* coords);						//E8E10

} //unnamed namespace

;

namespace hooks {

//probably create the single larva accompanying
//a building at game beginning
void function_0049D660(CUnit* main_building) {

	Point32 coords;

	//look for a location to spawn a larva at
	if(function_004E8E10(main_building,&coords)) {

		CUnit* unit_created = CreateUnit(UnitId::ZergLarva, coords.x, coords.y, main_building->playerId);

		if(unit_created != NULL) {

			//update various stuff (set hp, set shield...) not finished on CreateUnit
			function_004A01F0(unit_created);
			
			updateUnitStrength(unit_created);

			unit_created->connectedUnit = main_building;
			
			//set mainOrderState depending on the collision
			//state with the connected building			
			function_004E8DA0(unit_created);

			if(main_building->pAI != NULL)
				AI_TrainingOverlord(main_building,unit_created);

		} //if(unit_created != NULL)

	} //if(function_004E8E10(main_building,&coords))

} //void function_0049D660(CUnit* main_building) 

;

//filter for larva in internal game searches, use the limit of 3 larvas
bool function_004E8C80(CUnit* unit, CUnit* main_building) {

	u16* const LARVA_COUNTER = (u16*)0x0067D3F8;

	//while bReturnValue == false, the calling function keep calling,
	//so it return true only when max amount of larvas was counted,
	//since the counting itself is made with LARVA_COUNTER here and
	//not in the calling function.
	bool bReturnValue;

	if(unit->id == UnitId::ZergLarva) {

		if(unit->sprite->flags & CSprite_Flags::Hidden)
			bReturnValue = false;
		else
		if(unit->connectedUnit != main_building)
			bReturnValue = false;
		else {
			(*LARVA_COUNTER)++;
			bReturnValue = (*LARVA_COUNTER > 3);
		}
		
	}
	else
		bReturnValue = false;

	return bReturnValue;

}

;
	
void orders_StopCreepGrowth(CUnit* unit)
{

	unit->userActionFlags |= 1;

	if (unit->pAI != NULL) {

		if (unit->mainOrderId != OrderId::Die) {

			while (
				unit->orderQueueTail != NULL &&
				(
					orders_dat::CanBeInterrupted[unit->orderQueueTail->orderId] != 0 ||
					unit->orderQueueTail->orderId == OrderId::ComputerAI
				)
			)
				removeOrderFromUnitQueue(unit, unit->orderQueueTail);

			unit->performAnotherOrder(OrderId::ComputerAI, 0, 0, NULL, UnitId::None);

		}

	}
	else
		unit->order(units_dat::ReturnToIdleOrder[unit->id], 0, 0, NULL, UnitId::None, true);

	prepareForNextOrder(unit);

	if (unit->secondaryOrderId != OrderId::Nothing2) {
		unit->secondaryOrderId = OrderId::Nothing2;
		unit->secondaryOrderPos.y = 0;
		unit->secondaryOrderPos.x = 0;
		unit->currentBuildUnit = NULL;
		unit->secondaryOrderState = 0;
	}

}

;

void orders_InitCreepGrowth(CUnit* unit)
{

	unit->userActionFlags |= 1;

	if (unit->pAI != NULL) {

		if (unit->mainOrderId != OrderId::Die) {

			while (
				unit->orderQueueTail != NULL &&
				(
					orders_dat::CanBeInterrupted[unit->orderQueueTail->orderId] != 0 ||
					unit->orderQueueTail->orderId == OrderId::ComputerAI
				)
			)
				removeOrderFromUnitQueue(unit, unit->orderQueueTail);

			unit->performAnotherOrder(OrderId::ComputerAI, 0, 0, NULL, UnitId::None);

		}

	}
	else
		unit->order(units_dat::ReturnToIdleOrder[unit->id], 0, 0, NULL, UnitId::None, true);

	prepareForNextOrder(unit);

	if (unit->secondaryOrderId != OrderId::SpreadCreep) {
		unit->secondaryOrderId = OrderId::SpreadCreep;
		unit->secondaryOrderPos.y = 0;
		unit->secondaryOrderPos.x = 0;
		unit->currentBuildUnit = NULL;
		unit->secondaryOrderState = 0;
	}

}

;

void secondaryOrd_SpawningLarva(CUnit* unit) {

	u16* const LARVA_COUNTER = (u16*)0x0067D3F8;

	if(unit->orderQueueTimer == 0 && unit->playerId != 11) {

		//EA79E

		Box16 coords;

		*LARVA_COUNTER = 0;

		coords.left = unit->sprite->position.x - 256;
		coords.top = unit->sprite->position.y - 256;
		coords.right = unit->sprite->position.x + 256;
		coords.bottom = unit->sprite->position.y + 256;

		//Count existing larvas in LARVA_COUNTER
		IterateUnitsAtLocationTargetProc_LarvaCount(unit,&coords);

		if(*LARVA_COUNTER < 3) {

			if(unit->building.larvaTimer != 0)
				unit->building.larvaTimer--;
			else {

				Point32 coords_2;

				//EA80E:
				unit->building.larvaTimer = 37; /*0x25*/

				//if failed to find a spawn point for larva
				//set the timer before retry to 3
				if(!function_004E8E10(unit,&coords_2))
					unit->building.larvaTimer = 3;
				else {

					//EA830:
					CUnit* new_larva = CreateUnit(UnitId::ZergLarva,coords_2.x,coords_2.y,unit->playerId);

					if(new_larva != NULL) {

						//update various stuff (set hp, set shield...) not finished on CreateUnit
						function_004A01F0(new_larva);
						
						updateUnitStrength(new_larva);

						new_larva->connectedUnit = unit;
						
						//set mainOrderState depending on the collision
						//state with the connected building						
						function_004E8DA0(new_larva);

						if(unit->pAI != NULL)
							AI_TrainingOverlord(unit,new_larva);

					} //if(new_larva != NULL)

				} //if(!function_004E8E10(unit,&coords_2))

			} //if(unit->building.larvaTimer == 0)

		} //if(*LARVA_COUNTER < 3)

	} //if(unit->orderQueueTimer == 0 && unit->playerId != 11)

} //void secondaryOrd_SpawningLarva(CUnit* unit)

;

//Note: this is the function called while performing the OrderId::SpreadCreep
//order.
void secondaryOrd_SpreadCreepSpawningLarva(CUnit* unit) {

	if(
		unit->id == UnitId::ZergHatchery ||
		unit->id == UnitId::ZergLair ||
		unit->id == UnitId::ZergHive
		)
		secondaryOrd_SpawningLarva(unit);

	if(unit->building.creepTimer != 0)
		unit->building.creepTimer--;
	else {

		//EA8B6:

		u8 result_00414680;

		unit->building.creepTimer = 15; /*0F*/
		
		//look for a place to spawn creep on
		//I'm not sure how the return value is
		//calculated 
		//(number of creep tiles/distance/other?)		
		result_00414680 = function_00414680(UnitId::ZergHive,unit->sprite->position.x,unit->sprite->position.y);

		if(result_00414680 == 0) {

			//EA8DC:
			u8 orderId;

			bool isZergMainBuilding =
				unit->id == UnitId::ZergHatchery ||
				unit->id == UnitId::ZergLair ||
				unit->id == UnitId::ZergHive;

			if(isZergMainBuilding)
				orderId = OrderId::SpawningLarva;
			else
				orderId = OrderId::Nothing2;

			if(unit->secondaryOrderId != orderId) {
				unit->secondaryOrderId = orderId;
				unit->secondaryOrderPos.y = 0;
				unit->secondaryOrderPos.x = 0;
				unit->currentBuildUnit = NULL;
				unit->secondaryOrderState = 0;
			}

		}

	}

} //void secondaryOrd_SpreadCreepSpawningLarva(CUnit* unit)

;

} //hooks

//-------- Helper function definitions. Do NOT modify! --------//

namespace {

const u32 Func_Sub414680 = 0x00414680;
u8 function_00414680(u32 unitId, int x, int y) {

	static u8 return_value;

	__asm {
		PUSHAD
		MOV ECX, x
		MOV EAX, y
		MOV EDX, unitId
		CALL Func_Sub414680
		MOV return_value, AL
		POPAD
	}

	return return_value;

}

;

const u32 Func_AI_TrainingOverlord = 0x00435770;
void AI_TrainingOverlord(CUnit* main_building, CUnit* larva) {

	__asm {
		PUSHAD
		MOV EAX, larva
		MOV ECX, main_building
		CALL Func_AI_TrainingOverlord
		POPAD
	}

}

;
	
const u32 Func_removeOrderFromUnitQueue = 0x004742D0;
void removeOrderFromUnitQueue(CUnit* unit, COrder* order) {

	__asm {
		PUSHAD
		MOV ECX, unit
		MOV EAX, order
		CALL Func_removeOrderFromUnitQueue
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

const u32 Func_IterateUnitsAtLocationTargetProc = 0x004E8280;
//hardcoding the larva count function, keeping the return value
//though it's unused here
u32 IterateUnitsAtLocationTargetProc_LarvaCount(CUnit* unit, Box16* coords) {

	static u32 return_value;

	__asm {
		PUSHAD
		PUSH unit
		MOV EAX, coords
		MOV EBX, 0x004E8C80
		CALL Func_IterateUnitsAtLocationTargetProc
		MOV return_value, EAX
		POPAD
	}

	return return_value;

}

;

const u32 Func_Sub4E8DA0 = 0x004E8DA0;
void function_004E8DA0(CUnit* unit) {

	__asm {
		PUSHAD
		MOV EDI, unit
		CALL Func_Sub4E8DA0
		POPAD
	}

}

;

const u32 Func_Sub4E8E10 = 0x004E8E10;
bool function_004E8E10(CUnit* unit, Point32* coords) {

	static Bool32 return_pre_value;

	__asm {
		PUSHAD
		MOV EDI, coords
		MOV EAX, unit
		CALL Func_Sub4E8E10
		MOV return_pre_value, EAX
		POPAD
	}

	return (return_pre_value != 0);

}

;

} //Unnamed namespace

//End of helper functions

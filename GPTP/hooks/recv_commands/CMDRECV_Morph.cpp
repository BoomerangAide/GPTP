#include "CMDRECV_Morph.h"
#include <SCBW/api.h>

//Helper functions declaration

namespace {

bool HasMoneyCanMake(CUnit* builder, u32 unitToBuild);		//67250
void removeOrderFromUnitQueue(CUnit* unit, COrder* order);	//742D0

} //unnamed namespace

namespace hooks {

void CMDRECV_BuildingMorph(u16 wUnitType) {

	CUnit* building;
	*selectionIndexStart = 0;

	building = getActivePlayerNextSelection();

	if(
		building != NULL &&
		getActivePlayerNextSelection() == NULL &&
		building->id >= UnitId::ZergInfestedCommandCenter &&
		building->id <= UnitId::Special_CerebrateDaggoth &&
		wUnitType >= UnitId::ZergInfestedCommandCenter &&
		wUnitType <= UnitId::Special_CerebrateDaggoth &&
		building->canMakeUnit(wUnitType,*ACTIVE_NATION_ID) == 1 &&
		HasMoneyCanMake(building,wUnitType) && //start construction if successful
		building->mainOrderId != OrderId::Morph2
	)
	{
		building->orderComputerCL(OrderId::Morph2);
		scbw::refreshConsole();
	}

}

;

void CMDRECV_UnitMorph(u16 wUnitType) {

	CUnit* current_unit;
	*selectionIndexStart = 0;

	current_unit = getActivePlayerNextSelection();

	while(current_unit != NULL) {

		bool bAllowMorph;

		if(
			current_unit->id == UnitId::ZergHydralisk &&
			wUnitType == UnitId::ZergLurker
		)
			bAllowMorph = current_unit->canUseTech(TechId::LurkerAspect,*ACTIVE_NATION_ID) == 1;
		else
		if(	current_unit->id == UnitId::ZergLarva ||
			current_unit->id == UnitId::ZergMutalisk
		)
			bAllowMorph = current_unit->canMakeUnit(wUnitType,*ACTIVE_NATION_ID) == 1;
		else
			bAllowMorph = false;

		if(
			bAllowMorph &&
			current_unit->mainOrderId != OrderId::Morph1 &&
			HasMoneyCanMake(current_unit,wUnitType) //start construction if successful
		) 
		{

			current_unit->userActionFlags |= 1;

			if(current_unit->mainOrderId != OrderId::Die) {

				bool bKeepLooping = true;

				while(bKeepLooping) {

					if(current_unit->orderQueueTail == NULL)
						bKeepLooping = false;
					else
					if(	orders_dat::CanBeInterrupted[current_unit->orderQueueTail->orderId] ||
						current_unit->orderQueueTail->orderId == OrderId::Morph1
					)
						removeOrderFromUnitQueue(current_unit,current_unit->orderQueueTail);
					else
						bKeepLooping = false;

				}

				current_unit->performAnotherOrder(OrderId::Morph1,0,0,NULL,UnitId::None);

			}

			prepareForNextOrder(current_unit);

		}

		current_unit = getActivePlayerNextSelection();

	}

	scbw::refreshConsole();

}

;

} //namespace hooks

//-------- Helper function definitions. Do NOT modify! --------//

namespace {

const u32 Func_HasMoneyCanMake = 0x00467250;
bool HasMoneyCanMake(CUnit* builder, u32 unitToBuild) {

	static Bool32 bPreResult;

	__asm {
		PUSHAD
		PUSH unitToBuild
		MOV EDI, builder
		CALL Func_HasMoneyCanMake
		MOV bPreResult, EAX
		POPAD
	}

	return (bPreResult != 0);

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

} //unnamed namespace

//End of helper functions

#include "rescuable_order.h"
#include <SCBW/api.h>

//helper functions def
namespace {

CUnit** getAllUnitsInBounds(Box16* coords);						//2FF80
void GiveUnitHelper(CUnit* unit, u32 playerId, s32 unkScore);	//9EFA0

} //unnamed namespace

namespace hooks {

//order performed by units owned by a "rescuable" player
void orders_RescuePassive(CUnit* unit) {

	if (unit->mainOrderTimer == 0) {

		Box16 coords;
		CUnit** unitsInBounds;
		CUnit* humanPlayerUnit;
		int i = 0;

		unit->mainOrderTimer = 1;

		coords.right = unit->sprite->position.x + 64;
		coords.bottom = unit->sprite->position.y + 64;
		coords.left = unit->sprite->position.x - 64;
		coords.top = unit->sprite->position.y - 64;

		unitsInBounds = getAllUnitsInBounds(&coords);

		humanPlayerUnit = unitsInBounds[i];

		while (humanPlayerUnit != NULL && playerTable[humanPlayerUnit->playerId].type != PlayerType::Human) {
			i++;
			humanPlayerUnit = unitsInBounds[i];
		}

		if (humanPlayerUnit != NULL) {

			if ( //if main building, give all units owned by this unit player to the human player
				(
					unit->status & UnitStatus::Completed ||
					unit->id == UnitId::ZergHive ||
					unit->id == UnitId::ZergLair ||
					(unit->id == UnitId::ZergHatchery && unit->isRemorphingBuilding())
				) &&
				unit->status & UnitStatus::GroundedBuilding &&
				units_dat::BaseProperty[unit->id] & UnitProperty::ResourceDepot
			)
			{

				CUnit* rescuableUnit = firstPlayerUnit->unit[unit->playerId];
				CUnit* nextRescuableUnit;

				while (rescuableUnit != NULL) {

					nextRescuableUnit = rescuableUnit->player_link.next;

					if (rescuableUnit->sprite != NULL && rescuableUnit->mainOrderId != OrderId::Die) {

						GiveUnitHelper(rescuableUnit, humanPlayerUnit->playerId, 1);

						rescuableUnit->userActionFlags |= 1;

						if (rescuableUnit->status & UnitStatus::InBuilding)
							rescuableUnit->order(OrderId::BunkerGuard, 0, 0, NULL, UnitId::None, true);
						else
							rescuableUnit->order(units_dat::HumanIdleOrder[rescuableUnit->id], 0, 0, NULL, UnitId::None, true);

						prepareForNextOrder(rescuableUnit);

					}

					rescuableUnit = nextRescuableUnit;

				}

			}
			else //only giving this unit to the human player
			{

				GiveUnitHelper(unit, humanPlayerUnit->playerId, 1);

				unit->userActionFlags |= 1;

				if (unit->status & UnitStatus::InBuilding)
					unit->order(OrderId::BunkerGuard, 0, 0, NULL, UnitId::None, true);
				else
					unit->order(units_dat::HumanIdleOrder[unit->id], 0, 0, NULL, UnitId::None, true);

				prepareForNextOrder(unit);

			}

			if (*IS_IN_REPLAY == 0 && unit->playerId == *LOCAL_NATION_ID) {

				const u8*	CGame_player_race	=  (u8*)0x0057F1E2; //see RaceId enum

				unit->sprite->selectionTimer = 45;
				scbw::playSound(SoundId::Misc_ZRescue_wav + *CGame_player_race, unit);

			}

			scbw::refreshConsole();

		}

		//Cleanup after getAllUnitsInBounds()
		*tempUnitsListCurrentArrayCount = tempUnitsListArraysCountsListLastIndex[*tempUnitsListArraysCountsListLastIndex];
		*tempUnitsListArraysCountsListLastIndex = *tempUnitsListArraysCountsListLastIndex - 1;

	}

}

;

} //hooks

;

//-------- Helper function definitions. Do NOT modify! --------//

namespace {

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

const u32 Func_Sub_49EFA0 = 0x0049EFA0;
void GiveUnitHelper(CUnit* unit, u32 playerId, s32 unkScore) {

	__asm {
		PUSHAD
		PUSH unkScore
		MOV ECX, unit
		PUSH playerId
		CALL Func_Sub_49EFA0
		POPAD
	}

}

;

} //Unnamed namespace

//End of helper functions

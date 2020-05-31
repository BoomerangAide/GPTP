#include "merge_units.h"
#include <SCBW/api.h>

#ifndef MAXINT32
#define MAXINT32    (0xFFFFFFFF >> 1)
#endif
#ifndef MAXINT16
#define MAXINT16    (0xFFFF >> 1)
#endif

//Helper functions definitions

namespace {

	bool advanceRemainingBuildTime_Sub466940(CUnit* unit);					//0x00466940
	u8 function_0046A820(CUnit* unit);										//0x0046A820
	void orderImmediate(CUnit* unit, u8 order);								//0x00474B40
	void actUnitReturnToIdle(CUnit* unit);									//0x00475420
	void playMorphingCompleteSound(CUnit* unit);							//0x0048F440
	void mergeStatusIntoUnit_Sub493180(CUnit* unit_dst, CUnit* unit_src);	//0x00493180
	void function_0049B440(CUnit* unit, u32 soundId);			 			//0x0049B440
	void replaceUnitWithType(CUnit* unit, u16 newUnitId);					//0x0049FED0
	void changeUnitButtonSet_Sub4E5D60(CUnit* unit, u16 unitId);			//0x004E5D60
	void setNextWaypoint_Sub4EB290(CUnit* unit);							//0x004EB290
	void makeToHoldPosition(CUnit* unit);									//0x004EB5B0
	bool delayedSetMoveTarget_xy_Sub4EB960(CUnit* unit, u16 x, u16 y);		//0x004EB960

} //unnamed namespace

namespace hooks {

//equivalent to 0x004BFA80, but not identical
CUnit* templarMergePartner(CUnit* unit, CUnit** units_list, u32 units_list_length) {

	CUnit* nearest_unit = NULL;	//was ebp-04
	u32 best_distance = MAXINT32;

	for(u32 i = 0; i < units_list_length; i++) {

		if(units_list[i] != NULL) {

			u32 x_distance, y_distance;

			x_distance = 
				MAX(unit->sprite->position.x,units_list[i]->sprite->position.x) -
				MIN(unit->sprite->position.x,units_list[i]->sprite->position.x);

			x_distance *= x_distance;

			y_distance = 
				MAX(unit->sprite->position.y,units_list[i]->sprite->position.y) -
				MIN(unit->sprite->position.y,units_list[i]->sprite->position.y);

			y_distance *= y_distance;

			if( (x_distance + y_distance) < best_distance ) {

				CUnit* new_nearest_unit = units_list[i];
				units_list[i] = nearest_unit;
				nearest_unit = new_nearest_unit;
				best_distance = x_distance + y_distance;

			}

		} //if(units_list[i] != NULL)

	} //for(int i = units_list_length; i >= 0; i--)

	return nearest_unit;

}

;

void orders_CompletingArchonSummon(CUnit* unit) {

	u32*  const bCanUpdateCurrentButtonSet      = (u32*)	0x0068C1B0;
	u8*   const bCanUpdateSelectedUnitPortrait  = (u8*)		0x0068AC74;
	u8*   const bCanUpdateStatDataDialog        = (u8*)		0x0068C1F8;
	BinDlg**  const someDialogUnknown           = (BinDlg**)0x0068C1E8;
	BinDlg**  const someDialogUnknownUser       = (BinDlg**)0x0068C1EC;

	bool stopFunction = false;

	if(unit->mainOrderState == 0) {

		//93BB2
		if(advanceRemainingBuildTime_Sub466940(unit)) {

			unit->sprite->flags &= ~CSprite_Flags::IscriptCode;	//unset that flag

			if(unit->sprite->images.head != NULL) {

				CImage* current_image = unit->sprite->images.head;

				do {
					current_image->playIscriptAnim(IscriptAnimation::Init);
					current_image = current_image->link.next;
				}while (current_image != NULL);

			}

			unit->mainOrderState = 1;

		} //if(advanceRemainingBuildTime_Sub466940(unit)

	}else
	if( unit->mainOrderState == 1 && (unit->orderSignal & 4) ) {

		//93B37

		unit->orderSignal -= 4;
		unit->status |= UnitStatus::IsBuilding;

		//pathing related
		function_0046A820(unit);

		changeUnitButtonSet_Sub4E5D60(unit, unit->id);

		*bCanUpdateCurrentButtonSet = 1;
		*bCanUpdateSelectedUnitPortrait = 1;
		*bCanUpdateStatDataDialog = 1;
		*someDialogUnknown = NULL;
		*someDialogUnknownUser = NULL;

		playMorphingCompleteSound(unit);
		unit->order(units_dat::HumanIdleOrder[unit->id],0,0,NULL,UnitId::None,0);
		actUnitReturnToIdle(unit);

	}

}

;

void orders_WarpingDarkArchon(CUnit* unit) {

	CUnit* targetUnit = unit->orderTarget.unit;

	if(
		targetUnit == NULL || 
		unit->playerId != targetUnit->playerId ||
		targetUnit->mainOrderId != OrderId::WarpingDarkArchon ||
		targetUnit->orderTarget.unit != unit
		) 
	{	//conditions for merge not ok

		//94838
		setNextWaypoint_Sub4EB290(unit);

		if(
			unit->moveTarget.pt.x != unit->nextTargetWaypoint.x ||
			unit->moveTarget.pt.y != unit->nextTargetWaypoint.y
			)
		{
			//94857
			unit->nextTargetWaypoint.x = unit->moveTarget.pt.x;
			unit->nextTargetWaypoint.y = unit->moveTarget.pt.y;
		}

		//9485F

		if(unit->orderQueueHead != NULL) {

			//94866

			unit->userActionFlags |= 1;	//set flag
			prepareForNextOrder(unit);

		}
		else {

			//9487D
			if(unit->pAI != NULL)
				unit->orderComputerCL(OrderId::ComputerAI);
			else
				unit->orderComputerCL(units_dat::ReturnToIdleOrder[unit->id]);

		}

	}
	else {

		//946BE
		u32 distanceBetweenUnits;

		if(unit->status & UnitStatus::IsNormal) {
			if(unit->isTargetWithinMinRange(targetUnit,(2 * unit->current_speed2)/256)) {
				unit->status &= ~UnitStatus::IsNormal; //unset the flag
				orderImmediate(unit,OrderId::ResetCollision1);
			}
		}

		//946F3
		distanceBetweenUnits = 
			scbw::getDistanceFast(
				unit->sprite->position.x,
				unit->sprite->position.y,
				targetUnit->sprite->position.x,
				targetUnit->sprite->position.y
			);

		if(distanceBetweenUnits > 20) {

			//94724

											//unit unmovable			
			if(unit->mainOrderState != 0 && unit->getMovableState() == 2) {
				makeToHoldPosition(unit);
				unit->orderToIdle();
			}
			else {

				//94746

				unit->mainOrderState = 1;

				if(distanceBetweenUnits > 19)
					delayedSetMoveTarget_xy_Sub4EB960(
						unit,
						targetUnit->sprite->position.x,
						targetUnit->sprite->position.y
					);
				else {

					//94765 (will never happen?And that would be good because
					//I'm not sure what that stuff did and if I converted it
					//properly)

					s32 x,y;

					if(unit->sprite->position.x > MAXINT16)
						x = (0xFFFF0000) + unit->sprite->position.x;
					else
						x = unit->sprite->position.x;

					if(targetUnit->sprite->position.x > MAXINT16)
						x += (0xFFFF0000) + targetUnit->sprite->position.x;
					else
						x += targetUnit->sprite->position.x;

					if(x < 0)
						x -= 0xFFFFFFFF;

					if(unit->sprite->position.y > MAXINT16)
						y = (0xFFFF0000) + unit->sprite->position.y;
					else
						y = unit->sprite->position.y;

					if(targetUnit->sprite->position.y > MAXINT16)
						y += (0xFFFF0000) + targetUnit->sprite->position.y;
					else
						y += targetUnit->sprite->position.y;

					if(y < 0)
						y -= 0xFFFFFFFF;

					x /= 2; y /= 2;

					if( (u16)x > MAXINT16 )
						x = 0xFFFF0000 + (u16)x;
					else
						x = (u16)x;

					if( (u16)y > MAXINT16 )
						y = 0xFFFF0000 + (u16)y;
					else
						y = (u16)y;

					delayedSetMoveTarget_xy_Sub4EB960(unit,x,y);

				} //if(distanceBetweenUnits <= 19)

			} //if(unit->mainOrderState == 0 || unit->getMovableState() != 2)

		} //if(distanceBetweenUnits > 20)
		else {

			//9479C:
			mergeStatusIntoUnit_Sub493180(unit,targetUnit);
			replaceUnitWithType(unit, UnitId::ProtossDarkArchon);

			//update stuff related to the beginning of merge process I think
			function_0049B440(unit, SoundId::Terran_PHOENIX_TPhClo01_WAV);

			unit->sprite->flags &= ~CSprite_Flags::IscriptCode; //disable the flag if set

			if(unit->sprite->images.head != NULL) {

				CImage* current_image = unit->sprite->images.head;

				do {
					current_image->playIscriptAnim(IscriptAnimation::SpecialState1);
					current_image = current_image->link.next;
				}while (current_image != NULL);

			}

			//947E0:
			changeUnitButtonSet_Sub4E5D60(unit,UnitId::None);
			scbw::refreshConsole();

			unit->killCount += targetUnit->killCount;
			unit->energy = 12800;	//50 energy ingame
			
			//remove the 2nd unit of the merge process
			targetUnit->userActionFlags |= 4;
			targetUnit->remove();

			unit->status &= ~UnitStatus::IsBuilding; //disable the flag if set
			function_0046A820(unit); ////pathing related

			unit->orderComputerCL(OrderId::CompletingArchonSummon);

		} //if(distanceBetweenUnits <= 20)

	} //conditions for merge are ok

} //void orders_WarpingDarkArchon(CUnit* unit)

;

void orders_WarpingArchon(CUnit* unit) {

	CUnit* targetUnit = unit->orderTarget.unit;

	if(
		targetUnit == NULL || 
		unit->playerId != targetUnit->playerId ||
		targetUnit->mainOrderId != OrderId::WarpingArchon ||
		targetUnit->orderTarget.unit != unit
		) 
	{	//conditions for merge not ok

		//94A3F
		setNextWaypoint_Sub4EB290(unit);

		if(
			unit->moveTarget.pt.x != unit->nextTargetWaypoint.x ||
			unit->moveTarget.pt.y != unit->nextTargetWaypoint.y
			)
		{
			//94A5E
			unit->nextTargetWaypoint.x = unit->moveTarget.pt.x;
			unit->nextTargetWaypoint.y = unit->moveTarget.pt.y;
		}

		//94A66

		if(unit->orderQueueHead != NULL) {

			//94866
			unit->userActionFlags |= 1;	//set flag
			prepareForNextOrder(unit);

		}
		else {

			//94A84
			if(unit->pAI != NULL)
				unit->orderComputerCL(OrderId::ComputerAI);
			else
				unit->orderComputerCL(units_dat::ReturnToIdleOrder[unit->id]);

		}

	}
	else {

		//948EA
		u32 distanceBetweenUnits;

		if(unit->status & UnitStatus::IsNormal) {
			if(unit->isTargetWithinMinRange(targetUnit,(2 * unit->current_speed2)/256)) {
				unit->status &= ~UnitStatus::IsNormal; //unset the flag
				orderImmediate(unit,OrderId::ResetCollision1);
			}
		}

		//94913
		distanceBetweenUnits = 
			scbw::getDistanceFast(
				unit->sprite->position.x,
				unit->sprite->position.y,
				targetUnit->sprite->position.x,
				targetUnit->sprite->position.y
			);

		if(distanceBetweenUnits > 2) {

			//94944

											//unit unmovable
			if(unit->mainOrderState != 0 && unit->getMovableState() == 2) {
				makeToHoldPosition(unit);
				unit->orderToIdle();
			}
			else {

				//94966

				unit->mainOrderState = 1;

				if(distanceBetweenUnits > 10)
					delayedSetMoveTarget_xy_Sub4EB960(
						unit,
						targetUnit->sprite->position.x,
						targetUnit->sprite->position.y
					);

				else {

					//94985 (will never happen?And that would be good because
					//I'm not sure what that stuff did and if I converted it
					//properly)

					s32 x,y;

					if(unit->sprite->position.x > MAXINT16)
						x = (0xFFFF0000) + unit->sprite->position.x;
					else
						x = unit->sprite->position.x;

					if(targetUnit->sprite->position.x > MAXINT16)
						x += (0xFFFF0000) + targetUnit->sprite->position.x;
					else
						x += targetUnit->sprite->position.x;

					if(x < 0)
						x -= 0xFFFFFFFF;

					if(unit->sprite->position.y > MAXINT16)
						y = (0xFFFF0000) + unit->sprite->position.y;
					else
						y = unit->sprite->position.y;

					if(targetUnit->sprite->position.y > MAXINT16)
						y += (0xFFFF0000) + targetUnit->sprite->position.y;
					else
						y += targetUnit->sprite->position.y;

					if(y < 0)
						y -= 0xFFFFFFFF;

					x /= 2; y /= 2;

					if( (u16)x > MAXINT16 )
						x = 0xFFFF0000 + (u16)x;
					else
						x = (u16)x;

					if( (u16)y > MAXINT16 )
						y = 0xFFFF0000 + (u16)y;
					else
						y = (u16)y;

					delayedSetMoveTarget_xy_Sub4EB960(unit,x,y);

				} //if(distanceBetweenUnits <= 19)

			} //if(unit->mainOrderState == 0 || unit->getMovableState() != 2)

		} //if(distanceBetweenUnits > 20)
		else {

			//949BC:
			mergeStatusIntoUnit_Sub493180(unit,targetUnit);
			replaceUnitWithType(unit, UnitId::ProtossArchon);

			unit->sprite->flags &= ~CSprite_Flags::IscriptCode; //disable the flag if set

			if(unit->sprite->images.head != NULL) {

				CImage* current_image = unit->sprite->images.head;

				do {
					current_image->playIscriptAnim(IscriptAnimation::SpecialState1);
					current_image = current_image->link.next;
				}while (current_image != NULL);

			}

			//949F0:
			changeUnitButtonSet_Sub4E5D60(unit,UnitId::None);
			scbw::refreshConsole();

			unit->killCount += targetUnit->killCount;
			
			//remove the 2nd unit of the merge process
			targetUnit->userActionFlags |= 4;
			targetUnit->remove();

			unit->status &= ~UnitStatus::IsBuilding; //disable the flag if set
			function_0046A820(unit); ////pathing related

			unit->orderComputerCL(OrderId::CompletingArchonSummon);

		} //if(distanceBetweenUnits <= 20)

	} //conditions for merge are ok

} //void orders_WarpingArchon(CUnit* unit)

} //hooks

//-------- Helper function definitions. Do NOT modify! --------//

namespace {

u32 Func_Sub466940 = 0x00466940;
//return true if remainingBuildTime was 0 when called,
//remainingBuildTime change affected by cheating
bool advanceRemainingBuildTime_Sub466940(CUnit* unit) {

	static Bool32 bTimeWasZero;

	__asm {
		PUSHAD
		MOV ECX, unit
		CALL Func_Sub466940
		MOV bTimeWasZero, EAX
		POPAD
	}

	return (bTimeWasZero != 0);

}

;

u32 Func_Sub46A820 = 0x0046A820;
//return value may be useless since it's the exact value of pathingFlags
u8 function_0046A820(CUnit* unit) {

	static u8 result;

	__asm {
		PUSHAD
		MOV EAX, unit
		CALL Func_Sub46A820
		MOV result, AL
		POPAD
	}

	return result;

}

;

u32 Func_OrderImmediate = 0x00474B40;
void orderImmediate(CUnit* unit, u8 order) {

	__asm {
		PUSHAD
		MOV ECX, unit
		MOV AL, order
		CALL Func_OrderImmediate
		POPAD
	}

}

;

const u32 Func_ActUnitReturnToIdle = 0x00475420;
void actUnitReturnToIdle(CUnit* unit) {

  __asm {
    PUSHAD
    MOV EAX, unit
    CALL Func_ActUnitReturnToIdle
    POPAD
  }

}

;

const u32 Func_PlayMorphingCompleteSound = 0x0048F440;
void playMorphingCompleteSound(CUnit* unit) {

	__asm {
		PUSHAD
		MOV EAX, unit
		CALL Func_PlayMorphingCompleteSound
		POPAD
	}

}

;

const u32 Func_Sub493180 = 0x00493180;
//get status (defensiveMatrixHp to acidSporeTime) of unit_src
//and give it to unit_dst
void mergeStatusIntoUnit_Sub493180(CUnit* unit_dst, CUnit* unit_src) {

	__asm {
		PUSHAD
		MOV ECX, unit_src
		MOV EAX, unit_dst
		CALL Func_Sub493180
		POPAD
	}

}

;

const u32 Func_Sub49B440 = 0x0049B440;
void function_0049B440(CUnit* unit, u32 soundId) {

	__asm {
		PUSHAD
		MOV EBX, soundId
		MOV EAX, unit
		CALL Func_Sub49B440
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

const u32 Func_Sub4E5D60 = 0x004E5D60;
void changeUnitButtonSet_Sub4E5D60(CUnit* unit, u16 buttonSetId) {

	__asm {
		PUSHAD
		MOV EAX, unit
		MOV CX, buttonSetId
		CALL Func_Sub4E5D60
		POPAD
	}

}

;

const u32 Func_Sub4EB290 = 0x004EB290;
//Related to path/movement decision
void setNextWaypoint_Sub4EB290(CUnit* unit) {

  __asm {
    PUSHAD
    MOV EAX, unit
    CALL Func_Sub4EB290
    POPAD
  }

}

;

const u32 Func_OrdersHoldPositionSuicidal = 0x004EB5B0;
void makeToHoldPosition(CUnit* unit) {

  __asm {
    PUSHAD
    MOV ESI, unit
    CALL Func_OrdersHoldPositionSuicidal
    POPAD
  }
}

;

const u32 Func_Sub4EB960 = 0x004EB960;
//return true if waiting for recentOrderTimer to reach 0 or
//if setMoveTarget_xy returned true
bool delayedSetMoveTarget_xy_Sub4EB960(CUnit* unit, u16 x, u16 y) {

	Bool32 bPreResult;

	__asm {
		PUSHAD
		MOVSX EDI, y
		MOVSX EBX, x
		MOV EAX, unit
		CALL Func_Sub4EB960
		MOV bPreResult, EAX
		POPAD
	}

	return (bPreResult != 0);

}

;

} //Unnamed namespace

//End of helper functions

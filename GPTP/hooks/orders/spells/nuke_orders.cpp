#include "nuke_orders.h"
#include <SCBW/api.h>

//helper functions def

namespace {

void setAllOverlayDirectionsGeneric(CUnit* unit, u32 unk);							//01140
bool isDistanceGreaterThanHaltDistance(CUnit* unit, int x, int y, u32 distance);	//01240
void AI_orderUnitCloaking(CUnit* unit);												//3B970
u32 function_0043DB50(CUnit* unit);													//3DB50
void actUnitReturnToIdle(CUnit* unit);												//75420
void orders_NukeGround_Helper(CUnit* unit);											//79200
bool setThingyVisibilityFlags(CThingy* thingy);										//878F0
CThingy* createThingy(u32 spriteId, s16 x, s16 y, u32 playerId);					//88210
void showStatTxtToPlayer(char* text, u32 playerId);									//8CF00
void turnGraphic(CUnit* unit, s32 direction);										//95F20
void changeUnitButtonSet_Sub4E5D60(CUnit* unit, u16 unitId);						//E5D60
void hideAndDisableUnit(CUnit* unit);												//E6340
void showAndEnableUnit(CUnit* unit);												//E6490
void setNextWaypoint_Sub4EB290(CUnit* unit);										//EB290
bool SetMoveTarget_xy(CUnit* unit, int x, int y);									//EB820
bool function_004EB9C0(CUnit* unit, int x, int y);									//EB9C0

} //unnamed namespace

namespace hooks {

//was unreferenced, used by OrderId::NukePaint
void orders_NukePaint(CUnit* unit) {
	if (unit->orderTarget.unit != NULL)
		unit->orderTo(OrderId::NukeUnit, unit->orderTarget.unit);
	else
		unit->orderTo(OrderId::NukeGround, unit->orderTarget.pt.x, unit->orderTarget.pt.y);
}

;

//was unreferenced, used by OrderId::NukeLaunch
void orders_NukeLaunch(CUnit* unit) {

	const u8*	CGame_player_race	=  (u8*)0x0057F1E2; //see RaceId enum

	CUnit* silo = unit->connectedUnit;

	if (unit->mainOrderState < 5 && silo == NULL)
		unit->remove();
	else
	if (unit->mainOrderState <= 6) {

		if (unit->mainOrderState == 0) {
			scbw::playSound(SoundId::Bullet_TNsFir00_wav_2, unit);
			function_004EB9C0(unit, unit->sprite->position.x, units_dat::UnitBounds[UnitId::TerranNuclearMissile].top);
			unit->mainOrderTimer = 90;
			unit->mainOrderState = 1;
		}
		else
		if (unit->mainOrderState == 1) {

			if (unit->mainOrderTimer <= 45 || unit->getMovableState() != MovableState::NotReachedDestination) {
				scbw::playSound(*CGame_player_race + SoundId::Zerg_Advisor_ZAdUpd04_WAV, NULL);
				showStatTxtToPlayer((char*)statTxtTbl->getString(0x417), *LOCAL_NATION_ID); //"Nuclear launch detected.<0>"
				unit->mainOrderState = 2;
			}

		}
		else
		if (unit->mainOrderState == 2) {

			if (unit->mainOrderTimer == 0 || unit->getMovableState() != MovableState::NotReachedDestination) {
				hideAndDisableUnit(unit);
				silo->connectedUnit = unit;
				setNextWaypoint_Sub4EB290(unit);
				unit->mainOrderState = 3;
			}

		}
		else
		if (unit->mainOrderState == 3) {

			if (!(unit->movementFlags & MovementFlags::Accelerating)) {
				unit->sprite->playIscriptAnim(IscriptAnimation::WarpIn, true);
				unit->mainOrderState = 4;
			}

		}
		else
		if (unit->mainOrderState == 4) {

			if (unit->orderSignal & 2) {

				s16 x, y;

				unit->orderSignal -= 2;

				if (units_dat::UnitBounds[UnitId::TerranNuclearMissile].top > (s16)unit->orderTarget.pt.y - 320)
					y = units_dat::UnitBounds[UnitId::TerranNuclearMissile].top;
				else
					y = unit->orderTarget.pt.y - 320;

				if (units_dat::UnitBounds[UnitId::TerranNuclearMissile].left > unit->orderTarget.pt.x)
					x = units_dat::UnitBounds[UnitId::TerranNuclearMissile].left;
				else
					x = unit->orderTarget.pt.x;

				scbw::setUnitPosition(unit, x, y);

				//turn the missile to face down
				turnGraphic(unit, 128);

				function_004EB9C0(unit, unit->orderTarget.pt.x, unit->orderTarget.pt.y);
				showAndEnableUnit(unit);

				unit->mainOrderState = 5;

			}

		}
		else
		if (unit->mainOrderState == 5) {

			if (isDistanceGreaterThanHaltDistance(unit, unit->moveTarget.pt.x, unit->moveTarget.pt.y, 10)) {

				CImage* current_image = unit->sprite->images.head;

				unit->orderTarget.unit = unit;

				while (current_image != NULL) {
					current_image->playIscriptAnim(IscriptAnimation::SpecialState1);
					current_image = current_image->link.next;
				}

				unit->mainOrderState = 6;

			}

		}
		else
		if (unit->mainOrderState == 6) {

			if (unit->orderSignal & 1) {
				unit->orderSignal -= 1;
				unit->userActionFlags |= 4;
				unit->remove();
			}

		}

	}

}

;

void orders_NukeTrack(CUnit* unit) {

	if(unit->mainOrderState != 0) {

		if(unit->mainOrderState == 6) {

			if(
				unit->connectedUnit == NULL ||
				unit->connectedUnit->mainOrderState == 5
			)
			{ //64D3F

				CImage* current_image = unit->sprite->images.head;

				if(unit->orderQueueHead == NULL)
					unit->orderSimple(units_dat::ReturnToIdleOrder[unit->id],false);

				actUnitReturnToIdle(unit);
				changeUnitButtonSet_Sub4E5D60(unit,unit->id);
				scbw::refreshConsole();

				while(current_image != NULL) {
					current_image->playIscriptAnim(IscriptAnimation::WalkingToIdle);
					current_image = current_image->link.next;
				}

				if(unit->building.ghostNukeMissile != NULL) {

					current_image = (unit->building.ghostNukeMissile)->sprite->images.head; 

					while(current_image != NULL) {
						current_image->playIscriptAnim(IscriptAnimation::Death);
						current_image = current_image->link.next;
					}

					unit->building.ghostNukeMissile = NULL;

				}

				//64DD6
				function_0043DB50(unit);

			}

		}

	}
	else { //64DE3

		CThingy* nukeDot;

		unit->sprite->playIscriptAnim(IscriptAnimation::SpecialState1,true);

		nukeDot = createThingy(
			SpriteId::Nuke_Target_Dot,
			(unit->connectedUnit)->orderTarget.pt.x,
			(unit->connectedUnit)->orderTarget.pt.y,
			unit->playerId
			);

		unit->building.ghostNukeMissile = (CUnit*)nukeDot;

		if(nukeDot != NULL) {
			nukeDot->sprite->elevationLevel = unit->sprite->elevationLevel + 1;
			setThingyVisibilityFlags(nukeDot);
		}

		unit->mainOrderState = 6;

	}

}

;

void orders_NukeGround(CUnit* unit) {

	u32 sightRange, distance;

	if(
		unit->nextTargetWaypoint.x != unit->orderTarget.pt.x ||
		unit->nextTargetWaypoint.y != unit->orderTarget.pt.y
	)
	{
		unit->nextTargetWaypoint.x = unit->orderTarget.pt.x;
		unit->nextTargetWaypoint.y = unit->orderTarget.pt.y;
	}

	sightRange = unit->getSightRange(false) * 32;

	if(unit->pAI != NULL) {

		distance = scbw::getDistanceFast(
			unit->orderTarget.pt.x * 256,
			unit->orderTarget.pt.y * 256,
			unit->halt.x,
			unit->halt.y
		);

		if(distance / 256 <= sightRange * 3)
			AI_orderUnitCloaking(unit);

	}

	//79267
	distance = scbw::getDistanceFast(
		unit->orderTarget.pt.x * 256,
		unit->orderTarget.pt.y * 256,
		unit->halt.x,
		unit->halt.y
	);

	if(distance / 256 > sightRange) {

		if(unit->recentOrderTimer == 0)
			SetMoveTarget_xy(unit,unit->orderTarget.pt.x,unit->orderTarget.pt.y);

	}
	else { //792AE

		s32 direction;
		bool bEndThere = false;

		setNextWaypoint_Sub4EB290(unit);

		if(
			unit->nextTargetWaypoint.x != unit->position.x ||
			unit->nextTargetWaypoint.y != unit->position.y
		) 
		{

			direction = 
				unit->currentDirection1 - 
				scbw::getAngle(
				unit->sprite->position.x,
				unit->sprite->position.y,
				unit->nextTargetWaypoint.x,
				unit->nextTargetWaypoint.y
				);

			if(direction < -1 || direction > 1)
				bEndThere = true;
					
		}

		//792F9
		if(!bEndThere) {

			bool bLoopEnd = false;
			CUnit* current_unit = firstPlayerUnit->unit[unit->playerId];

			while(!bLoopEnd) {

				if(current_unit == NULL)
					bLoopEnd = true;
				else
				if(	current_unit->id == UnitId::TerranNuclearSilo &&
					current_unit->building.silo.isReady
				)
					bLoopEnd = true;
				else
					current_unit = current_unit->player_link.next;

			}

			if(current_unit == NULL)
				unit->orderToIdle();
			else { //79333

				CUnit* nuke = current_unit->building.silo.nuke;

				current_unit->building.silo.nuke = NULL;
				current_unit->building.silo.isReady = false;

				scbw::playSound(SoundId::Terran_GHOST_TGhLas00_wav,unit);

				nuke->orderTo(OrderId::NukeLaunch,unit->orderTarget.pt.x,unit->orderTarget.pt.y);
				nuke->connectedUnit = unit;
				setAllOverlayDirectionsGeneric(nuke,0);
				showAndEnableUnit(nuke);

				unit->connectedUnit = nuke;
				unit->orderTo(OrderId::NukeTrack,unit->sprite->position.x,unit->sprite->position.y);

				if(
					unit->status & UnitStatus::DoodadStatesThing ||
					unit->lockdownTimer != 0 ||
					unit->stasisTimer != 0 ||
					unit->maelstromTimer != 0
				)
				{ //793C4

					if(units_dat::BaseProperty[unit->id] & UnitProperty::Building)
						unit->currentButtonSet = UnitId::Buttons_CancelNukeStrike;
				}
				else
					unit->currentButtonSet = UnitId::Buttons_CancelNukeStrike;

				scbw::refreshConsole();

			}

		}

	}

}

;

//was unreferenced, used by OrderId::NukeUnit
void orders_NukeUnit(CUnit* unit) {

	CUnit* nuke = unit->orderTarget.unit;

	if (nuke == NULL) {

		setNextWaypoint_Sub4EB290(unit);

		if (unit->orderQueueHead != NULL) {
			unit->userActionFlags |= 1;
			prepareForNextOrder(unit);
		}
		else
		if(unit->pAI == NULL)
			unit->orderComputerCL(units_dat::ReturnToIdleOrder[unit->id]);
		else
			unit->orderComputerCL(OrderId::ComputerAI);

	}
	else
	{
		unit->orderTarget.pt.x = nuke->sprite->position.x;
		unit->orderTarget.pt.y = nuke->sprite->position.y;
		orders_NukeGround_Helper(unit);
	}

}

;

//Don't actually init the nuke creation, but still is an actual order
void orders_NukeTrain(CUnit* unit) {

	if (unit->mainOrderState == 0) {
		if (unit->currentBuildUnit != NULL) {
			unit->building.silo.nuke = unit->currentBuildUnit;
			(unit->currentBuildUnit)->connectedUnit = unit;
			unit->mainOrderState = 1;
		}
	}
	else
	if (unit->mainOrderState == 1) {

		CUnit* nuke = unit->building.silo.nuke;

		if (nuke == NULL)
			unit->mainOrderState = 0;
		else
		if (nuke->status & UnitStatus::Completed) {

			unit->building.silo.isReady = true;
			scbw::refreshConsole();
			unit->mainOrderState = 0;

			if (unit->pAI != NULL)
				unit->orderComputerCL(OrderId::ComputerAI);

		}

	}

}

;

} //namespace hooks

//-------- Helper function definitions. Do NOT modify! --------//

namespace {

const u32 Func_setAllOverlayDirectionsGeneric = 0x00401140;
void setAllOverlayDirectionsGeneric(CUnit* unit, u32 unk) {

	__asm {
		PUSHAD
		MOV EAX, unit
		PUSH unk
		CALL Func_setAllOverlayDirectionsGeneric
		POPAD
	}

}

;

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

const u32 Func_AI_orderUnitCloaking = 0x0043B970;
void AI_orderUnitCloaking(CUnit* unit) {

	__asm {
		PUSHAD
		MOV ESI, unit
		CALL Func_AI_orderUnitCloaking
		POPAD
	}

}

;

const u32 Func_Sub43DB50 = 0x0043DB50;
u32 function_0043DB50(CUnit* unit) {

	static u32 return_value;

	__asm {
		PUSHAD
		MOV ESI, unit
		CALL Func_Sub43DB50
		MOV return_value, EAX
		POPAD
	}

	return return_value;

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

const u32 Func_Sub479200 = 0x00479200;
void orders_NukeGround_Helper(CUnit* unit) {
	__asm {
		PUSHAD
		PUSH unit
		CALL Func_Sub479200
		POPAD
	}
}

//original referenced name was sub_4878F0, but using
//the name from bunker_hooks.cpp since it got meaning
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

const u32 Func_showStatTxtToPlayer = 0x0048CF00;
void showStatTxtToPlayer(char* text, u32 playerId) {
	__asm {
		PUSHAD
		MOV ECX, playerId
		MOV EAX, text
		CALL Func_showStatTxtToPlayer
		POPAD
	}
}

;

const u32 Func_turnGraphic = 0x00495F20;
void turnGraphic(CUnit* unit, s32 direction) {
	__asm {
		PUSHAD
		MOV EAX, unit
		MOV EBX, direction
		CALL Func_turnGraphic
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

const u32 Func_unitDeathSomething_0 = 0x004E6340;
void hideAndDisableUnit(CUnit* unit) {

	__asm {
		PUSHAD
		MOV EAX, unit
		CALL Func_unitDeathSomething_0
		POPAD
	  }

}

;

const u32 Func_InitUnitTrapDoodad = 0x004E6490;
void showAndEnableUnit(CUnit* unit) {

  __asm {
    PUSHAD
    MOV EDI, unit
    CALL Func_InitUnitTrapDoodad
    POPAD
  }
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

const u32 Func_SetMoveTarget_xy = 0x004EB820;
bool SetMoveTarget_xy(CUnit* unit, int x, int y) {

	static Bool32 bPreResult;

	__asm {
		PUSHAD
		MOV ESI, unit
		MOV EBX, x
		MOV EDI, y
		CALL Func_SetMoveTarget_xy
		MOV bPreResult, EAX
		POPAD
	}

	return (bPreResult != 0);

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

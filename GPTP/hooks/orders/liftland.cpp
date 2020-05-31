#include "larva_creep_spawn.h"
#include <SCBW/api.h>

//Helper functions definitions

namespace {

	void setUnitPathing(CUnit* unit, u8 unkPathRelated);															//01360
	bool isUnitPositions2Equal(CUnit* unit);																		//02160
	CUnit* connectAddonCheck(CUnit* unit);																			//404A0
	void function_00463470(CUnit* unit);																			//63470
	void function_004634E0(CUnit* unit);																			//634E0
	void function_00463640(CUnit* unit);																			//63640
	void completeAddon(CUnit* unit, CUnit* addon);																	//63D50
	void function_00464300(CUnit* unit);																			//64300
	void disconnectFromAddOn(CUnit* unit);																			//64930
	void function_00469EC0(CUnit* unit, int x, int y);																//69EC0
	void function_00469F60(CUnit* unit, int x, int y);																//69F60
	void function_0046A560(CUnit* unit);																			//6A560
	void function_0046A5A0(CUnit* unit);																			//6A5A0
	u32 function_00473FB0(CUnit* unit, u8 playerId, int x, int y, u16 unitId, u8 unk1, u8 unk2, u8 unk3, u8 unk4 );	//73FB0
	void removeOrderFromUnitQueue(CUnit* unit, COrder* order);														//742D0
	void function_00474760(CUnit* unit, COrder* order, u8 orderId);													//74760
	void actUnitReturnToIdle(CUnit* unit);																			//75420
	bool function_0047DF90(CUnit* unit);																			//7DF90
	void refreshLayer3And4();																						//8D9A0
	bool function_0048DDA0();																						//8DDA0
	void function_0048E310();																						//8E310
	void placebuildingProc();																						//8E6E0
	void playBuildingLandSound(CUnit* unit);																		//8F5A0
	void SetUnitMovementSpeed(CUnit* unit, u32 newSpeed);															//951C0
	void function_00498150(CSprite* sprite, u8 newVerticalOffset);													//98150
	void function_004997E0(CSprite* sprite);																		//997E0
	void function_004C3B40(CUnit* unit);																			//C3B40
	bool function_004EB9C0(CUnit* unit, int x, int y);																//EB9C0

} //unnamed namespace

namespace hooks {

void orders_BuildingLand(CUnit* unit) {

	Target unitTarget;
	unitTarget.unit = unit->orderTarget.unit;	//[ebp-04]
	unitTarget.pt = unit->orderTarget.pt;		//[ebp-08]

	Point16 coords;
	coords.x = 0;	//[ebp-10]
	coords.y = 0;	//[ebp-0e]

	if(unit->status & UnitStatus::GroundedBuilding) {

		unit->userActionFlags |= 1;

		if(unit->mainOrderId != OrderId::Die) {

			bool bLoopBreak = false;

			//643B0
			while(unit->orderQueueTail != NULL && !bLoopBreak) {

				bLoopBreak =
					!orders_dat::CanBeInterrupted[unit->orderQueueTail->orderId] &&
					(unit->orderQueueTail->orderId != OrderId::BuildingLiftoff);

				if(!bLoopBreak)
					removeOrderFromUnitQueue(unit, unit->orderQueueTail);

			}

			//643D3:
			unit->performAnotherOrder(
				OrderId::BuildingLiftoff, 
				coords.x,
				coords.y, 
				NULL, 
				0
			);

		} //if( (unit->mainOrderId != OrderId::Die) )

		//643E8
		prepareForNextOrder(unit);

		if(unit->mainOrderId != OrderId::Die) {

			while(unit->orderQueueTail != NULL && unit->orderQueueTail->orderId == OrderId::BuildingLand)
				removeOrderFromUnitQueue(unit, unit->orderQueueTail);

			//6440A:
			unit->performAnotherOrder(
				OrderId::BuildingLand,
				unitTarget.pt.x,
				unitTarget.pt.y,
				unitTarget.unit,
				UnitId::None
			);

		}

		//64422
		unit->order(units_dat::ReturnToIdleOrder[unit->id],0,0,NULL,UnitId::None,false);

	} //if( !(unit->status & UnitStatus::GroundedBuilding) )

	else
	//64450
	if(unit->mainOrderState == 0) {
		//plan vertical movement toward ground
		function_004EB9C0(unit,unit->orderTarget.pt.x,unit->orderTarget.pt.y - unit->sprite->images.tail->verticalOffset);
		unit->mainOrderState = 1;
	}
	else
	if(unit->mainOrderState == 1) {

		//6448A
		if(unit->getMovableState() != 0) {

			int x,y;

			x = (s16)units_dat::BuildingDimensions[unit->id].x;
			y = (s16)units_dat::BuildingDimensions[unit->id].y;

			if(x<0) x+=1;if(y<0) y+=1;

			x /= 2;y /= 2;

			x = unit->orderTarget.pt.x - x;
			y = unit->orderTarget.pt.y - y;

			if(x<0) x+=31;if(y<0) y+=31;

			x /= 32;y /= 32;

			//check for units occupying the landing zone before
			//the final vertical movement started?
			u32 return_00473FB0 = 
				function_00473FB0(unit,unit->playerId,x,y,unit->id,0,0,1,0);

			if(return_00473FB0 != 0) {

				//64504:

				//play "landing sequence interrupted"
				playBuildingLandSound(unit);

				if(	
					unit->orderQueueHead != NULL &&
					unit->orderQueueHead->orderId == OrderId::PlaceAddon
				)
					removeOrderFromUnitQueue(unit, unit->orderQueueHead);

				unit->orderToIdle();

			}
			else {

				//6452B:

				//use a AddFogMask() function after some coordinates calculations
				function_00469F60(unit,unit->orderTarget.pt.x,unit->orderTarget.pt.y);

				unit->building.landingTimer = 1;

				//plan movement again after latest updates
				function_004EB9C0(unit,unit->orderTarget.pt.x,unit->orderTarget.pt.y);

				if(unit->sprite->images.tail->paletteType == PaletteType::RLE_SHADOW)
					unit->sprite->images.tail->flags |= CImage_Flags::DontUpdate;

				unit->status |= UnitStatus::CanNotReceiveOrders;

				unit->flingyTopSpeed = 256;
				SetUnitMovementSpeed(unit,256);

				unit->mainOrderState = 2;

			} //if(return_00473FB0 == 0)

		} //if(unit->getMovableState()) != 0)

	} //if(unit->mainOrderState == 1)
	else
	if(unit->mainOrderState == 2) {

		bool bStopThere = false;

		//64587:
		if( !isUnitPositions2Equal(unit) ) {

			int someValue = 
				scbw::getAngle(
					unit->sprite->position.x,
					unit->sprite->position.y,
					unit->nextTargetWaypoint.x,
					unit->nextTargetWaypoint.y
				);

			bStopThere = ( (unit->velocityDirection1 - someValue) != 0 );

		}

		//645B7
		if(!bStopThere) {
			unit->sprite->playIscriptAnim(IscriptAnimation::Landing);
			unit->mainOrderState = 3;
		}


	} //if(unit->mainOrderState == 2)

	//there's no "else" here, mainOrderState = 2 can continue directly
	//into mainOrderState = 3 if mainOrderState was updated

	if(unit->mainOrderState == 3) {

		//645C7:
		//orderSignal is set by IScript animation
		if( (unit->getMovableState() != 0) && (unit->orderSignal & 0x10) ) {

			CUnit* addon;

			unit->orderSignal -= 0x10;

			//use a SetFogMask() function after some coordinates preparation
			function_00469EC0(unit,unit->orderTarget.pt.x,unit->orderTarget.pt.y);

			//recalculate position and update effect on
			//surroundings?
			function_0046A560(unit);

			//toggle pathing-related stuff to prepare
			//to no longer use pathing
			function_004634E0(unit);

			unit->sprite->elevationLevel = 4;
			setUnitPathing(unit,0);
			scbw::setUnitPosition(unit,unit->orderTarget.pt.x,unit->orderTarget.pt.y);

			//maybe update orders targetting the unit,
			//set repulse for units in surroundings,and 
			//some others things?			
			function_0046A5A0(unit);

			//Exclude unit from multiple selection
			if(unit->sprite->flags & CSprite_Flags::Selected && *clientSelectionCount > 1)
				function_004C3B40(unit);

			//64648

			//change the verticalOffset of images besides shadow to 0
			function_00498150(unit->sprite, 0);

			if(unit->sprite->images.tail->paletteType == PaletteType::RLE_SHADOW)
				unit->sprite->images.tail->flags &= ~CImage_Flags::DontUpdate;

			//probably what display the landing dust
			function_004997E0(unit->sprite);

			scbw::refreshConsole();

			//crush units that managed to get unmovable there
			//(tanks in siege, burrowed zerg)
			function_00464300(unit);

			if(unit->orderQueueHead != NULL) {

				bool bGotMoveOrder;

				if(	unit->orderQueueHead->orderId == OrderId::Move ||
					unit->orderQueueHead->orderId == OrderId::Follow
					)
					bGotMoveOrder = true;
				else {

					bGotMoveOrder = false;

					//6468F
					COrder* order = unit->orderQueueHead;

					if(order->orderId != OrderId::PlaceAddon)
						order = NULL;

					while(unit->orderQueueTail != order)
						removeOrderFromUnitQueue(unit, unit->orderQueueTail);

					//646AF
					if(unit->orderQueueHead == NULL)
						unit->orderComputerCL(units_dat::ReturnToIdleOrder[unit->id]);

				}

				if(bGotMoveOrder)
					//646C7
					//this cause the landed building to immediately take off again to
					//execute the queued move/follow order
					function_00474760(unit, unit->orderQueueHead, OrderId::BuildingLiftoff);

			} //if(unit->orderQueueHead != NULL)

			//646CE

			//some units iterations, idle orders affectations,
			//and unknown order assignment?
			//maybe affect another selected unit depending on
			//attempt at giving order?
			function_00463640(unit);

			actUnitReturnToIdle(unit);

			addon = connectAddonCheck(unit);
			if(addon != NULL)
				completeAddon(unit, addon);

			//646EC
			if(*IS_PLACING_BUILDING) {

				placebuildingProc();

				//add the last minute occupation info
				//of landing building to update an
				//ongoing attempt to choose a building
				//or landing location?
				if( !function_0048DDA0() ) {

					refreshLayer3And4();

					//refresh various stuff, including
					//buttonset, dialogs...
					function_0048E310();

				}

			}

		} //if( (unit->getMovableState() != 0) && (unit->orderSignal & 0x10) )

	} //if(unit->mainOrderState == 3)

}  //void orders_BuildingLand(CUnit* unit)

;

void orders_BuildingLiftoff(CUnit* unit) {

	if( (unit->status & UnitStatus::GroundedBuilding) && 
		(units_dat::BaseProperty[unit->id] & UnitProperty::FlyingBuilding) 
		) 
	{

		//649D5
		if(unit->building.addon != NULL)
			disconnectFromAddOn(unit);

		if(unit->isFactory())
			unit->rally.unit = unit;

		//update globally known position and may
		//remove repulse tile if no longer needed
		function_0046A560(unit);

		//disable GroundedBuilding status and
		//prepare to enable pathing?
		function_00463470(unit);

		//64A05
		unit->sprite->elevationLevel = 12;
		setUnitPathing(unit,1);

		//maybe update orders targetting the unit,
		//set repulse for units in surroundings,and 
		//some others things?
		function_0046A5A0(unit);

		if(unit->sprite->images.tail->paletteType == PaletteType::RLE_SHADOW)
			unit->sprite->images.tail->flags |= CImage_Flags::DontUpdate;

		//equivalent to 64A40
		scbw::refreshConsole();

		if(	unit->sprite->position.x != unit->nextTargetWaypoint.x ||
			unit->sprite->position.y - 42 != unit->nextTargetWaypoint.y
		) 
		{
			//64A76
			unit->nextTargetWaypoint.x = unit->sprite->position.x;
			unit->nextTargetWaypoint.y = unit->sprite->position.y - 42;
		}

		//64A7E
		//probably update things related to creep since
		//there may be some under or around an infested
		//command center, return value unused
		if(unit->id == UnitId::ZergInfestedCommandCenter)
			function_0047DF90(unit);

		if(*IS_PLACING_BUILDING)
			placebuildingProc();

		unit->mainOrderId = OrderId::Liftoff;

	} //GroundedBuilding && FlyingBuilding
	else {

		//64AA2
		if(unit->orderQueueHead == NULL)
			unit->order(units_dat::ReturnToIdleOrder[unit->id], 0, 0, NULL, UnitId::None, 0);

		actUnitReturnToIdle(unit);

	}

} //void orders_BuildingLiftoff(CUnit* unit)

;

} //hooks

//-------- Helper function definitions. Do NOT modify! --------//

namespace {

const u32 Func_SetUnitPathing = 0x00401360;
void setUnitPathing(CUnit* unit, u8 unkPathRelated) {

	__asm {
		PUSHAD
		MOV AL, unkPathRelated
		MOV ECX, unit
		CALL Func_SetUnitPathing
		POPAD
	}

}

;

//Equivalent to function @ 0x00402160
bool isUnitPositions2Equal(CUnit* unit) {

	return (
		unit->nextTargetWaypoint.x == unit->position.x &&
		unit->nextTargetWaypoint.y == unit->position.y
	);


}

;

const u32 Func_ConnectAddonCheck = 0x004404A0;
CUnit* connectAddonCheck(CUnit* unit) {

	CUnit* found_addon;

	__asm {
		PUSHAD
		MOV ESI, unit
		CALL Func_ConnectAddonCheck
		MOV found_addon, EAX
		POPAD
	}

	return found_addon;

}

;

const u32 Func_Sub463470 = 0x00463470;
void function_00463470(CUnit* unit) {

	__asm {
		PUSHAD
		MOV EAX, unit
		CALL Func_Sub463470
		POPAD
	}

}

;

const u32 Func_Sub4634E0 = 0x004634E0;
void function_004634E0(CUnit* unit) {

	__asm {
		PUSHAD
		MOV EAX, unit
		CALL Func_Sub4634E0
		POPAD
	}

}

;

const u32 Func_Sub463640 = 0x00463640;
void function_00463640(CUnit* unit) {

	__asm {
		PUSHAD
		MOV EAX, unit
		CALL Func_Sub463640
		POPAD
	}

}

;

const u32 Func_CompleteAddon = 0x00463D50;
void completeAddon(CUnit* unit, CUnit* addon) {

	__asm {
		PUSHAD
		MOV EDI, unit
		MOV EAX, addon
		CALL Func_CompleteAddon
		POPAD
	}

}

;

const u32 Func_Sub464300 = 0x00464300;
void function_00464300(CUnit* unit) {

	__asm {
		PUSHAD
		MOV ECX, unit
		CALL Func_Sub464300
		POPAD
	}

}

;

const u32 Func_Sub464930 = 0x00464930;
void disconnectFromAddOn(CUnit* unit) {

  __asm {
    PUSHAD
    MOV EAX, unit
    CALL Func_Sub464930
    POPAD
  }

}

;

const u32 Func_Sub469EC0 = 0x00469EC0;
void function_00469EC0(CUnit* unit, int x, int y) {

	__asm {
		PUSHAD
		MOV EAX, unit
		PUSH y
		PUSH x
		CALL Func_Sub469EC0
		POPAD
	}

}

;

const u32 Func_Sub469F60 = 0x00469F60;
void function_00469F60(CUnit* unit, int x, int y) {

	__asm {
		PUSHAD
		MOV EAX, unit
		PUSH y
		PUSH x
		CALL Func_Sub469F60
		POPAD
	}

}

;

const u32 Func_Sub46A560 = 0x0046A560;
void function_0046A560(CUnit* unit) {

	__asm {
		PUSHAD
		MOV EAX, unit
		CALL Func_Sub46A560
		POPAD
	}

}

;

const u32 Func_Sub46A5A0 = 0x0046A5A0;
void function_0046A5A0(CUnit* unit) {

	__asm {
		PUSHAD
		MOV EAX, unit
		CALL Func_Sub46A5A0
		POPAD
	}

}

;

const u32 Func_Sub473FB0 = 0x00473FB0;
u32 function_00473FB0(CUnit* unit, u8 playerId, int x, int y, u16 unitId, u8 unk1, u8 unk2, u8 unk3, u8 unk4 ) {

	u32 return_value;

	__asm {
		PUSHAD
		MOVZX EAX, unk4		/*28*/
		PUSH EAX		
		MOVZX EAX, unk3		/*24*/
		PUSH EAX		
		MOVZX EAX, unk2		/*20*/
		PUSH EAX		
		MOVZX EAX, unk1		/*1C*/
		PUSH EAX		
		MOVZX EAX, unitId	/*18*/
		PUSH EAX		
		PUSH y				/*14*/
		PUSH x				/*10*/
		MOVZX EAX, playerId	/*0C*/
		PUSH EAX		
		PUSH unit			/*08*/
		CALL Func_Sub473FB0
		MOV return_value, EAX
		POPAD
	}

	return return_value;

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

const u32 Func_Sub474760 = 0x00474760;
void function_00474760(CUnit* unit, COrder* order, u8 orderId) {

	__asm {
		PUSHAD
		MOV ESI, unit
		MOV EDI, order
		MOV BL, orderId
		CALL Func_Sub474760
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

const u32 Func_Sub47DF90 = 0x0047DF90;
bool function_0047DF90(CUnit* unit) {

	Bool32 pre_return_value;

	__asm {
		PUSHAD
		PUSH unit
		CALL Func_Sub47DF90
		MOV pre_return_value, EAX
		POPAD
	}

	return (pre_return_value != 0);

}

;

const u32 Func_RefreshLayer3And4 = 0x0048D9A0;
void refreshLayer3And4() {

	__asm {
		PUSHAD
		CALL Func_RefreshLayer3And4
		POPAD
	}

}

;

const u32 Func_Sub48DDA0 = 0x0048DDA0;
bool function_0048DDA0() {

	Bool32 pre_return_value;

	__asm {
		PUSHAD
		CALL Func_Sub48DDA0
		MOV pre_return_value, EAX
		POPAD
	}

	return (pre_return_value != 0);

}

;

const u32 Func_Sub48E310 = 0x0048E310;
void function_0048E310() {

	__asm {
		PUSHAD
		CALL Func_Sub48E310
		POPAD
	}

}

;

const u32 Func_PracebuildingProc = 0x0048E6E0;
void placebuildingProc() {

	__asm {
		PUSHAD
		CALL Func_PracebuildingProc
		POPAD
	}

}

;

const u32 Func_PlayBuildingLandSound = 0x0048F5A0;
void playBuildingLandSound(CUnit* unit) {

	__asm {
		PUSHAD
		MOV EAX, unit
		CALL Func_PlayBuildingLandSound
		POPAD
	}

}

;

const u32 Func_SetUnitMovementSpeed = 0x004951C0;
void SetUnitMovementSpeed(CUnit* unit, u32 newSpeed) {

	__asm {
		PUSHAD
		MOV EAX, unit
		MOV EDX, newSpeed
		CALL Func_SetUnitMovementSpeed
		POPAD
	}

}

;

const u32 Func_Sub498150 = 0x00498150;
void function_00498150(CSprite* sprite, u8 newVerticalOffset) {

	__asm {
		PUSHAD
		MOV EAX, sprite
		MOV CL, newVerticalOffset
		CALL Func_Sub498150
		POPAD
	}

}

;

const u32 Func_Sub4997E0 = 0x004997E0;
void function_004997E0(CSprite* sprite) {

	__asm {
		PUSHAD
		MOV EAX, sprite
		CALL Func_Sub4997E0
		POPAD
	}

}

;

const u32 Func_Sub4C3B40 = 0x004C3B40;
void function_004C3B40(CUnit* unit) {

	__asm {
		PUSHAD
		MOV EDX, unit
		CALL Func_Sub4C3B40
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

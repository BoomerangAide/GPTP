#include "building_terran.h"
#include <SCBW/api.h>
#include <Events/Events.h>

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

//helper functions def

namespace {

bool isDistanceGreaterThanHaltDistance(CUnit* unit, int x, int y, u32 distance);	//01240
void fixTargetLocation(Point16* coords, u32 unitId);								//01FA0
bool isUnitPositions2Equal(CUnit* unit);											//02160
void AI_TrainingOverlord(CUnit* main_unit, CUnit* created_unit);					//35770
bool function_00467030(CUnit* unit);												//67030
CUnit* function_004678A0(CUnit* unit, u32 unitId);									//678A0
Bool32 buildingAddon(CUnit* unit, u32 hpGain, Bool32 canBeAborted);					//679A0
void function_00474760(CUnit* unit, COrder* order, u8 orderId);						//74760
void function_004749A0(CUnit* unit, COrder* order, u8 orderId);						//749A0
void refreshLayer3And4();															//8D9A0
bool function_0048DDA0();															//8DDA0
void function_0048E310();															//8E310
void playBuildingCompleteSound(CUnit* building);									//8F310
void function_00494BB0(CUnit* unit,int x, int y);									//94BB0
void displayLastNetErrForPlayer(u32 playerId);										//9E530
CUnit* createUnit(u32 unitId, int x, int y, u32 playerId);							//A09D0
u32 RandomizeShort(u32 unknown_index);												//DC4A0
u32 RandBetween(u32 min, u32 max, u32 someIndex);									//DC550
void function_004E65E0(CUnit* unit, Bool32 flag);									//E65E0
void setNextWaypoint_Sub4EB290(CUnit* unit);										//EB290
bool SetMoveTarget_xy(CUnit* unit, int x, int y);									//EB820
bool orderToMoveToTarget(CUnit* unit, CUnit* target);								//EB980
bool function_004EB9C0(CUnit* unit, int x, int y);									//EB9C0
Bool32 function_004F1870(CUnit* unit, int x, int y);								//F1870

} //unnamed namespace

namespace hooks {

void orders_TerranBuildSelf(CUnit* building) {

	if(building->mainOrderState == 0) {
		if(building->hitPoints > 20 * units_dat::MaxHitPoints[building->id] / 100)
			building->mainOrderState = 1;
	}
	else
	if(building->mainOrderState == 1) {

		CImage* current_image = building->sprite->images.head;

		while(current_image != NULL) {
			current_image->playIscriptAnim(IscriptAnimation::SpecialState1);
			current_image = current_image->link.next;
		}

		building->mainOrderState = 2;
	}
	else
	if(building->mainOrderState == 2) {
		if(building->hitPoints > 40 * units_dat::MaxHitPoints[building->id] / 100)
			building->mainOrderState = 3;
	}
	else
	if(building->mainOrderState == 3) {

		CImage* current_image = building->sprite->images.head;

		while(current_image != NULL) {
			current_image->playIscriptAnim(IscriptAnimation::SpecialState2);
			current_image = current_image->link.next;
		}

		building->mainOrderState = 4;

	}
	else
	if(building->mainOrderState == 4) {
		if(building->hitPoints > 60 * units_dat::MaxHitPoints[building->id] / 100)
			building->mainOrderState = 5;
	}
	else
	if(building->mainOrderState == 5) {

		function_004E65E0(building,0);

		CImage* current_image = building->sprite->images.head;

		while(current_image != NULL) {
			current_image->playIscriptAnim(IscriptAnimation::AlmostBuilt);
			current_image = current_image->link.next;
		}

		building->mainOrderState = 6;

	}
	else
	if(building->mainOrderState == 6) {

		if(building->hitPoints > 80 * units_dat::MaxHitPoints[building->id] / 100)
			building->orderComputerCL(OrderId::Nothing2);

	}

}

;

//Go to build on an existing building
#ifdef EVENTS_SYSTEM
void orders_SCVBuild2(CUnit* unit) {

	CUnit* builtUnit = unit->orderTarget.unit;

	if(builtUnit != NULL && !(builtUnit->status & UnitStatus::Completed)) {

		bool jump_to_switch_1 = false;

		if(
			unit->mainOrderState > 8 ||
			unit->mainOrderState == 1
		)
			jump_to_switch_1 = true;

		if(unit->mainOrderState == 0) {
			if(orderToMoveToTarget(unit,builtUnit))
				unit->mainOrderState = 2;
		}
		
		if(unit->mainOrderState == 2) {

			u32 movableState = unit->getMovableState();

			if(movableState == MovableState::UnmovableAtDestination)
				unit->orderToIdle();
			else
			if(movableState != MovableState::NotReachedDestination) {

				scbw::refreshConsole();

				if (
					builtUnit->connectedUnit != NULL &&
					builtUnit->connectedUnit != unit &&
					(builtUnit->connectedUnit)->orderTarget.unit == builtUnit
				)
					unit->orderToIdle(); //another SCV is already on the job
				else { //67AF9

					builtUnit->connectedUnit = unit;
					unit->status &= ~UnitStatus::IsNormal;	//remove UnitStatus::IsNormal if present
					unit->sprite->elevationLevel = units_dat::Elevation[unit->id] + 1;

					unit->orderSimple(OrderId::ResetCollision1,false);
					unit->orderSimple(units_dat::ReturnToIdleOrder[unit->id],false);

					if (
						function_004EB9C0(	
							unit,
							builtUnit->sprite->position.x,
							builtUnit->sprite->position.y
						)
					)
						unit->mainOrderState = 3;

				}

			}

		} //if(unit->mainOrderState == 2)
		else
		if(unit->mainOrderState == 3) {
			if(unit->getMovableState() != MovableState::NotReachedDestination)
				unit->mainOrderState = 4;
		}
		else
		if(unit->mainOrderState == 4) {

			s16 x_min,y_min,x_max,y_max;
			s32 dimX, dimY;

			Box16 temp_box;

			temp_box.left = builtUnit->sprite->position.x - builtUnit->sprite->unkflags_12 / 2;
			temp_box.right = builtUnit->sprite->position.x + builtUnit->sprite->unkflags_12 / 2 - 1;
			temp_box.top = builtUnit->sprite->position.y - builtUnit->sprite->unkflags_13 / 2;
			temp_box.bottom = builtUnit->sprite->position.y + builtUnit->sprite->unkflags_13 / 2 - 1;

			dimX = (s16)units_dat::BuildingDimensions[builtUnit->id].x;

			if(dimX < 0)
				x_min = builtUnit->sprite->position.x - (dimX + 1) / 2;
			else
				x_min = builtUnit->sprite->position.x - dimX / 2;

			x_max = dimX + x_min - 1;

			dimY = (s16)units_dat::BuildingDimensions[builtUnit->id].y;

			if(dimY < 0)
				y_min = builtUnit->sprite->position.y - (dimY + 1) / 2;
			else
				y_min = builtUnit->sprite->position.y - dimY / 2;

			y_max = dimY + y_min - 1;

			x_min = x_min + units_dat::UnitBounds[unit->id].left + 1;
			x_max = x_max - (units_dat::UnitBounds[unit->id].right + 1);
			y_min = y_min + units_dat::UnitBounds[unit->id].top + 1;
			y_max = y_max - (units_dat::UnitBounds[unit->id].bottom + 1);

			if(x_min < temp_box.left)
				x_min = temp_box.left;

			if(y_min < temp_box.top)
				y_min = temp_box.top;

			if(x_max > temp_box.right)
				x_max = temp_box.right;

			if(y_max > temp_box.bottom)
				y_max = temp_box.bottom;

			unit->orderTarget.pt.x = RandBetween(temp_box.left,temp_box.right,9);
			unit->orderTarget.pt.y = RandBetween(temp_box.top,temp_box.bottom,9);

			fixTargetLocation(&unit->orderTarget.pt,unit->id);

			if(
				function_004F1870(
					unit,
					unit->orderTarget.pt.x - unit->sprite->position.x,
					unit->orderTarget.pt.y - unit->sprite->position.y
				) != 0
			) 
			{

				unit->orderTarget.pt.x = RandBetween(x_min,x_max,9);
				unit->orderTarget.pt.y = RandBetween(y_min,y_max,9);

				fixTargetLocation(&unit->orderTarget.pt,unit->id);

			}

			if(function_004EB9C0(unit,unit->orderTarget.pt.x,unit->orderTarget.pt.y))
			{
				unit->mainOrderState = 5;
				jump_to_switch_1 = true;
			}

		} //mainOrderState == 4
		else
		if(unit->mainOrderState == 5) {
			unit->mainOrderState = 6;
			jump_to_switch_1 = true;
		}
		else
		if(unit->mainOrderState == 6) {

			if(!isDistanceGreaterThanHaltDistance(
					unit,
					unit->orderTarget.pt.x,
					unit->orderTarget.pt.y,
					20 //0x14
				)
			)
				jump_to_switch_1 = true;
			else {

				s32 angle;

				setNextWaypoint_Sub4EB290(unit);

				function_00494BB0(
					unit,
					(unit->orderTarget.unit)->sprite->position.x,
					(unit->orderTarget.unit)->sprite->position.y
				);

				angle = scbw::getAngle(
					unit->sprite->position.x,
					unit->sprite->position.y,
					unit->nextTargetWaypoint.x,
					unit->nextTargetWaypoint.y
				);

				unit->orderTarget.pt.x = (angleDistance[angle].x * 20) / 256 + unit->sprite->position.x;
				unit->orderTarget.pt.y = (angleDistance[angle].y * 20) / 256 + unit->sprite->position.y;
				unit->mainOrderState = 7;
				jump_to_switch_1 = true;

			}

		}
		else
		if(unit->mainOrderState == 7) {

			if(!isUnitPositions2Equal(unit)) {

				s32 angle = 
					scbw::getAngle(
						unit->sprite->position.x,
						unit->sprite->position.y,
						unit->nextTargetWaypoint.x,
						unit->nextTargetWaypoint.y
					);

				angle = unit->currentDirection1 - angle;

				if(angle < 0)
					angle += 256;

				if (angle > 128)
					angle = 256 - angle;

				if(angle > weapons_dat::AttackAngle[unit->getGroundWeapon()])
					jump_to_switch_1 = true;
				else { //67E4A

					u8 rand_value = RandomizeShort(10);
					CImage* current_image = unit->sprite->images.head;

					unit->mainOrderTimer = (rand_value & 63) + 30; //63==0x3F, 30==0x1E

					while(current_image != NULL) {
						current_image->playIscriptAnim(IscriptAnimation::AlmostBuilt);
						current_image = current_image->link.next;
					}

					unit->mainOrderState = 8;
					jump_to_switch_1 = true;

				}


			}
			else { //67E4A

				u8 rand_value = RandomizeShort(10);
				CImage* current_image = unit->sprite->images.head;

				unit->mainOrderTimer = (rand_value & 63) + 30; //63==0x3F, 30==0x1E

				while(current_image != NULL) {
					current_image->playIscriptAnim(IscriptAnimation::AlmostBuilt); //SCV got that animation indeed
					current_image = current_image->link.next;
				}

				unit->mainOrderState = 8;
				jump_to_switch_1 = true;

			}

		}
		else
		if(unit->mainOrderState == 8) {

			if(unit->mainOrderTimer != 0)
				jump_to_switch_1 = true;
			else {

				CImage* current_image = unit->sprite->images.head;

				while(current_image != NULL) {
					current_image->playIscriptAnim(IscriptAnimation::GndAttkToIdle);
					current_image = current_image->link.next;
				}

				unit->mainOrderState = 4;

				jump_to_switch_1 = true;

			}

		}

		if(jump_to_switch_1) {

			std::vector<int*> events_building_terran_hp_add_arg(3);
			u32 hpGain = builtUnit->buildRepairHpGain;

			if(*CHEAT_STATE & CheatFlags::OperationCwal)
				hpGain *= 16;

			events_building_terran_hp_add_arg[0] = (int*)builtUnit;
			events_building_terran_hp_add_arg[1] = (int*)unit;
			events_building_terran_hp_add_arg[2] = (int*)&hpGain;

			EventManager::EventCalled(EventId::BUILDING_TERRAN_CONSTRUCTION_HP_ADD, events_building_terran_hp_add_arg);

			buildingAddon(builtUnit,hpGain,FALSE);

			if(builtUnit->status & UnitStatus::Completed) {

				CImage* current_image = unit->sprite->images.head;
				bool bEndThere = false;

				while(current_image != NULL) {
					current_image->playIscriptAnim(IscriptAnimation::WalkingToIdle);
					current_image = current_image->link.next;
				}

				if(builtUnit != NULL) {

					bool jump_to_67F3C = false;

					builtUnit->connectedUnit = NULL;
					playBuildingCompleteSound(unit); //sound come from SCV, not building

					if(builtUnit->status & UnitStatus::Completed) {

						if(
							builtUnit->id == UnitId::TerranRefinery ||
							builtUnit->id == UnitId::ProtossAssimilator ||
							builtUnit->id == UnitId::ZergExtractor
						)
						{
							if(builtUnit->playerId == unit->playerId)
								jump_to_67F3C = true;
						}

					}

					if(!jump_to_67F3C) { //67F2C

						if(	
							builtUnit->id >= UnitId::ResourceMineralField &&
							builtUnit->id <= UnitId::ResourceMineralFieldType3
						)
							jump_to_67F3C = true;

					}

					if(jump_to_67F3C) {
						unit->orderTo(OrderId::Harvest1,builtUnit);
						bEndThere = true;
					}

				}

				if(!bEndThere) { //67F4C

					if(unit->orderQueueHead != NULL) {
						unit->userActionFlags |= 1;
						prepareForNextOrder(unit);
					}
					else 
					if(unit->pAI != NULL) //67F6F
						unit->orderComputerCL(OrderId::ComputerAI);
					else
						unit->orderComputerCL(units_dat::ReturnToIdleOrder[unit->id]);

				}

			}

		}

	}
	else { //67ED0

		CImage* current_image = unit->sprite->images.head;
		bool bEndThere = false;

		while(current_image != NULL) {
			current_image->playIscriptAnim(IscriptAnimation::WalkingToIdle);
			current_image = current_image->link.next;
		}

		if(builtUnit != NULL) {

			bool jump_to_67F3C = false;

			builtUnit->connectedUnit = NULL;
			playBuildingCompleteSound(unit); //sound come from SCV, not building

			if(builtUnit->status & UnitStatus::Completed) {

				if(
					builtUnit->id == UnitId::TerranRefinery ||
					builtUnit->id == UnitId::ProtossAssimilator ||
					builtUnit->id == UnitId::ZergExtractor
				)
				{
					if(builtUnit->playerId == unit->playerId)
						jump_to_67F3C = true;
				}

			}

			if(!jump_to_67F3C) { //67F2C

				if(	
					builtUnit->id >= UnitId::ResourceMineralField &&
					builtUnit->id <= UnitId::ResourceMineralFieldType3
				)
					jump_to_67F3C = true;

			}

			if(jump_to_67F3C) {
				unit->orderTo(OrderId::Harvest1,builtUnit);
				bEndThere = true;
			}

		}

		if(!bEndThere) { //67F4C

			if(unit->orderQueueHead != NULL) {
				unit->userActionFlags |= 1;
				prepareForNextOrder(unit);
			}
			else 
			if(unit->pAI != NULL) //67F6F
				unit->orderComputerCL(OrderId::ComputerAI);
			else
				unit->orderComputerCL(units_dat::ReturnToIdleOrder[unit->id]);

		}

	}

}
#else
void orders_SCVBuild2(CUnit* unit) {

	CUnit* builtUnit = unit->orderTarget.unit;

	if(builtUnit != NULL && !(builtUnit->status & UnitStatus::Completed)) {

		bool jump_to_switch_1 = false;

		if(
			unit->mainOrderState > 8 ||
			unit->mainOrderState == 1
		)
			jump_to_switch_1 = true;

		if(unit->mainOrderState == 0) {
			if(orderToMoveToTarget(unit,builtUnit))
				unit->mainOrderState = 2;
		}
		
		if(unit->mainOrderState == 2) {

			u32 movableState = unit->getMovableState();

			if(movableState == MovableState::UnmovableAtDestination)
				unit->orderToIdle();
			else
			if(movableState != MovableState::NotReachedDestination) {

				scbw::refreshConsole();

				if (
					builtUnit->connectedUnit != NULL &&
					builtUnit->connectedUnit != unit &&
					(builtUnit->connectedUnit)->orderTarget.unit == builtUnit
				)
					unit->orderToIdle(); //another SCV is already on the job
				else { //67AF9

					builtUnit->connectedUnit = unit;
					unit->status &= ~UnitStatus::IsNormal;	//remove UnitStatus::IsNormal if present
					unit->sprite->elevationLevel = units_dat::Elevation[unit->id] + 1;

					unit->orderSimple(OrderId::ResetCollision1,false);
					unit->orderSimple(units_dat::ReturnToIdleOrder[unit->id],false);

					if (
						function_004EB9C0(	
							unit,
							builtUnit->sprite->position.x,
							builtUnit->sprite->position.y
						)
					)
						unit->mainOrderState = 3;

				}

			}

		} //if(unit->mainOrderState == 2)
		else
		if(unit->mainOrderState == 3) {
			if(unit->getMovableState() != MovableState::NotReachedDestination)
				unit->mainOrderState = 4;
		}
		else
		if(unit->mainOrderState == 4) {

			s16 x_min,y_min,x_max,y_max;
			s32 dimX, dimY;

			Box16 temp_box;

			temp_box.left = builtUnit->sprite->position.x - builtUnit->sprite->unkflags_12 / 2;
			temp_box.right = builtUnit->sprite->position.x + builtUnit->sprite->unkflags_12 / 2 - 1;
			temp_box.top = builtUnit->sprite->position.y - builtUnit->sprite->unkflags_13 / 2;
			temp_box.bottom = builtUnit->sprite->position.y + builtUnit->sprite->unkflags_13 / 2 - 1;

			dimX = (s16)units_dat::BuildingDimensions[builtUnit->id].x;

			if(dimX < 0)
				x_min = builtUnit->sprite->position.x - (dimX + 1) / 2;
			else
				x_min = builtUnit->sprite->position.x - dimX / 2;

			x_max = dimX + x_min - 1;

			dimY = (s16)units_dat::BuildingDimensions[builtUnit->id].y;

			if(dimY < 0)
				y_min = builtUnit->sprite->position.y - (dimY + 1) / 2;
			else
				y_min = builtUnit->sprite->position.y - dimY / 2;

			y_max = dimY + y_min - 1;

			x_min = x_min + units_dat::UnitBounds[unit->id].left + 1;
			x_max = x_max - (units_dat::UnitBounds[unit->id].right + 1);
			y_min = y_min + units_dat::UnitBounds[unit->id].top + 1;
			y_max = y_max - (units_dat::UnitBounds[unit->id].bottom + 1);

			if(x_min < temp_box.left)
				x_min = temp_box.left;

			if(y_min < temp_box.top)
				y_min = temp_box.top;

			if(x_max > temp_box.right)
				x_max = temp_box.right;

			if(y_max > temp_box.bottom)
				y_max = temp_box.bottom;

			unit->orderTarget.pt.x = RandBetween(temp_box.left,temp_box.right,9);
			unit->orderTarget.pt.y = RandBetween(temp_box.top,temp_box.bottom,9);

			fixTargetLocation(&unit->orderTarget.pt,unit->id);

			if(
				function_004F1870(
					unit,
					unit->orderTarget.pt.x - unit->sprite->position.x,
					unit->orderTarget.pt.y - unit->sprite->position.y
				) != 0
			) 
			{

				unit->orderTarget.pt.x = RandBetween(x_min,x_max,9);
				unit->orderTarget.pt.y = RandBetween(y_min,y_max,9);

				fixTargetLocation(&unit->orderTarget.pt,unit->id);

			}

			if(function_004EB9C0(unit,unit->orderTarget.pt.x,unit->orderTarget.pt.y))
			{
				unit->mainOrderState = 5;
				jump_to_switch_1 = true;
			}

		} //mainOrderState == 4
		else
		if(unit->mainOrderState == 5) {
			unit->mainOrderState = 6;
			jump_to_switch_1 = true;
		}
		else
		if(unit->mainOrderState == 6) {

			if(!isDistanceGreaterThanHaltDistance(
					unit,
					unit->orderTarget.pt.x,
					unit->orderTarget.pt.y,
					20 //0x14
				)
			)
				jump_to_switch_1 = true;
			else {

				s32 angle;

				setNextWaypoint_Sub4EB290(unit);

				function_00494BB0(
					unit,
					(unit->orderTarget.unit)->sprite->position.x,
					(unit->orderTarget.unit)->sprite->position.y
				);

				angle = scbw::getAngle(
					unit->sprite->position.x,
					unit->sprite->position.y,
					unit->nextTargetWaypoint.x,
					unit->nextTargetWaypoint.y
				);

				unit->orderTarget.pt.x = (angleDistance[angle].x * 20) / 256 + unit->sprite->position.x;
				unit->orderTarget.pt.y = (angleDistance[angle].y * 20) / 256 + unit->sprite->position.y;
				unit->mainOrderState = 7;
				jump_to_switch_1 = true;

			}

		}
		else
		if(unit->mainOrderState == 7) {

			if(!isUnitPositions2Equal(unit)) {

				s32 angle = 
					scbw::getAngle(
						unit->sprite->position.x,
						unit->sprite->position.y,
						unit->nextTargetWaypoint.x,
						unit->nextTargetWaypoint.y
					);

				angle = unit->currentDirection1 - angle;

				if(angle < 0)
					angle += 256;

				if (angle > 128)
					angle = 256 - angle;

				if(angle > weapons_dat::AttackAngle[unit->getGroundWeapon()])
					jump_to_switch_1 = true;
				else { //67E4A

					u8 rand_value = RandomizeShort(10);
					CImage* current_image = unit->sprite->images.head;

					unit->mainOrderTimer = (rand_value & 63) + 30; //63==0x3F, 30==0x1E

					while(current_image != NULL) {
						current_image->playIscriptAnim(IscriptAnimation::AlmostBuilt);
						current_image = current_image->link.next;
					}

					unit->mainOrderState = 8;
					jump_to_switch_1 = true;

				}


			}
			else { //67E4A

				u8 rand_value = RandomizeShort(10);
				CImage* current_image = unit->sprite->images.head;

				unit->mainOrderTimer = (rand_value & 63) + 30; //63==0x3F, 30==0x1E

				while(current_image != NULL) {
					current_image->playIscriptAnim(IscriptAnimation::AlmostBuilt); //SCV got that animation indeed
					current_image = current_image->link.next;
				}

				unit->mainOrderState = 8;
				jump_to_switch_1 = true;

			}

		}
		else
		if(unit->mainOrderState == 8) {

			if(unit->mainOrderTimer != 0)
				jump_to_switch_1 = true;
			else {

				CImage* current_image = unit->sprite->images.head;

				while(current_image != NULL) {
					current_image->playIscriptAnim(IscriptAnimation::GndAttkToIdle);
					current_image = current_image->link.next;
				}

				unit->mainOrderState = 4;

				jump_to_switch_1 = true;

			}

		}

		if(jump_to_switch_1) {

			u32 hpGain = builtUnit->buildRepairHpGain;

			if(*CHEAT_STATE & CheatFlags::OperationCwal)
				hpGain *= 16;

			buildingAddon(builtUnit,hpGain,FALSE);

			if(builtUnit->status & UnitStatus::Completed) {

				CImage* current_image = unit->sprite->images.head;
				bool bEndThere = false;

				while(current_image != NULL) {
					current_image->playIscriptAnim(IscriptAnimation::WalkingToIdle);
					current_image = current_image->link.next;
				}

				if(builtUnit != NULL) {

					bool jump_to_67F3C = false;

					builtUnit->connectedUnit = NULL;
					playBuildingCompleteSound(unit); //sound come from SCV, not building

					if(builtUnit->status & UnitStatus::Completed) {

						if(
							builtUnit->id == UnitId::TerranRefinery ||
							builtUnit->id == UnitId::ProtossAssimilator ||
							builtUnit->id == UnitId::ZergExtractor
						)
						{
							if(builtUnit->playerId == unit->playerId)
								jump_to_67F3C = true;
						}

					}

					if(!jump_to_67F3C) { //67F2C

						if(	
							builtUnit->id >= UnitId::ResourceMineralField &&
							builtUnit->id <= UnitId::ResourceMineralFieldType3
						)
							jump_to_67F3C = true;

					}

					if(jump_to_67F3C) {
						unit->orderTo(OrderId::Harvest1,builtUnit);
						bEndThere = true;
					}

				}

				if(!bEndThere) { //67F4C

					if(unit->orderQueueHead != NULL) {
						unit->userActionFlags |= 1;
						prepareForNextOrder(unit);
					}
					else 
					if(unit->pAI != NULL) //67F6F
						unit->orderComputerCL(OrderId::ComputerAI);
					else
						unit->orderComputerCL(units_dat::ReturnToIdleOrder[unit->id]);

				}

			}

		}

	}
	else { //67ED0

		CImage* current_image = unit->sprite->images.head;
		bool bEndThere = false;

		while(current_image != NULL) {
			current_image->playIscriptAnim(IscriptAnimation::WalkingToIdle);
			current_image = current_image->link.next;
		}

		if(builtUnit != NULL) {

			bool jump_to_67F3C = false;

			builtUnit->connectedUnit = NULL;
			playBuildingCompleteSound(unit); //sound come from SCV, not building

			if(builtUnit->status & UnitStatus::Completed) {

				if(
					builtUnit->id == UnitId::TerranRefinery ||
					builtUnit->id == UnitId::ProtossAssimilator ||
					builtUnit->id == UnitId::ZergExtractor
				)
				{
					if(builtUnit->playerId == unit->playerId)
						jump_to_67F3C = true;
				}

			}

			if(!jump_to_67F3C) { //67F2C

				if(	
					builtUnit->id >= UnitId::ResourceMineralField &&
					builtUnit->id <= UnitId::ResourceMineralFieldType3
				)
					jump_to_67F3C = true;

			}

			if(jump_to_67F3C) {
				unit->orderTo(OrderId::Harvest1,builtUnit);
				bEndThere = true;
			}

		}

		if(!bEndThere) { //67F4C

			if(unit->orderQueueHead != NULL) {
				unit->userActionFlags |= 1;
				prepareForNextOrder(unit);
			}
			else 
			if(unit->pAI != NULL) //67F6F
				unit->orderComputerCL(OrderId::ComputerAI);
			else
				unit->orderComputerCL(units_dat::ReturnToIdleOrder[unit->id]);

		}

	}

}
#endif

;

//Go to build a not yet existing building
//Switch to orders_SCVBuild2 for construction itself
void orders_SCVBuild(CUnit* unit) {

	bool bEndThere = false;

	if(unit->pAI != NULL)
		unit->status |= UnitStatus::IsGathering;

	if (unit->mainOrderState == 1) {

		if (
			unit->sprite->position.x == unit->moveTarget.pt.x &&
			unit->sprite->position.y == unit->moveTarget.pt.y
		)
		{

			/*Ignored a bugged unit->status & UnitStatus::Unmovable check here*/

			if (
				!(unit->status & UnitStatus::Unmovable) &&
				isDistanceGreaterThanHaltDistance(
					unit,
					unit->orderTarget.pt.x,
					unit->orderTarget.pt.y,
					128 //0x80
				) &&
				function_00467030(unit)
			)
			{

				CUnit* builtUnit;

				unit->status &= ~UnitStatus::IsNormal;
				unit->sprite->elevationLevel = units_dat::Elevation[unit->id] + 1;

				if (unit->orderQueueHead == NULL) { //680A5
					function_004749A0(unit, NULL, OrderId::ResetCollision1);
					function_004749A0(unit, NULL, units_dat::ReturnToIdleOrder[unit->id]);
				}
				else
					function_00474760(unit, unit->orderQueueHead, OrderId::ResetCollision1);

				if (unit->buildQueue[unit->buildQueueSlot] == UnitId::TerranRefinery)
					builtUnit = function_004678A0(unit, UnitId::TerranRefinery);
				else
					builtUnit = createUnit(
						unit->buildQueue[unit->buildQueueSlot],
						unit->orderTarget.pt.x,
						unit->orderTarget.pt.y,
						unit->playerId
					);

				unit->buildQueue[unit->buildQueueSlot] = UnitId::None;

				if (builtUnit == NULL) {
					displayLastNetErrForPlayer(unit->playerId);
					unit->orderComputerCL(units_dat::ReturnToIdleOrder[unit->id]);
					bEndThere = true;
				}
				else { //68131

					builtUnit->connectedUnit = unit;
					unit->mainOrderId = OrderId::ConstructingBuilding;
					unit->mainOrderState = 3;
					unit->orderTarget.unit = builtUnit;

					if (*IS_PLACING_BUILDING) {
						if (!function_0048DDA0()) {
							refreshLayer3And4();
							function_0048E310();
						}
					}

					scbw::refreshConsole();
					builtUnit->orderComputerCL(OrderId::BuildSelf1);
					AI_TrainingOverlord(unit, builtUnit);
					bEndThere = true;

				}

			}

			if (!bEndThere)
				unit->orderToIdle();

		}

	}
	else
	if(unit->mainOrderState != 0)
		bEndThere = true;

	if(!bEndThere) { //68188

		bool bMoveTargetResult;

		scbw::refreshConsole();

		if(unit->orderTarget.unit != NULL) {
			unit->orderTarget.pt.x = (unit->orderTarget.unit)->sprite->position.x;
			unit->orderTarget.pt.y = (unit->orderTarget.unit)->sprite->position.y;
		}

		bMoveTargetResult = SetMoveTarget_xy(unit,unit->orderTarget.pt.x,unit->orderTarget.pt.y);

		if(
			unit->orderTarget.pt.x != unit->nextTargetWaypoint.x ||
			unit->orderTarget.pt.y != unit->nextTargetWaypoint.y
			) 
		{
			unit->nextTargetWaypoint.x = unit->orderTarget.pt.x;
			unit->nextTargetWaypoint.y = unit->orderTarget.pt.y;
		}

		if(bMoveTargetResult)
			unit->mainOrderState = 1;

	}

}

;

} //namespace hooks

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

const u32 Func_fixTargetLocation = 0x00401FA0;
void fixTargetLocation(Point16* coords, u32 unitId) {

	__asm {
		PUSHAD
		MOV EAX, unitId
		MOV EDX, coords
		CALL Func_fixTargetLocation
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

const u32 Func_buildingAddon = 0x004679A0;
//if canBeAborted is TRUE, the unit can be aborted because there's
//no room around the constructing building
Bool32 buildingAddon(CUnit* unit, u32 hpGain, Bool32 canBeAborted) {

	static Bool32 bResult;

	__asm {
		PUSHAD
		MOV EDX, hpGain
		MOV EAX, unit
		PUSH canBeAborted
		CALL Func_buildingAddon
		MOV bResult, EAX
		POPAD
	}

	return bResult;

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

const u32 Func_Sub4749A0 = 0x004749A0;
void function_004749A0(CUnit* unit, COrder* order, u8 orderId) {

	__asm {
		PUSHAD
		MOV BL, orderId
		MOV ESI, unit
		MOV EDI, order
		CALL Func_Sub4749A0
		POPAD
	}

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

const u32 Func_Sub494BB0 = 0x00494BB0;
void function_00494BB0(CUnit* unit,int x, int y) {

	__asm {
		PUSHAD
		MOV ECX, y
		MOV EDX, x
		MOV EAX, unit
		CALL Func_Sub494BB0
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

const u32 Func_RandomizeShort = 0x004DC4A0;
u32 RandomizeShort(u32 unknown_index) {

	static u32 return_value;

	__asm {
		PUSHAD
		MOV EAX, unknown_index
		CALL Func_RandomizeShort
		MOV return_value, EAX
		POPAD
	}

	return return_value;

}

;

const u32 Func_RandBetween = 0x004DC550;
u32 RandBetween(u32 min, u32 max, u32 someIndex) {

	static u32 return_value;

	__asm {
		PUSHAD
		PUSH max
		MOV EAX, someIndex
		MOV EDX, min
		CALL Func_RandBetween
		MOV return_value, EAX
		POPAD
	}

	return return_value;

}

;

const u32 Func_Sub4E65E0 = 0x004E65E0;
void function_004E65E0(CUnit* unit, Bool32 flag) {

	__asm {
		PUSHAD
		MOV EDI, unit
		PUSH flag
		CALL Func_Sub4E65E0
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

const u32 Func__moveToTarget = 0x004EB980;
bool orderToMoveToTarget(CUnit* unit, CUnit* target) {

	static Bool32 bPreResult;
  
	__asm {
		PUSHAD
		MOV EAX, target
		MOV ECX, unit
		CALL Func__moveToTarget
		MOV bPreResult, EAX
		POPAD
	}

	return bPreResult != 0;

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

const u32 Func_Sub4F1870 = 0x004F1870;
Bool32 function_004F1870(CUnit* unit, int x, int y) {

	static Bool32 rValue;

	__asm {
		PUSHAD
		PUSH y
		PUSH x
		MOV EAX, unit
		CALL Func_Sub4F1870
		MOV rValue, EAX
		POPAD
	}

	return rValue;

}

;

} //Unnamed namespace

//End of helper functions

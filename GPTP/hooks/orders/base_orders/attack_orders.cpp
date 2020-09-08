#include "attack_orders.h"
#include <SCBW/api.h>

//helper functions def

namespace {

bool isDistanceGreaterThanHaltDistance(CUnit* unit, int x, int y, u32 distance);	//01240
bool unitCantSeeCloakedTarget(CUnit* unit, CUnit* target);							//01D60
bool isInfestableUnit(CUnit* unit);													//02210
bool unitCanInfest(CUnit* unit);													//02750
void AI_AttackUnit(CUnit* unit);													//3FFD0
CUnit* findBestAttackTarget(CUnit* unit);											//43080
bool function_00462EA0(CUnit* unit, u8 unknownByteValue);							//62EA0
void function_00465780(CUnit* unit);												//65780
bool function_00465810(CUnit* unit);												//65810
bool function_00465A60(CUnit* unit, int range1, int range2);						//65A60
void function_0046D30(CUnit* unit, u16 x, u16 y, u8 unk);							//65D30
CUnit* launchInterceptorScarab(CUnit* unit);										//66440
void removeOrderFromUnitQueue(CUnit* unit, COrder* order);							//742D0
void function_00474A70(CUnit* unit, CUnit* target, u8 orderId);						//74A70
bool isTargetWithinMinMovementRange(CUnit* unit, CUnit* target, u32 range);			//763D0
bool function_00476610(CUnit* unit, int x, int y);									//76610
bool isUnitInWeaponRange(CUnit* unit, CUnit* target);								//76870
void getWeaponBeginIscript(CUnit* unit, IscriptAnimation::Enum anim);				//76ED0
void function_00477510(CUnit* unit);												//77510
void function_00477820(CUnit* unit, u8 orderId);									//77820
bool function_00478370(CUnit* unit, u8 orderId);									//78370
bool function_00478540(CUnit* unit);												//78540
Bool32 attackApplyCooldown(CUnit* unit);											//78B40
bool isUnitCritter(CUnit* unit);													//7B450
void function_00495400(CUnit* unit, CUnit* target);									//95400
u16 SAI_GetRegionIdFromPxEx(s32 x, s32 y);											//9C9F0
bool isUnitVisible(CUnit* unit);													//E5DB0
bool isTargetVisible(CUnit* unit, CUnit* target);									//E5E30
u8 getRightClickActionOrder(CUnit* unit);											//E5EA0
void setNextWaypoint_Sub4EB290(CUnit* unit);										//EB290
void makeToHoldPosition(CUnit* unit);												//EB5B0
bool moveToTarget(CUnit* unit, CUnit* target);										//EB720
bool SetMoveTarget_xy(CUnit* unit, int x, int y);									//EB820
bool function_004EB900(CUnit* unit, CUnit* target);									//EB900
bool function_004EB9C0(CUnit* unit, int x, int y);									//EB9C0

} //unnamed namespace

namespace hooks {

void orders_ReaverAttack1(CUnit* unit) {

	CUnit* target = unit->orderTarget.unit;

	if(target == NULL) {

		if(unit->orderQueueHead != NULL) {
			//656A2:
			unit->userActionFlags |= 1;
			prepareForNextOrder(unit);
		}
		else {
			//656B2:
			if(unit->pAI != NULL)
				unit->orderComputerCL(OrderId::ComputerAI);
			else
				//656C6:
				unit->orderComputerCL(units_dat::ReturnToIdleOrder[unit->id]);
		}

	}
	else {

		//656D8:
		if(!unit->canAttackTarget(target,true))
			unit->orderTo(OrderId::Move, unit->orderTarget.pt.x, unit->orderTarget.pt.y);
		else {

			//656F7:
			u32 status_saved = (unit->status & UnitStatus::HoldingPosition) + (unit->status & UnitStatus::Disabled);
			u8 newOrderId;

			if(unit->id == UnitId::ProtossReaver || unit->id == UnitId::Hero_Warbringer) {
				//65720:
				if(unit->mainOrderId == OrderId::ReaverHold)
					newOrderId = OrderId::ReaverHold;
				else
					newOrderId = OrderId::ReaverFight;
			}
			else {
				//65730
				if(unit->mainOrderId == OrderId::HoldPosition1)
					newOrderId = OrderId::HoldPosition1;
				else
					newOrderId = OrderId::CarrierFight;
			}

			//6573E:
			function_00474A70(unit,target,newOrderId);
			prepareForNextOrder(unit);

			//restore the "disabled" and "holding position" status if they were lost
			unit->status &= ~(UnitStatus::Disabled + UnitStatus::HoldingPosition);
			unit->status = status_saved | unit->status;

		}

	}

} //void orders_ReaverAttack1(CUnit* unit)

;

void orders_CarrierAttack1(CUnit* unit) {

	CUnit* target = unit->orderTarget.unit;

	if(target == NULL) {

		if(unit->orderQueueHead != NULL) {
			//65966:
			unit->userActionFlags |= 1;
			prepareForNextOrder(unit);
		}
		else {
			//65981
			if(unit->pAI != NULL)
				unit->orderComputerCL(OrderId::ComputerAI);
			else //65998
				unit->orderComputerCL(units_dat::ReturnToIdleOrder[unit->id]);
		}

	}
	else {

		//659AD:
		if(!unit->canAttackTarget(target,true))
			unit->orderTo(OrderId::Move, unit->orderTarget.pt.x, unit->orderTarget.pt.y);
		else {

			//659CF
			u32 status_saved = (unit->status & UnitStatus::Disabled) + (unit->status & UnitStatus::HoldingPosition);
			u8 newOrderId;

			//give orders to interceptors/scarabs (through outHangarChild)
			function_00465780(unit);

			if(unit->id == UnitId::ProtossReaver || unit->id == UnitId::Hero_Warbringer) {
				//65A03:
				if(unit->mainOrderId == OrderId::ReaverHold)
					newOrderId = OrderId::ReaverHold;
				else
					newOrderId = OrderId::ReaverFight;
			}
			else {
				//65A13:
				if(unit->mainOrderId == OrderId::HoldPosition1)
					newOrderId = OrderId::HoldPosition1;
				else
					newOrderId = OrderId::CarrierFight;
			}

			//65A21:

			function_00474A70(unit,target,newOrderId);
			prepareForNextOrder(unit);

			//restore the "disabled" and "holding position" status if they were lost
			unit->status &= ~(UnitStatus::Disabled + UnitStatus::HoldingPosition);
			unit->status = status_saved | unit->status;

		}

	}

} //void orders_CarrierAttack1(CUnit* unit)

;

//Attack order of Reavers Scarabs
void orders_StrafeUnit2(CUnit* unit) {
	
	if (unit->mainOrderState <= 6) {

		if (unit->mainOrderState == 0) {

			if (unit->removeTimer == 0 || unit->removeTimer > 90)
				unit->removeTimer = 90;

			unit->status |= UnitStatus::NoCollide;
			unit->mainOrderTimer = 7;

			if (unit->orderTarget.unit != NULL) {
				moveToTarget(unit, unit->orderTarget.unit);
				unit->mainOrderState = 2;
			}
			else
			if(function_004EB9C0(unit,unit->orderTarget.pt.x,unit->orderTarget.pt.y))
				unit->mainOrderState = 2;

		}
		else
		if (unit->mainOrderState == 2) {

			if (unit->mainOrderTimer == 0) {
				unit->status &= ~UnitStatus::NoCollide; //unset flag if set
				unit->mainOrderState = 3;
			}

		}

		if (unit->mainOrderState == 2 || unit->mainOrderState == 3) {

			CUnit* target = unit->orderTarget.unit;
			bool bNotEndThere;

			if (target != NULL) {

				if (
					unit->isTargetWithinMinRange(target, 256) &&
					target->movementFlags & MovementFlags::Accelerating
				)
					function_00495400(unit, target);
				else
					function_004EB900(unit, target);

				bNotEndThere = unit->isTargetWithinMinRange(target, weapons_dat::InnerSplashRadius[units_dat::GroundWeapon[unit->id]]);

			}
			else
				bNotEndThere = (unit->getMovableState() != 0); //unit has reached destination

			if (bNotEndThere) {

				CImage* current_image;

				unit->orderTarget.pt.x = unit->sprite->position.x;
				unit->orderTarget.pt.y = unit->sprite->position.y;

				current_image = unit->sprite->images.head;

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
				unit->status |= UnitStatus::IsSelfDestructing;
				unit->remove();
			}

		}

	}

}

;

//Attack order of Carriers Interceptors
void orders_StrafeUnit(CUnit* unit) {

	if (
		unit->interceptor.parent != NULL &&
		unit->shields < (units_dat::MaxShieldPoints[UnitId::ProtossInterceptor] * 256) / 4
	)
		unit->orderComputerCL(OrderId::Return);
	else
	//function can cause interceptor to return instead for various reasons
	//and thus not execute following code if returning false
	if (function_00465810(unit) && unit->mainOrderState <= 5) {

		//Because of unit->remove() not taking the risk to access mainOrderState in some conditions
		bool bEndThere = false;

		if (unit->mainOrderState == 0) {

			if (unit->interceptor.parent == NULL)
				unit->remove();
			else
			{
				scbw::playSound(SoundId::Protoss_TEMPLAR_PTeSum00_WAV_1, unit); //sound name seems wrong, but underlying value is correct for interceptor launch sound
				function_0046D30(unit, unit->orderTarget.pt.x, unit->orderTarget.pt.y, 3);
				unit->mainOrderTimer = 15;
				unit->mainOrderState = 1;
			}

			//note: cannot continue into mainOrderState 1 because mainOrderTimer
			//would not be 0 and mainOrderState 1 would end instantly
			bEndThere = true;

		}
		else
		if (unit->mainOrderState == 1) {

			if (unit->mainOrderTimer == 0) {
				unit->sprite->elevationLevel += 2;
				unit->mainOrderState = 3;
			}
			else
				bEndThere = true;

		}

		if (!bEndThere) {
			if (unit->mainOrderState == 3) {

				CUnit* target = unit->orderTarget.unit;

				if (target != NULL) {

					if (isTargetVisible(unit, target)) {

						CUnit* current_unit;
						u8 weaponId;

						if (unit->subunit != NULL && units_dat::BaseProperty[unit->subunit->id] & UnitProperty::Subunit)
							current_unit = unit->subunit;
						else
							current_unit = unit;

						if (target->status & UnitStatus::InAir)
							weaponId = units_dat::AirWeapon[current_unit->id];
						else
						if (current_unit->id == UnitId::ZergLurker && !(current_unit->status & UnitStatus::Burrowed))
							weaponId = WeaponId::None;
						else
							weaponId = units_dat::GroundWeapon[current_unit->id];

						if (weaponId < WeaponId::None) {

							u32 minRange = weapons_dat::MinRange[weaponId];

							if (minRange == 0 || current_unit->isTargetWithinMinRange(target, minRange)) {

								if (unit->isTargetWithinMinRange(target, unit->getMaxWeaponRange(weaponId)))
									getWeaponBeginIscript(unit, IscriptAnimation::GndAttkRpt);

							}

						}

					}

				}
				else
					getWeaponBeginIscript(unit, IscriptAnimation::GndAttkRpt);

				if (isTargetWithinMinMovementRange(unit, NULL, 50)) {
					function_0046D30(unit, unit->orderTarget.pt.x, unit->orderTarget.pt.y, 1);
					unit->mainOrderState = 4;
				}
				else
					function_004EB9C0(unit, unit->orderTarget.pt.x, unit->orderTarget.pt.y);


			}
			else
			if (unit->mainOrderState == 4) {
				if (isDistanceGreaterThanHaltDistance(unit, unit->moveTarget.pt.x, unit->moveTarget.pt.y, 50)) {
					function_0046D30(unit, unit->orderTarget.pt.x, unit->orderTarget.pt.y, 2);
					unit->mainOrderState = 5;
				}
			}
			else
			if (unit->mainOrderState == 5) {
				if (isDistanceGreaterThanHaltDistance(unit, unit->moveTarget.pt.x, unit->moveTarget.pt.y, 50)) {
					if(function_004EB9C0(unit, unit->orderTarget.pt.x, unit->orderTarget.pt.y))
						unit->mainOrderState = 3;
				}
			}

		}

	}
	
}

;

//Used when unit->mainOrderId is 
//OrderId::Reaver, OrderId::ReaverFight or OrderId::ReaverHold
void orders_Reaver(CUnit* unit) {
	
	int seekRange = unit->getSeekRange() * 32;

	if (function_00465A60(unit, seekRange * 2, seekRange)) {
		
		if (
			unit->nextTargetWaypoint.x != unit->orderTarget.pt.x ||
			unit->nextTargetWaypoint.y != unit->orderTarget.pt.y
		)
		{
			unit->nextTargetWaypoint.x = unit->orderTarget.pt.x;
			unit->nextTargetWaypoint.y = unit->orderTarget.pt.y;
		}

		int angle = scbw::getAngle(
			unit->sprite->position.x,
			unit->sprite->position.y,
			unit->nextTargetWaypoint.x,
			unit->nextTargetWaypoint.y
		);

		if (unit->currentDirection1 - angle > 1)
			unit->orderQueueTimer = 0;
		else
		if(!(unit->status & UnitStatus::CanNotAttack) && unit->carrier.outHangarChild == NULL)
		{

			CUnit* sentUnit = launchInterceptorScarab(unit);

			if (sentUnit != NULL) {
				sentUnit->orderTo(units_dat::AttackUnitOrder[UnitId::ProtossScarab], unit->orderTarget.unit);
				sentUnit->interceptor.parent = unit;
				unit->mainOrderTimer = 60;
			}

		}

	}
	else
	if(unit->pAI != NULL && unit->mainOrderTimer == 0)
		unit->orderComputerCL(OrderId::ComputerAI);

}

;

//Used when unit->mainOrderId is 
//OrderId::Carrier, OrderId::CarrierFight or OrderId::HoldPosition1
void orders_Carrier(CUnit* unit) {

	if (function_00465A60(unit, 0, unit->getSeekRange() * 32)) {

		CUnit* sentUnit = launchInterceptorScarab(unit);

		if (sentUnit != NULL) {
			sentUnit->shields = units_dat::MaxShieldPoints[sentUnit->id] * 256;
			sentUnit->sprite->elevationLevel = unit->sprite->elevationLevel - 1;
			sentUnit->orderTo(units_dat::AttackUnitOrder[UnitId::ProtossInterceptor], unit->orderTarget.unit);
			unit->mainOrderTimer = 7;
		}

	}
	else
	if(unit->pAI != NULL && unit->mainOrderTimer == 0)
		unit->orderComputerCL(OrderId::ComputerAI);

}

;

void orders_TurretAttack(CUnit* unit) {

	CUnit* target;
	CUnit* current_unit;
	u8 current_unit_weaponId;
	
	bool jump_to_77A45 = false;
	bool jump_to_77A70 = false;
	bool jump_to_77A9D = false;
	bool jump_to_77AB7 = false;
	bool jump_to_77AFE = false;
	bool jump_to_77CDA = false;
	bool bJumpToEarlierCode;

	//perform lots of checks, possibly in an attempt to
	//auto-select a better target if the turret has choice
	function_00477510(unit);

	target = unit->orderTarget.unit;

	if(target != NULL) {
		unit->orderTarget.pt.x = target->sprite->position.x;
		unit->orderTarget.pt.y = target->sprite->position.y;
	}

	//779AE:
	if(
		unit->status & UnitStatus::DoodadStatesThing ||
		unit->lockdownTimer != 0 ||
		unit->stasisTimer != 0 ||
		unit->maelstromTimer != 0 ||
		target == NULL ||
		target->status & UnitStatus::Invincible ||
		target->sprite->flags & CSprite_Flags::Hidden ||
		unitCantSeeCloakedTarget(unit, target)
	)
		jump_to_77A45 = true;
	else {
	
		//77A04
		if((unit->subunit)->isSubunit()) 
			current_unit = unit->subunit;
		else
			current_unit = unit;

		//identical to check performed by unitIsCarrier @ 0x00401470
		if(unit->id == UnitId::ProtossCarrier || unit->id == UnitId::Hero_Gantrithor)
			jump_to_77AFE = true;
		else
		//identical to check performed by unitIsReaver @ 0x00401490, using if(!unitIsReaver())
		if(!(unit->id == UnitId::ProtossReaver || unit->id == UnitId::Hero_Warbringer))
			jump_to_77A9D = true;
		else
		if(target->status & UnitStatus::InAir)
			jump_to_77A45 = true;
		else
		if(unit->hasPathToUnit(target))
			jump_to_77AFE = true;
		else
			jump_to_77A45 = true;

	} //If Unit Able To Fight (various conditions)

	do {

		bJumpToEarlierCode = false;

		if(jump_to_77A45) {

			unit->userActionFlags |= 1;

			if(unit->mainOrderId == OrderId::Die)
				prepareForNextOrder(unit);	//77CED
			else
				jump_to_77A70 = true;

		} //if(jump_to_77A45)

		jump_to_77A45 = false;

		if(jump_to_77A70) { //77A70

			bool bStopLoop77A70 = false;

			while(!bStopLoop77A70 && unit->orderQueueTail != NULL) {

				if(
					!orders_dat::CanBeInterrupted[unit->orderQueueTail->orderId] && 
					unit->orderQueueTail->orderId != OrderId::TurretGuard
				)
					bStopLoop77A70 = true;
				else
					//77A94:
					removeOrderFromUnitQueue(unit,unit->orderQueueTail);
			}
			
			jump_to_77CDA = true;

		}

		jump_to_77A70 = false;

		if(jump_to_77A9D) { //77A9D

			if(unitCanInfest(unit)) {

				if(isInfestableUnit(target))
					jump_to_77AFE = true;
				else {
					jump_to_77A45 = true;
					bJumpToEarlierCode = true;
				}

			}
			else
				jump_to_77AB7 = true;

		}

		jump_to_77A9D = false;

		if(jump_to_77AB7) {

			if(unit->id == UnitId::ProtossArbiter && unit->pAI != NULL) {
				jump_to_77A45 = true;
				bJumpToEarlierCode = true;
			}
			else {

				//77ACC
				if(target->status & UnitStatus::InAir)
					current_unit_weaponId = units_dat::AirWeapon[current_unit->id]; //77AD2
				else
					current_unit_weaponId = current_unit->getGroundWeapon();		//77AE6

				//77AF6
				if(current_unit_weaponId == WeaponId::None) {
					jump_to_77A45 = true;
					bJumpToEarlierCode = true;
				}
				else
					jump_to_77AFE = true;

			} //if(unit->id != UnitId::ProtossArbiter || unit->pAI == NULL)

		}

		jump_to_77AB7 = false;

		if(jump_to_77AFE) {

			if(!isTargetVisible(unit,target)) {
				jump_to_77A45 = true;
				bJumpToEarlierCode = true;
			}
			else {

				//77B0E
				CUnit* current_subunit = unit->subunit;
				bool jump_to_77BD7 = false;

				if(
					unit->id != UnitId::TerranSiegeTankTankModeTurret ||
					current_subunit->pAI == NULL ||
					(current_subunit->canUseTech(TechId::TankSiegeMode,unit->playerId) != 1) ||
					!isUnitVisible(unit)
				)
					jump_to_77BD7 = true;
				else {

					//77B5F:
					u32 result_getMaxWpnRange = unit->getMaxWeaponRange(units_dat::GroundWeapon[UnitId::TerranSiegeTankSiegeModeTurret]);

					if(!isTargetWithinMinMovementRange(current_subunit,target,result_getMaxWpnRange))
						jump_to_77BD7 = true;
					else {

						//77B77:
						result_getMaxWpnRange = unit->getMaxWeaponRange(units_dat::GroundWeapon[UnitId::TerranSiegeTankTankModeTurret]);

						if(
							isTargetWithinMinMovementRange(current_subunit,target,result_getMaxWpnRange) ||
							isUnitCritter(target)
						)
							jump_to_77BD7 = true;
						else { //77B9D

							unit->orderComputerCL(OrderId::Nothing2);

							//this orderId is hardcoded, not calculated 
							unit->orderSimple(units_dat::AttackUnitOrder[UnitId::TerranSiegeTankSiegeModeTurret],false);

							current_subunit->orderComputerCL(OrderId::SiegeMode);

							//this orderId is hardcoded, not calculated 
							current_subunit->issueQueuedOrderTarget(units_dat::AttackUnitOrder[UnitId::TerranSiegeTankSiegeMode],target,false);

						}

					} //if(isTargetWithinMinMovementRange(current_subunit,target,result_getMaxWpnRange))

				}

				if(jump_to_77BD7) {

					bool jump_to_77C78 = false;
					bool jump_to_77CA1 = false;

					if(target != NULL) {

						if(isTargetVisible(unit,target)) {

							if(	(unit->subunit)->isSubunit() )
								current_unit = unit->subunit;
							else
								current_unit = unit;

							//77C0C:
							if(target->status & UnitStatus::InAir)
								current_unit_weaponId = current_unit->getAirWeapon();
							else
								current_unit_weaponId = current_unit->getGroundWeapon(); //include the lurker special case so it's ok

							//77C40
							if(current_unit_weaponId >= WeaponId::None)
								jump_to_77C78 = true;
							else
							if(	weapons_dat::MinRange[current_unit_weaponId] != 0 &&
								current_unit->isTargetWithinMinRange(target, weapons_dat::MinRange[current_unit_weaponId])
							)
								jump_to_77C78 = true;
							else {

								//77C63:

								if(	isTargetWithinMinMovementRange
									(
										current_unit,
										target,
										unit->getMaxWeaponRange(current_unit_weaponId)
									)
								)
									jump_to_77CA1 = true;
								else
									jump_to_77C78 = true;

							}

						} //if(isTargetVisible(unit,target))
						else
							jump_to_77C78 = true;

					} //if(target != NULL)
					else
						jump_to_77CA1 = true;

					if(jump_to_77C78) {

						if(	(unit->subunit)->mainOrderId == units_dat::AttackUnitOrder[(unit->subunit)->id] &&
							getRightClickActionOrder(unit->subunit) != RightClickActions::NoMove_NormalAttack
						)
							jump_to_77CA1 = true;
						else
							unit->orderComputerCL(OrderId::TurretGuard);	//77C93

					}

					jump_to_77C78 = false;

					if(jump_to_77CA1) {

						if(	unit->orderTarget.pt.x != unit->nextTargetWaypoint.x ||
							unit->orderTarget.pt.y != unit->nextTargetWaypoint.y
						)
						{
							unit->nextTargetWaypoint.x = unit->orderTarget.pt.x;
							unit->nextTargetWaypoint.y = unit->orderTarget.pt.y;
						}

						//77CC1
						if(!((unit->subunit)->movementFlags & MovementFlags::Accelerating))
							getWeaponBeginIscript(unit,IscriptAnimation::GndAttkRpt);

					}

					jump_to_77CA1 = false;

				} //if(jump_to_77BD7)

				jump_to_77BD7 = false;

			}
		} //if(jump_to_77AFE)

		jump_to_77AFE = false;
		
		if(jump_to_77CDA) {

			unit->performAnotherOrder(
				OrderId::TurretGuard,
				0,
				0,
				NULL,
				UnitId::None
			);

			prepareForNextOrder(unit);

		}

		jump_to_77CDA = false;

	} while(bJumpToEarlierCode);

} //void orders_TurretAttack(CUnit* unit)

;

void orders_AttackFixedRange(CUnit* unit) {

	CUnit* target;

	bool jump_to_77E48 = false;
	bool jump_to_77E99 = false;

	if(unit->pAI != NULL)
		unit->orderTarget.unit = findBestAttackTarget(unit); //77D13
	else
		//perform lots of checks, possibly in an attempt to
		//auto-select a better target if the unit has choice
		function_00477510(unit); //77D1F

	//77D25:
	target = unit->orderTarget.unit;

	if(target != NULL) {
		unit->orderTarget.pt.x = target->sprite->position.x;
		unit->orderTarget.pt.y = target->sprite->position.y;
	}

	//77D42:
	if(
		unit->status & UnitStatus::DoodadStatesThing ||
		unit->lockdownTimer != 0 ||
		unit->stasisTimer != 0 ||
		unit->maelstromTimer != 0 ||
		target == NULL ||
		target->status & UnitStatus::Invincible ||
		target->sprite->flags & CSprite_Flags::Hidden ||
		unitCantSeeCloakedTarget(unit,target)
	)
		jump_to_77E48 = true;
	else {

		CUnit* current_unit;
		bool jump_to_77DEF = false;

		//77DB4:
		if(unit->subunit->isSubunit()) //check if is not null AND is a subunit
			current_unit = unit->subunit;
		else
			current_unit = unit;

		//77DC4:
		//identical to check performed by unitIsCarrier @ 0x00401470
		if(unit->id == UnitId::ProtossCarrier || unit->id == UnitId::Hero_Gantrithor)
			jump_to_77E99 = true;
		else
		//identical to check performed by unitIsReaver @ 0x00401490, using if(!unitIsReaver())
		if(!(unit->id == UnitId::ProtossReaver || unit->id == UnitId::Hero_Warbringer))
			jump_to_77DEF = true;
		else
		if(target->status & UnitStatus::InAir)
			jump_to_77E48 = true;
		else
		if(unit->hasPathToUnit(target))
			jump_to_77E99 = true;
		else
			jump_to_77E48 = true;

		if(jump_to_77DEF) {

			if(unitCanInfest(unit)) {
				if(isInfestableUnit(target))
					jump_to_77E99 = true;
				else
					jump_to_77E48 = true;
			}
			else {

				//77E09:
				if(unit->id == UnitId::ProtossArbiter && unit->pAI != NULL)
					jump_to_77E48 = true;
				else {

					u8 currentWeaponId;

					//77E1A:
					if(target->status & UnitStatus::InAir)
						currentWeaponId = current_unit->getAirWeapon();
					else
						currentWeaponId = current_unit->getGroundWeapon();

					//77E44
					if(currentWeaponId != WeaponId::None)
						jump_to_77E99 = true;
					else
						jump_to_77E48 = true;

				}

			} //unit cannot infest

		} //jump_to_77DEF

	} //if unit able to fight (several conditions)

	if(jump_to_77E48) {

		if(unit->orderQueueHead != NULL) {
			unit->userActionFlags |= 1;
			prepareForNextOrder(unit);
		}
		else
		if(unit->pAI != NULL) //77E6B
			unit->orderComputerCL(OrderId::ComputerAI);
		else //77E83
			unit->orderComputerCL(units_dat::ReturnToIdleOrder[unit->id]);

	}

	if(jump_to_77E99) {

		bool jump_to_77EC8 = false;
		bool jump_to_77ED6 = false;

		if(isUnitInWeaponRange(unit,target)) {

			//77ED6
			if(	unit->orderTarget.pt.x != unit->nextTargetWaypoint.x ||
				unit->orderTarget.pt.y != unit->nextTargetWaypoint.y
			)
			{
				unit->nextTargetWaypoint.x = unit->orderTarget.pt.x;
				unit->nextTargetWaypoint.y = unit->orderTarget.pt.y;
			}

			getWeaponBeginIscript(unit,IscriptAnimation::GndAttkRpt);

		}
		else
		if(unit->pAI == NULL)
			unit->orderToIdle(); //77EC8
		else
			unit->orderTo(OrderId::AttackMove, unit->orderTarget.pt.x, unit->orderTarget.pt.y);

	}

} //void orders_AttackFixedRange(CUnit* unit)

;

void orders_SapUnit(CUnit* unit) {

	CUnit* target = unit->orderTarget.unit;

	if(target == NULL) { //788EB

		if(unit->orderQueueHead != NULL) {
			unit->userActionFlags |= 1;
			prepareForNextOrder(unit);
		}
		else {

			if(unit->pAI != NULL)
				unit->orderComputerCL(OrderId::ComputerAI);
			else
				unit->orderComputerCL(units_dat::ReturnToIdleOrder[unit->id]);
			
		} //if(unit->orderQueueHead == NULL)

	}
	else { //78928

		if(!unit->canAttackTarget(target,true))
			unit->orderTo(OrderId::Move,target->sprite->position.x,target->sprite->position.y);
		else {//7894D

			bool bStopThere = false;

			if(unit->mainOrderState == 0) { //78965
				if(moveToTarget(unit,target))
					unit->mainOrderState = 1;
				else
					bStopThere = true;
			} //if(unit->mainOrderState == 0)
			
			if(!bStopThere && unit->mainOrderState == 1) { //78975
				
				if(
					!unit->isTargetWithinMinRange(target,4) &&
					unit->getMovableState() != 2
				)
				{ //78998

					if(
						unit->isTargetWithinMinRange(target,256) &&
						target->movementFlags & MovementFlags::Accelerating
					)
						//maybe trigger the acceleration of the infested
						//terran, or something else related to distances?
						function_00495400(unit,target);

					else //789BF
						//may cause the unit to go to idle, or
						//execute and return the (unused here) 
						//result of moveToTarget @ 0x004EB720,
						//or something else
						function_004EB900(unit,target);

					bStopThere = true;

				}
				else { //789CC

					setNextWaypoint_Sub4EB290(unit);

					if(!unit->isTargetWithinMinRange
						(
							target,
							weapons_dat::OuterSplashRadius[WeaponId::Suicide_InfestedTerran]
						)
					)
					{
						unit->orderToIdle();
						bStopThere = true;
					}
					else { //789F4

						unit->orderTarget.unit = unit;
						CImage* current_image = unit->sprite->images.head;

						while(current_image != NULL) {
							current_image->playIscriptAnim(IscriptAnimation::SpecialState1);
							current_image = current_image->link.next;
						}

						//78A11:
						unit->status |= UnitStatus::CanNotReceiveOrders;
						unit->mainOrderState = 2;

					} //if(isTargetWithinMinRange)

				} //if target reachable

			} //if(unit->mainOrderState == 1)
			
			if(!bStopThere && unit->mainOrderState == 2) { //78A1E

				if(unit->orderSignal & 1) {
					unit->status |= UnitStatus::IsSelfDestructing;
					unit->remove(); //orders_SelfDestructing
				}

			}

		} //if(unit->canAttackTarget(target,true))

	} //if(target != NULL)

} //void orders_SapUnit(CUnit* unit)

;

void orders_SapLocation(CUnit* unit) {

	bool bStopThere = false;

	if(unit->mainOrderState == 0) { //78A5C

		bStopThere = !SetMoveTarget_xy(unit,unit->orderTarget.pt.x,unit->orderTarget.pt.y);

		if(	unit->orderTarget.pt.x != unit->nextTargetWaypoint.x ||
			unit->orderTarget.pt.y != unit->nextTargetWaypoint.y
		)
		{
			unit->nextTargetWaypoint.x = unit->orderTarget.pt.x;
			unit->nextTargetWaypoint.y = unit->orderTarget.pt.y;
		}

		if(!bStopThere)
			unit->mainOrderState = 1;

	} //if(unit->mainOrderState == 0)

	if(!bStopThere && unit->mainOrderState == 1) { //78A89

		if(	unit->sprite->position.x != unit->moveTarget.pt.x ||
			unit->sprite->position.y != unit->moveTarget.pt.y
		)
			bStopThere = true;
		else { //78AA0

			if(unit->status & UnitStatus::Unmovable) { //78AB8

				u32 distance = 
					scbw::getDistanceFast(
						unit->orderTarget.pt.x * 256,
						unit->orderTarget.pt.y * 256,
						unit->halt.x,
						unit->halt.y
					);

				if((distance / 256) > weapons_dat::OuterSplashRadius[WeaponId::Suicide_InfestedTerran]) {
					unit->orderToIdle();
					bStopThere = true;
				}

			}

			/*Ignored a bugged unit->status & UnitStatus::Unmovable check here*/

			if(!bStopThere) { //78AF3

				CImage* current_image = unit->sprite->images.head;

				unit->orderTarget.unit = unit;

				while(current_image != NULL) {
					current_image->playIscriptAnim(IscriptAnimation::SpecialState1);
					current_image  = current_image->link.next;
				}

				//78B10
				unit->status |= UnitStatus::CanNotReceiveOrders;
				unit->mainOrderState = 2;

			}

		}	//if unit reached target

	} //if(unit->mainOrderState == 1)

	if(!bStopThere && unit->mainOrderState == 2) { //78B1E

		if(unit->orderSignal & 1) {
			unit->status |= UnitStatus::IsSelfDestructing;
			unit->remove(); //orders_SelfDestructing
		}

	} //if(unit->mainOrderState == 2)

} //void orders_SapLocation(CUnit* unit)

;

void orders_AttackMoveEP(CUnit* unit) {

	u8 saved_mainOrderId = unit->mainOrderId; //[EBP-0x14]

	//function_00478370:
	//probably perform checks to see if the order
	//can be done, but which exactly I don't know

	if(	function_00478370(unit, unit->mainOrderId) &&
		unit->mainOrderTimer == 0
	)
	{ //78E0B

		unit->mainOrderTimer = 15;

		if(
			SAI_GetRegionIdFromPxEx(unit->sprite->position.x,unit->sprite->position.y)
			==
			SAI_GetRegionIdFromPxEx(unit->orderTarget.pt.x,unit->orderTarget.pt.y)
		)
		{
			makeToHoldPosition(unit);
			unit->orderToIdle();
		}
		else { //78E65

			bool jump_to_78EAE = false;

			if(
				unit->getMovableState() != 0 &&
				(unit->orderTarget.pt.x != unit->sprite->position.x ||
				 unit->orderTarget.pt.y != unit->sprite->position.y)
			)
			{ //78E80

				if(
					function_004EB9C0(				//return value is actually from 
						unit,						//SetMoveTarget_xy @ 0x004EB820
						unit->orderTarget.pt.x,
						unit->orderTarget.pt.y
					)
				)
					jump_to_78EAE = true;
				else //78E91
				if(function_00476610(unit,(s32)unit->orderTarget.pt.x,(s32)unit->orderTarget.pt.y))
					makeToHoldPosition(unit);

			}
			else
				jump_to_78EAE = true;

			if(jump_to_78EAE)
				function_00477820(unit, saved_mainOrderId);	//use attack range, may cause idle,
											//may look for target, and others...

		} //if different regions

	} //if order timer is at 0 and others order checks

} //void orders_AttackMoveEP(CUnit* unit)

;

void orders_AttackMove(CUnit* unit) {

	//function_00478370:
	//Logically, would check if a target is in range, and
	//if that's the case (not returning 0), stop the "move" 
	//part that follow to attack the unit met
	if(
		function_00478370(unit,unit->mainOrderId) != 0 &&
		unit->mainOrderTimer == 0
	)
	{ //79058

		unit->mainOrderTimer = 15;

		if(
			unit->moveTarget.pt.x != unit->sprite->position.x ||
			unit->moveTarget.pt.y != unit->sprite->position.y
			/*Ignored a bugged unit->status & UnitStatus::Unmovable check here*/
		)
		//Logically, this function would check one last time if a target is in range,
		//and attack it if that's the case, else AttackMove end here
			function_00477820(unit,unit->mainOrderId); //79092
		else
			unit->orderToIdle();

	}

} //void orders_AttackMove(CUnit* unit)

;

//Was unreferenced function @ 00479150
//Used by defense structures
void orders_TowerAttack(CUnit* unit) {

	if (
		unit->status & UnitStatus::DoodadStatesThing ||
		unit->lockdownTimer != 0 ||
		unit->stasisTimer != 0 ||
		unit->maelstromTimer != 0 ||
		attackApplyCooldown(unit) == 0 //try to perform attack here, return 0 if failed
	)
	{
		
		if (unit->orderQueueHead != NULL) {
			unit->userActionFlags |= 1;
			prepareForNextOrder(unit);
		}
		else
		if(unit->pAI != NULL)
			unit->orderComputerCL(OrderId::ComputerAI);
		else
			unit->orderComputerCL(units_dat::ReturnToIdleOrder[unit->id]);

	}
	else
	{

		SetMoveTarget_xy(unit, unit->sprite->position.x, unit->sprite->position.y);

		if ((unit->orderTarget.pt.x != unit->nextTargetWaypoint.x) || (unit->orderTarget.pt.y != unit->nextTargetWaypoint.y)) {
			unit->nextTargetWaypoint.x = unit->orderTarget.pt.x;
			unit->nextTargetWaypoint.y = unit->orderTarget.pt.y;
		}

	}

}

;

void orders_AttackUnit(CUnit* unit) {

	if(
		playerTable[unit->playerId].type != PlayerType::Computer 
		||
		(
			unit->orderTarget.unit != NULL && 
			units_dat::BaseProperty[(unit->orderTarget.unit)->id] & UnitProperty::Building
		) 
		||
		!function_00462EA0(unit,0)	//if unit controlled by AI, do lots of things and
									//return true after/if validating a final order
	)
	{ //79C05

		//perform lots of checks, possibly in an attempt to
		//auto-select a better target if the unit has choice
		function_00477510(unit);

		//function perform tests to see if attack is possible
		//and how it should be done and return true if attack
		//can be done
		if(function_00478540(unit)) { //79C16

			if(unit->pAI != NULL)
				AI_AttackUnit(unit);

			getWeaponBeginIscript(unit,IscriptAnimation::GndAttkRpt);

		}

	}

} //void orders_AttackUnit(CUnit* unit)

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

const u32 Func_unitCantSeeCloakedTarget = 0x00401D60;
bool unitCantSeeCloakedTarget(CUnit* unit, CUnit* target) {

	static Bool32 bPreResult;

	__asm {
		PUSHAD
		PUSH unit
		MOV EAX, target
		CALL Func_unitCantSeeCloakedTarget
		MOV bPreResult, EAX
		POPAD
	}

	return (bPreResult != 0);

}

;

const u32 Func_IsInfestable = 0x00402210;
bool isInfestableUnit(CUnit* unit) {

  static Bool32 bPreResult;
  
  __asm {
    PUSHAD
    MOV EDX, unit
    CALL Func_IsInfestable
    MOV bPreResult, EAX
    POPAD
  }

  return (bPreResult != 0);

}

;

const u32 Func_CanInfest = 0x00402750;
bool unitCanInfest(CUnit* unit) {

	static Bool32 bPreResult;

	__asm {
		PUSHAD
		MOV EAX, unit
		CALL Func_CanInfest
		MOV bPreResult, EAX
		POPAD
	}

	return (bPreResult != 0);

}

;

const u32 Func_AI_AttackUnit = 0x0043FFD0;
void AI_AttackUnit(CUnit* unit) {

	__asm {
		PUSHAD
		PUSH unit
		CALL Func_AI_AttackUnit
		POPAD
	}

}

;

const u32 Func_Sub443080 = 0x00443080;
//named according to attack_priority_inject.cpp
CUnit* findBestAttackTarget(CUnit* unit) {

	static CUnit* result;

	__asm {
		PUSHAD
		MOV EAX, unit
		CALL Func_Sub443080
		MOV result, EAX
		POPAD
	}

	return result;

}

;

const u32 Func_Sub462EA0 = 0x00462EA0;
bool function_00462EA0(CUnit* unit, u8 unknownByteValue) {

	static u32 _unknownByteValue;
	static Bool32 bPreResult;

	_unknownByteValue = unknownByteValue;

	__asm {
		PUSHAD
		MOV EAX, unit
		PUSH _unknownByteValue
		CALL Func_Sub462EA0
		MOV bPreResult, EAX
		POPAD
	}

	return (bPreResult != 0);

}

;

const u32 Func_Sub465780 = 0x00465780;
void function_00465780(CUnit* unit) {

	__asm {
		PUSHAD
		MOV EDI, unit
		CALL Func_Sub465780
		POPAD
	}

}

;

const u32 Func_Sub465810 = 0x00465810;
bool function_00465810(CUnit* unit) {

	static Bool32 bPreResult;

	__asm {
		PUSHAD
		MOV EAX, unit
		CALL Func_Sub465810
		MOV bPreResult, EAX
		POPAD
	}

	return (bPreResult != 0);

}

;

const u32 Func_Sub465A60 = 0x00465A60;
bool function_00465A60(CUnit* unit, int range1, int range2) {

	static Bool32 bPreResult;

	__asm {
		PUSHAD
		PUSH range1
		PUSH range2
		MOV EAX, unit
		CALL Func_Sub465A60
		MOV bPreResult, EAX
		POPAD
	}

	return (bPreResult != 0);

}

;

const u32 Func_Sub465D30 = 0x00465D30;
//"unk" seems to be used by random number generators
void function_0046D30(CUnit* unit, u16 x, u16 y, u8 unk) {

	static Point16 pos;
	pos.x = x; pos.y = y;

	__asm {
		PUSHAD
		PUSH pos
		MOV CL, unk
		MOV EAX, unit
		CALL Func_Sub465D30
		POPAD
	}

}

//Only used here, and making a modified copy could be useful,so hardcoded.
//Not using helpers, a copy should not reproduce that bad coding behavior.
//Function @ 00466440
CUnit* launchInterceptorScarab(CUnit* unit) {

	CUnit* launched_unit = unit->carrier.inHangarChild;

	while (launched_unit != NULL && !(launched_unit->status & UnitStatus::Completed))
		launched_unit = launched_unit->interceptor.hangar_link.prev;

	if (launched_unit != NULL) {

		CUnit* temp_unit = launched_unit->interceptor.hangar_link.prev;

		while (temp_unit != NULL) {

			if (
				(temp_unit->status & UnitStatus::Completed) &&
				(temp_unit->hitPoints > launched_unit->hitPoints)
			)
				launched_unit = temp_unit;

			temp_unit = temp_unit->interceptor.hangar_link.prev;

		}

		if (launched_unit->id == UnitId::ProtossInterceptor) {

			int maxHp = units_dat::MaxHitPoints[UnitId::ProtossInterceptor];

			if (maxHp < 0)
				maxHp += 1;

			if (launched_unit->hitPoints < maxHp / 2)
				launched_unit = NULL;

		}

		if (launched_unit != NULL) {

			const u32 Func_InitUnitTrapDoodad = 0x004E6490; //move to helpers if making a modified copy
			int x, y;

			if (launched_unit->id == UnitId::ProtossScarab) {

				const u32 Func_turnGraphic = 0x00495F20;	//move to helpers if making a modified copy
				u8 direction = unit->currentDirection1;

				x = unit->sprite->position.x + angleDistance[direction].x * 25 / 256;
				y = unit->sprite->position.y + angleDistance[direction].y * 25 / 256;

				//void turnGraphic(CUnit* unit, u8 direction) @ 00495F20
				__asm {
					PUSHAD
					MOV EAX, launched_unit
					MOV BL, direction
					CALL Func_turnGraphic
					POPAD
				}

			}
			else
			{
				x = unit->sprite->position.x;
				y = unit->sprite->position.y;
			}

			scbw::refreshConsole();

			if (unit->carrier.inHangarChild == launched_unit)
				unit->carrier.inHangarChild = launched_unit->interceptor.hangar_link.prev;

			if (launched_unit->interceptor.hangar_link.prev != NULL)
				(launched_unit->interceptor.hangar_link.prev)->interceptor.hangar_link.next = launched_unit->interceptor.hangar_link.next;

			if (launched_unit->interceptor.hangar_link.next != NULL)
				(launched_unit->interceptor.hangar_link.next)->interceptor.hangar_link.prev = launched_unit->interceptor.hangar_link.prev;

			unit->carrier.inHangarCount--;

			launched_unit->interceptor.hangar_link.prev = unit->carrier.outHangarChild;
			launched_unit->interceptor.hangar_link.next = NULL;

			if (unit->carrier.outHangarChild != NULL)
				(unit->carrier.outHangarChild)->interceptor.hangar_link.next = launched_unit;

			unit->carrier.outHangarChild = launched_unit;

			unit->carrier.outHangarCount++;

			scbw::setUnitPosition(launched_unit, x, y);

			//void showAndEnableUnit(CUnit* unit) @ 004E6490
			__asm {
				PUSHAD
				MOV EDI, launched_unit
				CALL Func_InitUnitTrapDoodad
				POPAD
			}

			launched_unit->interceptor.isOutsideHangar = true;

		}

	}

	return launched_unit;

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

const u32 Func_Sub474A70 = 0x00474A70;
void function_00474A70(CUnit* unit, CUnit* target, u8 orderId) {

	__asm {
		PUSHAD
		MOV CL, orderId
		MOV EDX, unit
		MOV EAX, target
		CALL Func_Sub474A70
		POPAD
	}

}

;

const u32 Func_isTargetWithinMinMovementRange = 0x004763D0;
bool isTargetWithinMinMovementRange(CUnit* unit, CUnit* target, u32 range) {

	static Bool32 bPreResult;

	__asm {
		PUSHAD
		MOV EDI, unit
		MOV EAX, target
		MOV ECX, range
		CALL Func_isTargetWithinMinMovementRange
		MOV bPreResult, EAX
		POPAD
	}

	return (bPreResult != 0);

}

;

const u32 Func_Sub476610 = 0x00476610;
bool function_00476610(CUnit* unit, int x, int y) {

	static Bool32 bPreResult;

	__asm {
		PUSHAD
		MOV ESI, unit
		MOV EBX, x
		MOV EDI, y
		CALL Func_Sub476610
		MOV bPreResult, EAX
		POPAD
	}

	return (bPreResult != 0);

}

;

const u32 Func_isUnitInWeaponRange = 0x00476870;
bool isUnitInWeaponRange(CUnit* unit, CUnit* target) {

	static Bool32 bPreResult;

	__asm {
		PUSHAD
		PUSH unit
		MOV EAX, target
		CALL Func_isUnitInWeaponRange
		MOV bPreResult, EAX
		POPAD
	}

	return (bPreResult != 0);

}

;

const u32 Func_getWeaponBeginIscript = 0x00476ED0;
void getWeaponBeginIscript(CUnit* unit, IscriptAnimation::Enum anim) {

	__asm {
		PUSHAD
		MOV EAX, unit
		MOV DL, anim
		CALL Func_getWeaponBeginIscript
		POPAD
	}

}

;

const u32 Func_Sub477510 = 0x00477510;
void function_00477510(CUnit* unit) {

	__asm {
		PUSHAD
		PUSH unit
		CALL Func_Sub477510
		POPAD
	}

}

;

const u32 Func_Sub477820 = 0x00477820;
void function_00477820(CUnit* unit, u8 orderId) {

	static u32 orderId_;

	orderId_ = orderId;

	__asm {
		PUSHAD
		PUSH orderId_
		MOV EAX, unit
		CALL Func_Sub477820
		POPAD
	}

}

;

const u32 Func_Sub478370 = 0x00478370;
bool function_00478370(CUnit* unit, u8 orderId) {

	static Bool32 bPreResult;
	static u32 orderId_;

	orderId_ = orderId;

	__asm {
		PUSHAD
		MOV EAX, unit
		PUSH orderId_
		CALL Func_Sub478370
		MOV bPreResult, EAX
		POPAD
	}

	return (bPreResult != 0);

}

;

const u32 Func_Sub478540 = 0x00478540;
bool function_00478540(CUnit* unit) {

	static Bool32 bPreResult;

	__asm {
		PUSHAD
		MOV EAX, unit
		CALL Func_Sub478540
		MOV bPreResult, EAX
		POPAD
	}

	return (bPreResult != 0);

}

;

//Identical to Unit::isCritter() @ 0x0047B450
bool isUnitCritter(CUnit* unit) {

	bool bReturnValue;

	if(
		unit->id == UnitId::Critter_Rhynadon ||
		unit->id == UnitId::Critter_Bengalaas ||
		unit->id == UnitId::Critter_Ragnasaur ||
		unit->id == UnitId::Critter_Kakaru ||
		unit->id == UnitId::Critter_Scantid ||
		unit->id == UnitId::Critter_Ursadon
	)
		bReturnValue = true;
	else
		bReturnValue = false;

	return bReturnValue;

}

;

const u32 Func_attackApplyCooldown = 0x00478B40;
Bool32 attackApplyCooldown(CUnit* unit) {

	static u32 result;

	__asm {
		PUSHAD
		MOV EAX, unit
		CALL Func_attackApplyCooldown
		MOV result, EAX
		POPAD
	}

	return result;

}

;

const u32 Func_Sub495400 = 0x00495400;
void function_00495400(CUnit* unit, CUnit* target) {

	__asm {
		PUSHAD
		MOV EDI, unit
		MOV EAX, target
		CALL Func_Sub495400
		POPAD
	}

}

;

const u32 Func_SAI_GetRegionIdFromPxEx = 0x0049C9F0;
u16 SAI_GetRegionIdFromPxEx(s32 x, s32 y) {

	static u16 result;

	__asm {
		PUSHAD
		MOV EDI, x
		MOV ECX, y
		CALL Func_SAI_GetRegionIdFromPxEx
		MOV result, AX
		POPAD
	}

	return result;

}

;

const u32 Func_isUnitVisible = 0x004E5DB0;
bool isUnitVisible(CUnit* unit) {

	static Bool32 bPreResult;

	__asm {
		PUSHAD
		MOV ESI, unit
		CALL Func_isUnitVisible
		MOV bPreResult ,EAX
		POPAD
	}

	return (bPreResult != 0);

}

;

const u32 Func_isTargetVisible = 0x004E5E30;
bool isTargetVisible(CUnit* unit, CUnit* target) {

	static Bool32 bPreResult;

	__asm {
		PUSHAD
		MOV ESI, unit
		MOV EAX, target
		CALL Func_isTargetVisible
		MOV bPreResult, EAX
		POPAD
	}

	return (bPreResult != 0);
	
}

;

//Identical to getRightClickActionOrder @ 0x004E5EA0;
u8 getRightClickActionOrder(CUnit* unit) {

	u8 result;

	if( unit->id == UnitId::ZergLurker && unit->status & UnitStatus::Burrowed )
		result = RightClickActions::NoMove_NormalAttack;
	else {

		u8 temp_result = units_dat::RightClickAction[unit->id];

		if(
			temp_result == RightClickActions::NoCommand_AutoAttack &&
			unit->status & UnitStatus::GroundedBuilding &&
			unit->isFactory()
		)
			result = RightClickActions::NormalMove_NoAttack;
		else
			result = temp_result;

	}

	return result;

}

;

//not related with assigning orders, but only with
//destinations of orders
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

const u32 Func_moveToTarget = 0x004EB720;
bool moveToTarget(CUnit* unit, CUnit* target) {

	static Bool32 bPreResult;

	__asm {
		PUSHAD
		MOV ESI, unit
		MOV EDI, target
		CALL Func_moveToTarget
		MOV bPreResult, EAX
		POPAD
	}

	return (bPreResult != 0);

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

const u32 Func_Sub4EB900 = 0x004EB900;
bool function_004EB900(CUnit* unit, CUnit* target) {

	static Bool32 bPreResult;

	__asm {
		PUSHAD
		MOV EAX, target
		MOV ECX, unit
		CALL Func_Sub4EB900
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

#include "siege_transform.h"
#include <SCBW/api.h>

//Helper functions definitions

namespace {

void unitSetRetreatPoint(CUnit* unit, u8 angleValue);				//011A0
bool getCloserToDestination_sub402BE0(CUnit* unit);					//02BE0
void orderReturnToIdle(CUnit* unit);								//63770
bool checkForCloseUnit_sub464290(CUnit* unit);						//64290
void playSubunitIscript(CUnit* unit, IscriptAnimation::Enum anim);	//74380
void actUnitReturnToIdle(CUnit* unit);								//75420
void replaceUnitWithType(CUnit* unit, u16 newUnitId);				//9FED0
void makeToHoldPosition(CUnit* unit);								//EB5B0

} //unnamed namespace

namespace hooks {

	//00464AE0
	void ordersTankMode(CUnit* unit) {

		bool bStopThere = false;

		if(unit->mainOrderState == 0) {

			if(unit->id == UnitId::TerranSiegeTankSiegeMode || unit->id == UnitId::Hero_EdmundDukeSiegeMode) {

				if(!(unit->subunit->status & UnitStatus::NoBrkCodeStart)) {

					unit->subunit->orderTo(OrderId::Nothing3,unit);
					unitSetRetreatPoint(unit->subunit, units_dat::StartDirection[unit->subunit->id]);
					playSubunitIscript(unit,IscriptAnimation::SpecialState1);
					unit->mainOrderState = 2;

				}

			}
			else {
				bStopThere = true;
				unit->orderToIdle();
			}

		} //unit->mainOrderState == 0

		if(!bStopThere && unit->mainOrderState == 2) {

			if(getCloserToDestination_sub402BE0(unit->subunit)) {

				CImage* current_image = unit->sprite->images.head;

				while(current_image != NULL) {
					current_image->playIscriptAnim(IscriptAnimation::SpecialState2);
					current_image = current_image->link.next;
				}

				playSubunitIscript(unit,IscriptAnimation::SpecialState2);
				unit->orderSignal &= ~0x01; //if bit 1 is set, unset it
				unit->mainOrderState = 3;

			}

		} //unit->mainOrderState == 2
		
		if(!bStopThere && unit->mainOrderState == 3) {

			if(unit->orderSignal & 1) {

				u16 unitNewId;

				unit->orderSignal -= 1;

				if(unit->id == UnitId::Hero_EdmundDukeSiegeMode)
					unitNewId = UnitId::Hero_EdmundDukeTankMode;else
				if(unit->id == UnitId::TerranSiegeTankSiegeMode)
					unitNewId = UnitId::TerranSiegeTankTankMode;
				else
					unitNewId = unit->id;

				replaceUnitWithType(unit,unitNewId);
				orderReturnToIdle(unit);
				orderReturnToIdle(unit->subunit);

			}

		} //unit->mainOrderState == 3


	} //ordersTankMode

	//00464BD0
	void ordersSiegeMode(CUnit* unit) {

		bool bStopThere = false;

		if(unit->mainOrderState == 0) {

			if(unit->id == UnitId::TerranSiegeTankTankMode || unit->id == UnitId::Hero_EdmundDukeTankMode) {

				if(unit->movementFlags & MovementFlags::Accelerating)
					makeToHoldPosition(unit);

				unit->mainOrderState = 1;

			}
			else {

				unit->orderToIdle();
				bStopThere = true;

			}

		} //unit->mainOrderState == 0

		if(!bStopThere && unit->mainOrderState == 1) {

			if( !(unit->movementFlags & MovementFlags::Accelerating) ) {

				if( !(unit->subunit->movementFlags & MovementFlags::Accelerating) ) {

					if( !(unit->subunit->status & UnitStatus::NoBrkCodeStart) ) {

						unitSetRetreatPoint(unit,units_dat::StartDirection[unit->id]);
						unit->subunit->orderTo(OrderId::Nothing3,unit);
						unitSetRetreatPoint(unit->subunit,units_dat::StartDirection[unit->subunit->id]);
						playSubunitIscript(unit,IscriptAnimation::SpecialState1);
						unit->mainOrderState = 2;

						if(checkForCloseUnit_sub464290(unit))
							bStopThere = true;

					}

				}
			}

		} //unit->mainOrderState == 1

		if(!bStopThere && unit->mainOrderState == 2) {

			if( getCloserToDestination_sub402BE0(unit) ) {

				if( getCloserToDestination_sub402BE0(unit->subunit) ) {

					u16 unitNewId = unit->id;

					//identical to @00463310
					if(unit->id == UnitId::TerranSiegeTankTankMode)
						unitNewId = UnitId::TerranSiegeTankSiegeMode;
					else
					if(unit->id == UnitId::Hero_EdmundDukeTankMode)
						unitNewId = UnitId::Hero_EdmundDukeSiegeMode;
					else
						unitNewId = unit->id;					

					replaceUnitWithType(unit,unitNewId);
					unit->mainOrderState = 3;

				}

			}

		} //unit->mainOrderState == 2

		if(!bStopThere && unit->mainOrderState == 3) {

			if(unit->orderSignal & 1) {

				unit->orderSignal -= 1;

				if(unit->orderQueueHead != NULL) {

					if( !(unit->orderQueueHead->orderId == OrderId::WatchTarget) ) {

						unit->orderComputerCL(units_dat::ReturnToIdleOrder[unit->id]);
						actUnitReturnToIdle(unit);
						orderReturnToIdle(unit->subunit);
						bStopThere = true;

					}

				}

				if(!bStopThere) {
					orderReturnToIdle(unit);
					orderReturnToIdle(unit->subunit);
				}

			}

		} //unit->mainOrderState == 3

	}

} //hooks

//-------- Helper function definitions. Do NOT modify! --------//

namespace {

const u32 Func_UnitSetRetreatPoint = 0x004011A0;
//angleValue is a guess
void unitSetRetreatPoint(CUnit* unit, u8 angleValue) {

	__asm {
		PUSHAD
		MOV AL, angleValue
		MOV EDX, unit
		CALL Func_UnitSetRetreatPoint
		POPAD
	}

}

;

const u32 Func_Sub402BE0_GetCloserToDestination = 0x00402BE0;
//return true when unit reached nextTargetWaypoint either at call
//or after some action inside the function, else return false
//At least that's what I think it does (not sure what I called
//"action inside the function" is, and how it's used to return
//true
bool getCloserToDestination_sub402BE0(CUnit* unit) {

	static Bool32 bResult;

	__asm {
		PUSHAD
		MOV ESI, unit
		CALL Func_Sub402BE0_GetCloserToDestination
		MOV bResult, EAX
		POPAD
	}

	return (bResult == 1);

}

;

const u32 Func_OrderReturnToIdle = 0x00463770;
void orderReturnToIdle(CUnit* unit) {

	__asm {
		PUSHAD
		MOV ESI, unit
		CALL Func_OrderReturnToIdle
		POPAD
	}

}

;

const u32 Func_Sub464290_CheckForCloseUnit = 0x00464290;
bool checkForCloseUnit_sub464290(CUnit* unit) {

	static Bool32 bResult;

	__asm {
		PUSHAD
		MOV EAX, unit
		CALL Func_Sub464290_CheckForCloseUnit
		MOV bResult, EAX
		POPAD
	}

	return (bResult == 1);

}

;

const u32 Func_PlaySubunitIscript = 0x00474380;
void playSubunitIscript(CUnit* unit, IscriptAnimation::Enum anim) {

	u32 anim_ = anim;

	__asm {
		PUSHAD
		MOV EBX, unit
		PUSH anim_
		CALL Func_PlaySubunitIscript
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

const u32 Func_OrdersHoldPositionSuicidal = 0x004EB5B0;
void makeToHoldPosition(CUnit* unit) {

	__asm {
		PUSHAD
		MOV ESI, unit
		CALL Func_OrdersHoldPositionSuicidal
		POPAD
	}

}

} //Unnamed namespace

//End of helper functions

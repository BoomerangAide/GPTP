#include "unit_train.h"
#include <SCBW/api.h>
#include <Events/Events.h>

#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

//helper functions def

namespace {

u32 getHPGainForRepair(CUnit* unit);										//02C40
void refundUnitTrainCost(u32 unitId, u8 playerId);							//2CEC0
void function_00432430(CUnit* unit, u32 buildQueueSlot);					//32430
void addHangarUnit(CUnit* main_unit, CUnit* added_unit);					//66300
void orderNewUnitToRally(CUnit* unit, CUnit* factory);						//66F50
Bool32 buildingAddon(CUnit* unit, u32 hpGain, Bool32 canBeAborted);			//679A0
CUnit* attemptTrainHatchUnit(CUnit* trainer, u32 builtUnitID, u32 flag);	//68200
void AI_CancelStructure(CUnit* unit);										//68280
void AI_TrainingUnit(CUnit* unit_creator, CUnit* created_unit);				//A2830
void hideAndDisableUnit(CUnit* unit);										//E6340

} //unnamed namespace

namespace hooks {

#ifdef EVENTS_SYSTEM
void secondaryOrd_TrainFighter(CUnit* unit) {

	if(
		(
			unit->id == UnitId::ProtossCarrier ||
			unit->id == UnitId::Hero_Gantrithor ||
			unit->id == UnitId::ProtossReaver ||
			unit->id == UnitId::Hero_Warbringer
		) &&
		unit->secondaryOrderState <= 3
	)
	{

		if(unit->secondaryOrderState == 0 || unit->secondaryOrderState == 1) {

			if(unit->buildQueue[unit->buildQueueSlot] == UnitId::None) {
				unit->secondaryOrderState = 3;
				unit->currentBuildUnit = NULL;
			}
			else {

				CUnit* builtUnit;

				if(unit->secondaryOrderState == 0)
					builtUnit = attemptTrainHatchUnit(unit, unit->buildQueue[unit->buildQueueSlot], 1);
				else
					builtUnit = attemptTrainHatchUnit(unit, unit->buildQueue[unit->buildQueueSlot], 0);

				unit->currentBuildUnit = builtUnit;

				if(builtUnit == NULL)
					unit->secondaryOrderState = 1;
				else {
					builtUnit->interceptor.parent = unit;
					unit->secondaryOrderState = 2;
				}

			}

		}
		else
		if(unit->secondaryOrderState == 2) {

			CUnit* builtUnit = unit->currentBuildUnit;
			u32 hpGain;

			if(builtUnit != NULL) {

				std::vector<int*> events_interceptor_construction_hp_gain_arg(3);
				hpGain = getHPGainForRepair(builtUnit);

				events_interceptor_construction_hp_gain_arg[0] = (int*)builtUnit;
				events_interceptor_construction_hp_gain_arg[1] = (int*)unit;
				events_interceptor_construction_hp_gain_arg[2] = (int*)&hpGain;

				EventManager::EventCalled(EventId::UNIT_INTERCEPTOR_CONSTRUCTION_HPGAIN, events_interceptor_construction_hp_gain_arg);

				buildingAddon(builtUnit,hpGain,FALSE);

				if(builtUnit->status & UnitStatus::Completed) {

					addHangarUnit(unit,builtUnit);

					unit->buildQueue[unit->buildQueueSlot] = UnitId::None;
					unit->buildQueueSlot++;

					if(unit->buildQueueSlot >= 5)
						unit->buildQueueSlot = 0;

					scbw::refreshConsole();

					unit->currentBuildUnit = NULL;
					unit->secondaryOrderState = 0;

				}

			}
			else {

				scbw::refreshConsole();

				unit->currentBuildUnit = NULL;
				unit->secondaryOrderState = 0;

			}

		}
		else
		if(unit->secondaryOrderState == 3) {

			if(	unit->id == UnitId::ProtossReaver ||
				unit->id == UnitId::Hero_Warbringer
			)
				unit->secondaryOrderState = 4;
			else {

				CUnit* inHangarChild = unit->carrier.inHangarChild;

				if(inHangarChild != NULL) {

					s32 hpInterceptor = units_dat::MaxHitPoints[UnitId::ProtossInterceptor];

					while(inHangarChild != NULL && inHangarChild->hitPoints >= hpInterceptor)
						inHangarChild = inHangarChild->interceptor.hangar_link.next;

					if (inHangarChild != NULL) {

						std::vector<int*> events_interceptor_repair_hp_set_arg(4);
						s32 newHP = inHangarChild->hitPoints + 128;

						events_interceptor_repair_hp_set_arg[0] = (int*)inHangarChild;
						events_interceptor_repair_hp_set_arg[1] = (int*)unit;
						events_interceptor_repair_hp_set_arg[2] = (int*)inHangarChild->hitPoints;
						events_interceptor_repair_hp_set_arg[3] = (int*)&newHP;

						EventManager::EventCalled(EventId::UNIT_INTERCEPTOR_REPAIR_HP_SET, events_interceptor_repair_hp_set_arg);

						inHangarChild->setHp(newHP);

					}

				}

			}

		}

	}

}
#else
void secondaryOrd_TrainFighter(CUnit* unit) {

	if(
		unit->id == UnitId::ProtossCarrier ||
		unit->id == UnitId::Hero_Gantrithor ||
		unit->id == UnitId::ProtossReaver ||
		unit->id == UnitId::Hero_Warbringer
	)
	{

		if(unit->secondaryOrderState == 0 || unit->secondaryOrderState == 1) {

			if(unit->buildQueue[unit->buildQueueSlot] == UnitId::None) {
				unit->secondaryOrderState = 3;
				unit->currentBuildUnit = NULL;
			}
			else {

				CUnit* builtUnit;

				if(unit->secondaryOrderState == 0)
					builtUnit = attemptTrainHatchUnit(unit, unit->buildQueue[unit->buildQueueSlot], 1);
				else
					builtUnit = attemptTrainHatchUnit(unit, unit->buildQueue[unit->buildQueueSlot], 0);

				unit->currentBuildUnit = builtUnit;

				if(builtUnit == NULL)
					unit->secondaryOrderState = 1;
				else {
					builtUnit->interceptor.parent = unit;
					unit->secondaryOrderState = 2;
				}

			}

		}
		else
		if(unit->secondaryOrderState == 2) {

			CUnit* builtUnit = unit->currentBuildUnit;
			u32 hpGain;

			if(builtUnit != NULL) {

				hpGain = getHPGainForRepair(builtUnit);

				buildingAddon(builtUnit,hpGain,FALSE);

				if(builtUnit->status & UnitStatus::Completed) {

					addHangarUnit(unit,builtUnit);

					unit->buildQueue[unit->buildQueueSlot] = UnitId::None;
					unit->buildQueueSlot++;

					if(unit->buildQueueSlot >= 5)
						unit->buildQueueSlot = 0;

					scbw::refreshConsole();

					unit->currentBuildUnit = NULL;
					unit->secondaryOrderState = 0;

				}

			}
			else {

				scbw::refreshConsole();

				unit->currentBuildUnit = NULL;
				unit->secondaryOrderState = 0;

			}

		}
		else
		if(unit->secondaryOrderState == 3) {

			if(	unit->id == UnitId::ProtossReaver ||
				unit->id == UnitId::Hero_Warbringer
			)
				unit->secondaryOrderState = 4;
			else {

				CUnit* inHangarChild = unit->carrier.inHangarChild;

				if(inHangarChild != NULL) {

					s32 hpInterceptor = units_dat::MaxHitPoints[UnitId::ProtossInterceptor];

					while(inHangarChild != NULL && inHangarChild->hitPoints >= hpInterceptor)
						inHangarChild = inHangarChild->interceptor.hangar_link.next;

					if(inHangarChild != NULL)
						inHangarChild->setHp(inHangarChild->hitPoints + 128);

				}

			}

		}

	}

}
#endif

;

//generic unit training function
#ifdef EVENTS_SYSTEM
void function_00468420(CUnit* unit) {

	if(
		!(unit->status & UnitStatus::DoodadStatesThing) &&
		unit->lockdownTimer == 0 &&
		unit->stasisTimer == 0 &&
		unit->maelstromTimer == 0 &&
		(
			!units_dat::GroupFlags[unit->id].isZerg ||
			unit->id == UnitId::ZergInfestedCommandCenter
		)
	)
	{

		bool jump_to_685D8 = false;

		if(unit->secondaryOrderState <= 1) {

			if(unit->buildQueue[unit->buildQueueSlot] == UnitId::None) {
				unit->setSecondaryOrder(OrderId::Nothing2);
				unit->sprite->playIscriptAnim(IscriptAnimation::WorkingToIdle,true);
			}
			else {

				CUnit* builtUnit;

				if(unit->secondaryOrderState == 0)
					builtUnit = attemptTrainHatchUnit(unit,unit->buildQueue[unit->buildQueueSlot],1);
				else
					builtUnit = attemptTrainHatchUnit(unit,unit->buildQueue[unit->buildQueueSlot],0);

				unit->currentBuildUnit = builtUnit;

				if(builtUnit == NULL)
					unit->secondaryOrderState = 1;
				else {

					CImage* current_image = unit->sprite->images.head;

					unit->secondaryOrderState = 2;

					while(current_image != NULL) {
						current_image->playIscriptAnim(IscriptAnimation::IsWorking);
						current_image = current_image->link.next;
					}

				}

			}

		}
		else
		if(unit->secondaryOrderState == 2) {

			CUnit* builtUnit = unit->currentBuildUnit;

			if(builtUnit != NULL) {

				std::vector<int*> events_unit_trained_hp_gain_arg(3);
				u32 hpGain = getHPGainForRepair(builtUnit);

				events_unit_trained_hp_gain_arg[0] = (int*)builtUnit;
				events_unit_trained_hp_gain_arg[1] = (int*)unit;
				events_unit_trained_hp_gain_arg[2] = (int*)&hpGain;

				EventManager::EventCalled(EventId::UNIT_TRAINED_HPGAIN, events_unit_trained_hp_gain_arg);

				if(buildingAddon(builtUnit,hpGain,TRUE)) {

					if(builtUnit->status & UnitStatus::Completed) {

						AI_TrainingUnit(unit,builtUnit);

						if(builtUnit->id == UnitId::TerranNuclearMissile)
							hideAndDisableUnit(builtUnit);
						else
							orderNewUnitToRally(builtUnit,unit);

						function_00432430(unit,unit->buildQueueSlot);

						unit->buildQueue[unit->buildQueueSlot] = UnitId::None;
						unit->buildQueueSlot = (unit->buildQueueSlot + 1) % 5;

						jump_to_685D8 = true;	

					}

				}
				else { //unit was canceled due to lack of space around the building

					u32 queueSlot = unit->buildQueueSlot % 5;

					if(unit->buildQueue[queueSlot] == UnitId::None)
						jump_to_685D8 = true;
					else {

						u8 counter = 4;

						if(unit->currentBuildUnit != NULL)
							AI_CancelStructure(unit->currentBuildUnit);
						else {

							if(!(units_dat::BaseProperty[unit->buildQueue[unit->buildQueueSlot % 5]] & UnitProperty::Building))
								refundUnitTrainCost(unit->buildQueue[unit->buildQueueSlot % 5],unit->playerId);

						}

						do {

							counter--;
							unit->buildQueue[unit->buildQueueSlot] = unit->buildQueue[(unit->buildQueueSlot + 1) % 5];

							if(unit->pAI != NULL && *(u8*)((u32)unit->pAI + 8) == 3) {
								*(u8*)((u32)unit->pAI + unit->buildQueueSlot + 9) = *(u8*)((u32)unit->pAI + (unit->buildQueueSlot + 1) % 5 + 9);
								*(u32*)((u32)unit->pAI + unit->buildQueueSlot * 4 + 0x18) = *(u32*)((u32)unit->pAI + ((unit->buildQueueSlot + 1) % 5) * 4 + 0x18);
							}


						}while(counter != 0);

						unit->buildQueue[(unit->buildQueueSlot + 1) % 5] = UnitId::None;

						jump_to_685D8 = true;

					}

				}

			}
			else
				jump_to_685D8 = true;

			if(jump_to_685D8) {
				scbw::refreshConsole();
				unit->currentBuildUnit = NULL;
				unit->secondaryOrderState = 0;
			}

		}


	}

}
#else
void function_00468420(CUnit* unit) {

	if(
		!(unit->status & UnitStatus::DoodadStatesThing) &&
		unit->lockdownTimer == 0 &&
		unit->stasisTimer == 0 &&
		unit->maelstromTimer == 0 &&
		(
			!units_dat::GroupFlags[unit->id].isZerg ||
			unit->id == UnitId::ZergInfestedCommandCenter
		)
	)
	{

		bool jump_to_685D8 = false;

		if(unit->secondaryOrderState <= 1) {

			if(unit->buildQueue[unit->buildQueueSlot] == UnitId::None) {
				unit->setSecondaryOrder(OrderId::Nothing2);
				unit->sprite->playIscriptAnim(IscriptAnimation::WorkingToIdle,true);
			}
			else {

				CUnit* builtUnit;

				if(unit->secondaryOrderState == 0)
					builtUnit = attemptTrainHatchUnit(unit,unit->buildQueue[unit->buildQueueSlot],1);
				else
					builtUnit = attemptTrainHatchUnit(unit,unit->buildQueue[unit->buildQueueSlot],0);

				unit->currentBuildUnit = builtUnit;

				if(builtUnit == NULL)
					unit->secondaryOrderState = 1;
				else {

					CImage* current_image = unit->sprite->images.head;

					unit->secondaryOrderState = 2;

					while(current_image != NULL) {
						current_image->playIscriptAnim(IscriptAnimation::IsWorking);
						current_image = current_image->link.next;
					}

				}

			}

		}
		else
		if(unit->secondaryOrderState == 2) {

			CUnit* builtUnit = unit->currentBuildUnit;

			if(builtUnit != NULL) {

				u32 hpGain = getHPGainForRepair(builtUnit);

				if(buildingAddon(builtUnit,hpGain,TRUE)) {

					if(builtUnit->status & UnitStatus::Completed) {

						AI_TrainingUnit(unit,builtUnit);

						if(builtUnit->id == UnitId::TerranNuclearMissile)
							hideAndDisableUnit(builtUnit);
						else
							orderNewUnitToRally(builtUnit,unit);

						function_00432430(unit,unit->buildQueueSlot);

						unit->buildQueue[unit->buildQueueSlot] = UnitId::None;
						unit->buildQueueSlot = (unit->buildQueueSlot + 1) % 5;

						jump_to_685D8 = true;	

					}

				}
				else { //unit was canceled due to lack of space around the building

					u32 queueSlot = unit->buildQueueSlot % 5;

					if(unit->buildQueue[queueSlot] == UnitId::None)
						jump_to_685D8 = true;
					else {

						u8 counter = 4;

						if(unit->currentBuildUnit != NULL)
							AI_CancelStructure(unit->currentBuildUnit);
						else {

							if(!(units_dat::BaseProperty[unit->buildQueue[unit->buildQueueSlot % 5]] & UnitProperty::Building))
								refundUnitTrainCost(unit->buildQueue[unit->buildQueueSlot % 5],unit->playerId);

						}

						do {

							counter--;
							unit->buildQueue[unit->buildQueueSlot] = unit->buildQueue[(unit->buildQueueSlot + 1) % 5];

							if(unit->pAI != NULL && *(u8*)((u32)unit->pAI + 8) == 3) {
								*(u8*)((u32)unit->pAI + unit->buildQueueSlot + 9) = *(u8*)((u32)unit->pAI + (unit->buildQueueSlot + 1) % 5 + 9);
								*(u32*)((u32)unit->pAI + unit->buildQueueSlot * 4 + 0x18) = *(u32*)((u32)unit->pAI + ((unit->buildQueueSlot + 1) % 5) * 4 + 0x18);
							}


						}while(counter != 0);

						unit->buildQueue[(unit->buildQueueSlot + 1) % 5] = UnitId::None;

						jump_to_685D8 = true;

					}

				}

			}
			else
				jump_to_685D8 = true;

			if(jump_to_685D8) {
				scbw::refreshConsole();
				unit->currentBuildUnit = NULL;
				unit->secondaryOrderState = 0;
			}

		}


	}

}
#endif

;

} //namespace hooks

;

//-------- Helper function definitions. Do NOT modify! --------//

namespace {

//Identical to getHPGainForRepair @ 0x00402C40
u32 getHPGainForRepair(CUnit* unit) {

	if(*CHEAT_STATE & CheatFlags::OperationCwal)
		return unit->buildRepairHpGain * 16;
	else
		return unit->buildRepairHpGain;

}

;

const u32 Func_refundUnitTrainCost = 0x0042CEC0;
void refundUnitTrainCost(u32 unitId, u8 playerId) {

	__asm {
		PUSHAD
		MOV ECX, unitId
		MOV AL, playerId
		CALL Func_refundUnitTrainCost
		POPAD
	}

}

;

const u32 Func_Sub432430 = 0x00432430;
void function_00432430(CUnit* unit, u32 buildQueueSlot) {

	__asm {
		PUSHAD
		MOV EAX, unit
		MOV ECX, buildQueueSlot
		CALL Func_Sub432430
		POPAD
	}

}

;

const u32 Func_addHangerUnit = 0x00466300;
void addHangarUnit(CUnit* main_unit, CUnit* added_unit) {

	__asm {
		PUSHAD
		MOV EDI, main_unit
		MOV ESI, added_unit
		CALL Func_addHangerUnit
		POPAD
	}

}

;

const u32 Func_Sub466F50 = 0x00466F50;
void orderNewUnitToRally(CUnit* unit, CUnit* factory) {

	__asm {
		PUSHAD
		MOV EAX, unit
		MOV ECX, factory
		CALL Func_Sub466F50
		POPAD
	}

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

const u32 Func_attemptTrainHatchUnit = 0x00468200;
CUnit* attemptTrainHatchUnit(CUnit* trainer, u32 builtUnitID, u32 flag) {

	static CUnit* builtUnit;

	__asm {
		PUSHAD
		MOV ESI, trainer
		MOV EDI, builtUnitID
		PUSH flag
		CALL Func_attemptTrainHatchUnit
		MOV builtUnit, EAX
		POPAD
	}

	return builtUnit;

}

;

const u32 Func_AI_CancelStructure = 0x00468280;
void AI_CancelStructure(CUnit* unit) {

	__asm {
		PUSHAD
		MOV ECX, unit
		CALL Func_AI_CancelStructure
		POPAD
	}

}

;

const u32 Func_AI_TrainingUnit = 0x004A2830;
void AI_TrainingUnit(CUnit* unit_creator, CUnit* created_unit) {
	__asm {
		PUSHAD
		MOV EAX, created_unit
		MOV ECX, unit_creator
		CALL Func_AI_TrainingUnit
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

} //Unnamed namespace

//End of helper functions

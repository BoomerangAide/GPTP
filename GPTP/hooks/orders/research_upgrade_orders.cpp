#include "research_upgrade_orders.h"
#include <SCBW/api.h>

//helper functions def

namespace {

	bool isUnitUpgradeAvailable(CUnit* unit);															//033D0
	void ApplySpeedUpgradeFromUpgradeType(CUnit* unit, u8 upgradeType);									//54540
	void removeOrderFromUnitQueue(CUnit* unit, COrder* order);											//742D0
	void playUpgradeCompleteSound(CUnit* unit);															//48F070
	void playResearchCompleteSound(CUnit* unit);														//48F150
	void setUpgradeLevel(u32 playerId, u32 upgradeType, u8 upgradeLevel);								//CE770
	
} //unnamed namespace

namespace hooks {

	void orders_Upgrade(CUnit* unit) {

		bool bStopThere = false;
		CImage* current_image;

		if(	unit->status & UnitStatus::GroundedBuilding &&
			unit->building.upgradeType != UpgradeId::None
		) 
		{

			unit->building.upgradeResearchTime--;

			if(unit->building.upgradeResearchTime != 0xFFFF) {

				u8 upgradeLevel;

				if(unit->building.upgradeType >= UpgradeId::UnusedUpgrade46)
					upgradeLevel = UpgradesBw->currentLevel[unit->playerId][unit->building.upgradeType - UpgradeId::UnusedUpgrade46];
				else
					upgradeLevel = UpgradesSc->currentLevel[unit->playerId][unit->building.upgradeType];
	
				if(	upgradeLevel < unit->building.upgradeLevel &&
					!(*CHEAT_STATE & CheatFlags::OperationCwal))
					bStopThere = true;

			}

			//5471D
			if(!bStopThere) {

				u8 upgradeLevel;
				
				playUpgradeCompleteSound(unit);

				if(unit->building.upgradeType >= UpgradeId::UnusedUpgrade46)
					upgradeLevel = UpgradesBw->currentLevel[unit->playerId][unit->building.upgradeType - UpgradeId::UnusedUpgrade46];
				else
					upgradeLevel = UpgradesSc->currentLevel[unit->playerId][unit->building.upgradeType];

				if(upgradeLevel < unit->building.upgradeLevel) {

					if(isUnitUpgradeAvailable(unit)) {
						setUpgradeLevel(unit->playerId,unit->building.upgradeType,unit->building.upgradeLevel);
						ApplySpeedUpgradeFromUpgradeType(unit,unit->building.upgradeType);
					}

				}

				//5477F
				scbw::refreshConsole();

			}

		}

		//547A7
		if(!bStopThere) {

			current_image = unit->sprite->images.head;

			while(current_image != NULL) {
				current_image->playIscriptAnim(IscriptAnimation::WorkingToIdle);
				current_image = current_image->link.next;
			}

			//code I could not understand enough to convert
			__asm {

				PUSHAD

				MOV ESI, unit
				MOVZX ECX, byte ptr [ESI+0xC9]	//unit->building.upgradeType
				MOVZX EAX, byte ptr [ESI+0x4C]	//unit->playerId
				MOV EDX, ECX
				SHR EDX, 0x3
				AND ECX, 0x80000007
				LEA EAX, [EDX+EAX*0x8+0x0058F3E0] //EAX = &(UpgradesBw->currentLevel[unit->playerId][unit->building.upgradeType CONVERTED?])
				JNS lbl547E7

				DEC ECX
				OR ECX, -0x8
				INC ECX

				lbl547E7:

				MOV DL, 0x1
				SHL DL, CL
				MOV CL, [EAX]
				NOT DL
				AND CL, DL
				MOV [EAX], CL

				POPAD

			}

			unit->building.upgradeType = UpgradeId::None;
			unit->building.upgradeLevel = 0;

			if(unit->pAI != NULL) {

				unit->userActionFlags |= 1;

				if(unit->mainOrderId != OrderId::Die) {

					bool bStopLoop = (unit->orderQueueTail == NULL);

					while(!bStopLoop) {

						if(	!orders_dat::CanBeInterrupted[unit->orderQueueTail->orderId] &&
							unit->orderQueueTail->orderId != OrderId::ComputerAI
						)
							bStopLoop = true;
						else
							removeOrderFromUnitQueue(unit,unit->orderQueueTail);

					}

					unit->performAnotherOrder(OrderId::ComputerAI,0,0,NULL,UnitId::None);

				}

				prepareForNextOrder(unit);

			}
			else { //54875

				if(unit->orderQueueHead != NULL) {
					unit->userActionFlags |= 1;
					prepareForNextOrder(unit);
				}
				else //54896
					unit->orderComputerCL(units_dat::ReturnToIdleOrder[unit->id]);

			}

		}

	}

	;

	void orders_ResearchTech(CUnit* unit) {

		bool bStopThere = false;

		if(unit->building.techType != TechId::None) {

			unit->building.upgradeResearchTime--;

			if(unit->building.upgradeResearchTime != 0xFFFF) {

				u8 bIsResearched;

				if(unit->building.techType < TechId::Restoration)
					bIsResearched = TechSc->isResearched[unit->playerId][unit->building.techType];
				else
					bIsResearched = TechBw->isResearched[unit->playerId][unit->building.techType - TechId::Restoration];

				if(	bIsResearched == 0 &&
					!(*CHEAT_STATE & CheatFlags::OperationCwal)
				)
					bStopThere = true;

			}

			//54923
			if(!bStopThere) {

				playResearchCompleteSound(unit);

				if(unit->building.techType < TechId::Restoration)
					TechSc->isResearched[unit->playerId][unit->building.techType] = 1;
				else if (*IS_BROOD_WAR)
					TechBw->isResearched[unit->playerId][unit->building.techType - TechId::Restoration] = 1;

				scbw::refreshConsole();

			}

		}

		if(!bStopThere) {

			CImage* current_image;

			current_image = unit->sprite->images.head;

			while(current_image != NULL) {
				current_image->playIscriptAnim(IscriptAnimation::WorkingToIdle);
				current_image = current_image->link.next;
			}

			//code I could not understand enough to convert
			__asm {

				PUSHAD

				MOV ESI, unit
				MOVZX ECX, byte ptr [ESI+0xC8] //unit->building.techType
				MOVZX EAX, byte ptr [ESI+0x4C] //unit->playerId
				MOV EDX, ECX
				SHR EDX, 0x3
				AND ECX, 0x80000007
				LEA EAX, [EAX+EAX*0x2]
				LEA EAX, [EDX+EAX*0x2+0x0058F230] //EAX = &(TechBw->isResearched[unit->playerId][unit->building.techType CONVERTED?])
				JNS lbl549C7

				DEC ECX
				OR ECX, -0x8
				INC ECX

				lbl549C7:
				MOV DL, BL
				SHL DL, CL
				MOV CL, [EAX]
				XOR EDI, EDI
				NOT DL
				AND CL, DL
				MOV [EAX], CL

				POPAD

			}

			unit->building.techType = TechId::None;

			if(unit->pAI != NULL) {

				unit->userActionFlags |= 1;

				if(unit->mainOrderId != OrderId::Die) {

					bool bStopLoop = (unit->orderQueueTail == NULL);

					while(!bStopLoop) {
						if(	!orders_dat::CanBeInterrupted[unit->orderQueueTail->orderId] &&
							unit->orderQueueTail->orderId != OrderId::ComputerAI
						)
							bStopLoop = true;
						else
							removeOrderFromUnitQueue(unit,unit->orderQueueTail);
					}

					unit->performAnotherOrder(OrderId::ComputerAI,0,0,NULL,UnitId::None);
					
				}

				prepareForNextOrder(unit);

			}
			else { //54A47

				if(unit->orderQueueHead != NULL) {
					unit->userActionFlags |= 1;
					prepareForNextOrder(unit);
				}
				else
					unit->orderComputerCL(units_dat::ReturnToIdleOrder[unit->id]);

			}

		}

	}

	;

} //namespace hooks

;

//-------- Helper function definitions. Do NOT modify! --------//

namespace {

	const u32 Func_isUnitUpgradeAvailable = 0x004033D0;
	bool isUnitUpgradeAvailable(CUnit* unit) {

		static Bool32 bPreResult;

		__asm {
			PUSHAD
			MOV EAX, unit
			CALL Func_isUnitUpgradeAvailable
			MOV bPreResult, EAX
			POPAD
		}

		return (bPreResult != 0);

	}

	;

	const u32 Func_ApplySpeedUpgradeFromUpgradeType = 0x00454540;
	void ApplySpeedUpgradeFromUpgradeType(CUnit* unit, u8 upgradeType) {

		__asm {
			PUSHAD
			MOV AL, upgradeType
			PUSH unit
			CALL Func_ApplySpeedUpgradeFromUpgradeType
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

	const u32 Func_playUpgradeCompleteSound = 0x0048F070;
	void playUpgradeCompleteSound(CUnit* unit) {

		__asm {
			PUSHAD
			MOV ESI, unit
			CALL Func_playUpgradeCompleteSound
			POPAD
		}

	}

	;

	const u32 Func_playResearchCompleteSound = 0x0048F150;
	void playResearchCompleteSound(CUnit* unit) {

		__asm {
			PUSHAD
			MOV ESI, unit
			CALL Func_playResearchCompleteSound
			POPAD
		}

	}

	;

	const u32 Func_setUpgradeLevel = 0x004CE770;
	void setUpgradeLevel(u32 playerId, u32 upgradeType, u8 upgradeLevel) {

		__asm {
			PUSHAD
			MOV DL, upgradeLevel
			MOV ECX, upgradeType
			MOV EAX, playerId
			CALL Func_setUpgradeLevel
			POPAD
		}

	}

	;

} //Unnamed namespace

//End of helper functions

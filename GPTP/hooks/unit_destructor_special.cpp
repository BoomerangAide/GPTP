//All functions in this file are heavily involved with memory management.
//Do NOT modify anything unless you really know what you are doing.

#include "unit_destructor_special.h"
#include "../SCBW/api.h"
#include "../hook_tools.h"

//Identical to 00466170  sub_466170
//Not hooked but used by hooked unitDestructorSpecialHook
void killAllHangarUnits(CUnit* unit) {

	u32 random_value;

	while(unit->carrier.inHangarCount != 0) {
		CUnit* childInside = unit->carrier.inHangarChild;
		unit->carrier.inHangarChild = childInside->interceptor.hangar_link.prev;
		childInside->interceptor.parent = NULL;
		childInside->remove();
		unit->carrier.inHangarCount--;
	}

	while (unit->carrier.outHangarCount != 0) {

		CUnit* childOutside = unit->carrier.outHangarChild;

		childOutside->interceptor.parent = NULL;

		//Kill interceptors only (Scarabs will defuse anyway)
		if (childOutside->id != UnitId::scarab) {

			u16 randomDeathTimer;

			//hardcoded RNG management
			if(*IS_IN_GAME_LOOP) {

				static u32* randomnessCounter_0051C6AC = (u32*)0x0051C6AC;
				static u32* randomnessCounter_0051CA18 = (u32*)0x0051CA18;

				*randomnessCounter_0051C6AC = *randomnessCounter_0051C6AC + 1;
				*randomnessCounter_0051CA18 = *randomnessCounter_0051CA18 + 1;
				*lastRandomNumber = (*lastRandomNumber * 0x015A4E35) + 1;

				//number between 0 and 32766
				random_value = (*lastRandomNumber / 65536) & 0x7FFF;

			}
			else
				random_value = 0;

			//should produce a number between 15 and 45 (or maybe 46)
			randomDeathTimer = ( (random_value * 31) / 32768 ) + 15;

			if (
				childOutside->removeTimer == 0 || 
				randomDeathTimer < childOutside->removeTimer
			)
				childOutside->removeTimer = randomDeathTimer;

		}

		//66239
		unit->carrier.outHangarChild = childOutside->interceptor.hangar_link.prev;
		childOutside->interceptor.hangar_link.prev = NULL;
		childOutside->interceptor.hangar_link.next = NULL;
		unit->carrier.outHangarCount--;

	} //while (unit->carrier.outHangarCount != 0)

	unit->carrier.outHangarChild = NULL;

}

;

//Identical to 00468770 sub_468770
//Not hooked but used by hooked unitDestructorSpecialHook
void freeResourceContainer(CUnit* resource) {

	resource->building.resource.gatherQueueCount = 0;

	CUnit* worker = resource->building.resource.nextGatherer;
	CUnit* nextWorker;

	while (worker != NULL) {

		if (worker->worker.harvest_link.prev != NULL) //untie the previous worker from current worker (tie it to next)
			(worker->worker.harvest_link.prev)->worker.harvest_link.next = worker->worker.harvest_link.next;
		else
			resource->building.resource.nextGatherer = worker->worker.harvest_link.next;

		if (worker->worker.harvest_link.next != NULL) //untie the next worker from current worker (tie it to previous)
			(worker->worker.harvest_link.next)->worker.harvest_link.prev = worker->worker.harvest_link.prev;

		nextWorker = worker->worker.harvest_link.next;

		worker->worker.harvestTarget = NULL;
		worker->worker.harvest_link.prev = NULL;
		worker->worker.harvest_link.next = NULL;

		worker = nextWorker;

	}

}

;

//Hooked down there instead of a specific inject.cpp
void unitDestructorSpecialHook(CUnit* unit) {

	//Destroy interceptors and scarabs
	if (
		unit->id == UnitId::ProtossCarrier || 
		unit->id == UnitId::Hero_Gantrithor || 
		unit->id == UnitId::ProtossReaver || 
		unit->id == UnitId::Hero_Warbringer
	) 
	{
		killAllHangarUnits(unit);
	}
	else

	//Destroy nuclear missiles mid-launch
	if (
		unit->id == UnitId::TerranGhost || 
		unit->id == UnitId::Hero_SarahKerrigan || 
		unit->id == UnitId::Hero_AlexeiStukov|| 
		unit->id == UnitId::Hero_SamirDuran|| 
		unit->id == UnitId::Hero_InfestedDuran
	) 
	{

		if (unit->building.ghostNukeMissile != NULL) {
			(unit->building.ghostNukeMissile)->sprite->playIscriptAnim(IscriptAnimation::Death,true);
			unit->building.ghostNukeMissile = NULL;
		} 

	}
	else

	//Is a harvestable mineral patch or gas building
	if (
		(unit->id >= UnitId::ResourceMineralField && unit->id <= UnitId::ResourceMineralFieldType3) ||
		(	
			(unit->id == UnitId::TerranRefinery || unit->id == UnitId::ProtossAssimilator || unit->id == UnitId::ZergExtractor) &&
			unit->status & UnitStatus::Completed
		)	/*this test is equivalent to function unit_isGeyserUnit @ 004688B0*/
	  ) 
	{
		freeResourceContainer(unit);
	}
	else

	if(unit->id >= UnitId::TerranNuclearMissile && unit->id <= UnitId::ProtossPylon) {

		//In original code, this part is handled by an internal id_based array and a
		//switch-like system to execute the code corresponding to the unit
		//If the id were within the previous test, the resource units would go through
		//those tests

		//Is a scarab or interceptor
		if (unit->id == UnitId::ProtossScarab || unit->id == UnitId::ProtossInterceptor) {

			if (unit->status & UnitStatus::Completed) {

				//following code is equivalent to a jump to function 004652A0
				//with unit as parameter in EAX

				if (unit->interceptor.parent != NULL) {

					if (unit->interceptor.hangar_link.next != NULL) //untie the next interceptor/scarab from current interceptor/scarab (tie it to previous)
						(unit->interceptor.hangar_link.next)->interceptor.hangar_link.prev = unit->interceptor.hangar_link.prev;
					else {
						if (unit->interceptor.isOutsideHangar)
							unit->interceptor.parent->carrier.outHangarChild = unit->interceptor.hangar_link.prev;
						else
							unit->interceptor.parent->carrier.inHangarChild = unit->interceptor.hangar_link.prev;
					}

					if (unit->interceptor.isOutsideHangar)
						unit->interceptor.parent->carrier.outHangarCount--;
					else
						unit->interceptor.parent->carrier.inHangarCount--;

					if (unit->interceptor.hangar_link.prev != NULL)  //untie the previous interceptor/scarab from current interceptor/scarab (tie it to next)
						(unit->interceptor.hangar_link.prev)->interceptor.hangar_link.next = unit->interceptor.hangar_link.next;

				}
				else {
					unit->interceptor.hangar_link.prev = NULL;
					unit->interceptor.hangar_link.next = NULL;
				}

			}

		}
		else

		//Is a Nuclear Silo
		if (unit->id == UnitId::TerranNuclearSilo) {

			if (unit->building.silo.nuke != NULL) {
				(unit->building.silo.nuke)->remove();
				unit->building.silo.nuke = NULL;
			}

		}
		else


		//Is a Nuclear Missile
		if (unit->id == UnitId::TerranNuclearMissile) {

			if (unit->connectedUnit != NULL && unit->connectedUnit->id == UnitId::TerranNuclearSilo) {
				(unit->connectedUnit)->building.silo.nuke = NULL;
				(unit->connectedUnit)->building.silo.isReady = false;
			}

		}
		else

		//Is a Protoss Pylon
		if (unit->id == UnitId::ProtossPylon) {

			//code from hooks::removePsiField(CUnit* unit)
			//copied because the content is hardcoded here

			if (unit->building.pylonAura != NULL) {
				unit->building.pylonAura->free();	//should be equivalent to SpriteDestructor @ 00497B40
				unit->building.pylonAura = NULL;
			}

			//begin hardcoding removeFromPsiProviderList @ 00493100 to avoid use of a helper

			if (unit->psi_link.prev != NULL)  //untie the previous pylon from current pylon (tie it to next)
				(unit->psi_link.prev)->psi_link.next = unit->psi_link.next;
			if (unit->psi_link.next != NULL)  //untie the next pylon from current pylon (tie it to previous)
				(unit->psi_link.next)->psi_link.prev = unit->psi_link.prev;

			if (unit == *firstPsiFieldProvider)
				*firstPsiFieldProvider = unit->psi_link.next;

			unit->psi_link.prev = NULL;
			unit->psi_link.next = NULL;

			//end hardcoding removeFromPsiProviderList @ 00493100 to avoid use of a helper

			*canUpdatePoweredStatus = true;

		}
		else

		//Is a Nydus Canal
		if (unit->id == UnitId::ZergNydusCanal) {

			CUnit* nydusExit = unit->building.nydusExit;

			if (nydusExit != NULL) {
				unit->building.nydusExit = NULL;
				nydusExit->building.nydusExit = NULL;
				nydusExit->remove();
			}

		}

	}

}

;

//-------- Hooking that usually happen in an inject file --------//

void __declspec(naked) unitDestructorSpecialWrapper() {

	static CUnit* unit;

	__asm {
		MOV unit, EAX
		PUSHAD
	}

	unitDestructorSpecialHook(unit);

	__asm {
		POPAD
		RETN
	}

}

;

namespace hooks {

void injectUnitDestructorSpecial() {
	jmpPatch(unitDestructorSpecialWrapper, 0x0049F4A0, 2);
}

} //hooks

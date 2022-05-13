#include "CMDRECV_Stimpack.h"
#include <SCBW/api.h>
#include "Events/Events.h"

namespace hooks {

#ifdef EVENTS_SYSTEM
void CMDRECV_Stimpack() {

	const int stimHPcost = 2560; //2560 / 256 => 10 hp cost on ingame display
	const int stimTimerBase = 37;
	
	CUnit* current_unit;

	std::vector<int*> events_check_override_arg(2);
	bool bCanUseStims;
	bool bReplaceNormalBehavior;

	*selectionIndexStart = 0;
	current_unit = getActivePlayerNextSelection();

	while (current_unit != NULL) {

		bCanUseStims = true;
		bReplaceNormalBehavior = false;

		events_check_override_arg[0] = (int*)current_unit;
		events_check_override_arg[1] = (int*)&bCanUseStims;

		EventManager::EventCalling(EventId::STIMPACK_PLAYER_UNIT_CAN_USE, &bReplaceNormalBehavior, events_check_override_arg);

		if(!bReplaceNormalBehavior)
			bCanUseStims = bCanUseStims && current_unit->canUseTech(TechId::Stimpacks, *ACTIVE_NATION_ID) == 1 && current_unit->hitPoints > stimHPcost;

		if (bCanUseStims) {

			int random_value;

			if(!*IS_IN_GAME_LOOP)
				random_value = 0;
			else {

				static u32* randomnessCounter_0051C68C = (u32*)0x0051C68C;
				static u32* randomnessCounter_0051CA18 = (u32*)0x0051CA18;

				*randomnessCounter_0051C68C = (*randomnessCounter_0051C68C) + 1;
				*randomnessCounter_0051CA18 = (*randomnessCounter_0051CA18) + 1;
				*lastRandomNumber = ((*lastRandomNumber) * 0x015A4E35) + 1;

				random_value = ((*lastRandomNumber) / 65535) & 32767;

			}

			random_value = (2 * random_value) / 32768; //result is 0 or 1
			scbw::playSound(SoundId::Terran_MARINE_TMaSti00_WAV + random_value, current_unit);

			current_unit->damageHp(stimHPcost, NULL, -1, true);

			if (current_unit->stimTimer < stimTimerBase) {
				current_unit->stimTimer = stimTimerBase;
				current_unit->updateSpeed();
			}


		}

		current_unit = getActivePlayerNextSelection();

	}

}
#else
void CMDRECV_Stimpack() {

	const int stimHPcost = 2560; //2560 / 256 => 10 hp cost on ingame display
	const int stimTimerBase = 37;
	
	CUnit* current_unit;

	*selectionIndexStart = 0;
	current_unit = getActivePlayerNextSelection();

	while (current_unit != NULL) {

		if (current_unit->canUseTech(TechId::Stimpacks, *ACTIVE_NATION_ID) == 1 && current_unit->hitPoints > stimHPcost) {

			int random_value;

			if(!*IS_IN_GAME_LOOP)
				random_value = 0;
			else {

				static u32* randomnessCounter_0051C68C = (u32*)0x0051C68C;
				static u32* randomnessCounter_0051CA18 = (u32*)0x0051CA18;

				*randomnessCounter_0051C68C = (*randomnessCounter_0051C68C) + 1;
				*randomnessCounter_0051CA18 = (*randomnessCounter_0051CA18) + 1;
				*lastRandomNumber = ((*lastRandomNumber) * 0x015A4E35) + 1;

				random_value = ((*lastRandomNumber) / 65535) & 32767;

			}

			random_value = (2 * random_value) / 32768; //result is 0 or 1
			scbw::playSound(SoundId::Terran_MARINE_TMaSti00_WAV + random_value, current_unit);

			current_unit->damageHp(stimHPcost, NULL, -1, true);

			if (current_unit->stimTimer < stimTimerBase) {
				current_unit->stimTimer = stimTimerBase;
				current_unit->updateSpeed();
			}


		}

		current_unit = getActivePlayerNextSelection();

	}

}
#endif

;

} //namespace hooks

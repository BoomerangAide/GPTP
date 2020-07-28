#include "cloak_tech.h"
#include "../SCBW/scbwdata.h"
#include "../SCBW/api.h"
#include "../hook_tools.h"

/*

There are 9 places where StarCraft checks unit ID for cloaking.

* 0x00429210: BTNSCOND_IsCloaked  (Used by Infested Kerrigan, Infested Duran, and Mixed Cloakable Group)(hooked in hooks\interface\btns_cond)
* 0x004292C0: BTNSCOND_CanCloak   (Used by Infested Kerrigan, Infested Duran, and Mixed Cloakable Group)(hooked in hooks\interface\btns_cond)
* 0x00491B30: ApplyCloakingOrder  (Used by all spell-cloaking units)
* 0x004C0720: CMDRECV_Cloak       (Used by all spell-cloaking units)(hooked in hooks\recv_commands\CMDRECV_Cloak_Decloak)
* 0x0043B970: AI_orderUnitCloaking(Used by AI-controlled units)
* 0x004C0660: CMDRECV_Decloak     (Used by all spell-cloaking units)(hooked in hooks\recv_commands\CMDRECV_Cloak_Decloak)
* 0x00491A50: getCloakingTech     (Used by "Create Unit With Properties" in StarEdit + create_unit opcode in aiscript.bin)
* 0x00491A90: getCloakingEnergyConsumption (Used by energy regen; This is editable from the energy hook stuff)
* 0x00423540: currentUnitSelectionCanCloak(Called by CMDACT_Cloak for all units; also checks energy amount)

Note that Wraiths and Ghosts use BTNSCOND_CanCloak_0 and BTNSCOND_IsCloaked_0,
which does NOT check unit IDs at all. (FireGraft has it wrong when it says that
Wraiths use "Can Cloak" and Infested Kerrigan uses "Can Cloak - Mixed")

Note: previous statements not from UndeadStar, not rechecked in details

*/

namespace {

//-------- Actual cloak orders --------//

//equivalent to 00491B30 ApplyCloakingOrder, not injected
void applyCloakingOrderHook(CUnit* unit) {

	if (unit->status & UnitStatus::RequiresDetection)
		return;

	//original code is hardcoded instead of using a function like getCloakingTech
	const u16 energyCost = techdata_dat::EnergyCost[hooks::getCloakingTech(unit)] * 256;

	if (scbw::isCheatEnabled(CheatFlags::TheGathering) || unit->energy >= energyCost) {

		if (!scbw::isCheatEnabled(CheatFlags::TheGathering))
			unit->energy -= energyCost;

		//original code is hardcoded instead of using a function like setSecondaryOrder
		unit->setSecondaryOrder(OrderId::Cloak);

	}

}

;

void __declspec(naked) cloakingTechWrapper_AI_cloakUnit() {

	static CUnit* unit;

	__asm {
		MOV unit, ESI
		PUSHAD
	}

	if (!(unit->status & UnitStatus::RequiresDetection)) {
		if (unit->canUseTech(hooks::getCloakingTech(unit), unit->playerId) == 1)
			applyCloakingOrderHook(unit);
	}

	__asm {
		POPAD
		RETN
	}
}

;

//-------- Other wrappers --------//

void __declspec(naked) getCloakingTechWrapper() {

	static CUnit* unit;
	static u8 result;

	__asm {
		MOV unit, EAX
		PUSHAD
	}

	result = hooks::getCloakingTech(unit);

	__asm {
		POPAD
		MOV AL, result
		RETN
	}
}

;

Bool32 __cdecl currentUnitSelectionCanCloakWrapper() {

	RaceId::Enum race;

	for (int i = 0; i < SELECTION_ARRAY_LENGTH; ++i) {

		CUnit* unit = clientSelectionGroup->unit[i];

		if (unit != NULL) {
			if (
				scbw::isCheatEnabled(CheatFlags::TheGathering) ||
				unit->energy >= techdata_dat::EnergyCost[hooks::getCloakingTech(unit)] * 256
			)
				return true;
		}

	}

	race = (*activePortraitUnit)->getRace();

	scbw::showErrorMessageWithSfx(
		(*activePortraitUnit)->playerId, 
		864 + race, //"Not enough energy."
		SoundId::Zerg_Advisor_ZAdErr06_WAV + race
	);

	return false;

}

;

} //unnamed namespace

namespace hooks {

void injectCloakingTechHooks() {
  jmpPatch(cloakingTechWrapper_AI_cloakUnit,    0x0043B970, 1);
  jmpPatch(getCloakingTechWrapper,              0x00491A50, 3);
  jmpPatch(currentUnitSelectionCanCloakWrapper, 0x00423540, 1);
}

} //hooks

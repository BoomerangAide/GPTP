#include "CMDRECV_Cloak_Decloak.h"
#include <SCBW\api.h>

//Helper functions declaration

namespace {

void applyCloakingOrderHelper(CUnit* unit);	//91B30

} //unnamed namespace

namespace hooks {

void CMDRECV_Cloak() {

	CUnit* current_unit;
	*selectionIndexStart = 0; 

	current_unit = getActivePlayerNextSelection();

	while (current_unit != NULL) {

		u8 techId;

		if 
		(
			current_unit->id == UnitId::TerranGhost ||
			current_unit->id == UnitId::Hero_SarahKerrigan ||
			current_unit->id == UnitId::Hero_AlexeiStukov ||
			current_unit->id == UnitId::Hero_SamirDuran ||
			current_unit->id == UnitId::Hero_InfestedDuran ||
			current_unit->id == UnitId::Hero_InfestedKerrigan
		)
			techId = TechId::PersonnelCloaking;
		else
		if (current_unit->id == UnitId::TerranWraith || current_unit->id == UnitId::Hero_TomKazansky)
			techId = TechId::CloakingField;
		else
			techId = TechId::None;

		if (current_unit->canUseTech(techId, *ACTIVE_NATION_ID) == 1)
			applyCloakingOrderHelper(current_unit);

		current_unit = getActivePlayerNextSelection();

	}

}

;

void CMDRECV_Decloak() {
	
	CUnit* current_unit;
	*selectionIndexStart = 0;

	current_unit = getActivePlayerNextSelection();

	while (current_unit != NULL) {

		u8 techId;

		if 
		(
			current_unit->id == UnitId::TerranGhost ||
			current_unit->id == UnitId::Hero_SarahKerrigan ||
			current_unit->id == UnitId::Hero_AlexeiStukov ||
			current_unit->id == UnitId::Hero_SamirDuran ||
			current_unit->id == UnitId::Hero_InfestedDuran ||
			current_unit->id == UnitId::Hero_InfestedKerrigan
		)
			techId = TechId::PersonnelCloaking;
		else
		if (current_unit->id == UnitId::TerranWraith || current_unit->id == UnitId::Hero_TomKazansky)
			techId = TechId::CloakingField;
		else
			techId = TechId::None;

		if (current_unit->canUseTech(techId, *ACTIVE_NATION_ID) == 1)
			current_unit->setSecondaryOrder(OrderId::Decloak); //original code is hardcoded instead of using a function like setSecondaryOrder

		current_unit = getActivePlayerNextSelection();

	}

}

;

} //hooks

//-------- Helper function definitions. Do NOT modify! --------//

namespace {

//equivalent to 00491B30 ApplyCloakingOrder, based on old GPTP code
void applyCloakingOrderHelper(CUnit* unit) {

	if (!(unit->status & UnitStatus::RequiresDetection)) {

		u8 techId;
		u16 energyCost;

		if 
		(
			unit->id == UnitId::TerranGhost ||
			unit->id == UnitId::Hero_SarahKerrigan ||
			unit->id == UnitId::Hero_AlexeiStukov ||
			unit->id == UnitId::Hero_SamirDuran ||
			unit->id == UnitId::Hero_InfestedDuran ||
			unit->id == UnitId::Hero_InfestedKerrigan
		)
			techId = TechId::PersonnelCloaking;
		else
		if (unit->id == UnitId::TerranWraith || unit->id == UnitId::Hero_TomKazansky)
			techId = TechId::CloakingField;
		else
			techId = TechId::None;

		energyCost = techdata_dat::EnergyCost[techId] * 256;

		if (scbw::isCheatEnabled(CheatFlags::TheGathering) || unit->energy >= energyCost) {

			if (!scbw::isCheatEnabled(CheatFlags::TheGathering))
				unit->energy -= energyCost;

			//original code is hardcoded instead of using a function like setSecondaryOrder
			unit->setSecondaryOrder(OrderId::Cloak);

		}

	}

}

;

} //unnamed namespace

//End of helper functions

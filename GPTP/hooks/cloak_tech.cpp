#include "cloak_tech.h"
#include <SCBW/scbwdata.h>

namespace hooks {

//Returns the tech ID used by this unit for the cloaking spell.
//For the cloaking energy consumption, see updateUnitEnergy.cpp
//Also see 
//		hooks\interface\btns_cond 
//		hooks\recv_commands\CMDRECV_Cloak_Decloak
//where it is hardcoded in the functions (so changes need 
//to be copied there).
//Note: an identical check is performed in hooks\update_unit_state.cpp
//for updateUnitEnergy(), but it directly return the cloaking energy
//cost without using a tech Id
u8 getCloakingTech(CUnit* unit) {

	u8 techId;

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

	return techId;

}

} //hooks

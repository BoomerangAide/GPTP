#include "psi_field.h"
#include "../SCBW/scbwdata.h"

namespace hooks {

//Check if the given unit id can generate psi fields
bool canMakePsiField(u16 unitId) {
	//Default StarCraft behavior
	if (unitId == UnitId::ProtossPylon)
		return true;
	return false;
}

;

//Actual state check whether a unit can generate a psi field
bool isReadyToMakePsiField(CUnit* unit) {
	//Default StarCraft behavior

	if (unit->id == UnitId::ProtossPylon)
		return true;

	return false;
}

;

} //hooks

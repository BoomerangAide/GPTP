#include "wireframe.h"
#include "../../../SCBW/api.h"

namespace hooks {

//was initially in a building_morph file
//the rest of the function this fragment is from use
//too many unknowns, thus this is only a fragment.
//This allow to change the wireframe displayed for
//the unit, the special case being that zerg morphing
//buildings show the wireframe of the new building
//before construction is complete
u16 statdata_UnitWireframeUpdate_Fragment(CUnit* unit) {

	u16 unitId;
	bool bUseCurrentUnitId = true;

	if(!(unit->status & UnitStatus::Completed)) {

		unitId = unit->buildQueue[unit->buildQueueSlot];

		if(
			unitId == UnitId::ZergHive ||
			unitId == UnitId::ZergLair ||
			unitId == UnitId::ZergGreaterSpire ||
			unitId == UnitId::ZergSporeColony ||
			unitId == UnitId::ZergSunkenColony
		)
			bUseCurrentUnitId = false;

	}

	//5694F
	if(bUseCurrentUnitId)
		unitId = unit->id;

	return unitId;

}

} //hooks

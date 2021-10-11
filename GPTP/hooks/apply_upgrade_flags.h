//Header file for the Apply Upgrade Flags hook module.
#pragma once
#include <SCBW/structures.h>

namespace hooks {

void applyUpgradeFlagsToNewUnitHook(CUnit* unit);						//54370
void applyUpgradeFlagsToExistingUnitsHook(CUnit* unit, u8 upgradeId);	//54540

void injectApplyUpgradeFlags();

} //hooks

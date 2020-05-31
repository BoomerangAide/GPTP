#include <SCBW/structures/CUnit.h>

namespace hooks {

const char* getWeaponTooltipString(u8 weaponId, CUnit* unit);
const char* getArmorTooltipString(CUnit* unit);
const char* getShieldTooltipString(CUnit* unit);
const char* getSpecialTooltipString(u16 iconUnitId, CUnit* unit);

void injectUnitTooltipHook();

} //hooks

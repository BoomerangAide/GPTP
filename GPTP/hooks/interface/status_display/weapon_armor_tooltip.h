#include <SCBW/structures/CUnit.h>

namespace hooks {

//Note: none of these functions are hooked, instead the only function calling them is hooked
const char* getWeaponTooltipString(u8 weaponId, CUnit* unit);
const char* getArmorTooltipString(CUnit* unit);
const char* getShieldTooltipString(CUnit* unit);
const char* getSpecialTooltipString(u16 iconUnitId, CUnit* unit);

void injectUnitTooltipHook();

} //hooks

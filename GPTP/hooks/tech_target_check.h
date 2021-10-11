//Header file for the Tech Target Check hook module.
#include <SCBW/structures/CUnit.h>

namespace hooks {

u16 getTechUseErrorMessageHook(CUnit* target, u8 castingPlayer, u16 techId);	//91E80

void injectTechTargetCheckHooks();

} //hooks

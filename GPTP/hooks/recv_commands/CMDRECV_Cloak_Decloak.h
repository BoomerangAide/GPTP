#pragma once
#include "../../SCBW/structures/CUnit.h"

namespace hooks {

void CMDRECV_Decloak();	//C0660
void CMDRECV_Cloak();	//C0720

void injectCloakingTechHooks();

} //hooks

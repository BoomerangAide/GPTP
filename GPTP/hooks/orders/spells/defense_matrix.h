#pragma once
#include "../../../SCBW/structures/CUnit.h"

namespace hooks {

void CreateDefensiveMatrix(CUnit* unit);	//54F90
void ApplyDefensiveMatrix(CUnit* unit);		//55010
void orders_DefensiveMatrix(CUnit* unit);	//550A0

void injectDefensiveMatrixHooks();

} //hooks
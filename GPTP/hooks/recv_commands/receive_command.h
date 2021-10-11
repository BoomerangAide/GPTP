#pragma once
#include "../../SCBW/structures/CUnit.h"

namespace hooks {

void receive_command(int x, int y, CUnit* unitParam, u32 wUnitType, u32 bActionOrder, u32 bCommandType); //9AB00

void injectRecvCmdHook();

} //hooks

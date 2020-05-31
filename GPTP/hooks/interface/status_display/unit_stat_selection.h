#pragma once
#include "../../../SCBW/structures.h"

namespace hooks {

bool UnitStatCond_Selection();									//24660
void UnitStatAct_Selection(BinDlg* dialog);						//25960
u32 statdata_UnitWireframeSelectUpdate_Part1(BinDlg* dialog);	//56F50
void statdata_UnitWireframeSelectUpdate_Part3(BinDlg* dialog);	//57046 (56F50)

void injectUnitStatSelectionHooks();

}
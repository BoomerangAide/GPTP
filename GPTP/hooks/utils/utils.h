#pragma once
#include "../../SCBW/structures.h"

namespace hooks {

void InitializeDialog(BinDlg* dialog, void* dlgFunc);	//19D20
bool unit_IsStandardAndMovable(CUnit* unit);			//7B770
CUnit* getActivePlayerFirstSelection();					//9A850

void injectUtilsHooks();

} //hooks

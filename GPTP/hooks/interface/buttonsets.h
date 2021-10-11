#pragma once
#include <SCBW/api.h>

namespace hooks {

void updateButtonSetEx();						//58BC0
void updateButtonSet_Sub458D50();				//58D50
void updateButtonSet();							//58DE0
void updateButtonSet_Sub4591D0();				//591D0
void updateCurrentButtonset();					//599A0

void statbtn_BIN_CustomCtrlID(BinDlg* dialog);	//59AD0

BUTTON_SET* getButtonSet(int index);

void injectButtonSetHooks();

} //hooks

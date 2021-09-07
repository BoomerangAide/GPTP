#pragma once
#include "../../../SCBW/structures.h"

namespace hooks {

void UnitStatAct_Default(BinDlg* dialog);		//25EE0
void UnitStatAct_Powerup(BinDlg* dialog);		//26EE0
void UnitStatAct_Standard(BinDlg* dialog);		//26F50
void UnitStatAct_Egg(BinDlg* dialog);			//27260
void UnitStatAct_Archon(BinDlg* dialog);		//273E0
void UnitStatAct_CarrierReaver(BinDlg* dialog);	//274A0
void UnitStatAct_Building(BinDlg* dialog);		//27890
void UnitStatAct_Dropship(BinDlg* dialog);		//27C90
void UnitStatAct_Overlord(BinDlg* dialog);		//27D30

void injectUnitStatActHooks();

}

#pragma once
#include "../../../SCBW/structures.h"

namespace hooks {

void UnitStatAct_Default(BinDlg* dialog);		//00425EE0
void UnitStatAct_Powerup(BinDlg* dialog);		//00426EE0
void UnitStatAct_Standard(BinDlg* dialog);		//00426F50
void UnitStatAct_Egg(BinDlg* dialog);			//00427260
void UnitStatAct_Archon(BinDlg* dialog);		//004273E0
void UnitStatAct_CarrierReaver(BinDlg* dialog);	//004274A0
void UnitStatAct_Building(BinDlg* dialog);		//00427890
void UnitStatAct_Dropship(BinDlg* dialog);		//00427C90
void UnitStatAct_Overlord(BinDlg* dialog);		//00427D30

void injectUnitStatActHooks();

}
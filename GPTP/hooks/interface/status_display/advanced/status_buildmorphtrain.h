#pragma once
#include "../../../../SCBW/structures.h"

namespace hooks {

void stats_AddOnBuilder(BinDlg* dialog);						//00425F30
void stats_IncompleteUnit(BinDlg* dialog);						//004260C0
void stats_QueueProgress(BinDlg* dialog);						//004268D0
void stats_MorphProgress(BinDlg* dialog, int index, u32 unk);	//00457250

void injectStatusBuildMorphTrain_Hooks();

}
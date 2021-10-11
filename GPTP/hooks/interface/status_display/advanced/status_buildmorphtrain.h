#pragma once
#include "../../../../SCBW/structures.h"

namespace hooks {

void stats_AddOnBuilder(BinDlg* dialog);						//25F30
void stats_IncompleteUnit(BinDlg* dialog);						//260C0
void stats_QueueProgress(BinDlg* dialog);						//268D0
void stats_MorphProgress(BinDlg* dialog, int index, u32 unk);	//57250

void injectStatusBuildMorphTrain_Hooks();

}

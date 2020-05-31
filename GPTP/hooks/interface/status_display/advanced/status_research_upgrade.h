#pragma once
#include "../../../../SCBW/structures.h"

namespace hooks {

void stats_upgrade_in_progress(BinDlg* dialog);		//00426500
void stats_research_in_progress(BinDlg* dialog);	//004266F0

void injectStatusResearchUpgradeHooks();

}
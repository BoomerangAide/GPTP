#pragma once
#include "../../../../SCBW/structures.h"

namespace hooks {

void stats_upgrade_in_progress(BinDlg* dialog);		//26500
void stats_research_in_progress(BinDlg* dialog);	//266F0

void injectStatusResearchUpgradeHooks();

}

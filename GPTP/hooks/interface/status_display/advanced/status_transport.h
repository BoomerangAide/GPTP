#pragma once
#include "../../../../SCBW/structures.h"

namespace hooks {

void stats_transports_loaded_units_buttons(BinDlg* dialog);	//00424BA0
void stats_transports_cache_for_display(BinDlg* dialog);	//00424FC0

void injectStatusTransportHooks();

}
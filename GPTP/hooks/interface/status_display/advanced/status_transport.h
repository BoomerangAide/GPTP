#pragma once
#include "../../../../SCBW/structures.h"

namespace hooks {

void stats_transports_loaded_units_buttons(BinDlg* dialog);	//24BA0
void stats_transports_cache_for_display(BinDlg* dialog);	//24FC0

void injectStatusTransportHooks();

}

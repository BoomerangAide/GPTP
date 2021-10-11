#pragma once
#include "../../../../SCBW/structures.h"

namespace hooks {

void stats_text_energy_timer(BinDlg* dialog);	//25A30
void stats_text_status_ranking(BinDlg* dialog);	//25B50
void stats_text_kills(BinDlg* dialog);			//25DD0
void stats_text_hp_shield(BinDlg* dialog);		//263E0

void injectStatusBaseTextHooks();

}

#pragma once
#include "../../../../SCBW/structures.h"

namespace hooks {

void stats_text_energy_timer(BinDlg* dialog);	//00425A30
void stats_text_status_ranking(BinDlg* dialog);	//00425B50
void stats_text_kills(BinDlg* dialog);			//00425DD0
void stats_text_hp_shield(BinDlg* dialog);		//004263E0

void injectStatusBaseTextHooks();

}
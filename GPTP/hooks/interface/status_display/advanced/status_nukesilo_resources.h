#pragma once
#include "../../../../SCBW/structures.h"

namespace hooks {

void stats_resources(BinDlg* dialog);	//00426190
void stats_nukesilo(BinDlg* dialog);	//00426FF0

void injectStatusNukeSilo_Resources_Hooks();

}
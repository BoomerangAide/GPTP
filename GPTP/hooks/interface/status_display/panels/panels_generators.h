#pragma once
#include <SCBW/structures.h>

namespace hooks {

void StatsNukesCount(BinDlg* dialog, u32 index);				//25310
void StatHangerCount(BinDlg* dialog, u32 index);				//253D0
void StatsShieldLevel(BinDlg* dialog, u32 index);				//25510
void StatsArmorLevel(BinDlg* dialog, u32 index);				//25600
void StatsWeaponLevel(BinDlg* dialog, u32 index, u32 weaponId);	//25790
Bool32 StatSpidermineCount(BinDlg* dialog, u32 index);			//26300

void injectPanelsGeneratorsHook();

}

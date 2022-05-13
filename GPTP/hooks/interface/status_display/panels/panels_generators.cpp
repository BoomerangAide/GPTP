#include "panels_generators.h"
#include <SCBW/api.h>
#include <Events/Events.h>

//Helper functions declaration

namespace {

void disableDialog(BinDlg* dialog);														//18640
void showDialog(BinDlg* dialog);														//186A0
void updateDialog(BinDlg* dialog);														//1C400
void SC_sprintf_s(char* buffer, int buffer_size, const char* format_string, int value);	//1F1B0

} //unnamed namespace

namespace hooks {

#ifdef EVENTS_SYSTEM
void StatsNukesCount(BinDlg* dialog, u32 index) {

	static char**	queues_buffers	= (char**)0x00519F40;
	static u32*		u32_0068C1E0	= (u32*)  0x0068C1E0; //unknown, maybe not always store the same thing

	BinDlg* current_dialog;
	CUnit* unit = *activePortraitUnit;
	CUnit* nuke = unit->building.silo.nuke;

	int nukesCount = (unit->building.silo.nuke == NULL) ? 0 : 1;
	std::vector<int*> events_panelcount_arg(3);

	events_panelcount_arg[0] = (int*)unit;
	events_panelcount_arg[1] = (int*)IconId::Nuclear_Missile;
	events_panelcount_arg[2] = &nukesCount;

	EventManager::EventCalled(EventId::STATUS_DISPLAYING_PANEL_COUNT, events_panelcount_arg);

	SC_sprintf_s(
		queues_buffers[index],
		8,
		FORMATSTRING_VALUE_SPACE,
		nukesCount
	);

	if (dialog->controlType != DialogControlTypes::DialogBox)
		current_dialog = dialog->parent->childrenDlg;
	else
		current_dialog = dialog->childrenDlg;

	while (current_dialog != NULL && current_dialog->index != index + 9)
		current_dialog = current_dialog->next;

	showDialog(current_dialog);
	disableDialog(current_dialog);

	u16 panelGraphic, iconId, itemId, tooltipTypeId;
	char* iconText;
	std::vector<int*> events_arg(6);

	panelGraphic = 12;
	iconId = IconId::Nuclear_Missile;
	tooltipTypeId = PanelTooltipTypes::SpecialPanelTooltip;
	itemId = UnitId::TerranNuclearMissile;
	iconText = queues_buffers[index];

	events_arg[0] = (int*)unit;
	events_arg[1] = (int*)&panelGraphic;
	events_arg[2] = (int*)&iconId;
	events_arg[3] = (int*)&tooltipTypeId;
	events_arg[4] = (int*)&itemId;
	events_arg[5] = (int*)&iconText;

	EventManager::EventCalled(EventId::STATUS_DISPLAYING_PANEL, events_arg);

	current_dialog->graphic = panelGraphic;
	current_dialog->statUser->unkUser_00 = *u32_0068C1E0;
	current_dialog->statUser->iconId_04 = iconId;
	current_dialog->statUser->tooltipType_06 = tooltipTypeId;
	current_dialog->statUser->id_08 = itemId;

	if(iconText != NULL)
		current_dialog->pszText = iconText;


	if (!(current_dialog->flags & BinDlgFlags::Unknown0)) {
		current_dialog->flags |= BinDlgFlags::Unknown0;
		updateDialog(current_dialog);
	}

}
#else
void StatsNukesCount(BinDlg* dialog, u32 index) {

	static char**	queues_buffers	= (char**)0x00519F40;
	static u32*		u32_0068C1E0	= (u32*)  0x0068C1E0; //unknown, maybe not always store the same thing

	BinDlg* current_dialog;
	CUnit* unit = *activePortraitUnit;
	CUnit* nuke = unit->building.silo.nuke;

	SC_sprintf_s(
		queues_buffers[index],
		8,
		FORMATSTRING_VALUE_SPACE,
		(unit->building.silo.nuke == NULL) ? 0 : 1
	);

	if (dialog->controlType != DialogControlTypes::DialogBox)
		current_dialog = dialog->parent->childrenDlg;
	else
		current_dialog = dialog->childrenDlg;

	while (current_dialog != NULL && current_dialog->index != index + 9)
		current_dialog = current_dialog->next;

	showDialog(current_dialog);
	disableDialog(current_dialog);

	current_dialog->graphic = 12;
	current_dialog->statUser->unkUser_00 = *u32_0068C1E0;
	current_dialog->statUser->iconId_04 = IconId::Nuclear_Missile;
	current_dialog->statUser->tooltipType_06 = PanelTooltipTypes::SpecialPanelTooltip;
	current_dialog->statUser->id_08 = UnitId::TerranNuclearMissile;

	current_dialog->pszText = queues_buffers[index];


	if (!(current_dialog->flags & BinDlgFlags::Unknown0)) {
		current_dialog->flags |= BinDlgFlags::Unknown0;
		updateDialog(current_dialog);
	}

}
#endif

;

#ifdef EVENTS_SYSTEM
void StatHangerCount(BinDlg* dialog, u32 index) {

	static char**	queues_buffers	= (char**)0x00519F40;
	static u32*		u32_0068C1E0	= (u32*)  0x0068C1E0; //unknown, maybe not always store the same thing

	BinDlg* current_dialog;
	CUnit* unit = *activePortraitUnit;
	int hangarCount = 0;

	if (unit->id == UnitId::ProtossReaver || unit->id == UnitId::Hero_Warbringer)
		hangarCount = unit->carrier.inHangarCount;
	else
	if (unit->id == UnitId::ProtossCarrier || unit->id == UnitId::Hero_Gantrithor)
		hangarCount = unit->carrier.outHangarCount + unit->carrier.inHangarCount;

	std::vector<int*> events_panelcount_arg(3);
	u16 iconId, itemId;

	if (unit->id == UnitId::ProtossReaver || unit->id == UnitId::Hero_Warbringer) {
		iconId = IconId::Scarab;
		itemId = UnitId::ProtossScarab;
	}
	else
	if (unit->id == UnitId::ProtossCarrier || unit->id == UnitId::Hero_Gantrithor) {
		iconId = IconId::Interceptor;
		itemId = UnitId::ProtossInterceptor;
	}
	else {
		iconId = IconId::Marine;
		itemId = UnitId::TerranMarine;
	}

	events_panelcount_arg[0] = (int*)unit;
	events_panelcount_arg[1] = (int*)iconId;
	events_panelcount_arg[2] = &hangarCount;

	EventManager::EventCalled(EventId::STATUS_DISPLAYING_PANEL_COUNT, events_panelcount_arg);

	SC_sprintf_s(
		queues_buffers[index],
		8,
		FORMATSTRING_VALUE_SPACE,
		hangarCount
	);

	if (dialog->controlType != DialogControlTypes::DialogBox)
		current_dialog = dialog->parent->childrenDlg;
	else
		current_dialog = dialog->childrenDlg;

	while (current_dialog != NULL && current_dialog->index != index + 9)
		current_dialog = current_dialog->next;

	showDialog(current_dialog);
	disableDialog(current_dialog);

	u16 panelGraphic, tooltipTypeId;
	char* iconText;
	std::vector<int*> events_arg(6);

	panelGraphic = 12;
	tooltipTypeId = PanelTooltipTypes::SpecialPanelTooltip;
	iconText = queues_buffers[index];

	events_arg[0] = (int*)unit;
	events_arg[1] = (int*)&panelGraphic;
	events_arg[2] = (int*)&iconId;
	events_arg[3] = (int*)&tooltipTypeId;
	events_arg[4] = (int*)&itemId;
	events_arg[5] = (int*)&iconText;

	EventManager::EventCalled(EventId::STATUS_DISPLAYING_PANEL, events_arg);

	current_dialog->graphic = panelGraphic;
	current_dialog->statUser->unkUser_00 = *u32_0068C1E0;
	current_dialog->statUser->iconId_04 = iconId;
	current_dialog->statUser->tooltipType_06 = tooltipTypeId;
	current_dialog->statUser->id_08 = itemId;

	if(iconText != NULL)
		current_dialog->pszText = iconText;

	if (!(current_dialog->flags & BinDlgFlags::Unknown0)) {
		current_dialog->flags |= BinDlgFlags::Unknown0;
		updateDialog(current_dialog);
	}

}
#else
void StatHangerCount(BinDlg* dialog, u32 index) {

	static char**	queues_buffers	= (char**)0x00519F40;
	static u32*		u32_0068C1E0	= (u32*)  0x0068C1E0; //unknown, maybe not always store the same thing

	BinDlg* current_dialog;
	CUnit* unit = *activePortraitUnit;
	int hangarCount = 0;

	if (unit->id == UnitId::ProtossReaver || unit->id == UnitId::Hero_Warbringer)
		hangarCount = unit->carrier.inHangarCount;
	else
	if (unit->id == UnitId::ProtossCarrier || unit->id == UnitId::Hero_Gantrithor)
		hangarCount = unit->carrier.outHangarCount + unit->carrier.inHangarCount;

	SC_sprintf_s(
		queues_buffers[index],
		8,
		FORMATSTRING_VALUE_SPACE,
		hangarCount
	);

	if (dialog->controlType != DialogControlTypes::DialogBox)
		current_dialog = dialog->parent->childrenDlg;
	else
		current_dialog = dialog->childrenDlg;

	while (current_dialog != NULL && current_dialog->index != index + 9)
		current_dialog = current_dialog->next;

	showDialog(current_dialog);
	disableDialog(current_dialog);

	current_dialog->graphic = 12;
	current_dialog->statUser->unkUser_00 = *u32_0068C1E0;
	current_dialog->statUser->tooltipType_06 = PanelTooltipTypes::SpecialPanelTooltip;

	if (unit->id == UnitId::ProtossReaver || unit->id == UnitId::Hero_Warbringer) {
		current_dialog->statUser->iconId_04 = IconId::Scarab;
		current_dialog->statUser->id_08 = UnitId::ProtossScarab;
	}
	else
	if (unit->id == UnitId::ProtossCarrier || unit->id == UnitId::Hero_Gantrithor) {
		current_dialog->statUser->iconId_04 = IconId::Interceptor;
		current_dialog->statUser->id_08 = UnitId::ProtossInterceptor;
	}
	else {
		current_dialog->statUser->iconId_04 = IconId::Marine;
		current_dialog->statUser->id_08 = UnitId::TerranMarine;
	}

	current_dialog->pszText = queues_buffers[index];

	if (!(current_dialog->flags & BinDlgFlags::Unknown0)) {
		current_dialog->flags |= BinDlgFlags::Unknown0;
		updateDialog(current_dialog);
	}

}
#endif

;

#ifdef EVENTS_SYSTEM
void StatsShieldLevel(BinDlg* dialog, u32 index) {

	static char**	queues_buffers					= (char**)	0x00519F40;
	static u32*		u32_0068C1E0					= (u32*)	0x0068C1E0; //unknown, maybe not always store the same thing
	static u16*		selectionVariableData3Offset	= (u16*)	0x006CA940;

	BinDlg* current_dialog;
	CUnit* unit = *activePortraitUnit;
	int shieldLevel = UpgradesSc->currentLevel[unit->playerId][UpgradeId::ProtossPlasmaShields];

	std::vector<int*> events_panelcount_arg(3);

	events_panelcount_arg[0] = (int*)unit;
	events_panelcount_arg[1] = (int*)IconId::Plasma_Shields;
	events_panelcount_arg[2] = &shieldLevel;

	EventManager::EventCalled(EventId::STATUS_DISPLAYING_PANEL_COUNT, events_panelcount_arg);

	SC_sprintf_s(
		queues_buffers[index],
		8,
		FORMATSTRING_VALUE_SPACE,
		shieldLevel
	);

	if (dialog->controlType != DialogControlTypes::DialogBox)
		current_dialog = dialog->parent->childrenDlg;
	else
		current_dialog = dialog->childrenDlg;

	while (current_dialog != NULL && current_dialog->index != index + 9)
		current_dialog = current_dialog->next;

	showDialog(current_dialog);
	disableDialog(current_dialog);

	u16 panelGraphic, iconId, itemId, tooltipTypeId;
	char* iconText;
	std::vector<int*> events_arg(6);

	panelGraphic = 12;
	iconId = IconId::Plasma_Shields;
	tooltipTypeId = PanelTooltipTypes::ShieldPanelTooltip;
	itemId = UpgradeId::ProtossPlasmaShields;
	iconText = queues_buffers[index];

	events_arg[0] = (int*)unit;
	events_arg[1] = (int*)&panelGraphic;
	events_arg[2] = (int*)&iconId;
	events_arg[3] = (int*)&tooltipTypeId;
	events_arg[4] = (int*)&itemId;
	events_arg[5] = (int*)&iconText;

	EventManager::EventCalled(EventId::STATUS_DISPLAYING_PANEL, events_arg);

	current_dialog->graphic = panelGraphic;
	current_dialog->statUser->unkUser_00 = *u32_0068C1E0;
	current_dialog->statUser->iconId_04 = iconId;
	current_dialog->statUser->tooltipType_06 = tooltipTypeId;
	current_dialog->statUser->id_08 = itemId;

	if(iconText != NULL)
		current_dialog->pszText = iconText;

	if (!(current_dialog->flags & BinDlgFlags::Unknown0)) {
		current_dialog->flags |= BinDlgFlags::Unknown0;
		updateDialog(current_dialog);
	}
	
	*selectionVariableData3Offset += shieldLevel;

}
#else
void StatsShieldLevel(BinDlg* dialog, u32 index) {

	static char**	queues_buffers					= (char**)	0x00519F40;
	static u32*		u32_0068C1E0					= (u32*)	0x0068C1E0; //unknown, maybe not always store the same thing
	static u16*		selectionVariableData3Offset	= (u16*)	0x006CA940;

	BinDlg* current_dialog;
	CUnit* unit = *activePortraitUnit;
	int shieldLevel = UpgradesSc->currentLevel[unit->playerId][UpgradeId::ProtossPlasmaShields];

	SC_sprintf_s(
		queues_buffers[index],
		8,
		FORMATSTRING_VALUE_SPACE,
		shieldLevel
	);

	if (dialog->controlType != DialogControlTypes::DialogBox)
		current_dialog = dialog->parent->childrenDlg;
	else
		current_dialog = dialog->childrenDlg;

	while (current_dialog != NULL && current_dialog->index != index + 9)
		current_dialog = current_dialog->next;

	showDialog(current_dialog);
	disableDialog(current_dialog);

	current_dialog->graphic = 12;
	current_dialog->statUser->unkUser_00 = *u32_0068C1E0;
	current_dialog->statUser->iconId_04 = IconId::Plasma_Shields;
	current_dialog->statUser->tooltipType_06 = PanelTooltipTypes::ShieldPanelTooltip;
	current_dialog->statUser->id_08 = UpgradeId::ProtossPlasmaShields;

	current_dialog->pszText = queues_buffers[index];

	if (!(current_dialog->flags & BinDlgFlags::Unknown0)) {
		current_dialog->flags |= BinDlgFlags::Unknown0;
		updateDialog(current_dialog);
	}
	
	*selectionVariableData3Offset += shieldLevel;

}
#endif

;

#ifdef EVENTS_SYSTEM
void StatsArmorLevel(BinDlg* dialog, u32 index) {

	static char**	queues_buffers					= (char**)	0x00519F40;
	static u32*		u32_0068C1E0					= (u32*)	0x0068C1E0; //unknown, maybe not always store the same thing
	static u16*		selectionVariableData3Offset	= (u16*)	0x006CA940;

	BinDlg* current_dialog;
	CUnit* unit = *activePortraitUnit;
	int armorCount = unit->getArmorBonus();
	u16 unitId;

	std::vector<int*> events_panelcount_arg(3);
	u16 iconId;

	//Note: original code don't call getLastQueueSlotType()
	if (unit->status & UnitStatus::Completed)
		unitId = unit->id;
	else {

		unitId = unit->buildQueue[unit->buildQueueSlot];

		if(
			unitId != UnitId::ZergHive &&
			unitId != UnitId::ZergLair &&
			unitId != UnitId::ZergGreaterSpire &&
			unitId != UnitId::ZergSporeColony &&
			unitId != UnitId::ZergSunkenColony
		)
			unitId = unit->id;

	}

	iconId = upgrades_dat::IconId[units_dat::ArmorUpgrade[unitId]];

	events_panelcount_arg[0] = (int*)unit;
	events_panelcount_arg[1] = (int*)iconId;
	events_panelcount_arg[2] = &armorCount;

	EventManager::EventCalled(EventId::STATUS_DISPLAYING_PANEL_COUNT, events_panelcount_arg);

	SC_sprintf_s(
		queues_buffers[index],
		8,
		FORMATSTRING_VALUE_SPACE,
		armorCount
	);

	if (dialog->controlType != DialogControlTypes::DialogBox)
		current_dialog = dialog->parent->childrenDlg;
	else
		current_dialog = dialog->childrenDlg;

	while (current_dialog != NULL && current_dialog->index != index + 9)
		current_dialog = current_dialog->next;

	showDialog(current_dialog);
	disableDialog(current_dialog);

	u16 panelGraphic, itemId, tooltipTypeId;
	char* iconText;
	std::vector<int*> events_arg(6);

	panelGraphic = 12;
	tooltipTypeId = PanelTooltipTypes::ArmorPanelTooltip;
	iconText = queues_buffers[index];

	itemId = units_dat::ArmorUpgrade[unitId];

	events_arg[0] = (int*)unit;
	events_arg[1] = (int*)&panelGraphic;
	events_arg[2] = (int*)&iconId;
	events_arg[3] = (int*)&tooltipTypeId;
	events_arg[4] = (int*)&itemId;
	events_arg[5] = (int*)&iconText;

	EventManager::EventCalled(EventId::STATUS_DISPLAYING_PANEL, events_arg);

	current_dialog->graphic = panelGraphic;
	current_dialog->statUser->unkUser_00 = *u32_0068C1E0;
	current_dialog->statUser->iconId_04 = iconId;
	current_dialog->statUser->tooltipType_06 = tooltipTypeId;
	current_dialog->statUser->id_08 = itemId;

	if(iconText != NULL)
		current_dialog->pszText = iconText;

	if (!(current_dialog->flags & BinDlgFlags::Unknown0)) {
		current_dialog->flags |= BinDlgFlags::Unknown0;
		updateDialog(current_dialog);
	}
	
	*selectionVariableData3Offset += armorCount;

}
#else
void StatsArmorLevel(BinDlg* dialog, u32 index) {

	static char**	queues_buffers					= (char**)	0x00519F40;
	static u32*		u32_0068C1E0					= (u32*)	0x0068C1E0; //unknown, maybe not always store the same thing
	static u16*		selectionVariableData3Offset	= (u16*)	0x006CA940;

	BinDlg* current_dialog;
	CUnit* unit = *activePortraitUnit;
	int armorCount = unit->getArmorBonus();
	u16 unitId;

	SC_sprintf_s(
		queues_buffers[index],
		8,
		FORMATSTRING_VALUE_SPACE,
		armorCount
	);

	if (dialog->controlType != DialogControlTypes::DialogBox)
		current_dialog = dialog->parent->childrenDlg;
	else
		current_dialog = dialog->childrenDlg;

	while (current_dialog != NULL && current_dialog->index != index + 9)
		current_dialog = current_dialog->next;

	showDialog(current_dialog);
	disableDialog(current_dialog);

	current_dialog->graphic = 12;
	current_dialog->statUser->unkUser_00 = *u32_0068C1E0;
	current_dialog->statUser->tooltipType_06 = PanelTooltipTypes::ArmorPanelTooltip;
	
	//Note: original code don't call getLastQueueSlotType()
	if (unit->status & UnitStatus::Completed)
		unitId = unit->id;
	else {

		unitId = unit->buildQueue[unit->buildQueueSlot];

		if(
			unitId != UnitId::ZergHive &&
			unitId != UnitId::ZergLair &&
			unitId != UnitId::ZergGreaterSpire &&
			unitId != UnitId::ZergSporeColony &&
			unitId != UnitId::ZergSunkenColony
		)
			unitId = unit->id;

	}

	current_dialog->statUser->iconId_04 = upgrades_dat::IconId[units_dat::ArmorUpgrade[unitId]];
	current_dialog->statUser->id_08 = units_dat::ArmorUpgrade[unitId];

	current_dialog->pszText = queues_buffers[index];

	if (!(current_dialog->flags & BinDlgFlags::Unknown0)) {
		current_dialog->flags |= BinDlgFlags::Unknown0;
		updateDialog(current_dialog);
	}
	
	*selectionVariableData3Offset += armorCount;

}
#endif

;

#ifdef EVENTS_SYSTEM
void StatsWeaponLevel(BinDlg* dialog, u32 index, u32 weaponId) {

	static char**	queues_buffers					= (char**)	0x00519F40;
	static u32*		u32_0068C1E0					= (u32*)	0x0068C1E0; //unknown, maybe not always store the same thing
	static u16*		selectionVariableData3Offset	= (u16*)	0x006CA940;

	BinDlg* current_dialog;
	CUnit* unit = *activePortraitUnit;
	u8 weaponUpgradeId = weapons_dat::DamageUpgrade[weaponId];
	int weaponLevel;

	if (weaponUpgradeId >= UpgradeId::UnusedUpgrade46)
		weaponLevel = UpgradesBw->currentLevel[unit->playerId][weaponUpgradeId - UpgradeId::UnusedUpgrade46];
	else
		weaponLevel = UpgradesSc->currentLevel[unit->playerId][weaponUpgradeId];

	std::vector<int*> events_panelcount_arg(3);
	u16 iconId;

	iconId = weapons_dat::WeaponIconId[weaponId];

	events_panelcount_arg[0] = (int*)unit;
	events_panelcount_arg[1] = (int*)iconId;
	events_panelcount_arg[2] = &weaponLevel;

	EventManager::EventCalled(EventId::STATUS_DISPLAYING_PANEL_COUNT, events_panelcount_arg);

	SC_sprintf_s(
		queues_buffers[index],
		8,
		FORMATSTRING_VALUE_SPACE,
		weaponLevel
	);

	if (dialog->controlType != DialogControlTypes::DialogBox)
		current_dialog = dialog->parent->childrenDlg;
	else
		current_dialog = dialog->childrenDlg;

	while (current_dialog != NULL && current_dialog->index != index + 9)
		current_dialog = current_dialog->next;

	showDialog(current_dialog);
	disableDialog(current_dialog);

	u16 panelGraphic, itemId, tooltipTypeId;
	char* iconText;
	std::vector<int*> events_arg(6);

	panelGraphic = 12;
	tooltipTypeId = PanelTooltipTypes::WeaponPanelTooltip;
	itemId = weaponId;
	iconText = queues_buffers[index];

	events_arg[0] = (int*)unit;
	events_arg[1] = (int*)&panelGraphic;
	events_arg[2] = (int*)&iconId;
	events_arg[3] = (int*)&tooltipTypeId;
	events_arg[4] = (int*)&itemId;
	events_arg[5] = (int*)&iconText;

	EventManager::EventCalled(EventId::STATUS_DISPLAYING_PANEL, events_arg);

	current_dialog->graphic = panelGraphic;
	current_dialog->statUser->unkUser_00 = *u32_0068C1E0;
	current_dialog->statUser->iconId_04 = iconId;
	current_dialog->statUser->tooltipType_06 = tooltipTypeId;
	current_dialog->statUser->id_08 = itemId;

	if(iconText != NULL)
		current_dialog->pszText = iconText;

	if (!(current_dialog->flags & BinDlgFlags::Unknown0)) {
		current_dialog->flags |= BinDlgFlags::Unknown0;
		updateDialog(current_dialog);
	}
	
	*selectionVariableData3Offset += weaponLevel;

}
#else
void StatsWeaponLevel(BinDlg* dialog, u32 index, u32 weaponId) {

	static char**	queues_buffers					= (char**)	0x00519F40;
	static u32*		u32_0068C1E0					= (u32*)	0x0068C1E0; //unknown, maybe not always store the same thing
	static u16*		selectionVariableData3Offset	= (u16*)	0x006CA940;

	BinDlg* current_dialog;
	CUnit* unit = *activePortraitUnit;
	u8 weaponUpgradeId = weapons_dat::DamageUpgrade[weaponId];
	int weaponLevel;

	if (weaponUpgradeId >= UpgradeId::UnusedUpgrade46)
		weaponLevel = UpgradesBw->currentLevel[unit->playerId][weaponUpgradeId - UpgradeId::UnusedUpgrade46];
	else
		weaponLevel = UpgradesSc->currentLevel[unit->playerId][weaponUpgradeId];

	SC_sprintf_s(
		queues_buffers[index],
		8,
		FORMATSTRING_VALUE_SPACE,
		weaponLevel
	);

	if (dialog->controlType != DialogControlTypes::DialogBox)
		current_dialog = dialog->parent->childrenDlg;
	else
		current_dialog = dialog->childrenDlg;

	while (current_dialog != NULL && current_dialog->index != index + 9)
		current_dialog = current_dialog->next;

	showDialog(current_dialog);
	disableDialog(current_dialog);

	current_dialog->graphic = 12;
	current_dialog->statUser->unkUser_00 = *u32_0068C1E0;
	current_dialog->statUser->iconId_04 = weapons_dat::WeaponIconId[weaponId];
	current_dialog->statUser->tooltipType_06 =  PanelTooltipTypes::WeaponPanelTooltip;
	current_dialog->statUser->id_08 = weaponId;

	current_dialog->pszText = queues_buffers[index];

	if (!(current_dialog->flags & BinDlgFlags::Unknown0)) {
		current_dialog->flags |= BinDlgFlags::Unknown0;
		updateDialog(current_dialog);
	}
	
	*selectionVariableData3Offset += weaponLevel;

}
#endif

;

#ifdef EVENTS_SYSTEMX
Bool32 StatSpidermineCount(BinDlg* dialog, u32 index) {

	CUnit* unit = *activePortraitUnit;

	if (unit->canUseTech(TechId::SpiderMines, unit->playerId)) {

		static char**	queues_buffers					= (char**)	0x00519F40;
		static u32*		u32_0068C1E0					= (u32*)	0x0068C1E0; //unknown, maybe not always store the same thing

		BinDlg* current_dialog;
		int minesCount = unit->vulture.spiderMineCount;

		std::vector<int*> events_panelcount_arg(3);

		events_panelcount_arg[0] = (int*)unit;
		events_panelcount_arg[1] = (int*)IconId::Vulture_Spider_Mine;
		events_panelcount_arg[2] = &minesCount;

		EventManager::EventCalled(EventId::STATUS_DISPLAYING_PANEL_COUNT, events_panelcount_arg);

		SC_sprintf_s(
			queues_buffers[index],
			8,
			FORMATSTRING_VALUE_SPACE,
			minesCount
		);

		if (dialog->controlType != DialogControlTypes::DialogBox)
			current_dialog = dialog->parent->childrenDlg;
		else
			current_dialog = dialog->childrenDlg;

		while (current_dialog != NULL && current_dialog->index != index + 9)
			current_dialog = current_dialog->next;

		showDialog(current_dialog);
		disableDialog(current_dialog);

		u16 panelGraphic, iconId, itemId, tooltipTypeId;
		char* iconText;
		std::vector<int*> events_arg(6);

		panelGraphic = 12;
		iconId = IconId::Vulture_Spider_Mine;
		tooltipTypeId =  PanelTooltipTypes::SpecialPanelTooltip;
		itemId = UnitId::TerranVultureSpiderMine;
		iconText = queues_buffers[index];

		events_arg[0] = (int*)unit;
		events_arg[1] = (int*)&panelGraphic;
		events_arg[2] = (int*)&iconId;
		events_arg[3] = (int*)&tooltipTypeId;
		events_arg[4] = (int*)&itemId;
		events_arg[5] = (int*)&iconText;

		EventManager::EventCalled(EventId::STATUS_DISPLAYING_PANEL, events_arg);

		current_dialog->graphic = panelGraphic;
		current_dialog->statUser->unkUser_00 = *u32_0068C1E0;
		current_dialog->statUser->iconId_04 = iconId;
		current_dialog->statUser->tooltipType_06 = tooltipTypeId;
		current_dialog->statUser->id_08 = itemId;

		if(iconText != NULL)
			current_dialog->pszText = iconText;

		if (!(current_dialog->flags & BinDlgFlags::Unknown0)) {
			current_dialog->flags |= BinDlgFlags::Unknown0;
			updateDialog(current_dialog);
		}

		return 1;

	}
	else
		return 0;

}
#else
Bool32 StatSpidermineCount(BinDlg* dialog, u32 index) {

	CUnit* unit = *activePortraitUnit;

	if (unit->canUseTech(TechId::SpiderMines, unit->playerId)) {

		static char**	queues_buffers					= (char**)	0x00519F40;
		static u32*		u32_0068C1E0					= (u32*)	0x0068C1E0; //unknown, maybe not always store the same thing

		BinDlg* current_dialog;
		int minesCount = unit->vulture.spiderMineCount;

		SC_sprintf_s(
			queues_buffers[index],
			8,
			FORMATSTRING_VALUE_SPACE,
			minesCount
		);

		if (dialog->controlType != DialogControlTypes::DialogBox)
			current_dialog = dialog->parent->childrenDlg;
		else
			current_dialog = dialog->childrenDlg;

		while (current_dialog != NULL && current_dialog->index != index + 9)
			current_dialog = current_dialog->next;

		showDialog(current_dialog);
		disableDialog(current_dialog);

		current_dialog->graphic = 12;
		current_dialog->statUser->unkUser_00 = *u32_0068C1E0;
		current_dialog->statUser->iconId_04 = IconId::Vulture_Spider_Mine;
		current_dialog->statUser->tooltipType_06 = PanelTooltipTypes::SpecialPanelTooltip;
		current_dialog->statUser->id_08 = UnitId::TerranVultureSpiderMine;

		current_dialog->pszText = queues_buffers[index];

		if (!(current_dialog->flags & BinDlgFlags::Unknown0)) {
			current_dialog->flags |= BinDlgFlags::Unknown0;
			updateDialog(current_dialog);
		}

		return 1;

	}
	else
		return 0;

}
#endif

;

} //namespace hooks

//-------- Helper function definitions. Do NOT modify! --------//

namespace {

const u32 Func_DisableDialog = 0x00418640;
void disableDialog(BinDlg* dialog) {

	__asm {
		PUSHAD
		MOV ESI, dialog
		CALL Func_DisableDialog
		POPAD
	}

}

;

const u32 Func_ShowDialog = 0x004186A0;
void showDialog(BinDlg* dialog) {

	__asm {
		PUSHAD
		MOV ESI, dialog
		CALL Func_ShowDialog
		POPAD
	}

}

;

const u32 Func_UpdateDialog	= 0x0041C400;
void updateDialog(BinDlg* dialog) {
	__asm {
		PUSHAD
		MOV EAX, dialog
		CALL Func_UpdateDialog
		POPAD
	}
}

;

const u32 Func_sprintf_s = 0x0041F1B0;
//Note: buffer_size may be max number of characters * 2, possibly for 16bits text support
void SC_sprintf_s(char* buffer, int buffer_size, const char* format_string, int value) {

	__asm {

		PUSHAD

		PUSH value
		PUSH format_string
		MOV ESI, buffer_size
		MOV EDI, buffer

		CALL Func_sprintf_s
		ADD ESP, 0x08

		POPAD

	}

}

;

} //unnamed namespace

//End of helper functions

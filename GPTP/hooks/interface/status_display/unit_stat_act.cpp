#include "unit_stat_act.h"
#include <SCBW/api.h>

//Helper functions declaration

namespace {

bool isQueueSlotActive(CUnit* unit, u32 queueSlot);						//01E70
BinDlg* getControlFromIndex(BinDlg* dialog, int index);					//18080
void hideDialog(BinDlg* dialog);										//18700
void updateDialog(BinDlg* dialog);										//1C400
void function_004248F0();												//248F0
bool function_00424A10_helper();										//24A10
void setSpellSpecialBtnGraphic(BinDlg* dialog);							//24BA0
void function_00424FC0();												//24FC0
void AddTextToDialog(BinDlg* dialog, int index, char* textOffset);		//258B0
void SetUnitStatusStrText(BinDlg* dialog);								//25B50
void SetKillsStrText(BinDlg* dialog);									//25DD0
void function_00425EC0(BinDlg* dialog);									//25EC0
void function_00425F30(BinDlg* dialog);									//25F30
void function_004260C0(BinDlg* dialog);									//260C0
void function_00426190(BinDlg* dialog);									//26190
void setTextStr(BinDlg* dialog);										//263E0
void function_00426500(BinDlg* dialog);									//26500
void function_004266F0(BinDlg* dialog);									//266F0
void function_004268D0(BinDlg* dialog);									//268D0
void stats_panel_display_Helper(BinDlg* dialog);						//26C60
void UnitStatAct_Standard_Helper(BinDlg* dialog);						//26F50
void function_00426FF0(BinDlg* dialog);									//26FF0
void function_00427540(BinDlg* dialog);									//27540
void UnitStatAct_Building_Helper(BinDlg* dialog);						//27890
void UnitStatAct_Dropship_Helper(BinDlg* dialog);						//27C90
void DLGsetProgressBarValue(BinDlg* dialog, int index, u32 progress);	//57200
void function_00457250(BinDlg* dialog, int index, u32 unk);				//57250
void function_00457310(BinDlg* dialog);									//57310
u32 function_004669E0(CUnit* unit);										//669E0
bool unit_isGeyserUnit(CUnit* unit);									//688B0
u16 getLastQueueSlotType(CUnit* unit);									//7B270
char* getUnitName(CUnit* unit);											//7B5A0
bool isAttemptingProtossBuild(CUnit* unit);								//E4C40
bool isConstructingAddon(CUnit* unit);									//E66B0
bool unitIsActiveTransport(CUnit* unit);								//E6BA0

} //unnamed namespace

u32* const u32_0068C1E0				= (u32*)	0x0068C1E0; //unknown, maybe not always store the same thing
u8* const u8_0068C1E5				= (u8*)		0x0068C1E5; //Some kind of ID of current type of display, to not redo memory allocation
s32* const selectionHPvalues		= (s32*)	0x006CA94C;	//array of SELECTION_ARRAY_LENGTH elements
s32* const selectionShieldvalue		= (s32*)	0x006CA9EC; //pointer on a single element (not array)
Bool8* const selectionUpdateNeeded	= (Bool8*)	0x006CA9F0;
u16* const selectionEnergyvalue		= (u16*)	0x006CAC0C; //pointer on a single element

const int TEXTLABELINDEX_UNITNAME = -5;
const int TEXTLABELINDEX_UNITSTATUS = -20;
const int TEXTLABELINDEX_SUMMONINGTEXT = -30; //archon summon
const int TEXTLABELINDEX_CONSTRUCTINGTEXT = -40; //morphing,building,opening warp gate and others...

const int ARCHONPROGRESSBARINDEX = 13;
const int EGGPROGRESSBARINDEX = 16;
const int EGGBUILTUNITICONINDEX = 17;

namespace hooks {

//0x00425EE0
//Possibly used by units turrets, nukes, spells...
//Probably Display Function 9 in FireGraft
void UnitStatAct_Default(BinDlg* dialog) {

	BinDlg* current_dialog;

	*u8_0068C1E5 = 0;

	if(dialog->controlType != DialogControlTypes::DialogBox)
		current_dialog = dialog;
	else
		current_dialog = dialog->childrenDlg;

	while(current_dialog != NULL) {
		hideDialog(current_dialog);
		current_dialog = current_dialog->next;
	}

	AddTextToDialog(dialog,TEXTLABELINDEX_UNITNAME,getUnitName(*activePortraitUnit));

}

;

//0x00426EE0
//Possibly used by spider mines, powerups, Floor armed traps...
//Probably Display Function 8 in FireGraft
void UnitStatAct_Powerup(BinDlg* dialog) {

	if(*u8_0068C1E5 != 14) {

		BinDlg* current_dialog;

		if(dialog->controlType != DialogControlTypes::DialogBox)
			current_dialog = dialog;
		else
			current_dialog = dialog->childrenDlg;

		while(current_dialog != NULL) {
			hideDialog(current_dialog);
			current_dialog = current_dialog->next;
		}

		*u8_0068C1E5 = 14;

	}

	selectionHPvalues[0] = (*activePortraitUnit)->hitPoints;

	stats_panel_display_Helper(dialog);
	setTextStr(dialog);

	AddTextToDialog(dialog,TEXTLABELINDEX_UNITNAME,getUnitName(*activePortraitUnit));

}

;

//0x00426F50
//Used by most units, sometimes directly after some checks
//made by other functions (usually: unit not belonging to
//current player or not having its possible special property)
//Probably Display Function 1 in FireGraft
void UnitStatAct_Standard(BinDlg* dialog) {

	CUnit* unit = *activePortraitUnit;

	selectionHPvalues[0] = unit->hitPoints;
	*selectionEnergyvalue = unit->energy / 256;
	*selectionShieldvalue = unit->shields / 256;

	function_004248F0();

	if( ((u8)*(u16*)u8_0068C1E5) != 2 ) {
		function_00457310(dialog);
		function_00457250(dialog,TEXTLABELINDEX_UNITSTATUS,12);
		*u8_0068C1E5 = 2;
	}

	//unknown, may be related to one of the elements displayed
	//by stats_panel_display, but not always the same
	*(u16*)(0x006CA940) = 1;

	stats_panel_display_Helper(dialog);
	SetUnitStatusStrText(dialog);
	SetKillsStrText(dialog);
	setTextStr(dialog);
	AddTextToDialog(dialog,TEXTLABELINDEX_UNITNAME,getUnitName(unit));

}

;

//0x00427260
//Used by ZergEgg,ZergCocoon and ZergLurkerEgg.
//Probably Display Function 6 in FireGraft
void UnitStatAct_Egg(BinDlg* dialog) {

	CUnit* unit = *activePortraitUnit;

	if(
		(*IS_IN_REPLAY == 0 &&
		unit->playerId != *LOCAL_NATION_ID) ||
		unit->status & UnitStatus::IsHallucination
	)
		UnitStatAct_Standard_Helper(dialog);
	else {

		BinDlg* current_dialog;
		u8 raceId;
		u16 builtUnitId = unit->buildQueue[unit->buildQueueSlot]; //done earlier than normal, but should not matter

		if(*u8_0068C1E5 != 10) {
			function_00457310(dialog);
			function_00457250(dialog,TEXTLABELINDEX_CONSTRUCTINGTEXT,17);
			*u8_0068C1E5 = 10;
		}

		DLGsetProgressBarValue(dialog,EGGPROGRESSBARINDEX,function_004669E0(unit));

		if(dialog->controlType == DialogControlTypes::DialogBox)
			current_dialog = dialog->childrenDlg;
		else
			current_dialog = dialog->parent->childrenDlg;

		while(current_dialog != NULL && current_dialog->index != EGGBUILTUNITICONINDEX)
			current_dialog = current_dialog->next;

		//original code don't handle the case of current_dialog being NULL
		//at this point, and even overwrite it with 0 in this case,
		//leading necessarily to a crash.Weird but probably mean current_dialog
		//will definitely never be NULL here.

		current_dialog->graphic = 2;

		current_dialog->statUser->unkUser_00 = *u32_0068C1E0;
		current_dialog->statUser->iconId_04 = builtUnitId;
		current_dialog->statUser->unknown_06 = 3;
		current_dialog->statUser->id_08 = builtUnitId;

		if(!(current_dialog->flags & BinDlgFlags::Unknown0)) {
			current_dialog->flags |= BinDlgFlags::Unknown0;
			updateDialog(dialog);
		}

		if(units_dat::GroupFlags[unit->id].isZerg)
			raceId = RaceId::Zerg;
		else
		if(units_dat::GroupFlags[unit->id].isProtoss)
			raceId = RaceId::Protoss;
		else
		if(units_dat::GroupFlags[unit->id].isTerran)
			raceId = RaceId::Terran;
		else
			raceId = RaceId::Neutral;

		//skipping hardcoded use of statTxtTbl

		//Zerg: "Morphing", Terran: "Building", Protoss: "Opening Warp Gate", Neutral: "Building"
		AddTextToDialog(dialog,TEXTLABELINDEX_CONSTRUCTINGTEXT,(char*)statTxtTbl->getString(0x303 + raceId));

		setTextStr(dialog);
		AddTextToDialog(dialog,TEXTLABELINDEX_UNITNAME,getUnitName(unit));

	}

}

;

//0x004273E0
//Used by ProtossDarkArchon and ProtossArchon (not heroic Archon)
//Probably Display Function 5 in FireGraft
void UnitStatAct_Archon(BinDlg* dialog) {

	CUnit* unit = *activePortraitUnit;

	if(
		(*IS_IN_REPLAY == 0 &&
		unit->playerId != *LOCAL_NATION_ID) ||
		unit->mainOrderId != OrderId::CompletingArchonSummon
	)
		UnitStatAct_Standard_Helper(dialog);
	else {

		function_00457250(dialog,TEXTLABELINDEX_SUMMONINGTEXT,13);

		if(*u8_0068C1E5 != 6) {
			function_00457310(dialog);
			*u8_0068C1E5 = 6;
		}

		DLGsetProgressBarValue(dialog,ARCHONPROGRESSBARINDEX,function_004669E0(unit));

		//skipping hardcoded use of statTxtTbl

		//Text: "Summoning"
		AddTextToDialog(dialog,TEXTLABELINDEX_SUMMONINGTEXT,(char*)statTxtTbl->getString(0x316));

		setTextStr(dialog);
		AddTextToDialog(dialog,TEXTLABELINDEX_UNITNAME,getUnitName(unit));

	}

}

;

//0x004274A0
//Used by Carrier,Reaver, and their heroic versions
//Probably Display Function 4 in FireGraft
void UnitStatAct_CarrierReaver(BinDlg* dialog) {

	CUnit* unit = *activePortraitUnit;

	if(
		*IS_IN_REPLAY == 0 &&
		unit->playerId != *LOCAL_NATION_ID
	)
		UnitStatAct_Standard_Helper(dialog);
	else {

		u16 builtUnitId;

		if(function_00424A10_helper())
			*selectionUpdateNeeded = 1;
		else
			*selectionUpdateNeeded = 0;

		selectionHPvalues[0] = unit->hitPoints;
		*selectionEnergyvalue = unit->energy / 256;
		*selectionShieldvalue = unit->shields / 256;
		function_004248F0();

		builtUnitId = unit->buildQueue[unit->buildQueueSlot % 5];

		if(builtUnitId > UnitId::Spell_DisruptionWeb) //builtUnitId is a building,UnitId::None or other
			UnitStatAct_Standard_Helper(dialog);
		else {
			function_004268D0(dialog);
			setTextStr(dialog);
			AddTextToDialog(dialog,TEXTLABELINDEX_UNITNAME,getUnitName(unit));
		}

	}

}

;

//0x00427890
//Used by buildings and maybe others...
//Probably Display Function 2 in FireGraft
void UnitStatAct_Building(BinDlg* dialog) {

	CUnit* unit = *activePortraitUnit;

	bool isResourceUnit;

	selectionHPvalues[0] = unit->hitPoints;
	*selectionEnergyvalue = unit->energy / 256;
	*selectionShieldvalue = unit->shields / 256;

	if(function_00424A10_helper())
		*selectionUpdateNeeded = 1;
	else
		*selectionUpdateNeeded = 0;

	function_004248F0();

	isResourceUnit =
		(
			unit->id >= UnitId::ResourceMineralField &&
			(unit->id <= UnitId::ResourceMineralFieldType3 ||
			unit->id == UnitId::ResourceVespeneGeyser)
		);

	if(!isResourceUnit) {

		if(
			*IS_IN_REPLAY == 0 &&
			unit->playerId != *LOCAL_NATION_ID
		)
		{
			if( ((u8)*(u16*)u8_0068C1E5) != 4 ) {
				function_00457310(dialog);
				*u8_0068C1E5 = 4;
			}
		}
		else
		if(!(unit->status & UnitStatus::Completed))
			function_004260C0(dialog);
		else
		if(unit->id == UnitId::TerranNuclearSilo)
			function_00426FF0(dialog);
		else
		if(unitIsActiveTransport(unit)) {
			if( ((u8)*(u16*)u8_0068C1E5) != 4 ) {
				function_00457310(dialog);
				*u8_0068C1E5 = 4;
			}
		}
		else
		if(isQueueSlotActive(unit,0))
			function_004268D0(dialog);
		else
		if(
			isConstructingAddon(unit) ||
			isAttemptingProtossBuild(unit)
		)
			function_00425F30(dialog);
		else
		if(unit->building.techType != TechId::None)
			function_004266F0(dialog);
		else
		if(unit->building.upgradeType != UpgradeId::None)
			function_00426500(dialog);
		else
		if(units_dat::SupplyProvided[unit->id] != 0)
			function_00427540(dialog);
		else
		if(unit_isGeyserUnit(unit))
			isResourceUnit = true;
		else
		if(
			units_dat::AirWeapon[getLastQueueSlotType(unit)] == WeaponId::None &&
			unit->getGroundWeapon() == WeaponId::None &&
			units_dat::ShieldsEnabled[unit->id] == 0
		)
		{
			if( ((u8)*(u16*)u8_0068C1E5) != 4 ) {
				function_00457310(dialog);
				*u8_0068C1E5 = 4;
			}
		}
		else {
			
			if( ((u8)*(u16*)(u8_0068C1E5)) != 3 ) {
				function_00457310(dialog);
				function_00457250(dialog,TEXTLABELINDEX_UNITSTATUS,12);
				*u8_0068C1E5 = 3;
			}

			stats_panel_display_Helper(dialog);
			hideDialog(getControlFromIndex(dialog,-21));

		}

	}

	if(isResourceUnit) //unit is mineral field or vespene geyser or geyser using building
		function_00426190(dialog);

	//below: basic display used in addition (or replacement) to all of the above
	setTextStr(dialog);
	SetUnitStatusStrText(dialog);
	AddTextToDialog(dialog,TEXTLABELINDEX_UNITNAME,getUnitName(unit));

}

;

//0x00427C90
//Used (directly) by TerranDropship,ProtossShuttle and TerranBunker
//Probably Display Function 3 in FireGraft
void UnitStatAct_Dropship(BinDlg* dialog) {

	CUnit* unit = *activePortraitUnit;

	if(
		(*IS_IN_REPLAY == 0 && unit->playerId != *LOCAL_NATION_ID) ||
		unit->status & UnitStatus::IsHallucination
	) 
	{
		if(unit->status & UnitStatus::GroundedBuilding)
			UnitStatAct_Building_Helper(dialog);
		else
			UnitStatAct_Standard_Helper(dialog);
	}
	else {

		function_00424FC0();

		if(
			!unit->hasLoadedUnit() ||
			(*IS_IN_REPLAY == 0 && unit->playerId != *LOCAL_NATION_ID)
		)
		{
			if(unit->status & UnitStatus::GroundedBuilding)
				UnitStatAct_Building_Helper(dialog);
			else
				UnitStatAct_Standard_Helper(dialog);
		}
		else {

			if( ((u8)*(u16*)u8_0068C1E5) != 11 ) {
				function_00457310(dialog);
				*u8_0068C1E5 = 11;
			}

			setSpellSpecialBtnGraphic(dialog);
			setTextStr(dialog);
			function_00425EC0(dialog);

		}

	}

}

;

//0x00427D30
//Used by Overlord/Yggdrasill
//Probably Display Function 7 in FireGraft
void UnitStatAct_Overlord(BinDlg* dialog) {

	CUnit* unit = *activePortraitUnit;

	if(
		(*IS_IN_REPLAY == 0 && unit->playerId != *LOCAL_NATION_ID) ||
		unit->status & UnitStatus::IsHallucination
	)
		UnitStatAct_Standard_Helper(dialog);
	else {

		if(unit->hasLoadedUnit())
			UnitStatAct_Dropship_Helper(dialog);
		else {
			function_00427540(dialog);
			setTextStr(dialog);
			SetUnitStatusStrText(dialog);
			AddTextToDialog(dialog,TEXTLABELINDEX_UNITNAME,getUnitName(unit));
		}

	}

}

;

} //namespace hooks

//-------- Helper function definitions. Do NOT modify! --------//

namespace {

//Logically equivalent to isQueueSlotActive @ 0x00401E70
//Spell_DisruptionWeb is last id before command center and
//other buildings
bool isQueueSlotActive(CUnit* unit, u32 queueSlot) {

	u32 queueSlotChecked = unit->buildQueue[(unit->buildQueueSlot + queueSlot) % 5];

	return (queueSlotChecked <= UnitId::Spell_DisruptionWeb);

}

;

const u32 Func_Sub457250 = 0x00457250;
//Can use mouse coordinates, dialog->fxnInteract and
//updateDialog.
//Used by UnitStatAct_Standard, UnitStatAct_Egg, 
//UnitStatAct_Archon and UnitStatAct_Building.
void function_00457250(BinDlg* dialog, int index, u32 unk) {
	__asm {
		PUSHAD
		MOV EAX, dialog
		MOV ECX, index
		PUSH unk
		CALL Func_Sub457250
		POPAD
	}
}

;

const u32 Func_getControlFromIndex = 0x00418080;
BinDlg* getControlFromIndex(BinDlg* dialog, int index) {

	static BinDlg* control;

	__asm {
		PUSHAD
		MOV ECX, index
		MOV EAX, dialog
		CALL Func_getControlFromIndex
		MOV control, EAX
		POPAD
	}

	return control;

}

;

const u32 Func_HideDialog = 0x00418700;
void hideDialog(BinDlg* dialog) {

	__asm {
		PUSHAD
		MOV ESI, dialog
		CALL Func_HideDialog
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

const u32 Func_Sub4248F0 = 0x004248F0;
//Update the content of selectionVariableData data for
//Cond functions, with scarabs/interceptors/remove timer
void function_004248F0() {
	__asm {
		PUSHAD
		CALL Func_Sub4248F0
		POPAD
	}
}

;

const u32 Func_Sub424A10 = 0x00424A10;
//Hooked in unit_stat_cond
bool function_00424A10_helper() {
	
	static Bool32 bPreResult;

	__asm {
		PUSHAD
		CALL Func_Sub424A10
		MOV bPreResult, EAX
		POPAD
	}

	return (bPreResult != 0);

}

;

const u32 Func_setSpellSpecialBtnGraphic = 0x00424BA0;
//probably display for transport with special property
//that each loaded unit is clickable (button)
void setSpellSpecialBtnGraphic(BinDlg* dialog) {
	__asm {
		PUSHAD
		MOV EAX, dialog
		CALL Func_setSpellSpecialBtnGraphic
		POPAD
	}
}

;

const u32 Func_Sub424FC0 = 0x00424FC0;
//Refresh local selection data values for transport
void function_00424FC0() {
	__asm {
		PUSHAD
		CALL Func_Sub424FC0
		POPAD
	}
}

;

const u32 Func_AddTextToDialog = 0x004258B0;
void AddTextToDialog(BinDlg* dialog, int index, char* textOffset) {

	__asm {
		PUSHAD
		PUSH textOffset
		MOV ECX, index
		MOV EAX, dialog
		CALL Func_AddTextToDialog
		POPAD
	}

}

;

const u32 Func_SetUnitStatusStrText = 0x00425B50;
void SetUnitStatusStrText(BinDlg* dialog)  {

	__asm {
		PUSHAD
		PUSH dialog
		CALL Func_SetUnitStatusStrText
		POPAD
	}

}
;

const u32 Func_SetKillsStrText = 0x00425DD0;
void SetKillsStrText(BinDlg* dialog) {

	__asm {
		PUSHAD
		MOV EBX, dialog
		CALL Func_SetKillsStrText
		POPAD
	}

}

;

const u32 Func_Sub425EC0 = 0x00425EC0;
//Display the unit name
void function_00425EC0(BinDlg* dialog) {
	__asm {
		PUSHAD
		PUSH dialog
		CALL Func_Sub425EC0
		POPAD
	}
}

;

const u32 Func_Sub425F30 = 0x00425F30;
//Constructing addon or protoss building from building
void function_00425F30(BinDlg* dialog) {
	__asm {
		PUSHAD
		MOV EDI, dialog
		CALL Func_Sub425F30
		POPAD
	}
}

;

const u32 Func_Sub4260C0 = 0x004260C0;
//Display for an incomplete building
void function_004260C0(BinDlg* dialog) {
	__asm {
		PUSHAD
		MOV EDI, dialog
		CALL Func_Sub4260C0
		POPAD
	}
}

;

const u32 Func_Sub426190 = 0x00426190;
//Display for a resource or extractor
void function_00426190(BinDlg* dialog) {
	__asm {
		PUSHAD
		MOV EAX, dialog
		CALL Func_Sub426190
		POPAD
	}
}

;

const u32 Func_setTextStr = 0x004263E0;
//Display HP, Shields, Energy
void setTextStr(BinDlg* dialog) {
	__asm {
		PUSHAD
		PUSH dialog
		CALL Func_setTextStr
		POPAD
	}
}

;

const u32 Func_Sub426500 = 0x00426500;
//Display for researching an upgrade
void function_00426500(BinDlg* dialog) {
	__asm {
		PUSHAD
		PUSH dialog
		CALL Func_Sub426500
		POPAD
	}
}

;

const u32 Func_Sub4266F0 = 0x004266F0;
//Display for researching a tech
void function_004266F0(BinDlg* dialog) {
	__asm {
		PUSHAD
		PUSH dialog
		CALL Func_Sub4266F0
		POPAD
	}
}

;

const u32 Func_Sub4268D0 = 0x004268D0;
//Display progress of constructing units with queue
void function_004268D0(BinDlg* dialog) {
	__asm {
		PUSHAD
		PUSH dialog
		CALL Func_Sub4268D0
		POPAD
	}
}

;

const u32 Func_Sub426C60 = 0x00426C60;
//Display icons with weapon, shield, armor...
void stats_panel_display_Helper(BinDlg* dialog) {

	__asm {
		PUSHAD
		PUSH dialog
		CALL Func_Sub426C60
		POPAD
	}

}

;

const u32 Func_Sub426FF0 = 0x00426FF0;
//Display for nuclear silo
void function_00426FF0(BinDlg* dialog) {
	__asm {
		PUSHAD
		PUSH dialog
		CALL Func_Sub426FF0
		POPAD
	}
}

;

const u32 Func_UnitStatAct_Standard = 0x00426F50;
void UnitStatAct_Standard_Helper(BinDlg* dialog) {
	__asm {
		PUSHAD
		MOV ECX, dialog
		CALL Func_UnitStatAct_Standard
		POPAD
	}
}

;

const u32 Func_Sub427540 = 0x00427540;
//Display for supply provider
void function_00427540(BinDlg* dialog) {
	__asm {
		PUSHAD
		MOV EAX, dialog
		CALL Func_Sub427540
		POPAD
	}
}

;

const u32 Func_UnitStatAct_Building = 0x00427890;
void UnitStatAct_Building_Helper(BinDlg* dialog) {
	__asm {
		PUSHAD
		MOV ECX, dialog
		CALL Func_UnitStatAct_Building
		POPAD
	}
}

;

const u32 Func_UnitStatAct_Dropship = 0x00427C90;
void UnitStatAct_Dropship_Helper(BinDlg* dialog) {
	__asm {
		PUSHAD
		MOV ECX, dialog
		CALL Func_UnitStatAct_Dropship
		POPAD
	}
}

;

const u32 Func_DLGsetProgressBarValue = 0x00457200;
void DLGsetProgressBarValue(BinDlg* dialog, int index, u32 progress) {

	__asm {
		PUSHAD
		PUSH progress
		MOV ECX, index
		MOV EAX, dialog
		CALL Func_DLGsetProgressBarValue
		POPAD
	}

}

;

const u32 Func_Sub457310 = 0x00457310;
//Hide some parts of the dialog
void function_00457310(BinDlg* dialog) {

	__asm {
		PUSHAD
		MOV EAX, dialog
		CALL Func_Sub457310
		POPAD
	}

}

;

//Identical to function @ 0x004688B0 
bool unit_isGeyserUnit(CUnit* unit) {

	bool return_value = false;

	if(
		unit->status & UnitStatus::Completed &&
		(
			unit->id == UnitId::TerranRefinery ||
			unit->id == UnitId::ProtossAssimilator ||
			unit->id == UnitId::ZergExtractor
		)
	)
		return_value = true;

	return return_value;

}

;

//Identical to function @ 0x004669E0
//Return the progress percentage of a
//building unit
//unit = ESI
u32 function_004669E0(CUnit* unit) {

	u32 return_value;
	u32 timeCost;
	u16 unitId = unit->id;

	if(
		unitId == UnitId::ZergEgg ||
		unitId == UnitId::ZergCocoon ||
		unitId == UnitId::ZergLurkerEgg ||
		unit->isRemorphingBuilding()
	)
		unitId = unit->buildQueue[unit->buildQueueSlot];

	timeCost = units_dat::TimeCost[unitId];

	return_value = (100 * (timeCost - unit->remainingBuildTime) ) / timeCost;

	return return_value;

}

;

//Identical to function at 0x0047B270
//EDX = unit
//If the unit is currently mutating into
//a level 2/3 zerg building, return the
//id of the building morphed into, else
//return the normal unit id.
u16 getLastQueueSlotType(CUnit* unit) {

	u16 unitId;

	if(unit->status & UnitStatus::Completed)
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

	return unitId;

}

;

//Identical to Unit::getName() @ 0047B5A0
char* getUnitName(CUnit* unit) {

	char* return_value;

	if(unit->status & UnitStatus::Completed)
		return_value = unit->getName();
	else {

		u16 builtUnitId = unit->buildQueue[unit->buildQueueSlot];

		if(
			builtUnitId == UnitId::ZergHive ||
			builtUnitId == UnitId::ZergLair ||
			builtUnitId == UnitId::ZergGreaterSpire ||
			builtUnitId == UnitId::ZergSporeColony ||
			builtUnitId == UnitId::ZergSunkenColony
		)
			return_value = CUnit::getName(builtUnitId);
		else
			return_value = unit->getName();

	}

	return return_value;

}

;

//Identical to isAttemptingProtossBuild @ 0x004E4C40
bool isAttemptingProtossBuild(CUnit* unit) {

	bool return_value = true;

	if(
		unit->mainOrderId != OrderId::BuildProtoss1 ||
		!(unit->status & UnitStatus::GroundedBuilding) ||
		unit->orderTarget.unit == NULL ||
		(unit->orderTarget.unit)->status & UnitStatus::Completed
	)
		return_value = false;

	return return_value;

}

;

//Identical to isConstructingAddon @ 0x004E66B0
bool isConstructingAddon(CUnit* unit) {

	bool return_value = true;

	if(
		unit->secondaryOrderId != OrderId::BuildAddon ||
		!(unit->status & UnitStatus::GroundedBuilding) ||
		unit->currentBuildUnit == NULL ||
		unit->currentBuildUnit->status & UnitStatus::Completed
	)
		return_value = false;

	return return_value;

}

;

//Equivalent to code unitIsActiveTransport @ 0x004E6BA0
bool unitIsActiveTransport(CUnit* unit) {

	bool result = false;

	if(!(unit->status & UnitStatus::IsHallucination)) {

		if(
			unit->id != UnitId::ZergOverlord ||
			UpgradesSc->currentLevel[unit->playerId][UpgradeId::VentralSacs] != 0
		)
		{
			if(units_dat::SpaceProvided[unit->id] != 0)
				result = true;
		}


	}

	return result;

}

;

} //unnamed namespace

//End of helper functions
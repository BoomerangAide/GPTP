#include "stats_panel_display.h"
#include <SCBW/api.h>
#include <Events/Events.h>

//Helper functions declaration

namespace {

void hideDialog(BinDlg* dialog);										//18700
void StatsNukesCount_Helper(BinDlg* dialog, u32 index);					//25310
void StatHangerCount_Helper(BinDlg* dialog, u32 index);					//253D0
void StatsShieldLevel_Helper(BinDlg* dialog, u32 index);				//25510
void StatsArmorLevel_Helper(BinDlg* dialog, u32 index);					//25600
void StatsWeaponLevel_Helper(BinDlg* dialog, u32 index, u32 weaponId);	//25790
bool StatSpidermineCount_Helper(BinDlg* dialog, u32 index);				//26300
u16 getLastQueueSlotType(CUnit* unit);									//7B270

} //unnamed namespace

namespace hooks {

//Renamed from sub_426C60
//Attempting to be true to the original result
//in badly unoptimized code
#ifdef EVENTS_SYSTEM
void stats_panel_display(BinDlg* dialog) {

	BinDlg* current_dialog;
	CUnit* activeUnit = *activePortraitUnit;
	u32 index = 0;

	if(
		!(activeUnit->status & UnitStatus::IsHallucination) ||	//not hallucination, display stuff
		(
			*IS_IN_REPLAY == 0 &&						//not in replay, which hide stuff as if you were the hallucination owner
			activeUnit->playerId != *LOCAL_NATION_ID	//not the owner, so the display is same as normal unit
		)
	)
	{

		bool bNoAirWeaponDisplay;

		u32 airWeapon;			//[EBP-01] then [EBP-05]
		u8 airWeaponMainUnit;	//BL
		u8 airWeaponSubunit;	//AL

		bool cancelDisplay = false;

		std::vector<int*> events_override_arg(2);

		events_override_arg[0] = (int*)activeUnit;
		events_override_arg[1] = (int*)&index;

		u16 unitId = getLastQueueSlotType(activeUnit);

		EventManager::EventCalling(EventId::STATUS_DISPLAYING_PANEL_ARMOR_OVERRIDE, &cancelDisplay, events_override_arg);

		if (!cancelDisplay) {
			if (units_dat::ArmorUpgrade[unitId] != UpgradeId::GlobalUpgrade60) {
				StatsArmorLevel_Helper(dialog, index);
				index++;
			}
		}
		else
			cancelDisplay = false;

		EventManager::EventCalling(EventId::STATUS_DISPLAYING_PANEL_SHIELD_OVERRIDE, &cancelDisplay, events_override_arg);

		if (!cancelDisplay) {
			if (units_dat::ShieldsEnabled[activeUnit->id]) {
				StatsShieldLevel_Helper(dialog, index);
				index++;
			}
		}
		else
			cancelDisplay = false;

		EventManager::EventCalling(EventId::STATUS_DISPLAYING_PANEL_WEAPON_GROUND_OVERRIDE, &cancelDisplay, events_override_arg);

		if(!cancelDisplay) {

			if(
				activeUnit->id != UnitId::ZergLurker ||
				activeUnit->status & UnitStatus::Burrowed
			) 
			{

				bool bNoGroundWeaponDisplay = false;

				u32 groundWeapon;													//[EBP-05]
				u8 groundWeaponMainUnit = units_dat::GroundWeapon[activeUnit->id];	//BL
				u8 groundWeaponSubunit;												//CL

				if(groundWeaponMainUnit == WeaponId::None) {
					if(activeUnit->subunit == NULL)
						bNoGroundWeaponDisplay = true;
					else
						groundWeaponSubunit = units_dat::GroundWeapon[activeUnit->subunit->id];
				}
				else
					groundWeaponSubunit = groundWeaponMainUnit;

				if(!bNoGroundWeaponDisplay) {

					if(groundWeaponSubunit == WeaponId::None)
						bNoGroundWeaponDisplay = true;			//26D19
					else
					if(activeUnit->id == UnitId::ZergLurker && !(activeUnit->status & UnitStatus::Burrowed))
						groundWeapon = UnitId::None;			//26D2A
					else
					if(groundWeaponMainUnit != WeaponId::None)
						groundWeapon = groundWeaponMainUnit;	//26D35
					else
					if(activeUnit->subunit == NULL)
						groundWeapon = WeaponId::None;			//26D41
					else
						groundWeapon = units_dat::GroundWeapon[activeUnit->subunit->id];	//26D51

					if(!bNoGroundWeaponDisplay) {
						//26D54
						StatsWeaponLevel_Helper(dialog, index, groundWeapon);
						index++;
					}

				}

			}

		}
		else
			cancelDisplay = false;

		EventManager::EventCalling(EventId::STATUS_DISPLAYING_PANEL_WEAPON_AIR_OVERRIDE, &cancelDisplay, events_override_arg);

		if(!cancelDisplay) {

			//26D67
			bNoAirWeaponDisplay = false;
			airWeaponMainUnit = units_dat::AirWeapon[activeUnit->id];

			if(airWeaponMainUnit != WeaponId::None)
				airWeaponSubunit = airWeaponMainUnit;	//26D79
			else
			if(activeUnit->subunit == NULL)
				bNoAirWeaponDisplay = true;				//26D82
			else
				airWeaponSubunit = units_dat::AirWeapon[activeUnit->subunit->id];	//26D8C

			if(!bNoAirWeaponDisplay) {

				if(airWeaponSubunit == WeaponId::None)
					bNoAirWeaponDisplay = true;		//26D94
				else
				if(airWeaponMainUnit != WeaponId::None)
					airWeapon = airWeaponMainUnit;	//26DA2
				else {
					if(activeUnit->subunit == NULL)
						airWeapon = WeaponId::None;	//26DAF
					else
						airWeapon = units_dat::AirWeapon[activeUnit->subunit->id];
				}

				if(!bNoAirWeaponDisplay) {

					u8 groundWeapon;	//AL

					//26DBE
					if(activeUnit->id == UnitId::ZergLurker && !(activeUnit->status & UnitStatus::Burrowed))
						groundWeapon = WeaponId::None;
					else {

						groundWeapon = units_dat::GroundWeapon[activeUnit->id];

						if(groundWeapon == WeaponId::None && activeUnit->subunit != NULL)
							groundWeapon = units_dat::GroundWeapon[activeUnit->subunit->id]; //26DE6

					}

					//26DF0
					if(airWeapon == groundWeapon)
						bNoAirWeaponDisplay = true;
					else
					if(airWeaponMainUnit != WeaponId::None)
						airWeapon = airWeaponMainUnit;	//26DFA
					else
					if(airWeaponMainUnit == WeaponId::None) { //26DFF

						if(activeUnit->subunit == NULL)
							airWeapon = WeaponId::None;	//26E06
						else
							airWeapon = units_dat::AirWeapon[activeUnit->subunit->id]; //26E16

					}

					if(!bNoAirWeaponDisplay) { //26E19
						StatsWeaponLevel_Helper(dialog, index, airWeapon);
						index++;
					}

				} //if(!bNoAirWeaponDisplay)

			} //if(!bNoAirWeaponDisplay)
		}
		else
			cancelDisplay = false;

		EventManager::EventCalling(EventId::STATUS_DISPLAYING_PANEL_HANGAR_OVERRIDE, &cancelDisplay, events_override_arg);

		if(!cancelDisplay) {
			//26E31
			if(
				activeUnit->id == UnitId::ProtossCarrier ||
				activeUnit->id == UnitId::Hero_Gantrithor ||
				activeUnit->id == UnitId::ProtossReaver ||
				activeUnit->id == UnitId::Hero_Warbringer
			)
			{
				StatHangerCount_Helper(dialog, index);
				index++;
			}
		}
		else
			cancelDisplay = false;

		EventManager::EventCalling(EventId::STATUS_DISPLAYING_PANEL_SPIDERMINES_OVERRIDE, &cancelDisplay, events_override_arg);

		if(!cancelDisplay) {
			if(
				activeUnit->id == UnitId::TerranVulture ||
				activeUnit->id == UnitId::Hero_JimRaynorVulture
			)
			{
				if(StatSpidermineCount_Helper(dialog, index))
					index++; //don't increase the index if not displaying
			}
		}
		else
			cancelDisplay = false;

		EventManager::EventCalling(EventId::STATUS_DISPLAYING_PANEL_NUKES_OVERRIDE, &cancelDisplay, events_override_arg);

		if(!cancelDisplay) {
			unitId = getLastQueueSlotType(activeUnit);

			if(unitId == UnitId::TerranNuclearSilo) {
				StatsNukesCount_Helper(dialog, index);
				index++;
			}
		}

	}

	if (index < 4) {

		std::vector<int*> events_arg(2);

		events_arg[0] = (int*)activeUnit;
		events_arg[1] = (int*)&index;

		EventManager::EventCalled(EventId::STATUS_DISPLAYING_PANEL_EXTRAS, events_arg);

	}

	//26E96
	if(dialog->controlType == DialogControlTypes::DialogBox)
		current_dialog = dialog;
	else
		current_dialog = dialog->parent;

	if(current_dialog->childrenDlg != NULL) {

		current_dialog = current_dialog->childrenDlg;

		while(current_dialog != NULL && current_dialog->index != index + 9)
			current_dialog = current_dialog->next;

		while(index < 4 && current_dialog != NULL) {
			hideDialog(current_dialog);
			current_dialog = current_dialog->next;
			index++;
		}

	}

} //void stats_panel_display(BinDlg* dialog)
#else
void stats_panel_display(BinDlg* dialog) {

	BinDlg* current_dialog;
	CUnit* activeUnit = *activePortraitUnit;
	u32 index = 0;

	if(
		!(activeUnit->status & UnitStatus::IsHallucination) ||	//not hallucination, display stuff
		(
			*IS_IN_REPLAY == 0 &&						//not in replay, which hide stuff as if you were the hallucination owner
			activeUnit->playerId != *LOCAL_NATION_ID	//not the owner, so the display is same as normal unit
		)
	)
	{

		bool bNoAirWeaponDisplay;

		u32 airWeapon;			//[EBP-01] then [EBP-05]
		u8 airWeaponMainUnit;	//BL
		u8 airWeaponSubunit;	//AL

		u16 unitId = getLastQueueSlotType(activeUnit);

		if (units_dat::ArmorUpgrade[unitId] != UpgradeId::GlobalUpgrade60) {
			StatsArmorLevel_Helper(dialog, index);
			index++;
		}

		if (units_dat::ShieldsEnabled[activeUnit->id]) {
			StatsShieldLevel_Helper(dialog, index);
			index++;
		}

		if(
			activeUnit->id != UnitId::ZergLurker ||
			activeUnit->status & UnitStatus::Burrowed
		) 
		{

			bool bNoGroundWeaponDisplay = false;

			u32 groundWeapon;													//[EBP-05]
			u8 groundWeaponMainUnit = units_dat::GroundWeapon[activeUnit->id];	//BL
			u8 groundWeaponSubunit;												//CL

			if(groundWeaponMainUnit == WeaponId::None) {
				if(activeUnit->subunit == NULL)
					bNoGroundWeaponDisplay = true;
				else
					groundWeaponSubunit = units_dat::GroundWeapon[activeUnit->subunit->id];
			}
			else
				groundWeaponSubunit = groundWeaponMainUnit;

			if(!bNoGroundWeaponDisplay) {

				if(groundWeaponSubunit == WeaponId::None)
					bNoGroundWeaponDisplay = true;			//26D19
				else
				if(activeUnit->id == UnitId::ZergLurker && !(activeUnit->status & UnitStatus::Burrowed))
					groundWeapon = UnitId::None;			//26D2A
				else
				if(groundWeaponMainUnit != WeaponId::None)
					groundWeapon = groundWeaponMainUnit;	//26D35
				else
				if(activeUnit->subunit == NULL)
					groundWeapon = WeaponId::None;			//26D41
				else
					groundWeapon = units_dat::GroundWeapon[activeUnit->subunit->id];	//26D51

				if(!bNoGroundWeaponDisplay) {
					//26D54
					StatsWeaponLevel_Helper(dialog, index, groundWeapon);
					index++;
				}

			}

		}

		//26D67
		bNoAirWeaponDisplay = false;
		airWeaponMainUnit = units_dat::AirWeapon[activeUnit->id];

		if(airWeaponMainUnit != WeaponId::None)
			airWeaponSubunit = airWeaponMainUnit;	//26D79
		else
		if(activeUnit->subunit == NULL)
			bNoAirWeaponDisplay = true;				//26D82
		else
			airWeaponSubunit = units_dat::AirWeapon[activeUnit->subunit->id];	//26D8C

		if(!bNoAirWeaponDisplay) {

			if(airWeaponSubunit == WeaponId::None)
				bNoAirWeaponDisplay = true;		//26D94
			else
			if(airWeaponMainUnit != WeaponId::None)
				airWeapon = airWeaponMainUnit;	//26DA2
			else {
				if(activeUnit->subunit == NULL)
					airWeapon = WeaponId::None;	//26DAF
				else
					airWeapon = units_dat::AirWeapon[activeUnit->subunit->id];
			}

			if(!bNoAirWeaponDisplay) {

				u8 groundWeapon;	//AL

				//26DBE
				if(activeUnit->id == UnitId::ZergLurker && !(activeUnit->status & UnitStatus::Burrowed))
					groundWeapon = WeaponId::None;
				else {

					groundWeapon = units_dat::GroundWeapon[activeUnit->id];

					if(groundWeapon == WeaponId::None && activeUnit->subunit != NULL)
						groundWeapon = units_dat::GroundWeapon[activeUnit->subunit->id]; //26DE6

				}

				//26DF0
				if(airWeapon == groundWeapon)
					bNoAirWeaponDisplay = true;
				else
				if(airWeaponMainUnit != WeaponId::None)
					airWeapon = airWeaponMainUnit;	//26DFA
				else
				if(airWeaponMainUnit == WeaponId::None) { //26DFF

					if(activeUnit->subunit == NULL)
						airWeapon = WeaponId::None;	//26E06
					else
						airWeapon = units_dat::AirWeapon[activeUnit->subunit->id]; //26E16

				}

				if(!bNoAirWeaponDisplay) { //26E19
					StatsWeaponLevel_Helper(dialog, index, airWeapon);
					index++;
				}

			} //if(!bNoAirWeaponDisplay)

		} //if(!bNoAirWeaponDisplay)

		//26E31
		if(
			activeUnit->id == UnitId::ProtossCarrier ||
			activeUnit->id == UnitId::Hero_Gantrithor ||
			activeUnit->id == UnitId::ProtossReaver ||
			activeUnit->id == UnitId::Hero_Warbringer
		)
		{
			StatHangerCount_Helper(dialog, index);
			index++;
		}

		if(
			activeUnit->id == UnitId::TerranVulture ||
			activeUnit->id == UnitId::Hero_JimRaynorVulture
		)
		{
			if(StatSpidermineCount_Helper(dialog, index))
				index++; //don't increase the index if not displaying
		}

		unitId = getLastQueueSlotType(activeUnit);

		if(unitId == UnitId::TerranNuclearSilo) {
			StatsNukesCount_Helper(dialog, index);
			index++;
		}

	}

	//26E96
	if(dialog->controlType == DialogControlTypes::DialogBox)
		current_dialog = dialog;
	else
		current_dialog = dialog->parent;

	if(current_dialog->childrenDlg != NULL) {

		current_dialog = current_dialog->childrenDlg;

		while(current_dialog != NULL && current_dialog->index != index + 9)
			current_dialog = current_dialog->next;

		while(index < 4 && current_dialog != NULL) {
			hideDialog(current_dialog);
			current_dialog = current_dialog->next;
			index++;
		}

	}

} //void stats_panel_display(BinDlg* dialog)
#endif

;

} //namespace hooks

//-------- Helper function definitions. Do NOT modify! --------//

namespace {

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

const u32 Func_Sub425310 = 0x00425310;
void StatsNukesCount_Helper(BinDlg* dialog, u32 index) {

	__asm {
		PUSHAD
		MOV EAX, dialog
		PUSH index
		CALL Func_Sub425310
		POPAD
	}

}

;

const u32 Func_StatHangerCount = 0x004253D0;
void StatHangerCount_Helper(BinDlg* dialog, u32 index) {

	__asm {
		PUSHAD
		MOV EBX, dialog
		PUSH index
		CALL Func_StatHangerCount
		POPAD
	}

}

;

const u32 Func_Sub425510 = 0x00425510;
void StatsShieldLevel_Helper(BinDlg* dialog, u32 index) {

	__asm {
		PUSHAD
		MOV EAX, dialog
		PUSH index
		CALL Func_Sub425510
		POPAD
	}

}

;

const u32 Func_Sub425600 = 0x00425600;
void StatsArmorLevel_Helper(BinDlg* dialog, u32 index) {

	__asm {
		PUSHAD
		MOV EAX, dialog
		PUSH index
		CALL Func_Sub425600
		POPAD
	}

}

;

const u32 Func_Sub425790 = 0x00425790;
void StatsWeaponLevel_Helper(BinDlg* dialog, u32 index, u32 weaponId) {

	__asm {
		PUSHAD
		MOV EAX, dialog
		PUSH weaponId
		PUSH index
		CALL Func_Sub425790
		POPAD
	}

}

;

const u32 Func_StatSpidermineCount = 0x00426300;
bool StatSpidermineCount_Helper(BinDlg* dialog, u32 index) {

	static Bool32 bPreResult;

	__asm {
		PUSHAD
		MOV EAX, dialog
		PUSH index
		CALL Func_StatSpidermineCount
		MOV bPreResult, EAX
		POPAD
	}

	return (bPreResult != 0);

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

} //unnamed namespace

//End of helper functions

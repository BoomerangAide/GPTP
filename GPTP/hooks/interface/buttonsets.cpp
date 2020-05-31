#include "buttonsets.h"

//Helper functions

namespace {

void registerUserDialogAction(BinDlg* dialog,u32 array_of_fxnInteractFunc,u32 sizeOfArray);	//18100
void disableDialog(BinDlg* dialog);															//18640
void showDialog(BinDlg* dialog);															//186A0
void hideDialog(BinDlg* dialog);															//18700
void function_418E00(BinDlg* dialog);														//18E00
void updateDialog(BinDlg* dialog);															//1C400
void replayStatBtns(BinDlg* dialog);														//27A80
void function_4591D0();																		//591D0
void function_459770();																		//59770
void BINDLG_BlitSurface(BinDlg* dialog);													//C35F0

u32 fxnInteract(BinDlg* dialog, u32 data_struct_offset);
s32 req_check(u32 reqFunc, u16 reqVar, u32 playerId, CUnit* unit);

} //unnamed namespace

//Main names from Firegraft, comments names from vgce
namespace ButtonRequirements {
enum Enum : u32 {
	Slow_Down_Replay = 0x004DEB40,										//BTNSCOND_ReplaySlowDown
	Speed_Up_Replay = 0x004DEAF0,										//BTNSCOND_ReplaySpeedUp
	PlayorPause_Replay = 0x004DEAC0,									//BTNSCOND_ReplayPlayPause
	Always = 0x004282D0,												//BTNSCOND_Always
	Can_Create_UnitorBuilding = 0x00428E60,								//BTNSCOND_CanBuildUnit
	Can_Build_Subunit = 0x00428E00,										//BTNSCOND_TrainingFighter
	Mixed_Group_MoveorPatrolorHold_Position = 0x00428DA0,				//BTNSCOND_CanMove
	Mixed_Group_Stop = 0x00428D40,										//BTNSCOND_CanMoveSpecialCase
	Unit_Has_A_Weapon = 0x00428F30,										//BTNSCOND_CanAttack
	Attacking_Building = 0x00428ED0,									//BTNSCOND_Stationary
	Cancel_Last = 0x00428530,											//BTNSCOND_IsTraining
	Cancel_AddOn = 0x00428920,											//BTNSCOND_IsBuildingAddon
	Larva_Exists_Upgrading_Only = 0x00428500,							//BTNSCOND_HatcheryLairHive
	Rally_Point = 0x00429520,											//BTNSCOND_Rally
	Rally_Point_While_Upgrading_Only = 0x00429740,						//BTNSCOND_HatcheryLairHiveRally
	ConstructionorMutation_Underway = 0x004284E0,						//BTNSCOND_IsConstructing
	Upgrade_Underway = 0x00428900,										//BTNSCOND_isUpgrading
	Research_Underway = 0x004288E0,										//BTNSCOND_IsResearching
	Unit_Is_Carrying_Nothing__Gather_Probe = 0x004284B0,				//BTNSCOND_NoCargo
	Unit_Is_Carrying_Something_Return_Cargo_Probe = 0x00428480,			//BTNSCOND_HasCargo
	Unit_Capacity_Has_Not_Been_Met = 0x00428FF0,						//BTNSCOND_HasRoom
	Carrying_Some_Units = 0x00428EA0,									//BTNSCOND_HasUnit
	Tech_Spell_Not_Researched = 0x00429500,								//BTNSCOND_CanResearch
	Spell_Researched = 0x004294E0,										//BTNSCOND_HasTech
	Has_Spidermine_and_Is_Researched = 0x00429470,						//BTNSCOND_HasSpidermines
	Upgrade_Not_At_Max_Level = 0x00429450,								//BTNSCOND_CanUpgrade
	Can_Cloak = 0x004293E0,												//BTNSCOND_CanCloak_0
	Can_Decloak = 0x00429370,											//BTNSCOND_IsCloaked_0
	Can_Cloak_Mixed = 0x004292C0,										//BTNSCOND_CanCloak
	Can_Decloak_Mixed = 0x00429210,										//BTNSCOND_IsCloaked
	Tank_Is_In_Tank_Mode = 0x004291C0,									//BTNSCOND_IsUnsieged
	Tank_Is_In_Siege_Mode = 0x00429170,									//BTNSCOND_IsSieged
	Tank_Is_In_Tank_Mode_Move = 0x00428860,								//BTNSCOND_TankMove
	Nuke_Available = 0x00428810,										//BTNSCOND_HasNuke
	Recharge_Shields = 0x00428440,										//BTNSCOND_CanRechargeShields
	Building_Can_Move_And_Has_Lifted_Off = 0x00428420,					//BTNSCOND_IsLiftedCanMove
	Building_Has_Lifted_Off = 0x004283F0,								//BTNSCOND_IsLifted
	Building_Has_Landed = 0x004287D0,									//BTNSCOND_IsLanded
	Ability_Is_Researched = 0x004290F0,									//BTNSCOND_CanBurrow
	Is_Using_Ability = 0x00429070,										//BTNSCOND_IsBurrowed
	Morph_To_Lurker = 0x00429720,										//BTNSCOND_CanMorphLurker
	Not_Burrowed_MoveorStoporPatrolorHold_Position = 0x004283C0,		//BTNSCOND_Movement
	Not_Burrowed__Attack = 0x00428FA0,									//BTNSCOND_BattleOrders
	Has_No_Cargo__Gather_Drone = 0x00428780,							//BTNSCOND_ZergNoCargo
	Has_Cargo__Return_Cargo_Drone = 0x00428730,							//BTNSCOND_ZergHasCargo
	Burrowed__Stop_Lurker = 0x00428360,									//BTNSCOND_LurkerStop
	Has_No_Exit = 0x00428340,											//BTNSCOND_NoNydusExit
	Zerg_Basic_Buildings = 0x00428CB0,									//BTNSCOND_ZergBasic
	Zerg_Advanced_Buildings = 0x00428C30,								//BTNSCOND_ZergAdvanced
	Has_Interceptors = 0x004286E0,										//BTNSCOND_HasInterceptors
	Has_Scarabs = 0x004286A0,											//BTNSCOND_HasScarabs
	Two_Units_Selected_and_Not_Researched_Archon_Merge = 0x004296F0,	//BTNSCOND_CanMergeArchonOneSelected
	Two_Units_Selected__Archon_Warp = 0x004296B0,						//BTNSCOND_CanMergeArchonTwoSelected
	Two_Units_Selected_and_Not_Researched_Dark_Archon_Meld = 0x00429680,//BTNSCOND_CanMergeDarkArchonOneSelected
	Two_Units_Selected__Dark_Archon_Meld = 0x00429640,					//BTNSCOND_CanMergeDarkArchonTwoSelected
	Protoss_Basic_Buildings = 0x00428B80,								//BTNSCOND_ProtossBasic
	Protoss_Advanced_Buildings = 0x00428AD0,							//BTNSCOND_ProtossAdvanced
	Is_Constructing__Halt_SCV = 0x00428310,								//BTNSCOND_SCVisBuilding
	Move__SCV = 0x00428670,												//BTNSCOND_SCVCanMove
	Stop__SCV = 0x00428640,												//BTNSCOND_SCVCanStop
	Attack__SCV = 0x00428610,											//BTNSCOND_SCVCanAttack
	Repair__SCV = 0x004285E0,											//BTNSCOND_CanRepair
	Gather__SCV = 0x004285A0,											//BTNSCOND_NoCargoTerran
	Return_Cargo__SCV = 0x00428560,										//BTNSCOND_HasCargoTerran
	Terran_Basic_Buildings = 0x00428A10,								//BTNSCOND_TerranBasic
	Terran_Advanced_Buildings = 0x00428990,								//BTNSCOND_TerranAdvanced
	Nuke_Train = 0x00428960												//BTNSCOND_SiloHasNoNuke
};
}

;

//Main names from Firegraft, comments names from vgce
namespace ButtonActions {
enum Enum : u32 {
	Cancel_Infestation = 0x00423180,									//BTNSACT_DoNothing
	Rally_Point = 0x004244A0,											//BTNSACT_RallyPoint
	Select_Larva = 0x00423930,											//BTNSACT_SelectLarva
	Create_Unit = 0x004234B0,											//CMDACT_Train
	Cancel_Last = 0x00423490,											//CMDACT_CancelTrain
	Tank_Mode = 0x00423470,												//CMDACT_Unsiege
	Siege_Mode = 0x00423450,											//CMDACT_Siege
	Cancel_Construction = 0x00423430,									//CMDACT_CancelConstruction
	Cancel_Morph = 0x00423410,											//CMDACT_CancelUnitMorph
	Move = 0x00424440,													//BTNSACT_Move
	Stop = 0x004233F0,													//CMDACT_Stop
	Attack = 0x00424380,												//BTNSACT_AttackMove
	Attack_Suicide = 0x00424320,										//BTNSACT_AttackUnit
	Attack_Building = 0x004242C0,										//BTNSACT_TowerAttack
	Move__CarrierorReaver = 0x00424260,									//BTNSACT_CarrierMove
	Stop_Carrier = 0x004233D0,											//CMDACT_CarrierStop
	Stop_Reaver = 0x004233B0,											//CMDACT_ReaverStop
	Attack_Carrier = 0x00424200,										//BTNSACT_CarrierAttack
	Attack_Reaver = 0x004241A0,											//BTNSACT_ReaverAttack
	Build_Subunit = 0x00423390,											//CMDACT_TrainFighter
	Patrol = 0x00424140,												//BTNSACT_Patrol
	Hold_Position = 0x00423370,											//CMDACT_HoldPosition
	Research_Technology = 0x00423350,									//CMDACT_Research
	Cancel_Technology_Research = 0x00423330,							//CMDACT_CancelResearch
	Use_Technology = 0x00423F70,										//BTNSACT_UseTech
	StimPack = 0x004234D0,												//CMDACT_Stimpack
	Research_Upgrade = 0x00423310,										//CMDACT_Upgrade
	Cancel_Upgrade_Research = 0x004232F0,								//CMDACT_CancelUpgrade
	Cancel_AddOn = 0x004232D0,											//CMDACT_CancelAddon
	Create_Building_Terran = 0x00423EB0,								//BTNSACT_BuildTerran
	Place_COP = 0x00423E90,												//BTNSACT_PlaceCOP
	Create_Building_Protoss = 0x00423DD0,								//BTNSACT_BuildProtoss
	Create_AddOn = 0x00423D10,											//BTNSACT_PlaceAddon
	Create_Building_Zerg = 0x00423C50,									//BTNSACT_ZergBuild
	Place_Nydus_Canal_Exit = 0x00423C40,								//BTNSACT_PlaceNydusExit
	Building_Morph = 0x00423860,										//CMDACT_BuildingMorph
	Land = 0x00423C30,													//BTNSACT_BuildingLand
	Repair = 0x00423BD0,												//BTNSACT_Repair
	Unit_Morph = 0x00423790,											//CMDACT_UnitMorph
	Gather = 0x00423B70,												//BTNSACT_Harvest
	Return_Cargo = 0x00423760,											//CMDACT_ReturnCargo
	Burrow = 0x004232B0,												//CMDACT_Burrow
	Unburrow = 0x00423290,												//CMDACT_Unburrow
	Cloak = 0x00423730,													//CMDACT_Cloak
	Decloak = 0x00423270,												//CMDACT_Decloak
	Lift_Off = 0x00423230,												//CMDACT_Lift
	Load = 0x00423B40,													//BTNSACT_LoadUnit
	Unload = 0x00423B00,												//CMDACT_UnloadAll
	Archon_Warp = 0x00423210,											//CMDACT_MergeArchon
	Dark_Archon_Meld = 0x004231F0,										//CMDACT_MergeDarkArchon
	Recharge_Shields = 0x00423AA0,										//BTNSACT_RechargeShields
	Nuclear_Strike = 0x00423A40,										//BTNSACT_UseNuke
	Cancel_Nuclear_Strike = 0x004231D0,									//CMDACT_CancelNuke
	Heal = 0x004239E0,													//BTNSACT_Heal
	Slow_Down_Replay = 0x004DF300,										//CMDACT_DecreaseReplaySpeed
	PlayorPause_Replay = 0x004DF2C0,									//CMDACT_SetReplaySpeed
	Speed_Up_Replay = 0x004DF260,										//CMDACT_IncreaseReplaySpeed
	Cancel = 0x00429770,												//BTNSACT_TargetOrderCancel
	Cancel_Place_Building = 0x00429780,									//BTNSACT_CancelPlacement
	Change_Displayed_Buttons = 0x00459AF0								//BTNSACT_ChangeButtons
};
}


;

//Example on how to create a custom button set.
//This make a copy of the Marine buttonset.
//Note that the function would have to be duplicated
//for each buttonset to create
BUTTON_SET* getCustomButtonSet(BUTTON_SET* base_buttonset) {

	//Use static variables to avoid memory allocation trouble
	static BUTTON_SET customButtonSet;
	static BUTTON customButtonsArray[50]; //more than 9 buttons if 1 button can replace another (siege mode/tank mode,burrow/unburrow...)

	//since it's a copy of the marine set, it would have the same amount of buttons
	//this need to be modded according to what you expected selected due to hardcoded marine in example
	customButtonSet.buttonsInSet = buttonSetTable[UnitId::TerranMarine].buttonsInSet;
	//customButtonSet.buttonsInSet = base_buttonset->buttonsInSet; //to use buttonset passed

	//connectedUnit should usually be init at 0xFFFF.Almost unused (maybe not fully implemented).
	customButtonSet.connectedUnit = 0xFFFF;

	//this should always be like this since the array is static
	customButtonSet.firstButton = &customButtonsArray[0];

	//For each button in the array until reaching the amount of buttonsInSet, copy the data for
	//each member of the button structure from the unmodded set to the custom set.
	//this need to be modded according to what you expected selected due to hardcoded marine in example
	for(u32 i = 0; i < customButtonSet.buttonsInSet; i++) {
		//note: for each line, could also be:
		//customButtonsArray[i].something = base_buttonset->firstButton[i].something;
		customButtonsArray[i].actFunc = buttonSetTable[UnitId::TerranMarine].firstButton[i].actFunc;
		customButtonsArray[i].actStringID = buttonSetTable[UnitId::TerranMarine].firstButton[i].actStringID;
		customButtonsArray[i].actVar = buttonSetTable[UnitId::TerranMarine].firstButton[i].actVar;
		customButtonsArray[i].iconID = buttonSetTable[UnitId::TerranMarine].firstButton[i].iconID;
		customButtonsArray[i].position = buttonSetTable[UnitId::TerranMarine].firstButton[i].position;
		customButtonsArray[i].reqFunc = buttonSetTable[UnitId::TerranMarine].firstButton[i].reqFunc;
		customButtonsArray[i].reqStringID = buttonSetTable[UnitId::TerranMarine].firstButton[i].reqStringID;
		customButtonsArray[i].reqVar = buttonSetTable[UnitId::TerranMarine].firstButton[i].reqVar;
	}

	return &customButtonSet;

}

namespace hooks {

	; //458BC0 

	//Called (with a jump) as last instruction of
	//updateButtonSet
	//Will update the buttonset for the currently
	//selected unit if alone.
	//Handle special case of group orders if multiple
	//units are selected
	void updateButtonSetEx() {

		u16* const BUTTONSET_CURRENT_BUTTONSETID = (u16*)0x0068C1C4;

		u8 countWorkers = 0;
		u8 countCloakables = 0;
		u8 countNotInAirAndZergs = 0;
		u8 countBurrowables = 0;

		u16 activeButtonSetId;
		u16 selectedButtonSetId;

		u32 flagsHolder;

		int unitIndexFromSelection = 0;

		CUnit* unitFromSelection;

		activeButtonSetId = (*activePortraitUnit)->currentButtonSet;

		do { //count units to know if will be using special group buttonset (peons,cloakables...)

			unitFromSelection = clientSelectionGroup->unit[unitIndexFromSelection];

			if(unitFromSelection != NULL) {

				flagsHolder = units_dat::BaseProperty[unitFromSelection->id];

				selectedButtonSetId = unitFromSelection->currentButtonSet;

				if(flagsHolder & UnitProperty::Worker)
					countWorkers++;

				if(flagsHolder &  UnitProperty::Cloakable)
					countCloakables++;

				//count flying zerg units (to disable "burrow group" if there's one)
				if( !(unitFromSelection->status & UnitStatus::InAir) &&
					units_dat::GroupFlags[unitFromSelection->id].isZerg
					)
					countNotInAirAndZergs++;

				if(flagsHolder & UnitProperty::Burrowable)
					countBurrowables++;

				//if you use a custom buttonset, and are reusing the currentButtonSet property
				//of an unit differently, change these conditions to take it in account
				if(selectedButtonSetId != activeButtonSetId) {

					BUTTON_SET* selectedUnitButtonSet = hooks::getButtonSet(selectedButtonSetId);
					BUTTON_SET* activeUnitButtonSet = hooks::getButtonSet(activeButtonSetId);

					if(activeUnitButtonSet->firstButton != selectedUnitButtonSet->firstButton) {

						if(selectedUnitButtonSet->connectedUnit != activeButtonSetId) {
							//kerrigan(ghost) and raynor(vulture) have the corresponding normal
							//unit as "connected unit", they are the only ones unless you add
							//some custom ones
							
							if(activeUnitButtonSet->connectedUnit == selectedButtonSetId) {
								activeButtonSetId = selectedButtonSetId;
								*BUTTONSET_CURRENT_BUTTONSETID = selectedButtonSetId;
							}
							else
								*BUTTONSET_CURRENT_BUTTONSETID = UnitId::Buttons_GroupMixed;
						}

					}

				} //if(DX_holder != activeButtonSetId)

			} //if(unitFromSelection != NULL)

			//58C88
			unitIndexFromSelection++;

		}while(unitIndexFromSelection < SELECTION_ARRAY_LENGTH);

		if(*BUTTONSET_CURRENT_BUTTONSETID != UnitId::Buttons_Blank) {

			if(countWorkers == *clientSelectionCount)
				*BUTTONSET_CURRENT_BUTTONSETID = UnitId::Buttons_GroupPeons;
			else
			if(countCloakables == *clientSelectionCount)
				*BUTTONSET_CURRENT_BUTTONSETID = UnitId::Buttons_GroupCloaker;
			else
			if( (countBurrowables != 0) && (countNotInAirAndZergs == *clientSelectionCount) )
				*BUTTONSET_CURRENT_BUTTONSETID = UnitId::Buttons_GroupBurrower; //aerial units and non-zergs units prevent this (but not Ultralisk)

		}

	} //updateButtonSetEx
	
	; /*458D50*/

	//Not calling others converted to C functions.
	//Probably making it so the button corresponding 
	//to current order has light on.
	//Last function called within updateCurrentButtonSet
	void updateButtonSet_Sub458D50() {

		BinDlg** const BUTTONSET_DIALOG	= (BinDlg**)0x0068C148;
		u8* const BUTTONSET_SUBUNIT_MAINORDERID = (u8*)0x0068C1B8;
		u16* const BUTTONSET_MAINBUTTONGRAPHIC = (u16*)0x0068C1BC;
		u8* const BUTTONSET_UNIT_MAINORDERID = (u8*)0x0068C1E4;

		u8 currentUnitMainOrderId;
		u8 currentSubUnitMainOrderId;

		u16 currentUnitButtonIcon;
		u16 currentSubUnitButtonIcon;

		u16 usedButtonIcon;

		BinDlg* currentButtonDlg;

		currentUnitMainOrderId = *BUTTONSET_UNIT_MAINORDERID;
		currentUnitButtonIcon = orders_dat::ButtonIcon[currentUnitMainOrderId];

		currentButtonDlg = (*BUTTONSET_DIALOG)->childrenDlg;

		currentSubUnitMainOrderId = *BUTTONSET_SUBUNIT_MAINORDERID;

		//if the subunit hold the weapon, get the icon from it
		if(currentSubUnitMainOrderId != 0xFF)
			currentSubUnitButtonIcon = orders_dat::ButtonIcon[currentSubUnitMainOrderId];
		else
			currentSubUnitButtonIcon = 0xFFFF;

		if(currentUnitButtonIcon != 0xFFFF) {

			if(currentSubUnitButtonIcon == 0xFFFF || currentUnitButtonIcon != IconId::Stop)
				usedButtonIcon = currentUnitButtonIcon;
			else
				usedButtonIcon = currentSubUnitButtonIcon;

		}
		else
			usedButtonIcon = currentSubUnitButtonIcon;

		if(currentButtonDlg != NULL) {

			bool bStopLooping = false;

			do { 

				//look for the button corresponding to the one used
				//for the current mainOrder

				if(currentButtonDlg->flags & BinDlgFlags::Visible) {

					if(currentButtonDlg->graphic != usedButtonIcon)
						currentButtonDlg = currentButtonDlg->next;
					else
						bStopLooping = true;

				}
				else
					currentButtonDlg = currentButtonDlg->next;

			}while(!bStopLooping && currentButtonDlg != NULL);

		}

		if(currentButtonDlg == NULL)
			*BUTTONSET_MAINBUTTONGRAPHIC = 0xFFFF;
		else {
			
			if( !(currentButtonDlg->flags & BinDlgFlags::Unknown0) ) {
				currentButtonDlg->flags |= BinDlgFlags::Unknown0;
				updateDialog(currentButtonDlg);
			}

			*BUTTONSET_MAINBUTTONGRAPHIC = usedButtonIcon;

		}

	} //void updateButtonSet_Sub458D50()

	;
	
	; //458DE0

	//Case when buttonset would use 
	//BUTTONSET_SPECIAL_BUTTONSETID
	//Use updateButtonSetEx() for individual
	//units and groups
	//Called by updateCurrentButtonset and
	//updateSelectedUnitData (004C38B0)
	void updateButtonSet() {

		u16* const BUTTONSET_PORTRAIT_BUTTONSETID	= (u16*)		0x0068C14C;
		u16*  const BUTTONSET_CURRENT_BUTTONSETID	= (u16*)		0x0068C1C4;
		u16*  const BUTTONSET_SPECIAL_BUTTONSETID	= (u16*)		0x0068C1C8; //replay, cancel building...

		Bool8* const bDoingCancellableTargetOrder	= (Bool8*)		0x00641694;

		*BUTTONSET_SPECIAL_BUTTONSETID = UnitId::Buttons_Blank;
		*BUTTONSET_PORTRAIT_BUTTONSETID = UnitId::Buttons_Blank;

		if(!scbw::isInReplay()) {

			if(*clientSelectionCount != 0) {

				if(*bDoingCancellableTargetOrder == 0) {
					if(*IS_PLACING_BUILDING)
						*BUTTONSET_SPECIAL_BUTTONSETID = UnitId::Buttons_CancelPlaceBuilding;
				}
				else
					*BUTTONSET_SPECIAL_BUTTONSETID = UnitId::Buttons_Cancel;

			}

		}
		else {		

			if(*IS_REPLAY_PAUSED != 0)
				*BUTTONSET_SPECIAL_BUTTONSETID = UnitId::Buttons_ReplayPaused;
			else
				*BUTTONSET_SPECIAL_BUTTONSETID = UnitId::Buttons_ReplayPlaying;

		}

		*BUTTONSET_CURRENT_BUTTONSETID = UnitId::Buttons_Blank;

		if( (*clientSelectionCount > 1) && (!scbw::isInReplay()) )
			updateButtonSetEx();

	} //updateButtonSet

	
	;

	//Add active/selected units checks here
	//and you can change the buttonset to any
	//you created here in GPTP using something
	//based on getCustomButtonSet method or
	//a more customized way.
	//Note: if you use updateButtonSet_Sub4591D0,
	//you can disable the injection of this
	//function in the _inject.cpp file.
	BUTTON_SET* getButtonSet(int index) {
		return &(buttonSetTable[index]);
		//if using custom button set, call it with return getCustomButtonSet(&buttonSetTable[index]);
	}

	; //4599A0  

	//Main updateButtonSet function because it
	//call updateButtonSet() and others
	void updateCurrentButtonset() {

		BinDlg** const BUTTONSET_DIALOG	= (BinDlg**)0x0068C148;
		u16* const BUTTONSET_PORTRAIT_BUTTONSETID = (u16*)0x0068C14C;
		u8* const BUTTONSET_SUBUNIT_MAINORDERID = (u8*)0x0068C1B8;
		u16* const BUTTONSET_CURRENT_BUTTONSETID = (u16*)0x0068C1C4;
		u16* const BUTTONSET_SPECIAL_BUTTONSETID = (u16*)0x0068C1C8; //replay, cancel building...
		u8* const BUTTONSET_UNIT_MAINORDERID = (u8*)0x0068C1E4;

		Bool32* const bCanUpdateCurrentButtonSet = (Bool32*)0x0068C1B0;

		bool jumpTo59A38 = false;
		bool jumpTo59A9F = false;
		bool jumpTo59A83 = false;
		bool jumpTo59A86 = false;

		u8 mainOrderIdHolderUnit_EBP;
		u8 mainOrderIdHolderSubUnit_BL;
		u16 CX_Holder;

		if(!*bCanUpdateCurrentButtonSet) {

			//599B7
			bool bNeedUpdateButtonSet = false;

			if(*BUTTONSET_SPECIAL_BUTTONSETID != UnitId::Buttons_Blank) {
				if(*IS_IN_REPLAY)
					bNeedUpdateButtonSet = true;
			}
			else
				bNeedUpdateButtonSet = true;

			if(bNeedUpdateButtonSet)
				updateButtonSet();

		}

		CX_Holder = *BUTTONSET_SPECIAL_BUTTONSETID;

		//599D3

		if( *BUTTONSET_SPECIAL_BUTTONSETID == UnitId::Buttons_Blank ) {

			//599DF

			CX_Holder = *BUTTONSET_CURRENT_BUTTONSETID;

			if(*BUTTONSET_CURRENT_BUTTONSETID == UnitId::Buttons_Blank) {
				if ( *activePortraitUnit == NULL )
					jumpTo59A38 = true;
				else
					CX_Holder = (*activePortraitUnit)->currentButtonSet;
			}

		}

		if(!jumpTo59A38) {

			//599F8

			if(*BUTTONSET_PORTRAIT_BUTTONSETID == CX_Holder)
				jumpTo59A38 = true;
			else {
				*BUTTONSET_PORTRAIT_BUTTONSETID = CX_Holder;
				*bCanUpdateCurrentButtonSet = 1;
			}

			if(!jumpTo59A38) {

				//59A12:

				//draw buttons
				function_4591D0();

				//refresh popup text over button
				function_459770();

				jumpTo59A9F = (*activePortraitUnit == NULL);

				if(!jumpTo59A9F) {

					mainOrderIdHolderUnit_EBP = (*activePortraitUnit)->mainOrderId;
					CX_Holder = mainOrderIdHolderUnit_EBP;

					if( (*activePortraitUnit)->subunit == NULL)
						jumpTo59A83 = true;
					else {
						mainOrderIdHolderSubUnit_BL = (*activePortraitUnit)->subunit->mainOrderId;
						jumpTo59A86 = true;
					}

				} //if(!jumpTo59A9F)

			}

		} //if(!jumpTo59A38)

		if(jumpTo59A38) {

			if(*bCanUpdateCurrentButtonSet) {

				//59A12:

				//draw buttons
				function_4591D0();

				//refresh popup text over button
				function_459770();

				jumpTo59A9F = (*activePortraitUnit == NULL);

				if(!jumpTo59A9F) {
					
					mainOrderIdHolderUnit_EBP = (*activePortraitUnit)->mainOrderId;
					CX_Holder = mainOrderIdHolderUnit_EBP;

					if( (*activePortraitUnit)->subunit == NULL)
						jumpTo59A83 = true;
					else {
						mainOrderIdHolderSubUnit_BL = (*activePortraitUnit)->subunit->mainOrderId;
						jumpTo59A86 = true;
					}

				} //if(!jumpTo59A9F)

			} //if(*bCanUpdateCurrentButtonSet != 0)

		} //if(jumpTo59A38)

		if(!jumpTo59A83 && !jumpTo59A86 && !jumpTo59A9F) {

			//59A3C

			if(*activePortraitUnit != NULL) {

				bool jumpTo59ABD = false;

				mainOrderIdHolderUnit_EBP = (*activePortraitUnit)->mainOrderId;

				if( (*activePortraitUnit)->subunit != NULL )
					mainOrderIdHolderSubUnit_BL = (*activePortraitUnit)->subunit->mainOrderId;
				else
					mainOrderIdHolderSubUnit_BL = 0xFF;

				//59A55

				if( *BUTTONSET_UNIT_MAINORDERID == mainOrderIdHolderUnit_EBP) {

					//59A5D

					if( *BUTTONSET_SUBUNIT_MAINORDERID == mainOrderIdHolderSubUnit_BL)
						jumpTo59ABD = true;

				}

				if(!jumpTo59ABD) {

					//59A65

					//light up the button of the current order
					updateButtonSet_Sub458D50();

					*BUTTONSET_UNIT_MAINORDERID = mainOrderIdHolderUnit_EBP;
					*BUTTONSET_SUBUNIT_MAINORDERID = mainOrderIdHolderSubUnit_BL;

					//light up the button of the current order
					updateButtonSet_Sub458D50();

				}

			}

		}

		if(jumpTo59A83)	{ //cannot be set if a jump beyond is set
			mainOrderIdHolderSubUnit_BL = 0xFF;
			jumpTo59A86 = true;
		}

		if(jumpTo59A86) {

			//light up the button of the current order
			updateButtonSet_Sub458D50();

			*BUTTONSET_UNIT_MAINORDERID = mainOrderIdHolderUnit_EBP;
			*BUTTONSET_SUBUNIT_MAINORDERID = mainOrderIdHolderSubUnit_BL;

			//light up the button of the current order
			updateButtonSet_Sub458D50();

			jumpTo59A9F = true;

		}

		if(jumpTo59A9F) {

			if(*IS_IN_REPLAY)
				replayStatBtns(*BUTTONSET_DIALOG);

			//59AB3

			*bCanUpdateCurrentButtonSet = 0;

		}
		
	} //void updateCurrentButtonset()

	;

void updateButtonSet_Sub4591D0() {

	BinDlg**	const BUTTONSET_DIALOG	=				(BinDlg**)	0x0068C148;
	u16*		const BUTTONSET_PORTRAIT_BUTTONSETID =	(u16*)		0x0068C14C;
	BUTTON_SET* const current_buttonset = getButtonSet(*BUTTONSET_PORTRAIT_BUTTONSETID);
	u32*		const unknown_0066FF60 =				(u32*)		0x0066FF60;

	bool bStopThere = false;
	bool jumpto59336 = false;
	bool jumpto5933C = false;
	bool jumpto5934E = false;

	s32 current_button_state;
	BUTTON* current_button;
	BinDlg* current_dialog;	//BinDlg version of the button most of the time
	u16 buttons_count = 0;

	current_dialog = *BUTTONSET_DIALOG;
	current_button = current_buttonset->firstButton;
	current_button_state = BUTTON_STATE::Invisible;

	if(current_dialog->controlType != DialogControlTypes::DialogBox)
		current_dialog = current_dialog->parent;

	//59203:
	current_dialog = current_dialog->childrenDlg;

	while(current_dialog != NULL && current_dialog->index != 1)
		current_dialog = current_dialog->next;

	if(current_dialog == NULL || current_dialog->index != 1)
		bStopThere = true;

	while(!bStopThere) {

		//59230
		if(current_dialog->index < 1)
			bStopThere = true;
		else
		if(current_dialog->index > 9)
			jumpto5934E = true;
		else { //loop to ignore the first unused/disabled buttons

			current_button_state = BUTTON_STATE::Invisible;

			while (current_button_state == BUTTON_STATE::Invisible && (buttons_count < current_buttonset->buttonsInSet)) {

				current_button_state = req_check((u32)current_button->reqFunc,(u8)current_button->reqVar,(u8)*LOCAL_NATION_ID,*activePortraitUnit);

				if(current_button_state == BUTTON_STATE::Invisible) {
					buttons_count++;
					current_button++;	//select next button (advance by sizeof(BUTTON))
				}

			};

		}

		//59278
		if(!bStopThere && !jumpto5934E) {

			jumpto5933C = 
				(!*IS_IN_REPLAY) &&
				(*activePortraitUnit != NULL) &&
				( (*activePortraitUnit)->playerId != *LOCAL_NATION_ID );

			if(!jumpto5933C) {

				//5929A:

				if(	buttons_count >= current_buttonset->buttonsInSet ||
					current_dialog->index < current_button->position)
					jumpto5933C = true;
				else {

					//592B5
					current_dialog->buttonUser = current_button;

					if(current_dialog->graphic != current_button->iconID) {

						//592C2
						struct {
							u32 unknown_value_1;	//[EBP-1C]	[00]
							u32 unknown_value_2;	//[EBP-18]	[04]
							u32 not_allocated;		//[EBP-14]	[08]
							u16 unknown_value_3;	//[EBP-10]	[0C]
							s16 mouseX;				//[EBP-0E]	[0E]
							s16 mouseY;				//[EBP-0C]	[10]
							u16 not_allocated_2;	//[EBP-0A]	[12]
							u32 buttonState;		//[EBP-08]	[14]
							BUTTON_SET* buttonset;	//[EBP-04]	[18]
						} stack_placeholder;

						current_dialog->graphic = current_button->iconID;

						if( !(current_dialog->flags & BinDlgFlags::Unknown0) ) {
							current_dialog->flags |= BinDlgFlags::Unknown0;
							updateDialog(current_dialog);
						}


						stack_placeholder.not_allocated	= 0;
						stack_placeholder.not_allocated_2 = 0;

						stack_placeholder.unknown_value_1 = 0x00000006;
						stack_placeholder.unknown_value_2 = 0x00000000;
						stack_placeholder.unknown_value_3 = 0x000E;
						stack_placeholder.mouseY = mouse->y;
						stack_placeholder.mouseX = mouse->x;
						stack_placeholder.buttonState = current_button_state;
						stack_placeholder.buttonset = current_buttonset;

						fxnInteract(current_dialog,(u32)&stack_placeholder);

						current_button_state = stack_placeholder.buttonState;

					}

					//5930B
					showDialog(current_dialog);

					if(current_button_state == BUTTON_STATE::Disabled) {

						disableDialog(current_dialog);

						//5931C
						if(*unknown_0066FF60 == 0x15) {
							current_button->actStringID = 0x02FA; /*Text: Not Implemented.*/
							current_button++;	//select next button (advance by sizeof(BUTTON)
							buttons_count++;
							jumpto5934E = true;
						}
						else
							jumpto59336 = true;

					}

					//59331
					if(!jumpto59336 && !jumpto5934E)
						function_418E00(current_dialog);

					if(!jumpto5934E) {
						//59336
						current_button++;	//select next button (advance by sizeof(BUTTON)
						buttons_count++;
						jumpto5934E = true;
					}

				} //not met conditions to jump to 5933C for now

			} //if(!jumpto5933C)

		} //if(!bStopThere && !jumpto5934E)

		if(!bStopThere && !jumpto5934E) { 
			//5933C
			hideDialog(current_dialog);
			current_dialog->graphic = 0xFFFF;
			current_dialog->buttonUser = NULL;
		}

		//5934E
		current_dialog = current_dialog->next;

		if(current_dialog == NULL)
			bStopThere = true;

		jumpto59336 = false;
		jumpto5933C = false;
		jumpto5934E = false;

	} //while(!bStopThere)

} //void updateButtonSet_Sub4591D0()

	; //459AD0

	//Identical to array at 0x0051AD30
	//size is 40 = 4 * 10 = 0x28
	//0x004598D0 is statbtn_Btn_Interact
	//0x00459890 is statbtn_BIN_ReplayProgressbar
	const u32 Btn_fxnInteractFuncs[] =
	{
		0x004598D0, 0x004598D0, 0x004598D0,
		0x004598D0, 0x004598D0, 0x004598D0,
		0x004598D0, 0x004598D0, 0x004598D0,
		0x00459890
	};

	//Using a local array instead of the original may allow to go past
	//the 9 buttons limitations
	//May need a special case using the original array adress and size
	//when in replay if modified
	void statbtn_BIN_CustomCtrlID(BinDlg* dialog) {
		registerUserDialogAction(dialog,(u32)&Btn_fxnInteractFuncs,sizeof(Btn_fxnInteractFuncs));
		BINDLG_BlitSurface(dialog);
	}

	;

} //hooks

;

namespace {

	/**** Definitions of helper functions. Do NOT modify anything below! ****/

	const u32 Func_registerUserDialogAction = 0x00418100;
	void registerUserDialogAction(BinDlg* dialog,u32 array_of_fxnInteractFunc,u32 sizeOfArray) {

		__asm {
			PUSHAD
			MOV EAX, dialog
			MOV EDI, array_of_fxnInteractFunc
			MOV ECX, sizeOfArray
			CALL Func_registerUserDialogAction
			POPAD
		}

	}

	;

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

	const u32 Func_Sub418E00 = 0x00418E00;
	void function_418E00(BinDlg* dialog) {

		__asm {
			PUSHAD
			MOV ESI, dialog
			CALL Func_Sub418E00
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

	const u32 Func_ReplayStatBtns = 0x00427A80;
	void replayStatBtns(BinDlg* dialog) {
		__asm {
			PUSHAD
			MOV EBX, dialog
			CALL Func_ReplayStatBtns
			POPAD
		}
	}

	;

	const u32 Func_Sub_4591D0 = 0x004591D0;
	void function_4591D0() {

		__asm {
			PUSHAD
			CALL Func_Sub_4591D0
			POPAD
		}

	}

	;

	const u32 Func_Sub_459770 = 0x00459770;
	void function_459770() {
		__asm {
			PUSHAD
			CALL Func_Sub_459770
			POPAD
		}
	}

	;

	const u32 Func_BINDLG_BlitSurface = 0x004C35F0;
	void BINDLG_BlitSurface(BinDlg* dialog) {
		__asm {
			PUSHAD
			MOV ESI, dialog
			CALL Func_BINDLG_BlitSurface
			POPAD
		}
	}

	;

	//dev info: in this context this call 4598D0 statbtn_Btn_Interact
	u32 fxnInteract(BinDlg* dialog, u32 data_struct_offset) {

		static u32 return_value;
		static u32 fxnInteract;

		fxnInteract = (u32)dialog->fxnInteract;

		__asm {
			PUSHAD
			MOV ESI, dialog
			MOV ECX, dialog
			MOV EDX, data_struct_offset
			CALL fxnInteract
			MOV return_value, EAX
			POPAD
		}

		return return_value;

	}

	;

	//dev info: examples of calls: 429520,428e60,428420,4287D0,4292C0
	s32 req_check(u32 reqFunc, u16 reqVar, u32 playerId, CUnit* unit) {

		static s32 return_value;

		__asm {
			PUSHAD
			MOV CX, reqVar
			MOV EDX, playerId
			PUSH unit
			CALL reqFunc
			MOV return_value, EAX
			POPAD
		}

		return return_value;

	}

	;

} //unnamed namespace

#include "buttonsets.h"

namespace {
	//Helper function
	void updateDialog(BinDlg* dialog);
} //unnamed namespace

	//scbw::refreshConsole()
	u8*   const bCanUpdateSelectedUnitPortrait  = (u8*)			0x0068AC74;
	u32*  const bCanUpdateCurrentButtonSet      = (u32*)		0x0068C1B0;
	BinDlg**  const someDialogUnknown           = (BinDlg**)	0x0068C1E8;
	BinDlg**  const someDialogUnknownUser       = (BinDlg**)	0x0068C1EC;
	u8*   const bCanUpdateStatDataDialog        = (u8*)			0x0068C1F8;

	//for buttonsets
	BUTTON_SET* const buttonSetTable			= (BUTTON_SET*)	0x005187E8;

	//0x00641691 -> order 3
	//0x00641692 -> order 1
	//0x00641693 -> order 2	
	Bool8* const bDoingCancellableTargetOrder	= (Bool8*)		0x00641694;

	BinDlg** const BUTTONSET_DIALOG				= (BinDlg**)	0x0068C148;
	u16* const BUTTONSET_PORTRAIT_BUTTONSETID	= (u16*)		0x0068C14C;
	u8*   const BUTTONSET_SUBUNIT_MAINORDERID	= (u8*)			0x0068C1B8;
	u16* const BUTTONSET_MAINBUTTONGRAPHIC		= (u16*)		0x0068C1BC;
	u16*  const BUTTONSET_CURRENT_BUTTONSETID	= (u16*)		0x0068C1C4;
	u16*  const BUTTONSET_SPECIAL_BUTTONSETID	= (u16*)		0x0068C1C8; //replay, cancel building...
	u8*   const BUTTONSET_UNIT_MAINORDERID		= (u8*)			0x0068C1E4;

	Bool32* const IS_REPLAY_PAUSED				= (Bool32*)		0x006D11B0;

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

//Main names from Firegraft, comments names from vgce
namespace ButtonActions {
enum Enum : u32 {
	Cancel_Infestation = 0x00423180,									//BTNSACT_DoNothing
	Rally_Point = 0x004244A0,											//BTNSACT_RallyPoint
	Select_Larva = 0x00423930,											//BTNSACT_SelectLarva
	Create_Unit = 0x004234B0,											//CMDACT_Train
	Cancel_Last = 0x00423490,											//CMDACT_CancelTrain
	Tank_Mode = 0x00423470,												//CMDACT_Unsiege
	Siege_Mode = 0x00423450,											//CMDACT_Unsiege
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

//Example on how to create a custom button set.
//This make a copy of the Marine buttonset.
//Note that the function would have to be duplicated
//for each buttonset to create
BUTTON_SET* getCustomButtonSet() {

	//Use static variables to avoid memory allocation trouble
	static BUTTON_SET customButtonSet;
	static BUTTON customButtonsArray[50]; //

	//since it's a copy of the marine set, it would have the same amount of buttons
	customButtonSet.buttonsInSet = buttonSetTable[UnitId::TerranMarine].buttonsInSet;

	//connectedUnit should usually be init at 0xFFFF.Almost unused (maybe not fully implemented).
	customButtonSet.connectedUnit = 0xFFFF;

	//this should always be like this since the array is static
	customButtonSet.firstButton = &customButtonsArray[0];

	//For each button in the array until reaching the amount of buttonsInSet, copy the data for
	//each member of the button structure from the marine to the custom set.
	for(u32 i = 0; i < customButtonSet.buttonsInSet; i++) {
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

		u8 currentUnitMainOrderId;
		u8 currentSubUnitMainOrderId;

		u16 currentUnitButtonIcon;
		u16 currentSubUnitButtonIcon;

		u16 usedButtonIcon;

		BinDlg* currentButtonDlg;

		currentUnitMainOrderId = *BUTTONSET_UNIT_MAINORDERID;
		currentUnitButtonIcon = orders_dat::ButtonIcon[currentUnitMainOrderId];

		currentButtonDlg = (BinDlg*)((*BUTTONSET_DIALOG)->childrenSmk);

		currentSubUnitMainOrderId = *BUTTONSET_SUBUNIT_MAINORDERID;

		//if the subunit hold the weapon, get the icon from it
		if(currentSubUnitMainOrderId == 0xFF)
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

			u16 currentButtonDlgGraphic = currentButtonDlg->graphic;
			
			if( !(currentButtonDlg->flags & BinDlgFlags::Unknown0) ) {
				currentButtonDlg->flags |= BinDlgFlags::Unknown0;
				updateDialog(currentButtonDlg);
			}

			*BUTTONSET_MAINBUTTONGRAPHIC = currentButtonDlgGraphic;

		}

	} //void updateButtonSet_Sub458D50()
	
	; //458DE0

	//Case when buttonset would use 
	//BUTTONSET_SPECIAL_BUTTONSETID
	//Use updateButtonSetEx() for individual
	//units and groups
	//Called by updateCurrentButtonset and
	//updateSelectedUnitData (004C38B0)
	bool updateButtonSet() {

		bool JumpAtEnd = false;

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
			JumpAtEnd = true;

		return JumpAtEnd;

	} //updateButtonSet

	
	;

	//Add active/selected units checks here
	//and you can change the buttonset to any
	//you created here in GPTP using something
	//based on getCustomButtonSet method or
	//a more customized way.
	BUTTON_SET* getButtonSet(int index) {
		return &(buttonSetTable[index]);
	}

} //hooks

namespace {

	/**** Definitions of helper functions. Do NOT modify anything below! ****/

	const u32 Func_UpdateDialog	= 0x0041C400;

	void updateDialog(BinDlg* dialog) {
		__asm {
			PUSHAD
			MOV EAX, dialog
			CALL Func_UpdateDialog
			POPAD
		}
	}

} //unnamed namespace

//Only for information
//Too many CALL to be made reliable
//Others functions around updateButtonSet
//were doing tasks relying on available
//functions, and drawing was hard to handle,
//so that's it

	; //4599A0  
	////Main updateButtonSet function because it
	////call updateButtonSet() and others
	//void updateCurrentButtonset() {

	//	bool jumpTo59A38 = false;
	//	bool jumpTo59A9F = false;
	//	bool jumpTo59A83 = false;
	//	bool jumpTo59A86 = false;

	//	u8 mainOrderIdHolderUnit_EBP;
	//	u8 mainOrderIdHolderSubUnit_BL;
	//	u16 CX_Holder;

	//	if(*bCanUpdateCurrentButtonSet != 0) {

	//		//599B7
	//		bool JumpTo599C7 = false;

	//		if(*(u16*)(0x0068C1C8) != UnitId::Buttons_Blank) {
	//			if(*IS_IN_REPLAY != 0)
	//				JumpTo599C7 = true;
	//			else
	//				JumpTo599C7 = false;
	//		}
	//		else
	//			JumpTo599C7 = true;

	//		if(JumpTo599C7)
	//			updateButtonSet();

	//	}

	//	CX_Holder = *(u16*)(0x0068C1C8);

	//	//599D3

	//	if( *(u16*)(0x0068C1C8) == UnitId::Buttons_Blank ) {	//*(u16*)(0x0068C1C8)

	//		CX_Holder = *BUTTONSET_CURRENT_BUTTONSET;

	//		if(*BUTTONSET_CURRENT_BUTTONSET == UnitId::Buttons_Blank) {
	//			if ( *activePortraitUnit == NULL )
	//				jumpTo59A38 = true;
	//			else
	//				CX_Holder = (*activePortraitUnit)->currentButtonSet;
	//		}

	//	}

	//	if(!jumpTo59A38) {

	//		//599F8

	//		if(*(u16*)(0x0068C14C) == CX_Holder)
	//			jumpTo59A38 = true;
	//		else {
	//			*(u16*)(0x0068C14C) = CX_Holder;
	//			*bCanUpdateCurrentButtonSet = 1;
	//		}

	//		if(!jumpTo59A38) {

	//			//59A12:

	//			updateButtonSet_Sub4591D0();
	//			updateButtonSet_Sub459770Wrapper();

	//			jumpTo59A9F = (*activePortraitUnit == NULL);

	//			if(!jumpTo59A9F) {

	//				CX_Holder = (*activePortraitUnit)->mainOrderId;
	//				mainOrderIdHolderUnit_EBP = (*activePortraitUnit)->mainOrderId;

	//				if( (*activePortraitUnit)->subunit == NULL)
	//					jumpTo59A83 = true;
	//				else {
	//					mainOrderIdHolderSubUnit_BL = (*activePortraitUnit)->subunit->mainOrderId;
	//					jumpTo59A86 = true;
	//				}

	//			} //if(!jumpTo59A9F)

	//		}

	//	} //if(!jumpTo59A38)

	//	if(jumpTo59A38) {

	//		if(*bCanUpdateCurrentButtonSet != 0) {

	//			//59A12:

	//			updateButtonSet_Sub4591D0();
	//			updateButtonSet_Sub459770Wrapper();

	//			jumpTo59A9F = (*activePortraitUnit == NULL);

	//			if(!jumpTo59A9F) {

	//				CX_Holder = (*activePortraitUnit)->mainOrderId;
	//				mainOrderIdHolderUnit_EBP = (*activePortraitUnit)->mainOrderId;

	//				if( (*activePortraitUnit)->subunit == NULL)
	//					jumpTo59A83 = true;
	//				else {
	//					mainOrderIdHolderSubUnit_BL = (*activePortraitUnit)->subunit->mainOrderId;
	//					jumpTo59A86 = true;
	//				}

	//			} //if(!jumpTo59A9F)

	//		} //if(*bCanUpdateCurrentButtonSetPointer != 0)

	//	} //if(jumpTo59A38)

	//	if(!jumpTo59A83 && !jumpTo59A86 && !jumpTo59A9F) {

	//		//59A3C

	//		if(*activePortraitUnit != NULL) {

	//			bool jumpTo59ABD = false;

	//			mainOrderIdHolderUnit_EBP = (*activePortraitUnit)->mainOrderId;

	//			if( (*activePortraitUnit)->subunit != NULL )
	//				mainOrderIdHolderSubUnit_BL = (*activePortraitUnit)->subunit->mainOrderId;
	//			else
	//				mainOrderIdHolderSubUnit_BL = 0xFF;

	//			if( *BUTTONSET_UNIT_MAINORDERID == mainOrderIdHolderUnit_EBP) {

	//				//59A5D

	//				if( *BUTTONSET_SUBUNIT_MAINORDERID == mainOrderIdHolderSubUnit_BL)
	//					jumpTo59ABD = true;

	//			}

	//			if(!jumpTo59ABD) {

	//				//59A65

	//				updateButtonSet_Sub458D50();

	//				*BUTTONSET_UNIT_MAINORDERID = mainOrderIdHolderUnit_EBP;
	//				*BUTTONSET_SUBUNIT_MAINORDERID = mainOrderIdHolderSubUnit_BL;

	//				updateButtonSet_Sub458D50();

	//			}

	//		}

	//	}

	//	if(jumpTo59A83)	{ //cannot be set if a jump beyond is set
	//		mainOrderIdHolderSubUnit_BL = 0xFF;
	//		jumpTo59A86 = true;
	//	}

	//	if(jumpTo59A86) {

	//		updateButtonSet_Sub458D50();

	//		*BUTTONSET_UNIT_MAINORDERID = mainOrderIdHolderUnit_EBP;
	//		*BUTTONSET_SUBUNIT_MAINORDERID = mainOrderIdHolderSubUnit_BL;

	//		updateButtonSet_Sub458D50();

	//		jumpTo59A9F = true;

	//	}

	//	if(jumpTo59A9F) {

	//		if(*IS_IN_REPLAY != 0) {

	//			__asm {
	//				PUSHAD
	//				MOV EBX, 0x006D0F14
	//				MOV EBX, [EBX]
	//				CALL Func_ReplayStatBtns
	//				POPAD
	//			}

	//		}

	//		*bCanUpdateCurrentButtonSet = 0;

	//	}
	//	
	//}
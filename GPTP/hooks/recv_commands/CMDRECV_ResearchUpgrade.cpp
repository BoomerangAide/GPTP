#include "CMDRECV_ResearchUpgrade.h"
#include <SCBW/api.h>

//Helper functions declaration

namespace {

bool BeginUpgrade(CUnit* unit, u8 bUpgradeType);					//54A80
bool BeginResearch(CUnit* unit, u8 bTechType);						//54B70
bool ResearchAllowed(CUnit* unit, u8 bTechType, u32 playerId);		//6DE90
bool UpgradeAllowed(CUnit* unit, u8 bUpgradeType, u32 playerId);	//6DFC0

} //unnamed namespace

namespace hooks {

void CMDRECV_Upgrade(u8 bUpgradeType) {

	Bool8* const bCanUpdateSelectedUnitPortrait	= (Bool8*)	0x0068AC74;
	Bool32* const bCanUpdateCurrentButtonSet	= (Bool32*)	0x0068C1B0;
	BinDlg** const someDialogUnknown			= (BinDlg**)0x0068C1E8;
	BinDlg** const someDialogUnknownUser		= (BinDlg**)0x0068C1EC;
	Bool8* const bCanUpdateStatDataDialog		= (Bool8*)	0x0068C1F8;

	CUnit* unit;

	*selectionIndexStart = 0;
	unit = getActivePlayerNextSelection();

	if (
		unit != NULL &&
		getActivePlayerNextSelection() == NULL &&
		UpgradeAllowed(unit,bUpgradeType,*ACTIVE_NATION_ID) &&
		BeginUpgrade(unit, bUpgradeType)
	)
	{
		unit->orderComputerCL(OrderId::Upgrade);
		*bCanUpdateCurrentButtonSet = 1;
		*bCanUpdateSelectedUnitPortrait = 1;
		*bCanUpdateStatDataDialog = 1;
		*someDialogUnknown = NULL;
		*someDialogUnknownUser = NULL;
	}

}

;

void CMDRECV_Research(u8 bTechType) {

	Bool8* const bCanUpdateSelectedUnitPortrait	= (Bool8*)	0x0068AC74;
	Bool32* const bCanUpdateCurrentButtonSet	= (Bool32*)	0x0068C1B0;
	BinDlg** const someDialogUnknown			= (BinDlg**)0x0068C1E8;
	BinDlg** const someDialogUnknownUser		= (BinDlg**)0x0068C1EC;
	Bool8* const bCanUpdateStatDataDialog		= (Bool8*)	0x0068C1F8;

	CUnit* unit;

	*selectionIndexStart = 0;
	unit = getActivePlayerNextSelection();

	if (
		unit != NULL &&
		getActivePlayerNextSelection() == NULL &&
		ResearchAllowed(unit,bTechType,*ACTIVE_NATION_ID) &&
		BeginResearch(unit, bTechType)
	)
	{
		unit->orderComputerCL(OrderId::ResearchTech);
		*bCanUpdateCurrentButtonSet = 1;
		*bCanUpdateSelectedUnitPortrait = 1;
		*bCanUpdateStatDataDialog = 1;
		*someDialogUnknown = NULL;
		*someDialogUnknownUser = NULL;
	}

}

;

} //namespace hooks

//-------- Helper function definitions. Do NOT modify! --------//

namespace {

const u32 Func_BeginUpgrade = 0x00454A80;
bool BeginUpgrade(CUnit* unit, u8 bUpgradeType) {

	static Bool32 bPreResult;

	__asm {
		PUSHAD
		MOV ECX, unit
		MOVZX EAX, bUpgradeType
		CALL Func_BeginUpgrade
		MOV bPreResult, EAX
		POPAD
	}

	return (bPreResult != 0);

}

;

const u32 Func_BeginResearch = 0x00454B70;
bool BeginResearch(CUnit* unit, u8 bTechType) {

	static Bool32 bPreResult;

	__asm {
		PUSHAD
		MOV EDX, unit
		MOVZX EAX, bTechType
		CALL Func_BeginResearch
		MOV bPreResult, EAX
		POPAD
	}

	return (bPreResult != 0);

}

;

const u32 Func_ResearchAllowed = 0x0046DE90;
bool ResearchAllowed(CUnit* unit, u8 bTechType, u32 playerId) {

	static Bool32 bPreResult;

	__asm {
		PUSHAD
		MOVZX EBX, bTechType
		MOV EDI, playerId
		PUSH unit
		CALL Func_ResearchAllowed
		MOV bPreResult, EAX
		POPAD
	}

	return (bPreResult != 0);

}

;

const u32 Func_UpgradeAllowed = 0x0046DFC0;
bool UpgradeAllowed(CUnit* unit, u8 bUpgradeType, u32 playerId) {

	static Bool32 bPreResult;

	__asm {
		PUSHAD
		MOVZX EBX, bUpgradeType
		MOV EDI, playerId
		PUSH unit
		CALL Func_UpgradeAllowed
		MOV bPreResult, EAX
		POPAD
	}

	return (bPreResult != 0);

}

;


} //unnamed namespace

//End of helper functions
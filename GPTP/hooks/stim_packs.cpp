#include "stim_packs.h"
#include "../SCBW/api.h"

//helper functions def
namespace {

void QueueGameCommand(u8* params, u32 param_length);				//0x00485BD0
u32 get_statTxt_Str_0(CUnit* unit, u32 playerId, char* message);	//0x0048EF30
u32 RandBetween(u32 min, u32 max, u32 someIndex);					//0x004DC550

} //unnamed namespace

namespace hooks {

const int MIN_HITPOINTS_FOR_STIMPACKS = 2560; //2560 / 256 => 10 hp cost on ingame display

//Equivalent to original code, only doubt is QueueGameCommand
//Local answer when using stimpacks (check units of selection,
//and can just display an error message without notifying others
//players of the attempt if no unit can do the stimpack action)
void CMDACT_Stimpack() {

	CUnit* currentClientSelectionUnit;
	bool bFoundUnitAbleToStimpack = false;


	//original code used pointer comparison with clientSelectionGroupEnd
	//and incrementing a pointer starting at clientSelectionGroup
	for(int i = 0; i < SELECTION_ARRAY_LENGTH && !bFoundUnitAbleToStimpack; i++) {

		currentClientSelectionUnit = clientSelectionGroup->unit[i];

		if(
			currentClientSelectionUnit != NULL && 
			currentClientSelectionUnit->hitPoints > MIN_HITPOINTS_FOR_STIMPACKS
		) 
			bFoundUnitAbleToStimpack = true;

	}

	if(bFoundUnitAbleToStimpack) {
		static u8 opcode[] = { 0x36 };	//packet opcode for stimpack is 0x36, no others args
		QueueGameCommand(&opcode[0],1);
	}
	else {
		char* message = (char*)statTxtTbl->getString(874); //not enough life remaining
		get_statTxt_Str_0(*activePortraitUnit,*LOCAL_NATION_ID,message);
	}

}

;

//Identical to StimpackSFXRand @ 004554A0
//code used by the AI (no questions about sync between players in that case)
void useStimPacksAIHook(CUnit* unit) {

	const int stimTimerBase = 37;

	if (unit->hitPoints > MIN_HITPOINTS_FOR_STIMPACKS) {

		scbw::playSound(
			RandBetween(SoundId::Terran_MARINE_TMaSti00_WAV, SoundId::Terran_MARINE_TMaSti01_WAV,31), 
			unit
		);

		unit->damageHp(MIN_HITPOINTS_FOR_STIMPACKS,NULL,-1,true);

		if (unit->stimTimer < stimTimerBase) {
			unit->stimTimer = stimTimerBase;
			unit->updateSpeed();
		}

	}

}

;

} //hooks

;

//-------- Helper function definitions. Do NOT modify! --------//

namespace {

const u32 Func_QueueGameCommand = 0x00485BD0;
void QueueGameCommand(u8* params, u32 param_length) {

	__asm {
		PUSHAD
		MOV EDX, param_length
		MOV ECX, params
		CALL Func_QueueGameCommand
		POPAD
	}

}

;

const u32 Func_get_statTxt_Str_0 = 0x0048EF30;
u32 get_statTxt_Str_0(CUnit* unit, u32 playerId, char* message) {

	static u32 return_value;

	__asm {
		PUSHAD
		MOV EAX, message
		MOV ECX, playerId
		MOV EDX, unit
		CALL Func_get_statTxt_Str_0
		MOV EAX, return_value
		POPAD
	}

	return return_value;

}

;

const u32 Func_RandBetween = 0x004DC550;
u32 RandBetween(u32 min, u32 max, u32 someIndex) {

	static u32 return_value;

	__asm {
		PUSHAD
		PUSH max
		MOV EAX, someIndex
		MOV EDX, min
		CALL Func_RandBetween
		MOV return_value, EAX
		POPAD
	}

	return return_value;

}

;

} //Unnamed namespace

//End of helper functions

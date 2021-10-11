#include "cheat_codes.h"
#include <SCBW/api.h>
#include <Windows.h>
#include <logger.h>

#define WRITE_TO_LOG(x) GPTP::logger<<x<<std::endl

//helper functions def

namespace {

void SStrCopy(char* destination, char* source, u32 maxLength);														//1008E
s32 SStrCmpI(char* text1, char* text2, u32 maxLength);																//10094
u32 SStrLen(char* text);																							//100A0
void SC_sprintf_s(char* buffer, int buffer_size, const char* format_string, int value1, int value2);				//1F1B0
void ApplySpeedUpgradeFromUpgradeType(CUnit* unit, u8 upgradeType);													//54540
void function_004807F0();																							//807F0
void RefreshLayer5();																								//BD350
Bool32 parseCmpgnCheatTypeString(char* inputText, char* levelCheatText, u32 unk1, u32 unk2, u32 unk3, char* unk4);	//DBC60

u32 System_GetTickCount();
	
} //unnamed namespace

namespace hooks {

struct TextRelatedStruct {
	char* pointerToText;
	u32 unknown04;
	u32 unknown08;
	u32 unknown0C;
};

//Meant to confirm the effect of the level warp cheat code,
//this allow to use exactly the string entered by the
//player, where other code functions encrypt the input,
//then compare it to the encrypted cheat codes.
//While this is a necessary step, the actual cheat effect
//should be applied only afterward, in cheatActivation.
Bool32 campaignTypeCheatStrings(char* code_entered, u32* newCheatState) {

	static u16* u16_multiplayer_mode = (u16*)0x0057F0B4;	//0 is single player, 1 is multiplayer according to EUDBDB

	Bool32 return_value;

	if (((u8)*u16_multiplayer_mode) != 0) {
		return_value = 0;
	}
	else
	if(!(*CHEAT_STATE & CheatFlags::Ophelia))
	{

		/*Custom example code START*/

		static char* CUSTOM_CHEAT_CODE = "Hello GPTP";
		int length = SStrLen(CUSTOM_CHEAT_CODE);

		//Note: the additional "strlen" is a difference with how observed code work.
		//Without it, you could type a code followed by anything and it would still validate.
		if (SStrCmpI(code_entered, CUSTOM_CHEAT_CODE, length) == 0 && strlen(code_entered) == length)
		{

			*newCheatState ^= (1 << 20);	//An unused flag, check CheatFlags enum to find your spot.
											//Or observe what the other function do with that
											//writeable parameter and adapt.
											//Code based on unhooked cheatcode verifier function and
											//radiofreezerg validator (because it also validate "noglues"
											//somehow).

			return_value = 1;

		}					
		else

		/*Custom example code END*/
			return_value = 0;
	}
	else {

		TextRelatedStruct* textAndData = (TextRelatedStruct*)0x00503580;
		s32 comparison_result = -1;
		int length;

		do {
			length = SStrLen(textAndData->pointerToText);
			comparison_result = SStrCmpI(code_entered, textAndData->pointerToText, length);
			textAndData++;
		} while (comparison_result != 0 && textAndData < (TextRelatedStruct*)0x005035E0);

		if (comparison_result != 0) {
			return_value = 0;
		}
		else
		{

			char unknownOpheliaData = 0;
			Bool32 parseResult;

			textAndData--;

			//not repeated as we still have the value at this point
			//length = SStrLen(textAndData->pointerToText);

			parseResult = parseCmpgnCheatTypeString(
				&code_entered[length],
				textAndData->pointerToText,
				textAndData->unknown04,
				textAndData->unknown08,
				textAndData->unknown0C,
				&unknownOpheliaData
			);

			if (parseResult == 0 || unknownOpheliaData == '?')
				return_value = 1;
			else
			{

				static u32*	u32_0051BFF8	=	(u32*)0x0051BFF8;
				static u32*	u32_0051C410	=	(u32*)0x0051C410;
				static u32*	u32_0051C4BC	=	(u32*)0x0051C4BC;
				static u32*	u32_0051C608	=	(u32*)0x0051C608;
				static u16*	u16_0051CE90	=	(u16*)0x0051CE90;
				static u32*	gwGameMode		=	(u32*)0x00596904;	//Determines the game's mode. In-Game, in-menu, battle.net, etc. according to EUDDB
				static u32*	u32_006D0F31	=	(u32*)0x006D0F31;
				static u32*	u32_006D11BC	=	(u32*)0x006D11BC;
				static u8*	u8_006D11EC		=	(u8*) 0x006D11EC;

				*u32_0051BFF8 = 1;
				*u32_0051C410 = unknownOpheliaData;
				*u32_0051C608 = textAndData->unknown04;
				*u32_0051C4BC = textAndData->unknown0C;

				/*Some optimization below for more compact code because of common lines*/
				*u32_006D11BC = 0;

				if (*gwGameMode != 3)
					*gwGameMode = 4;
				else {

					*u8_006D11EC = 0;
					*u16_0051CE90 = 4;

					if (*IS_IN_REPLAY == 0)
						*u32_006D0F31 = *elapsedTimeFrames;

				}

				return_value = 1;

			}

		}

	}

	return return_value;

}

;

//This is where the cheats (besides radiofreezerg) do take effect.
//Another "custom example" is there to show how things can be
//made to work.
//playerName is either a pointer to a Player class on the name
//entry, or NULL.
void cheatActivation(u32 newCheatState, char* playerName) {

	static u8*			u8_multiplayer_mode	= (u8*)			0x0057F0B4;		//0 is single player, 1 is multiplayer according to EUDBDB
	static u32*			gwGameMode			= (u32*)		0x00596904;		//Determines the game's mode. In-Game, in-menu, battle.net, etc. according to EUDDB
	static u16*			u16_0064096C		= (u16*)		0x0064096C;
	static u32*			u32_00640B54		= (u32*)		0x00640B54;
	static u32*			u32_00641598		= (u32*)		0x00641598;
	static u8*			u8_00641680			= (u8*)			0x00641680;
	static u32*			u32_006CF2C8		= (u32*)		0x006CF2C8;
	static StringTbl*	strTbl_006D1220		= (StringTbl*)  0x006D1220;

	if (*u8_multiplayer_mode == 0) {

		bool bCheatStateChanged = (newCheatState != *CHEAT_STATE);
		u32 oldCheatState = *CHEAT_STATE;
		u32 toggledCheats = *CHEAT_STATE ^ newCheatState; //XOR operation, used for codes turned on/off
		u32 currentNewCheatState = newCheatState;

		*CHEAT_STATE = newCheatState;

		if (toggledCheats & CheatFlags::WarAintWhatItUsedToBe) {
			if (*gwGameMode == 3) {
				RefreshLayer5();
				currentNewCheatState = *CHEAT_STATE;
			}
		}

		//B1FF4
		if (toggledCheats & CheatFlags::BlackSheepWall) {
			if (*gwGameMode == 3) {
				function_004807F0();
				RefreshLayer5();
				currentNewCheatState = *CHEAT_STATE;
			}
		}

		/*Custom example code START*/
		if (currentNewCheatState & (1 << 20)) {

			currentNewCheatState -= (1 << 20);
			*CHEAT_STATE = currentNewCheatState;

			scbw::printText("GPTP: Hello, cheaters!");

		}
		/*Custom example code END*/

		//B200F
		if (currentNewCheatState & CheatFlags::ShowMeTheMoney) {

			currentNewCheatState -= CheatFlags::ShowMeTheMoney;
			*CHEAT_STATE = currentNewCheatState;

			for (int i = 0; i < PLAYER_COUNT; i++) {
				if (playerTable[i].type == PlayerType::Human) {
					resources->minerals[i] += 10000;
					resources->gas[i] += 10000;
				}
			}

		}

		//B208A
		if (currentNewCheatState & CheatFlags::WhatsMineIsMine) {

			currentNewCheatState -= CheatFlags::WhatsMineIsMine;
			*CHEAT_STATE = currentNewCheatState;

			for (int i = 0; i < PLAYER_COUNT; i++) {
				if (playerTable[i].type == PlayerType::Human) {
					resources->minerals[i] += 500;
				}
			}

		}

		//B20F8
		if (currentNewCheatState & CheatFlags::BreatheDeep) {

			currentNewCheatState -= CheatFlags::BreatheDeep;
			*CHEAT_STATE = currentNewCheatState;

			for (int i = 0; i < PLAYER_COUNT; i++) {
				if (playerTable[i].type == PlayerType::Human) {
					resources->gas[i] += 500;
				}
			}

		}

		//B215E
		if (currentNewCheatState & CheatFlags::SomethingForNothing) {

			currentNewCheatState -= CheatFlags::SomethingForNothing;
			*CHEAT_STATE = currentNewCheatState;

			for (int i = 0; i < PLAYER_COUNT; i++) {
				if (playerTable[i].type == PlayerType::Human) {
					//Note: normally, it include GlobalUpgrade60, but when reaching it, skip the content of the loop
					//until the part equivalent to "upgradeId++", then reach UpgradeId::None and end.
					//Optimized it in the way below.
					for (int upgradeId = UpgradeId::TerranInfantryArmor; upgradeId < UpgradeId::GlobalUpgrade60; upgradeId++) {

						u8 upgradeLevel;
						u8 upgradeIdWithBW;

						if (upgradeId >= UpgradeId::UnusedUpgrade46) {
							upgradeIdWithBW = upgradeId - UpgradeId::UnusedUpgrade46;
							upgradeLevel = UpgradesBw->currentLevel[i][upgradeIdWithBW];
						}
						else {
							upgradeIdWithBW = upgradeId;
							upgradeLevel = UpgradesSc->currentLevel[i][upgradeIdWithBW];
						}

						if (upgradeLevel < upgrades_dat::MaxRepeats[upgradeId]) {

							if (upgradeId >= UpgradeId::UnusedUpgrade46) {
								if(*IS_BROOD_WAR != 0)
									UpgradesBw->currentLevel[i][upgradeIdWithBW] = upgradeLevel + 1;
							}
							else
								UpgradesSc->currentLevel[i][upgradeIdWithBW] = upgradeLevel + 1;

							if (firstPlayerUnit->unit[i] != NULL)
								ApplySpeedUpgradeFromUpgradeType(firstPlayerUnit->unit[i], upgradeId);

						}

					}

				}
			}

		}

		//B222E:
		if (*gwGameMode != 3 || !bCheatStateChanged) {
			scbw::refreshConsole();
		}
		else
		{

			char* currentText;
			int ticksCount;

			if (currentNewCheatState < *CHEAT_STATE)
				currentText = (char*)strTbl_006D1220->getString(0x42);
			else
				currentText = (char*)strTbl_006D1220->getString(0x41);

			if (playerName != NULL && *u8_multiplayer_mode != 0) {

				static char buffer[256];

				SC_sprintf_s(buffer, 256, FORMATSTRING_TEXT_SPACE_TEXT, (int)playerName, (int)currentText);
				currentText = buffer;

			}
			//Note: skipping if(currentText == NULL) check because impossible

			SStrCopy((char*)u32_00641598, currentText, 218);
			*u8_00641680 = 4;

			//Things get weird here, maybe it's for some logging
			ticksCount = System_GetTickCount();
			*u32_00640B54 = ticksCount + 7000;

			//skipping a test on negativity because impossible
			ticksCount = *u16_0064096C + 295;

			if (ticksCount > 479)
				ticksCount = 479;

			//skipping more impossible stuff since negativity is still impossible
			ticksCount /= 16;

			ticksCount = (ticksCount * 5) - 85;

			__asm {

				PUSHAD

				MOV ECX, ticksCount
				SHL ECX, 3
				MOV EDX, ECX
				SHR ECX, 2
				MOV EAX, 0x01010101
				MOV EDI, u32_006CF2C8
				REPE STOSD
				MOV ECX, EDX
				AND ECX, 3
				REPE STOSB

				POPAD

			}

			scbw::refreshConsole();

		}

	}

}

;

} //namespace hooks

;

//-------- Helper function definitions. Do NOT modify! --------//

namespace {

const u32 Func_SStrCopy = 0x0041008E;
//no return value confirmed, could be destination string or length copied if exist
void SStrCopy(char* destination, char* source, u32 maxLength) {

	__asm {
		PUSHAD
		PUSH maxLength
		PUSH source
		PUSH destination
		CALL Func_SStrCopy
		POPAD
	}
}

;

const u32 Func_SStrCmpI = 0x00410094;
//Result of 0 means string are identical
s32 SStrCmpI(char* text1, char* text2, u32 maxLength) {

	static s32 result;

	__asm {
		PUSHAD
		PUSH maxLength
		PUSH text2
		PUSH text1
		CALL Func_SStrCmpI
		MOV result, EAX
		POPAD
	}

	return result;

}

;

const u32 Func_SStrLen = 0x004100A0;
u32 SStrLen(char* text) {

	static u32 result;

	__asm {
		PUSHAD
		PUSH text
		CALL Func_SStrLen
		MOV result, EAX
		POPAD
	}

	return result;

}

;

const u32 Func_sprintf_s = 0x0041F1B0;
//Note: buffer_size may be max number of characters * 2, possibly for 16bits text support
void SC_sprintf_s(char* buffer, int buffer_size, const char* format_string, int value1, int value2) {

	__asm {

		PUSHAD

		PUSH value2
		PUSH value1
		PUSH format_string
		MOV ESI, buffer_size
		MOV EDI, buffer

		CALL Func_sprintf_s
		ADD ESP, 0x0C

		POPAD

	}

}

;

const u32 Func_ApplySpeedUpgradeFromUpgradeType = 0x00454540;
void ApplySpeedUpgradeFromUpgradeType(CUnit* unit, u8 upgradeType) {

	__asm {
		PUSHAD
		MOV AL, upgradeType
		PUSH unit
		CALL Func_ApplySpeedUpgradeFromUpgradeType
		POPAD
	}

}

;

const u32 Func_004807F0 = 0x004807F0;
//different initialization, but call BWFXN_Refresh like RefreshLayer5
void function_004807F0() {
	__asm {
		PUSHAD
		CALL Func_004807F0
		POPAD
	}
}

;

const u32 Func_RefreshLayer5 = 0x004BD350;
void RefreshLayer5() {
	__asm {
		PUSHAD
		CALL Func_RefreshLayer5
		POPAD
	}
}

;

const u32 Func_parseCmpgnCheatTypeString = 0x004DBC60;
//Check the 2nd part of the ophelia cheat code
Bool32 parseCmpgnCheatTypeString(char* inputText, char* levelCheatText, u32 unk1, u32 unk2, u32 unk3, char* unk4) {

	static Bool32 result;

	__asm {
		PUSHAD
		PUSH unk4
		PUSH unk3
		PUSH unk2
		PUSH unk1
		PUSH levelCheatText
		MOV ECX, inputText
		CALL Func_parseCmpgnCheatTypeString
		MOV result, EAX
		POPAD
	}

	return result;

}

;

//const u32 Func_004FE0C4 = 0x004FE0C4;
//Based on observation, commented code seemed a reliable conclusion in this context
//But didn't work, so skipped to system function that should be called indirectly
u32 System_GetTickCount() {

#pragma warning( suppress: 28159 )
	return GetTickCount();

	//static u32 result;

	//__asm {
	//	PUSHAD
	//	CALL DWORD PTR [Func_004FE0C4]
	//	MOV result, EAX
	//	POPAD
	//}

	//return result;

}

;

} //Unnamed namespace

//End of helper functions

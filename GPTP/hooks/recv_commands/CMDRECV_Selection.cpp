#include "CMDRECV_Selection.h"
#include <SCBW/api.h>

//Helper functions declaration

namespace {

bool unit_IsStandardAndMovable_Helper(CUnit* unit);								//7B770
s8 function_00496560(u32 playerId);												//96560
void function_004965D0_Helper(u8 selectionGroup, Bool32 unkBoolean);			//965D0
void function_00496940_Helper(u32 bGroupSlot);									//96940
void CreateDashedSelection(CSprite* sprite, u32 alliance_color);				//99A10
bool function_0049A110(u32 playerId);											//9A110
void function_0049A740(u32 playerId);											//9A740
bool function_0049AF80_Helper(CUnit* unit, u32 playerId, u32 selection_slot);	//9AF80
void _CreateDashedSelection(CUnit* unit);										//E65C0

} //unnamed namespace

namespace hooks {

//Function hooked because it is used when recalling group, and
//could exclude some units from a selection, if not modified as
//well as others functions, for normally unallowed selections.
//004965D0
void function_004965D0(u8 selectionGroup, Bool32 unkBoolean) {

	u32* const			CGameStruct_selection_hotkeys	= (u32*)	0x0057FE60; //array of units refs of StoredUnit type stored as u32
	CUnit* const		unitTable_0059CB58				= (CUnit*)	0x0059CB58;	//array of CUnit structures
	
	bool jumpToEnd = false;
	int selectionLength = 0;

	if (unkBoolean == 0) { //adding current selection to selection group

		StoredUnit stored_unit;
		
		stored_unit.fullValue = CGameStruct_selection_hotkeys[0xC * (0x12 * *ACTIVE_PLAYER_ID + selectionGroup) + 0];

		if (stored_unit.fullValue != 0) {

			CUnit* unit = &unitTable_0059CB58[stored_unit.innerValues.index];

			if (
				unit->sprite != NULL &&
				(unit->mainOrderId != OrderId::Die || unit->mainOrderState != 1) &&
				unit->targetOrderSpecial == stored_unit.innerValues.unitId &&
				!unit_IsStandardAndMovable_Helper(unit)
			)
				jumpToEnd = true;

		}

		if (!jumpToEnd) {
			//logically equivalent to original code
			while (selectionLength < SELECTION_ARRAY_LENGTH &&  stored_unit.fullValue != 0) {
				selectionLength++;
				stored_unit.fullValue = CGameStruct_selection_hotkeys[0xC * (0x12 * *ACTIVE_PLAYER_ID + selectionGroup) + selectionLength];
			}
		}

	}
	else //replacing existing selection group content by current selection, so clearing existing one
		for (int i = 0; i < SELECTION_ARRAY_LENGTH; i++)
			CGameStruct_selection_hotkeys[0xC * (0x12 * *ACTIVE_PLAYER_ID + selectionGroup) + i] = 0;

	//966CC
	if (!jumpToEnd) {

		int index = 0;
		CUnit* current_unit;

		do {

			current_unit = playersSelections->unit[*ACTIVE_PLAYER_ID][index];

			if (current_unit == NULL || current_unit->playerId != *ACTIVE_NATION_ID)
				jumpToEnd = true;
			else
			{

				int unitIndex;

				__asm {
					PUSHAD
					MOV ESI, current_unit
					SUB ESI, unitTable
					MOV EAX, 0x86186187
					MUL ESI
					SUB ESI, EDX
					SHR ESI, 1
					ADD ESI, EDX
					SHR ESI, 0x08
					INC ESI
					MOV unitIndex, ESI
					POPAD
				}

				if (unitIndex <= UNIT_ARRAY_LENGTH) {

					StoredUnit current_unitAsStoredUnit;

					current_unitAsStoredUnit.innerValues.unitId = current_unit->targetOrderSpecial;
					current_unitAsStoredUnit.innerValues.index = unitIndex;

					if (current_unitAsStoredUnit.fullValue != 0) {

						bool jump_to_96750;
						bool jump_to_96759;
						bool jump_to_9676F = false;

						jump_to_96759 = (unkBoolean != 0);

						if (!jump_to_96759) { //adding current selection to selection group

							if (selectionLength <= 0) {
								if (selectionLength == 0) {
									jump_to_96759 = true;
									jump_to_96750 = false;
								}
								else
									jump_to_96750 = true;
							}
							else
								jump_to_96750 = false;

						}

						//96740
						if (!jump_to_96759 && !jump_to_96750) {

							int loopCounter = 0;

							while (loopCounter < selectionLength && !jump_to_9676F)
							{
								if (CGameStruct_selection_hotkeys[0xC * (0x12 * *ACTIVE_PLAYER_ID + selectionGroup) + loopCounter * 4] == current_unitAsStoredUnit.fullValue)
									jump_to_9676F = true;
								else
									loopCounter++;
							}

							if (!jump_to_9676F) {
								if (selectionLength == 0)
									jump_to_96759 = true;
								else
									jump_to_96750 = true;
							}

						}

						if (!jump_to_9676F && jump_to_96750) {
							if (!unit_IsStandardAndMovable_Helper(current_unit))
								jump_to_9676F = true;
							else
								jump_to_96759 = true;
						}

						if (!jump_to_9676F && jump_to_96759) {
							CGameStruct_selection_hotkeys[0xC * (0x12 * *ACTIVE_PLAYER_ID + selectionGroup) + selectionLength] = current_unitAsStoredUnit.fullValue;
							if (selectionLength + 1 >= SELECTION_ARRAY_LENGTH)
								jumpToEnd = true;
							else
								selectionLength++;
						}

					}

				}

				//9676F
				if (!jumpToEnd)
					index++;

			}

		}while (!jumpToEnd && index < SELECTION_ARRAY_LENGTH);

	}

}

;

//Function to recall a group saved from a hotkey
//00496940
void function_00496940(u32 bGroupSlot) {

	u16* const			u16_0057EEBC					= (u16*)	0x0057EEBC; //unknown pointer on u16 (or is it StoredUnit?)
	u32* const			CGameStruct_selection_hotkeys	= (u32*)	0x0057FE60; //array of units refs of StoredUnit type stored as u32
	u8* const			CGame_self_alliance_colors		= (u8*)		0x00586D6A; //array of colors of alliance for each player id
	CUnit* const		unitTable_0059CB58				= (CUnit*)	0x0059CB58;	//array of CUnit structures
	u16* const			u16_0063FE2C					= (u16*)	0x0063FE2C; //array of 16 bits values (or StoredUnit values?)
	u16* const			u16_0063FE40					= (u16*)	0x0063FE40; //array of 16 bits values (or StoredUnit values?)

	int nbElems = 0;
	int index;

	//optimized compared to original code
	while (nbElems < SELECTION_ARRAY_LENGTH && CGameStruct_selection_hotkeys[0xC * (0x12 * *ACTIVE_PLAYER_ID + bGroupSlot) + nbElems] != 0)
		nbElems++;

	if (nbElems != 0) {

		index = 0;

		function_0049A740(*ACTIVE_PLAYER_ID);

		//969CC (removed impossible condition here (nbElems <= 0 causing loop skip))

		do {

			StoredUnit stored_unit;
			CUnit* unit;

			stored_unit.fullValue = (u16)CGameStruct_selection_hotkeys[0xC * (0x12 * *ACTIVE_PLAYER_ID + bGroupSlot) + index];

			if (stored_unit.fullValue != 0) {

				unit = &unitTable_0059CB58[stored_unit.innerValues.index];

				if(
					unit->sprite != NULL &&
					(unit->mainOrderId != OrderId::Die || unit->mainOrderState != 1) &&
					stored_unit.innerValues.unitId == unit->targetOrderSpecial &&
					unit->playerId == *ACTIVE_NATION_ID &&
					!(unit->sprite->flags & CSprite_Flags::Hidden) &&
					(unit_IsStandardAndMovable_Helper(unit) || nbElems <= 1)
				)
				{

					if (
						*ACTIVE_PLAYER_ID < 8 &&
						index < SELECTION_ARRAY_LENGTH &&
						(index == 0 || unit_IsStandardAndMovable_Helper(unit))
					)
					{

						playersSelections->unit[*ACTIVE_PLAYER_ID][index] = unit;

						if (function_0049A110(*ACTIVE_PLAYER_ID))
							CreateDashedSelection(unit->sprite, CGame_self_alliance_colors[unit->playerId]);

					}

					index++;
					
				}
				else //96ABB
				{

					StoredUnit old_stored_unit;

					nbElems--;
					old_stored_unit.fullValue = (u16)CGameStruct_selection_hotkeys[0xC * (0x12 * *ACTIVE_PLAYER_ID + bGroupSlot) + nbElems + index];
					CGameStruct_selection_hotkeys[0xC * (0x12 * *ACTIVE_PLAYER_ID + bGroupSlot) + index] = (u32)old_stored_unit.fullValue;
					CGameStruct_selection_hotkeys[0xC * (0x12 * *ACTIVE_PLAYER_ID + bGroupSlot) + nbElems + index] = 0;

				}

			}
			else //96ABB
			{

				StoredUnit old_stored_unit;

				nbElems--;
				old_stored_unit.fullValue = (u16)CGameStruct_selection_hotkeys[0xC * (0x12 * *ACTIVE_PLAYER_ID + bGroupSlot) + nbElems + index];
				CGameStruct_selection_hotkeys[0xC * (0x12 * *ACTIVE_PLAYER_ID + bGroupSlot) + index] = (u32)old_stored_unit.fullValue;
				CGameStruct_selection_hotkeys[0xC * (0x12 * *ACTIVE_PLAYER_ID + bGroupSlot) + nbElems + index] = 0;

			}
			

		} while (index < nbElems);

		//96AEC
		if (bGroupSlot >= 10) {
			u16_0063FE2C[8 * *ACTIVE_PLAYER_ID + bGroupSlot] = *u16_0057EEBC;
		}
		else
		{
			u8 someValue = function_00496560(*ACTIVE_PLAYER_ID);
			function_004965D0_Helper(someValue + 10, 1);
			u16_0063FE40[8 * *ACTIVE_PLAYER_ID + someValue] = *u16_0057EEBC;
		}

	}

}

;

//Function hooked because it is used in CMDRECV_Select, and
//could exclude some units from a selection, if not modified as
//well as others functions, for normally unallowed selections.
//0049AF80
bool function_0049AF80(CUnit* unit, u32 playerId, u32 selection_slot) {

	bool return_value;

	if (
		unit != NULL &&
		playerId < PLAYABLE_PLAYER_COUNT &&
		selection_slot < SELECTION_ARRAY_LENGTH &&
		!(unit->sprite->flags & CSprite_Flags::Hidden) &&
		(
			selection_slot <= 0 ||
			(unit_IsStandardAndMovable_Helper(unit) && unit->playerId == *ACTIVE_NATION_ID)
		)
	)
	{
		playersSelections->unit[playerId][selection_slot] = unit;
		if (function_0049A110(playerId))
			_CreateDashedSelection(unit);
		return_value = true;
	}
	else
		return_value = false;

	return return_value;

}

;

//004C2560
void CMDRECV_ShiftSelect(u8 packetId, s8 bCount, StoredUnit* selectedCommandUnits) {

	u16* const		u16_0057EEBC		= (u16*)	0x0057EEBC; //unknown pointer on u16 (or is it StoredUnit?)
	CUnit* const	unitTable_0059CB58	= (CUnit*)	0x0059CB58;	//array of CUnit structures
	u16* const		u16_0063FE40		= (u16*)	0x0063FE40; //array of 16 bits values (or StoredUnit values?)
	u16* const		u16_0063FE50		= (u16*)	0x0063FE50; //very unknown array

	if (bCount <= SELECTION_ARRAY_LENGTH) {

		int index = 0;

		//optimized compared to original code
		while (index < SELECTION_ARRAY_LENGTH && playersSelections->unit[*ACTIVE_PLAYER_ID][index] != NULL)
			index++;

		if (index + bCount <= SELECTION_ARRAY_LENGTH) {

			if(bCount > 0) //C2601
				for (int i = 0; i < bCount; i++) {

					StoredUnit current_stored_unit = selectedCommandUnits[i];

					if (current_stored_unit.fullValue != 0) {

						CUnit* unit = &unitTable_0059CB58[current_stored_unit.innerValues.index];

						if (
							unit->sprite != NULL &&
							(unit->mainOrderId != OrderId::Die || unit->mainOrderState != 1) &&
							unit->targetOrderSpecial == current_stored_unit.innerValues.unitId
						) 
						{

							int counter = 0;

							//equivalent to original code
							//seek if "unit" is already in selection, and
							//skip things if it is
							do {

								CUnit* unit_checked = playersSelections->unit[*ACTIVE_PLAYER_ID][counter];

								if (unit_checked == NULL)
									counter = SELECTION_ARRAY_LENGTH;
								else
								if(unit_checked == unit)
									counter = SELECTION_ARRAY_LENGTH + 1;
								else
									counter++;

							} while (counter < SELECTION_ARRAY_LENGTH);

							if (
								counter == SELECTION_ARRAY_LENGTH &&
								*ACTIVE_PLAYER_ID < PLAYABLE_PLAYER_COUNT && 
								index < SELECTION_ARRAY_LENGTH &&
								!(unit->sprite->flags & CSprite_Flags::Hidden)
							) 
							{

								if (
									index <= 0 ||
									(unit_IsStandardAndMovable_Helper(unit) && unit->playerId == *ACTIVE_NATION_ID)
								)
								{ //C26AB

									playersSelections->unit[*ACTIVE_PLAYER_ID][index] = unit;

									if (function_0049A110(*ACTIVE_PLAYER_ID))
										_CreateDashedSelection(unit);

									index++;

								}

							}

						}

					}

				}

			//C26ED
			if (index > 1) {

				int countdown = 8;
				int altcount = -1;
				int unknownValue = 0x0000FFFF;
				u16* unknownArrayItemOffset = &u16_0063FE50[*ACTIVE_PLAYER_ID * 8];

				do {

					countdown--;
					unknownArrayItemOffset--;

					if (*unknownArrayItemOffset < unknownValue) {
						unknownValue = *unknownArrayItemOffset;
						altcount = countdown;
					}

				} while (countdown != 0);

				function_004965D0_Helper(altcount + 10, 1);
				u16_0063FE40[8 * *ACTIVE_PLAYER_ID + altcount] = *u16_0057EEBC;

			}

		}

	}

}

;

//004C2750
void CMDRECV_Select(u8 packetId, s8 bCount, StoredUnit* selectedCommandUnits) {

	u16* const			u16_0057EEBC					= (u16*)	0x0057EEBC; //unknown pointer on u16 (or is it StoredUnit?)
	u32* const			CGameStruct_selection_hotkeys	= (u32*)	0x0057FE60; //array of units refs of StoredUnit type stored as u32
	CUnit* const		unitTable_0059CB58				= (CUnit*)	0x0059CB58;	//array of CUnit structures
	u16* const			u16_0063FE40					= (u16*)	0x0063FE40; //array of 16 bits values (or StoredUnit values?)

	if (bCount <= SELECTION_ARRAY_LENGTH) {

		function_0049A740(*ACTIVE_PLAYER_ID);

		if (bCount > 0) {

			int index = 0;
			int selection_length = 0;
			StoredUnit stored_unit;
			CUnit* unit;

			do {

				stored_unit = selectedCommandUnits[index];

				if (stored_unit.fullValue != 0) {

					unit = &unitTable_0059CB58[stored_unit.innerValues.index];

					if (
						unit->sprite != NULL &&
						(unit->mainOrderId != OrderId::Die || unit->mainOrderState != 1) &&
						stored_unit.innerValues.unitId == unit->targetOrderSpecial
					)
					{

						int local_index = 0;

						//equivalent to original code
						//seek if "unit" is already in selection, to
						//skip things if it is
						do {

							CUnit* unit_checked = playersSelections->unit[*ACTIVE_PLAYER_ID][local_index];

							if (unit_checked == NULL)
								local_index = SELECTION_ARRAY_LENGTH;
							else
							if(unit_checked == unit)
								local_index = SELECTION_ARRAY_LENGTH + 1;
							else
								local_index++;

						} while (local_index < SELECTION_ARRAY_LENGTH);

						if (
							local_index == SELECTION_ARRAY_LENGTH &&
							unit->id != UnitId::TerranNuclearMissile &&
							function_0049AF80_Helper(unit,*ACTIVE_PLAYER_ID,selection_length)
						)
							selection_length++;


					}

				}

				index++;


			} while (index < bCount);

			if (selection_length > 1) {
				u8 someValue = function_00496560(*ACTIVE_PLAYER_ID);
				function_004965D0_Helper(someValue + 10, 1);
				u16_0063FE40[8 * *ACTIVE_PLAYER_ID + someValue] = *u16_0057EEBC;
			}

		}

	}

}

;

//004C2870
void CMDRECV_Hotkey(u8 packetId, u8 bGroupType, u8 bGroupSlot) {
	if (bGroupSlot <= 18) {
		if (bGroupType == 0)
			function_004965D0_Helper(bGroupSlot, 1);		//save
		else
		if(bGroupType == 1)
			function_00496940_Helper(bGroupSlot);	//recall
		else
		if(bGroupType == 2)
			function_004965D0_Helper(bGroupSlot, 0);		//add
	}
}

;

} //namespace hooks

//-------- Helper function definitions. Do NOT modify! --------//

namespace {

const u32 Func_Unit_IsStandardAndMovable = 0x0047B770;
bool unit_IsStandardAndMovable_Helper(CUnit* unit) {

	static Bool32 return_value_unconverted;

	__asm {
		PUSHAD
		MOV ECX, unit
		CALL Func_Unit_IsStandardAndMovable
		MOV return_value_unconverted, EAX
		POPAD
	}

	return (return_value_unconverted != 0);

}

;

const u32 Func_Sub496560 = 0x00496560;
//function searching for something and returning a value
//accordingly
s8 function_00496560(u32 playerId) {

	static s8 return_value;

	__asm {
		PUSHAD
		MOV ECX, playerId
		CALL Func_Sub496560
		MOV return_value, AL
		POPAD
	}

	return return_value;

}

;

const u32 Func_Sub4965D0 = 0x004965D0;
//Function manipulating the unit array and using
//unit_IsStandardAndMovable
void function_004965D0_Helper(u8 selectionGroup, Bool32 unkBoolean) {
	__asm {
		PUSHAD
		PUSH unkBoolean
		MOV AL, selectionGroup
		CALL Func_Sub4965D0
		POPAD
	}
}

;

const u32 Func_Sub496940 = 0x00496940;
//hooked here, function to recall a group from hotkey
void function_00496940_Helper(u32 bGroupSlot) {
	__asm {
		PUSHAD
		PUSH bGroupSlot
		CALL Func_Sub496940
		POPAD
	}
}

;

const u32 Func_CreateDashedSelection = 0x00499A10;
void CreateDashedSelection(CSprite* sprite, u32 alliance_color) {
	__asm {
		PUSHAD
		MOV ESI, sprite
		PUSH alliance_color
		CALL Func_CreateDashedSelection
		POPAD
	}
}

;

const u32 Func_Sub49A110 = 0x0049A110;
//perform checks related to alliances
bool function_0049A110(u32 playerId) {

	static Bool32 return_value_holder;

	__asm {
		PUSHAD
		MOV EAX, playerId
		CALL Func_Sub49A110
		MOV return_value_holder, EAX
		POPAD
	}

	return (return_value_holder != 0);

}

;

const u32 Func_Sub49A740 = 0x0049A740;
//may remove selection circles
void function_0049A740(u32 playerId) {
	__asm {
		PUSHAD
		MOV EAX, playerId
		CALL Func_Sub49A740
		POPAD
	}
}

;

const u32 Func_Sub49AF80 = 0x0049AF80;
//may visually add to selection and return true,
//else return false...?
bool function_0049AF80_Helper(CUnit* unit, u32 playerId, u32 selection_slot) {

	static Bool32 return_value_holder;

	__asm {
		PUSHAD
		MOV ESI, unit
		MOV EDI, playerId
		MOV EBX, selection_slot
		CALL Func_Sub49AF80
		MOV return_value_holder, EAX
		POPAD
	}

	return (return_value_holder != 0);

}

;

const u32 Func__CreateDashedSelection = 0x004E65C0;
void _CreateDashedSelection(CUnit* unit) {
	__asm {
		PUSHAD
		MOV EAX, unit
		CALL Func__CreateDashedSelection
		POPAD
	}
}

;

} //unnamed namespace

//End of helper functions

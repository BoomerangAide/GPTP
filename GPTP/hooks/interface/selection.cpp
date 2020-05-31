#include "selection.h"
#include <SCBW/api.h>

#pragma warning( disable: 6386 )

//Helper functions declaration
namespace {

	bool unitIsOwnedByCurrentPlayer(CUnit* unit); 																	//01170
	bool isUnitBurrowed(CUnit* unit);																				//02A70
	void SC_memcpy_0(u32 dest, u32 src, u32 memsize);																//08FD0
	CUnit** getAllUnitsInBounds(Box16* coords);																		//2FF80
	CUnit** FindAllUnits(Box16* coords);																			//308A0	
	void function_00468670(CUnit* unit);																			//68670
	bool unit_isUnselectable(u16 unitId);																			//6ED80
	void function_0046F040_Helper(CUnit* current_unit, CUnit** unit_list, CUnit* clicked_unit, u32 list_length);	//6F040
	u32 SortAllUnits_Helper(CUnit* unit, CUnit** unit_list, CUnit** units_in_bounds);								//6F0F0
	u32 combineLists_Sub_6F290(CUnit* unit,CUnit** unit_list_1,CUnit** unit_list_2,u32 list_length);				//6F290
	CUnit* function_0046F3A0(u32 unk1, u32 unk2);																	//6F3A0
	void applyNewSelect_Sub_6FA00(CUnit** unit_list,u32 unit_list_length);											//6FA00
	u32 CUnitToUnitID(CUnit* unit);																					//7B1D0
	bool unit_IsStandardAndMovable(CUnit* unit); 																	//7B770
	void CancelTargetOrder();																						//8CA10
	void refreshLayer3And4();																						//8D9A0
	void function_0048E310();																						//8E310
	void selectBuildingSFX(CUnit* unit);																			//8F910
	Bool32 selectSingleUnitFromID(u32 unitIndex);																	//96D30
	Bool32 compareUnitRank(CUnit* current_unit, CUnit* other_unit);													//9A350
	void CreateNewUnitSelectionsFromList(CUnit** unit_list, u32 unit_list_length);									//9AE40
	void selectMultipleUnitsFromUnitList(CUnit** unitList, u32 unitsCount, Bool32 unk1, Bool32 unk2);				//9AEF0
	void CMDACT_HotkeyUnit(u8 selectionGroupNumber, u32 params_length, CUnit** selection_array, u32 array_length);	//C07B0
	void CMDACT_Select(CUnit** unit_list, u32 unit_list_length); 													//C0860

} //unnamed namespace

namespace hooks {

	///
	/// Function used by Ctrl+Click (select all units of same type) and
	/// selection by drawing a box holding click and dragging the mouse
	///
	u32 SortAllUnits(CUnit* unit,CUnit** unit_list,CUnit** units_in_bounds) {

		const u32* VISIBILITY_CHECK_0057F0B0 = (u32*)0x0057F0B0;
		
		CUnit* current_unit;
		CUnit* backup_current_unit;
		u32 current_index_in_unit_list;

		bool bStopBigLoop = false;

		bool bKeepForEmptyListCase = false;
		bool bDontAddToList = false;

		backup_current_unit = NULL;

		//if unit not null, become first entry of unit_list
		if(unit != NULL) {
			unit_list[0] = unit;
			current_index_in_unit_list = 1;
		}
		else
			current_index_in_unit_list = 0;

		//work with the first unit from units_in_bounds
		current_unit = *units_in_bounds;

		if(current_unit == NULL)
			bStopBigLoop = true;

		while(!bStopBigLoop) {

			if(current_unit != unit) {

				 //if current_unit==unit, then it's already in
				//unit_list

				if(current_unit->subunit != NULL) {

					//if what got selected is the turret or another subunit, swap
					//the unit and the subunit as current_unit
					if(units_dat::BaseProperty[current_unit->id] & UnitProperty::Subunit)
						current_unit = current_unit->subunit;

					if(current_index_in_unit_list != 0) {

						//6F150

						u32 index = current_index_in_unit_list;

						do {

							index--;

							//if the unit is already in the list, skip it
							if(unit_list[index] == current_unit)
								bDontAddToList = true;

						}while (index != 0 && !bDontAddToList);

					}

				} //if(current_unit->subunit != NULL)

				if(!bDontAddToList) {

					//6F161
					
					//if the unit sprite is visible, perform additionnal visibility test,
					//else skip the unit
					if(current_unit->sprite->visibilityFlags & (u8)*VISIBILITY_CHECK_0057F0B0) {

						//if the unit is cloaked and visibility status test (detection by detectors
						//units probably) fail, the unit is skipped
						if(current_unit->status & (UnitStatus::Cloaked + UnitStatus::RequiresDetection))
							if( !(current_unit->visibilityStatus & *VISIBILITY_CHECK_0057F0B0) )
								bDontAddToList = true;

					}
					else
						bDontAddToList = true;

					if(!bDontAddToList) {

						//6F18C

						if(unit_isUnselectable(current_unit->id))
							bDontAddToList = true;
						else
						if(!unit_IsStandardAndMovable(current_unit))
							bKeepForEmptyListCase = true; //skip the unit for the list, unless it's the only available
						else 
						if(*IS_IN_REPLAY)
							bKeepForEmptyListCase = true; //skip the unit for the list, unless it's the only available
						else
						if(current_unit->playerId != *LOCAL_NATION_ID)
							bKeepForEmptyListCase = true; //skip the unit for the list, unless it's the only available
						else
						{

							//6F1C5

							if(unit != NULL) {

								//all units should belong to the same player (the one owning the clicked unit)
								if(unit->playerId != current_unit->playerId)
									bDontAddToList = true;
								else
								//select only units with same Id
								if(unit->id != current_unit->id)
									bDontAddToList = true;
								else {

									//6F1D9

									//equivalent to " unit->status XOR current_unit->status "
									//keep the flags appearing in only 1 of the variables
									u32 mixed_flags =
										( ~(unit->status) & current_unit->status ) | 
										( unit->status & ~(current_unit->status) )
										;

									//Only select all burrowed OR all unburrowed
									//units.The test exclude current_unit if the
									//burrowed state differ from the one of unit.
									if( mixed_flags & UnitStatus::Burrowed )
										bDontAddToList = true;
									else {

										//6F1E8
										if( !isUnitBurrowed(current_unit) ) {

											//Only select all detectables OR all undetectables
											//units.The test exclude current_unit if the
											//detectable state differ from the one of unit.
											if(mixed_flags & UnitStatus::RequiresDetection)
												bDontAddToList = true;

										}

										if(!bDontAddToList) {

											//6F1F8

											//Only select hallucinations OR non-hallucinations
											//units.The test exclude current_unit if the
											//hallucination state differ from the one of unit.
											if(mixed_flags & UnitStatus::IsHallucination)
												bDontAddToList = true;

										}

									}

								}

							} //if(unit != NULL)

							if(!bDontAddToList) {

								//6F203

								//0x0046F208 (use of SELECTION_ARRAY_LENGTH)
								if(current_index_in_unit_list >= SELECTION_ARRAY_LENGTH) //action when unit_list is full
									function_0046F040_Helper(current_unit, unit_list, unit, current_index_in_unit_list);
								else {
									//6F217
									//Add current_unit to unit_list
									unit_list[current_index_in_unit_list] = current_unit;
									current_index_in_unit_list++;
								}

							}

						} //several ifs

					} //if(!bDontAddToList) => 6F18C

				} //if(!bDontAddToList) => 6F161

			} //if(current_unit != unit)

			if(bKeepForEmptyListCase)
				backup_current_unit = current_unit;

			//6F226

			//select next unit in bounds
			units_in_bounds++;				//this actually advance by 4 bytes
			current_unit = *units_in_bounds;

			bKeepForEmptyListCase = false;
			bDontAddToList = false;

			//continue until current_unit is NULL
			if(current_unit == NULL)
				bStopBigLoop = true;

		} //loop back at 6F237

		//6F242

		if(current_index_in_unit_list == 0) {

			//if current_index_in_unit_list == 0, the list is empty
			//and cannot be used, so another solution is found here.

			if(backup_current_unit == NULL) {

				//6F24E

				if(unit == NULL)
					current_index_in_unit_list = 0; //return 0 (no list)
				else {
					//6F252
					unit_list[0] = unit;
					current_index_in_unit_list = 1; //return 1 (the clicked unit alone)
				}

			} //if(backup_current_unit == NULL)
			else {

				//6F264

				if(unit != NULL) {
					unit_list[0] = unit;
					current_index_in_unit_list = 1; //return 1 (the clicked unit alone)
				}
				else {
					unit_list[0] = backup_current_unit;
					current_index_in_unit_list = 1; //return 1 (an unit preserved by bKeepForEmptyListCase)
				}

			}

		}

		return current_index_in_unit_list;

	} //u32 SortAllUnits(CUnit* unit,CUnit** unit_list,CUnit* units_in_bounds)

	;

	///
	/// Function that may be required to mod selection, as it is using unit_IsStandardAndMovable
	/// that is a likely blocker for buildings and other special selections
	///
	u32 combineSelectionsLists(CUnit* unit, CUnit** unit_list_1, CUnit** unit_list_2, u32 list_length) {

		CUnit* current_unit;
		int list2_length;
		u32 return_value;
		bool bEndThere = false;

		list2_length = 0;

		if (unit_list_2[0] != NULL) {

			do {
				current_unit = unit_list_2[list2_length+1];
				list2_length++;
			} while (current_unit != NULL);

			if (list2_length >= SELECTION_ARRAY_LENGTH) {
				return_value = SELECTION_ARRAY_LENGTH;
				bEndThere = true;
			}
			else
			if(list2_length != 0) {

				if (
					!unit_IsStandardAndMovable(unit_list_1[0]) ||
					*IS_IN_REPLAY != 0 ||
					unit_list_1[0]->playerId != *LOCAL_NATION_ID ||
					!unit_IsStandardAndMovable(unit_list_2[0]) ||
					unit_list_2[0]->playerId != *LOCAL_NATION_ID
				)
				{
					return_value = list2_length;
					bEndThere = true;
				}

			}

			//!bEndThere => 6F307

		}

		if (!bEndThere) {

			if (list_length <= 0)
				return_value = list2_length;
			else
			{

				return_value = list2_length;

				for (int counter = 0; list_length > 0; counter++) {

					bool bUnitAlreadyInList = false;

					current_unit = unit_list_1[counter];

					if (list2_length != 0) {

						int i = list2_length;

						do {

							i--;

							if (unit_list_2[i] == current_unit)
								bUnitAlreadyInList = true;

						} while (i != 0 && !bUnitAlreadyInList);

					}

					//6F33A
					if (!bUnitAlreadyInList) {

						if (return_value >= SELECTION_ARRAY_LENGTH)
							function_0046F040_Helper(current_unit, &unit_list_2[return_value], unit, counter);
						else { //6F356
							unit_list_2[return_value] = current_unit;
							return_value++;
						}

					}

					list_length--;

				}

			}
		}

		return return_value;

	}

	;

	///
	/// Select units within the rectangle drawn by the mouse
	///
	void getSelectedUnitsInBox(Box16* coords) {

		static CUnit* local_array_1[SELECTION_ARRAY_LENGTH];
		static CUnit* local_array_2[SELECTION_ARRAY_LENGTH];
		u32 someResult;

		for (int i = 0; i < SELECTION_ARRAY_LENGTH; i++)
			local_array_1[i] = NULL;

		someResult = SortAllUnits_Helper(NULL, local_array_1, FindAllUnits(coords));

		*tempUnitsListCurrentArrayCount = tempUnitsListArraysCountsListLastIndex[*tempUnitsListArraysCountsListLastIndex];
		*tempUnitsListArraysCountsListLastIndex = *tempUnitsListArraysCountsListLastIndex - 1;

		if (someResult != 0) {

			if (*IS_HOLDING_SHIFT && activePlayerSelection->unit[0] != NULL)
			{
				for (int i = 0; i < SELECTION_ARRAY_LENGTH; i++)
					local_array_2[i] = activePlayerSelection->unit[i];
				someResult = combineLists_Sub_6F290(NULL, local_array_1, local_array_2, someResult);
				applyNewSelect_Sub_6FA00(local_array_2, someResult);
			}
			else
			{

				bool bEndThere = false;

				if (
					someResult != 1 ||
					*IS_HOLDING_ALT == 0 ||
					selectSingleUnitFromID(CUnitToUnitID(local_array_1[0])) == 0
				)
				{
					selectMultipleUnitsFromUnitList(local_array_1, someResult, 1, 1);
					bEndThere = (someResult != 1);
				}

				if (
					!bEndThere &&
					local_array_1[0] != NULL &&
					local_array_1[0]->playerId == *ACTIVE_NATION_ID &&
					local_array_1[0]->isFactory() //replace call to 7B2E0 unitIsFactoryUnit
				)
					function_00468670(local_array_1[0]); //this is what make the rally point briefly appear

			}

		}

	}

	;

	///
	/// Function used by Click on unit (possibly with key pressed)
	///
	void getSelectedUnitsAtPoint(u32 unk1, u32 unk2) {

		Bool32* const IS_DOUBLE_CLICKING =	(Bool32*)	0x0066FF58;

		Bool8* const bCanUpdateSelectedUnitData		= (Bool8*)	0x0059723C;
		Bool8* const bCanUpdateSelectedUnitPortrait	= (Bool8*)	0x0068AC74;
		Bool32* const bCanUpdateCurrentButtonSet	= (Bool32*)	0x0068C1B0;
		BinDlg** const someDialogUnknown			= (BinDlg**)0x0068C1E8; //related to MouseOver?
		Bool8* const bCanUpdateStatDataDialog		= (Bool8*)	0x0068C1F8;
		BinDlg** const someDialogUnknownUser		= (BinDlg**)0x0068C1EC; //related to MouseOver? Usually someDialogUnknown->user if not 0 or -1

		CUnit* local_temp_array_1[SELECTION_ARRAY_LENGTH];	//used instead of an array starting from [EBP-3C]
		CUnit* local_temp_array_2[SELECTION_ARRAY_LENGTH];	//used instead of an array starting from [EBP-6C]

		Box16 local_temp_box16_structure;	//used instead of using from [EBP-0C] to [EBP-06]

		CUnit* clicked_unit = function_0046F3A0(unk1, unk2);

		if (clicked_unit != NULL)
		{

		bool isHoldingCtrl_OR_isDoubleClickingSelectedClickedUnit = 
			*IS_HOLDING_CTRL || 
			( *IS_DOUBLE_CLICKING && (clicked_unit->sprite->flags & CSprite_Flags::Selected) );

		for(int i = 0; i < SELECTION_ARRAY_LENGTH; i++)
			local_temp_array_1[i] = NULL;

		if(
			isHoldingCtrl_OR_isDoubleClickingSelectedClickedUnit ||
				(*IS_HOLDING_SHIFT != 0 && (activePlayerSelection->unit[0] != NULL))
			) 
		{

				//6FC59

				if (*IS_HOLDING_SHIFT == 0) {

				//6FE07

				if(isHoldingCtrl_OR_isDoubleClickingSelectedClickedUnit) {

					//Ctrl+click on unit or double-click on already
					//selected unit without other key
					// =>	select surrounding units with same type as 
					//		clicked unit, making them the current 
					//		selection

					CUnit** units_in_bounds;
					u32 sorted_list_length;

					//locate surrounding units for selection
					local_temp_box16_structure.left = *MoveToX;
					local_temp_box16_structure.top = *MoveToY;
					local_temp_box16_structure.right = *MoveToX + 640;
					local_temp_box16_structure.bottom = *MoveToY + 400;

					units_in_bounds = getAllUnitsInBounds(&local_temp_box16_structure);
						sorted_list_length = SortAllUnits_Helper(clicked_unit, local_temp_array_1, units_in_bounds);

					//reload the previous temporary unit list from before the call to getAllUnitsInBounds
						*tempUnitsListCurrentArrayCount = tempUnitsListArraysCountsListLastIndex[*tempUnitsListArraysCountsListLastIndex];
					*tempUnitsListArraysCountsListLastIndex = *tempUnitsListArraysCountsListLastIndex - 1;

					if(sorted_list_length != 0) {

						//turn the local array as the newly applied selection
						applyNewSelect_Sub_6FA00(local_temp_array_1,sorted_list_length);

						//this is what make the rally point briefly appear when
						//selecting a building with one.
						//maybe can do others things?
						if(
							sorted_list_length == 1 &&
							clicked_unit->playerId == *ACTIVE_NATION_ID &&
							clicked_unit->isFactory()
							) 
							function_00468670(clicked_unit);

					}

				} //if(isHoldingCtrl_OR_isDoubleClickingSelectedClickedUnit)

			} //if( !*IS_HOLDING_SHIFT )
			else {

					//6FC64

				//holding SHIFT (if not holding CTRL, then there's an active selection too)

				if(isHoldingCtrl_OR_isDoubleClickingSelectedClickedUnit) {

					//{Ctrl+Shift+click on unit} OR {Shift+double-click on 
					//selected unit WHILE having an active selection}
					// =>	select units same type as clicked unit, adding them to the
					//		active selection

					CUnit** units_in_bounds;
					u32 sorted_list_length;

					//prepare to locate surrounding units for selection
					local_temp_box16_structure.left = *MoveToX;
					local_temp_box16_structure.top = *MoveToY;
					local_temp_box16_structure.right = *MoveToX + 640;
					local_temp_box16_structure.bottom = *MoveToY + 400;

					//save existing selection to a temporary array
					for(int i = 0; i < SELECTION_ARRAY_LENGTH; i++)
						local_temp_array_2[i] = activePlayerSelection->unit[i];

					//locate surrounding units for selection
					units_in_bounds = getAllUnitsInBounds(&local_temp_box16_structure);
						sorted_list_length = SortAllUnits_Helper(clicked_unit, local_temp_array_1, units_in_bounds);

					//reload the previous temporary unit list from before the call to getAllUnitsInBounds
						*tempUnitsListCurrentArrayCount = tempUnitsListArraysCountsListLastIndex[*tempUnitsListArraysCountsListLastIndex];
					*tempUnitsListArraysCountsListLastIndex = *tempUnitsListArraysCountsListLastIndex - 1;

					if(sorted_list_length != 0) {

						//6FCD4

						//recombine current selection and newly selectable items in local_temp_array_2
						u32 list_remixed_length = combineLists_Sub_6F290(clicked_unit,local_temp_array_1,local_temp_array_2,sorted_list_length);

						//turn local_temp_array_2 as the newly applied selection
						applyNewSelect_Sub_6FA00(local_temp_array_2,list_remixed_length);

					}

				} //if(isHoldingCtrl_OR_isDoubleClickingSelectedClickedUnit)
				else {

					//6FCF6

					//Shift + click while there's an existing selection 
					//(because hold on CTRL case was already dealt with
					//so there's an existing selection at this point)
					// => add/remove unit from selection

					int arrayIndex;

					//copy the existing selection into the local array
					for(arrayIndex = 0 ; arrayIndex < SELECTION_ARRAY_LENGTH && activePlayerSelection->unit[arrayIndex] != NULL; arrayIndex++)
						local_temp_array_1[arrayIndex] = activePlayerSelection->unit[arrayIndex];

					if( !(clicked_unit->sprite->flags & CSprite_Flags::Selected) ) {	
						
							//6FD18
						//unit not selected, so it's added to current selection if valid

						if(
							arrayIndex < SELECTION_ARRAY_LENGTH && 
							unit_IsStandardAndMovable(local_temp_array_1[0]) && 
							unitIsOwnedByCurrentPlayer(local_temp_array_1[0]) &&
							unit_IsStandardAndMovable(clicked_unit) && 
							unitIsOwnedByCurrentPlayer(clicked_unit)
							)
						{

							//new unit is added at end of array, and the index
							//is increased to be used as new array length
							local_temp_array_1[arrayIndex] = clicked_unit;
							arrayIndex++;

							//turn local_temp_array_1 as the newly applied selection
							applyNewSelect_Sub_6FA00(local_temp_array_1,arrayIndex);

						}

					}
					else {

						//6FD77

						//unit already selected, remove it from selection

						u32 memcpy_size;
						bool bUpdateSelection = true;

						//decrease it so it's equal to the index of last element
						arrayIndex--;

						//each element is 4 bytes, multiply it by the number of elements
						//after the one to remove
						memcpy_size = (arrayIndex - clicked_unit->sprite->selectionIndex) * 4;

						//Copy the elements on the right of the one to delete one step left
						//to overwrite the one to delete
						SC_memcpy_0(
							(u32)&local_temp_array_1[clicked_unit->sprite->selectionIndex], 
							(u32)&local_temp_array_1[clicked_unit->sprite->selectionIndex+1], 
							memcpy_size
						);

						//either erase the element to delete or one that was repeated
						//after memcpy
						local_temp_array_1[arrayIndex] = NULL;
						
						if(arrayIndex == 1) {

							if(*IS_HOLDING_ALT) {

								//It seems you can unselect things with Shift+Alt like
								//with shift alone, UNTIL you have 2 units selected.
								//It is the code below that is causing the
								//deselection to fail where simple shift+click would
								//continue to deselect.

								u32 unitIndex = CUnitToUnitID(local_temp_array_1[0]);

								if(selectSingleUnitFromID(unitIndex) != 0)
									bUpdateSelection = false;

							}

						}

						if(bUpdateSelection) {

							//Create a new unit selection (since the clicked unit is unselected)
							//and choose it as current selection)
							CreateNewUnitSelectionsFromList(local_temp_array_1,arrayIndex);
							CMDACT_Select(local_temp_array_1,arrayIndex);

							//tell the GUI to update stuff
							*someDialogUnknown = NULL;
							*someDialogUnknownUser = NULL;
							*bCanUpdateSelectedUnitData = 1;
							*bCanUpdateCurrentButtonSet = 1;
							*bCanUpdateSelectedUnitPortrait = 1;
							*bCanUpdateStatDataDialog = 1;

						}

					} //if (clicked_unit->sprite->flags & CSprite_Flags::Selected)

				} //if(!isHoldingCtrl_OR_isDoubleClickingSelectedClickedUnit)

			} //if(*IS_HOLDING_SHIFT)

		} //using double clicks, shift, ctrl...

		else {

			//6FBB7

			//Simple click...almost (there's still ALT key)

			bool bRecreatedSelectionThroughAltClick = false;

				if (*IS_HOLDING_ALT != 0) {

				//Recall the selection the unit belong to if there's one
				//In this case, skip the code for a single unit handling

				u32 temp_unit_index = CUnitToUnitID(clicked_unit);

				if(selectSingleUnitFromID(temp_unit_index) != 0)
					bRecreatedSelectionThroughAltClick = true;

			}

			if(!bRecreatedSelectionThroughAltClick) {

				//6FBD1

				//Create a new selection containing only the clicked unit
				CreateNewUnitSelectionsFromList(&clicked_unit,1);

					if(compareUnitRank(clicked_unit,NULL) == 0)
						selectBuildingSFX(NULL);
					else
				//play the sound of the unit (don't need to be a building)
				selectBuildingSFX(clicked_unit);

				//select the unit
				CMDACT_Select(&clicked_unit,1);

				//tell the GUI to update stuff
				*bCanUpdateSelectedUnitData = 1;
				*bCanUpdateCurrentButtonSet = 1;
				*bCanUpdateSelectedUnitPortrait = 1;
				*bCanUpdateStatDataDialog = 1;
				*someDialogUnknown = NULL;
				*someDialogUnknownUser = NULL;

			}

			//6FC2C

			//this is what make the rally point briefly appear when
			//selecting a building with one.
			//maybe can do others things?
				if (
					clicked_unit->playerId == *ACTIVE_NATION_ID &&
				clicked_unit->isFactory()
				)
				function_00468670(clicked_unit);

		}

	}

	}

	;

	//centerviewUnitGroup is not hooked due to complexity, but if selection
	//is modded to allow multiple buildings or other special things through
	//changes on unit_IsStandardAndMovable, then you can modify an entry in
	//selection_inject.cpp to use a custom unit_IsStandardAndMovable
	//function and allow it to use all the selection content properly.

	;

	///
	/// Function used to select units from a group
	/// Needed in addition to the CMDRECV functions due to a
	/// call to unit_IsStandardAndMovable that could cause some
	/// units to be filtered out if that function is only modded
	/// elsewhere.
	///
	void selectUnitGroup(u32 selectionGroupNumber) {

		u32* const			CGameStruct_selection_hotkeys	= (u32*)	0x0057FE60; //array of units refs of StoredUnit type stored as u32
		Bool8* const		bCanUpdateSelectedUnitData		= (Bool8*)	0x0059723C;
		CUnit* const		unitTable_0059CB58				= (CUnit*)	0x0059CB58;	//array of CUnit structures
		Bool8* const		bDoingCancellableTargetOrder	= (Bool8*)	0x00641694;
		Bool8* const		bCanUpdateSelectedUnitPortrait	= (Bool8*)	0x0068AC74;
		Bool32* const		bCanUpdateCurrentButtonSet		= (Bool32*)	0x0068C1B0;
		BinDlg** const		someDialogUnknown				= (BinDlg**)0x0068C1E8;
		BinDlg** const		someDialogUnknownUser			= (BinDlg**)0x0068C1EC;
		Bool8* const		bCanUpdateStatDataDialog		= (Bool8*)	0x0068C1F8;

		int i;
		u32 selectionLength = 0;
		StoredUnit current_stored_unit;
		CUnit* current_unit;
		CUnit* temp_selection_array[SELECTION_ARRAY_LENGTH];
		u32* CGameStruct_selection_hotkeys_current = &CGameStruct_selection_hotkeys[0xC * (0x12 * *LOCAL_HUMAN_ID + selectionGroupNumber)];

		current_stored_unit.fullValue = CGameStruct_selection_hotkeys_current[selectionLength];

		while (selectionLength < SELECTION_ARRAY_LENGTH && current_stored_unit.fullValue != 0) {
			selectionLength++;
			current_stored_unit.fullValue = CGameStruct_selection_hotkeys_current[selectionLength];
		}

		for (i = 0; i < SELECTION_ARRAY_LENGTH; i++)
			temp_selection_array[i] = NULL;

		if (selectionLength > 0) {

			CUnit* validated_unit;
			int index = 0;
			i = 0;

			do {

				current_stored_unit.fullValue = CGameStruct_selection_hotkeys_current[i];

				if (current_stored_unit.fullValue != 0) {

					current_unit = &unitTable_0059CB58[current_stored_unit.innerValues.index];

					if (
						current_unit->sprite != NULL &&
						(current_unit->mainOrderId != OrderId::Die || current_unit->mainOrderState != 1) &&
						current_unit->targetOrderSpecial == current_stored_unit.innerValues.unitId &&
						!(current_unit->sprite->flags & CSprite_Flags::Hidden) &&
						current_unit->playerId == *ACTIVE_NATION_ID &&
						(unit_IsStandardAndMovable(current_unit) || selectionLength <= 1)
						)
					{
						validated_unit = current_unit;
						temp_selection_array[index] = current_unit;
						index++;
					}
					else
						validated_unit = NULL;

				}
				else
					validated_unit = NULL;

				selectionLength--;
				i++;

			} while (selectionLength != 0);

			//96C0E
			if (index != 0) {

				if (
					index == 1 &&
					validated_unit != NULL &&
					validated_unit->playerId == *ACTIVE_NATION_ID &&
					(
						validated_unit->id == UnitId::TerranCommandCenter ||
						validated_unit->id == UnitId::TerranBarracks ||
						validated_unit->id == UnitId::TerranFactory ||
						validated_unit->id == UnitId::TerranStarport ||
						validated_unit->id == UnitId::ZergInfestedCommandCenter ||
						validated_unit->id == UnitId::ZergHatchery ||
						validated_unit->id == UnitId::ZergLair ||
						validated_unit->id == UnitId::ZergHive ||
						validated_unit->id == UnitId::ProtossNexus ||
						validated_unit->id == UnitId::ProtossGateway ||
						validated_unit->id == UnitId::ProtossStargate ||
						validated_unit->id == UnitId::ProtossRoboticsFacility
					)
				) 
				{
					//this is what make the rally point briefly appear
					function_00468670(validated_unit);
				}

				if (*IS_PLACING_BUILDING != 0) {
					refreshLayer3And4();
					function_0048E310();
				}

				if (*bDoingCancellableTargetOrder != 0)
					CancelTargetOrder();

				CreateNewUnitSelectionsFromList(temp_selection_array, index);

				if (index > 0) {

					int temp_index = index;
					CUnit* previous_unit = NULL;
					i = 0;

					do {

						current_unit = temp_selection_array[i];

						if (compareUnitRank(current_unit, previous_unit) != 0)
							previous_unit = current_unit;

						i++;
						temp_index--;

					} while (temp_index != 0);

					current_unit = previous_unit;

				}
				else
					current_unit = NULL;

				selectBuildingSFX(current_unit);

				*bCanUpdateSelectedUnitData = 1;
				*bCanUpdateCurrentButtonSet = 1;
				*bCanUpdateSelectedUnitPortrait = 1;
				*bCanUpdateStatDataDialog = 1;
				*someDialogUnknown = NULL;
				*someDialogUnknownUser = NULL;

				CMDACT_HotkeyUnit(selectionGroupNumber, 1, temp_selection_array, index);

			}

		}

	}

	;

} //namespace hooks

//-------- Helper function definitions. Do NOT modify! --------//

namespace {

	//Identical to 0x00401170
	bool unitIsOwnedByCurrentPlayer(CUnit* unit) {

		bool return_value;

		if(*IS_IN_REPLAY != 0 || unit->playerId != *LOCAL_NATION_ID)
			return_value = false;
		else
			return_value = true;

		return return_value;

	}

	;
	
	//Identical to 0x00402A70
	bool isUnitBurrowed(CUnit* unit) {
		return (unit->status & (UnitStatus::CloakingForFree + UnitStatus::Burrowed)) != 0;
	}

	;
	
	const u32 Func_memcpy_0 = 0x00408FD0;
	void SC_memcpy_0(u32 dest, u32 src, u32 memsize) {

		__asm {
			PUSHAD
			PUSH memsize
			PUSH src
			PUSH dest
			CALL Func_memcpy_0
			ADD ESP, 0x0C
			POPAD
		}

	}

	;
	
	const u32 Func_GetAllUnitsInBounds = 0x0042FF80;
	CUnit** getAllUnitsInBounds(Box16* coords) {

		static CUnit** units_in_bounds;

		__asm {
			PUSHAD
			MOV EAX, coords
			CALL Func_GetAllUnitsInBounds
			MOV units_in_bounds, EAX
			POPAD
		}

		return units_in_bounds;

	}

	;
	
	const u32 Func_FindAllUnits = 0x004308A0;
	CUnit** FindAllUnits(Box16* coords) {

		static CUnit** units_in_coords;

		__asm {
			PUSHAD
			PUSH coords
			CALL Func_FindAllUnits
			MOV units_in_coords, EAX
			POPAD
		}

		return units_in_coords;

	}

	;
	
	const u32 Func_Sub_468670 = 0x00468670;
	void function_00468670(CUnit* unit) {

		__asm {
			PUSHAD
			MOV EAX, unit
			CALL Func_Sub_468670
			POPAD
		}

	}

	;
	
	const u32 Func_Unit_isUnselectable = 0x0046ED80;
	bool unit_isUnselectable(u16 unitId) {

		Bool32 bHalfReturnValue;

		__asm {
			PUSHAD
			XOR EBX, EBX
			MOV BX, unitId
			MOV EAX, EBX
			CALL Func_Unit_isUnselectable
			MOV bHalfReturnValue, EAX
			POPAD
		}

		return (bHalfReturnValue != 0);

	}

	;
	
	const u32 Func_Sub_6F040 = 0x0046F040;
	void function_0046F040_Helper(CUnit* current_unit, CUnit** unit_list, CUnit* clicked_unit, u32 list_length) {																//

		__asm {
			PUSHAD
			PUSH clicked_unit
			PUSH current_unit
			MOV ECX, unit_list
			MOV EAX, list_length
			CALL Func_Sub_6F040
			POPAD
		}

	}

	;
	
	const u32 Func_SortAllUnits = 0x0046F0F0;
	u32 SortAllUnits_Helper(CUnit* unit, CUnit** unit_list, CUnit** units_in_bounds) {

		static u32 result;

		__asm {
			PUSHAD
			PUSH unit
			PUSH unit_list
			PUSH units_in_bounds
			CALL Func_SortAllUnits
			MOV result, EAX
			POPAD
		}

		return result;

	}

	;
	
	const u32 Func_Sub_46F290 = 0x0046F290;
	u32 combineLists_Sub_6F290(CUnit* unit,CUnit** unit_list_1,CUnit** unit_list_2,u32 list_length) {

		u32 return_value;

		__asm {
			PUSHAD
			PUSH unit
			PUSH list_length
			PUSH unit_list_1
			MOV EDI, unit_list_2
			CALL Func_Sub_46F290
			MOV return_value, EAX
			POPAD
		}

		return return_value;

	}

	;
	
	const u32 Func_function_0046F3A0 = 0x0046F3A0;
	CUnit* function_0046F3A0(u32 unk1, u32 unk2) {

		static CUnit* returned_unit;

		__asm {
			PUSHAD
			PUSH unk1
			PUSH unk2
			CALL Func_function_0046F3A0
			MOV returned_unit, EAX
			POPAD
		}

		return returned_unit;

	}

	;
	
	const u32 Func_Sub_46FA00 = 0x0046FA00;
	void applyNewSelect_Sub_6FA00(CUnit** unit_list,u32 unit_list_length) {

		__asm {
			PUSHAD
			PUSH 0x01
			PUSH 0x01
			MOV EBX, unit_list
			MOV EDI, unit_list_length
			CALL Func_Sub_46FA00
			POPAD
		}

	}
	
	;

	const u32 Func_CUnitToUnitID = 0x0047B1D0;
	u32 CUnitToUnitID(CUnit* unit) {

		static u32 return_value;

		__asm {
			PUSHAD
			MOV ESI, unit
			CALL Func_CUnitToUnitID
			MOV return_value, EAX
			POPAD
		}

		return return_value;

	}

	;
	
	const u32 Func_Unit_IsStandardAndMovable = 0x0047B770;
	//hooked in utils
	bool unit_IsStandardAndMovable(CUnit* unit) {

		Bool32 return_value_unconverted;

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
	
	const u32 Func_CancelTargetOrder = 0x0048CA10;
	void CancelTargetOrder() {
		__asm {
			PUSHAD
			CALL Func_CancelTargetOrder
			POPAD
		}
	}

	;

	const u32 Func_RefreshLayer3And4 = 0x0048D9A0;
	void refreshLayer3And4() {

		__asm {
			PUSHAD
			CALL Func_RefreshLayer3And4
			POPAD
		}

	}

	;

	const u32 Func_Sub48E310 = 0x0048E310;
	void function_0048E310() {

		__asm {
			PUSHAD
			CALL Func_Sub48E310
			POPAD
		}

	}

	;
	
	const u32 Func_SelectBuildingSFX = 0x0048F910;
	void selectBuildingSFX(CUnit* unit) {

		__asm {
			PUSHAD
			PUSH unit
			CALL Func_SelectBuildingSFX
			POPAD
		}

	}

	;
	
	const u32 Func_SelectSingleUnitFromID = 0x00496D30;
	Bool32 selectSingleUnitFromID(u32 unitIndex) {

		Bool32 bReturnedValue;

		__asm {
			PUSHAD
			PUSH unitIndex
			CALL Func_SelectSingleUnitFromID
			MOV bReturnedValue, EAX
			POPAD
		}

		return bReturnedValue;

	}

	;
	
	const u32 Func_CompareUnitRank = 0x0049A350;
	//Check if current_unit is before other_unit in selection.
	//If not, return 0 (I think).
	Bool32 compareUnitRank(CUnit* current_unit, CUnit* other_unit) {

		static Bool32 bResult;

		__asm {
			PUSHAD
			MOV EDI, current_unit
			MOV ESI, other_unit
			CALL Func_CompareUnitRank
			MOV bResult, EAX
			POPAD
		}

		return bResult;

	}

	;
	
	const u32 Func_CreateNewUnitSelectionsFromList = 0x0049AE40;
	void CreateNewUnitSelectionsFromList(CUnit** unit_list, u32 unit_list_length) {

		__asm {
			PUSHAD
			PUSH unit_list_length
			MOV EAX, unit_list
			CALL Func_CreateNewUnitSelectionsFromList
			POPAD
		}

	}	

	;
	
	const u32 Func_selectMultipleUnitsFromUnitList = 0x0049AEF0;
	void selectMultipleUnitsFromUnitList(CUnit** unitList, u32 unitsCount, Bool32 unk1, Bool32 unk2) {

		__asm {
			PUSHAD
			PUSH unk2
			PUSH unk1
			PUSH unitList
			PUSH unitsCount
			CALL Func_selectMultipleUnitsFromUnitList
			POPAD
		}

	}

	;

	const u32 Func_CMDACT_HotkeyUnit = 0x004C07B0;
	void CMDACT_HotkeyUnit(u8 selectionGroupNumber, u32 params_length, CUnit** selection_array, u32 array_length) {
		__asm {
			PUSHAD
			MOV BL, selectionGroupNumber
			PUSH array_length
			PUSH selection_array
			PUSH params_length
			CALL Func_CMDACT_HotkeyUnit
			POPAD
		}
	}

	;

	const u32 Func_CMDACT_Select = 0x004C0860;
	void CMDACT_Select(CUnit** unit_list, u32 unit_list_length) {

		__asm {
			PUSHAD
			PUSH unit_list
			PUSH unit_list_length
			CALL Func_CMDACT_Select
			POPAD
		}

	}

	;

} //unnamed namespace

//End of helper functions

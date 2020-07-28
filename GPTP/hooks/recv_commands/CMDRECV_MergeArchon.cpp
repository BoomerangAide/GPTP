#include "CMDRECV_MergeArchon.h"
#include <SCBW/api.h>

//Helper functions declaration

namespace {

void removeOrderFromUnitQueue(CUnit* unit, COrder* order);							//742D0
CUnit* templarMergePartner(CUnit* unit, CUnit** units_list, u32 units_list_length);	//BFA80

} //unnamed namespace

namespace hooks {

void CMDRECV_MergeDarkArchon() {

	CUnit* current_unit;
	CUnit* templar_stored;
	CUnit* templar_merge_partner;
	CUnit* templars_stored[SELECTION_ARRAY_LENGTH];
	CUnit** templars_stored_iterator;
	int templars_stored_count;

	*selectionIndexStart = 0;
	current_unit = getActivePlayerNextSelection();

	if(	current_unit != NULL && 
		current_unit->canUseTech(TechId::DarkArchonMeld,*ACTIVE_NATION_ID) == 1) 
	{

		for(int i = 0;i < SELECTION_ARRAY_LENGTH;i++)
			templars_stored[i] = NULL;

		templars_stored_count = 0;

		while(current_unit != NULL) {

			if(current_unit->id == UnitId::ProtossDarkTemplar) {
				templars_stored[templars_stored_count] = current_unit;
				templars_stored_count++;
			}

			current_unit = getActivePlayerNextSelection();

		}

		templars_stored_iterator = &templars_stored[1];
		templars_stored_count--;

		while(templars_stored_count > 0) {

			templar_stored = templars_stored_iterator[-1];

			if(templar_stored != NULL) {

				templar_merge_partner = templarMergePartner(templar_stored,templars_stored_iterator,templars_stored_count);

				if(templar_merge_partner != NULL) {

					templar_stored->userActionFlags |= 1;

					if(templar_stored->mainOrderId != OrderId::Die) {

						bool bKeepLooping = true;

						while(bKeepLooping) {

							if(templar_stored->orderQueueTail == NULL)
								bKeepLooping = false;
							else 
							if(orders_dat::CanBeInterrupted[templar_stored->orderQueueTail->orderId] ||
								templar_stored->orderQueueTail->orderId == OrderId::WarpingDarkArchon
							)
								removeOrderFromUnitQueue(templar_stored,templar_stored->orderQueueTail);
							else
								bKeepLooping = false;

						}

						templar_stored->performAnotherOrder(
							OrderId::WarpingDarkArchon,
							templar_merge_partner->sprite->position.x,
							templar_merge_partner->sprite->position.y,
							templar_merge_partner,
							UnitId::None
						);



					}

					prepareForNextOrder(templar_stored);

					templar_merge_partner->userActionFlags |= 1;

					if(templar_merge_partner->mainOrderId != OrderId::Die) {

						bool bKeepLooping = true;

						while(bKeepLooping) {

							if(templar_merge_partner->orderQueueTail == NULL)
								bKeepLooping = false;
							else 
							if(orders_dat::CanBeInterrupted[templar_merge_partner->orderQueueTail->orderId] ||
								templar_merge_partner->orderQueueTail->orderId == OrderId::WarpingDarkArchon
							)
								removeOrderFromUnitQueue(templar_merge_partner,templar_merge_partner->orderQueueTail);
							else
								bKeepLooping = false;

						}

						templar_merge_partner->performAnotherOrder(
							OrderId::WarpingDarkArchon,
							templar_stored->sprite->position.x,
							templar_stored->sprite->position.y,
							templar_stored,
							UnitId::None
						);

					}

					prepareForNextOrder(templar_merge_partner);

				}

			}

			templars_stored_iterator++;
			templars_stored_count--;

		}

	}

	scbw::refreshConsole();

}

;

void CMDRECV_MergeArchon() {

	CUnit* current_unit;
	CUnit* templar_stored;
	CUnit* templar_merge_partner;
	CUnit* templars_stored[SELECTION_ARRAY_LENGTH];
	CUnit** templars_stored_iterator;
	int templars_stored_count;

	*selectionIndexStart = 0;
	current_unit = getActivePlayerNextSelection();

	if(	current_unit != NULL && 
		current_unit->canUseTech(TechId::ArchonWarp,*ACTIVE_NATION_ID) == 1) 
	{

		for(int i = 0;i < SELECTION_ARRAY_LENGTH;i++)
			templars_stored[i] = NULL;

		templars_stored_count = 0;

		while(current_unit != NULL) {

			if(current_unit->id == UnitId::ProtossHighTemplar) {
				templars_stored[templars_stored_count] = current_unit;
				templars_stored_count++;
			}

			current_unit = getActivePlayerNextSelection();

		}

		templars_stored_iterator = &templars_stored[1];
		templars_stored_count--;

		while(templars_stored_count > 0) {

			templar_stored = templars_stored_iterator[-1];

			if(templar_stored != NULL) {

				templar_merge_partner = templarMergePartner(templar_stored,templars_stored_iterator,templars_stored_count);

				if(templar_merge_partner != NULL) {

					templar_stored->userActionFlags |= 1;

					if(templar_stored->mainOrderId != OrderId::Die) {

						bool bKeepLooping = true;

						while(bKeepLooping) {

							if(templar_stored->orderQueueTail == NULL)
								bKeepLooping = false;
							else 
							if(orders_dat::CanBeInterrupted[templar_stored->orderQueueTail->orderId] ||
								templar_stored->orderQueueTail->orderId == OrderId::WarpingArchon
							)
								removeOrderFromUnitQueue(templar_stored,templar_stored->orderQueueTail);
							else
								bKeepLooping = false;

						}

						templar_stored->performAnotherOrder(
							OrderId::WarpingArchon,
							templar_merge_partner->sprite->position.x,
							templar_merge_partner->sprite->position.y,
							templar_merge_partner,
							UnitId::None
						);



					}

					prepareForNextOrder(templar_stored);

					templar_merge_partner->userActionFlags |= 1;

					if(templar_merge_partner->mainOrderId != OrderId::Die) {

						bool bKeepLooping = true;

						while(bKeepLooping) {

							if(templar_merge_partner->orderQueueTail == NULL)
								bKeepLooping = false;
							else 
							if(orders_dat::CanBeInterrupted[templar_merge_partner->orderQueueTail->orderId] ||
								templar_merge_partner->orderQueueTail->orderId == OrderId::WarpingArchon
							)
								removeOrderFromUnitQueue(templar_merge_partner,templar_merge_partner->orderQueueTail);
							else
								bKeepLooping = false;

						}

						templar_merge_partner->performAnotherOrder(
							OrderId::WarpingArchon,
							templar_stored->sprite->position.x,
							templar_stored->sprite->position.y,
							templar_stored,
							UnitId::None
						);

					}

					prepareForNextOrder(templar_merge_partner);

				}

			}

			templars_stored_iterator++;
			templars_stored_count--;

		}

	}

	scbw::refreshConsole();

}

;

} //namespace hooks

//-------- Helper function definitions. Do NOT modify! --------//

namespace {

const u32 Func_removeOrderFromUnitQueue = 0x004742D0;
void removeOrderFromUnitQueue(CUnit* unit, COrder* order) {

	__asm {
		PUSHAD
		MOV ECX, unit
		MOV EAX, order
		CALL Func_removeOrderFromUnitQueue
		POPAD
	}

}

;

const u32 Func_templarMergePartner = 0x004BFA80;
CUnit* templarMergePartner(CUnit* unit, CUnit** units_list, u32 units_list_length) {

	static CUnit* return_value;

	__asm {
		PUSHAD
		MOV EAX, units_list_length
		PUSH units_list
		PUSH unit
		CALL Func_templarMergePartner
		MOV return_value, EAX
		POPAD
	}

	return return_value;

}

;

} //unnamed namespace

//End of helper functions

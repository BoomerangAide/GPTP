#include "powerup.h"
#include <SCBW/api.h>

//helper functions def
namespace {

void	removeOrderFromUnitQueue(CUnit* unit, COrder* order);								//742D0
void	updateImagePositionOffset(CImage* image);											//D5A00
void	networkTBLPlayerMessage(u32 playerId, u32 someId);									//E3DE0
void	CTF_TakeFlagEvent(u32 playerId);													//E3F30
void	hideAndDisableUnit(CUnit* unit);													//E6340
void	showAndEnableUnit(CUnit* unit);														//E6490
CUnit*	IterateUnitsAtLocationTargetProc_FindWorkerToCarry(CUnit* powerup, Box16* coords);	//E8280
void	destroyPowerupImageOverlay(CUnit* unit);											//F3900
void	pickupPowerup_Helper(CUnit* worker, CUnit* powerup);								//F3CA0
void	orders_Powerup2_Helper(CUnit* powerup);												//F3E10

} //unnamed namespace

namespace hooks {

//Check used to evaluate if a worker can pick up the powerup
bool powerupCanBePickedUpProc(CUnit* worker, CUnit* powerup) {

	bool return_value;

	if (
		(units_dat::BaseProperty[worker->id] & UnitProperty::Worker) &&
		(worker->status & UnitStatus::Completed) &&
		!(worker->status & UnitStatus::Burrowed) &&
		!(worker->status & UnitStatus::IsHallucination) &&
		worker->worker.powerup == NULL &&
		(powerup->id != UnitId::Powerup_Flag || powerup->playerId != worker->playerId)
	)
		return_value = true;
	else
		return_value = false;

	return return_value;

}

;

//Kill the powerup carried by the worker
void killPowerup(CUnit* worker) {

	if (worker->resourceType != 0) {

		CUnit* powerup = worker->worker.powerup;

		destroyPowerupImageOverlay(worker);

		if (powerup != NULL) {
			powerup->userActionFlags |= 4;
			powerup->remove();
		}

		worker->resourceType = 0;
		worker->worker.powerup = NULL;

	}

}

;

//Function @ 004F3CA0
//Called by orders_Powerup2 if a worker/powerup pair was found
void pickupPowerup(CUnit* worker, CUnit* powerup) {

	bool bEndThere = false;

	if (worker->resourceType != 0) {
		if (worker->worker.powerup != NULL)
			bEndThere = true;
		else
		{
			destroyPowerupImageOverlay(worker);
			worker->resourceType = 0;
		}
	}

	if (!bEndThere) {

		CImage* overlay;
		u32 imageId;

		if (powerup->id == UnitId::Powerup_Flag)
			CTF_TakeFlagEvent(powerup->playerId);

		if (powerup->id == UnitId::Powerup_UrajCrystal)
			imageId = ImageId::Uraj;
		else
		if (powerup->id == UnitId::Powerup_KhalisCrystal)
			imageId = ImageId::Khalis;
		else
			imageId = powerup->id + 177;	//give a value between ImageId::Flag and ImageId::TerranGasTankType2

		hideAndDisableUnit(powerup);
		powerup->userActionFlags |= 1;

		if (powerup->mainOrderId != OrderId::Die) {

			while (
				powerup->orderQueueTail != NULL &&
				(
					(orders_dat::CanBeInterrupted[powerup->orderQueueTail->orderId] != 0) ||
					(powerup->orderQueueTail->orderId == OrderId::Nothing2)
					)
			)
				removeOrderFromUnitQueue(powerup, powerup->orderQueueTail);

			powerup->performAnotherOrder(OrderId::Nothing2, 0, 0, NULL, UnitId::None);

		}

		prepareForNextOrder(powerup);

		powerup->building.powerup.carrying_unit = worker;
		overlay = worker->sprite->createOverlay(imageId);

		if (overlay != NULL) {

			if (overlay->paletteType == PaletteType::RLE_PLAYER_SIDE)
				overlay->coloringData = (void*)powerup->playerId;

			if (!(overlay->flags & CImage_Flags::Flag07)) {
				overlay->flags |= CImage_Flags::Flag07;
				updateImagePositionOffset(overlay);
			}

		}

		worker->worker.powerup = powerup;
		worker->resourceType |= 4;

		scbw::refreshConsole();

	}

}

;

//Powerup order, seek a worker that can pick up the powerup
void orders_Powerup2(CUnit* powerup) {

	Box16 coords;
	CUnit* worker;

	if (powerup->sprite->flags & CSprite_Flags::Hidden)
		showAndEnableUnit(powerup);

	coords.left = powerup->sprite->position.x - 24;
	coords.right = powerup->sprite->position.x + 24;
	coords.top = powerup->sprite->position.y - 24;
	coords.bottom = powerup->sprite->position.y + 24;

	worker = IterateUnitsAtLocationTargetProc_FindWorkerToCarry(powerup, &coords);

	if (worker != NULL) {
		pickupPowerup_Helper(worker, powerup);
		scbw::refreshConsole();
	}

}

;

//Possibly order that return a flag to its original position/beacon after a time
void orders_Powerup1(CUnit* powerup) {

	orders_Powerup2_Helper(powerup);

	if (powerup->mainOrderState == 0) {
		powerup->remainingBuildTime = 337; //0x151
		powerup->mainOrderState = 1;
	}
	else
	if(powerup->remainingBuildTime != 0)
		powerup->remainingBuildTime--;
	else
	{

		const u8* victory_conditions = (u8*)(0x0059686D);

		scbw::setUnitPosition(powerup, powerup->building.powerup.powerupOrigin.x, powerup->building.powerup.powerupOrigin.y);

		if (*victory_conditions == 4) //4 is CTF
			networkTBLPlayerMessage(powerup->playerId, 127); //0x7F (not stat_txt text, nor sound)

		powerup->orderComputerCL(OrderId::Powerup2);

	}

}

;

} //hooks

;

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

const u32 Func_updateImagePositionOffset = 0x004D5A00;
void updateImagePositionOffset(CImage *image) {

	__asm {
		PUSHAD
		MOV ECX, image
		CALL Func_updateImagePositionOffset
		POPAD
	}

}

;

const u32 Func_networkTBLPlayerMessage = 0x004E3DE0;
void networkTBLPlayerMessage(u32 playerId, u32 someId) {
	__asm {
		PUSHAD
		MOV EAX, playerId
		MOV ECX, someId
		CALL Func_networkTBLPlayerMessage
		POPAD
	}
}

;

const u32 Func_CTF_TakeFlagEvent = 0x004E3F30;
void CTF_TakeFlagEvent(u32 playerId) {
	__asm {
		PUSHAD
		PUSH playerId
		CALL Func_CTF_TakeFlagEvent
		POPAD
	}
}

;

const u32 Func_unitDeathSomething_0 = 0x004E6340;
void hideAndDisableUnit(CUnit* unit) {

	__asm {
		PUSHAD
		MOV EAX, unit
		CALL Func_unitDeathSomething_0
		POPAD
	  }

}

;

const u32 Func_InitUnitTrapDoodad = 0x004E6490;
void showAndEnableUnit(CUnit* unit) {

	__asm {
		PUSHAD
		MOV EDI, unit
		CALL Func_InitUnitTrapDoodad
		POPAD
	}

}

;

const u32 Func_IterateUnitsAtLocationTargetProc = 0x004E8280;
//hardcoding the filter function offset since return value
//use and nature vary depending on it, so hardcoding each 
//variant.
CUnit* IterateUnitsAtLocationTargetProc_FindWorkerToCarry(CUnit* powerup, Box16* coords) {

	static CUnit* return_value;

	__asm {
		PUSHAD
		PUSH powerup
		MOV EAX, coords
		MOV EBX, 0x00440930	//powerupCanBePickedUpProc
		CALL Func_IterateUnitsAtLocationTargetProc
		MOV return_value, EAX
		POPAD
	}

	return return_value;

}

;

//Logically equivalent to function @ 004F3900
//Should have a return value (id of image removed or
//ImageId::Khalis of nothing removed), but it doesn't
//seem to be ever used, so ignored
void destroyPowerupImageOverlay(CUnit* unit) {

	//Remove powerup shadows (mineral chunk shadow, psi emitter shadow, etc.)
	unit->sprite->removeOverlay(ImageId::MineralChunkShadow, ImageId::PsiEmitterShadow_Carried);

	//Remove powerup graphics
	unit->sprite->removeOverlay(ImageId::Flag, ImageId::TerranGasTankType2);

	//Remove Uraj / Khalis
	unit->sprite->removeOverlay(ImageId::Uraj, ImageId::Khalis);

}

;

const u32 Func_Sub4F3CA0 = 0x004F3CA0;
void pickupPowerup_Helper(CUnit* worker, CUnit* powerup) {
	__asm {
		PUSHAD
		PUSH powerup
		PUSH worker
		CALL Func_Sub4F3CA0
		POPAD
	}
}

;

const u32 Func_orders_Powerup2 = 0x004F3E10;
void orders_Powerup2_Helper(CUnit* powerup) {
	__asm {
		PUSHAD
		MOV EAX, powerup
		CALL Func_orders_Powerup2
		POPAD
	}
}

;

} //Unnamed namespace

//End of helper functions

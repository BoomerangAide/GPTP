#include "harvest.h"
#include "../SCBW/api.h"

//Helper functions
void updateMineralPatchImage(CUnit* mineralPatch);													//68830
void setResourceAmountCarried(CUnit* worker, u8 amountCarried, u32 chunkImageId, bool isMineral);	//F3AF0

//-------- Actual hooks --------//

namespace hooks {

const int RESOURCE_HARVESTED = 8;

//Harvests minerals/gas from the resource and returns the amount that a
//worker should carry.
//Identical to sub_469650
u8 harvestResourceFrom(CUnit* resource, bool isMineral) {

	u8 return_value;

	if (resource->building.resource.resourceAmount < RESOURCE_HARVESTED) {

		if (isMineral) {
			resource->remove();
			return_value = (u8)(resource->building.resource.resourceAmount);
		}
		else {
			resource->building.resource.resourceAmount = 0;
			return_value = 2;
		}

	}
	else {

		resource->building.resource.resourceAmount -= RESOURCE_HARVESTED;
		
		if (isMineral) {
			if (resource->building.resource.resourceAmount > 0)
				updateMineralPatchImage(resource);	//equivalent to setResourceCount @ 68830 ?
			else
				resource->remove();
		}
		else 
		if (resource->building.resource.resourceAmount < RESOURCE_HARVESTED)
			scbw::showErrorMessageWithSfx(resource->playerId, 875, 20); //Gas depleted message and sound
		
		return_value = RESOURCE_HARVESTED;

	}

	return return_value;

} //u8 harvestResourceFrom(CUnit* resource, bool isMineral)

;

//Transfers a set amount of resources from a resource patch to a worker.
void transferResourceToWorkerHook(CUnit* worker, CUnit* resource) {

	if(resource->id >= UnitId::TerranRefinery && resource->id <= UnitId::ResourceMineralFieldType3) {

		u32 chunkImageId;
		bool isMineral = false;
		bool bEndThere = false;

		if (
			resource->id >= UnitId::ResourceMineralField && 
			resource->id <= UnitId::ResourceMineralFieldType3
		) 
		{	//Is a mineral patch
			chunkImageId = ImageId::MineralChunkType1;
			isMineral = true;
		}
		else 
		if (resource->id == UnitId::assimilator)
			chunkImageId = ImageId::ProtossGasOrbType1;
		else 
		if (resource->id == UnitId::extractor)
			chunkImageId = ImageId::ZergGasSacType1;
		else 
		if (resource->id == UnitId::refinery)
			chunkImageId = ImageId::TerranGasTankType1;
		else
			bEndThere = true;

		if(!bEndThere) {

			u8 resourceAmount = harvestResourceFrom(resource, isMineral);

			if (resourceAmount < RESOURCE_HARVESTED)
				chunkImageId += 1;	//Use depleted (smaller) chunk image

			if (resourceAmount > 0) {

				if (worker->resourceType & 3) { //Is carrying a mineral / gas

					//Equivalent to function @ 0x004F3900
					//Remove powerup shadows (mineral chunk shadow, psi emitter shadow, etc.)
					worker->sprite->removeOverlay(ImageId::MineralChunkShadow, ImageId::PsiEmitterShadow_Carried);

					//Remove powerup graphics
					worker->sprite->removeOverlay(ImageId::Flag, ImageId::TerranGasTankType2);

					//Remove Uraj / Khalis
					worker->sprite->removeOverlay(ImageId::Uraj, ImageId::Khalis);

					//End of function @ 0x004F3900 equivalent, return value is overwritten					
					
					worker->resourceType = 0;

				}

				setResourceAmountCarried(worker, resourceAmount, chunkImageId, isMineral);

			}

		}

	}

} //void transferResourceToWorkerHook(CUnit* worker, CUnit* resource)

;

} //hooks

//-------- Helper function definitions. Do NOT change this! --------//

//Identical to function @ 0x00468830
void updateMineralPatchImage(CUnit* mineralPatch) {

	IscriptAnimation::Enum anim;

	if (mineralPatch->building.resource.resourceAmount >= 750)
		anim = IscriptAnimation::WorkingToIdle;
	else 
	if (mineralPatch->building.resource.resourceAmount >= 500)
		anim = IscriptAnimation::AlmostBuilt;
	else 
	if (mineralPatch->building.resource.resourceAmount >= 250)
		anim = IscriptAnimation::SpecialState2;
	else
		anim = IscriptAnimation::SpecialState1;

	if (anim != mineralPatch->building.resource.resourceIscript) {
		mineralPatch->building.resource.resourceIscript = anim;
		mineralPatch->sprite->playIscriptAnim(anim);
	}

}

void updateImagePositionOffset(CImage *image) {

	const u32 Func_updateImagePositionOffset = 0x004D5A00;

	__asm {
		PUSHAD
		MOV ECX, image
		CALL Func_updateImagePositionOffset
		POPAD
	}

}

//Identical to function @ 0x004F3AF0
void setResourceAmountCarried(CUnit* worker, u8 amountCarried, u32 chunkImageId, bool isMineral) {

	if (worker->resourceType != 0) 
		return;

	worker->resourceType = isMineral ? 2 : 1;
	
	CImage* chunkImage = worker->sprite->createOverlay(chunkImageId);

	if (chunkImage && !(chunkImage->flags & CImage_Flags::Flag07)) {
		chunkImage->flags |= CImage_Flags::Flag07;
		updateImagePositionOffset(chunkImage);
	}

	worker->worker.resourceCarryAmount = amountCarried;
	scbw::refreshConsole();

}

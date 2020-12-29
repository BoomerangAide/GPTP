#include "warpin.h"
#include <SCBW/api.h>

//helper functions def
namespace {

void actUnitReturnToIdle(CUnit* unit);										//75420
void replaceSpriteImages(CSprite* sprite, u32 imageId, u32 imageDirection);	//99BB0
void PlaySoundFromDirect(CUnit* unit, u32 soundId);							//9B5B0

} //unnamed namespace

namespace hooks {

//Exact use unclear, units performing this are not cloaked by Arbiters.
//Seems unused, though iscript WarpIn is still used at least by nukes,
//but the order is not as far as I know.
void orders_Warpin(CUnit* unit) {

	if (unit->mainOrderState == 0) {
		if (unit->orderSignal & 1) {

			CImage* current_image;

			unit->orderSignal -= 1;
			replaceSpriteImages(unit->sprite, sprites_dat::ImageId[unit->sprite->spriteId], unit->currentDirection1);

			current_image = unit->sprite->images.head;

			while (current_image != NULL) {
				current_image->playIscriptAnim(IscriptAnimation::WarpIn);
				current_image = current_image->link.next;
			}

			unit->mainOrderState = 1;

		}
	}
	else
	if (unit->mainOrderState == 1) {
		if (unit->orderSignal & 1) {

			unit->orderSignal -= 1;

			if (units_dat::BaseProperty[unit->id] & UnitProperty::PermanentCloak)
				PlaySoundFromDirect(unit, SoundId::Terran_PHOENIX_TPhClo00_WAV);

			actUnitReturnToIdle(unit);

		}
	}

}

;

} //hooks

;

//-------- Helper function definitions. Do NOT modify! --------//

namespace {

const u32 Func_ActUnitReturnToIdle = 0x00475420;
void actUnitReturnToIdle(CUnit* unit) {
	__asm {
		PUSHAD
		MOV EAX, unit
		CALL Func_ActUnitReturnToIdle
		POPAD
	}
}

;

const u32 Func_Sub499BB0 = 0x00499BB0;
void replaceSpriteImages(CSprite *sprite, u32 imageId, u32 imageDirection) {
	__asm {
		PUSHAD
		PUSH imageDirection
		PUSH imageId
		MOV EAX, sprite
		CALL Func_Sub499BB0
		POPAD
	}
}

;

const u32 Func_PlaySoundFromDirect = 0x0049B5B0;
//linked to sound playing, cloaking, and others things
void PlaySoundFromDirect(CUnit* unit, u32 soundId) {
	__asm {
		PUSHAD
		MOV EAX, unit
		MOV ECX, soundId
		CALL Func_PlaySoundFromDirect
		POPAD
	}
}

;

} //Unnamed namespace

//End of helper functions

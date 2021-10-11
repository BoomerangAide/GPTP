#include "bunker_hooks.h"
#include <SCBW/scbwdata.h>

//Helper function declarations. Do NOT modify!
namespace {

void setThingyVisibilityFlags(CThingy *thingy);						//878F0
CThingy* createThingy(u16 spriteId, s16 x, s16 y, u8 playerId = 0);	//88210
void setImageDirection(CImage *image, s8 direction);				//D5F80

} //unnamed namespace

namespace hooks {

//not checked against original code by UndeadStar
void createBunkerAttackThingyHook(CUnit* unit) {

	CImage *bunkerImage = unit->connectedUnit->sprite->mainGraphic;

	u8 frameDirection = (unit->currentDirection1 + 16) / 32 % 8;

	const LO_Header *loFile = lo_files->attackOverlays[bunkerImage->id];

	Point8 offset = loFile->getOffset(bunkerImage->frameIndex, frameDirection);
	
	if (bunkerImage->flags & CImage_Flags::Mirrored) //Is inverted
		offset.x = -offset.x;

	u8 frameAngle;
	u16 spriteId;

	if (unit->id == UnitId::TerranFirebat || unit->id == UnitId::Hero_GuiMontag) {
		frameAngle = ((unit->currentDirection1 + 8) / 16 % 16) * 16;
		spriteId = SpriteId::FlameThrower; //Firebat flamethrower graphics
	}
	else {
		frameAngle = frameDirection * 32;
		spriteId = SpriteId::Bunker_Overlay; //Bunker attack overlay
	}

	CThingy *bunkerAttackEffect = 
		createThingy( spriteId, offset.x + unit->getX(), offset.y + unit->getY() );

	if (!bunkerAttackEffect) 
		return;

	bunkerAttackEffect->sprite->elevationLevel = unit->sprite->elevationLevel + 1;

	for (	
			CImage *image = bunkerAttackEffect->sprite->images.head;
			image != NULL; 
			image = image->link.next
		) 
	{
		setImageDirection(image, frameAngle);
	}

	setThingyVisibilityFlags(bunkerAttackEffect);

}

;

} //hooks


//-------- Helper function definitions. Do NOT modify! --------//

namespace {

const u32 Func_SetThingyVisibilityFlags = 0x004878F0;
void setThingyVisibilityFlags(CThingy *thingy) {

	__asm {
		PUSHAD
		MOV ESI, thingy
		CALL Func_SetThingyVisibilityFlags
		POPAD
	}

}

;

const u32 Func_CreateThingy = 0x00488210;
CThingy* createThingy(u16 spriteId, s16 x, s16 y, u8 playerId) {

	static CThingy* thingy;
	static s32 x_;
	static s32 playerId_;
	static u32 spriteId_;

	x_ = x;
	playerId_ = playerId;
	spriteId_ = spriteId;

	__asm {
		PUSHAD
		PUSH playerId_
		MOV DI, y
		PUSH x_
		PUSH spriteId_
		CALL Func_CreateThingy
		MOV thingy, EAX
		POPAD
	}

	return thingy;

}

;

const u32 Func_SetImageDirection = 0x004D5F80;
void setImageDirection(CImage *image, s8 direction) {

	static u32 direction_;

	direction_ = direction;

	__asm {
		PUSHAD
		PUSH direction_
		MOV ESI, image
		CALL Func_SetImageDirection
		POPAD
	}

}

;

};

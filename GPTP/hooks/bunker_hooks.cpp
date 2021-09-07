#include "bunker_hooks.h"
#include <SCBW/scbwdata.h>
#include "../SCBW/enumerations.h"

//Helper function declarations. Do NOT modify!
namespace {

CUnit* findRandomAttackTarget(CUnit* unit);							//42FC0
Bool32 attackApplyCooldown(CUnit* unit);							//78B40
void setThingyVisibilityFlags(CThingy *thingy);						//878F0
CThingy* createThingy(u16 spriteId, s16 x, s16 y, u8 playerId = 0);	//88210
void setImageDirection(CImage *image, s8 direction);				//D5F80


} //unnamed namespace

namespace hooks {

/// This is actually the order ran by units within a bunker.
/// See ordersIDCases from hooks\orders\0_orders\orders_root.cpp
void unitAttackFromInsideBunkerHook(CUnit* unit) {

	if (
		unit->id == UnitId::TerranMarine ||
		unit->id == UnitId::Hero_JimRaynorMarine ||
		unit->id == UnitId::TerranGhost ||
		unit->id == UnitId::Hero_SarahKerrigan ||
		unit->id == UnitId::Hero_AlexeiStukov ||
		unit->id == UnitId::Hero_SamirDuran ||
		unit->id == UnitId::Hero_InfestedDuran ||
		unit->id == UnitId::TerranFirebat ||
		unit->id == UnitId::Hero_GuiMontag
	)
	{

		Bool32 resultApplyCooldown;

		unit->status |= UnitStatus::HoldingPosition;

		if(unit->subunit != NULL)
			(unit->subunit)->status |= UnitStatus::HoldingPosition;

		resultApplyCooldown = attackApplyCooldown(unit);

		if(resultApplyCooldown != 0) {

			if(
				unit->orderTarget.pt.x != unit->nextTargetWaypoint.x ||
				unit->orderTarget.pt.y != unit->nextTargetWaypoint.y
			)
			{
				unit->nextTargetWaypoint.x = unit->orderTarget.pt.x;
				unit->nextTargetWaypoint.y = unit->orderTarget.pt.y;
			}

		}
		else {

			if(unit->mainOrderTimer == 0) {

				unit->mainOrderTimer = 15;

				unit->orderTarget.unit = findRandomAttackTarget(unit);

				if(unit->orderTarget.unit != NULL)
					unit->orderQueueTimer = 0;

			} //if(unit->mainOrderTimer == 0)

		} //if(resultApplyCooldown == 0)

	} //if valid id

}

;

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

	if (unit->id == UnitId::firebat || unit->id == UnitId::gui_montag) {
		frameAngle = ((unit->currentDirection1 + 8) / 16 % 16) * 16;
		spriteId = 378; //Firebat flamethrower graphics
	}
	else {
		frameAngle = frameDirection * 32;
		spriteId = 377; //Bunker attack overlay
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

} //hooks


//-------- Helper function definitions. Do NOT modify! --------//

namespace {

const u32 Func_findRandomAttackTarget = 0x00442FC0;
CUnit* findRandomAttackTarget(CUnit* unit) {

	static CUnit* target;

	__asm {
		PUSHAD
		MOV ESI, unit
		CALL Func_findRandomAttackTarget
		MOV target, EAX
		POPAD
	}

	return target;

}

;

const u32 Func_attackApplyCooldown = 0x00478B40;
Bool32 attackApplyCooldown(CUnit* unit) {

	static u32 result;

	__asm {
		PUSHAD
		MOV EAX, unit
		CALL Func_attackApplyCooldown
		MOV result, EAX
		POPAD
	}

	return result;

}
	
;

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

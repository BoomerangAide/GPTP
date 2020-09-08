#include "die_order.h"
#include <SCBW/api.h>

//helper functions def

namespace {

CThingy* createThingy(u32 spriteId, s16 x, s16 y, u32 playerId);		//88210
void clearUnitSprites(CSprite* unit_sprite, CSprite* subunit_sprite);	//96F00
void function_00497480(CSprite* thingySprite, u32 visibility_flags);	//97480
void _UnitDestructor(CUnit* unit);										//A0740
void UnitDestructor(CUnit* unit);										//A0990
void hideAndDisableUnit(CUnit* unit);									//E6340

} //unnamed namespace

namespace hooks {

void orders_Die(CUnit* unit) {

	bool jump_to_795AB = false;

	if(unit->userActionFlags & 4)
		hideAndDisableUnit(unit);

	if(unit->subunit != NULL) {

		clearUnitSprites(unit->sprite,unit->subunit->sprite);

		if(unit->subunit->subunit != NULL) {

			if(units_dat::BaseProperty[(unit->subunit->subunit)->id] & UnitProperty::Subunit) {
				UnitDestructor(unit->subunit->subunit);
				unit->subunit->subunit = NULL;
			}

			_UnitDestructor(unit->subunit);
			unit->subunit->sprite->free();

			unit->subunit->sprite = NULL;
			unit->subunit = NULL;

		}

	}

	unit->mainOrderState = 1;

	if(!(unit->sprite->flags & CSprite_Flags::Hidden)) {

		if(
			!(unit->status & UnitStatus::IsHallucination) ||
			unit->status & UnitStatus::IsSelfDestructing
		)
			jump_to_795AB = true;
		else {

			CThingy* hallucination_smoke;

			scbw::playSound(SoundId::Protoss_TEMPLAR_PTeHal01_WAV,unit);

			hallucination_smoke = 
				createThingy(
					SpriteId::Hallucination_Death1,
					unit->getX(),
					unit->getY(),
					0
				);

			if(hallucination_smoke != NULL) {

				//allow players to see smoke if could already see unit vanishing
				function_00497480(hallucination_smoke->sprite,unit->sprite->visibilityFlags);

			}

		}

	}

	if(!jump_to_795AB) {//79566

		hideAndDisableUnit(unit);

		if(unit->subunit != NULL) {

			if(units_dat::BaseProperty[unit->subunit->id] & UnitProperty::Subunit) {
				UnitDestructor(unit->subunit);
				unit->subunit = NULL;
			}

		}

		_UnitDestructor(unit);
		unit->sprite->free();
		unit->sprite = NULL;

	}
	else {
		unit->sprite->playIscriptAnim(IscriptAnimation::Death,true);
		_UnitDestructor(unit);
	}

}

;

} //namespace hooks

//-------- Helper function definitions. Do NOT modify! --------//

namespace {

//original referenced name was replaceSprite (but this one is probably
//more accurate since it does create something rather than replacing)
const u32 Func_CreateThingy = 0x00488210;
CThingy* createThingy(u32 spriteId, s16 x, s16 y, u32 playerId) {

	static CThingy* thingy;
	s32 x_ = x;

	__asm {
		PUSHAD
		PUSH playerId
		MOVSX EDI, y
		PUSH x_
		PUSH spriteId
		CALL Func_CreateThingy
		MOV thingy, EAX
		POPAD
	}

	return thingy;

}

;

const u32 Func_clearUnitSprites = 0x00496F00;
void clearUnitSprites(CSprite* unit_sprite, CSprite* subunit_sprite) {

	__asm {
		PUSHAD
		MOV ECX, subunit_sprite
		MOV ESI, unit_sprite
		CALL Func_clearUnitSprites
		POPAD
	}

}

;

const u32 Func_Sub497480 = 0x00497480;
void function_00497480(CSprite* thingySprite, u32 visibility_flags) {

	__asm {
		PUSHAD
		MOV EBX, thingySprite
		PUSH visibility_flags
		CALL Func_Sub497480
		POPAD
	}

}

;

const u32 Func__UnitDestructor = 0x004A0740;
void _UnitDestructor(CUnit* unit) {

	__asm {
		PUSHAD
		MOV EAX, unit
		CALL Func__UnitDestructor
		POPAD
	}

}

;

const u32 Func_UnitDestructor = 0x004A0990;
void UnitDestructor(CUnit* unit) {

	__asm {
		PUSHAD
		MOV ECX, unit
		CALL Func_UnitDestructor
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

} //Unnamed namespace

//End of helper functions

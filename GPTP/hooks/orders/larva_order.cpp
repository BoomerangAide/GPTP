#include "larva_order.h"
#include <SCBW/api.h>

//helper functions def
namespace {

bool function_004E8BC0(int x, int y);				//E8BC0
bool ChkLarvaStray(CUnit* unit, int x, int y);		//E93E0
void ChkLarvaStray_0(CUnit* unit, int* x, int* y);	//E94B0
bool moveToTarget(CUnit* unit, CUnit* target);		//EB720
bool function_004EB9C0(CUnit* unit, int x, int y);	//EB9C0

} //unnamed namespace

namespace hooks {

//Order performed by the lava, making it roam around
//near the main building while staying on creep, and
//dying if off creep.
void orders_Larva(CUnit* unit) {

	static u32*			u32_0051C660				= (u32*)0x0051C660;
	static u32*			randomnessCounter_0051CA18	= (u32*)0x0051CA18;
	static u16* const	maxBoxRightValue			= (u16*)0x00628450;	//should usually be mapTileSize->width * 32
	static u16* const	maxBoxBottomValue			= (u16*)0x006284B4;	//should usually be mapTileSize->height * 32

	bool bEndThere = false;
	CUnit* connectedUnit = unit->connectedUnit;

	if (
		connectedUnit != NULL &&
		!(unit->isTargetWithinMinRange(connectedUnit,10))
	) 
	{

		moveToTarget(unit, connectedUnit);

		if (unit->nextTargetWaypoint != connectedUnit->sprite->position) {
			unit->nextTargetWaypoint.x = connectedUnit->sprite->position.x;
			unit->nextTargetWaypoint.y = connectedUnit->sprite->position.y;
		}

	}

	/*Ignored a bugged unit->status & UnitStatus::Unmovable check here*/

	if (unit->sprite->position != unit->moveTarget.pt) {
		bEndThere = 
			(
				(unit->moveTarget.unit == NULL) || 
				(unit->moveTarget.pt == (unit->moveTarget.unit)->sprite->position) ||
				function_004E8BC0(unit->moveTarget.pt.x, unit->moveTarget.pt.y)
			);
	}

	if (!bEndThere) {

		if ((connectedUnit == NULL) || (unit->moveTarget.unit != connectedUnit)) {

			int posX = (s16)unit->sprite->position.x, posY = (s16)unit->sprite->position.y;
			int tileIndex;

			if (posY < 0)
				posY = (posY + 31) / 32;
			else
				posY = posY / 32;

			if (posX < 0)
				posX = (posX + 31) / 32;
			else
				posX = posX / 32;

			tileIndex = posY * mapTileSize->width + posX;

			if (!((*activeTileArray)[tileIndex].hasCreep)) {
				unit->remove();
				bEndThere = true;
			}


		}

		if (!bEndThere) {

			int randX, randY;
			u32 random_value;

			if (*IS_IN_GAME_LOOP == 0)
				random_value = 0;
			else
			{

				*u32_0051C660 = *u32_0051C660 + 1;
				*randomnessCounter_0051CA18 = *randomnessCounter_0051CA18 + 1;
				*lastRandomNumber = (*lastRandomNumber * 0x015A4E35) + 1;

				random_value = (*lastRandomNumber / 0x10000) & 0x7FFF;

			}

			randX = (s16)unit->sprite->position.x;
			randY = (s16)unit->sprite->position.y;

			//RNG calculations are scary, better leave them as asm, less likely
			//to get hellishly hard to detect and fix errors
			__asm {

				PUSH EAX
				PUSH EDX
				PUSH ESI
				PUSH EDI

				MOV EAX, random_value
				MOV EDX, random_value
				AND DL, 0x08
				NEG DL
				SBB EDX, EDX
				AND EDX, 0x14
				AND AL, 0x80
				ADD EDX, -0x0A
				ADD EDX, randX
				NEG AL
				MOV ESI, EDX
				MOV randX, ESI
				SBB EAX, EAX
				AND EAX, 0x14
				ADD EAX, -0x0A
				ADD EAX, randY
				MOV EDI, EAX
				MOV randY, EDI

				POP EDI
				POP ESI
				POP EDX
				POP EAX

			}

			if (
				((s16)randX) < *maxBoxRightValue &&
				((s16)randY) < *maxBoxBottomValue &&
				function_004E8BC0(randX, randY) != 0
			)
			{

				if (!ChkLarvaStray(unit, randX, randY)) {

					bEndThere = ChkLarvaStray(unit, unit->sprite->position.x, unit->sprite->position.y);

					if (!bEndThere)
						ChkLarvaStray_0(unit, &randX, &randY);

				}

				if (!bEndThere)
					function_004EB9C0(unit, randX, randY);

			}

		}


	}

}

;

} //hooks

;

//-------- Helper function definitions. Do NOT modify! --------//

namespace {

const u32 Func_Sub4E8BC0 = 0x004E8BC0;
//Possibly check if there's creep at that location
bool function_004E8BC0(int x, int y) {

	static Bool32 bPreResult;

	__asm {
		PUSHAD
		MOV EAX, y
		PUSH x
		CALL Func_Sub4E8BC0
		MOV bPreResult, EAX
		POPAD
	}

	return (bPreResult != 0);

}

;

const u32 Func_ChkLarvaStray = 0x004E93E0;
bool ChkLarvaStray(CUnit* unit, int x, int y) {

	static Bool32 bPreResult;

	__asm {
		PUSHAD
		PUSH y
		PUSH x
		MOV EBX, unit
		CALL Func_ChkLarvaStray
		MOV bPreResult, EAX
		POPAD
	}

	return (bPreResult != 0);

}

;

const u32 Func_ChkLarvaStray_0 = 0x004E94B0;
//rather than checking, modify the x and y underlying values
void ChkLarvaStray_0(CUnit* unit, int* x, int* y) {
	__asm {
		PUSHAD
		PUSH y
		PUSH x
		PUSH unit
		CALL Func_ChkLarvaStray_0
		POPAD
	}
}

;

const u32 Func_moveToTarget = 0x004EB720;
bool moveToTarget(CUnit* unit, CUnit* target) {

	static Bool32 bPreResult;

	__asm {
		PUSHAD
		MOV ESI, unit
		MOV EDI, target
		CALL Func_moveToTarget
		MOV bPreResult, EAX
		POPAD
	}

	return (bPreResult != 0);

}

;

const u32 Func_Sub4EB9C0 = 0x004EB9C0;
bool function_004EB9C0(CUnit* unit, int x, int y) {

	static Bool32 bPreResult;

	__asm {
		PUSHAD
		MOV EDX, unit
		MOV ECX, x
		MOV EAX, y
		CALL Func_Sub4EB9C0
		MOV bPreResult, EAX
		POPAD
	}

	return (bPreResult != 0);

}

;

} //Unnamed namespace

//End of helper functions

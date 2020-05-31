#include "selection.h"
#include <hook_tools.h>

namespace {
	
	void __declspec(naked) SortAllUnits_Wrapper() {

		static CUnit* unit;
		static CUnit** unit_list;
		static CUnit** units_in_bounds;
		static u32 result;

		__asm {
			PUSH EBP
			MOV EBP, ESP
			SUB ESP, 0x08
			MOV EAX, [EBP+0x10]
			MOV unit, EAX
			MOV EAX, [EBP+0x0C]
			MOV unit_list, EAX
			MOV EAX, [EBP+0x08]
			MOV units_in_bounds, EAX
			PUSHAD
		}

		result = hooks::SortAllUnits(unit,unit_list,units_in_bounds);

		__asm {
			POPAD
			MOV EAX, result
			MOV ESP, EBP
			POP EBP
			RETN 0x0C
		}

	}

	;

	void __declspec(naked) combineSelectionsLists_Wrapper() {

		static CUnit* unit;
		static CUnit** unit_list_1;
		static CUnit** unit_list_2;
		static u32 list_length;
		static u32 result;

		__asm {

			PUSH EBP
			MOV EBP, ESP

			MOV EAX, [EBP+0x10]
			MOV unit, EAX

			MOV EAX, [EBP+0x0C]
			MOV list_length, EAX

			MOV EAX, [EBP+0x08]
			MOV unit_list_1, EAX

			MOV unit_list_2, EDI

			PUSHAD

		}

		result = hooks::combineSelectionsLists(unit,unit_list_1,unit_list_2,list_length);

		__asm {
			POPAD
			MOV EAX, result
			MOV ESP, EBP
			POP EBP
			RETN 0x0C
		}

	}

	;

	void __declspec(naked) getSelectedUnitsInBox_Wrapper() {

		static Box16* coords;

		__asm {
			PUSH EBP
			MOV EBP, ESP
			MOV EAX, [EBP+0x08]
			MOV coords, EAX
			PUSHAD
		}

		hooks::getSelectedUnitsInBox(coords);

		__asm {
			POPAD
			MOV ESP, EBP
			POP EBP
			RETN 4
		}

	}

	;

	void __declspec(naked) getSelectedUnitsAtPoint_Wrapper() {

		static u32 unk1, unk2;

		__asm {
			PUSH EBP
			MOV EBP, ESP
			MOV unk1, ECX
			MOV unk2, EDX
			PUSHAD
		}

		hooks::getSelectedUnitsAtPoint(unk1, unk2);

		__asm {
			POPAD
			MOV ESP, EBP
			POP EBP
			RETN
		}

	}

	;

	//Moddable here
	const u32 Func_unit_IsStandardAndMovable = 0x0047B770;
	void __declspec(naked) CallToUnit_IsStandardAndMovable_Wrapper() {

		static CUnit* unit;
		static Bool32 return_value;

		//Do not change this
		__asm {
			MOV unit, ECX
			PUSHAD
		}

		//Replace this section by a call to a modded function,
		//to use a replacement for the call to unit_IsStandardAndMovable
		//in the centerviewUnitGroup function that could not be hooked
		//due to its complexity
		__asm {
			MOV ECX, unit
			CALL Func_unit_IsStandardAndMovable
			MOV return_value, EAX
		}

		//Do not change this
		__asm {
			POPAD
			MOV EAX, return_value
			RETN
		}

	}

	;

	void __declspec(naked) selectUnitGroup_Wrapper() {

		static u32 selectionGroupNumber;

		__asm {
			PUSH EBP
			MOV EBP, ESP
			MOV EAX, [EBP+0x08]
			MOV selectionGroupNumber, EAX
			PUSHAD
		}

		hooks::selectUnitGroup(selectionGroupNumber);

		__asm {
			POPAD
			MOV ESP, EBP
			POP EBP
			RETN 4
		}

	}

	;

}//unnamed namespace

namespace hooks {

	void injectSelectMod() {
		jmpPatch(SortAllUnits_Wrapper,						0x0046F0F0,	 1);
		jmpPatch(combineSelectionsLists_Wrapper,			0x0046F290,	 1);
		jmpPatch(getSelectedUnitsInBox_Wrapper,				0x0046FA40,	 1);
		jmpPatch(getSelectedUnitsAtPoint_Wrapper,			0x0046FB40, 27);
		callPatch(CallToUnit_IsStandardAndMovable_Wrapper,	0x004968C3,  0);
		jmpPatch(selectUnitGroup_Wrapper,					0x00496B40,  1);
	}

}

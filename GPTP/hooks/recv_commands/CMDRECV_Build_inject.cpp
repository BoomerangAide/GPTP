//Injector source file for the CMDRECV Build hooks module.
#include "CMDRECV_Build.h"
#include <hook_tools.h>

namespace {

void __declspec(naked) CMDRECV_PlaceBuildingAllowed_Wrapper() {

	static CUnit* builder;
	static u8 orderId;
	static u16 builtUnitId;
	static Bool32 bReturnValue;

	__asm {
		MOV builder, ECX
		MOV orderId, DL
		MOV builtUnitId, AX
		PUSHAD
	}

	bReturnValue = (hooks::CMDRECV_PlaceBuildingAllowed(builder,orderId,builtUnitId)) ? 1 : 0;

	__asm {
		POPAD
		MOV EAX, bReturnValue
		RETN
	}

}

;

void __declspec(naked) cmdRECV_PlaceBuildingOrder_Wrapper() {

	static CUnit* builder;
	static u32 orderId;
	static u32 builtUnitId;
	static Point16 coords;

	__asm {

		PUSH EBP
		MOV EBP, ESP

		MOV builder, EAX

		MOVSX EAX, WORD PTR [EBP+0x0C]
		MOV coords.x, AX

		MOVSX EAX, WORD PTR [EBP+0x0E]
		MOV coords.y, AX

		MOV EAX, [EBP+0x08]
		MOV orderId, EAX

		MOV EAX, [EBP+0x10]
		MOV builtUnitId, EAX

		PUSHAD

	}

	hooks::cmdRECV_PlaceBuildingOrder(builder,builtUnitId,coords,orderId);

	__asm {
		POPAD
		MOV ESP, EBP
		POP EBP
		RETN 0xC
	}

}

;

void __declspec(naked) cmdRECV_PlaceBuildingNormal_Wrapper() {

	static s16 x;
	static s16 y;
	static u32 orderId;
	static u32 builtUnitId;

	__asm {

		PUSH EBP
		MOV EBP, ESP

		MOV AX, [EBP+0x08]
		MOV x, AX

		MOV AX, [EBP+0x0A]
		MOV y, AX

		MOV EAX, [EBP+0x0C]
		MOV orderId, EAX

		MOV EAX, [EBP+0x10]
		MOV builtUnitId, EAX

		PUSHAD

	}

	hooks::cmdRECV_PlaceBuildingNormal(x,y,orderId,builtUnitId);

	__asm {
		POPAD
		MOV ESP, EBP
		POP EBP
		RETN 0x0C
	}

}

;

void __declspec(naked) cmdRECV_PlaceBuildingAddon_Wrapper() {

	static s16 x;
	static s16 y;
	static u32 orderId;
	static u32 builtUnitId;

	__asm {

		PUSH EBP
		MOV EBP, ESP

		MOV AX, [EBP+0x08]
		MOV x, AX

		MOV AX, [EBP+0x0A]
		MOV y, AX

		MOV EAX, [EBP+0x0C]
		MOV orderId, EAX

		MOV EAX, [EBP+0x10]
		MOV builtUnitId, EAX

		PUSHAD

	}

	hooks::cmdRECV_PlaceBuildingAddon(x,y,orderId,builtUnitId);

	__asm {
		POPAD
		MOV ESP, EBP
		POP EBP
		RETN 0x0C
	}

}

;

void __declspec(naked) CMDRECV_Build_Wrapper() {

	static u8 orderId;
	static s16 x;
	static s16 y;
	static u16 builtUnitId;

	__asm {

		MOV AL, [ESI+0x01]
		MOV orderId, AL

		MOV AX, [ESI+0x02]
		MOV x, AX

		MOV AX, [ESI+0x04]
		MOV y, AX

		MOV AX, [ESI+0x06]
		MOV builtUnitId, AX

		PUSHAD

	}

	hooks::CMDRECV_Build(orderId,x,y,builtUnitId);

	__asm {
		POPAD
		RETN
	}

}

;

}//unnamed namespace

namespace hooks {

	void injectCMDRECV_BuildHooks() {
		jmpPatch(CMDRECV_PlaceBuildingAllowed_Wrapper,	0x0048DBD0, 1);
		jmpPatch(cmdRECV_PlaceBuildingNormal_Wrapper,	0x0048E010, 0);
		jmpPatch(cmdRECV_PlaceBuildingAddon_Wrapper,	0x0048E0A0, 0);
		jmpPatch(CMDRECV_Build_Wrapper,					0x004C23C0, 3);
	}

} //hooks

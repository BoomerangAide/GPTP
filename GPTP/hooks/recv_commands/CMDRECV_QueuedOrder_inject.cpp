//Injector source file for the CMDRECV Queued Order hooks module.
#include "CMDRECV_QueuedOrder.h"
#include <hook_tools.h>

namespace {

void __declspec(naked) queueNewOrder_Wrapper() {

	static CUnit* unit;
	static u32 orderId;
	static int x;
	static int y;
	static CUnit* target;
	static u32 unitId;
	static Point16 unkPos1;
	static Point16 unkPos2;
	static u32 unkQueuedOrderType;

	__asm {

		PUSH EBP
		MOV EBP, ESP

		MOV unkQueuedOrderType, EAX

		MOV EAX, [EBP+0x1C]
		MOV unkPos1, EAX

		MOV EAX, [EBP+0x18]
		MOV unkPos2, EAX

		MOV EAX, [EBP+0x14]
		MOV unitId, EAX

		MOV EAX, [EBP+0x10]
		MOV target, EAX	

		MOV EAX, [EBP+0x0C]
		MOV orderId, EAX

		MOV EAX, [EBP+0x08]
		MOV unit, EAX

		MOV x, EDX
		MOV y, ECX

		PUSHAD

	}

	hooks::queueNewOrder(unit, orderId, x, y, target, unitId, unkPos1, unkPos2, unkQueuedOrderType);

	__asm {
		POPAD
		POP EBP
		RETN 0x18
	}

}

;

void __declspec(naked) function_004754F0Wrapper() {

	static CUnit* unit;
	static u32 orderId;
	static int x;
	static int y;
	static CUnit* target;
	static u32 unitId;
	static Point16 unkPos1;
	static Point16 unkPos2;
	static Bool32 isQueued;
	static u32 unkQueuedOrderType;

	__asm {

		PUSH EBP
		MOV EBP, ESP

		MOV ECX, [EBP+0x1C]
		MOV unkPos1, ECX

		MOV ECX, [EBP+0x18]
		MOV unkPos2, ECX

		MOV ECX, [EBP+0x14]
		MOV target, ECX

		MOV ECX, [EBP+0x0C]
		MOV isQueued, ECX

		MOV ECX, [EBP+0x08]
		MOV unkQueuedOrderType, ECX

		MOV ECX, [EBP+0x10]
		MOV orderId, ECX	

		MOV unit, ESI
		MOV unitId, EBX
		MOV x, EDX
		MOV y, EAX

		PUSHAD

	}

	hooks::function_004754F0(unit, orderId, x, y, target, unitId, unkPos1, unkPos2, isQueued, unkQueuedOrderType);

	__asm {
		POPAD
		POP EBP
		RETN 0x18
	}

}

;

}//unnamed namespace

namespace hooks {

void injectCMDRECV_QueuedOrderHooks() {
	jmpPatch(queueNewOrder_Wrapper,		0x00474B90, 1);
	jmpPatch(function_004754F0Wrapper,	0x004754F0, 1);
}

} //hooks
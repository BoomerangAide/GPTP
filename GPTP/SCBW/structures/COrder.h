#pragma once
#include "Target.h"

#pragma pack(1)

struct COrder {
/*00*/  COrder*   prev;
/*04*/  COrder*   next;
/*08*/  u16       orderId;
/*0A*/  u16       unitId;
/*0C*/  Target    target;
/*0C*/			/*target.pt.x*/
/*0E*/			/*target.pt.y*/	
/*10*/			/*target.unit*/
};

C_ASSERT(sizeof(COrder) == 20);
//static_assert(sizeof(COrder) == 20, "The size of the COrder structure is invalid");

#pragma pack()

#pragma once
#include "../SCBW/structures/CUnit.h"

struct DetectorCheckParam {
  u32 visionFlags;
  CUnit* target;
};

C_ASSERT(sizeof(DetectorCheckParam) == 8);

namespace hooks {

bool unitCanDetectHook(CUnit* unit);											//03430
void getCloakedTargetVisibility(CUnit* detector, DetectorCheckParam* param);	//408A0

void injectDetectorHooks();

} //hooks

#pragma once
#include "../SCBW/structures/CUnit.h"

namespace hooks {

void transferResourceToWorkerHook(CUnit* worker, CUnit* resource);	//696D0

void injectHarvestResource();

} //hooks

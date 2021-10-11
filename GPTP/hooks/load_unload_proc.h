#include <SCBW/structures/CUnit.h>

namespace hooks {

void loadUnitProc(CUnit* unit, CUnit* unitToLoad);	//E78E0
Bool32 unloadUnitProc(CUnit* unit);					//E7F70

void injectLoadUnloadProcHooks();

} //hooks

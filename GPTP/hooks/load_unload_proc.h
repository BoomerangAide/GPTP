#include <SCBW/structures/CUnit.h>

namespace hooks {

void loadUnitProc(CUnit* unit, CUnit* unitToLoad);	//004E78E0
Bool32 unloadUnitProc(CUnit* unit);					//004E7F70

void injectLoadUnloadProcHooks();

} //hooks

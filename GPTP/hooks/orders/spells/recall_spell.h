#pragma once
#include "../../../SCBW/structures/CUnit.h"

namespace hooks {

	void spells_Recall_ActionOnValidTargetForRecall(CUnit* target, CUnit* caster);	//942D0
	void spells_Recall_ActionOnTargetInRange(CUnit* target, CUnit* caster);			//94400
	void orders_Recall(CUnit* unit);												//94470

	void injectRecallSpellHooks();

} //hooks

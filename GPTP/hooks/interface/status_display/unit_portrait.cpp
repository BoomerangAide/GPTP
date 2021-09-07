#include "unit_portrait.h"
#include <SCBW/api.h>

namespace hooks {

//0045E320  setBuildingSelPortrait

//It is supposed to use only unitId, but
//whenever it's called, the CUnit* data
//always exist, so it make it useful for
//modding even non-buildings
u16 setBuildingSelPortrait(CUnit* unit, u16 unitId) {

	u16* const Campaign_Missions_Amount			= (u16*)0x0057F244; //was "campaign_mission" member from CGame structure
	u16* const BW_Campaign_Advisor_PortraitId	= (u16*)0x00663024; //name is my interpretation, but really may be wrong
	u16* const SC_Campaign_Advisor_PortraitId	= (u16*)0x006630B8; //name is my interpretation, but really may be wrong

#define SC_MISSIONS_AMOUNT	31 //including Boot Camp									//name is my interpretation, but really may be wrong
#define BW_MISSIONS_AMOUNT	28 //including secret mission, and a mission in 2 versions	//name is my interpretation, but really may be wrong

	u16 portraitId_returned = units_dat::PortraitId[unitId];

	if (portraitId_returned == PortraitId::Overmind) {
		if (*Campaign_Missions_Amount > SC_MISSIONS_AMOUNT)
			portraitId_returned = *SC_Campaign_Advisor_PortraitId;
	}
	else
	if(
		portraitId_returned == PortraitId::Protoss_Advisor &&
		*Campaign_Missions_Amount >= BW_MISSIONS_AMOUNT
	)
		portraitId_returned = *BW_Campaign_Advisor_PortraitId;

	return portraitId_returned;

}

;
	
//This is a hack rather than a hook, check and modify the values as you need here.
//Testing advised, may not work the way you expect it to (like being called
//again and again without selected unit changing).
//Provided because there are several cases where setBuildingSelPortrait is
//not the source of the portraitId argument.
//
//If unkParam is 2, will display talking portrait. Not sure of other effects
//beyond that
void displayUpdatePortraitHack(u16* portraitId, CUnit** unit, u32* unkParam) {

	//INSERT CODE HERE AND ENABLE IN UNIT_PORTRAIT_INJECT.CPP

	//Example:
	/* Selected marines change portrait depending on their hp

	if (*portraitId == PortraitId::Marine)
	{

		CUnit* _unit = *unit;

		if (_unit->hitPoints > 39*256)
			*portraitId = PortraitId::Marine;
		else
		if (_unit->hitPoints > 29*256)
			*portraitId = PortraitId::Terran_Advisor;
		else
		if (_unit->hitPoints > 10*256)
			*portraitId = PortraitId::Firebat;
		else
			*portraitId = PortraitId::Infested_Terran;

	}
	
	/**/
}

;

} //namespace hooks

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

#define PORTRAIT_ID_OVERMIND		38
#define PORTRAIT_ID_PROTOSSADVISOR	60

#define SC_MISSIONS_AMOUNT	31 //including Boot Camp									//name is my interpretation, but really may be wrong
#define BW_MISSIONS_AMOUNT	28 //including secret mission, and a mission in 2 versions	//name is my interpretation, but really may be wrong

	u16 portraitId_returned = units_dat::PortraitId[unitId];

	if (portraitId_returned == PORTRAIT_ID_OVERMIND) {
		if (*Campaign_Missions_Amount > SC_MISSIONS_AMOUNT)
			portraitId_returned = *SC_Campaign_Advisor_PortraitId;
	}
	else
	if(
		portraitId_returned == PORTRAIT_ID_PROTOSSADVISOR &&
		*Campaign_Missions_Amount >= BW_MISSIONS_AMOUNT
	)
		portraitId_returned = *BW_Campaign_Advisor_PortraitId;

	return portraitId_returned;

}

;

} //namespace hooks
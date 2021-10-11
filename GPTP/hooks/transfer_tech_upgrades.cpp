//Source file for the Transfer Tech & Upgrades hook module.
//This controls how technologies and upgrades are transferred when units are
//passed from one player to another, either by triggers or casting Mind Control.
#include "transfer_tech_upgrades.h"
#include "apply_upgrade_flags.h"
#include <SCBW/api.h>

namespace {

typedef int (__stdcall *SErrSetLastErrorFunc)(int);
SErrSetLastErrorFunc const SErrSetLastError = (SErrSetLastErrorFunc) 0x00410172;

struct TechTransferData {
  u16 unitId;
  u16 relatedTech[5]; //Maximum number of tech (use TechId::None to mark a premature end-of-list)
};

const TechTransferData techTransferData[] = {
  //Terran
  {UnitId::TerranMarine, TechId::Stimpacks, TechId::None},
  {UnitId::TerranFirebat, TechId::Stimpacks, TechId::None},
  {UnitId::TerranGhost, TechId::Lockdown, TechId::PersonnelCloaking, TechId::None},
  {UnitId::TerranMedic, TechId::Healing, TechId::Restoration, TechId::OpticalFlare, TechId::None},
  {UnitId::TerranVulture, TechId::SpiderMines, TechId::None},
  {UnitId::TerranSiegeTankTankMode, TechId::TankSiegeMode, TechId::None},
  {UnitId::TerranSiegeTankSiegeMode, TechId::TankSiegeMode, TechId::None},
  {UnitId::TerranWraith, TechId::CloakingField, TechId::None},
  {UnitId::TerranScienceVessel, TechId::DefensiveMatrix, TechId::Irradiate, TechId::EMPShockwave, TechId::None},
  {UnitId::TerranBattlecruiser, TechId::YamatoGun, TechId::None},

  //Zerg
  {UnitId::ZergQueen, TechId::Infestation, TechId::Parasite, TechId::SpawnBroodlings, TechId::Ensnare, TechId::None},
  {UnitId::ZergDefiler, TechId::Burrowing, TechId::DarkSwarm, TechId::Plague, TechId::Consume, TechId::None},
  {UnitId::ZergDrone, TechId::Burrowing, TechId::None},
  {UnitId::ZergZergling, TechId::Burrowing, TechId::None},
  {UnitId::ZergHydralisk, TechId::Burrowing, TechId::LurkerAspect, TechId::None},
  {UnitId::ZergInfestedTerran, TechId::Burrowing, TechId::None},
  {UnitId::ZergLurker, TechId::LurkerAspect, TechId::None},

  //Protoss
  {UnitId::ProtossHighTemplar, TechId::ArchonWarp, TechId::PsionicStorm, TechId::Hallucination, TechId::None},
  {UnitId::ProtossArbiter, TechId::Recall, TechId::StasisField, TechId::None},
  {UnitId::ProtossCorsair, TechId::DisruptionWeb, TechId::None},
  {UnitId::ProtossDarkTemplar, TechId::DarkArchonMeld, TechId::None},
  {UnitId::ProtossDarkArchon, TechId::MindControl, TechId::Feedback, TechId::Maelstrom, TechId::None},
};

struct UpgradeTransferData {
  u16 unitId;
  u8 relatedUpgrades[4]; //Maximum number of tech (use UpgradeId::None to mark a premature end-of-list)
};

const UpgradeTransferData upgradeTransferData[] = {
  //Terran
  {UnitId::TerranMarine, UpgradeId::U_238Shells, UpgradeId::None},
  {UnitId::TerranGhost, UpgradeId::OcularImplants, UpgradeId::MoebiusReactor, UpgradeId::None},
  {UnitId::TerranMedic, UpgradeId::CaduceusReactor, UpgradeId::None},
  {UnitId::TerranVulture, UpgradeId::IonThrusters, UpgradeId::None},
  {UnitId::TerranGoliath, UpgradeId::CharonBooster, UpgradeId::None},
  {UnitId::TerranScienceVessel, UpgradeId::TitanReactor, UpgradeId::None},
  {UnitId::TerranWraith, UpgradeId::ApolloReactor, UpgradeId::None},
  {UnitId::TerranBattlecruiser, UpgradeId::ColossusReactor, UpgradeId::None},

  //Zerg
  {UnitId::ZergOverlord, UpgradeId::VentralSacs, UpgradeId::Antennae, UpgradeId::None},
  {UnitId::ZergZergling, UpgradeId::MetabolicBoost, UpgradeId::AdrenalGlands, UpgradeId::None},
  {UnitId::ZergHydralisk, UpgradeId::MuscularAugments, UpgradeId::GroovedSpines, UpgradeId::None},
  {UnitId::ZergQueen, UpgradeId::GameteMeiosis, UpgradeId::None},
  {UnitId::ZergDefiler, UpgradeId::MetasynapticNode, UpgradeId::None},
  {UnitId::ZergUltralisk, UpgradeId::AnabolicSynthesis, UpgradeId::ChitinousPlating, UpgradeId::None},

  //Protoss
  {UnitId::ProtossZealot, UpgradeId::LegEnhancements, UpgradeId::None},
  {UnitId::ProtossDragoon, UpgradeId::SingularityCharge, UpgradeId::None},
  {UnitId::ProtossHighTemplar, UpgradeId::KhaydarinAmulet, UpgradeId::None},
  {UnitId::ProtossDarkArchon, UpgradeId::ArgusTalisman, UpgradeId::None},
  {UnitId::ProtossReaver, UpgradeId::ScarabDamage, UpgradeId::ReaverCapacity, UpgradeId::None},
  {UnitId::ProtossShuttle, UpgradeId::GraviticDrive, UpgradeId::None},
  {UnitId::ProtossObserver, UpgradeId::SensorArray, UpgradeId::GraviticBoosters, UpgradeId::None},
  {UnitId::ProtossScout, UpgradeId::ApialSensors, UpgradeId::GraviticThrusters, UpgradeId::None},
  {UnitId::ProtossCarrier, UpgradeId::CarrierCapacity, UpgradeId::None},
  {UnitId::ProtossArbiter, UpgradeId::KhaydarinCore, UpgradeId::None},
  {UnitId::ProtossCorsair, UpgradeId::ArgusJewel, UpgradeId::None},
};

} //Unnamed namespace

namespace hooks {

//summon a function passed as parameter, having the same variables layout for both upgrade and tech related functions.
bool researchHelper(u32 researchTechFunc, CUnit* sourceUnit, u32 techId, u32 relativeVariable,u32 destPlayerId) {

	static u32 bPreResult;

	__asm {
		PUSHAD
		PUSH destPlayerId		//[EBP+0x14]
		PUSH relativeVariable	//[EBP+0x10] //this variable has different meaning depending on the caller (boolean or number) 
		PUSH techId				//[EBP+0x0C]
		PUSH sourceUnit			//[EBP+0x08]
		CALL researchTechFunc
		MOV bPreResult, EAX
		POPAD
	}

	return (bPreResult != 0);

} //bool researchHelper(u32 researchTechFunc, CUnit* sourceUnit, u32 techId, u32 relativeVariable, u32 destPlayerId)

;

//Transfers all upgrades related to the @p source unit to @p targetPlayerId.
//Originally upgradeAllUpgradesFromUnit @ 004E2B50
void transferUnitUpgradesToPlayerHook(CUnit* sourceUnit, u8 targetPlayerId, u32 researchUpgradesFunc) {

	const UpgradeTransferData* upgradeDataArrayPosBegin	= &upgradeTransferData[0];									//original was (UpgradeTransferData*) 0x005012E2;
	const UpgradeTransferData* upgradeDataArrayPosEnd	= &upgradeTransferData[ARRAY_SIZE(upgradeTransferData)];	//original was (UpgradeTransferData*) 0x005013DC;
	const int relatedUpgradeArraySize					= 4;

	//Stop if the source unit does not exist
	if (researchUpgradesFunc == NULL || sourceUnit == NULL)
		SErrSetLastError(87);	//0x57
	else {

		bool bBreakMainLoop = false;
		bool bBreakSubLoop;

		//Browse through UpgradeTransferData structures
		for (UpgradeTransferData* data = (UpgradeTransferData*)upgradeDataArrayPosBegin; !bBreakMainLoop && (data != upgradeDataArrayPosEnd); data++) {

			if (sourceUnit->id == data->unitId) {

				bBreakSubLoop = false;

				//Browse through upgrades for current unit
				for (int upgradeIndex = 0; !bBreakSubLoop && upgradeIndex < relatedUpgradeArraySize; upgradeIndex++) {

					u16 upgradeId = data->relatedUpgrades[upgradeIndex];

					//forced end of array, stop checking upgrades, but still check others UpgradeTransferData
					//(in case the unit is represented several times?)
					if (upgradeId == UpgradeId::None)
						bBreakSubLoop = true;
					else {

						u8 sourceUpgradeLevel;

						//check level of upgrade of source unit for transfer
						if(upgradeId >= UpgradeId::UnusedUpgrade46)
							sourceUpgradeLevel = UpgradesBw->currentLevel[sourceUnit->playerId][upgradeId - UpgradeId::UnusedUpgrade46];
						else
							sourceUpgradeLevel = UpgradesSc->currentLevel[sourceUnit->playerId][upgradeId];

						//probably increase the upgrade level of player until reaching same point as sourceUnit
						//if return false, stop checking upgrades and UpgradeTransferData
						if(!researchHelper(researchUpgradesFunc,sourceUnit,upgradeId,sourceUpgradeLevel,targetPlayerId)) {
							bBreakSubLoop = true;
							bBreakMainLoop = true;			
						}

					}

				}

			} //if (sourceUnit->id == data->unitId)

		} //for (upgradeTransferData* data = (upgradeTransferData*)upgradeDataArrayPosBegin; !bBreakMainLoop && (data != upgradeDataArrayPosEnd); data++)

	} //if (researchupgradeFunc != NULL && sourceUnit != NULL)

}

;

//Transfers all tech related to the @p source unit to @p targetPlayerId.
//Originally researchAllTechsFromUnit @ 004E2C00
void transferUnitTechToPlayerHook(CUnit* sourceUnit, u8 targetPlayerId, u32 researchTechFunc) {

	const TechTransferData* techDataArrayPosBegin	= &techTransferData[0];								//original was (TechTransferData*) 0x005011CA;
	const TechTransferData* techDataArrayPosEnd		= &techTransferData[ARRAY_SIZE(techTransferData)];	//original was (TechTransferData*) 0x005012DE;
	const int relatedTechArraySize					= 5;

	//Stop if the source unit does not exist
	if (researchTechFunc == NULL || sourceUnit == NULL)
		SErrSetLastError(87);
	else {

		bool bBreakMainLoop = false;
		bool bBreakSubLoop;

		//Browse through TechTransferData structures
		for (TechTransferData* data = (TechTransferData*)techDataArrayPosBegin; !bBreakMainLoop && (data != techDataArrayPosEnd); data++) {

			if (sourceUnit->id == data->unitId) {

				bBreakSubLoop = false;

				//Browse through techs for current unit
				for (int techIndex = 0; !bBreakSubLoop && techIndex < relatedTechArraySize; techIndex++) {

					u16 techId = data->relatedTech[techIndex];

					//forced end of array, stop checking techs, but still check others TechTransferData
					//(in case the unit is represented several times?)
					if (techId == TechId::None)
						bBreakSubLoop = true;
					else {

						Bool8 bIsResearched;

						//check if tech is researched by source unit for transfer
						if(techId >= TechId::Restoration)
							bIsResearched = TechBw->isResearched[sourceUnit->playerId][techId - TechId::Restoration];
						else
							bIsResearched = TechSc->isResearched[sourceUnit->playerId][techId];

						//probably research the tech of unit if not already researched by player
						//if return false, stop checking techs and TechTransferData
						if(!researchHelper(researchTechFunc,sourceUnit,techId,bIsResearched,targetPlayerId)) {
							bBreakSubLoop = true;
							bBreakMainLoop = true;			
						}

					}

				}

			} //if (sourceUnit->id == data->unitId)

		} //for (TechTransferData* data = (TechTransferData*)techDataArrayPosBegin; !bBreakMainLoop && (data != techDataArrayPosEnd); data++)

	} //if (researchTechFunc != NULL && sourceUnit != NULL)

} //void transferUnitTechToPlayerHook(CUnit* sourceUnit, u8 targetPlayerId, u32 researchTechFunc)

;

} //hooks

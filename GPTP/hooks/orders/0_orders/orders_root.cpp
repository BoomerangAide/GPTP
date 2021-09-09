#include "orders_root.h"
#include <SCBW/api.h>

//helper functions def

namespace {

void EAX_order(CUnit* unit, u32 order_func_offset);
void EBX_order(CUnit* unit, u32 order_func_offset);
void ECX_order(CUnit* unit, u32 order_func_offset);
void EDI_order(CUnit* unit, u32 order_func_offset);
void ESI_order(CUnit* unit, u32 order_func_offset);
void PUSH_order(CUnit* unit, u32 order_func_offset);

} //unnamed namespace

#ifndef NOTVALID
#define NOTVALID 0
#endif

namespace Func_Type {
enum Enum : u8 {
	EAX,
	EBX,
	ECX,
	EDI,
	ESI,
	PUSH
};
}

static u32 mainOrder_func_offsets[][3] {
	{ 0x00479480,	/*OrderId::Die*/					Func_Type::PUSH,0},
	{ 0x0047BBA0,	/*OrderId::Stop*/					Func_Type::EAX,	2},
	{ 0x00475B90,	/*OrderId::Guard*/					Func_Type::ESI,	2},
	{ 0x004774A0,	/*OrderId::PlayerGuard*/			Func_Type::EAX,	2},
	{ 0x004777F0,	/*OrderId::TurretGuard*/			Func_Type::EAX,	1},
	{ NOTVALID,		/*OrderId::BunkerGuard*/			Func_Type::EAX,	0},
	{ 0x0047C950,	/*OrderId::Move*/					Func_Type::EAX,	2},
	{ 0x004654B0,	/*OrderId::ReaverStop*/				Func_Type::PUSH,2},
	{ NOTVALID,		/*OrderId::Attack1*/				Func_Type::EAX,	0},
	{ 0x004F6FA0,	/*OrderId::Attack2*/				Func_Type::EAX,	2},
	{ 0x00479BD0,	/*OrderId::AttackUnit*/				Func_Type::ESI,	2},
	{ 0x00477D00,	/*OrderId::AttackFixedRange*/		Func_Type::EAX,	2},
	{ NOTVALID,		/*OrderId::AttackTile*/				Func_Type::EAX,	0},
	{ NOTVALID,		/*OrderId::Hover*/					Func_Type::EAX,	0},
	{ 0x00479040,	/*OrderId::AttackMove*/				Func_Type::ESI,	2},
	{ 0x004EA4C0,	/*OrderId::InfestMine1*/			Func_Type::EAX,	2},
	{ NOTVALID,		/*OrderId::Nothing1*/				Func_Type::EAX,	0},
	{ 0x004F3EA0,	/*OrderId::Powerup1*/				Func_Type::EAX,	2},
	{ 0x00476F50,	/*OrderId::TowerGuard*/				Func_Type::EAX,	2},
	{ 0x00479150,	/*OrderId::TowerAttack*/			Func_Type::EAX,	2},
	{ 0x00463DF0,	/*OrderId::VultureMine*/			Func_Type::EAX,	2},
	{ 0x0047C4F0,	/*OrderId::StayinRange*/			Func_Type::EAX,	2},
	{ 0x00477980,	/*OrderId::TurretAttack*/			Func_Type::EAX,	1},
	{ 0x00475000,	/*OrderId::Nothing2*/				Func_Type::ECX,	2},
	{ NOTVALID,		/*OrderId::Nothing3*/				Func_Type::EAX,	0},
	{ 0x0045CF80,	/*OrderId::DroneStartBuild*/		Func_Type::EAX,	2},
	{ 0x0045E090,	/*OrderId::DroneBuild*/				Func_Type::PUSH,1},
	{ 0x004E98E0,	/*OrderId::InfestMine2*/			Func_Type::EAX,	2},
	{ 0x004F6FA0,	/*OrderId::InfestMine3*/			Func_Type::EAX,	2},
	{ 0x004EA290,	/*OrderId::InfestMine4*/			Func_Type::EAX,	2},
	{ 0x00467FD0,	/*OrderId::BuildTerran*/			Func_Type::PUSH,1},
	{ 0x004E4D00,	/*OrderId::BuildProtoss1*/			Func_Type::EDI,	1},
	{ 0x004E4F20,	/*OrderId::BuildProtoss2*/			Func_Type::EAX,	2},
	{ 0x00467A70,	/*OrderId::ConstructingBuilding*/	Func_Type::EAX,	1},
	{ 0x004673D0,	/*OrderId::Repair1*/				Func_Type::EAX,	1},
	{ 0x004F6FA0,	/*OrderId::Repair2*/				Func_Type::EAX,	2},
	{ 0x004E6880,	/*OrderId::PlaceAddon*/				Func_Type::PUSH,2},
	{ NOTVALID,		/*OrderId::BuildAddon*/				Func_Type::EAX,	0},
	{ NOTVALID,		/*OrderId::Train*/					Func_Type::EAX,	0},
	{ NOTVALID,		/*OrderId::RallyPoint1*/			Func_Type::EAX,	0},
	{ NOTVALID,		/*OrderId::RallyPoint2*/			Func_Type::EAX,	0},
	{ 0x0045DD60,	/*OrderId::ZergBirth*/				Func_Type::EAX,	1},
	{ 0x0045DEA0,	/*OrderId::Morph1*/					Func_Type::EAX,	1},
	{ 0x0045D0D0,	/*OrderId::Morph2*/					Func_Type::PUSH,2},
	{ 0x00467760,	/*OrderId::BuildSelf1*/				Func_Type::EAX,	1},
	{ 0x0045D500,	/*OrderId::ZergBuildSelf*/			Func_Type::EAX,	1},
	{ 0x0045DC20,	/*OrderId::Build5*/					Func_Type::EBX,	2},
	{ 0x004EA3E0,	/*OrderId::Enternyduscanal*/		Func_Type::EAX,	2},
	{ 0x004E4F40,	/*OrderId::BuildSelf2*/				Func_Type::EAX,	0},
	{ 0x0047C7B0,	/*OrderId::Follow*/					Func_Type::EAX,	2},
	{ 0x004666A0,	/*OrderId::Carrier*/				Func_Type::EDI,	2},
	{ 0x0047C950,	/*OrderId::CarrierIgnore1*/			Func_Type::EAX,	2},
	{ 0x00465910,	/*OrderId::CarrierStop*/			Func_Type::EAX,	2},
	{ 0x00465950,	/*OrderId::CarrierAttack1*/			Func_Type::EAX,	2},
	{ 0x004F6FA0,	/*OrderId::CarrierAttack2*/			Func_Type::EAX,	2},
	{ 0x00466720,	/*OrderId::CarrierIgnore2*/			Func_Type::EAX,	2},
	{ 0x004666A0,	/*OrderId::CarrierFight*/			Func_Type::EDI,	2},
	{ 0x004666A0,	/*OrderId::HoldPosition1*/			Func_Type::EDI,	2},
	{ 0x004665D0,	/*OrderId::Reaver*/					Func_Type::EDI,	2},
	{ 0x00465690,	/*OrderId::ReaverAttack1*/			Func_Type::EAX,	2},
	{ 0x004F6FA0,	/*OrderId::ReaverAttack2*/			Func_Type::EAX,	2},
	{ 0x004665D0,	/*OrderId::ReaverFight*/			Func_Type::EDI,	2},
	{ 0x004665D0,	/*OrderId::ReaverHold*/				Func_Type::EDI,	2},
	{ NOTVALID,		/*OrderId::TrainFighter*/			Func_Type::EAX,	0},
	{ 0x00465F60,	/*OrderId::StrafeUnit1*/			Func_Type::EAX,	2},
	{ 0x00465E00,	/*OrderId::StrafeUnit2*/			Func_Type::EAX,	1},
	{ 0x00493DD0,	/*OrderId::RechargeShields1*/		Func_Type::EDI,	1},
	{ 0x00493990,	/*OrderId::Rechargeshields2*/		Func_Type::EAX,	2},
	{ NOTVALID,		/*OrderId::ShieldBattery*/			Func_Type::EAX,	0},
	{ 0x00466350,	/*OrderId::Return*/					Func_Type::EAX,	2},
	{ 0x004E9AA0,	/*OrderId::DroneLand*/				Func_Type::EAX,	2},
	{ 0x00464360,	/*OrderId::BuildingLand*/			Func_Type::EAX,	1},
	{ 0x004649B0,	/*OrderId::BuildingLiftoff*/		Func_Type::ESI,	1},
	{ NOTVALID,		/*OrderId::DroneLiftoff*/			Func_Type::EAX,	0},
	{ 0x00463AC0,	/*OrderId::Liftoff*/				Func_Type::EAX,	2},
	{ 0x004548B0,	/*OrderId::ResearchTech*/			Func_Type::EAX,	1},
	{ 0x004546A0,	/*OrderId::Upgrade*/				Func_Type::EAX,	1},
	{ 0x004E9F90,	/*OrderId::Larva*/					Func_Type::EAX,	2},
	{ NOTVALID,		/*OrderId::SpawningLarva*/			Func_Type::EAX,	0},
	{ 0x00469500,	/*OrderId::Harvest1*/				Func_Type::EAX,	2},
	{ 0x004F6FA0,	/*OrderId::Harvest2*/				Func_Type::EAX,	2},
	{ 0x00469500,	/*OrderId::HarvestGas1*/			Func_Type::EAX,	2},
	{ 0x00469000,	/*OrderId::HarvestGas2*/			Func_Type::EAX,	2},
	{ 0x00469980,	/*OrderId::HarvestGas3*/			Func_Type::EAX,	2},
	{ 0x004690C0,	/*OrderId::ReturnGas*/				Func_Type::EAX,	2},
	{ 0x00469240,	/*OrderId::MoveToMinerals*/			Func_Type::EAX,	2},
	{ 0x00468F60,	/*OrderId::HarvestMinerals2*/		Func_Type::EAX,	2},
	{ 0x004697C0,	/*OrderId::MiningMinerals*/			Func_Type::EAX,	2},
	{ 0x00468E80,	/*OrderId::Harvest3*/				Func_Type::ESI,	1},
	{ 0x00468ED0,	/*OrderId::Harvest4*/				Func_Type::EAX,	1},
	{ 0x004690C0,	/*OrderId::ReturnMinerals*/			Func_Type::EAX,	2},
	{ 0x00493920,	/*OrderId::Harvest5*/				Func_Type::EDI,	1},
	{ 0x004E7CF0,	/*OrderId::EnterTransport*/			Func_Type::EAX,	2},
	{ 0x004E7300,	/*OrderId::Pickup1*/				Func_Type::EAX,	2},
	{ 0x004E75D0,	/*OrderId::Pickup2*/				Func_Type::EDI,	2},
	{ 0x004E73B0,	/*OrderId::Pickup3*/				Func_Type::EDI,	2},
	{ 0x004E7B70,	/*OrderId::Pickup4*/				Func_Type::EAX,	2},
	{ 0x004F3E10,	/*OrderId::Powerup2*/				Func_Type::EAX,	2},
	{ 0x00464BD0,	/*OrderId::SiegeMode*/				Func_Type::EDI,	1},
	{ 0x00464AE0,	/*OrderId::TankMode*/				Func_Type::EAX,	1},
	{ 0x0047BAB0,	/*OrderId::WatchTarget*/			Func_Type::EAX,	2},
	{ 0x004E96D0,	/*OrderId::InitCreepGrowth*/		Func_Type::EAX,	2},
	{ NOTVALID,		/*OrderId::SpreadCreep*/			Func_Type::EAX,	0},
	{ 0x004E95E0,	/*OrderId::StoppingCreepGrowth*/	Func_Type::EAX,	2},
	{ NOTVALID,		/*OrderId::GuardianAspect*/			Func_Type::EAX,	0},
	{ 0x004948B0,	/*OrderId::WarpingArchon*/			Func_Type::EAX,	1},
	{ 0x00493B10,	/*OrderId::CompletingArchonSummon*/	Func_Type::ESI,	1},
	{ 0x00478D10,	/*OrderId::HoldPosition2*/			Func_Type::EAX,	2},
	{ 0x004EB5B0,	/*OrderId::HoldPosition3*/			Func_Type::ESI,	2},
	{ NOTVALID,		/*OrderId::Cloak*/					Func_Type::EAX,	0},
	{ NOTVALID,		/*OrderId::Decloak*/				Func_Type::EAX,	0},
	{ 0x004E80D0,	/*OrderId::Unload*/					Func_Type::EAX,	2},
	{ 0x004E7700,	/*OrderId::MoveUnload*/				Func_Type::PUSH,2},
	{ 0x00492850,	/*OrderId::FireYamatoGun1*/			Func_Type::EAX,	2},
	{ 0x004F6FA0,	/*OrderId::FireYamatoGun2*/			Func_Type::EAX,	2},
	{ 0x00492850,	/*OrderId::MagnaPulse*/				Func_Type::EAX,	2},
	{ 0x004E9E60,	/*OrderId::Burrow*/					Func_Type::EAX,	2},
	{ 0x004E9860,	/*OrderId::Burrowed*/				Func_Type::EAX,	2},
	{ 0x004EA670,	/*OrderId::Unburrow*/				Func_Type::EAX,	2},
	{ 0x00492850,	/*OrderId::DarkSwarm*/				Func_Type::EAX,	2},
	{ 0x00492850,	/*OrderId::CastParasite*/			Func_Type::EAX,	2},
	{ 0x00492850,	/*OrderId::SummonBroodlings*/		Func_Type::EAX,	2},
	{ 0x00492850,	/*OrderId::EmpShockwave*/			Func_Type::EAX,	2},
	{ NOTVALID,		/*OrderId::NukeWait*/				Func_Type::EAX,	0},
	{ 0x004E6700,	/*OrderId::NukeTrain*/				Func_Type::EAX,	1},
	{ 0x00464730,	/*OrderId::NukeLaunch*/				Func_Type::EAX,	2},
	{ 0x00463610,	/*OrderId::NukePaint*/				Func_Type::EAX,	2},
	{ 0x00479410,	/*OrderId::NukeUnit*/				Func_Type::EAX,	2},
	{ 0x00479200,	/*OrderId::NukeGround*/				Func_Type::PUSH,2},
	{ 0x00464D10,	/*OrderId::NukeTrack*/				Func_Type::EBX,	0},
	{ 0x00493A80,	/*OrderId::InitArbiter*/			Func_Type::ESI,	1},
	{ NOTVALID,		/*OrderId::CloakNearbyUnits*/		Func_Type::EAX,	0},
	{ 0x00464FD0,	/*OrderId::PlaceMine*/				Func_Type::EAX,	2},
	{ 0x004F6EF0,	/*OrderId::Rightclickaction*/		Func_Type::EAX,	2},
	{ 0x004788E0,	/*OrderId::SapUnit*/				Func_Type::EAX,	2},
	{ 0x00478A40,	/*OrderId::SapLocation*/			Func_Type::EAX,	2},
	{ 0x004EB5B0,	/*OrderId::HoldPosition4*/			Func_Type::ESI,	2},
	{ 0x00494470,	/*OrderId::Teleport*/				Func_Type::PUSH,2},
	{ NOTVALID,		/*OrderId::TeleporttoLocation*/		Func_Type::EAX,	0},
	{ 0x00464E40,	/*OrderId::PlaceScanner*/			Func_Type::EAX,	2},
	{ 0x00463D30,	/*OrderId::Scanner*/				Func_Type::EAX,	2},
	{ 0x004550A0,	/*OrderId::DefensiveMatrix*/		Func_Type::PUSH,2},
	{ 0x00492850,	/*OrderId::PsiStorm*/				Func_Type::EAX,	2},
	{ 0x00492850,	/*OrderId::Irradiate*/				Func_Type::EAX,	2},
	{ 0x00492850,	/*OrderId::Plague*/					Func_Type::EAX,	2},
	{ 0x00492850,	/*OrderId::Consume*/				Func_Type::EAX,	2},
	{ 0x00492850,	/*OrderId::Ensnare*/				Func_Type::EAX,	2},
	{ 0x00492850,	/*OrderId::StasisField*/			Func_Type::EAX,	2},
	{ 0x004F6C40,	/*OrderId::Hallucianation1*/		Func_Type::EDI,	2},
	{ NOTVALID,		/*OrderId::Hallucination2*/			Func_Type::EAX,	0},
	{ 0x004671B0,	/*OrderId::ResetCollision1*/		Func_Type::EAX,	1},
	{ 0x0042E320,	/*OrderId::ResetCollision2*/		Func_Type::EAX,	1},
	{ 0x004780F0,	/*OrderId::Patrol*/					Func_Type::EAX,	2},
	{ 0x004E4210,	/*OrderId::CTFCOPInit*/				Func_Type::EAX,	2},
	{ 0x004E41A0,	/*OrderId::CTFCOP1*/				Func_Type::ESI,	2},
	{ 0x004E3FB0,	/*OrderId::CTFCOP2*/				Func_Type::EAX,	1},
	{ 0x004A28B0,	/*OrderId::ComputerAI*/				Func_Type::EAX,	2},
	{ 0x00478DE0,	/*OrderId::AtkMoveEP*/				Func_Type::EAX,	2},
	{ 0x00478EC0,	/*OrderId::HarassMove*/				Func_Type::EAX,	2},
	{ 0x004A1D80,	/*OrderId::AIPatrol*/				Func_Type::ESI,	2},
	{ 0x004778E0,	/*OrderId::GuardPost*/				Func_Type::EAX,	2},
	{ 0x004A1EF0,	/*OrderId::RescuePassive*/			Func_Type::EAX,	2},
	{ 0x004A1C20,	/*OrderId::Neutral*/				Func_Type::EDI,	2},
	{ 0x00478490,	/*OrderId::ComputerReturn*/			Func_Type::EAX,	2},
	{ 0x00493F70,	/*OrderId::InitPsiProvider*/		Func_Type::EAX,	2},
	{ 0x00475710,	/*OrderId::SelfDestrucing*/			Func_Type::EAX,	1},
	{ 0x0047C3C0,	/*OrderId::Critter*/				Func_Type::EAX,	1},
	{ 0x0047BF80,	/*OrderId::HiddenGun*/				Func_Type::EAX,	2},
	{ 0x0047BCD0,	/*OrderId::OpenDoor*/				Func_Type::ESI,	2},
	{ 0x0047BC50,	/*OrderId::CloseDoor*/				Func_Type::ESI,	2},
	{ 0x0047C0A0,	/*OrderId::HideTrap*/				Func_Type::EAX,	2},
	{ 0x0047C1B0,	/*OrderId::RevealTrap*/				Func_Type::EAX,	2},
	{ 0x0047BE80,	/*OrderId::Enabledoodad*/			Func_Type::EAX,	2},
	{ 0x0047BD60,	/*OrderId::Disabledoodad*/			Func_Type::EAX,	2},
	{ 0x004E4C70,	/*OrderId::Warpin*/					Func_Type::EDI,	0},
	{ 0x00463900,	/*OrderId::Medic*/					Func_Type::EAX,	2},
	{ 0x00464180,	/*OrderId::MedicHeal1*/				Func_Type::EAX,	1},
	{ 0x004637B0,	/*OrderId::HealMove*/				Func_Type::PUSH,1},
	{ 0x00464050,	/*OrderId::MedicHoldPosition*/		Func_Type::ESI,	1},
	{ 0x00463740,	/*OrderId::MedicHeal2*/				Func_Type::EDI,	1},
	{ 0x00492850,	/*OrderId::Restoration*/			Func_Type::EAX,	2},
	{ 0x00492850,	/*OrderId::CastDisruptionWeb*/		Func_Type::EAX,	2},
	{ 0x004F6950,	/*OrderId::CastMindControl*/		Func_Type::PUSH,2},
	{ 0x00494690,	/*OrderId::WarpingDarkArchon*/		Func_Type::EAX,	1},
	{ 0x004F6D40,	/*OrderId::CastFeedback*/			Func_Type::PUSH,2},
	{ 0x00492850,	/*OrderId::CastOpticalFlare*/		Func_Type::EAX,	2},
	{ 0x00492850,	/*OrderId::CastMaelstrom*/			Func_Type::EAX,	2},
	{ 0x0047C210,	/*OrderId::JunkYardDog*/			Func_Type::EAX,	2},
	{ NOTVALID,		/*OrderId::Fatal*/					Func_Type::EAX,	0}
};

namespace hooks {
	
//Called from a global unitUpdate function
//Deal with some automatic orders like
//being in a bunker
void ordersIDCases(CUnit* unit) {

	switch (unit->mainOrderId)
	{

		case OrderId::Die:
			PUSH_order(unit, mainOrder_func_offsets[unit->mainOrderId][0]);
			break;

		case OrderId::TurretAttack:
		case OrderId::Nothing2:
		case OrderId::Neutral:
		case OrderId::Medic:
		case OrderId::MedicHeal1:
			break;

		case OrderId::PlayerGuard:
		case OrderId::TurretGuard:
		case OrderId::StayinRange:
		case OrderId::EnterTransport:
			if (unit->status & UnitStatus::InBuilding)
				unit->orderComputerCL(OrderId::BunkerGuard);
			else
				unit->orderComputerCL(OrderId::Nothing2);
			break;

				case OrderId::HarvestGas3:
				case OrderId::NukeLaunch:
		case OrderId::InfestMine4:
				case OrderId::ResetCollision1:
				case OrderId::ResetCollision2:
		case OrderId::Powerup1:
		case OrderId::Powerup2:
					EAX_order(unit, mainOrder_func_offsets[unit->mainOrderId][0]);
					break;

				default:
					if (unit->orderQueueTimer != 0)
						unit->orderQueueTimer--;
					else {

						unit->orderQueueTimer = 8;

						if (unit->mainOrderId == OrderId::BunkerGuard)
							EAX_order(unit, 0x004790A0); //see unitCanAttackInsideBunker in hooks\bunker_hooks
						else
						if(unit->mainOrderId == OrderId::Pickup4)
							EAX_order(unit, mainOrder_func_offsets[OrderId::Pickup4][0]);
						else
						if (unit->mainOrderId == OrderId::ComputerAI) {
							if(unit->status & UnitStatus::InBuilding)
								EAX_order(unit, 0x004790A0); //see unitCanAttackInsideBunker in hooks\bunker_hooks
						}
						else
						if(unit->mainOrderId == OrderId::RescuePassive)
							EAX_order(unit, mainOrder_func_offsets[OrderId::RescuePassive][0]);

					}
					break;

	}

}

;

//Also known as 004EC170 performSecondaryOrders
//Less complex than the one about main orders, so
//not using any array. Still using a switch though.
void secondaryOrdersRoot(CUnit* unit) {
	
	if (unit->secondaryOrderId == OrderId::Hallucination2)
		EAX_order(unit, 0x004F63A0);
	else
	if(
		!(unit->status & UnitStatus::DoodadStatesThing) &&
		unit->lockdownTimer == 0 &&
		unit->stasisTimer == 0 &&
		unit->maelstromTimer == 0
	)
	{

		switch (unit->secondaryOrderId)
		{

			case OrderId::BuildAddon:
				EAX_order(unit, 0x004E6790);
				break;

			case OrderId::Cloak:
				EAX_order(unit, 0x00491790);
				break;

			case OrderId::CloakNearbyUnits:
				EDI_order(unit, 0x00491C20);
				break;

			case OrderId::Decloak:
				EAX_order(unit, 0x004633E0);
				break;

			case OrderId::ShieldBattery:
				EDI_order(unit, 0x004932D0);
				break;

			case OrderId::SpawningLarva:
				ESI_order(unit, 0x004EA780);
				break;

			case OrderId::SpreadCreep:
				EAX_order(unit, 0x004EA880);
				break;

			case OrderId::TrainFighter:
				EAX_order(unit, 0x00466790);
				break;

			case OrderId::Train:
				EAX_order(unit, 0x00468420);
				break;

			default:
				break;

		}

	}

}

;

//Also known as 004EC4D0  ordersEntries
//Called between updateUnitState() (see hooks\update_unit_state) and
//performSecondaryOrders() (see secondaryOrdersRoot)
//Implementation quite different from original because hard to
//manipulate.
void mainOrdersRoot(CUnit* unit) {

	//1st part is functions that ignore disabled/stunned/other status
	//Not implemented through an array like original, since there are
	//only 4 valid cases.

	if (unit->mainOrderId == OrderId::Die)
		PUSH_order(unit, mainOrder_func_offsets[OrderId::Die][0]);
	else
	if(unit->mainOrderId == OrderId::BuildSelf2)
		EAX_order(unit,mainOrder_func_offsets[OrderId::BuildSelf2][0]);
	else
	if(unit->mainOrderId == OrderId::NukeTrack)
		EBX_order(unit,mainOrder_func_offsets[OrderId::NukeTrack][0]);
	else
	if(unit->mainOrderId == OrderId::Warpin)
		EDI_order(unit,mainOrder_func_offsets[OrderId::Warpin][0]);
	else
	if(
		unit->status & UnitStatus::DoodadStatesThing ||
		unit->lockdownTimer != 0 ||
		unit->stasisTimer != 0 ||
		unit->maelstromTimer != 0 ||
		(!(unit->status & UnitStatus::IsBuilding) && unit->status & UnitStatus::CanNotAttack)
	)
		EAX_order(unit, 0x0043FF00); //known as "getMaelstromTarget", though don't make much sense in this context
	else {

		//2nd part is functions that are used if not one of those of the 1st part and not disabled unit.
		//Ignore orderQueueTimer, and if they change the mainOrderId to something that can be used in
		//the 3rd part, then both the 2nd and 3rd part functions would indeed be used.
		if (
			mainOrder_func_offsets[unit->mainOrderId][2] == 1 &&
			mainOrder_func_offsets[unit->mainOrderId][0] != NOTVALID
		) 
		{

			switch (mainOrder_func_offsets[unit->mainOrderId][1])
			{

				case Func_Type::EAX:
					EAX_order(unit, mainOrder_func_offsets[unit->mainOrderId][0]);
					break;

				case Func_Type::EBX:
					EBX_order(unit, mainOrder_func_offsets[unit->mainOrderId][0]);
					break;

				case Func_Type::ECX:
					ECX_order(unit, mainOrder_func_offsets[unit->mainOrderId][0]);
					break;

				case Func_Type::EDI:
					EDI_order(unit, mainOrder_func_offsets[unit->mainOrderId][0]);
					break;

				case Func_Type::ESI:
					ESI_order(unit, mainOrder_func_offsets[unit->mainOrderId][0]);
					break;

				case Func_Type::PUSH:
					PUSH_order(unit, mainOrder_func_offsets[unit->mainOrderId][0]);
					break;

			}

		}

		if (unit->orderQueueTimer != 0)
			unit->orderQueueTimer--;
		else
		{

			//3rd part, executed after the 2nd part (whether it did something or not)
			//only if unit->orderQueueTimer was 0.
			//Execute "getTargetSomething" before, and has some special cases, otherwise
			//work pretty much like 2nd part

			unit->orderQueueTimer = 8;
			EAX_order(unit, 0x0043FF90); //known as "getTargetSomething"
			unit->aiActionFlags = 0;

			if (unit->mainOrderId == OrderId::HoldPosition3 || unit->mainOrderId == OrderId::HoldPosition4) {

				if (unit->mainOrderState == 0) {
					ESI_order(unit, mainOrder_func_offsets[unit->mainOrderId][0]);
					unit->mainOrderState = 1;
				}

				if(unit->orderQueueHead != NULL)
					ECX_order(unit, mainOrder_func_offsets[OrderId::Nothing2][0]);

			}
			else
			if (unit->mainOrderId == OrderId::Nothing2) {
				if(unit->orderQueueHead != NULL)
					ECX_order(unit, mainOrder_func_offsets[OrderId::Nothing2][0]);
			}
			else
			if(mainOrder_func_offsets[unit->mainOrderId][2] == 2)
				switch (mainOrder_func_offsets[unit->mainOrderId][1])
				{

					case Func_Type::EAX:
						EAX_order(unit, mainOrder_func_offsets[unit->mainOrderId][0]);
						break;

					case Func_Type::EBX:
						EBX_order(unit, mainOrder_func_offsets[unit->mainOrderId][0]);
						break;

					case Func_Type::ECX:
						ECX_order(unit, mainOrder_func_offsets[unit->mainOrderId][0]);
						break;

					case Func_Type::EDI:
						EDI_order(unit, mainOrder_func_offsets[unit->mainOrderId][0]);
						break;

					case Func_Type::ESI:
						ESI_order(unit, mainOrder_func_offsets[unit->mainOrderId][0]);
						break;

					case Func_Type::PUSH:
						PUSH_order(unit, mainOrder_func_offsets[unit->mainOrderId][0]);
						break;

				}

		}

	}

}

;


} //namespace hooks

;

//-------- Helper function definitions. Do NOT modify! --------//

namespace {

void EAX_order(CUnit* unit, u32 order_func_offset) {
	__asm {
		PUSHAD
		MOV EAX, unit
		CALL order_func_offset
		POPAD
	}
}

;

void EBX_order(CUnit* unit, u32 order_func_offset) {
	__asm {
		PUSHAD
		MOV EBX, unit
		CALL order_func_offset
		POPAD
	}
}

;

void ECX_order(CUnit* unit, u32 order_func_offset) {
	__asm {
		PUSHAD
		MOV ECX, unit
		CALL order_func_offset
		POPAD
	}
}

;

void EDI_order(CUnit* unit, u32 order_func_offset) {
	__asm {
		PUSHAD
		MOV EDI, unit
		CALL order_func_offset
		POPAD
	}
}

;

void ESI_order(CUnit* unit, u32 order_func_offset) {
	__asm {
		PUSHAD
		MOV ESI, unit
		CALL order_func_offset
		POPAD
	}
}

;

void PUSH_order(CUnit* unit, u32 order_func_offset) {
	__asm {
		PUSHAD
		PUSH unit
		CALL order_func_offset
		POPAD
	}
}

;

} //Unnamed namespace

//End of helper functions

// Contains the definitions of various data structures, arrays, pointers and
// internal constants in StarCraft.exe.
// The contents of this file are merged from: unit.cpp, triggers.cpp,
// resources.cpp, player.cpp, locations.cpp, buttons.cpp, and qdp.cpp.

#pragma once
#include "../definitions.h"
#include "structures.h"
#pragma pack(1)

#define SCBW_DATA(type, name, offset) type const name = (type)offset;

/// @name Linked list nodes
//////////////////////////////////////////////////////////////// @{

/// The first nodes in StarCraft's internal linked lists (taken from BWAPI's Offsets.h)
SCBW_DATA(CUnit**,      firstVisibleUnit,       0x00628430);
/// Includes units in transports/bunkers, workers inside gas buildings, nukes in
/// silos, queens inside command centers, and units in production.
SCBW_DATA(CUnit**,      firstHiddenUnit,        0x006283EC);
SCBW_DATA(CUnit**,      firstScannerSweep,      0x006283F4);
SCBW_DATA(CUnit**,      firstPsiFieldProvider,  0x0063FF54);
SCBW_DATA(CBullet**,    firstBullet,            0x0064DEC4);

struct Units12 { CUnit* unit[12]; };
SCBW_DATA(Units12*,     firstPlayerUnit,        0x006283F8);  //Indexed by player Id

//////////////////////////////////////////////////////////////// @}

/// @name Structure tables
//////////////////////////////////////////////////////////////// @{

SCBW_DATA(CUnit*,       unitTable,    0x0059CCA8);
SCBW_DATA(CBullet*,     bulletTable,  0x0064B2E8);
SCBW_DATA(UNITDEATHS*,  deathTable,   0x0058A364);
SCBW_DATA(UNITDEATHS*,  unit_deaths,  deathTable);
SCBW_DATA(CSprite*,     spriteTable,  0x00629D98);

//From locations.cpp
SCBW_DATA(const MapSize*, mapTileSize,    0x0057F1D4);
SCBW_DATA(LOCATION*,      locationTable,  0x0058DC60);

//for buttonsets
SCBW_DATA(BUTTON_SET*,	  buttonSetTable, 0x005187E8);

SCBW_DATA(ActionPointer*, actionTable,    0x00512800);  //From triggers.cpp

//This was in the InitializePlugin() code in qdp.cpp.
//Not sure what this does, so I'm leaving it as a comment.
//actionTable[59] = &EnableDebugModeAction;

//////////////////////////////////////////////////////////////// @}

/// @name Data Variables and Constants
//////////////////////////////////////////////////////////////// @{

//From resources.cpp
struct Resources {
  int minerals[12];
  int gas[12];
  int cumulativeGas[12];
  int cumulativeMinerals[12];
};
SCBW_DATA(Resources*,     resources,      0x0057F0F0);

/// TBL files
SCBW_DATA(const StringTbl*, statTxtTbl,         0x006D1238);
SCBW_DATA(const StringTbl*, mapStringTbl,       0x005993D4);

SCBW_DATA(const char*,	  StringEmpty,			0x00501B7D);

/// Units that are selected by the current player (or the player viewing the replay).
struct UnitsSel { CUnit* unit[SELECTION_ARRAY_LENGTH]; };
SCBW_DATA(UnitsSel*, clientSelectionGroup, 0x00597208);
SCBW_DATA(CUnit**, clientSelectionGroupEnd, 0x00597238);
SCBW_DATA(u8*,    clientSelectionCount,   0x0059723D);

/// Screen position relative to the map
SCBW_DATA(s32*,         screenX,                0x00628448);
SCBW_DATA(s32*,         screenY,                0x00628470);

///To use with a player ID
SCBW_DATA(Point16*,		startPositions,			0x0058D720);

/// Mouse position relative to the screen
SCBW_DATA(const Point32*, mouse,                0x006CDDC4);

/// Booleans indicating which keyboard buttons are held

//use IS_KEYCODE_USED[VK_param] where VK_param is one of the
//Virtual-Key Codes of Windows as seen in msdn here:
//https://msdn.microsoft.com/fr-fr/library/windows/desktop/dd375731%28v=vs.85%29.aspx
SCBW_DATA(const Bool8*,  IS_KEYCODE_USED,      0x00596A18);

SCBW_DATA(const Bool8*,  IS_HOLDING_SHIFT,      0x00596A28);
SCBW_DATA(const Bool8*,  IS_HOLDING_CTRL,       0x00596A29);
SCBW_DATA(const Bool8*,  IS_HOLDING_ALT,		0x00596A2A);

/// Maybe used by Map triggers to init a progressive movement toward the corresponding location, see GameImpl::setScreenPosition of BWAPI
SCBW_DATA(u32*,			MoveToX,				0x0062848C);
SCBW_DATA(u32*,			MoveToY,				0x006284A8);

/// Contains various information (names, player types, race types, and associated forces) of each player in the current game
SCBW_DATA(const PLAYER*,  playerTable,          0x0057EEE0);

struct SupplyData {
  u32 provided[PLAYER_COUNT];
  u32 used[PLAYER_COUNT];
  u32 max[PLAYER_COUNT];
};
SCBW_DATA(SupplyData*,    raceSupply,           0x00582144);  //Array; Use CUnit::getRace() to get the index.

SCBW_DATA(const u32*,     elapsedTimeFrames,    0x0057F23C);  //Elapsed game time in frames
SCBW_DATA(const u32*,     CountdownTimer,		0x0058D6F4);  //Countdown Timer (in seconds) (From BWAPI Offsets.h)
SCBW_DATA(const u32*,     elapsedTimeSeconds,   0x0058D6F8);  //Elapsed game time in seconds
SCBW_DATA(const u8*,      GAME_TYPE,            0x00596820);  //Part of a larger structure; Compare with GameType::Enum.
SCBW_DATA(const s32*,     ACTIVE_NATION_ID,     0x00512678);  //AKA g_ActiveNationID
SCBW_DATA(const s32*,     ACTIVE_PLAYER_ID,     0x0051267C);  //Used by GetActivePlayerNextSelectionFunc
SCBW_DATA(const s32*,     LOCAL_NATION_ID,      0x00512684);  //AKA g_LocalNationID; Actually stores the player ID.
SCBW_DATA(const s32*,     LOCAL_HUMAN_ID,       0x00512688);  //AKA g_LocalHumanID; Invalid in replay games.
SCBW_DATA(const u8*,      CURRENT_TILESET,      0x00596828);  //Tileset of current map. Compare with TilesetType::Enum
////possible values were 0057F1DC and [00596828]

// Array of 8 integers of 32 bits, thus 256 bits, one per switch
// Use scbw::getMapSwitch() and scbw::setMapSwitch() to manipulate
// a switch properly
SCBW_DATA(u32*,			  switchStatesBW,		0x0058DC40);

// Added from BWAPI Offsets.h, but made const since it would not be safe to modify those
SCBW_DATA(const char*,	  CurrentMapFileName,	0x0057FD3C);
SCBW_DATA(const char*,	  CurrentMapName,		0x0057FE40);
SCBW_DATA(const _uavail*, UnitAvailability,		0x0057F27C);

//////////////////////////////////////////////////////////////// @}

/// @name *.DAT File Data
//////////////////////////////////////////////////////////////// @{

// The following code is based on BWAPI's BW/Offsets.h
// you can get the other addresses in http://code.google.com/p/bwapi/source/browse/trunk/bwapi/BWAPI/Source/BW/Offsets.h
// Since StarCraft is already running when the plugin is loaded, the DatLoad
// structs are guaranteed to have the corrent offsets.

struct DatLoad {
  u32 address;
  u32 length;
  u32 entries;
};

namespace units_dat {
SCBW_DATA(const DatLoad*, unitsDat, 0x00513C30);

SCBW_DATA(u8*,		Graphic,				unitsDat[0].address);	//006644F8
SCBW_DATA(u16*,		SubUnit,				unitsDat[1].address);	//006607C0
//SCBW_DATA(u16*,	???,					unitsDat[2].address);	//00660C38 //Subunit 2 according to EUDDB
SCBW_DATA(u16*,		InfestedUnitPartial,	unitsDat[3].address);	//00664980, (Id - UnitId::TerranCommandCenter) for it to work, last valid id is UnitId::Special_OvermindCocoon
SCBW_DATA(u32*,		ConstructionGraphic,	unitsDat[4].address);	//006610B0
SCBW_DATA(u8*,		StartDirection,			unitsDat[5].address);	//006605F0  Direction unit will face after it is created. From 0 (top) to 31 (before top turning clockwise).32 means random.
SCBW_DATA(u8*,		ShieldsEnabled,			unitsDat[6].address);	//006647B0
SCBW_DATA(u16*,		MaxShieldPoints,		unitsDat[7].address);	//00660E00
SCBW_DATA(s32*,		MaxHitPoints,			unitsDat[8].address);	//00662350
SCBW_DATA(u8*,		Elevation,				unitsDat[9].address);	//00663150
SCBW_DATA(u8*,		MovementFlags,			unitsDat[10].address);	//00660FC8
SCBW_DATA(u8*,		Rank,					unitsDat[11].address);	//00663DD0
SCBW_DATA(u8*,		ComputerIdleOrder,		unitsDat[12].address);	//00662EA0
SCBW_DATA(u8*,		HumanIdleOrder,			unitsDat[13].address);	//00662268
SCBW_DATA(u8*,		ReturnToIdleOrder,		unitsDat[14].address);	//00664898
SCBW_DATA(u8*,		AttackUnitOrder,		unitsDat[15].address);	//00663320
SCBW_DATA(u8*,		AttackMoveOrder,		unitsDat[16].address);	//00663A50
SCBW_DATA(u8*,		GroundWeapon,			unitsDat[17].address);	//006636B8
SCBW_DATA(u8*,		MaxGroundHits,			unitsDat[18].address);	//006645E0
SCBW_DATA(u8*,		AirWeapon,				unitsDat[19].address);	//006616E0
SCBW_DATA(u8*,		MaxAirHits,				unitsDat[20].address);	//0065FC18
//SCBW_DATA(u8*,	???,					unitsDat[21].address);	//00660178 //"Computer AI Unit Type" according to EUDDB
SCBW_DATA(u32*,		BaseProperty,			unitsDat[22].address);  //00664080 AKA SpecialAbilityFlags, UnitProperty, etc. Use with UnitProperty::Enum.
SCBW_DATA(u8*,		SeekRange,				unitsDat[23].address);  //00662DB8 AKA Target Acquisition Range, Acquire Target Range, etc.
SCBW_DATA(u8*,		SightRange,				unitsDat[24].address);	//00663238
SCBW_DATA(u8*,		ArmorUpgrade,			unitsDat[25].address);	//006635D0
SCBW_DATA(u8*,		SizeType,				unitsDat[26].address);	//00662180
SCBW_DATA(u8*,		ArmorAmount,			unitsDat[27].address);	//0065FEC8
SCBW_DATA(u8*,		RightClickAction,		unitsDat[28].address);	//00662098
SCBW_DATA(u16*,		SoundReady,				unitsDat[29].address);	//00661FC0
SCBW_DATA(u16*,		SoundWhatFirst,			unitsDat[30].address);	//0065FFB0
SCBW_DATA(u16*,		SoundWhatLast,			unitsDat[31].address);	//00662BF0
SCBW_DATA(u16*,		SoundAnnoyedFirst,		unitsDat[32].address);	//00663B38
SCBW_DATA(u16*,		SoundAnnoyedLast,		unitsDat[33].address);	//00661EE8
SCBW_DATA(u16*,		SoundYesFirst,			unitsDat[34].address);	//00663C10
SCBW_DATA(u16*,		SoundYesLast,			unitsDat[35].address);	//00661440
SCBW_DATA(Point16*,	BuildingDimensions,		unitsDat[36].address);	//00662860
SCBW_DATA(Point16*,	AddonPlacement,			unitsDat[37].address);	//006626E0
SCBW_DATA(Box16*,	UnitBounds,				unitsDat[38].address);	//006617C8
SCBW_DATA(u16*,		PortraitId,				unitsDat[39].address);	//00662F88
SCBW_DATA(u16*,		MineralCost,			unitsDat[40].address);	//00663888
SCBW_DATA(u16*,		GasCost,				unitsDat[41].address);	//0065FD00
SCBW_DATA(u16*,		TimeCost,				unitsDat[42].address);	//00660428
SCBW_DATA(u16*,		unitsDat43,				unitsDat[43].address);	//00660A70 //"Restriction Address" according to EUDDB
SCBW_DATA(GroupFlag*, GroupFlags,			unitsDat[44].address);	//006637A0
SCBW_DATA(u8*,		SupplyProvided,			unitsDat[45].address);  //006646C8 Supply shown in-game is (true supply) / 2; e.g. Zerglings have 1 true supply, Marines have 2 true supply, etc.
SCBW_DATA(u8*,		SupplyRequired,			unitsDat[46].address);  //00663CE8 Supply shown in-game is (true supply) / 2; e.g. Zerglings have 1 true supply, Marines have 2 true supply, etc.
SCBW_DATA(u8*,		SpaceRequired,			unitsDat[47].address);	//00664410
SCBW_DATA(u8*,		SpaceProvided,			unitsDat[48].address);	//00660988
SCBW_DATA(u16*,		BuildScore,				unitsDat[49].address);	//00663408
SCBW_DATA(u16*,		DestroyScore,			unitsDat[50].address);	//00663EB8
SCBW_DATA(u16*,		MapStringId,			unitsDat[51].address);	//00660260
SCBW_DATA(u8*,		BroodwarOnly,			unitsDat[52].address);	//006606D8
//SCBW_DATA(u16*, ???,						unitsDat[53].address);	//00661518 //"Staredit Availability Flags" according to EUDDB
} //units_dat (last entry at 0x00513EAC)

namespace flingy_dat {
SCBW_DATA(const DatLoad*, flingyDat, 0x00515A38);

SCBW_DATA(u16*, SpriteID,           flingyDat[0].address);	//006CA318
SCBW_DATA(u32*, TopSpeed,           flingyDat[1].address);	//006C9EF8
SCBW_DATA(u16*, Acceleration,       flingyDat[2].address);	//006C9C78
SCBW_DATA(u32*, HaltDistance,       flingyDat[3].address);	//006C9930
SCBW_DATA(u8*,  TurnSpeed,          flingyDat[4].address);	//006C9E20 Incorrectly known as "Turn Radius"
//SCBW_DATA(u8*, ???,				flingyDat[5].address);	//006CA240
SCBW_DATA(u8*,  MovementControl,    flingyDat[6].address);	//006C9858
} //flingy_dat (last entry at 0x00515A80)

namespace weapons_dat {
SCBW_DATA(const DatLoad*, weaponsDat, 0x00513868);

SCBW_DATA(u16*, Label,              weaponsDat[0].address); //006572E0
SCBW_DATA(u32*, FlingyId,           weaponsDat[1].address); //00656CA8
//SCBW_DATA(u8*, ???,				weaponsDat[2].address); //006573E8 //"Special Attack" according to EUDDB
SCBW_DATA(TargetFlag*, TargetFlags, weaponsDat[3].address); //00657998
SCBW_DATA(u32*, MinRange,           weaponsDat[4].address); //00656A18
SCBW_DATA(u32*, MaxRange,           weaponsDat[5].address); //00657470
SCBW_DATA(u8*,  DamageUpgrade,      weaponsDat[6].address); //006571D0
SCBW_DATA(u8*,  DamageType,         weaponsDat[7].address); //00657258 Normal, Explosive, etc.. Use with DamageType::Enum.
SCBW_DATA(u8*,  Behavior,           weaponsDat[8].address); //00656670 Fly and follow target, appear on target unit, etc. Use with WeaponBehavior::Enum.
SCBW_DATA(u8*,  RemoveAfter,        weaponsDat[9].address); //00657040
SCBW_DATA(u8*,  ExplosionType,      weaponsDat[10].address);//006566F8 NormalHit, RadialSplash, EnemySplash, etc. Use with WeaponEffect::Enum.
SCBW_DATA(u16*, InnerSplashRadius,  weaponsDat[11].address);//00656888
SCBW_DATA(u16*, MediumSplashRadius, weaponsDat[12].address);//006570C8
SCBW_DATA(u16*, OuterSplashRadius,  weaponsDat[13].address);//00657780
SCBW_DATA(u16*, DamageAmount,       weaponsDat[14].address);//00656EB0
SCBW_DATA(u16*, DamageBonus,        weaponsDat[15].address);//00657678
SCBW_DATA(u8*,  Cooldown,           weaponsDat[16].address);//00656FB8
SCBW_DATA(u8*,  DamageFactor,       weaponsDat[17].address);//006564E0
SCBW_DATA(u8*,  AttackAngle,        weaponsDat[18].address);//00656990
SCBW_DATA(u8*,  LaunchSpin,         weaponsDat[19].address);//00657888
SCBW_DATA(u8*,  ForwardOffset,      weaponsDat[20].address);//00657910
SCBW_DATA(u8*,  VerticalOffset,     weaponsDat[21].address);//00656C20
//SCBW_DATA(u16*, ???,				weaponsDat[22].address);//00656568 //Target Error Message according to EUDDB
SCBW_DATA(u16*, WeaponIconId,		weaponsDat[23].address);//00656780
} //weapons_dat (last entry at 0x0051397C)

namespace upgrades_dat {
SCBW_DATA(const DatLoad*, upgradesDat, 0x005136E0);

SCBW_DATA(u16*, MineralCostBase,    upgradesDat[0].address);	//00656740
SCBW_DATA(u16*, MineralCostFactor,  upgradesDat[1].address);	//006559C0
SCBW_DATA(u16*, GasCostBase,        upgradesDat[2].address);	//00655840
SCBW_DATA(u16*, GasCostFactor,      upgradesDat[3].address);	//006557C0
SCBW_DATA(u16*, TimeCostBase,       upgradesDat[4].address);	//00655B80
SCBW_DATA(u16*, TimeCostFactor,     upgradesDat[5].address);	//00655940
//SCBW_DATA(u16*, ???,              upgradesDat[6].address);	//006558C0	//Restriction Flags according to EUDDB
SCBW_DATA(u16*, IconId,             upgradesDat[7].address);	//00655AC0	//"Icon, cmdicons.grp frame" according to EUDDB
SCBW_DATA(u16*, Label,              upgradesDat[8].address);	//00655A40
SCBW_DATA(u8*,  Race,               upgradesDat[9].address);	//00655BFC
SCBW_DATA(u8*,  MaxRepeats,         upgradesDat[10].address);	//00655700
//SCBW_DATA(u8*, ???,				upgradesDat[11].address);	//00655B3C	//Brood War Flag according to EUDDB
} //upgrades_dat (last entry at 0x00513764)

namespace techdata_dat {
SCBW_DATA(const DatLoad*, techdataDat, 0x005137D8);

SCBW_DATA(u16*, MineralCost,        techdataDat[0].address);	//00656248
SCBW_DATA(u16*, GasCost,            techdataDat[1].address);	//006561F0
SCBW_DATA(u16*, TimeCost,           techdataDat[2].address);	//006563D8
SCBW_DATA(u16*, EnergyCost,         techdataDat[3].address);	//00656380
//SCBW_DATA(u16*, ???,				techdataDat[4].address);	//00656198
//SCBW_DATA(u16*, ???,				techdataDat[5].address);	//006562F8	//Restriction Flags according to EUDDB
SCBW_DATA(u16*, IconId,				techdataDat[6].address);	//00656430	//"Icon, cmdicons.grp frame" according to EUDDB
SCBW_DATA(u16*, Label,              techdataDat[7].address);	//006562A0
//SCBW_DATA(u8*, ???,				techdataDat[8].address);	//00656488	//Race according to EUDDB
//SCBW_DATA(u8*, ???,				techdataDat[9].address);	//00656350	//"Researched" according to EUDDB
//SCBW_DATA(u8*, ???,				techdataDat[10].address);	//006564B4	//Brood War Flag according to EUDDB
} //techdata_dat (last entry at 0x00513850)

namespace orders_dat {
SCBW_DATA(const DatLoad*, ordersDat, 0x00513EC8);

SCBW_DATA(u16*,		Label,              ordersDat[0].address);	//00665280
SCBW_DATA(u8*,		UseWeaponTargeting, ordersDat[1].address);	//00664B00
//SCBW_DATA(u8*,	???,				ordersDat[2].address);	//00665940 //May be "secondary order" used for debug/assert only / "Order goes in bSecondaryOrderID instead of bMainOrderID." according to EUDDB
//SCBW_DATA(u8*,	???,				ordersDat[3].address);	//00665A00 //May be "Non-subunit" used for debug only
//SCBW_DATA(u8*,	???,				ordersDat[4].address);	//00664A40
//SCBW_DATA(u8*,	???,				ordersDat[5].address);	//006657C0 //May be "Subunit can use" used for debug only
SCBW_DATA(Bool8*,	CanBeInterrupted,	ordersDat[6].address);	//00665040
//SCBW_DATA(u8*,	???,				ordersDat[7].address);	//00665100
//SCBW_DATA(u8*,	???,				ordersDat[8].address);	//00665700 //"Can Be Queued" according to EUDDB
//SCBW_DATA(u8*,	???,				ordersDat[9].address);	//006651C0
SCBW_DATA(u8*,		CanBeObstructed,	ordersDat[10].address);	//006654C0 //"Can Be Obstructed?" according to EUDDB
//SCBW_DATA(u8*,	???,				ordersDat[11].address);	//00664C80
//SCBW_DATA(u8*,	???,				ordersDat[12].address);	//00664BC0 //May be "Order requires moving"
SCBW_DATA(u8*,		OrderWeaponId,		ordersDat[13].address);	//00665880
SCBW_DATA(u8*,		TechUsed,           ordersDat[14].address);	//00664E00
SCBW_DATA(u8*,		OrderIscriptAnim,	ordersDat[15].address);	//00664D40 //Animation (IscriptAnimation) according to EUDDB
SCBW_DATA(u16*,		ButtonIcon,			ordersDat[16].address);	//00664EC0
SCBW_DATA(u16*,		RequirementsOffset,	ordersDat[17].address);	//00665580 //based on context
//SCBW_DATA(u8*,	???,				ordersDat[18].address);	//00665400 //"Obscured Order" according to EUDDB
} //orders_dat (last entry at 0x00513FA0)

namespace sprites_dat {
SCBW_DATA(const DatLoad*, spritesDat, 0x00513FB8);

SCBW_DATA(u16*, ImageId,            spritesDat[0].address);		//00666160
SCBW_DATA(s8*,  HpBarSize,          spritesDat[1].address);		//00665E50
//SCBW_DATA(s8*,  ???,				spritesDat[2].address);		//00666570
SCBW_DATA(u8*,  IsVisible,          spritesDat[3].address);		//00665C48
//SCBW_DATA(s8*,  ???,				spritesDat[4].address);		//00665AC0 //"Selection Circle" according to EUDDB, not starting at 0
//SCBW_DATA(s8*,  ???,				spritesDat[5].address);		//00665FD8 //"Selection Vertical Offset" according to EUDDB
} //sprites_dat (last entry at 0x00513FF4)

namespace images_dat {
SCBW_DATA(const DatLoad*, imagesDat, 0x00514010);

//SCBW_DATA(const u32*,  ???,		imagesDat[0].address);	//00668AA0 //"GRP File" according to EUDDB
SCBW_DATA(const u8*,  IsTurnable,   imagesDat[1].address);	//0066E860
SCBW_DATA(const u8*,  IsClickable,  imagesDat[2].address);	//0066C150
//SCBW_DATA(const u8*,  ???,		imagesDat[3].address);	//0066D4D8 //"Use Full Iscript" according to EUDDB
//SCBW_DATA(const u8*,  ???,		imagesDat[4].address);	//00667718 //"Draw If Cloaked" according to EUDDB
SCBW_DATA(const u8*,  RLE_Function, imagesDat[5].address);	//00669E28
SCBW_DATA(const u8*,  Remapping,    imagesDat[6].address);	//00669A40
//SCBW_DATA(const u32*,  ???,		imagesDat[7].address);	//0066EC48 //"Iscript ID" according to EUDDB
//SCBW_DATA(const u32*,  ???,		imagesDat[8].address);	//0066C538 //"Shields Overlay" according to EUDDB
//SCBW_DATA(const u32*,  ???,		imagesDat[9].address);	//0066B1B0 //"Attack Overlay" according to EUDDB
//SCBW_DATA(const u32*,  ???,		imagesDat[10].address);	//0066A210 //"Damage Overlay" according to EUDDB
//SCBW_DATA(const u32*,  ???,		imagesDat[11].address);	//00667B00 //"Special Overlay" according to EUDDB
//SCBW_DATA(const u32*,  ???,		imagesDat[12].address);	//00666778 //"Landing Dust Overlay" according to EUDDB
//SCBW_DATA(const u32*,  ???,		imagesDat[13].address);	//0066D8C0 //"Lift-off Dust Overlay" according to EUDDB
} //images_dat (last entry at 0x005140AC)


//////////////////////////////////////////////////////////////// @}

/// @name Reserved data
/// Warning: The following section contains data reserved for internal use only.
///          If you are a modder, use these at your own risk!
//////////////////////////////////////////////////////////////// @{

//Helper functions for several hooks
SCBW_DATA(u8*,          selectionIndexStart,    0x006284B6);
SCBW_DATA(const UnitsSel*, activePlayerSelection, 0x006284B8);
typedef CUnit* (__cdecl *GetActivePlayerNextSelectionFunc)();
SCBW_DATA(GetActivePlayerNextSelectionFunc, getActivePlayerNextSelection, 0x0049A850);

/// Selections of all players
struct PlayersSel { CUnit* unit[PLAYABLE_PLAYER_COUNT][SELECTION_ARRAY_LENGTH]; };
SCBW_DATA(PlayersSel*, playersSelections, 0x006284E8);

typedef void (__fastcall *PrepareForNextOrderFunc)(CUnit*);
SCBW_DATA(PrepareForNextOrderFunc, prepareForNextOrder, 0x00475000);

//Contains various info on the tiles.
SCBW_DATA(ActiveTile**, activeTileArray,        0x006D1260);

// Stores an ordered list of all units currently in the game.
SCBW_DATA(UnitFinderData*, unitOrderingX,       0x0066FF78);
SCBW_DATA(UnitFinderData*, unitOrderingY,       0x006769B8);
SCBW_DATA(const u32*,   unitOrderingCount,      0x0066FF74);

SCBW_DATA(const Point32*, angleDistance,        0x00512D28);

//Used internally by Starcraft to manage temporary units lists
SCBW_DATA(CUnit***,	tempUnitsListsArray,					0x006BB938);
SCBW_DATA(u32*,		tempUnitsListCurrentArrayCount,			0x006BEE64);
SCBW_DATA(u32*,		tempUnitsListArraysCountsListLastIndex,	0x006BEE6C);

// Font & Drawing
namespace graphics { class Font; class Bitmap; }
SCBW_DATA(graphics::Font**, fontBase,           0x006CE0F4); //array of increasingly big fonts (max_index: 3?)
SCBW_DATA(graphics::Bitmap*, gameScreenBuffer,  0x006CEFF0);
SCBW_DATA(u8*,          refreshRegions,         0x006CEFF8);  //640 x 480 divided into 1200 squares of 16x16
SCBW_DATA(Layers*,      screenLayers,           0x006CEF50);
typedef void (__stdcall *DrawGameProc)(graphics::Bitmap *surface, Bounds *bounds);
static DrawGameProc const oldDrawGameProc = (DrawGameProc) 0x004BD580;

//Array used in multiplayer to randomize factions
//used by unit.getFactionColor() and
//player.getFactionColor()
//Max index is PLAYABLE_PLAYER_COUNT-1
SCBW_DATA(int*,         factionsColorsOrdering,	0x0057F21C);

///Access to true player colors that differ from
///standard versions from graphics.h
///This is the PlayerColors from BWAPI
///Max index is PLAYER_COUNT-1
SCBW_DATA(u8*,         playersColors,	0x00581DD6);

const CListExtern<CImage, &CImage::link> unusedImages(0x0057EB68, 0x0057EB70);
SCBW_DATA(CList<CSprite>*, unusedSprites,       0x0063FE30);

struct SpriteTileData {
  CSprite* tails[256];
  CSprite* heads[256];
};
SCBW_DATA(SpriteTileData*, spritesOnTileRow,    0x00629288);

//Image-related
struct ImagesDatExtraOverlayLO_Files {
  LO_Header* attackOverlays[IMAGE_TYPE_COUNT];
  LO_Header* damageOverlays[IMAGE_TYPE_COUNT];
  LO_Header* specialOverlays[IMAGE_TYPE_COUNT];
  LO_Header* landingDustOverlays[IMAGE_TYPE_COUNT];
  LO_Header* liftoffDustOverlays[IMAGE_TYPE_COUNT];
};
SCBW_DATA(const ImagesDatExtraOverlayLO_Files*, lo_files, 0x0051F2A8);
SCBW_DATA(const LO_Header* const*, shieldOverlays, 0x0052E5C8);

SCBW_DATA(const ColorShiftData*, colorShift, 0x005128F8); //Use ColorRemapping::Enum as index
SCBW_DATA(GrpHead* const*, imageGrpGraphics, 0x0051CED0);

template <typename T>
struct PlayerFlags {
  T flags[PLAYER_COUNT];
};
SCBW_DATA(PlayerFlags<u8>*, playerAlliance,     0x0058D634);  //See scbw::isAlliedTo()
SCBW_DATA(PlayerFlags<u32>*, playerVision,      0x0057F1EC);

SCBW_DATA(CUnit**,  activePortraitUnit,   0x00597248);

SCBW_DATA(AI_Main*,       AIScriptController,   0x0068FEE8);
SCBW_DATA(AiCaptain* const*, AiRegionCaptains,  0x0069A604);
SCBW_DATA(u32*,           aiSupplyReserved,     0x006CA4BC);

//-------- Internal constants --------//

SCBW_DATA(const Bool32*,  IS_GAME_PAUSED,       	0x006509C4);  //See scbw::isGamePaused()
SCBW_DATA(const Bool8*,   IS_BROOD_WAR,         	0x0058F440);  //See scbw::isBroodWarMode()
SCBW_DATA(const u32*,     CHEAT_STATE,          	0x006D5A6C);  //See scbw::isCheatEnabled()
SCBW_DATA(const s32*,     MAX_UNIT_WIDTH,       	0x006BEE68);
SCBW_DATA(const s32*,     MAX_UNIT_HEIGHT,      	0x006BB930);
SCBW_DATA(const Bool32*,  IS_IN_REPLAY,         	0x006D0F14);  //See scbw::isInReplay()
SCBW_DATA(const Bool32*,  IS_REPLAY_PAUSED,			0x006D11B0);
SCBW_DATA(Bool32*,        IS_IN_GAME_LOOP,			0x006D11C8);
SCBW_DATA(u32*,           lastRandomNumber,     	0x0051CA14);  //See scbw::random(), scbw::randBetween()
SCBW_DATA(Bool32*,        canUpdatePoweredStatus,	0x0063FF44);
SCBW_DATA(Bool32*,		  IS_PLACING_BUILDING,  	0x00640880);

//-------- Format strings used by Starcraft implementations of printf --------//

SCBW_DATA(const char*,  FORMATSTRING_COLORYELLOW_VALUE_COLORCYAN_SPACE,						0x00505708); //"\x03%d\x01 "
SCBW_DATA(const char*,	FORMATSTRING_VALUE_SPACE,											0x00505710); //"%d "
SCBW_DATA(const char*,	FORMATSTRING_CURRENTVALUE1_MAXVALUE1_COLOR_CURRENTVALUE2_MAXVALUE2, 0x00505714); //"%d/%d %c%d/%d"
SCBW_DATA(const char*,	FORMATSTRING_COLOR_CURRENTVALUE_MAXVALUE,							0x00505724); //"%c%d/%d"
SCBW_DATA(const char*,	FORMATSTRING_COLORWHITE_CURRENTVALUE_MAXVALUE,						0x0050572C); //"\x04%d/%d"
SCBW_DATA(const char*,	FORMATSTRING_TEXT_SPACE_VALUE_IN_PARENTHESIS,						0x00505734); //"%s (%d)"
SCBW_DATA(const char*,	FORMATSTRING_TEXT_SPACE_VALUE,										0x0050573C); //"%s %d"

//-------- Tech levels --------//

struct _scTechs {
  u8 isEnabled[PLAYER_COUNT][24];		//0x0058CE24
  u8 isResearched[PLAYER_COUNT][24];	//0x0058CF44
};
struct _bwTechs {
  u8 isEnabled[PLAYER_COUNT][20];		//0x0058F050
  u8 isResearched[PLAYER_COUNT][20];	//0x0058F140
};

// Available and Researched techs are exactly next to each other,
// that's why with 2 entries structures, only 2 addresses are
// needed here when BWAPI got 4 of them.
SCBW_DATA(_scTechs*, TechSc,   0x0058CE24); //Use with ScTech::Enum
SCBW_DATA(_bwTechs*, TechBw,   0x0058F050); //Use with BwTech::Enum

//-------- Upgrade levels --------//

struct _scUpgrs {
  u8 maxLevel[PLAYER_COUNT][46];		//0x0058D088
  u8 currentLevel[PLAYER_COUNT][46];	//0x0058D2B0
};
struct _bwUpgrs {
  u8 maxLevel[PLAYER_COUNT][15];		//0x0058F278
  u8 currentLevel[PLAYER_COUNT][15];	//0x0058F32C
};
SCBW_DATA(_scUpgrs*, UpgradesSc,  0x0058D088); //Use with ScUpgrade::Enum
SCBW_DATA(_bwUpgrs*, UpgradesBw,  0x0058F278); //Use with BwUpgrade::Enum

//////////////////////////////////////////////////////////////// @}

#undef SCBW_DATA
#pragma pack()

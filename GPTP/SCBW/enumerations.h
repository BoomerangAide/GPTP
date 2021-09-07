//All-in-one header file for various enumerations used by GPTP.
#pragma once
#pragma warning( disable: 4482 )
#pragma warning( disable: 26812 )

#include "enumerations/UnitId.h"
#include "enumerations/WeaponId.h"
#include "enumerations/UpgradeId.h"
#include "enumerations/TechId.h"
#include "enumerations/OrderId.h"
#include "enumerations/ImageId.h"
#include "enumerations/SpriteId.h"
#include "enumerations/FlingyId.h"
#include "enumerations/SoundId.h"
#include "enumerations/IconId.h"
#include "enumerations/PortraitId.h"

namespace RaceId {
enum Enum : u8 {
  Zerg    = 0,
  Terran  = 1,
  Protoss = 2,
  Other   = 3,
  Neutral = 4,
  Select  = 5,
  Random  = 6,
  None    = 7,
};
}

namespace PlayerType {
enum Enum {
  NotUsed          = 0,
  Computer         = 1,
  Human            = 2,
  Rescuable        = 3,
  Unknown0         = 4,
  ComputerSlot     = 5,
  OpenSlot         = 6,
  Neutral          = 7,
  ClosedSlot       = 8,
  Unknown1         = 9,
  HumanDefeated    = 10, /**< Left */
  ComputerDefeated = 11,  /**< Left */
};
}

namespace MovableState {
enum Enum : u8 {
	NotReachedDestination = 0,
	ReachedDestination = 1,
	UnmovableAtDestination = 2
};
}

//Based on BWAPI; for use with units_dat::BaseProperty[]
namespace UnitProperty {
enum Enum : u32 {
  Building            = 0x00000001,
  Addon               = 0x00000002,
  Flyer               = 0x00000004,
  Worker              = 0x00000008,
  Subunit             = 0x00000010,
  FlyingBuilding      = 0x00000020,
  Hero                = 0x00000040,
  RegeneratesHP       = 0x00000080,
  AnimatedIdle        = 0x00000100,
  Cloakable           = 0x00000200,
  TwoUnitsIn1Egg      = 0x00000400,
  NeutralAccessories  = 0x00000800, //AKA "Single entity" (prevents multi-select, set on all pickup items)
  ResourceDepot       = 0x00001000,	//Place where resources are brought back
  ResourceContainer   = 0x00002000,	//Resource Source
  RoboticUnit         = 0x00004000,
  Detector            = 0x00008000,
  Organic             = 0x00010000,
  CreepBuilding       = 0x00020000,
  Unused              = 0x00040000,
  RequiredPsi         = 0x00080000,
  Burrowable          = 0x00100000,
  Spellcaster         = 0x00200000,
  PermanentCloak      = 0x00400000,
  NPCOrAccessories    = 0x00800000, //AKA "Pickup item" (data disc, crystals, mineral chunks, gas tanks, etc.)
  MorphFromOtherUnit  = 0x01000000,
  MediumOverlay       = 0x02000000, //Used to determine overlay for various spells and effects
  LargeOverlay        = 0x04000000, //Used to determine overlay for various spells and effects
  AutoAttackAndMove   = 0x08000000,
  Attack              = 0x10000000, /**< Can attack */
  Invincible          = 0x20000000,
  Mechanical          = 0x40000000,
  ProducesUnits       = 0x80000000, /**< It can produce units directly (making buildings doesn't count) */
};
}

//Based on BWAPI; For use with the CUnit::status member
namespace UnitStatus {
enum Enum : u32 {
  Completed             = 0x00000001,
  GroundedBuilding      = 0x00000002, // a building that is on the ground
  InAir                 = 0x00000004,
  Disabled              = 0x00000008,  /**< Protoss Unpowered */
  Burrowed              = 0x00000010,
  InBuilding            = 0x00000020,
  InTransport           = 0x00000040,
  CanBeChased           = 0x00000080,  /**< @todo Unknown */
  RequiresDetection     = 0x00000100,
  Cloaked               = 0x00000200,
  DoodadStatesThing     = 0x00000400,  /**< Unknown. ++protoss unpowered buildings have this flag set */
  CloakingForFree       = 0x00000800,  /**< Requires no energy to cloak */
  CanNotReceiveOrders   = 0x00001000,
  NoBrkCodeStart        = 0x00002000,  /**< Unbreakable code section in iscript */
  UNKNOWN2              = 0x00004000,  /**< @todo Unknown */
  CanNotAttack          = 0x00008000,  /**< @todo Unknown */
  CanTurnAroundToAttack = 0x00010000,  // canAttack? /**< @todo Unknown */ named IsAUnit in BWAPI
  IsBuilding            = 0x00020000,
  IgnoreTileCollision   = 0x00040000,
  Unmovable             = 0x00080000,
  IsNormal              = 0x00100000,  /**< 1 for "normal" units, 0 for hallucinated units */
  NoCollide             = 0x00200000, // ++if set, other units wont collide with the unit (like burrowed units)
  UNKNOWN5              = 0x00400000,
  IsGathering           = 0x00800000, // ++if set, the unit wont collide with other units (like workers gathering)
  UNKNOWN6              = 0x01000000,
  UNKNOWN7              = 0x02000000, // Turret related
  Invincible            = 0x04000000,
  HoldingPosition       = 0x08000000, // Set if the unit is currently holding position
  SpeedUpgrade          = 0x10000000,
  CooldownUpgrade       = 0x20000000,
  IsHallucination       = 0x40000000,  /**< 1 for hallucinated units, 0 for "normal" units */
  IsSelfDestructing     = 0x80000000  // Set for when the unit is self-destructing (scarab, scourge, infested terran)
};
}

namespace BUTTON_STATE {
enum Enum {
	Disabled = -1,
	Invisible= 0,
	Enabled   = 1
};
}

// For use with scbw::printText()
namespace GameTextColor {
enum Enum {
  Grey          = 0x01,
  Grey2         = 0x02,
  Yellow        = 0x03,
  White         = 0x04,
  DarkGrey      = 0x05,
  Red           = 0x06,
  Green         = 0x07,
  Invisible     = 0x08,
  Tab           = 0x09,
  Newline       = 0x0A,
  Unknown       = 0x0B,
  Newline2      = 0x0C,
  Invisible2    = 0x0D,
  AlignRight    = 0x12,
  AlignCenter   = 0x13,
  TextGone      = 0x14,
  RiverBlue     = 0x0E,
  Teal          = 0x0F,
  Purple        = 0x10,
  Orange        = 0x11,
  Brown         = 0x15,
  LightGrey     = 0x16,
  Yellow2       = 0x17,
  DarkGreen     = 0x18,
  Yellow3       = 0x19,
  Yellow4       = 0x1A,
  Navy          = 0x1C,
  ArmyGreen     = 0x1D,
  Grey3         = 0x1E,
  Teal2         = 0x1F,
  AlignCenter2  = 0x13,
  TextGone2     = 0x14,
};
}

//Unused in GPTP values are based on PyTBL v1.7 infos
enum TEXTLABELCOLOR {
/*0x01*/	CYAN1 = 1,
/*0x02*/	CYAN2 = 2,
/*0x03*/	YELLOW = 3,
/*0x04*/	WHITE = 4,
/*0x05*/	GREY_LAST = 5,			//Starcraft will ignore other color tags if used
/*0x06*/	RED = 6,
/*0x07*/	GREEN = 7,
/*0x08*/	RED_PLAYER1 = 8,
/*0x0B*/	INVISIBLE_LAST = 11,	//Starcraft will ignore other color tags if used
/*0x0C*/	TRUNCATE = 12,
/*0x0E*/	BLUE_PLAYER2 = 14,
/*0x0F*/	TEAL_PLAYER3 = 15,
/*0x10*/	PURPLE_PLAYER4 = 16,
/*0x11*/	ORANGE_PLAYER5 = 17,
/*0x14*/	INVISIBLE_LAST2 = 20,	//Starcraft will ignore other color tags if used
/*0x15*/	BROWN_PLAYER6 = 21,
/*0x16*/	WHITE_PLAYER7 = 22,
/*0x17*/	YELLOW_PLAYER8 = 23,
/*0x18*/	GREEN_PLAYER9 = 24,
/*0x19*/	BRIGHT_YELLOW_PLAYER10 = 25,
/*0x1A*/	CYAN3 = 26,
/*0x1B*/	PINKISH_PLAYER11 = 27,
/*0x1C*/	DARK_CYAN_PLAYER12 = 28,
/*0x1D*/	GREYGREEN = 29,
/*0x1E*/	BLUEGREY = 30,
/*0x1F*/	TURQUOISE = 31
};

namespace LobbyTextColor {
enum Enum {
  Purple1     = 0x01,
  Purple2     = 0x02,
  YellowGreen = 0x04,
  Grey        = 0x05,
  White       = 0x06,
  Red         = 0x07,
  Tab         = 0x09,
  Black       = 0x10, // Could be wrong. Needs check!
  AlignRight  = 0x18,
  AlignCenter = 0x19,
};
};

// For use with scbw::playIscriptAnim()
namespace IscriptAnimation {
enum Enum : u8 {
Init          = 0x00,
Death         = 0x01,
GndAttkInit   = 0x02,
AirAttkInit   = 0x03,
Unused1       = 0x04,
GndAttkRpt    = 0x05,
AirAttkRpt    = 0x06,
CastSpell     = 0x07,
GndAttkToIdle = 0x08,
AirAttkToIdle = 0x09,
Unused2       = 0x0A,
Walking       = 0x0B,
WalkingToIdle = 0x0C,
SpecialState1 = 0x0D,
SpecialState2 = 0x0E,
AlmostBuilt   = 0x0F,
Built         = 0x10,
Landing       = 0x11,
LiftOff       = 0x12,
IsWorking     = 0x13,
WorkingToIdle = 0x14,
WarpIn        = 0x15,
Unused3       = 0x16,
StarEditInit  = 0x17,
Disable       = 0x18,
Burrow        = 0x19,
UnBurrow      = 0x1A,
Enable        = 0x1B
};
}

namespace CheatFlags {
enum Enum {
  None                   = 0,
  BlackSheepWall         = 1 <<  0,		/*0x00000001*/
  OperationCwal          = 1 <<  1,		/*0x00000002*/
  PowerOverwhelming       = 1 <<  2,	/*0x00000004*/
  SomethingForNothing    = 1 <<  3,		/*0x00000008*/
  ShowMeTheMoney         = 1 <<  4,		/*0x00000010*/
  GameOverMan            = 1 <<  6,		/*0x00000040*/
  ThereIsNoCowLevel      = 1 <<  7,		/*0x00000080*/
  StayingAlive           = 1 <<  8,		/*0x00000100*/
  Ophelia                = 1 <<  9,		/*0x00000200*/
  TheGathering           = 1 << 11,		/*0x00000800*/
  MedievalMan            = 1 << 12,		/*0x00001000*/
  ModifyThePhaseVariance = 1 << 13,		/*0x00002000*/
  WarAintWhatItUsedToBe  = 1 << 14,		/*0x00004000*/
  FoodForThought         = 1 << 17,		/*0x00020000*/
  WhatsMineIsMine        = 1 << 18,		/*0x00040000*/
  BreatheDeep            = 1 << 19,		/*0x00080000*/
  NoGlues                = 1 << 29		/*0x20000000*/
};
}

namespace DamageType {
enum Enum {
  Independent = 0,
  Explosive   = 1,
  Concussive  = 2,
  Normal      = 3,
  IgnoreArmor = 4,
};
}

//Use with weapons_dat::ExplosionType[]
namespace WeaponEffect {
enum Enum {
  None            = 0,
  NormalHit       = 1,
  SplashRadial    = 2,
  SplashEnemy     = 3,
  Lockdown        = 4,
  NuclearMissile  = 5,
  Parasite        = 6,
  Broodlings      = 7,
  EmpShockwave    = 8,
  Irradiate       = 9,
  Ensnare         = 10,
  Plague          = 11,
  StasisField     = 12,
  DarkSwarm       = 13,
  Consume         = 14,
  YamatoGun       = 15,
  Restoration     = 16,
  DisruptionWeb   = 17,
  CorrosiveAcid   = 18,
  MindControl     = 19,
  Feedback        = 20,
  OpticalFlare    = 21,
  Maelstrom       = 22,
  Unknown_Crash   = 23,
  SplashAir       = 24
};
}

//Use with weapons_dat::Behavior[]
namespace WeaponBehavior {
enum Enum {
  Fly_DoNotFollowTarget = 0,
  Fly_FollowTarget      = 1,
  AppearOnTargetUnit    = 2,
  PersistOnTargetSite   = 3,  //Psionic Storm
  AppearOnTargetSite    = 4,
  AppearOnAttacker      = 5,
  AttackAndSelfDestruct = 6,
  Bounce                = 7,  //Mutalisk Glave Wurms
  AttackNearbyArea      = 8,  //Valkyrie Halo Rockets
  GoToMaxRange          = 9   //Lurker Subterranean Spines
};
}

//Use with colorShift
namespace ColorRemapping {
enum Enum {
  None        = 0,
  OFire       = 1,
  GFire       = 2,
  BFire       = 3,
  BExpl       = 4,
  Trans50     = 5,  //Special - OwnCloak
  Red_Crash   = 6,
  Green_Crash = 7,
};
}

//Compare with (*GAME_TYPE)
namespace GameType {
enum Enum {
  Melee           = 2,
  FreeForAll      = 3,
  OneOnOne        = 4,
  CaptureTheFlag  = 5,
  Greed           = 6,
  Slaughter       = 7,
  SuddenDeath     = 8,
  Ladder          = 9,
  UseMapSettings  = 10,
  TeamMelee       = 11,
  TeamFreeForAll  = 12,
  TeamCTF         = 13,
  TopVsBottom     = 15,
};
}

/// Compare with (*CURRENT_TILESET)
namespace TilesetType {
enum Enum {
	Badlands = 0,
	Space_Platform = 1,
	Installation = 2,
	Ash_World = 3,
	Jungle_World = 4,
	Desert = 5,
	Ice = 6,
	Twilight = 7
};
}

namespace BinDlgFlags {
enum Enum {
	Unknown0 =					0x00000001,
	Disabled =					0x00000002,
	Active =					0x00000004,
	Visible =					0x00000008,
	RespondsToEvents =			0x00000010,
	Unknown5 =					0x00000020,
	CancelButton =				0x00000040,
	NoHoverSound =				0x00000080,
	SpecialHotkeyVirtualKey =	0x00000100,
	HasHotkey =					0x00000200,
	SmallestFontSize10 =		0x00000400,
	LargerFontSize16 =			0x00000800,
	Unknown12 =					0x00001000,
	TransparentHideColor0 =		0x00002000,
	LargestFontSize16x =		0x00004000,
	AlternateStyleWC2 =			0x00008000,
	SmallerFontSize14 =			0x00010000,
	RemoveStyles =				0x00020000,
	ApplyTranslucency =			0x00040000,
	DefaultButton =				0x00080000,
	BringToFront =				0x00100000,
	HorizontalAlignmentLeft =	0x00200000,
	HorizontalAlignmentRight =	0x00400000,
	HorizontalAlignmentCenter =	0x00800000,
	VerticalAlignmentTop =		0x01000000,
	VerticalAlignmentMiddle =	0x02000000,
	VerticalAlignmentBottom =	0x04000000,
	Unknown27 =					0x08000000,
	ReverseDialogDirection =	0x10000000,
	UseAlternateStyle =			0x20000000,
	NoClickSound =				0x40000000,
	HighlightText =				0x80000000
};
}

namespace SmkFlags {
enum Enum {
	FadeIn			= 0x0001,
	Dark			= 0x0002,
	RepeatForever	= 0x0004,
	ShowIfOver		= 0x0008,
	Unknown4		= 0x0010,
	Flag_0020		= 0x0020,
	Flag_0040		= 0x0040,
	Flag_0080		= 0x0080,
	Flag_0100		= 0x0100,
	Flag_0200		= 0x0200,
	Flag_0400		= 0x0400,
	Flag_0800		= 0x0800,
	Flag_1000		= 0x1000,
	Flag_2000		= 0x2000,
	Flag_4000		= 0x4000,
	Flag_8000		= 0x8000
};
}

namespace DialogControlTypes {
enum Enum {
	DialogBox			= 0x00000000,
	DefaultButton		= 0x00000001,
	Button				= 0x00000002,
	OptionButton		= 0x00000003,
	CheckBox			= 0x00000004,
	Image				= 0x00000005,
	HorizontalScrollBar	= 0x00000006,
	VerticalScrollBar	= 0x00000007,
	TextBox				= 0x00000008,
	LabelLeftAlign		= 0x00000009,
	LabelRightAlign		= 0x0000000A,
	LabelCenterAlign	= 0x0000000B,
	ListBox				= 0x0000000C,
	ComboBox			= 0x0000000D,
	LightUpButton		= 0x0000000E
};
}

//Use with units_dat::RightClickAction
namespace RightClickActions {
	enum Enum : u8 {
		NoCommand_AutoAttack = 0,
		NormalMove_NormalAttack = 1,
		NormalMove_NoAttack = 2,
		NoMove_NormalAttack = 3,
		Harvest = 4,
		HarvestAndRepair = 5,
		Unknown = 6				//only by ComSat addon
};
}

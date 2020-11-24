//All-in-one include file
//Structs by poiuy_qwert, A_of_s_t, ravenwolf and pastelmind.
//Special thanks to Heinermann

#pragma once

const int PLAYER_COUNT			= 12;
const int PLAYABLE_PLAYER_COUNT	= 8;
const int UNIT_ARRAY_LENGTH		= 1700;
const int BULLET_ARRAY_LENGTH	= 100;
const int SPRITE_ARRAY_LENGTH	= 2500;
const int SELECTION_ARRAY_LENGTH = 12;
const int UNIT_TYPE_COUNT		= 228;
const int TECH_TYPE_COUNT		= 44;
const int UPGRADE_TYPE_COUNT	= 61;
const int WEAPON_TYPE_COUNT		= 130;
const int FLINGY_TYPE_COUNT		= 209;
const int IMAGE_TYPE_COUNT		= 999;

#include "structures/CUnit.h"
#include "structures/CBullet.h"
#include "structures/CSprite.h"
#include "structures/CImage.h"
#include "structures/COrder.h"
#include "structures/Layer.h"
#include "structures/Player.h" //made into external file to have .cpp

#pragma pack(1)

struct UNITDEATHS {
  u32        player[12];
};

//Based on http://code.google.com/p/bwapi/source/browse/branches/bwapi4/bwapi/BWAPI/Source/BW/CImage.h

struct TEXT {
  char  textdisplay[218][13];  //[last is text at bottom; ex: "Cheat enabled."]
  UNK   unknown1;
  UNK   unknown2;
  u8    color[13];
  UNK   unknown3[3];  //[last is text at bottom; ex: "Cheat enabled."]
  u8    timer[13];
};

struct TILE {
/*00*/  u16 index;
/*02*/  u8 buildability; /**< 8th bit should sign not buildable. */
/*03*/  u8 groundHeight; /**< Ground Height(4 lower bits) - Deprecated? Some values are incorrect. */
/*04*/  u16 leftEdge;
/*06*/  u16 topEdge;
/*08*/  u16 rightEdge;
/*0A*/  u16 buttomEdge;
/*0C*/  u16 _1;
/*0E*/  u16 _2; /**<  Unknown - Edge piece has rows above it. (Recently noticed; not fully understood.)
                o 1 = Basic edge piece.
                o 2 = Right edge piece.
                o 3 = Left edge piece.*/
/*10*/  u16 _3;
/*12*/  u16 _4;
/*14*/  u16 miniTile[16]; /** MegaTile References (VF4/VX4) */
};

struct DOODAD {
/*00*/  u16 index;
/*02*/ u8 buildability; /**< 8th bit should sign not buildable. */
/*03*/  u8 groundHeightAndOverlayFlags; /**< Ground Height(4 lower bits) - Deprecated? Some values are incorrect.
                   * Overlay Flags:
                   * o 0x0 - None
                   * o 0x1 - Sprites.dat Reference
                   * o 0x2 - Units.dat Reference (unit sprite)
                   * o 0x4 - Overlay is Flipped
                   */
/*04*/  u16 overlayID;
/*06*/  u16 _1;
/*08*/  u16 doodatGrupString;
/*0A*/  u16 _2;
/*0C*/  u16 dddataBinIndex;
/*0E*/  u16 doodatHeight;
/*10*/  u16 doodatWidth;
/*12*/  u16 _3;
/*14*/  u16 miniTile[16]; /** MegaTile References (VF4/VX4) */
};	//size 34

struct CThingy {
/*00*/  CThingy *prev;
/*04*/  CThingy *next;
/*08*/  u32     unitType;
/*0C*/  CSprite *sprite;
};

//---- Taken from locations.cpp ----//

struct MapSize {
  u16   width;
  u16   height;
};

struct LOCATION {
  Box32 dimensions;
  u16   stringId;
  u16   flags;
};

//---- Taken from buttons.cpp ----//

typedef u32 ButtonState;
typedef ButtonState (__fastcall *REQ_FUNC)(u32, u32, CUnit*);
//(u32 reqVar, u32 playerId, CUnit* selected);
typedef void (__fastcall *ACT_FUNC)(u32,u32); //__fastcall(ECX, EDX, [EBP+??], ... , [EBP+0x0C], [EBP+0x08])
//(u32 actVar, u32 shiftClick);

struct BUTTON {
/*00*/  u16       position;
/*02*/  u16       iconID;
/*04*/  REQ_FUNC  *reqFunc;
/*08*/  ACT_FUNC  *actFunc;
/*0C*/  u16       reqVar;
/*0E*/  u16       actVar;
/*10*/  u16       reqStringID;
/*12*/  u16       actStringID;
};

struct BUTTON_SET {
  u32     buttonsInSet;
  BUTTON  *firstButton;
  u32     connectedUnit;
};

//---- Taken from triggers.h ----//
struct CONDITION {
  u32        location;
  u32        player;
  u32        number;
  u16        unit;
  u8        comparison;
  u8        condition;
  u8        type;
  u8        flags;
};

typedef Bool32 (__fastcall *ConditionPointer)(CONDITION*);

struct ACTION {
  u32        location;
  u32        string;
  u32        wavString;
  u32        time;
  u32        player;
  u32        number;
  u16        unit;
  u8        action;
  u8        number2;
  u8        flags;
};

typedef Bool32 (__fastcall *ActionPointer)(ACTION*);

//GRP frame header
struct GrpFrame {
  s8  x;
  s8  y;
  s8  width;
  s8  height;
  u32 dataOffset;	//might be u16* (but as a pointer, would still fit in u32)
};

C_ASSERT(sizeof(GrpFrame) == 8); 
//static_assert(sizeof(GrpFrame) == 8, "The size of the GrpFrame structure is invalid");

//GRP file header
struct GrpHead {
  u16 frameCount;
  s16 width;
  s16 height;
  GrpFrame frames[1];
};

C_ASSERT(sizeof(GrpHead) == 14); 
//static_assert(sizeof(GrpHead) == 14, "The size of the GrpHead structure is invalid");

//LO* file header
struct LO_Header {
  u32 frameCount;
  u32 overlayCount;
  u32 frameOffsets[1];

  Point8 getOffset(int frameNum, int overlayNum) const {
    return ((Point8*)(this->frameOffsets[frameNum] + (u32)this))[overlayNum];
  }
};

C_ASSERT(sizeof(LO_Header) == 12);
//static_assert(sizeof(LO_Header) == 12, "The size of the LO_Header structure is invalid");


//Image Remapping Data
struct ColorShiftData {
  u32 index;
  void* data;
  char name[12];
};

C_ASSERT(sizeof(ColorShiftData) == 20);
//static_assert(sizeof(ColorShiftData) == 20, "The size of the ColorShiftData structure is invalid");

//-------- Flag structures --------//

//Based on BWAPI's Offsets.h
struct ActiveTile {
/*00*/  u8 visibilityFlags;
/*01*/  u8 exploredFlags;
/*02*/  u8 isWalkable         : 1; // Set on tiles that can be walked on
		u8 unknown1           : 1; // Unused?
		u8 isUnwalkable       : 1; // Set on tiles that can't be walked on
		u8 unknown2           : 1; // Unused?
		u8 hasDoodadCover     : 1; // Set when a doodad cover (trees) occupies the area
		u8 unknown3           : 1; // Unused?
		u8 hasCreep           : 1; // Set when creep occupies the area
		u8 alwaysUnbuildable  : 1; // always unbuildable, like water
/*03*/  u8 groundHeight       : 3; // ground height; 1 for low ground, 2 for middle ground and 4 for high ground
		u8 currentlyOccupied  : 1; // unbuildable but can be made buildable
		u8 creepReceeding     : 1; // Set when the nearby structure supporting the creep is destroyed
		u8 cliffEdge          : 1; // Set if the tile is a cliff edge
		u8 temporaryCreep     : 1; // Set when the creep occupying the area was created. Not set if creep tiles were preplaced. Used in drawing routine.
		u8 unknown4           : 1; // Unused?
};

C_ASSERT(sizeof(ActiveTile) == 4);
//static_assert(sizeof(ActiveTile) == 4, "The size of the ActiveTile structure is invalid");

struct GroupFlag {
  u8 isZerg         : 1;	/*0x01*/
  u8 isTerran       : 1;	/*0x02*/
  u8 isProtoss      : 1;	/*0x04*/
  u8 isMen          : 1;	/*0x08*/
  u8 isBuilding     : 1;	/*0x10*/
  u8 isFactory      : 1;	/*0x20*/
  u8 isIndependent  : 1;	/*0x40*/
  u8 isNeutral      : 1;	/*0x80*/
};

C_ASSERT(sizeof(GroupFlag) == 1);
//static_assert(sizeof(GroupFlag) == 1, "The size of the GroupFlag structure is invalid");

struct TargetFlag {
  u16 air         : 1;
  u16 ground      : 1;
  u16 mechanical  : 1;
  u16 organic     : 1;
  u16 nonBuilding : 1;
  u16 nonRobotic  : 1;
  u16 terrain     : 1;
  u16 orgOrMech   : 1;
  u16 playerOwned : 1;
};

C_ASSERT(sizeof(TargetFlag) == 2);
//static_assert(sizeof(TargetFlag) == 2, "The size of the TargetFlag structure is invalid");

//-------- End of flag structures --------//

struct UnitFinderData {
  s32 unitIndex;
  s32 position;
  bool operator < (const UnitFinderData& rhs) const {
    return this->position < rhs.position;
  }
};

C_ASSERT(sizeof(UnitFinderData) == 8);
//static_assert(sizeof(UnitFinderData) == 8, "The size of the UnitFinderData structure is invalid");

struct UnknownUserStruct {
/*0x00*/ u32	unkUser_00;
/*0x04*/ u16	iconId_04;
/*0x06*/ u16	unknown_06;
/*0x08*/ u16	id_08;
};

//BinDlg is defined in https://code.google.com/p/vgce/source/browse/trunk/docs/Blizzard/Starcraft/BIN%20FORMAT.txt
//as Dialog Structure
//That structure is variable depending on the content

struct BinDlg {
/*0x00*/  BinDlg  *next;
/*0x04*/  Bounds  bounds;			//0x4 left,0x6 top,0x8 right,0xA bottom,0xC width,0xE height
/*0x10*/  u8      *buffer;
/*0x14*/  char    *pszText;
/*0x18*/  u32     flags;			//use BinDlgFlags::Enum
/*0x1C*/  u32     unk_1c;
/*0x20*/  s16     index;
/*0x22*/  u16     controlType;		//use DialogControlTypes::Enum
/*0x24*/  u16     graphic;

union {
/*0x26*/  u32*					user;
/*0x26*/  CUnit*				unitUser;
/*0x26*/  BinDlg*				dlgUser;
/*0x26*/  BUTTON*				buttonUser;
/*0x26*/  u32					nonUserValue;
/*0x26*/  UnknownUserStruct*	statUser;
};

/*0x2A*/  void    *fxnInteract;
/*0x2E*/  void    *fxnUpdate;
/*0x32*/  BinDlg  *parent;

union {
struct {
/*0x36*/  Box16   responseArea;
/*0x3E*/  BinDlg  *unk_3e;
}initialVersion0x36;
struct {
/*0x36*/  s16     responseAreaWidth;		//guess
/*0x38*/  s16     responseAreaHeight;		//guess
/*0x3A*/  u32     *responseAreaBitArray;	//set with a memory allocation function, bit array is a guess
/*0x3E*/  u32     *unk3e;					//unverified filler
}onInitDialog0x36;
struct {
/*0x36*/  void    *unkFunc36;				//should be a function
/*0x3A*/  u16     scrollMinValue;			//guess to be confirmed if used
/*0x3C*/  u16     scrollMaxValue;			//guess to be confirmed if used
/*0x3E*/  u16     scrollCurrentValue;		//guess to be confirmed if used
/*0x40*/  u16     unk40;					//unverified filler
}scrollDialog0x36;							//probably for scrollbar involving dialogs?
struct {
/*0x36*/  BinDlg  *unkDlg36;
/*0x3A*/  u32     unk3a;					//unverified filler
/*0x3E*/  u32     unk3e;					//unverified filler
}dialogOn0x36;								//for when 0x36 is clearly BinDlg* but the rest is unknown
struct {
/*0x36*/  Bool32  unkBool32_36;				//based on the value only being compared to 0
/*0x3A*/  u32     unk3a;					//unverified filler
/*0x3E*/  u32     unk3e;					//unverified filler
}checkBox0x36;								//not completely sure
struct {
/*0x36*/  u8      unkFlags36;				//"flag" because of possible unkFlags36 |= 1
/*0x37*/  UNK     unk37[3];					//unverified filler
/*0x3A*/  u8*     *unk3aArrayCharPtr;		//seen in a different context so may be wrong and belong to another category
/*0x3E*/  u8      pszTextLen;				//should be the length of pszText
/*0x3F*/  UNK     unk3f[3];					//unverified filler
}textDialog0x36;							//seems to apply to some text related dialogs
};

union {
/*0x42*/  void    *childrenSmk;
/*0x42*/  BinDlg  *childrenDlg;
};

union {
struct {
/*0x46*/  Point16 textPos;
/*0x4A*/  u16     responseAreaWidth;
/*0x4C*/  u16     responseAreaHeight;
/*0x4E*/  UNK     unk_4e[8];
}initialVersion0x46;
struct {
/*0x46*/  Point16 textPos;					//unverified filler
/*0x4A*/  u32     unk4a;
/*0x4E*/  UNK     unk_4e[8];				//unverified filler
}onInitDialog0x46;
struct {
/*0x46*/  u8	  unk46;
/*0x47*/  u8      unk47;					//unverified filler
/*0x48*/  u8      unk48;
/*0x49*/  u8      unk49;
/*0x4A*/  u8      unk4a;
/*0x4B*/  u8      unk4b;
/*0x4C*/  Bool8   unk4c;
/*0x4D*/  u8      unk4d;
/*0x4E*/  u8      unk4e;
/*0x4F*/  UNK     unk_4f[7];				//unverified filler
}alternateVersion0x46;
};

};

C_ASSERT(sizeof(BinDlg) == 86); //0x56
//static_assert(sizeof(BinDlg) == 86, "The size of the BinDlg structure is invalid");

//Based on https://github.com/bwapi/bwapi/blob/master/bwapi/BWAPI/Source/BW/Path.h
struct Path
{
/*0x00*/ Point16 start;
/*0x04*/ Point16 next;
/*0x08*/ Point16 finish;
/*0x0C*/ u32 timeStarted;
/*0x10*/ u32 unitID;
/*0x14*/ u32 unitUnknown_0x38_0x3C_copy;   /* Starcraft copies field 0x38 of the unit
                                                if the unit's flingy movement type is 0,
                                                otherwise it will copy field 0x3C
                                            */
/*0x18*/ u8  isCalculating;  //1 if the path is being calculated and not yet ready
/*0x19*/ u8  pathDelay;      //Delay before a new path is created
/*0x1A*/ u8  unk_1A;         //unknown flag?
/*0x1B*/ u8  pathCount;      //dup of num_areas?
/*0x1C*/ u8  num_areas;      //Official name, number of regions following the segments
/*0x1D*/ u8  cur_area;       //The current region index
/*0x1E*/ u8  num_segments;   //Official name, number of Position segments
/*0x1F*/ u8  cur_segment;    //The current position segment
/*0x20*/ Point16 steps[1];
}; 
C_ASSERT(sizeof(Path) == 36); //0x24

// note: u16 areas[num_areas] follow after steps[num_segments]
// The last entry of the above struct can be viewed as follows:
// Position steps[num_segments];
// u16      regions[num_areas];  //region ID for SAI_Paths->regions; typically fills the rest of the struct with region IDs
								 //to the destination, and cut off if there isn't enough room

//End of Path.h based

struct SMK_Structure {
/*0x00*/ u32		smkOverlayOffset;
/*0x04*/ u16		flags;				//use SmkFlags::Enum
/*0x06*/ u32		internalUse1;
/*0x0A*/ u32		smkFilenameOffset;
/*0x0E*/ u32		internalUse2;
/*0x12*/ Point16	smkOverlayPosition;
/*0x16*/ u32		internalUse3;
/*0x1A*/ u32		internalUse4;
};

C_ASSERT(sizeof(SMK_Structure) == 30); //0x1E
//static_assert(sizeof(SMK_Structure) == 30, "The size of the SMK_Structure structure is invalid");

struct GuiOverlay {
/*0x00*/  UNK     unk_0[6];
/*0x06*/  u16     overlayType;
/*0x08*/  u16     id;
/*0x0A*/  UNK     unk_a[34];
};

C_ASSERT(sizeof(GuiOverlay) == 44); //0x2C
//static_assert(sizeof(GuiOverlay) == 44, "The size of the GuiOverlay structure is invalid");

//-------- AI related stuff --------//

struct AI_Main {
/*000*/  s32 oreCollection;
/*004*/  s32 gasCollection;
/*008*/  s32 supplyCollection;
/*00C*/  s32 ore;
/*010*/  s32 gas;
/*014*/  s32 supply;
/*018*/  u8  unknown_0x18;
/*019*/  u8  newBuildType;
/*01A*/  u16 nextBuildType;
/*01C*/  void  *pTownMain;
/*020*/  u32 unknown_0x20[124];
/*210*/  u8  unknown_0x210;
/*211*/  u8  builtSomething;
/*212*/  u8  AI_NukeRate;
/*213*/  u8  AI_Attacks;
/*214*/  u32 AI_LastNukeTime;
/*218*/  struct {
    u16 isSecureFinished    : 1;	/*0001*/
    u16 isTownStarted       : 1;	/*0002*/
    u16 isDefaultBuildOff   : 1;	/*0004*/
    u16 isTransportsOff     : 1;	/*0008*/
    u16 isFarmsNotimingOn   : 1;	/*0010*/
    u16 isUseMapSettings    : 1;	/*0020*/
    u16 flag_0x40           : 1;	/*0040*/
    u16 spreadCreep         : 1;	/*0080*/
    u16 flag_0x100          : 1;	/*0100*/
    u16 hasStrongestGndArmy : 1;	/*0200*/ //Set if the AI's ground army is stronger than the total strength of the strongest enemy force
    u16 bUpgradesFinished   : 1;	/*0400*/
    u16 bTargetExpansion    : 1;	/*0800*/
  } AI_Flags;
/*21A*/  u16 AI_PanicBlock;
/*21C*/  u16 AI_MaxForce;
/*21E*/  u16 AI_AttackGroup;
/*220*/  u16 waitForceCount;
/*222*/  u8  AI_DefaultMin;
/*223*/  u8  unknown_0x223;
/*224*/  u32 lastIndividualUpdateTime;
/*228*/  u32 AI_AttackTimer;
/*22C*/  u8  unknown_0x22C;
/*22D*/  u8  spellcasterTimer;
/*22E*/  u8  attackManagerTimer;
/*22F*/  u8  AI_IfDif;
/*230*/  u16 AI_AttackGroups[64];
/*2B0*/  u32 AI_DefenseBuild_GG[10];
/*2D8*/  u32 AI_DefenseBuild_AG[10];
/*300*/  u32 AI_DefenseBuild_GA[10];
/*328*/  u32 AI_DefenseBuild_AA[10];
/*350*/  u32 AI_DefenseUse_GG[10];
/*378*/  u32 AI_DefenseUse_AG[10];
/*3A0*/  u32 AI_DefenseUse_GA[10];
/*3C8*/  u32 AI_DefenseUse_AA[10];
/*3F0*/  u8  AI_DefineMax[UNIT_TYPE_COUNT];
/*4D4*/  CUnit* mainMedic;
/*4D8*/  Box32 genCmdLoc;
};

C_ASSERT(sizeof(AI_Main) == 1256); /*0x4E8*/
//static_assert(sizeof(AI_Main) == 1256, "The size of the AI_Main structure is invalid");

struct AiCaptain {
/*00*/  u16 region;
/*02*/  u16 unknown_0x2;
/*04*/  u8  playerId;
/*05*/  u8  captainType;
/*06*/  u8  unknown_0x6;
/*07*/  u8  unknown_0x7;
/*08*/  u8  captainFlags;
/*09*/  u8  unknown_0x9;
/*0A*/  u8  unknown_0xA;
/*0B*/  u8  unknown_0xB;
/*0C*/  u16 unknown_0xC;
/*0E*/  u16 unknown_0xE;
/*10*/  u16 regionGndStrength;
/*12*/  u16 regionAirStrength;
/*14*/  u16 fullGndStrength;
/*16*/  u16 fullAirStrength;
/*18*/  u16 unknown_0x18;
/*1A*/  u16 unknown_0x1A;
/*1C*/  u32 unknown_0x1C;
/*20*/  u32 unknown_0x20;
/*24*/  CUnit* slowestUnit;
/*28*/  CUnit* followTarget;
/*2C*/  CUnit* mainMedic;
/*30*/  void  *town;
};

C_ASSERT(sizeof(AiCaptain) == 52); 
//static_assert(sizeof(AiCaptain) == 52, "The size of the AiCaptain structure is invalid");

// From BWAPI Offsets.h, used in scbwdata.h by UnitAvailability
struct _uavail {
	u8 available[PLAYER_COUNT][UNIT_TYPE_COUNT];
};

class StringTbl {

public:

    /// Returns the number of string entries in the TBL file.
	u16 getStringCount() const { return *buffer; }

    /// Returns the string at @p index in the TBL file.
    /// If the @p index is 0, returns NULL.
    /// If the @p index is out of bounds, returns an empty string.
	const char* getString(u16 index) const {

		//Based on function @ 0x004C36F0

		if (index == 0)
			return NULL;
		else 
		if (index <= getStringCount())
			return (const char*)(buffer) + buffer[index];
		else
			return (const char*)0x00501B7D;	//StringEmpty in scbwdata.h, SC default empty string
    }

private:
	u16* buffer;

};

#pragma pack()

//Based on BWAPI's BW/CBullet.h

#pragma once
#include "Target.h"
#pragma pack(1)

//V241 for VS2008

struct CUnit;
struct CSprite;

// See Unit.h and compare this entire structure with the first part of the unit structure
// I'm not sure how much of it is the same so I didn't bother updating this.
struct CBullet {
// v-- POSSIBLE SHARED CBullet/CUnit STRUCTURE BEGIN
  /*0x00*/ CBullet      *previous;
  /*0x04*/ CBullet      *next;
  /*0x08*/ u32          exists; //usually holds 1 (exists?)
  /*0x0C*/ CSprite      *sprite; // sprite pointer
  /*0x10*/ Target       moveTarget; //address? animation related? changes a during the same bullet
  /*0x18*/ Point16      targetPosition;
  /*0x1C*/ Point16      targetPosition2; //usually same as targetPosition
  /*0x20*/ u8           movementFlags; //usually holds 0x09 for probes, 0x0b for dragoons, 0x11 for mutas
  /*0x21*/ u8           direction1; //not sure how this and direction2 differ from currentDirection and velocityDirection
  /*0x22*/ u8           flingyTurnRadius;
  /*0x23*/ u8           direction2;
  /*0x24*/ u16          type;
  /*0x26*/ u8           unknown_0x26;
  /*0x27*/ u8           flingyMoveControl;
  /*0x28*/ Point16      position;
  /*0x2C*/ Point32      halt; // (xHalt >> 4) == xHalt
  /*0x34*/ s32          flingyTopSpeed;
  /*0x38*/ s32          unknown_0x38; //increasing counter or timer
  /*0x3C*/ s32          unknown_0x3C; //increasing counter or timer, often the same value as unknown_0x38
  /*0x40*/ Point32      current_speed;
  /*0x48*/ u16          flingyAcceleration;
  /*0x4A*/ u8           currentDirection;
  /*0x4B*/ u8           velocityDirection; //seems to always be the same as currentDirection
  /*0x4C*/ u8           srcPlayer; // the player ID of the bullet owner
  /*0x4D*/ u8           behaviourTypeInternal; // 0 = none
                                               // 1 = fly
                                               // 2 = fly & follow target
                                               // 3 = bounce
                                               // 4 = persist on site
                                               // 5 = appear on target/instant
                                               // 6 = attack target area
  /*0x4E*/ u16          unknown_0x4E;
  /*0x50*/ u16          someUnitType; // usually holds UnitType::None (Unknown unit type stuff?)
  /*0x52*/ u16          unknown_0x52; // possibly padding
  /*0x54*/ s32          unknown_0x54; // usually holds 0
  /*0x58*/ Target       attackTarget;
// ^-- POSSIBLE SHARED CBullet/CUnit STRUCTURE END

  /*0x60*/ u8           weaponType;
  /*0x61*/ u8           time_remaining;
  /*0x62*/ u8           hitFlags;           // 0x01 = miss target; 0x02 = no damage (hallucination)
  /*0x63*/ u8           remainingBounces;   // Bounce count for mutalisk weapon (official name: ubChain)
  /*0x64*/ CUnit*       sourceUnit;
  /*0x68*/ CUnit*       nextBounceUnit;     // Next unit for bounce, temporary
  /*0x6C*/ u32          cyclicMissileIndex; // some sort of cyclic burst counter (used by valkyries, goliath air attack)
};

C_ASSERT(sizeof(CBullet) == 112);
//static_assert(sizeof(CBullet) == 112, "The size of the CBullet structure is invalid");

#pragma pack()

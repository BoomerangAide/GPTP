//Based on BWAPI's BW/Target.h

#pragma once
#include "common.h"
#pragma pack(1)

struct CUnit;

/** The scbw::Target structure is a position followed by a unit pointer. This
 *  structure is common and seen in CUnit, CBullet, COrder, CAIControl,
 *  AI Captain, and others.
 */
struct Target
{
  Point16       pt;       // official name
  CUnit         *unit;
};

#pragma pack()

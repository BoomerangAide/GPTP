//Based on BWAPI's BW/Position.h

#pragma once
#include "../../types.h"
#pragma pack(1)

struct Point8 {
  s8 x;
  s8 y;
};

struct Point16 {
  u16 x;
  u16 y;

  bool operator ==(const Point16 &rhs) const {
    return x == rhs.x && y == rhs.y;
  }
  bool operator !=(const Point16 &rhs) const {
    return !(*this == rhs);
  }
};

struct Point32 {
  int x;
  int y;
};

struct Box16 {
  s16 left;
  s16 top;
  s16 right;
  s16 bottom;
};

struct Box32 {
  s32 left;
  s32 top;
  s32 right;
  s32 bottom;
};

struct Bounds {
  u16 left;
  u16 top;
  u16 right;
  u16 bottom;
  u16 width;
  u16 height;
};

C_ASSERT(sizeof(Point8) == 2);
C_ASSERT(sizeof(Point16) == 4);
C_ASSERT(sizeof(Point32) == 8);
C_ASSERT(sizeof(Box16) == 8);
C_ASSERT(sizeof(Box32) == 16);
C_ASSERT(sizeof(Bounds) == 12);

//static_assert(sizeof(Point8)  == 2,   "The size of the Point8 structure is invalid");
//static_assert(sizeof(Point16) == 4,   "The size of the Point16 structure is invalid");
//static_assert(sizeof(Point32) == 8,   "The size of the Point32 structure is invalid");
//static_assert(sizeof(Box16)   == 8,   "The size of the Box16 structure is invalid");
//static_assert(sizeof(Box32)   == 16,  "The size of the Box32 structure is invalid");
//static_assert(sizeof(Bounds) == 12, "The size of the Bounds structure is invalid");

#pragma pack()

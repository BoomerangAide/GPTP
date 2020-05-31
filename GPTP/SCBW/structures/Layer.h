#pragma once
#include "../../types.h"

namespace graphics { class Bitmap; }
struct bounds;

struct Layer {
/*00*/  u8  buffers;
//u8  bits;
/*01*/  u8  hasBeenRefreshed  : 1;
/*01*/  u8  unk               : 7;
/*02*/  u16 left;
/*04*/  u16 top;
/*06*/  u16 width;
/*08*/  u16 height;
/*0A*/  u16 alignment;
/*0C*/  graphics::Bitmap  *pSurface;
/*10*/  void (__stdcall *pUpdate)(graphics::Bitmap *pSurface, bounds *pBounds);
};

C_ASSERT(sizeof(Layer) == 20); /*0x14*/
//static_assert(sizeof(Layer) == 20, "The size of the Layer structure is invalid");


struct Layers {
  Layer mouse;
  Layer context;
  Layer dialog;
  Layer placeBuildingA;
  Layer placeBuildingB;
  Layer game;
};

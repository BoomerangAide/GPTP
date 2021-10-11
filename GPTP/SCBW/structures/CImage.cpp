#include "CImage.h"
#include "CSprite.h"
#include <SCBW/api.h>
#include <SCBW/scbwdata.h>
#include <cassert>

void CImage::playIscriptAnim(IscriptAnimation::Enum animation) {

  u32 animation_ = (u8)animation;

  const u32 Func_PlayIscriptAnim = 0x004D8470;  //AKA playImageIscript();
	
  __asm {
    PUSHAD
    PUSH animation_
    MOV ECX, this
    CALL Func_PlayIscriptAnim
    POPAD
  }
	
}

;

//should be equivalent to ImageDestructor @ 0x004D4CE0 with ESI = image
void CImage::free() 
{

	assert(this);

	if (!(screenLayers->game.hasBeenRefreshed))
		scbw::refreshScreen(this->screenPosition.x,
							this->screenPosition.y,
							this->screenPosition.x + this->grpSize.right,
							this->screenPosition.y + this->grpSize.bottom
							);	//should be equivalent to a call to refreshImage @ 0x004970A0 with EAX = this

	//perform identically to original code
	this->parentSprite->images.unlink<&CImage::link>(this);

	this->grpOffset = NULL;

	//perform identically to original code
	unusedImages.insertAfterHead(this);

}

;

//Loosely based on code at @ 0x004D5A50
void CImage::setRemapping(ColorRemapping::Enum remapping) {
  assert(this);
  this->coloringData = colorShift[remapping].data;
}

;

//Identical to function @ 0x004D5A50
void CImage::initializeData(CSprite *parent, u16 imageId, s8 x, s8 y) {
  assert(this);
  this->id = imageId;
  this->grpOffset = imageGrpGraphics[imageId];
  this->flags = ((images_dat::IsTurnable[imageId] & 1) << 3) | ((images_dat::IsClickable[imageId] & 1) << 5);
  this->frameSet = 0;
  this->direction = 0;
  this->frameIndex = 0;
  this->parentSprite = parent;
  this->horizontalOffset = x;
  this->verticalOffset = y;
  this->grpSize = Box16();
  this->coloringData = 0;

  //Initialize iscript data
  this->iscriptHeaderOffset = 0;
  this->iscriptOffset = 0;
  this->unknown14 = 0;
  this->animation = IscriptAnimation::Init;
  this->wait = 0;

  if (images_dat::RLE_Function[imageId] == PaletteType::RLE_PLAYER_SIDE)
    *(u32*)(&this->coloringData) = parent->playerId;
  if (images_dat::RLE_Function[imageId] == PaletteType::RLE_EFFECT)
    this->coloringData = colorShift[images_dat::Remapping[imageId]].data;
}

;

GrpFrame* CImage::getCurrentFrame() const {
	assert(this);
	return &this->grpOffset->frames[this->frameIndex];
}

;

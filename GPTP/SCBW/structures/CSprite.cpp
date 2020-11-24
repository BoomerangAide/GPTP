#include "CSprite.h"
#include "CImage.h"
#include "../scbwdata.h"
#include <algorithm>
#include <cassert>

//Functionally identical to playSpriteIscript() (offset 0x00499D00)
void CSprite::playIscriptAnim(IscriptAnimation::Enum animation, bool bReplaceCurrentScript) {

	assert(this);

	if(bReplaceCurrentScript || !(this->flags & CSprite_Flags::IscriptCode))
		for (CImage* img = this->images.head; img != NULL; img = img->link.next)
			img->playIscriptAnim(animation);

}

//Equivalent to SpriteDestructor @ 0x00497B40
void CSprite::free() {

	assert(this);

	CImage* image = this->images.head;
	s32 sprite_row_position;

	while (image != NULL) {
		CImage* nextImage = image->link.next;
		image->free();
		image = nextImage;
	}

	if( ((s32)this->position.y) < 0 )
		sprite_row_position = (this->position.y + 31) / 32;
	else
		sprite_row_position = (this->position.y) / 32;

	if(sprite_row_position < 0)
		sprite_row_position = 0;
	else
	if(sprite_row_position >= mapTileSize->height)
		sprite_row_position = mapTileSize->height - 1;

	//remove the sprite from the spritesOnTileRow list
	if(spritesOnTileRow->heads[sprite_row_position] == this)
		spritesOnTileRow->heads[sprite_row_position] = this->link.next;

	if(spritesOnTileRow->tails[sprite_row_position] == this)
		spritesOnTileRow->tails[sprite_row_position] = this->link.prev;

	//untie the sprite from the previous and next one
	if(this->link.prev != NULL)
		(this->link.prev)->link.next = this->link.next;

	if(this->link.next != NULL)
		(this->link.next)->link.prev = this->link.prev;

	this->link.prev = NULL;
	this->link.next = NULL;

	//perform identically to original code
	unusedSprites->insertAfterHead<&CSprite::link>(this);

}


void CSprite::setPosition(u16 x, u16 y) {
  assert(this);
  if (this->position.x == x && this->position.y == y) return;

  const int oldTileY = CLAMP(this->position.y / 32, 0, mapTileSize->height - 1);
  const int newTileY = CLAMP(y / 32, 0, mapTileSize->height - 1);

  this->position.x = x;
  this->position.y = y;

  if (oldTileY != newTileY) {
    const CListExtern<CSprite, &CSprite::link>
      spritesOnOldTileRow(spritesOnTileRow->heads[oldTileY],
                          spritesOnTileRow->tails[oldTileY]),
      spritesOnNewTileRow(spritesOnTileRow->heads[newTileY],
                          spritesOnTileRow->tails[newTileY]);

    spritesOnOldTileRow.unlink(this);
    spritesOnNewTileRow.insertAfterHead(this);
  }

  for (CImage *i = this->images.head; i; i = i->link.next)
	  i->flags |= CImage_Flags::Redraw;
}


//-------- Create overlay --------//

void createUpdateImageSomething(CImage *image) {
  const u32 Func_createUpdateImageSomething = 0x004D66B0;

  __asm {
    PUSHAD
    MOV EAX, image
    CALL Func_createUpdateImageSomething
    POPAD
  }
}

void updateImageDirection(CImage *image, u32 direction) {
  const u32 Func_updateImageDirection = 0x004D5EA0;

  __asm {
    PUSHAD
    PUSH direction
    MOV EAX, image
    CALL Func_updateImageDirection
    POPAD
  }
}

//-------- Create overlay --------//

//Identical to function @ 0x00498E00
CImage* CSprite::createOverlay(u32 imageId, s8 x, s8 y, u32 direction) {
  assert(this);
  CImage *overlay = unusedImages.popHead();
  
  if (overlay) {
    if (this->images.head)
      this->images.insertBefore<&CImage::link>(overlay, this->mainGraphic);
    else {
      this->mainGraphic = overlay;
      this->images.insertAfterHead<&CImage::link>(overlay);
    }

    overlay->initializeData(this, imageId, x, y);
    createUpdateImageSomething(overlay);
    updateImageDirection(overlay, direction);
  }
  return overlay;
}

//Identical to function @ 0x00498EA0
//EAX = sprite
//ESI = imageId
//[EBP+0x08] = x OR y
//[EBP+0x0C] = y OR x
//[EBP+0x10] = direction
CImage* CSprite::createTopOverlay(u32 imageId, s8 x, s8 y, u32 direction) {
  assert(this);
  CImage *overlay = unusedImages.popHead();
  
  if (overlay) {
    if (this->images.head)
      this->images.insertBeforeHead<&CImage::link>(overlay);
    else {
      this->mainGraphic = overlay;
      this->images.insertAfterHead<&CImage::link>(overlay);
    }

    overlay->initializeData(this, imageId, x, y);
    createUpdateImageSomething(overlay);
    updateImageDirection(overlay, direction);
  }
  return overlay;
}


//-------- Remove overlay --------//

//Based on function @ 0x004E5CF0
void CSprite::removeOverlay(u32 imageIdStart, u32 imageIdEnd) {
  assert(this);
  assert(imageIdStart <= imageIdEnd);

  for (CImage *image = this->images.head; image; image = image->link.next) {
    if (imageIdStart <= image->id && image->id <= imageIdEnd) {
      image->free();
      return;
    }
  }
}

void CSprite::removeOverlay(u32 imageId) {
  assert(this);

  for (CImage *image = this->images.head; image; image = image->link.next) {
    if (image->id == imageId) {
      image->free();
      return;
    }
  }
}

//-------- Get overlay --------//

//Identical to function @ 0x00497460
CImage* CSprite::getOverlay(u16 imageId) const {
  assert(this);

  for (CImage *image = this->images.head; image; image = image->link.next)
    if (image->id == imageId)
      return image;

  return NULL;
}

//-------- Other --------//

bool CSprite::isVisibleTo(u8 playerId) const {
  assert(this);
  return (this->visibilityFlags & (1 << playerId)) != 0;
}

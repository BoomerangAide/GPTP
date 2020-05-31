//Based on BWAPI's BW/CSprite.h

#pragma once
#include "common.h"
#include "CList.h"
#include "../enumerations.h"
#pragma pack(1)

struct CImage;

//note: the flag number is chosen according to the value used in 
//<< flag operations, that's why it start with 0.
namespace CSprite_Flags {
enum Enum : u8 {
	DrawSelCircle	= 0x01,		//flag 0: 0x01, Draw selection circle.
	Flag01			= 0x02,		//flag 1: 0x02
	Flag02			= 0x04,		//flag 2: 0x04
	Selected		= 0x08,		//flag 3: 0x08, Selected.
	Flag04			= 0x10,		//flag 4: 0x10
	Hidden			= 0x20,		//flag 5: 0x20, Hidden
	Burrowed		= 0x40,		//flag 6: 0x40, Burrowed
	IscriptCode		= 0x80,		//flag 7: 0x80, Iscript unbreakable code section.
};
}

struct CSprite {
//Utility method definitions added by pastelmind

  /// Makes the sprite play the specified Iscript animation entry.
  void playIscriptAnim(IscriptAnimation::Enum animation, bool bReplaceCurrentScript = true);

  /// AKA SpriteDestructor() @ 0x00497B40
  /// Removes the sprite from the linked list of valid images. This is different
  /// from a class destructor, since CSprite objects are stored in a static
  /// array and never truly destroyed.
  void free();

  /// AKA fixSpritePosition @ 0x00497A10
  /// Moves the sprite to the new position.
  void setPosition(u16 x, u16 y);

  /// Creates an image overlay over the main image of this sprite.
  CImage* createOverlay(u32 imageId, s8 x = 0, s8 y = 0, u32 direction = 0);

  /// Creates an image overlay on top of all other images of this sprite.
  CImage* createTopOverlay(u32 imageId, s8 x = 0, s8 y = 0, u32 direction = 0);

  /// Removes the first image overlay with an ID value between @p imageIdStart
  /// and @p imageIdEnd.
  void removeOverlay(u32 imageIdStart, u32 imageIdEnd);

  /// Removes the first image overlay with the given image ID.
  void removeOverlay(u32 imageId);

  /// Returns the overlay image of this sprite that has the given @p imageId.
  /// If the image cannot be found, returns NULL.
  CImage* getOverlay(u16 imageId) const;

  /// Checks whether this sprite is visible to @p playerId (i.e. not covered by
  /// the fog of war). This does NOT check detectability.
  bool isVisibleTo(u8 playerId) const;

////////////////////////////////////////////////////////////////
//Actual data structure

  /*0x00*/ CLink<CSprite> link;
  /*0x08*/ u16       spriteId;
  /*0x0A*/ u8        playerId;          // officially "creator"
  /*0x0B*/ u8        selectionIndex;    // 0 <= selectionIndex <= 11. Index in the selection area at bottom of screen.
  /*0x0C*/ u8        visibilityFlags;   // Determines whether the sprite is visible (not hidden by the fog-of-war).
  /*0x0D*/ u8        elevationLevel;
  /*0x0E*/ u8        flags;				// Use CSprite_Flags
  /*0x0F*/ u8        selectionTimer;
  /*0x10*/ u16       index;				//u32->u16 based on BWAPI/Source/BW/CSprite.h
  /*0x12*/ UNK		   unkflags_12;		//based on BWAPI/Source/BW/CSprite.h
  /*0x13*/ UNK		   unkflags_13;		//based on BWAPI/Source/BW/CSprite.h
  /*0x14*/ Point16   position;
  /*0x18*/ CImage    *mainGraphic;    // officially "pImagePrimary"
  /*0x1C*/ CList<CImage> images;      // officially "pImageHead" and "pImageTail"

  ////////////////////////////////////////////////////////////////////
  // Official Broodwar methods (from beta), ignore these
  /*
  bool  Dead() const;
  bool  HasDamageFrames() const;
  */
};

C_ASSERT(sizeof(CSprite) == 36);
//static_assert(sizeof(CSprite) == 36, "The size of the CSprite structure is invalid");

#pragma pack()

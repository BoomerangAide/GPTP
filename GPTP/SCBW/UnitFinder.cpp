#include "UnitFinder.h"
#include "api.h"
#include <cassert>

namespace scbw {

UnitFinder::UnitFinder() : unitCount(0) {}

UnitFinder::UnitFinder(int left, int top, int right, int bottom)
: unitCount(0) {
  this->search(left, top, right, bottom);
}

int UnitFinder::getUnitCount() const {
  return this->unitCount;
}

CUnit* UnitFinder::getUnit(int index) const {
  if (0 <= index && index <= this->unitCount)
    return this->units[index];
  return NULL;
}

UnitFinderData* UnitFinder::getStartX() {
  return unitOrderingX;
}

UnitFinderData* UnitFinder::getStartY() {
  return unitOrderingY;
}

UnitFinderData* UnitFinder::getEndX() {
  return unitOrderingX + *unitOrderingCount;
}

UnitFinderData* UnitFinder::getEndY() {
  return unitOrderingY + *unitOrderingCount;
}

// The heart and core of StarCraft's unit search engine.
// Based on BWAPI's Shared/Templates.h
void UnitFinder::search(int left, int top, int right, int bottom) {
  //0: No match, 1: Passed X iteration, 2: Passed Y iteration
  //(Necessary because each UnitFinderData array has 2 entries per unit)
  u8 unitSearchFlags[UNIT_ARRAY_LENGTH + 1] = {0};

  int r = right, b = bottom;
  const bool isWidthExtended  = right - left - 1 < *MAX_UNIT_WIDTH;
  const bool isHeightExtended = top - bottom - 1 < *MAX_UNIT_HEIGHT;

  // Check if the location is smaller than the largest unit
  if (isWidthExtended)
    r += *MAX_UNIT_WIDTH;
  if (isHeightExtended)
    b += *MAX_UNIT_HEIGHT;

  // Create UnitFinderData elements for compatibility with stl functions
  UnitFinderData finderVal;

  // Search for the values using built-in binary search algorithm and comparator
  finderVal.position = left;
  UnitFinderData *pLeft   = std::lower_bound(getStartX(), getEndX(), finderVal);

  finderVal.position = top;
  UnitFinderData *pTop    = std::lower_bound(getStartY(), getEndY(), finderVal);

  finderVal.position = r;
  UnitFinderData *pRight  = std::lower_bound(pLeft, getEndX(), finderVal);

  finderVal.position = b;
  UnitFinderData *pBottom = std::lower_bound(pTop, getEndY(), finderVal);
  
  // Iterate the Y entries of the finder
  for (UnitFinderData *py = pTop; py < pBottom; ++py) {
    if (unitSearchFlags[py->unitIndex] == 0) {
      // If height is small, check unit bounds
      if (!isHeightExtended
          || CUnit::getFromIndex(py->unitIndex)->getTop() < bottom)
        unitSearchFlags[py->unitIndex] = 1;
    }
  }

  // Iterate the X entries of the finder
  this->unitCount = 0;
  for (UnitFinderData *px = pLeft; px < pRight; ++px) {
    if (unitSearchFlags[px->unitIndex] == 1) {
      // If width is small, check unit bounds
      if (!isWidthExtended
          || CUnit::getFromIndex(px->unitIndex)->getLeft() < right)
      {
        CUnit* unit = CUnit::getFromIndex(px->unitIndex);
        if (unit)
          this->units[this->unitCount++] = unit;
      }      
      unitSearchFlags[px->unitIndex] = 0; //Prevent duplicates
    }
  }
}

//-------- UnitFinder::getNearest() family --------//

//Based on function @ 0x004E8320
CUnit* UnitFinder::getNearest(int x, int y,
  int boundsLeft, int boundsTop, int boundsRight, int boundsBottom,
  UnitFinderData* left,  UnitFinderData* top,
  UnitFinderData* right, UnitFinderData* bottom,
  UnitFinderCallbackMatchInterface &match, CUnit* sourceUnit)
{

	int bestDistance = scbw::getDistanceFast(0, 0,
    std::max(x - boundsLeft, boundsRight - x),
    std::max(y - boundsTop, boundsBottom - y));

  CUnit* bestUnit = NULL;
  bool canContinueSearch, hasFoundBestUnit;

  do {
    canContinueSearch = false;
    hasFoundBestUnit = false;
    
    //Search to the left
    if (getStartX() <= left) {
      CUnit* unit = CUnit::getFromIndex(left->unitIndex);
      
      if (boundsLeft <= unit->getX()) {
        if (boundsTop <= unit->getY() && unit->getY() < boundsBottom) {
			if (unit != sourceUnit && match.match(unit)) {
				int distance = scbw::getDistanceFast(x, y, unit->getX(), unit->getY());
            if (distance < bestDistance) {
              bestDistance = distance;
              bestUnit = unit;
              hasFoundBestUnit = true;
            }
          }
        }
      }
      else
        left = getStartX() - 1;
      
      canContinueSearch = true;
      --left;
    }
    
    //Search to the right
    if (right < getEndX()) {
      CUnit* unit = CUnit::getFromIndex(right->unitIndex);
      
      if (unit->getX() < boundsRight) {
        if (boundsTop <= unit->getY() && unit->getY() < boundsBottom) {
			if (unit != sourceUnit && match.match(unit)) {
            int distance = scbw::getDistanceFast(x, y, unit->getX(), unit->getY());
            if (distance < bestDistance) {
              bestDistance = distance;
              bestUnit = unit;
              hasFoundBestUnit = true;
            }
          }
        }
      }
      else
        right = getEndX();
      
      canContinueSearch = true;
      ++right;
    }
    
    //Search upwards
    if (getStartY() <= top) {
      CUnit* unit = CUnit::getFromIndex(top->unitIndex);
      
      if (boundsTop <= unit->getY()) {
        if (boundsLeft <= unit->getX() && unit->getX() < boundsRight) {
          if (unit != sourceUnit && match.match(unit)) {
            int distance = scbw::getDistanceFast(x, y, unit->getX(), unit->getY());
            if (distance < bestDistance) {
              bestDistance = distance;
              bestUnit = unit;
              hasFoundBestUnit = true;
            }
          }
        }
      }
      else
        top = getStartY() - 1;
      
      canContinueSearch = true;
      --top;
    }
    
    //Search downwards
    if (bottom < getEndY()) {
      CUnit* unit = CUnit::getFromIndex(bottom->unitIndex);
      
      if (unit->getY() < boundsBottom) {
        if (boundsLeft <= unit->getX() && unit->getX() < boundsRight) {
			if (unit != sourceUnit && match.match(unit)) {
            int distance = scbw::getDistanceFast(x, y, unit->getX(), unit->getY());
            if (distance < bestDistance) {
              bestDistance = distance;
              bestUnit = unit;
              hasFoundBestUnit = true;
            }
          }
        }
      }
      else
        bottom = getEndY();
      
      canContinueSearch = true;
      ++bottom;
    }

    //Reduce the search bounds
    if (hasFoundBestUnit) {
      boundsLeft    = std::max(boundsLeft,    x - bestDistance);
      boundsRight   = std::min(boundsRight,   x + bestDistance);
      boundsTop     = std::max(boundsTop,     y - bestDistance);
      boundsBottom  = std::max(boundsBottom,  y + bestDistance);
    }
  } while (canContinueSearch);

  return bestUnit;
}

CUnit* UnitFinder::getNearestTarget(int left, int top, int right, int bottom,
  CUnit* sourceUnit, UnitFinderCallbackMatchInterface& match)
{
  UnitFinderData *searchLeft, *searchTop, *searchRight, *searchBottom;

  //If the unit sprite is hidden
  if (sourceUnit->sprite->flags & CSprite_Flags::Hidden) {
    UnitFinderData temp;
    temp.position = sourceUnit->getX();
    searchRight   = std::lower_bound(getStartX(), getEndX(), temp);
    searchLeft    = searchRight - 1;
    temp.position = sourceUnit->getY();
    searchBottom  = std::lower_bound(getStartY(), getEndY(), temp);
    searchTop     = searchBottom - 1;
  }
  else {
    searchLeft    = getStartX() + sourceUnit->finderIndex.right - 1;
    searchRight   = getStartX() + sourceUnit->finderIndex.left + 1;
    searchTop     = getStartY() + sourceUnit->finderIndex.bottom - 1;
    searchBottom  = getStartY() + sourceUnit->finderIndex.top + 1;
  }

  return getNearest(sourceUnit->getX(), sourceUnit->getY(),
    left, top, right, bottom,
    searchLeft, searchTop, searchRight, searchBottom,
    match, sourceUnit);
}

CUnit* UnitFinder::getNearestTarget(CUnit* sourceUnit, UnitFinderCallbackMatchInterface& match) {
  return getNearestTarget(0, 0, mapTileSize->width * 32, mapTileSize->height * 32,
    sourceUnit, match);
}

//-------- Template member function definitions --------//

void UnitFinder::forEach(scbw::UnitFinderCallbackProcInterface &callback) const {
  for (int i = 0; i < this->getUnitCount(); ++i)
    callback.proc(this->getUnit(i));
}

CUnit* UnitFinder::getFirst(UnitFinderCallbackMatchInterface &callback) const {

  for (int i = 0; i < this->getUnitCount(); ++i)
	  if (callback.match(this->getUnit(i)))
		return this->getUnit(i);

  return NULL;

}

CUnit* UnitFinder::getBest(scbw::UnitFinderCallbackScoreInterface &callback) const {

  int bestScore = -1;
  CUnit* bestUnit = NULL;

  for (int i = 0; i < this->getUnitCount(); ++i) {

    const int score = callback.score(this->getUnit(i));

    if (score > bestScore) {
		bestUnit = this->getUnit(i);
		bestScore = score;
    }

  }

  return bestUnit;
}

} //scbw

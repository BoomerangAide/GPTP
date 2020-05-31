/// Graphics module for GPTP, adapted from BWAPI.

#pragma once
#include <string>
#include "../types.h"

namespace graphics {

//Based on BWAPI's ColorId.h and ColorId.cpp
typedef u8 ColorId;
const ColorId RED     = 111;  //Player 1 color					(0x67)
const ColorId BLUE    = 165;  //Player 2 color					(0xA5)
const ColorId TEAL    = 159;  //Player 3 color					(0x9F)
const ColorId PURPLE  = 164;  //Player 4 color					(0xA4)
const ColorId ORANGE  = 179;  //Player 5 color					(0xB3)
const ColorId BROWN   = 19;   //Player 6 color					(0x13)
const ColorId WHITE   = 255;  //Player 7 color					(0xFF)
const ColorId YELLOW  = 135;  //Player 8 color					(0x87)
const ColorId GREEN   = 117;  //Player 7 color for Ice tilesets	(0x75)
const ColorId CYAN    = 128;  //Neutral player color			(0x80)
const ColorId BLACK   = 0;	  //								(0x00)
const ColorId GREY    = 74;	  //								(0x4A)

/// For use with getFactionColor() functions, max index is PLAYER_COUNT-1
const static ColorId standardColors[] = { RED , BLUE, TEAL, PURPLE, ORANGE, BROWN, WHITE, YELLOW, GREEN, CYAN, BLACK, GREY };

//Font sizes accepted by graphics::drawTextOnScreen() and drawTextOnMap().
enum FontSize {
  FONT_SMALL    = 0,
  FONT_MEDIUM   = 1,
  FONT_LARGE    = 2,
  FONT_LARGEST  = 3,
};

//Coordinate types
enum CoordType {
  ON_SCREEN,
  ON_MAP,
  ON_MOUSE,  //Shapes drawn this way are positioned relative to the mouse
};

/// Resets all previous graphics and clears error messages. This should be
/// called every frame in hooks/main/game_hooks.cpp.
void resetAllGraphics();

void drawText(int x, int y, const std::string& str,
              FontSize fontSize = FONT_MEDIUM, CoordType ct = ON_SCREEN);

void drawDot(int x, int y, ColorId color, CoordType ct = ON_SCREEN);

void drawLine(int x1, int y1, int x2, int y2, ColorId color, CoordType ct = ON_SCREEN);

void drawBox(int left, int top, int right, int bottom, ColorId color, CoordType ct = ON_SCREEN);

void drawCircle(int x, int y, int radius, ColorId color, CoordType ct = ON_SCREEN);

void drawFilledBox(int left, int top, int right, int bottom, ColorId color, CoordType ct = ON_SCREEN);

void drawFilledCircle(int x, int y, int radius, ColorId color, CoordType ct = ON_SCREEN);

void drawEllipse(int left, int top, int right, int bottom, ColorId color, CoordType ct = ON_SCREEN);

void drawDottedLine(int x1, int y1, int x2, int y2, ColorId color, CoordType ct = ON_SCREEN);

void drawDottedLine2(int x1, int y1, int x2, int y2, ColorId color1, ColorId color2, CoordType ct = ON_SCREEN);

void drawDottedEllipse(int left, int top, int right, int bottom, ColorId color, CoordType ct = ON_SCREEN);

void drawDottedEllipse2(int left, int top, int right, int bottom, ColorId color1, ColorId color2, CoordType ct = ON_SCREEN);

} //graphics

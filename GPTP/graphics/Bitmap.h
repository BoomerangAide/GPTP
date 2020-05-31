//Based on BWAPI's BW/Bitmap.h
#pragma once
#include "graphics.h"
#pragma pack(1)

namespace graphics {

class Bitmap {
  public:
    bool blitString(const char *pszStr, int x, int y, u8 size);
    u16 getWidth() const;
    u16 getHeight() const;

    void drawDot(int x, int y, ColorId color);
    void drawLine(int x1, int y1, int x2, int y2, ColorId color);
    void drawBox(int left, int top, int right, int bottom, ColorId color);
    void drawFilledBox(int left, int top, int right, int bottom, ColorId color);
    void drawCircle(int x, int y, int radius, ColorId color);
    void drawFilledCircle(int x, int y, int radius, ColorId color);
	void drawEllipse(int left, int top, int right, int bottom, ColorId color);
	void drawDottedLine(int x1, int y1, int x2, int y2, ColorId color);
	void drawDottedLine2(int x1, int y1, int x2, int y2, ColorId color1, ColorId color2);
	void drawDottedEllipse(int left, int top, int right, int bottom, ColorId color);
	void drawDottedEllipse2(int left, int top, int right, int bottom, ColorId color1, ColorId color2);

  private:
    u16 width, height;
    u8* data;

    void drawHorizontalLine(int x1, int x2, int y, ColorId color);
    void drawVerticalLine(int x, int y1, int y2, ColorId color);
    void drawDotUnsafe(int x, int y, ColorId color);
    void drawHorizontalLineUnsafe(int x1, int x2, int y, ColorId color);
    void drawVerticalLineUnsafe(int x, int y1, int y2, ColorId color);

    void blitKoreanChar(const char *ch, int &x, int &y, u8 fontSize, u8 color);

    //Trivially checks whether the line should be drawn; based on the trivial
    //check in the Cohen-Sutherland line clipping algorithm.
    bool isLineTriviallyIgnorable(int x1, int y1, int x2, int y2);
    int computeOutcode(int x, int y);
};

} //graphics

#pragma pack()

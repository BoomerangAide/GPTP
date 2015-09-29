#pragma once
#include "../SCBW/structures/common.h"
#include "graphics.h"
#include <string>

//V241 for VS2008

namespace graphics {

//Speed-efficient non-virtual class for storing shapes that will be drawn.
//All shape setter methods merely copy data and contain no actual logic.
//All coordinates are relative to screen.
class Shape {
  public:
    void setText(int x, int y, int stringIndex, FontSize fontSize, CoordType coordType);
    void setDot(int x, int y, ColorId color, CoordType coordType);
    void setLine(int x1, int y1, int x2, int y2, ColorId color, CoordType coordType);
    void setBox(int left, int top, int right, int bottom, ColorId color, CoordType coordType);
    void setCircle(int x, int y, int radius, ColorId color, CoordType coordType);
    void setFilledBox(int left, int top, int right, int bottom, ColorId color, CoordType coordType);
    void setFilledCircle(int x, int y, int radius, ColorId color, CoordType coordType);
    void setEllipse(int left, int top, int right, int bottom, ColorId color, CoordType coordType);
    void setDottedLine(int x1, int y1, int x2, int y2, ColorId color, CoordType coordType);
    void setDottedEllipse(int left, int top, int right, int bottom, ColorId color, CoordType coordType);
    void draw() const;

  private:
    enum ShapeType {
      NONE,
      TEXT,
      DOT,
      LINE,
      BOX,
      CIRCLE,
      FILLED_BOX,
      FILLED_CIRCLE,
      ELLIPSE,
      DOTTED_LINE,
      DOTTED_ELLIPSE
    } type;
    CoordType coordType;
    Point32 p1;
    Point32 p2;
    int radius;
    ColorId color;
    FontSize fontSize;
    int stringIndex;
};

} //graphics

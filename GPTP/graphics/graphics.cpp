#include <cassert>
#include "graphics.h"
#include "graphics_errors.h"
#include "graphics_misc.h"
#include "Shape.h"

namespace graphics {

const int MAX_SHAPES  = 10000;
const int MAX_STRINGS = 2000;

//Static arrays for fast drawing
Shape shapes[MAX_SHAPES];
std::string strings[MAX_STRINGS];
int shapeCount = 0;
int stringCount = 0;

void resetAllGraphics() {
  shapeCount = 0;
  stringCount = 0;
  clearErrors();
}


void drawText(int x, int y, const std::string& str, FontSize fontSize, CoordType ct) {
  if (stringCount >= MAX_STRINGS) {
    setError(ERR_TOO_MANY_STRINGS);
    return;
  }
  if (shapeCount >= MAX_SHAPES) {
    setError(ERR_TOO_MANY_SHAPES);
    return;
  }
  strings[stringCount] = str;
  shapes[shapeCount++].setText(x, y, stringCount++, fontSize, ct);
}

void drawDot(int x, int y, ColorId color, CoordType ct) {
  if (shapeCount >= MAX_SHAPES) {
    setError(ERR_TOO_MANY_SHAPES);
    return;
  }
  shapes[shapeCount++].setDot(x, y, color, ct);
}

void drawLine(int x1, int y1, int x2, int y2, ColorId color, CoordType ct) {
  if (shapeCount >= MAX_SHAPES) {
    setError(ERR_TOO_MANY_SHAPES);
    return;
  }
  shapes[shapeCount++].setLine(x1, y1, x2, y2, color, ct);
}

void drawBox(int left, int top, int right, int bottom, ColorId color, CoordType ct) {
  if (shapeCount >= MAX_SHAPES) {
    setError(ERR_TOO_MANY_SHAPES);
    return;
  }
  shapes[shapeCount++].setBox(left, top, right, bottom, color, ct);
}

void drawCircle(int x, int y, int radius, ColorId color, CoordType ct) {
  if (shapeCount >= MAX_SHAPES) {
    setError(ERR_TOO_MANY_SHAPES);
    return;
  }
  shapes[shapeCount++].setCircle(x, y, radius, color, ct);
}

void drawFilledBox(int left, int top, int right, int bottom, ColorId color, CoordType ct) {
  if (shapeCount >= MAX_SHAPES) {
    setError(ERR_TOO_MANY_SHAPES);
    return;
  }
  shapes[shapeCount++].setFilledBox(left, top, right, bottom, color, ct);
}

void drawFilledCircle(int x, int y, int radius, ColorId color, CoordType ct) {
  if (shapeCount >= MAX_SHAPES) {
    setError(ERR_TOO_MANY_SHAPES);
    return;
  }
  shapes[shapeCount++].setFilledCircle(x, y, radius, color, ct);
}

void drawEllipse(int left, int top, int right, int bottom, ColorId color, CoordType ct) {
  if (shapeCount >= MAX_SHAPES) {
    setError(ERR_TOO_MANY_SHAPES);
    return;
  }
  shapes[shapeCount++].setEllipse(left, top, right, bottom, color, ct);
}

void drawDottedLine(int x1, int y1, int x2, int y2, ColorId color, CoordType ct) {
  if (shapeCount >= MAX_SHAPES) {
    setError(ERR_TOO_MANY_SHAPES);
    return;
  }
  shapes[shapeCount++].setDottedLine(x1, y1, x2, y2, color, ct);
}

void drawDottedLine2(int x1, int y1, int x2, int y2, ColorId color1, ColorId color2,CoordType ct) {
	if (shapeCount >= MAX_SHAPES) {
		setError(ERR_TOO_MANY_SHAPES);
		return;
	}
	shapes[shapeCount++].setDottedLine2(x1, y1, x2, y2, color1, color2, ct);
}

void drawDottedEllipse(int left, int top, int right, int bottom, ColorId color, CoordType ct) {

  if (shapeCount >= MAX_SHAPES) {
    setError(ERR_TOO_MANY_SHAPES);
    return;
  }

  shapes[shapeCount++].setDottedEllipse(left, top, right, bottom, color, ct);

}

void drawDottedEllipse2(int left, int top, int right, int bottom, ColorId color1, ColorId color2, CoordType ct) {

	if (shapeCount >= MAX_SHAPES) {
		setError(ERR_TOO_MANY_SHAPES);
		return;
	}

	shapes[shapeCount++].setDottedEllipse2(left, top, right, bottom, color1, color2, ct);

}

const std::string& getStringFromIndex(int stringIndex) {
  assert(0 <= stringIndex && stringIndex < MAX_STRINGS);
  return strings[stringIndex];
}

int drawAllShapes() {
  int drawnShapeCount = 0;
  for (int i = 0; i < shapeCount; ++i)
    shapes[i].draw();
  drawErrorMessages();
  return shapeCount;  //TODO: Fix this
}

} //graphics

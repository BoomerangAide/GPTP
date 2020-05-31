#include "Shape.h"
#include "graphics_misc.h"
#include "graphics_errors.h"
#include "Bitmap.h"
#include "../SCBW/scbwdata.h"

namespace graphics {

//-------- Registering shapes --------//

void Shape::setText(int x, int y, int stringIndex, FontSize fontSize, CoordType coordType) {
  this->type = TEXT;
  this->p1.x = x;
  this->p1.y = y;
  this->stringIndex = stringIndex;
  this->fontSize = fontSize;
  this->coordType = coordType;
}

void Shape::setDot(int x, int y, ColorId color, CoordType coordType) {
  this->type = DOT;
  this->p1.x = x;
  this->p1.y = y;
  this->color = color;
  this->coordType = coordType;
}

void Shape::setLine(int x1, int y1, int x2, int y2, ColorId color, CoordType coordType) {
  this->type = LINE;
  this->p1.x = x1;
  this->p1.y = y1;
  this->p2.x = x2;
  this->p2.y = y2;
  this->color = color;
  this->coordType = coordType;
}

void Shape::setBox(int left, int top, int right, int bottom, ColorId color, CoordType coordType) {
  this->type = BOX;
  this->p1.x = left;
  this->p1.y = top;
  this->p2.x = right;
  this->p2.y = bottom;
  this->color = color;
  this->coordType = coordType;
}

void Shape::setEllipse(int left, int top, int right, int bottom, ColorId color, CoordType coordType) {
  this->type = ELLIPSE;
  this->p1.x = left;
  this->p1.y = top;
  this->p2.x = right;
  this->p2.y = bottom;
  this->color = color;
  this->coordType = coordType;
}

void Shape::setDottedEllipse(int left, int top, int right, int bottom, ColorId color, CoordType coordType) {
  this->type = DOTTED_ELLIPSE;
  this->p1.x = left;
  this->p1.y = top;
  this->p2.x = right;
  this->p2.y = bottom;
  this->color = color;
  this->coordType = coordType;
}

void Shape::setDottedEllipse2(int left, int top, int right, int bottom, ColorId color1, ColorId color2, CoordType coordType) {
	this->type = DOTTED_ELLIPSE2;
	this->p1.x = left;
	this->p1.y = top;
	this->p2.x = right;
	this->p2.y = bottom;
	this->color = color1;
	this->color2 = color2;
	this->coordType = coordType;
}

void Shape::setDottedLine(int x1, int y1, int x2, int y2, ColorId color, CoordType coordType) {
  this->type = DOTTED_LINE;
  this->p1.x = x1;
  this->p1.y = y1;
  this->p2.x = x2;
  this->p2.y = y2;
  this->color = color;
  this->coordType = coordType;
}

void Shape::setDottedLine2(int x1, int y1, int x2, int y2, ColorId color1, ColorId color2, CoordType coordType) {
	this->type = DOTTED_LINE;
	this->p1.x = x1;
	this->p1.y = y1;
	this->p2.x = x2;
	this->p2.y = y2;
	this->color = color1;
	this->color2 = color2;
	this->coordType = coordType;
}

void Shape::setCircle(int x, int y, int radius, ColorId color, CoordType coordType) {
  this->type = CIRCLE;
  this->p1.x = x;
  this->p1.y = y;
  this->radius = radius;
  this->color = color;
  this->coordType = coordType;
};

void Shape::setFilledBox(int left, int top, int right, int bottom, ColorId color, CoordType coordType) {
  this->type = FILLED_BOX;
  this->p1.x = left;
  this->p1.y = top;
  this->p2.x = right;
  this->p2.y = bottom;
  this->color = color;
  this->coordType = coordType;
};

void Shape::setFilledCircle(int x, int y, int radius, ColorId color, CoordType coordType) {
  this->type = FILLED_CIRCLE;
  this->p1.x = x;
  this->p1.y = y;
  this->radius = radius;
  this->color = color;
  this->coordType = coordType;
};

//-------- Drawing --------//

void Shape::draw() const {
  Point32 p1 = this->p1, p2 = this->p2;
  switch (this->coordType) {
    case ON_SCREEN:
      break;
    case ON_MAP:
      p1.x -= *screenX;
      p1.y -= *screenY;
      p2.x -= *screenX;
      p2.y -= *screenY;
      break;
    case ON_MOUSE:
      p1.x += mouse->x;
      p1.y += mouse->y;
      p2.x += mouse->x;
      p2.y += mouse->y;
      break;
    default:
      setError(ERR_UNKNOWN_COORD_TYPE);
      break;
  }

  switch (this->type) {

    case TEXT:
      //TODO: Add ability to change font size
      gameScreenBuffer->blitString(
        getStringFromIndex(this->stringIndex).c_str(), p1.x, p1.y, this->fontSize);
      break;

    case DOT:
      gameScreenBuffer->drawDot(p1.x, p1.y, this->color);
      break;

    case LINE:
      gameScreenBuffer->drawLine(p1.x, p1.y, p2.x, p2.y, this->color);
      break;

    case BOX:
      gameScreenBuffer->drawBox(p1.x, p1.y, p2.x, p2.y, this->color);
      break;

    case CIRCLE:
      gameScreenBuffer->drawCircle(p1.x, p1.y, this->radius, this->color);
      break;

    case FILLED_BOX:
      gameScreenBuffer->drawFilledBox(p1.x, p1.y, p2.x, p2.y, this->color);
      break;

    case FILLED_CIRCLE:
      gameScreenBuffer->drawFilledCircle(p1.x, p1.y, this->radius, this->color);
      break;

	case ELLIPSE:
      gameScreenBuffer->drawEllipse(p1.x, p1.y, p2.x, p2.y, this->color);
      break;

	case DOTTED_LINE:
      gameScreenBuffer->drawDottedLine(p1.x, p1.y, p2.x, p2.y, this->color);
      break;

	case DOTTED_LINE2:
	  gameScreenBuffer->drawDottedLine2(p1.x, p1.y, p2.x, p2.y, this->color, this->color2);
	  break;

	case DOTTED_ELLIPSE:
      gameScreenBuffer->drawDottedEllipse(p1.x, p1.y, p2.x, p2.y, this->color);
      break;

	case DOTTED_ELLIPSE2:
	  gameScreenBuffer->drawDottedEllipse2(p1.x, p1.y, p2.x, p2.y, this->color, this->color2);
	  break;

    default:
      setError(ERR_UNKNOWN_SHAPE);
      return;
  }
}

} //graphics

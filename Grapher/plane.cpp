#include <TXLib.h>  // BTW, it doesn't compile with -std=c++03

#include "plane.h"

#include <cassert>
#include <cmath>


bool Plane::ctor() {
    leftX = topY = 0;
    width = height = 300;
    centerX = centerY = 150;
    ppuH = ppuV = 10.0d;

    return false;
}

bool Plane::ctor(unsigned new_leftX, unsigned new_topY, unsigned new_width, unsigned new_height) {
    leftX = new_leftX;
    topY = new_topY;
    width = new_width;
    height = new_height;
    centerX = new_width / 2;
    centerY = new_height / 2;
    ppuH = ((double)new_width) / 10.0d;
    ppuV = ((double)new_height) / 10.0d;

    return false;
}

bool Plane::ctor(unsigned new_leftX, unsigned new_topY, unsigned new_width, unsigned new_height,
                 int new_centerX, int new_centerY, double new_ppuH, double new_ppuV) {
    leftX = new_leftX;
    topY = new_topY;
    width = new_width;
    height = new_height;
    centerX = new_centerX;
    centerY = new_centerY;
    ppuH = new_ppuH;
    ppuV = new_ppuV;

    return false;
}

void Plane::dtor() {}

void Plane::shiftBox(int deltaX, int deltaY) {
    leftX += deltaX;
    topY += deltaY;
}

void Plane::scale(double byH, double byV) {
    ppuH /= byH;
    ppuV /= byV;
}

bool Plane::pointToPos(const Vector2 *point, int *x, int *y) const {
    assert(point && x && y);

    *x = point->getX() * ppuH + centerX + leftX;
    *y = point->getY() * ppuV + centerY + topY;

    return false;
}

bool Plane::posToPoint(int x, int y, Vector2 *point) const {
    assert(point);

    return point->ctor((double)(x - leftX - centerX) / ppuH, (double)(y - topY - centerY) / ppuV);
}

bool Plane::drawBase() const {
}

bool Plane::drawGraph(double (*func)(double)) const;

bool Plane::drawVector(const BoundVector2 *vec) const;

bool Plane::drawArrow(const BoundVector2 *vec) const;

bool Plane::drawLine(const BoundVector2 *vec) const;

bool Plane::drawCircle(const Vector2 *pos) const;

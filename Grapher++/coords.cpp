#include "coords.h"


/*

bool Plot::ctor(SDL_Renderer *rend) {
    return ctor(rend, 0, 0, 300, 300);
}

bool Plot::ctor(SDL_Renderer *rend, int new_leftX, int new_topY, int new_width, int new_height) {
    return ctor(rend, new_leftX, new_topY, new_width, new_height,
                new_width / 2, new_height / 2,
                ((double)new_width) / 10.0d, ((double)new_height) / 10.0d);
}

bool Plot::ctor(SDL_Renderer *rend, int new_leftX, int new_topY, int new_width, int new_height,
                 int new_centerX, int new_centerY, double new_ppuH, double new_ppuV) {
    assert(rend);

    clipRect.x = new_leftX;
    clipRect.y = new_topY;
    clipRect.w = new_width;
    clipRect.h = new_height;
    centerX = new_centerX;
    centerY = new_centerY;
    ppuH = new_ppuH;
    ppuV = new_ppuV;

    renderer = rend;

    TRY_B(SDL_SetRenderDrawColor(renderer, PLANE_COLOR_BG, SDL_ALPHA_OPAQUE));
    TRY_B(SDL_RenderSetClipRect(renderer, &clipRect));

    return false;
}

void Plot::pointToPos(const Vector2 *point, int *x, int *y) const {
    assert(point);

    if (x)  *x = point->x * ppuH + centerX + clipRect.x;
    if (y)  *y = -point->y * ppuV + centerY + clipRect.y;
}

void Plot::posToPoint(int x, int y, Vector2 *point) const {
    assert(point);

    point->ctor((double)(x - clipRect.x - centerX) / ppuH, -(double)(y - clipRect.y - centerY) / ppuV);
}

*/


Coords::Coords() :
    Coords({0, 0, 300, 300}) {}

Coords::Coords(const Coords &other) :
    clipRect{other.clipRect},
    center{other.center[0], other.center[1]},
    ppu{other.ppu[0], other.ppu[1]} {}

Coords &Coords::operator=(const Coords &other) {
    clipRect = other.clipRect;

    centerX(other.centerX());
    centerY(other.centerY());

    ppuH(other.ppuH());
    ppuV(other.ppuV());

    return *this;
}

Coords::Coords(const Rect<int> &new_clipRect) :
    Coords(new_clipRect, new_clipRect.w() / 2, new_clipRect.h() / 2) {}

Coords::Coords(const Rect<int> &new_clipRect, int new_centerX, int new_centerY) :
    Coords(new_clipRect, new_centerX, new_centerY,
           (double)new_clipRect.w() / 10.d, (double)new_clipRect.h() / 10.d) {}

Coords::Coords(const Rect<int> &new_clipRect, int new_centerX, int new_centerY,
               double new_ppuH, double new_ppuV) :
    clipRect{new_clipRect}, center{new_centerX, new_centerY}, ppu{new_ppuH, new_ppuV} {}

/// Converts from the virtual coordinates to the screen ones, with rounding if necessary
void Coords::virt2screen(const Vector2 &point, int *x, int *y) const {
    if (x)  *x =  point.x * ppuH() + centerX() + clipRect.x();
    if (y)  *y = -point.y * ppuV() + centerY() + clipRect.y();
}

/// Converts from the screen coordinates to the virtual ones
void Coords::screen2virt(Vector2 &dest, int x, int y) const {
    dest = {(double)(x - clipRect.x() - centerX()) / ppuH(), -(double)(y - clipRect.y() - centerY()) / ppuV()};
}


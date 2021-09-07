#include <coords.h>


bool Coords::ctor();

void Coords::ctor(const Coords *other);

/// Rect
void Coords::ctor(const Rect<int> *new_clipRect);
void Coords::ctor(const Rect<int> *new_clipRect, int new_centerX, int new_centerY);
void Coords::ctor(const Rect<int> *new_clipRect, int new_centerX, int new_centerY,
                  double new_ppuH, double new_ppuV);

/// Width-Height
void Coords::ctorWH(int new_x, int new_y, int new_w, int new_h);
void Coords::ctorWH(int new_x, int new_y, int new_w, int new_h,
                    int new_centerX, int new_centerY);
void Coords::ctorWH(int new_x, int new_y, int new_w, int new_h,
                    int new_centerX, int new_centerY, double new_ppuH, double new_ppuV);

/// Start-end
void Coords::ctorSE(int new_x0, int new_y0, int new_x1, int new_y1);
void Coords::ctorSE(int new_x0, int new_y0, int new_x1, int new_y1,
                    int new_centerX, int new_centerY);
void Coords::ctorSE(int new_x0, int new_y0, int new_x1, int new_y1,
                    int new_centerX, int new_centerY, double new_ppuH, double new_ppuV);

void Coords::dtor();

#ifndef PLANE_H
#define PLANE_H

#include "general.h"
#include "vector2.h"

#include "SDL2/SDL.h"


bool clearScreen(SDL_Renderer *renderer);


class Plane {
public:
    FACTORIES(Plane)

    /// Warning: do not use, other than explicitly
    bool ctor();

    bool ctor(SDL_Renderer *rend);

    bool ctor(SDL_Renderer *rend, int new_leftX, int new_topY, int new_width, int new_height);

    bool ctor(SDL_Renderer *rend, int new_leftX, int new_topY, int new_width, int new_height,
              int new_centerX, int new_centerY, double new_ppuH, double new_ppuV);

    void dtor();

    bool shiftBox(int deltaX, int deltaY);

    void scale(double byH, double byV);

    /// Converts from the virtual coordinates to the screen ones, with rounding if necessary
    void pointToPos(const Vector2 *point, int *x, int *y) const;

    /// Converts from the physical coordinates to the screen ones
    void posToPoint(int x, int y, Vector2 *point) const;

    bool drawBase() const;

    bool drawGraph(double (*func)(double)) const;

    bool drawVector(const BoundVector2 *vec) const;

    bool drawArrow(const BoundVector2 *vec) const;

protected:
    SDL_Rect viewport;
    int centerX, centerY;
    double ppuH, ppuV;  // Pixels per Unit on Horizontal and Vertical axis

    SDL_Renderer *renderer;

    /// The following private draw functions don't set the brush nor do they render

    bool drawLine_(const BoundVector2 *vec) const;

    bool drawLine_(const Vector2 *start, const Vector2 *end) const;

    bool drawCircle_(const Vector2 *pos, unsigned radius = 2) const;

    bool drawDash_(const Vector2 *pos, bool horizontal, unsigned length) const;

};


#endif // PLANE_H

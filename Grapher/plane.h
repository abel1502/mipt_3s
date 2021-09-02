#ifndef PLANE_H
#define PLANE_H

#include "general.h"
#include "vector2.h"


class Plane {
public:
    FACTORIES(Plane)

    bool ctor();

    bool ctor(unsigned new_leftX, unsigned new_topY, unsigned new_width, unsigned new_height);

    bool ctor(unsigned new_leftX, unsigned new_topY, unsigned new_width, unsigned new_height,
              int new_centerX, int new_centerY, double new_ppuH, double new_ppuV);

    void dtor();

    void shiftBox(int deltaX, int deltaY);

    void scale(double byH, double byV);

    /// Converts from the virtual coordinates to the screen ones, with rounding if necessary
    bool pointToPos(const Vector2 *point, int *x, int *y) const;

    /// Converts from the physical coordinates to the screen ones
    bool posToPoint(int x, int y, Vector2 *point) const;

    bool drawBase() const;

    bool drawGraph(double (*func)(double)) const;

    bool drawVector(const BoundVector2 *vec) const;

    bool drawArrow(const BoundVector2 *vec) const;

    bool drawLine(const BoundVector2 *vec) const;

    bool drawCircle(const Vector2 *pos) const;

protected:
    unsigned leftX, topY;
    unsigned width, height;
    int centerX, centerY;
    double ppuH, ppuV;  // Pixels per Unit on Horizontal and Vertical axis

};


#endif // PLANE_H

#ifndef PLANE_H
#define PLANE_H

#include "general.h"


class Plane {
public:
    FACTORIES(Plane)

    bool ctor();

    bool ctor(unsigned new_leftX, unsigned new_topY, unsigned new_width, unsigned new_height);

    bool ctor(unsigned new_leftX, unsigned new_topY, unsigned new_width, unsigned new_height,
              unsigned new_centerX, unsigned new_centerY, double new_ppuH, double new_ppuY);

    void dtor();

    void shiftBox(unsigned deltaX, unsigned deltaY);

    void shift(double deltaH, double deltaV);

    void scale(double byH, double byV);

    bool drawBase() const;

    bool drawGraph(double (*func)(double))

protected:
    unsigned leftX, topY;
    unsigned width, height;
    unsigned centerX, centerY;
    double ppuH, ppyV;  // Pixels per Unit on Horizontal and Vertical axis

};


#endif // PLANE_H

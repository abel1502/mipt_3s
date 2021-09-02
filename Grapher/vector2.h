#ifndef VECTOR2_H
#define VECTOR2_H

#include "general.h"


class Vector2 {
public:
    FACTORIES(Vector2)

    bool ctor();

    bool ctor(double new_x, double new_y);

    void dtor();

protected:
    double x, y;

};


class BoundVector2 {
public:
    FACTORIES(BoundVector2)

    bool ctor();

    bool ctor(const Vector2 *new_start, const Vector2 *new_vec);

    void dtor();

protected:
    const Vector2 *start, vec;

};


#endif // VECTOR2_H

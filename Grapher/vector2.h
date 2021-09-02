#ifndef VECTOR2_H
#define VECTOR2_H

#include "general.h"


class Vector2 {
public:
    static const Vector2 ZERO;

    FACTORIES(Vector2)

    bool ctor();

    bool ctor(double new_x, double new_y);

    bool ctor(const Vector2 *other);

    void dtor();

    inline double getX() const {
        return x;
    }

    inline double getY() const {
        return y;
    }

    void iadd(const Vector2 *other);

    void isub(const Vector2 *other);

    void imul(double val);

    void idiv(double val);

protected:
    double x, y;

    // Not for actual use, but for unavoidable implicit purposes. Pretty much not used anywhere
    inline Vector2() :
        x(0.0d), y(0.0d) {}

    inline Vector2(double new_x, double new_y) :
        x(new_x), y(new_y) {}

};


class BoundVector2 {
public:
    FACTORIES(BoundVector2)

    bool ctor();

    bool ctor(const Vector2 *new_start, const Vector2 *new_vec);

    bool ctor(const BoundVector2 *other);

    void dtor();

    inline const Vector2 *getStart() const {
        return start;
    }

    inline const Vector2 *getVec() const {
        return vec;
    }

    void getEnd(Vector2 *dest) const;

protected:
    const Vector2 *start, *vec;

};


#endif // VECTOR2_H

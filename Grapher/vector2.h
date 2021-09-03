#ifndef VECTOR2_H
#define VECTOR2_H

#include "general.h"


MOLECULAR_CLASS Vector2 {
public:
    static const Vector2 ZERO;
    static const Vector2 ONE_X;
    static const Vector2 ONE_Y;

    double x, y;

    // Forced explicit definition due to the other, private explicit constructor
    inline Vector2() :
        x(0.0d), y(0.0d) {}

    FACTORIES(Vector2)

    bool ctor();

    void ctor(double new_x, double new_y);

    void ctor(const Vector2 *other);

    void dtor();

    void iadd(const Vector2 *other);

    void isub(const Vector2 *other);

    void imul(double val);

    void idiv(double val);

    double length() const;

protected:
    // Not for actual use, but for unavoidable implicit purposes. Pretty much not used anywhere
    inline Vector2(double new_x, double new_y) :
        x(new_x), y(new_y) {}

};


MOLECULAR_CLASS BoundVector2 {
public:
    FACTORIES(BoundVector2)

    bool ctor();

    void ctor(const Vector2 *new_start, const Vector2 *new_vec);

    void ctor(const BoundVector2 *other);

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

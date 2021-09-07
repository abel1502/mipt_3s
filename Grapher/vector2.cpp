#include <cassert>

#include "vector2.h"


const Vector2 Vector2::ZERO{0.0d, 0.0d};
const Vector2 Vector2::ONE_X{1.0d, 0.0d};
const Vector2 Vector2::ONE_Y{0.0d, 1.0d};


bool Vector2::ctor() {
    x = 0.d;
    y = 0.d;

    return false;
}

void Vector2::ctor(double new_x, double new_y) {
    x = new_x;
    y = new_y;
}

void Vector2::ctor(const Vector2 *other) {
    assert(other);

    x = other->x;
    y = other->y;
}

void Vector2::dtor() {}

void Vector2::iadd(const Vector2 *other) {
    assert(other);

    x += other->x;
    y += other->y;
}

void Vector2::isub(const Vector2 *other) {
    assert(other);

    x -= other->x;
    y -= other->y;
}

void Vector2::imul(double val) {
    x *= val;
    y *= val;
}

void Vector2::idiv(double val) {
    assert(val != 0.0d && val != -0.0d);

    x /= val;
    y /= val;
}

void Vector2::scaleBy(double coeff) {
    assert(std::isfinite(coeff));

    x *= coeff;
    y *= coeff;
}

void Vector2::rotateRadians(double radians) {
    double new_x = cos(radians) * x - sin(radians) * y;
    y = sin(radians) * x + cos(radians) * y;
    x = new_x;
}

double Vector2::length() const {
    return hypot(x, y);
}

//--------------------------------------------------------------------------------

bool BoundVector2::ctor() {
    start = nullptr;
    vec = nullptr;

    return false;
}

void BoundVector2::ctor(const Vector2 *new_start, const Vector2 *new_vec) {
    start = new_start;
    vec = new_vec;
}

void BoundVector2::ctor(const BoundVector2 *other) {
    assert(other);

    start = other->start;
    vec = other->vec;
}

void BoundVector2::dtor() {
    start = nullptr;
    vec = nullptr;
}

void BoundVector2::getEnd(Vector2 *dest) const {
    assert(dest);

    /*dest->dtor();  // Just in case
    REQUIRE(!dest->ctor(start));  // It should be unconditionally true, so we can afford an assertion-check*/
    // Now that Vector2 is molecular, this got a lot easier
    dest->ctor(start);
    dest->iadd(vec);
}


#include <cassert>

#include "vector2.h"


const Vector2 Vector2::ZERO{0.0d, 0.0d};


bool Vector2::ctor() {
    x = 0.d;
    y = 0.d;

    return false;
}

bool Vector2::ctor(double new_x, double new_y) {
    x = new_x;
    y = new_y;

    return false;
}

bool Vector2::ctor(const Vector2 *other) {
    assert(other);

    x = other->x;
    y = other->y;

    return false;
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

//--------------------------------------------------------------------------------

bool BoundVector2::ctor() {
    start = nullptr;
    vec = nullptr;

    return false;
}

bool BoundVector2::ctor(const Vector2 *new_start, const Vector2 *new_vec) {
    start = new_start;
    vec = new_vec;

    return false;
}

bool BoundVector2::ctor(const BoundVector2 *other) {
    assert(other);

    start = other->start;
    vec = other->vec;

    return false;
}

void BoundVector2::dtor() {
    start = nullptr;
    vec = nullptr;
}

void BoundVector2::getEnd(Vector2 *dest) const {
    assert(dest);

    dest->dtor();  // Just in case
    REQUIRE(!dest->ctor(start));  // It should be unconditionally true, so we can afford an assertion-check
    dest->iadd(vec);
}


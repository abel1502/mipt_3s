#include <cassert>

#include "vector2.h"


const Vector2 Vector2::ZERO{0.0d, 0.0d};
const Vector2 Vector2::ONE_X{1.0d, 0.0d};
const Vector2 Vector2::ONE_Y{0.0d, 1.0d};


Vector2::Vector2() :
    x{0.0d}, y{0.0d} {}

Vector2::Vector2(double new_x, double new_y) :
    x{new_x}, y{new_y} {}

Vector2::Vector2(const Vector2 &other) :
    x{other.x}, y{other.y} {}

Vector2 &Vector2::operator=(const Vector2 &other) {
    x = other.x;
    y = other.y;

    return *this;
}

Vector2 &Vector2::operator+=(const Vector2 &other) {
    x += other.x;
    y += other.y;

    return *this;
}

Vector2 &Vector2::operator-=(const Vector2 &other) {
    x -= other.x;
    y -= other.y;

    return *this;
}

Vector2 &Vector2::operator*=(double other) {
    x *= other;
    y *= other;

    return *this;
}

Vector2 &Vector2::operator/=(double other) {
    x /= other;
    y /= other;

    return *this;
}

// Scalar multiplication
double Vector2::operator*(const Vector2 &other) const {
    return x * other.x + y * other.y;
}

// Vector multiplication
double Vector2::operator&(const Vector2 &other) const {
    return x * other.y - y * other.x;
}

Vector2 &Vector2::normalize() {
    double length_ = length();

    if (length_ == 0.d)
        throw new error("Zero division");

    return *this *= 1 / length_;
}

Vector2 Vector2::normalized() {
    Vector2 result{*this};

    return result.normalize();
}

Vector2 &Vector2::orthogonalize() {
    std::swap(x, y);
    y *= -1.d;

    return *this;
}

Vector2 Vector2::orthogonalized() {
    Vector2 result{*this};

    return result.orthogonalize();
}

Vector2 &Vector2::rotateRadians(double radians) {
    double new_x = cos(radians) * x - sin(radians) * y;
    y = sin(radians) * x + cos(radians) * y;
    x = new_x;

    return *this;
}

double Vector2::length() const {
    return hypot(x, y);
}



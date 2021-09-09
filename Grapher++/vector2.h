#ifndef VECTOR2_H
#define VECTOR2_H

#define _USE_MATH_DEFINES
#include <cmath>
#include <stdexcept>

#include "general.h"


class Vector2 {
public:
    class error : std::runtime_error {
    public:
        error(const char *what) : std::runtime_error(what) {}
    };

    static const Vector2 ZERO;
    static const Vector2 ONE_X;
    static const Vector2 ONE_Y;

    double x, y;

    Vector2();

    Vector2(double new_x, double new_y);

    Vector2(const Vector2 &other);

    Vector2 &operator=(const Vector2 &other);

    Vector2 &operator+=(const Vector2 &other);

    Vector2 &operator-=(const Vector2 &other);

    Vector2 &operator*=(double other);

    Vector2 &operator/=(double other);

    /// Scalar multiplication
    double operator*(const Vector2 &other) const;

    /// Vector multiplication
    double operator&(const Vector2 &other) const;

    Vector2 &normalize();

    Vector2 normalized();

    Vector2 &orthogonalize();

    Vector2 orthogonalized();

    Vector2 &rotateRadians(double radians);

    inline Vector2 &rotateDegrees(double degrees) {
        rotateRadians(degrees / 180.d * M_PI);

        return *this;
    }

    inline Vector2 rotatedRadians(double radians) {
        Vector2 result{*this};

        return result.rotateRadians(radians);
    }

    inline Vector2 rotatedDegrees(double degrees) {
        return rotatedRadians(degrees / 180.d * M_PI);
    }

    double length() const;

    double length2() const;

    inline bool isZero() const {
        return x == 0 && y == 0;
    }

protected:
    //

};

inline Vector2 operator+(Vector2 self, const Vector2 &other) {
    return self += other;
}

inline Vector2 operator-(Vector2 self, const Vector2 &other) {
    return self -= other;
}

inline Vector2 operator*(Vector2 self, double other) {
    return self *= other;
}

inline Vector2 operator/(Vector2 self, double other) {
    return self /= other;
}

inline Vector2 operator-(Vector2 self) {
    return self *= -1.d;
}



#endif // VECTOR2_H

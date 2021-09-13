#ifndef COLOR_H
#define COLOR_H

#include "ACL/general.h"
#include "ACL/math/vector.h"

using abel::math::Vector3f;


struct PackedColor {
    unsigned char R = 0,
                  G = 0,
                  B = 0,
                  A = 0;
};


class Color : Vector3f {
public:
    constexpr Color() :
        Vector3f() {}

    constexpr Color(float new_r, float new_g, float new_b) :
        Vector3f(new_r, new_g, new_b) {}

    template <typename T>
    constexpr Color(const abel::math::Vector3<T> &other) :
        abel::math::Vector3f(other) {}

    template <typename T>
    constexpr Color &operator=(const abel::math::Vector3<T> &other) {
        Vector3f::operator=(other);

        return *this;
    }

    constexpr float &r() {
        return values[0];
    }

    constexpr const float &r() const {
        return values[0];
    }

    constexpr float &g() {
        return values[1];
    }

    constexpr const float &g() const {
        return values[1];
    }

    constexpr float &b() {
        return values[2];
    }

    constexpr const float &b() const {
        return values[2];
    }

    constexpr Color &clamp() {
        for (unsigned i = 0; i < DIM; ++i) {
            if (values[i] < 0.f)
                values[i] = 0.f;

            if (values[i] > 1.f)
                values[i] = 1.f;
        }

        return *this;
    }

    constexpr Color clamped() const {
        Color result{*this};

        return result.clamp();
    }

    constexpr PackedColor pack() const {
        Color tmp = clamped() * 255;

        return PackedColor{(unsigned char)tmp.r(),
                           (unsigned char)tmp.g(),
                           (unsigned char)tmp.b()};
    }

    constexpr Color &reflect(const Color &other) {
        r() *= other.r();
        g() *= other.g();
        b() *= other.b();

        return *this;
    }

    constexpr Color reflected(const Color &other) const {
        Color result{*this};

        return result.reflect(other);
    }

};


#endif // COLOR_H


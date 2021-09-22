#ifndef COLOR_H
#define COLOR_H

#include <ACL/general.h>
#include <ACL/math/vector.h>
#include <cstdint>

using abel::math::Vector3f;


struct PackedColor {
    constexpr PackedColor() :
        PackedColor(0, 0, 0) {}

    constexpr PackedColor(unsigned char r, unsigned char g, unsigned char b) :
        R{r}, G{g}, B{b}, A{0} {}

    union {
        struct {
            unsigned char R, G, B, A;
        };

        uint32_t value;
    };
};
static_assert(sizeof(PackedColor) == 4);
static_assert(offsetof(PackedColor, R) == 0);
static_assert(offsetof(PackedColor, G) == 1);
static_assert(offsetof(PackedColor, B) == 2);
static_assert(offsetof(PackedColor, A) == 3);


class Color : public Vector3f {
public:
    constexpr Color() noexcept :
        Vector3f() {}

    constexpr Color(float new_r, float new_g, float new_b) noexcept :
        Vector3f(new_r, new_g, new_b) {}

    template <typename T>
    constexpr Color(const abel::math::Vector3<T> &other) noexcept :
        abel::math::Vector3f(other) {}

    template <typename T>
    constexpr Color &operator=(const abel::math::Vector3<T> &other) noexcept {
        Vector3f::operator=(other);

        return *this;
    }

    static constexpr Color fromRGB(unsigned char r, unsigned char g, unsigned char b) {
        return Vector3f(r, g, b) / 255.f;
    }

    constexpr float &r() noexcept {
        return values[0];
    }

    constexpr const float &r() const noexcept {
        return values[0];
    }

    constexpr float &g() noexcept {
        return values[1];
    }

    constexpr const float &g() const noexcept {
        return values[1];
    }

    constexpr float &b() noexcept {
        return values[2];
    }

    constexpr const float &b() const noexcept {
        return values[2];
    }

    constexpr Color &clamp() noexcept {
        for (unsigned i = 0; i < DIM; ++i) {
            if (values[i] < 0.f)
                values[i] = 0.f;

            if (values[i] > 1.f)
                values[i] = 1.f;
        }

        return *this;
    }

    constexpr Color clamped() const noexcept {
        Color result{*this};

        return result.clamp();
    }

    constexpr PackedColor pack() const noexcept {
        Color tmp = clamped() * 255;

        return PackedColor{(unsigned char)tmp.r(),
                           (unsigned char)tmp.g(),
                           (unsigned char)tmp.b()};
    }

    constexpr Color &reflect(const Color &other) noexcept {
        r() *= other.r();
        g() *= other.g();
        b() *= other.b();

        return *this;
    }

    constexpr Color reflected(const Color &other) const noexcept {
        Color result{*this};

        return result.reflect(other);
    }

};


#endif // COLOR_H


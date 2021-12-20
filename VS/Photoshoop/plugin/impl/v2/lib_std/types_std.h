#pragma once


#include <cstdint>
#include <cmath>
#include <ACL/math/vector.h>
#include <ACL/gui/color.h>


namespace plugin {

enum Status {
    OK,
    ERR,
};

enum FeatureLevel {
    SHADER_SUPPORT   = 1 << 0,
    SETTINGS_SUPPORT = 1 << 1,
};

enum PluginType {
    EFFECT,
    TOOL,
    EXTENSION
};

enum BlendMode {
    COPY,
    ALPHA_BLEND,
};

enum ShaderType {
    VERTEX,
    FRAGMENT,
    COMPUTE,
};

enum MouseButton {
    none,
    left,
    right,
    middle
};

template <typename T>
union Vec2T {
    struct {
        T x;
        T y;
    };
    T data[2];

    constexpr Vec2T()         : x(0),   y(0)   {}
    constexpr Vec2T(T val)    : x(val), y(val) {}
    constexpr Vec2T(T x, T y) : x(x),   y(y)   {}

    template <typename U>
    constexpr Vec2T(const abel::math::Vector2<U> &vec) :
        x{(T)vec.x()}, y{(T)vec.y()} {}

    template <typename U>
    constexpr operator abel::math::Vector2<U>() const {
        return abel::math::Vector2<U>{(U)x, (U)y};
    }

};

using Vec2f = Vec2T<float>;
using Vec2d = Vec2T<double>;
using Vec2i = Vec2T<int>;
using Vec2s = Vec2T<size_t>;


union RGBA {
    struct {
        unsigned char r;
        unsigned char g;
        unsigned char b;
        unsigned char a;
    };
    unsigned char rgba[4];
    uint32_t ui32;

    constexpr RGBA()           : ui32(0) {}
    constexpr RGBA(uint32_t x) : ui32(x) {}

    constexpr RGBA(unsigned char r, unsigned char g, unsigned char b, unsigned char a = 255) : r(r), g(g), b(b), a(a) {}

    constexpr RGBA(const abel::gui::PackedColor &color) :
        r{color.R}, g{color.G}, b{color.B}, a{color.A} {}

    constexpr operator abel::gui::PackedColor() const {
        return abel::gui::PackedColor{r, g, b, a};
    }

};

struct Shader {
    virtual ~Shader() {}

    virtual void set_uniform_int      (const char *name, int  val) = 0;
    virtual void set_uniform_int_arr  (const char *name, int *val, size_t cnt) = 0;

    virtual void set_uniform_float    (const char *name, float  val) = 0;
    virtual void set_uniform_float_arr(const char *name, float *val, size_t cnt) = 0;
};

struct RenderMode {
    BlendMode blend;
    Shader *shader;

    RenderMode() : blend(BlendMode::ALPHA_BLEND), shader(nullptr) {}
    RenderMode(const BlendMode &blend, Shader *shader = nullptr) : blend(blend), shader(shader) {}
};

}

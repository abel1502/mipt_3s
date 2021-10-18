#ifndef ACL_GUI_RECT_H
#define ACL_GUI_RECT_H

#include <ACL/math/vector.h>
#include <ACL/type_traits.h>


namespace abel::gui {


template <typename T>
class Rect {
    //static_assert(std::is_arithmetic_v<T>);

public:
    constexpr Rect<T>() noexcept :
        x_{0}, y_{0}, w_{0}, h_{0} {}

    /// Width-height
    constexpr Rect<T>(T new_x, T new_y, T new_w, T new_h) noexcept {
        x(new_x);
        y(new_y);
        w(new_w);
        h(new_h);
    }

    static constexpr Rect<T> wh(T x, T y, T w, T h) noexcept {
        return Rect(x, y, w, h);
    }

    /// Start-end
    constexpr Rect<T>(T new_x0, T new_y0, T new_x1, T new_y1, int) noexcept {
        x0(new_x0);
        y0(new_y0);
        x1(new_x1);
        y1(new_y1);
    }

    static constexpr Rect<T> se(T x0, T y0, T x1, T y1) noexcept {
        return Rect(x0, y0, x1, y1, 0);
    }

    constexpr T  x() const noexcept { return x_; }
    constexpr T  y() const noexcept { return y_; }
    constexpr T  w() const noexcept { return w_; }
    constexpr T  h() const noexcept { return h_; }
    constexpr T x0() const noexcept { return x_; }
    constexpr T y0() const noexcept { return y_; }
    constexpr T x1() const noexcept { return x_ + w_; }
    constexpr T y1() const noexcept { return y_ + h_; }

    constexpr void  x(T val) noexcept { x_ = val; }
    constexpr void  y(T val) noexcept { y_ = val; }
    constexpr void  w(T val) noexcept { w_ = val; }
    constexpr void  h(T val) noexcept { h_ = val; }
    constexpr void x0(T val) noexcept { x_ = val; }
    constexpr void y0(T val) noexcept { y_ = val; }
    constexpr void x1(T val) noexcept { w_ = val - x_; }
    constexpr void y1(T val) noexcept { h_ = val - y_; }

    template <typename T2>
    constexpr operator Rect<T2>() const {
        return Rect<T2>((T2)x(), (T2)y(), (T2)w(), (T2)h());
    }


    using Vector2T = typename abel::math::Vector2<typename abel::make_signed_t<T>>;

    constexpr Vector2T getStart() const { return Vector2T(x0(), y0()); }
    constexpr Vector2T getEnd()   const { return Vector2T(x1(), y1()); }
    constexpr Vector2T getDiag()  const { return Vector2T(w(), h());   }

    constexpr Vector2T getVertex(bool xCoord, bool yCoord) const {
        return Vector2T(x() + xCoord * w(),
                        y() + yCoord * h());
    }

    /// Width-height, vector
    constexpr Rect<T>(Vector2T from, Vector2T size) noexcept {
        x(from.x());
        y(from.y());
        w(size.x());
        h(size.y());
    }

    static constexpr Rect<T> wh(Vector2T from, Vector2T size) noexcept {
        return Rect(from, size);
    }

    /// Start-end, vector
    constexpr Rect<T>(Vector2T from, Vector2T to, int) noexcept {
        x0(from.x());
        y0(from.y());
        x1(to.x());
        y1(to.y());
    }

    static constexpr Rect<T> se(Vector2T start, Vector2T end) noexcept {
        return Rect(start, end, 0);
    }

protected:
    T x_, y_, w_, h_;

};


}


#endif // ACL_GUI_RECT_H

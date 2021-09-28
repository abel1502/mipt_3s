#ifndef GUI_H
#define GUI_H


#ifndef UNDERLYING_GUI_FRAMEWORK
#error "Specify a gui framework via UNDERLYING_GUI_FRAMEWORK: UGF_SDL2 or UGF_TXLIB"
#endif

#define UGF_SDL2   1
#define UGF_TXLIB  2


#if UNDERLYING_GUI_FRAMEWORK == UGF_TXLIB
#define TX_CONSOLE_MODE SW_SHOW
#include <TXLib.h>  // Because it demands being included first
#endif


#include <ACL/general.h>
#include <ACL/math/vector.h>
#include <ACL/gui/color.h>

#include <type_traits>


DECLARE_ERROR(gui_error, std::runtime_error)


using abel::math::Vector2i;
using abel::gui::Color;
using abel::gui::PackedColor;


template <typename T>
class Rect {
    static_assert(std::is_arithmetic<T>::value);

public:
    Rect<T>() noexcept :
        x_{0}, y_{0}, w_{0}, h_{0} {}

    /// Width-Height
    Rect<T>(T new_x, T new_y, T new_w, T new_h) noexcept {
        x(new_x);
        y(new_y);
        w(new_w);
        h(new_h);
    }

    /// Start-end
    Rect<T>(T new_x0, T new_y0, T new_x1, T new_y1, int) noexcept {
        x0(new_x0);
        y0(new_y0);
        x1(new_x1);
        y1(new_y1);
    }

    inline T  x() const noexcept { return x_; }
    inline T  y() const noexcept { return y_; }
    inline T  w() const noexcept { return w_; }
    inline T  h() const noexcept { return h_; }
    inline T x0() const noexcept { return x_; }
    inline T y0() const noexcept { return y_; }
    inline T x1() const noexcept { return x_ + w_; }
    inline T y1() const noexcept { return y_ + h_; }

    inline void  x(T val) noexcept { x_ = val; }
    inline void  y(T val) noexcept { y_ = val; }
    inline void  w(T val) noexcept { w_ = val; }
    inline void  h(T val) noexcept { h_ = val; }
    inline void x0(T val) noexcept { x_ = val; }
    inline void y0(T val) noexcept { y_ = val; }
    inline void x1(T val) noexcept { w_ = val - x_; }
    inline void y1(T val) noexcept { h_ = val - y_; }

    template <typename T2>
    operator Rect<T2>() const {

    }

protected:
    T x_, y_, w_, h_;

};


class Window;
class Texture;


#if 0
/// This is the skeleton of what implementation-specific modules should define

/// DELTA_T should expand to the name of a double variable where the event loop stores the calculated deltaT.
/// It should be accessible to the code inside MAIN_EVENT_LOOP
#define DELTA_T

/// This macro should expand to a while loop that would execute infinitely until the program is closed.
/// Its responsibility is also to calculate deltaT every tick.
/// TICK_CODE will be executed on every iteration; DELAY, if not zero, will be used as the number
/// of milliseconds for the loop to slip after an iteration.
#define MAIN_EVENT_LOOP(TICK_CODE, DELAY)

class Window {
public:
    Window();

    Window(unsigned width, unsigned height);

    Window(const Rect<unsigned> &pos);

    Window(Window &&other) noexcept;

    Window(const Window &other) = delete;
    Window &operator=(const Window &other) = delete;
    Window &operator=(Window &&other) = delete;

    ~Window() noexcept;

    void render(const Texture &texture);

    void renderAt(const Vector2i &at, const Texture &texture);

    void renderAt(const Rect<unsigned> &at, const Texture &texture);

    void clear();

    void update();

    unsigned width() noexcept;

    unsigned height() noexcept;
};

class Texture {
public:
    Texture();

    Texture(unsigned width, unsigned height);

    Texture(const Window &wnd);

    Texture(Texture &&other) noexcept;
    Texture &operator=(Texture &&other) noexcept;

    Texture(const Texture &other) = delete;
    Texture &operator=(const Texture &other) = delete;

    ~Texture() noexcept;

    PackedColor *getBuf();

    void update();

    unsigned width() noexcept;

    unsigned height() noexcept;
};

#endif


#define MAIN_DECL \
    int main()

#if UNDERLYING_GUI_FRAMEWORK == UGF_SDL2

#include "gui_sdl2.h"
#undef MAIN_DECL
#define MAIN_DECL \
    extern "C" int main(int, char **)

#elif UNDERLYING_GUI_FRAMEWORK == UGF_TXLIB

#include "gui_txlib.h"

#else
#error "Invalid gui framework specified"
#endif


#endif // GUI_H

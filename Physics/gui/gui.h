#ifndef GUI_GUI_H
#define GUI_GUI_H


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
#include <ACL/gui/rect.h>

#include <type_traits>


DECLARE_ERROR(gui_error, std::runtime_error)


using abel::math::Vector2i;
using abel::math::Vector2d;
using abel::gui::Color;
using abel::gui::PackedColor;
using abel::gui::Rect;


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

    void renderAt(const Rect<int> &at, const Texture &texture);

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

    Color *getBuf();

    void update();

    void clear();

    void drawLine(const Vector2d &from, const Vector2d &to);

    void drawLineAlong(const Vector2d &from, const Vector2d &along);

    void drawLineInf(const Vector2d &from, const Vector2d &to);

    void drawLineInfAlong(const Vector2d &from, const Vector2d &along);

    void drawCircle(const Vector2d &center, double radius, const Color &color);

    void drawEllipse(const Vector2d &center, const Vector2d &dimensions, const Color &color);

    void drawSquare(const Vector2d &center, double side, const Color &color);

    void drawRect(const Rect<double> &at, const Color &color);

    unsigned width() const noexcept;

    unsigned height() const noexcept;

    Rect<double> getScreenRect() const noexcept;
    Rect<int> getScreenRectInt() const noexcept;
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


#endif // GUI_GUI_H

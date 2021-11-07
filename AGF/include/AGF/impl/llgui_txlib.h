#ifndef AGF_IMPL_LLGUI_TXLIB_H
#define AGF_IMPL_LLGUI_TXLIB_H

#ifndef AGF_LLGUI_IMPL_GUARD
#pragma GCC error "This file should not be included manually"
#endif

// Has to be included before everything else
#define TX_CONSOLE_MODE SW_SHOW
#include <TXLib.h>

#include <ACL/general.h>
#include <ACL/math/vector.h>
#include <ACL/gui/color.h>
#include <ACL/gui/rect.h>


namespace abel::gui {

DECLARE_ERROR(llgui_error, abel::error)

using abel::math::Vector2i;
using abel::math::Vector2d;

static_assert(sizeof(RGBQUAD) == sizeof(PackedColor));
static_assert(offsetof(RGBQUAD, rgbRed)   == offsetof(PackedColor, R));
static_assert(offsetof(RGBQUAD, rgbGreen) == offsetof(PackedColor, G));
static_assert(offsetof(RGBQUAD, rgbBlue)  == offsetof(PackedColor, B));

#define IMPL_SPECIFIC_

class Window;
class Texture;


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

    inline void render(const Texture &texture) {
        return renderAt(Rect<int>::wh(0, 0, width(), height()), texture);
    }

    void renderAt(const Vector2i &at, const Texture &texture);

    void renderAt(const Rect<int> &at, const Texture &texture);

    void clear(const Color &color = Color::WHITE);

    void update();

    constexpr unsigned width() const noexcept { return width_; }

    constexpr unsigned height() const noexcept { return height_; }

    constexpr Vector2d getSize() const noexcept { return Vector2d{(double)width_, (double)height_}; }

    void setWndProc(WNDPROC wndProc);

    void resetWndProc();

protected:
    static unsigned exists;

    unsigned width_, height_;
    HWND window;

    void destroy() noexcept;

};

class Texture {
public:
    Texture();

    Texture(unsigned width, unsigned height);

    Texture(const Window &wnd);

    Texture(Texture &&other) noexcept;
    Texture &operator=(Texture &&other) noexcept;

    // TODO: Enable?
    Texture(const Texture &other) = delete;
    Texture &operator=(const Texture &other) = delete;

    ~Texture() noexcept;

    inline       PackedColor *getBuf()       { return (      PackedColor *)buf; }
    inline const PackedColor *getBuf() const { return (const PackedColor *)buf; }

    IMPL_SPECIFIC_
    inline HDC getHDC() const { return handle; }

    void resize(const Vector2d &newSize);

    void update();

    void clear(const Color &color = Color::WHITE);

    void embed(const Rect<double> &at, const Texture &other);

    void drawLine(const Vector2d &from, const Vector2d &to, const Color &color = Color::BLACK);

    inline void drawLineAlong(const Vector2d &from, const Vector2d &along, const Color &color = Color::BLACK) {
        drawLine(from, from + along, color);
    }

    void drawLineInf(Vector2d from, Vector2d to, const Color &color = Color::BLACK);

    inline void drawLineInfAlong(const Vector2d &from, const Vector2d &along, const Color &color = Color::BLACK) {
        drawLineInf(from, from + along, color);
    }

    inline void drawCircle(const Vector2d &center, double radius, const Color &color = Color::RED) {
        drawEllipse(center, Vector2d{radius, radius}, color);
    }

    void drawEllipse(const Vector2d &center, const Vector2d &dimensions, const Color &color = Color::RED);

    inline void drawSquare(const Vector2d &center, double side, const Color &color = Color::BLUE) {
        drawRect(Rect<double>::wh(center - Vector2d{side, side}, 2 * Vector2d{side, side}), color);
    }

    void drawRect(const Rect<double> &at, const Color &color = Color::BLUE);

    void drawBounds(const Rect<double> &at, const Color &color = Color::ORANGE);

    void drawText(const Rect<double> &at, const char *text,
                  unsigned format = DT_CENTER | DT_VCENTER | DT_WORDBREAK | DT_WORD_ELLIPSIS,
                  const Color &color = Color::BLACK);

    void setFont(const char *name, double sizeY);

    constexpr unsigned width()  const noexcept { return width_;  }
    constexpr unsigned height() const noexcept { return height_; }

    constexpr Rect<double> getScreenRect() const noexcept { return Rect<double>::wh(0, 0, (double)width(), (double)height()); }
    constexpr Rect<int> getScreenRectInt() const noexcept { return Rect<int>   ::wh(0, 0,    (int)width(),    (int)height()); }

protected:
    unsigned width_, height_;
    HDC handle;
    RGBQUAD *buf;

    void destroy() noexcept;

    void setColor(const Color &color);

};

#undef IMPL_SPECIFIC_

}


#endif // AGF_IMPL_LLGUI_TXLIB_H

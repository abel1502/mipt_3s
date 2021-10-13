#ifndef GUI_GUI_TXLIB_H
#define GUI_GUI_TXLIB_H

#ifndef GUI_GUI_H
#error "Don't include this file manually!"
#endif

// #include <TXLib.h>  // Has to be done in gui.h
#include <ACL/math/cmath.h>


#define DELTA_T deltaT_

#define MAIN_EVENT_LOOP(TICK_CODE, DELAY) {             \
    double DELTA_T = 0.0d;                              \
                                                        \
    /* 5 == txFramesToAverage, but the constant */      \
    /* doesn't exist. */                                \
    for (unsigned i = 0; i < 5; ++i) {                  \
        txGetFPS();                                     \
    }                                                   \
                                                        \
    while (!txGetAsyncKeyState(VK_ESCAPE)) {            \
        DELTA_T = std::fmin(1.d / txGetFPS(), 1.d);     \
                                                        \
        TICK_CODE;                                      \
                                                        \
        txSleep(DELAY);                                 \
    }                                                   \
}


static_assert(sizeof(RGBQUAD) == sizeof(PackedColor));
static_assert(offsetof(RGBQUAD, rgbRed)   == offsetof(PackedColor, R));
static_assert(offsetof(RGBQUAD, rgbGreen) == offsetof(PackedColor, G));
static_assert(offsetof(RGBQUAD, rgbBlue)  == offsetof(PackedColor, B));


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

    void clear();

    void update();

    inline unsigned width() const noexcept { return width_; }

    inline unsigned height() const noexcept { return height_; }

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

    Texture(const Texture &other) = delete;
    Texture &operator=(const Texture &other) = delete;

    ~Texture() noexcept;

    inline PackedColor *getBuf() { return (PackedColor *)buf; }

    inline const PackedColor *getBuf() const  { return (const PackedColor *)buf; }

    inline HDC getHDC() const { return handle; }

    void update();

    void clear();

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


#endif // GUI_GUI_TXLIB_H

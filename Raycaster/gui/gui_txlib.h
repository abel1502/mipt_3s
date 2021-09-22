#ifndef GUI_TXLIB_H
#define GUI_TXLIB_H

#ifndef GUI_H
#error "Don't include this file manually!"
#endif

// #include <TXLib.h>  // Has to be done in gui.h
#include <ACL/math/cmath.h>


#define DELTA_T deltaT_

#define MAIN_EVENT_LOOP(TICK_CODE, DELAY) {             \
    double DELTA_T = 0.0d;                              \
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
        return renderAt(Rect<unsigned>(0, 0, width(), height()), texture);
    }

    void renderAt(const Vector2i &at, const Texture &texture);

    void renderAt(const Rect<unsigned> &at, const Texture &texture);

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

    inline unsigned width() const noexcept { return width_; }

    inline unsigned height() const noexcept { return height_; }

protected:
    unsigned width_, height_;
    HDC handle;
    RGBQUAD *buf;

    void destroy() noexcept;

};


#endif // GUI_TXLIB_H

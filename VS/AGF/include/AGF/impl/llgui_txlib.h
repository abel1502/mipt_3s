#ifndef AGF_IMPL_LLGUI_TXLIB_H
#define AGF_IMPL_LLGUI_TXLIB_H

#ifndef AGF_LLGUI_IMPL_GUARD
#error "This file should not be included manually"
#endif

#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#pragma comment(lib, "Comctl32.lib")
#pragma comment(lib, "UXTheme.lib")
#pragma comment(lib, "Gdiplus.lib")

// Has to be included before everything else
#define TX_CONSOLE_MODE SW_SHOW
// #define WIN32_LEAN_AND_MEAN
#define _WIN32_WINNT _WIN32_WINNT_WIN8

#if defined(UNICODE) || defined(_UNICODE)
#error "This project requires unicode to be disabled"
#endif

#include <Windows.h>
#include <gdiplus.h>
#include <TXLib.h>
#include <Uxtheme.h>

#include <AGF/llgui_pre.h>
#include <AGF/impl/llgui_wintheme.h>


namespace abel::gui {

DECLARE_ERROR(llgui_error, abel::error)

using abel::math::Vector2i;
using abel::math::Vector2d;

static_assert(sizeof(RGBQUAD) == sizeof(PackedColor));
static_assert(offsetof(RGBQUAD, rgbRed)   == offsetof(PackedColor, R));
static_assert(offsetof(RGBQUAD, rgbGreen) == offsetof(PackedColor, G));
static_assert(offsetof(RGBQUAD, rgbBlue)  == offsetof(PackedColor, B));

#define IMPL_SPECIFIC_


namespace _impl {

class TextureBase {
public:
    ~TextureBase() noexcept = default;

    TextureBase(TextureBase &&other) noexcept = default;
    TextureBase &operator=(TextureBase &&other) noexcept = default;

    TextureBase(const TextureBase &other) = delete;
    TextureBase &operator=(const TextureBase &other) = delete;

    constexpr unsigned width()  const noexcept { return width_; }
    constexpr unsigned height() const noexcept { return height_; }
    constexpr Vector2d getSize()    const noexcept { return Vector2d{(double)width_, (double)height_}; }
    constexpr Vector2i getSizeInt() const noexcept { return Vector2i{   (int)width_,    (int)height_}; }
    constexpr Rect<double> getRect()    const noexcept { return Rect<double>::wh(         offset() , getSize   ()); }
    constexpr Rect<int>    getRectInt() const noexcept { return Rect<int>   ::wh(Vector2i{offset()}, getSizeInt()); }


    void setColor(const Color &color);
    void setFont(const char *name, double sizeY);

    #pragma region Color-setting draw functions
    // TODO: Reset color to the previous one?

    inline void clear(const Color &color = Color::WHITE) {
        setColor(color);
        clearRaw();
    }

    inline void drawLine(const Vector2d &from, const Vector2d &to, const Color &color = Color::BLACK) {
        setColor(color);
        drawLineRaw(from, to);
    }

    inline void drawLineAlong(const Vector2d &from, const Vector2d &along, const Color &color = Color::BLACK) {
        drawLine(from, from + along, color);
    }

    inline void drawLineInf(const Vector2d &from, const Vector2d &to, const Color &color = Color::BLACK) {
        setColor(color);
        drawLineInfRaw(from, to);
    }

    inline void drawLineInfAlong(const Vector2d &from, const Vector2d &along, const Color &color = Color::BLACK) {
        drawLineInf(from, from + along, color);
    }

    inline void drawCircle(const Vector2d &center, double radius, const Color &color = Color::RED) {
        drawEllipse(center, Vector2d{radius, radius}, color);
    }

    inline void drawEllipse(const Vector2d &center, const Vector2d &dimensions, const Color &color = Color::RED) {
        setColor(color);
        drawEllipseRaw(center, dimensions);
    }

    inline void drawSquare(const Vector2d &center, double side, const Color &color = Color::BLUE) {
        drawRect(Rect<double>::wh(center - Vector2d{side, side}, 2 * Vector2d{side, side}), color);
    }

    inline void drawRect(const Rect<double> &at, const Color &color = Color::BLUE) {
        setColor(color);
        drawRectRaw(at);
    }

    inline void drawFrame(const Rect<double> &at, const Color &color = Color::ORANGE) {
        setColor(color);
        drawFrameRaw(at);
    }

    static constexpr unsigned DEFAULT_TEXT_FORMAT = DT_CENTER | DT_VCENTER | DT_WORDBREAK | DT_WORD_ELLIPSIS;
    inline void drawText(const Rect<double> &at, const char *text, unsigned format = DEFAULT_TEXT_FORMAT,
                         const Color &color = Color::BLACK) {
        setColor(color);
        drawTextRaw(at, text, format);
    }
    #pragma endregion Color-setting draw functions

    #pragma region Raw draw functions
    void clearRaw();

    void drawLineRaw(Vector2d from, Vector2d to);

    inline void drawLineAlongRaw(const Vector2d &from, const Vector2d &along) {
        drawLineRaw(from, from + along);
    }

    void drawLineInfRaw(Vector2d from, Vector2d to);
    
    inline void drawLineInfAlongRaw(const Vector2d &from, const Vector2d &along) {
        drawLineInfRaw(from, from + along);
    }

    inline void drawCircleRaw(const Vector2d &center, double radius) {
        drawEllipseRaw(center, Vector2d{radius, radius});
    }

    void drawEllipseRaw(Vector2d center, const Vector2d &dimensions);

    inline void drawSquareRaw(const Vector2d &center, double side) {
        drawRectRaw(Rect<double>::wh(center - Vector2d{side, side}, 2 * Vector2d{side, side}));
    }

    void drawRectRaw(Rect<double> at);

    void drawFrameRaw(Rect<double> at);

    void drawTextRaw(Rect<double> at, const char *text, unsigned format = DEFAULT_TEXT_FORMAT);
    #pragma endregion Raw draw functions

    void drawFrameControl(Rect<double> at, unsigned type, unsigned state);

    void drawThemedControl(Rect<double> at, const WinTheme &theme, unsigned type, unsigned state);

    // void drawThemedText;  // TODO!!

    // Only actual textures can be embedded, but into any TextureBase
    inline void embed(const Rect<double> &at, const Texture &other);

    void embedPart(Rect<double> at, const Texture &other, Rect<double> part);

    constexpr const Vector2d &offset() const { return offset_; }
    constexpr       Vector2d &offset()       { return offset_; }

protected:
    unsigned width_  = 0,
             height_ = 0;
    HDC handle = NULL;
    Vector2d offset_{};

    constexpr TextureBase(unsigned width = 0, unsigned height = 0,
                          HDC handle = NULL, const Vector2d &offset = {}) :
        width_{width}, height_{height}, handle{handle}, offset_{offset} {}

};

}


class Window : public _impl::TextureBase {
public:
    enum WinThemeType {
        WT_WINDOW,
        WT_BUTTON,
        WT_TEXTSTYLE,
        WT_REBAR,
        WT_SCROLLBAR,

        WT_COUNT,  ///< Not a valid enum value!
    };

    Window();

    Window(unsigned width, unsigned height);

    Window(const Rect<unsigned> &pos);

    Window(Window &&other) noexcept;

    Window(const Window &other) = delete;
    Window &operator=(const Window &other) = delete;
    Window &operator=(Window &&other) = delete;

    ~Window() noexcept;

    inline void render(const Texture &texture) {
        embed(Rect<double>::wh(0, 0, width(), height()), texture);
    }

    void renderAt(Vector2d at, const Texture &texture);

    inline void renderAt(const Rect<double> &at, const Texture &texture) {
        embed(at, texture);
    }

    void update();

    void setWndProc(WNDPROC wndProc);

    void resetWndProc();

    Vector2d getMousePos();

    void demandRedraw();

    bool isMouseCaptured();
    void captureMouse();
    void releaseMouse();

    static inline Window &getInstance() {
        if (!instance) {
            throw llgui_error("Window absent");
        }

        return *instance;
    }

    template <WinThemeType I>
    constexpr WinTheme &getTheme() noexcept {
        static_assert(I < WT_COUNT);

        return themes[I];
    }

protected:
    friend class Texture;

    static Window *instance;

    HWND window = NULL;
    ULONG_PTR gdiToken = NULL;
    WinTheme themes[WT_COUNT] = {};

};

class Texture : public _impl::TextureBase {
public:
    Texture();

    Texture(unsigned width, unsigned height);

    Texture(const Rect<double> &at);

    Texture(const Window &wnd);

    Texture(const char *srcFileName);

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

protected:
    RGBQUAD *buf;

};


inline void _impl::TextureBase::embed(const Rect<double> &at, const Texture &other) {
    return embedPart(at, other, other.getRect());
}


#undef IMPL_SPECIFIC_

}


#endif // AGF_IMPL_LLGUI_TXLIB_H

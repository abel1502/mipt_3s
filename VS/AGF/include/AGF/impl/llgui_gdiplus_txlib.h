#ifndef ACL_LLGUI_GDIPLUS_TXLIB_H
#define ACL_LLGUI_GDIPLUS_TXLIB_H


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
#define GDIPVER 0x110

#if defined(UNICODE) || defined(_UNICODE)
#error "This project requires unicode to be disabled"
#endif

#include <Windows.h>
#include <gdiplus.h>
#include <TXLib.h>
#include <Uxtheme.h>
#include <algorithm>

#include <AGF/llgui_pre.h>
#include <AGF/impl/llgui_wintheme.h>
#include <ACL/vector.h>


namespace abel::gui {

DECLARE_ERROR(llgui_error, abel::error)


namespace _impl {

class TextureBase {
public:
    constexpr TextureBase           (TextureBase &&other) noexcept = default;
    constexpr TextureBase &operator=(TextureBase &&other) noexcept = default;

    TextureBase           (const TextureBase &other) = delete;
    TextureBase &operator=(const TextureBase &other) = delete;

    ~TextureBase() noexcept = default;

    constexpr unsigned width()  const noexcept { return width_; }
    constexpr unsigned height() const noexcept { return height_; }
    constexpr const Vector2d &offset() const noexcept { return offset_; }
    constexpr       Vector2d &offset()       noexcept { return offset_; }
    constexpr Vector2d getSize()    const noexcept { return Vector2d{(double)width_, (double)height_}; }
    constexpr Vector2i getSizeInt() const noexcept { return Vector2i{   (int)width_,    (int)height_}; }
    constexpr Rect<double> getRect()    const noexcept { return Rect<double>::wh(         offset() , getSize   ()); }
    constexpr Rect<int>    getRectInt() const noexcept { return Rect<int>   ::wh(Vector2i{offset()}, getSizeInt()); }

protected:
    unsigned width_  = 0,
             height_ = 0;
    Vector2d offset_{};

    constexpr TextureBase(unsigned width = 0, unsigned height = 0, const Vector2d &offset = {}) :
        width_{width}, height_{height}, offset_{offset} {}

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
        WT_TRACKBAR,

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
        renderAt(Rect<double>::wh(0, 0, width(), height()), texture);
    }

    void renderAt(Vector2d at, const Texture &texture);

    void renderAt(Rect<double> at, const Texture &texture);

    void update();

    void setWndProc(WNDPROC wndProc);

    void resetWndProc();

    Vector2d getMousePos();

    void demandRedraw();

    bool isMouseCaptured();
    void captureMouse();
    void releaseMouse();

    static inline Window &getInstance() {
        // Window isn't really a singleton - while it stores a pointer to its instance, it doesn't own it

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

    static inline Color HSL2RGB(const abel::math::Vector3d &hsl) {
        COLORREF sysHSL = RGB((unsigned)(hsl.x() * 255.),
                              (unsigned)(hsl.y() * 255.),
                              (unsigned)(hsl.z() * 255.));

        COLORREF sysRGB = txHSL2RGB(sysHSL);

        return Color(((float)GetRValue(sysRGB)) / 255.f,
                     ((float)GetGValue(sysRGB)) / 255.f,
                     ((float)GetBValue(sysRGB)) / 255.f);
    }

    static inline abel::math::Vector3d RGB2HSL(const Color &rgb) {
        COLORREF sysRGB = RGB((unsigned)(rgb.x() * 255.f),
                              (unsigned)(rgb.y() * 255.f),
                              (unsigned)(rgb.z() * 255.f));

        COLORREF sysHSL = txRGB2HSL(sysRGB);

        return abel::math::Vector3d(((double)GetRValue(sysHSL)) / 255.,
                                    ((double)GetGValue(sysHSL)) / 255.,
                                    ((double)GetBValue(sysHSL)) / 255.);
    }

    static inline Color HSV2RGB(const abel::math::Vector3d &hsv) {
        unsigned h_idx = (unsigned)(hsv.x() * 6);
        assert(h_idx <= 6);

        float v     = (float)hsv.z();
        float vMin  = (1.f - (float)hsv.y()) * v;
        float adj   = (float)((v - vMin) * (hsv.x() * 6 - h_idx));
        float vInc  = vMin + adj;
        float vDec  = v - adj;

        switch (h_idx) {
        case 6:
        case 0:
            return Color(v, vInc, vMin);

        case 1:
            return Color(vDec, v, vMin);

        case 2:
            return Color(vMin, v, vInc);

        case 3:
            return Color(vMin, vDec, v);

        case 4:
            return Color(vInc, vMin, v);

        case 5:
            return Color(v, vMin, vDec);

        NODEFAULT
        }
    }

    static inline abel::math::Vector3d RGB2HSV(const Color &rgb) {
        double max = std::max({rgb.r(), rgb.g(), rgb.b()});
        double min = std::min({rgb.r(), rgb.g(), rgb.b()});

        double v = max;
        double s = isZero(max) ?
            0 :
            1 - min / max;

        double h = 0;
        constexpr double PART = 1. / 6;

        if (cmpDbl(max, min) == 0) {
            // Undefined, so we may keep h == 0
        } else if (cmpDbl(max, rgb.r()) == 0) {
            h = PART * (rgb.g() - rgb.b()) / (max - min);

            if (cmpDbl(rgb.g(), rgb.b()) < 0) {
                h += 1;
            }
        } else if (cmpDbl(max, rgb.g()) == 0) {
            h = PART * (rgb.b() - rgb.r()) / (max - min) +
                PART * 2;
        } else {
            h = PART * (rgb.r() - rgb.g()) / (max - min) +
                PART * 4;
        }

        return abel::math::Vector3d{h, s, v};
    }

    void close() noexcept;

protected:
    friend class Texture;

    static Window *instance;

    HDC  handle = NULL;
    HWND window = NULL;
    ULONG_PTR gdiToken = NULL;
    WinTheme themes[WT_COUNT] = {};

};

class Texture : public _impl::TextureBase {
public:
    enum TextFormat {
        TFM_STRICT_FIT = Gdiplus::StringFormatFlagsNoFitBlackBox,
        TFM_ONE_LINE = Gdiplus::StringFormatFlagsNoWrap,
        TFM_NO_VCROP_LAST_LINE = Gdiplus::StringFormatFlagsLineLimit,
        TFM_NO_CLIP = Gdiplus::StringFormatFlagsNoClip,
    };

    enum TextAlign {
        TAL_LEFT   = Gdiplus::StringAlignmentNear,
        TAL_TOP    = TAL_LEFT,
        TAL_CENTER = Gdiplus::StringAlignmentCenter,
        TAL_RIGHT  = Gdiplus::StringAlignmentFar,
        TAL_BOTTOM = TAL_RIGHT,
    };

    enum TextTrimming {
        TTR_NONE          = Gdiplus::StringTrimmingNone,
        TTR_CHAR          = Gdiplus::StringTrimmingCharacter,
        TTR_WORD          = Gdiplus::StringTrimmingWord,
        TTR_CHAR_ELLIPSIS = Gdiplus::StringTrimmingEllipsisCharacter,
        TTR_WORD_ELLIPSIS = Gdiplus::StringTrimmingEllipsisWord,
    };


    Texture() = delete;  // For now, I guess

    explicit Texture(unsigned width, unsigned height);

    explicit inline Texture(const Vector2d &size);

    explicit Texture(const Rect<double> &at);

    explicit Texture(const Window &wnd);

    explicit Texture(const char *srcFileName);

    // Can't have those, Gdiplus stuff has no move ctors(
    Texture(Texture &&other) = delete;
    Texture &operator=(Texture &&other) = delete;

    // TODO: Enable? (probably not this implicitly, though
    Texture(const Texture &other) = delete;
    Texture &operator=(const Texture &other) = delete;

    ~Texture() noexcept;

    void setLineColor(const Color &color, float alpha = 1);
    void setFillColor(const Color &color, float alpha = 1);

    inline Color getLineColor() const;
    inline Color getFillColor() const;

    inline float getLineAlpha() const;
    inline float getFillAlpha() const;

    void setLineWidth(float lineWidth);
    inline float getLineWidth() const;

    inline void setFont(const char *name, double size);
    void setFont(const char *name);
    void setFontSize(double size);
    inline double getFontSize() const;

    void setOverwrite(bool value = true);
    inline void resetOverwrite();

    void clear();

    #pragma region Draw functions
    void drawLine(Vector2d from, Vector2d to);

    inline void drawLineAlong(const Vector2d &from, const Vector2d &along) {
        return drawLine(from, from + along);
    }

    void drawLineInf(Vector2d from, Vector2d to);

    inline void drawLineInfAlong(const Vector2d &from, const Vector2d &along) {
        return drawLineInf(from, from + along);
    }

    inline void drawCircle(const Vector2d &center, double radius, bool fill = true) {
        return drawEllipse(center, Vector2d{radius, radius}, fill);
    }

    void drawEllipse(Vector2d center, Vector2d dimensions, bool fill = true);

    inline void drawSquare(const Vector2d &center, double side, bool fill = true) {
        drawRect(Rect<double>::wh(center - Vector2d{side, side}, 2 * Vector2d{side, side}));
    }

    void drawRect(Rect<double> at, bool fill = true);

    inline void drawFrame(Rect<double> at) {
        return drawRect(at, false);
    }

    void drawText(Rect<double> at, const char *text, TextAlign hAlign = TAL_CENTER, TextAlign vAlign = TAL_CENTER,
                  TextTrimming trimming = TTR_CHAR_ELLIPSIS, unsigned format = 0);

    void drawFrameControl(Rect<double> at, unsigned type, unsigned state);

    void drawThemedControl(Rect<double> at, const WinTheme &theme, unsigned type, unsigned state);

    void drawThemedText(Rect<double> at, const char *text, const WinTheme &theme, unsigned type, unsigned state,
                        TextAlign hAlign = TAL_CENTER, TextAlign vAlign = TAL_CENTER,
                        TextTrimming trimming = TTR_CHAR_ELLIPSIS, unsigned format = 0);
    #pragma endregion Draw functions

    inline void embed(const Rect<double> &at, const Texture &other);

    void embedPart(Rect<double> at, const Texture &other, Rect<double> part);

    inline void embedTransformed(const Rect<double> &at, const Texture &other,
                                 const abel::math::Vector4f &colorCoeffs);

    void embedTransformedPart(Rect<double> at, const Texture &other,
                              const abel::math::Vector4f &colorCoeffs, Rect<double> part);

    /// Remember to call applyBuf to have the changes take effect
    [[nodiscard]] PackedColor *getBuf(bool read = true, bool write = true);

    void flushBuf();

    PackedColor getPixelColor(Vector2d pos) const;

    // void resize(const Vector2d &newSize);

    // void update();

    void clipPush(Rect<double> rect, bool overwrite = false);
    void clipPop();
    Rect<double> getClipRect() const;

protected:
    friend class Window;

    Gdiplus::Bitmap     bitmap;
    Gdiplus::Graphics   graphics;
    Gdiplus::SolidBrush brush{Gdiplus::Color()};
    Gdiplus::Pen        pen{Gdiplus::Color(), 1.f};
    std::wstring        fontName{L"SegoeUI"};
    double              fontSize = 14;

    bool bufRead  = false,
         bufWrite = false;
    Gdiplus::BitmapData bitmapData{};

    vector<Rect<double>> clipStack{};


    Gdiplus::Color getGdiplusLineColor() const;
    Gdiplus::Color getGdiplusFillColor() const;

    static unsigned convertTextFmt(TextAlign hAlign, TextAlign vAlign, TextTrimming trimming, unsigned format);

    void setup(bool shouldClear = true);

    void applyClipStack();

};


inline Texture::Texture(const Vector2d &size) :
    Texture((unsigned)size.x(), (unsigned)size.y()) {}

inline Color Texture::getLineColor() const {
    Gdiplus::Color color = getGdiplusLineColor();

    return Color{color.GetR() / 255.f, color.GetG() / 255.f, color.GetB() / 255.f};
}


inline Color Texture::getFillColor() const {
    Gdiplus::Color color = getGdiplusFillColor();

    return Color{color.GetR() / 255.f, color.GetG() / 255.f, color.GetB() / 255.f};
}

inline float Texture::getLineAlpha() const {
    Gdiplus::Color color = getGdiplusLineColor();

    return color.GetA() / 255.f;
}

inline float Texture::getFillAlpha() const {
    Gdiplus::Color color = getGdiplusFillColor();

    return color.GetA() / 255.f;
}

inline float Texture::getLineWidth() const {
    return pen.GetWidth();
}

inline double Texture::getFontSize() const {
    return fontSize;
}

inline void Texture::setFont(const char *name, double size) {
    setFont(name);
    setFontSize(size);
}

inline void Texture::resetOverwrite() {
    setOverwrite(false);
}

inline void Texture::embed(const Rect<double> &at, const Texture &other) {
    return embedPart(at, other, other.getRect());
}

inline void Texture::embedTransformed(const Rect<double> &at, const Texture &other,
                                      const abel::math::Vector4f &colorCoeffs) {
    return embedTransformedPart(at, other, colorCoeffs, other.getRect());
}



}


#endif // ACL_LLGUI_GDIPLUS_TXLIB_H

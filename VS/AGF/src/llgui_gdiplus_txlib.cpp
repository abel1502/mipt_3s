#include <AGF/llgui.h>


#if AGF_BASE_FRAMEWORK == AGF_BASE_FRAMEWORK_GDIPLUS_TXLIB

#include <utility>
#include <cassert>
#include <cstdio>
#include <string>  // For wstring


namespace abel::gui {


static std::wstring strToWstr(const char *str) {
    errno_t res = 0;

    size_t requiredLength = 0;
    res = mbstowcs_s(&requiredLength, nullptr, 0, str, 0);
    if (res) {
        throw llgui_error("mbstowcs_s failed");
    }

    std::wstring result{};
    result.resize(requiredLength + 1);  // Just in case
    size_t actualLength = 0;
    res = mbstowcs_s(&actualLength, result.data(), requiredLength + 1, str, requiredLength);
    if (res) {
        throw llgui_error("mbstowcs_s failed");
    }
    result.resize(actualLength);

    return result;
}


#pragma region Window
Window *Window::instance = nullptr;


Window::Window() :
    Window(800, 600) {}

Window::Window(unsigned width, unsigned height) :
    Window(Rect<unsigned>::wh(100, 100, width, height)) {}

Window::Window(const Rect<unsigned> &pos) :
    TextureBase(pos.w(), pos.h()) {

    if (instance) {
        throw llgui_error("Multiple windows creation attempted");
    }
    instance = this;

    _txConsole = -1;  // To disable echoing
    txDisableAutoPause();
    Win32::_fpreset();
    txBegin();

    window = txCreateWindow(pos.w(), pos.h());
    if (!window) {
        throw llgui_error("TXLib window creation failed");
    }

    handle = txDC();
    if (!handle) {
        throw llgui_error("TXLib window handle missing");
    }

    INITCOMMONCONTROLSEX ccPrefs{};
    ccPrefs.dwSize = sizeof(ccPrefs);
    ccPrefs.dwICC = ICC_STANDARD_CLASSES | ICC_COOL_CLASSES | ICC_WIN95_CLASSES;
    if (!InitCommonControlsEx(&ccPrefs)) {
        throw llgui_error("InitCommonControlsEx failed");
    }

    themes[WT_WINDOW]    = WinTheme{L"Window"};
    themes[WT_BUTTON]    = WinTheme{L"Button"};
    themes[WT_TEXTSTYLE] = WinTheme{L"TextStyle"};
    themes[WT_REBAR]     = WinTheme{L"Rebar"};
    themes[WT_SCROLLBAR] = WinTheme{L"Scrollbar"};
    themes[WT_TRACKBAR]  = WinTheme{L"Trackbar"};

    Gdiplus::GdiplusStartupInput gdiplusStartupInput{};
    Gdiplus::GdiplusStartup(&gdiToken, &gdiplusStartupInput, NULL);
}

Window::Window(Window &&other) noexcept :
    TextureBase(std::move(other)) {

    std::swap(window, other.window);
    std::swap(handle, other.handle);
}

Window::~Window() noexcept {
    // txDestroyWindow(window);  // Will happen automatically, shouldn't be done manually

    if (window) {  // To avoid destroying temporary objects
        txEnd();

        SetWindowText(txWindow(), "[Closed]");  // We ignore the result, because this is a destructor anyway

        Gdiplus::GdiplusShutdown(gdiToken);
    }

    width_ = 0;
    height_ = 0;
    handle = NULL;
    window = NULL;
    gdiToken = NULL;
    instance = nullptr;
}

void Window::renderAt(Vector2d at, const Texture &texture) {
    at -= offset();

    {
        Gdiplus::Graphics graphics{handle};
        // graphics.SetInterpolationMode(Gdiplus::InterpolationModeNearestNeighbor);

        if (graphics.DrawImage(const_cast<Gdiplus::Bitmap *>(&texture.bitmap),
                               (Gdiplus::REAL)at.x(), (Gdiplus::REAL)at.y()) != Gdiplus::Ok) {
            throw llgui_error("GDI+ DrawImage failed");
        }
    }
}

void Window::renderAt(Rect<double> at, const Texture &texture) {
    at -= offset();

    {
        Gdiplus::Graphics graphics{handle};
        // graphics.SetInterpolationMode(Gdiplus::InterpolationModeNearestNeighbor);

        if (graphics.DrawImage(const_cast<Gdiplus::Bitmap *>(&texture.bitmap),
                               (Gdiplus::REAL)at.x(), (Gdiplus::REAL)at.y(),
                               (Gdiplus::REAL)at.w(), (Gdiplus::REAL)at.h()) != Gdiplus::Ok) {
            throw llgui_error("GDI+ DrawImage failed");
        }
    }
}

void Window::update() {
    txRedrawWindow();
}

void Window::setWndProc(WNDPROC wndProc) {
    txSetWindowsHook(wndProc);
}

void Window::resetWndProc() {
    txSetWindowsHook();
}

Vector2d Window::getMousePos() {
    POINT pos{};
    if (!GetCursorPos(&pos)) {
        throw llgui_error("GetCursorPos failed");
    }

    if (!ScreenToClient(window, &pos)) {
        throw llgui_error("ScreenToClient failed");
    }

    return Vector2d{(double)pos.x, (double)pos.y};
}

void Window::demandRedraw() {
    if (!InvalidateRect(window, nullptr, false)) {  // TODO: Maybe true for erase
        throw llgui_error("InvalidateRect failed");
    }
}

bool Window::isMouseCaptured() {
    return GetCapture() == window;
}

void Window::captureMouse() {
    SetCapture(window);
}

void Window::releaseMouse() {
    if (!ReleaseCapture()) {
        throw llgui_error("ReleaseCapture failed");
    }
}

Vector2d Window::screenToClient(const Vector2d &screen) const {
    POINT sysPoint{
        (LONG)screen.x(),
        (LONG)screen.y(),
    };

    if (!ScreenToClient(window, &sysPoint)) {
        throw llgui_error("ScreenToClient failed");
    }

    return Vector2d{(double)sysPoint.x,
                    (double)sysPoint.y};
}

Vector2d Window::clientToScreen(const Vector2d &client) const {
    POINT sysPoint{
        (LONG)client.x(),
        (LONG)client.y(),
    };

    if (!ClientToScreen(window, &sysPoint)) {
        throw llgui_error("CientToScreen failed");
    }

    return Vector2d{(double)sysPoint.x,
                    (double)sysPoint.y};
}

void Window::close() noexcept {
    if (!DestroyWindow(window)) {
        // Shouldn't throw, as most other destruction functions
        // throw llgui_error("DestroyWindow failed");

        ERR("DestroyWindow faied");
    }
}
#pragma endregion Window


#pragma region Texture
Texture::Texture(unsigned width, unsigned height) :
    TextureBase(width, height),
    bitmap{width, height},
    graphics{&bitmap} {

    setup();
}

Texture::Texture(const Rect<double> &at) :
    TextureBase((unsigned)at.w(), (unsigned)at.h(), at.getPos()),
    bitmap{width(), height()},
    graphics{&bitmap} {

    setup();
}

Texture::Texture(const Window &wnd) :
    Texture(wnd.width(), wnd.height()) {}

Texture::Texture(const char *srcFileName) :
    Texture(strToWstr(srcFileName).data()) {}

Texture::Texture(const wchar_t *srcFileName):
    TextureBase(/* width and height will be set later */),
    bitmap{srcFileName},
    graphics{&bitmap} {

    width_  = bitmap.GetWidth ();
    height_ = bitmap.GetHeight();

    // wprintf(L"%s\n", strToWstr(srcFileName).data());
    // system((std::string("wsl -- file ") + srcFileName).data());

    setup(false);
}

Texture::~Texture() noexcept {
    while (bufRefCnt > 0) {
        flushBuf();
    }

    width_ = 0;
    height_ = 0;

    // Everything else is deleted automatically
}

void Texture::setup(bool shouldClear) {
    assert(width () == bitmap.GetWidth ());
    assert(height() == bitmap.GetHeight());

    if (!Window::instance) {
        throw llgui_error("Window uninitialized yet");
    }

    REQUIRE(bitmap  .GetLastStatus() == Gdiplus::Ok);
    REQUIRE(graphics.GetLastStatus() == Gdiplus::Ok);

    // Should be synchronized with Window, otherwise fonts on textures and on the window will look differently
    // graphics.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);

    // TODO: Experiment - find a way to fix thumb's tip
    // graphics.SetInterpolationMode(Gdiplus::InterpolationModeNearestNeighbor);

    if (shouldClear) {
        setFillColor(Color::WHITE);
        clear();
    }
}

void Texture::setLineColor(const Color &color, float alpha) {
    PackedColor pc = color.pack(alpha);

    if (pen.SetColor(Gdiplus::Color{pc.A, pc.R, pc.G, pc.B}) != Gdiplus::Ok) {
        throw llgui_error("GDI+ SetColor failed");
    }
}

void Texture::setFillColor(const Color &color, float alpha) {
    PackedColor pc = color.pack(alpha);

    if (brush.SetColor(Gdiplus::Color{pc.A, pc.R, pc.G, pc.B}) != Gdiplus::Ok) {
        throw llgui_error("GDI+ SetColor failed");
    }
}

void Texture::setLineWidth(float lineWidth) {
    if (pen.SetWidth(lineWidth) != Gdiplus::Ok) {
        throw llgui_error("GDI+ SetWidth failed");
    }
}

void Texture::setFont(const char *name) {
    fontName = strToWstr(name);
}

void Texture::setFontSize(double size) {
    fontSize = size;
}

void Texture::setOverwrite(bool value) {
    Gdiplus::Status status = graphics.SetCompositingMode(
        value ?
        Gdiplus::CompositingModeSourceCopy :
        Gdiplus::CompositingModeSourceOver);

    if (status != Gdiplus::Ok) {
        throw llgui_error("GDI+ SetCompositingMode failed");
    }
}

Gdiplus::Color Texture::getGdiplusLineColor() const {
    Gdiplus::Color color{};

    if (pen.GetColor(&color) != Gdiplus::Ok) {
        throw llgui_error("GDI+ GetColor failed");
    }

    return color;
}

Gdiplus::Color Texture::getGdiplusFillColor() const {
    Gdiplus::Color color{};

    if (brush.GetColor(&color) != Gdiplus::Ok) {
        throw llgui_error("GDI+ GetColor failed");
    }

    return color;
}

void Texture::clear() {
    graphics.Clear(getGdiplusFillColor());
}

#pragma region Draw Functions
void Texture::drawLine(Vector2d from, Vector2d to) {
    from -= offset();
    to   -= offset();

    if ((from - to).isZero()) {
        // Trivial draw
        return;
    }

    if (graphics.DrawLine(&pen, (float)from.x(), (float)from.y(),
                                (float)to.x(), (float)to.y()) != Gdiplus::Ok) {
        throw llgui_error("GDI+ DrawLine failed");
    }
}

void Texture::drawLineInf(Vector2d from, Vector2d to) {
    // Unnecessary, because it will be subtracted in the underlying call to drawLine
    // from -= offset();
    // to   -= offset();

    if (to == from) {
        throw llgui_error("Can't draw a line through just one point");
    }

    Vector2d delta{to - from};
    delta.normalize();

    // TODO: Point can be outside our texture, should account for that as well
    from -= delta * (width() + height());
    to   += delta * (width() + height());

    drawLine(from, to);
}

#if 0
// A somewhat generalized template for when (if) I need it

/**/ -= offset();

if (fill) {
    if (graphics.Fill/**/(&brush, (float)/**/.x(), (float)/**/.y(),
                                  (float)/**/.x(), (float)/**/.y()) != Gdiplus::Ok) {
        throw llgui_error("GDI+ Fill/**/ failed");
    }
} else {
    if (graphics.Draw/**/(&pen, (float)/**/.x(), (float)/**/.y(),
                                (float)/**/.x(), (float)/**/.y()) != Gdiplus::Ok) {
        throw llgui_error("GDI+ Draw/**/ failed");
    }
}
#endif

void Texture::drawEllipse(Vector2d center, Vector2d dimensions, bool fill) {
    center -= offset();
    center -= dimensions;
    dimensions *= 2;

    if (fill) {
        if (graphics.FillEllipse(&brush, (float)center    .x(), (float)center    .y(),
                                         (float)dimensions.x(), (float)dimensions.y()) != Gdiplus::Ok) {
            throw llgui_error("GDI+ FillEllipse failed");
        }
    } else {
        if (graphics.DrawEllipse(&pen, (float)center    .x(), (float)center    .y(),
                                       (float)dimensions.x(), (float)dimensions.y()) != Gdiplus::Ok) {
            throw llgui_error("GDI+ DrawEllipse failed");
        }
    }

}

void Texture::drawRect(Rect<double> at, bool fill) {
    at -= offset();

    if (fill) {
        if (graphics.FillRectangle(&brush, (float)at.x(), (float)at.y(),
                                           (float)at.w(), (float)at.h()) != Gdiplus::Ok) {
            throw llgui_error("GDI+ FillRectangle failed");
        }
    } else {
        if (graphics.DrawRectangle(&pen, (float)at.x(), (float)at.y(),
                                         (float)at.w(), (float)at.h()) != Gdiplus::Ok) {
            throw llgui_error("GDI+ DrawRectangle failed");
        }
    }
}

void Texture::drawTriangle(Vector2d p1, Vector2d p2, Vector2d p3, bool fill) {
    p1 -= offset();
    p2 -= offset();
    p3 -= offset();

    Gdiplus::PointF points[3] = {
        {(float)p1.x(), (float)p1.y()},
        {(float)p2.x(), (float)p2.y()},
        {(float)p3.x(), (float)p3.y()},
    };

    if (fill) {
        if (graphics.FillPolygon(&brush, points, 3) != Gdiplus::Ok) {
            throw llgui_error("GDI+ FillRectangle failed");
        }
    } else {
        if (graphics.DrawPolygon(&pen, points, 3) != Gdiplus::Ok) {
            throw llgui_error("GDI+ DrawRectangle failed");
        }
    }
}

void Texture::drawText(Rect<double> at, const char *text, TextAlign hAlign, TextAlign vAlign,
                       TextTrimming trimming, unsigned format) {
    at -= offset();

    Gdiplus::Font font{fontName.data(), (float)fontSize, Gdiplus::FontStyleRegular};

    Gdiplus::RectF gdiAt{(float)at.x(), (float)at.y(),
                         (float)at.w(), (float)at.h()};

    Gdiplus::StringFormat gdiFmt{(int)format};
    gdiFmt.SetAlignment    ((Gdiplus::StringAlignment)hAlign);
    gdiFmt.SetLineAlignment((Gdiplus::StringAlignment)vAlign);
    gdiFmt.SetTrimming     ((Gdiplus::StringTrimming)trimming);

    if (graphics.DrawString(strToWstr(text).data(), -1, &font, gdiAt, &gdiFmt, &brush) != Gdiplus::Ok) {
        throw llgui_error("GDI+ DrawString failed");
    }
}

void Texture::drawFrameControl(Rect<double> at, unsigned type, unsigned state) {
    at -= offset();

    RECT rect{(LONG)at.x0(), (LONG)at.y0(),
              (LONG)at.x1(), (LONG)at.y1()};

    HDC hdc = graphics.GetHDC();
    bool success = txGDI(DrawFrameControl(hdc, &rect, type, state), hdc);
    graphics.ReleaseHDC(hdc);

    if (!success) {
        throw llgui_error("DrawFrameControl failed");
    }
}

void Texture::drawThemedControl(Rect<double> at, const WinTheme &theme, unsigned type, unsigned state) {
    at -= offset();

    RECT rect{(LONG)at.x0(), (LONG)at.y0(),
              (LONG)at.x1(), (LONG)at.y1()};

    Rect<double> clipRect = getClipRect() - offset();

    RECT sysClipRect{(LONG)clipRect.x0(), (LONG)clipRect.y0(),
                     (LONG)clipRect.x1(), (LONG)clipRect.y1()};

    HDC hdc = graphics.GetHDC();
    HRESULT result = txGDI(DrawThemeBackground(theme.getHandle(), hdc, type, state, &rect, &sysClipRect), hdc);
    graphics.ReleaseHDC(hdc);

    if (result != S_OK) {
        throw llgui_error("DrawThemeBackground failed");
    }
}

void Texture::drawThemedText(Rect<double> at, const char *text, const WinTheme &theme, unsigned type, unsigned state,
                             TextAlign hAlign, TextAlign vAlign, TextTrimming trimming, unsigned format) {
    at -= offset();

    RECT rect{(LONG)at.x0(), (LONG)at.y0(),
              (LONG)at.x1(), (LONG)at.y1()};

    HDC hdc = graphics.GetHDC();
    HRESULT result = txGDI(DrawThemeText(theme.getHandle(), hdc, type, state,
                                         strToWstr(text).data(), -1,
                                         convertTextFmt(hAlign, vAlign, trimming, format),
                                         0, &rect), hdc);
    graphics.ReleaseHDC(hdc);

    if (result != S_OK) {
        throw llgui_error("DrawThemeText failed");
    }
}
#pragma endregion Draw Functions

unsigned Texture::convertTextFmt(TextAlign hAlign, TextAlign vAlign, TextTrimming trimming, unsigned format) {
    unsigned result = 0;

    switch (hAlign) {
    case TAL_LEFT:
        result |= DT_LEFT;
        break;

    case TAL_CENTER:
        result |= DT_CENTER;
        break;

    case TAL_RIGHT:
        result |= DT_RIGHT;
        break;

    NODEFAULT
    }

    switch (vAlign) {
    case TAL_TOP:
        result |= DT_TOP;
        break;

    case TAL_CENTER:
        result |= DT_VCENTER;
        break;

    case TAL_BOTTOM:
        result |= DT_BOTTOM;
        break;

    NODEFAULT
    }

    switch (trimming) {
    case TTR_CHAR_ELLIPSIS:
    case TTR_WORD_ELLIPSIS:
        result |= DT_END_ELLIPSIS;
        break;

    case TTR_CHAR:
    case TTR_WORD:
        break;

    case TTR_NONE:
        // result |= DT_NOCLIP;  // TODO: ?
        break;

    NODEFAULT
    }

    if (format & TFM_NO_CLIP) {
        result |= DT_NOCLIP;
    }

    if (format & TFM_NO_VCROP_LAST_LINE) {
        result |= DT_EDITCONTROL;  // Seems to achieve this
    }

    if (format & TFM_ONE_LINE) {
        result |= DT_SINGLELINE;
    }

    if (format & TFM_STRICT_FIT) {
        result |= DT_EXTERNALLEADING;  // TODO: ?
    }

    return result;
}

void Texture::embedPart(Rect<double> at, const Texture &other, Rect<double> part) {
    at   -=       offset();
    part -= other.offset();

    Gdiplus::RectF gdiDestRect{(float)at.x(), (float)at.y(),
                               (float)at.w(), (float)at.h()};

    Gdiplus::RectF gdiSrcRect{(float)part.x(), (float)part.y(),
                              (float)part.w(), (float)part.h()};

    Gdiplus::Status status = graphics.DrawImage(const_cast<Gdiplus::Bitmap *>(&other.bitmap),
                                                gdiDestRect, gdiSrcRect, Gdiplus::UnitPixel);

    if (status != Gdiplus::Ok) {
        throw llgui_error("GDI+ DrawImage failed");
    }
}

void Texture::embedTransformedPart(Rect<double> at, const Texture &other,
                                   const abel::math::Vector4f &colorCoeffs, Rect<double> part) {
    // Just to tidy up the matrix expression
    const auto &cc = colorCoeffs;

    abel::math::Vector4f cn{
        (float)(cc.x() < 0),
        (float)(cc.y() < 0),
        (float)(cc.z() < 0),
        (float)(cc.w() < 0),
    };

    Gdiplus::ColorMatrix matrix = {
        cc.x(),    0.f,    0.f,    0.f,    0.f,
           0.f, cc.y(),    0.f,    0.f,    0.f,
           0.f,    0.f, cc.z(),    0.f,    0.f,
           0.f,    0.f,    0.f, cc.w(),    0.f,
        cn.x(), cn.y(), cn.z(), cn.w(),    1.f,
    };

    Gdiplus::ImageAttributes attrs{};
    attrs.SetColorMatrix(&matrix, Gdiplus::ColorMatrixFlagsDefault, Gdiplus::ColorAdjustTypeBitmap);

    at   -=       offset();
    part -= other.offset();

    Gdiplus::RectF gdiDestRect{(float)at.x(), (float)at.y(),
                               (float)at.w(), (float)at.h()};

    Gdiplus::RectF gdiSrcRect{(float)part.x(), (float)part.y(),
                              (float)part.w(), (float)part.h()};

    Gdiplus::Status status = graphics.DrawImage(const_cast<Gdiplus::Bitmap *>(&other.bitmap),
                                                gdiDestRect, gdiSrcRect, Gdiplus::UnitPixel, &attrs);

    if (status != Gdiplus::Ok) {
        throw llgui_error("GDI+ DrawImage failed");
    }
}

static void swapBufBytes(uint32_t *buf, size_t size) {
    for (size_t i = 0; i < size; ++i) {
        std::swap(((uint8_t *)buf)[4 * i + 0],
                  ((uint8_t *)buf)[4 * i + 2]);
    }
}

PackedColor *Texture::getBuf(bool read, bool write, bool rgba) {
    if (!bufRefCnt) {
        Gdiplus::Rect gdiRect{0, 0, (int)width_, (int)height_};

        unsigned flags = 0;
        if (read) {
            flags |= Gdiplus::ImageLockModeRead;
        }
        if (write) {
            flags |= Gdiplus::ImageLockModeWrite;
        }

        bufRead = read;
        bufWrite = write;


        if (bitmap.LockBits(&gdiRect, flags, PixelFormat32bppARGB, &bitmapData) != Gdiplus::Ok) {
            throw llgui_error("GDI+ LockBits failed");
        }

        if (bitmapData.Stride != width() * sizeof(PackedColor)) {
            throw llgui_error("GDI+ Bitmap has extra alignment");
        }

        bufRGBA = rgba;
        if (bufRGBA) {
            swapBufBytes((uint32_t *)bitmapData.Scan0, (size_t)bitmapData.Height * (size_t)bitmapData.Width);
        }
    }

    if (!(read <= bufRead && write <= bufWrite && rgba == bufRGBA)) {
        throw llgui_error("Demanded another buffer with incompatible properties");
    }

    ++bufRefCnt;

    return (PackedColor *)bitmapData.Scan0;
}

uint32_t *Texture::getBufRGBA(bool read, bool write) {
    return (uint32_t *)getBuf(read, write, true);
}

void Texture::flushBuf() {
    REQUIRE(bufRefCnt > 0);

    --bufRefCnt;

    if (bufRefCnt == 0) {
        if (bufRGBA) {
            swapBufBytes((uint32_t *)bitmapData.Scan0, bitmapData.Height * bitmapData.Width);
        }

        bitmap.UnlockBits(&bitmapData);
        bitmapData = {};
    }
}

PackedColor Texture::getPixelColor(Vector2d pos) const {
    pos -= offset();

    Gdiplus::Color gdiColor{};

    Gdiplus::Status status = const_cast<Gdiplus::Bitmap &>(bitmap).GetPixel((int)pos.x(), (int)pos.y(), &gdiColor);

    if (status != Gdiplus::Ok) {
        throw llgui_error("GDI+ GetPixel failed");
    }

    return PackedColor(gdiColor.GetR(),
                       gdiColor.GetG(),
                       gdiColor.GetB(),
                       gdiColor.GetA());
}

void Texture::clipPush(Rect<double> rect, bool overwrite) {
    // TODO: Perhaps offset?

    if (!overwrite) {
        rect.clamp(getClipRect());
    }

    clipStack.append(rect);

    applyClipStack();
}

void Texture::clipPop() {
    if (clipStack.isEmpty()) {
        throw llgui_error("Cannot pop clip from empty stack");
    }

    clipStack.pop();

    applyClipStack();
}

void Texture::applyClipStack() {
    Rect<double> clipRect = getClipRect();
    clipRect -= offset();

    Gdiplus::RectF gdiClipRect{(float)clipRect.x(),
                               (float)clipRect.y(),
                               (float)clipRect.w(),
                               (float)clipRect.h()};

    graphics.SetClip(gdiClipRect);
}

Rect<double> Texture::getClipRect() const {
    if (clipStack.isEmpty()) {
        return getRect();
    }

    return clipStack[-1];
}
#pragma endregion Texture


}


#endif
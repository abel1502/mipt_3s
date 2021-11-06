#include <AGF/llgui.h>


#if AGF_BASE_FRAMEWORK == AGF_BASE_FRAMEWORK_TXLIB


#include <utility>
#include <cassert>
#include <ACL/unique_ptr.h>


namespace abel::gui {


unsigned Window::exists = 0;


Window::Window() :
    Window(800, 600) {}

Window::Window(unsigned width, unsigned height) :
    Window(Rect<unsigned>::wh(100, 100, width, height)) {}

Window::Window(const Rect<unsigned> &pos) :
    width_{pos.w()}, height_{pos.h()} {

    if (exists) {
        throw llgui_error("Multiple windows creation attempted");
    }
    exists++;

    _txConsole = -1;  // To disable echoing
    txDisableAutoPause();
    Win32::_fpreset();
    txBegin();

    window = txCreateWindow(pos.w(), pos.h());
    if (!window) {
        throw llgui_error("TXLib window creation failed");
    }
}

Window::Window(Window &&other) noexcept {
    std::swap(width_, other.width_);
    std::swap(height_, other.height_);
    std::swap(window, other.window);
}

Window::~Window() noexcept {
    destroy();
}

void Window::renderAt(const Vector2i &at, const Texture &texture) {
    if (!txBitBlt(at.x(), at.y(), texture.getHDC())) {
        throw llgui_error("TXLib BitBlt failed");
    }
}

void Window::renderAt(const Rect<int> &at, const Texture &texture) {
    HDC dest = txDC();

    if (!txGDI(Win32::StretchBlt(dest, at.x(), at.y(), at.w(), at.h(),
                                 texture.getHDC(), 0, 0, texture.width(), texture.height(),
                                 SRCCOPY), dest)) {
        throw llgui_error("TXLib StretchBlt failed");
    }
}

void Window::clear(const Color &color) {
    // TODO: Encapsulate?
    PackedColor packedColor = color.pack();
    COLORREF internalColor = RGB(packedColor.R, packedColor.G, packedColor.B);

    if (!txSetFillColor(internalColor) ||
        !txSetColor(internalColor, 1.d)) {
        throw llgui_error("TXLib set color failed");
    }

    if (!txClear()) {
        throw llgui_error("TXLib clear failed");
    }
}

void Window::update() {
    txRedrawWindow();
}

void Window::destroy() noexcept {
    // txDestroyWindow(window);  // Will happen automatically, shouldn't be done manually

    txEnd();

    SetWindowText(txWindow(), "[Closed]");  // We ignore the result, because this is a destructor anyway

    exists--;
}

void Window::setWndProc(WNDPROC wndProc) {
    txSetWindowsHook(wndProc);
}

void Window::resetWndProc() {
    txSetWindowsHook();
}

//================================================================================

Texture::Texture() :
    width_{0}, height_{0}, handle{NULL}, buf{nullptr} {}

Texture::Texture(unsigned width, unsigned height) :
    width_{width}, height_{height} {

    handle = txCreateDIBSection(width, height, &buf);

    if (!handle) {
        throw llgui_error("TXLib texture creation failed");
    }
}

Texture::Texture(const Window &wnd) :
    Texture(wnd.width(), wnd.height()) {}

Texture::Texture(Texture &&other) noexcept :
    width_{other.width_}, height_{other.height_}, handle{other.handle}, buf{other.buf} {

    other.handle = NULL;
    other.buf = nullptr;
}

Texture &Texture::operator=(Texture &&other) noexcept {
    std::swap(width_, other.width_);
    std::swap(height_, other.height_);
    std::swap(handle, other.handle);
    std::swap(buf, other.buf);

    return *this;
}

Texture::~Texture() noexcept {
    destroy();
}

void Texture::resize(const Vector2d &newSize) {
    Texture result{(unsigned)newSize.x(), (unsigned)newSize.y()};

    result.embed(result.getScreenRect(), *this);

    *this = std::move(result);
}

/// Same reasons as Window::update
void Texture::update() {}

void Texture::clear(const Color &color) {
    setColor(color);

    if (!txClear(handle)) {
        throw llgui_error("TXLib failed to clear");
    }
}

void Texture::setColor(const Color &color) {
    PackedColor packedColor = color.pack();
    COLORREF internalColor = RGB(packedColor.R, packedColor.G, packedColor.B);

    if (!txSetFillColor(internalColor, handle) ||
        !txSetColor(internalColor, 1.d, handle)) {
        throw llgui_error("TXLib set color failed");
    }
}

void Texture::embed(const Rect<double> &at, const Texture &other) {
    if (!txGDI(Win32::StretchBlt(handle, (int)at.x(), (int)at.y(), (int)at.w(), (int)at.h(),
                                 other.getHDC(), 0, 0, other.width(), other.height(),
                                 SRCCOPY), handle)) {
        throw llgui_error("TXLib StretchBlt failed");
    }
}

void Texture::drawLine(const Vector2d &from, const Vector2d &to, const Color &color) {
    setColor(color);

    if (!txLine(from.x(), from.y(), to.x(), to.y(), handle)) {
        throw llgui_error("TXLib draw line failed");
    }
}

void Texture::drawLineInf(Vector2d from, Vector2d to, const Color &color) {
    setColor(color);

    if (to == from) {
        throw llgui_error("Can't draw a line through just one point");
    }

    Vector2d delta{to - from};
    delta.normalize();

    from -= delta * (width() + height());
    to   += delta * (width() + height());

    drawLine(from, to);
}

void Texture::drawEllipse(const Vector2d &center, const Vector2d &dimensions, const Color &color) {
    setColor(color);

    if (!txEllipse(center.x() - dimensions.x(), center.y() - dimensions.y(),
                   center.x() + dimensions.x(), center.y() + dimensions.y(), handle)) {
        throw llgui_error("TXLib draw ellipse failed");
    }
}

void Texture::drawRect(const Rect<double> &at, const Color &color) {
    setColor(color);

    if (!txRectangle(at.x0(), at.y0(), at.x1(), at.y1(), handle)) {
        throw llgui_error("TXLib draw rectangle failed");
    }
}

void Texture::drawText(const Rect<double> &at, const char *text, unsigned format, const Color &color) {
    setColor(color);

    if (!txDrawText(at.x0(), at.y0(), at.x1(), at.y1(), text, format, handle)) {
        throw llgui_error("TXLib draw text failed");
    }
}

void Texture::setFont(const char *name, double sizeY) {
    if (!txFontExist(name)) {
        throw llgui_error("TXLib font not found");
    }

    txSelectFont(name, sizeY, sizeY * 0.4, FW_DONTCARE, false, false, false, 0, handle);
}

void Texture::destroy() noexcept {
    if (handle)
        txDeleteDC(handle);  // We don't care about success

    width_ = 0;
    height_ = 0;
    handle = NULL;
    buf = nullptr;
}


}


template
class abel::unique_ptr<abel::gui::Window>;

template
class abel::unique_ptr<abel::gui::Texture>;


#endif // AGF_BASE_FRAMEWORK == AGF_BASE_FRAMEWORK_TXLIB


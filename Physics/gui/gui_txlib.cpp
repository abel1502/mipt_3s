#include "gui.h"

#include <algorithm>
#include <cassert>


unsigned Window::exists = 0;


Window::Window() :
    Window(800, 600) {}

Window::Window(unsigned width, unsigned height) :
    Window(Rect<unsigned>::wh(100, 100, width, height)) {}

Window::Window(const Rect<unsigned> &pos) :
    width_{pos.w()}, height_{pos.h()} {

    if (exists) {
        throw gui_error("Multiple windows creation attempted");
    }
    exists++;

    _txConsole = -1;  // To disable echoing
    txDisableAutoPause();
    Win32::_fpreset();
    txBegin();

    window = txCreateWindow(pos.w(), pos.h());
    if (!window) {
        throw gui_error("TXLib window creation failed");
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
        throw gui_error("TXLib BitBlt failed");
    }
}

void Window::renderAt(const Rect<int> &at, const Texture &texture) {
    HDC dest = txDC();

    if (!txGDI(Win32::StretchBlt(dest, at.x(), at.y(), at.w(), at.h(),
                                 texture.getHDC(), 0, 0, texture.width(), texture.height(),
                                 SRCCOPY), dest)) {
        throw gui_error("TXLib StretchBlt failed");
    }
}

void Window::clear() {
    if (!txClear()) {
        throw gui_error("TXLib clear failed");
    }
}

/// Not noexcept, because see gui_sdl2
void Window::update() {
    txRedrawWindow();
}

void Window::destroy() noexcept {
    // txDestroyWindow(window);  // Will happen automatically, shouldn't be done manually

    txEnd();

    SetWindowText(txWindow(), "[Closed]");  // We ignore the result, because this is a destructor anyway

    exists--;
}

//================================================================================

Texture::Texture() :
    Texture(300, 300) {}

Texture::Texture(unsigned width, unsigned height) :
    width_{width}, height_{height} {

    handle = txCreateDIBSection(width, height, &buf);

    if (!handle) {
        throw gui_error("TXLib texture creation failed");
    }
}

Texture::Texture(const Window &wnd) :
    Texture(wnd.width(), wnd.height()) {}

Texture::Texture(Texture &&other) noexcept {
    std::swap(width_, other.width_);
    std::swap(height_, other.height_);
    std::swap(handle, other.handle);
    std::swap(buf, other.buf);
}

Texture &Texture::operator=(Texture &&other) noexcept {
    destroy();

    std::swap(width_, other.width_);
    std::swap(height_, other.height_);
    std::swap(handle, other.handle);
    std::swap(buf, other.buf);

    return *this;
}

Texture::~Texture() noexcept {
    destroy();
}

/// Same reasons as Window::update
void Texture::update() {}

void Texture::clear() {
    setColor(Color{1, 1, 1});

    if (!txClear(handle)) {
        throw gui_error("TXLib failed to clear");
    }
}

void Texture::setColor(const Color &color) {
    PackedColor packedColor = color.pack();
    COLORREF internalColor = RGB(packedColor.R, packedColor.G, packedColor.B);

    if (!txSetFillColor(internalColor, handle) ||
        !txSetColor(internalColor, 1.d, handle)) {
        throw gui_error("TXLib set color failed");
    }
}

void Texture::drawLine(const Vector2d &from, const Vector2d &to, const Color &color) {
    setColor(color);

    if (!txLine(from.x(), from.y(), to.x(), to.y(), handle)) {
        throw gui_error("TXLib draw line failed");
    }
}

void Texture::drawLineInf(Vector2d from, Vector2d to, const Color &color) {
    setColor(color);

    if (to == from) {
        throw gui_error("Can't draw a line through just one point");
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
        throw gui_error("TXLib draw ellipse failed");
    }
}

void Texture::drawRect(const Rect<double> &at, const Color &color) {
    setColor(color);

    if (!txRectangle(at.x0(), at.y0(), at.x1(), at.y1(), handle)) {
        throw gui_error("TXLib draw rectangle failed");
    }
}

void Texture::destroy() noexcept {
    if (handle)
        txDeleteDC(handle);  // We don't care about success

    width_ = 0;
    height_ = 0;
    handle = NULL;
    buf = nullptr;
}

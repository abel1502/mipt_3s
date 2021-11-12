#include <AGF/llgui.h>


#if AGF_BASE_FRAMEWORK == AGF_BASE_FRAMEWORK_TXLIB


#include <utility>
#include <cassert>
#include <ACL/unique_ptr.h>


namespace abel::gui {


#pragma region TextureBase
namespace _impl {

// TODO: Split into several, and allow to set thickness alone
// It currently sets line and text color (as well as line width) in txSetColor,
// and also the fill color.
void TextureBase::setColor(const Color &color) {
    PackedColor packedColor = color.pack();
    COLORREF internalColor = RGB(packedColor.R, packedColor.G, packedColor.B);

    if (!txSetFillColor(internalColor, handle) ||
        !txSetColor(internalColor, 1., handle)) {
        throw llgui_error("TXLib set color failed");
    }
}

// TODO: More options
void TextureBase::setFont(const char *name, double sizeY) {
    if (!txFontExist(name)) {
        throw llgui_error("TXLib font not found");
    }

    txSelectFont(name, sizeY, sizeY * 0.4, FW_DONTCARE, false, false, false, 0, handle);
}

void TextureBase::embed(const Rect<double> &at, const Texture &other) {
    // TODO: Perhaps use BitBlt where applicable
    if (!txGDI(Win32::StretchBlt(handle, (int)at.x(), (int)at.y(), (int)at.w(), (int)at.h(),
                                 other.handle, 0, 0, other.width(), other.height(),
                                 SRCCOPY), handle)) {
        throw llgui_error("TXLib StretchBlt failed");
    }
}

void TextureBase::clearRaw() {
    if (!txClear(handle)) {
        throw llgui_error("TXLib clear failed");
    }
}

void TextureBase::drawLineRaw(const Vector2d &from, const Vector2d &to) {
    if (!txLine(from.x(), from.y(), to.x(), to.y(), handle)) {
        throw llgui_error("TXLib draw line failed");
    }
}

void TextureBase::drawLineInfRaw(Vector2d from, Vector2d to) {
    if (to == from) {
        throw llgui_error("Can't draw a line through just one point");
    }

    Vector2d delta{to - from};
    delta.normalize();

    from -= delta * (width() + height());
    to   += delta * (width() + height());

    drawLine(from, to);
}

void TextureBase::drawEllipseRaw(const Vector2d &center, const Vector2d &dimensions) {
    if (!txEllipse(center.x() - dimensions.x(), center.y() - dimensions.y(),
                   center.x() + dimensions.x(), center.y() + dimensions.y(), handle)) {
        throw llgui_error("TXLib draw ellipse failed");
    }
}

void TextureBase::drawRectRaw(const Rect<double> &at) {
    if (!txRectangle(at.x0(), at.y0(), at.x1(), at.y1(), handle)) {
        throw llgui_error("TXLib draw rectangle failed");
    }
}

void TextureBase::drawFrameRaw(const Rect<double> &at) {
    RECT rect{(LONG)at.x0(), (LONG)at.y0(),
              (LONG)at.x1(), (LONG)at.y1()};

    HBRUSH brush = (HBRUSH)txGDI(Win32::GetCurrentObject(handle, OBJ_BRUSH), handle);

    if (!brush) {
        throw llgui_error("GetCurrentObject (brush) failed");
    }

    if (!txGDI(FrameRect(handle, &rect, brush), handle)) {
        throw llgui_error("FrameRect failed");
    }

    // drawLine(at.getVertex(0, 0), at.getVertex(0, 1), color);
    // drawLine(at.getVertex(0, 1), at.getVertex(1, 1), color);
    // drawLine(at.getVertex(1, 1), at.getVertex(1, 0), color);
    // drawLine(at.getVertex(1, 0), at.getVertex(0, 0), color);
}

void TextureBase::drawTextRaw(const Rect<double> &at, const char *text, unsigned format) {
    if (!txDrawText(at.x0(), at.y0(), at.x1(), at.y1(), text, format, handle)) {
        throw llgui_error("TXLib draw text failed");
    }
}

}  // namespace _impl
#pragma endregion TextureBase


#pragma region Window
unsigned Window::exists = 0;


Window::Window() :
    Window(800, 600) {}

Window::Window(unsigned width, unsigned height) :
    Window(Rect<unsigned>::wh(100, 100, width, height)) {}

Window::Window(const Rect<unsigned> &pos) :
    TextureBase(pos.w(), pos.h()) {

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

    handle = txDC();
    if (!handle) {
        throw llgui_error("TXLib window handle missing");
    }
}

Window::Window(Window &&other) noexcept :
    TextureBase(std::move(other)) {

    std::swap(window, other.window);
}

Window::~Window() noexcept {
    destroy();
}

void Window::renderAt(const Vector2d &at, const Texture &texture) {
    if (!txBitBlt(at.x(), at.y(), texture.getHDC())) {
        throw llgui_error("TXLib BitBlt failed");
    }
}

void Window::update() {
    txRedrawWindow();
}

void Window::destroy() noexcept {
    // txDestroyWindow(window);  // Will happen automatically, shouldn't be done manually

    txEnd();

    SetWindowText(txWindow(), "[Closed]");  // We ignore the result, because this is a destructor anyway

    width_ = 0;
    height_ = 0;
    handle = NULL;
    window = NULL;
    exists--;  // TODO: Maybe not
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
    if (!ReleaseCapture())
        throw llgui_error("ReleaseCapture failed");
}
#pragma endregion Window


#pragma region Texture
Texture::Texture() :
    TextureBase(), buf{nullptr} {}

Texture::Texture(unsigned width, unsigned height) :
    TextureBase(width, height) {

    REQUIRE(width > 0 && height > 0);
    handle = txCreateDIBSection(width, height, &buf);

    if (!handle) {
        throw llgui_error("TXLib texture creation failed");
    }
}

Texture::Texture(const Window &wnd) :
    Texture(wnd.width(), wnd.height()) {}

Texture::Texture(Texture &&other) noexcept :
    TextureBase(std::move(other)), buf{other.buf} {

    other.handle = NULL;
    other.buf = nullptr;
}

Texture &Texture::operator=(Texture &&other) noexcept {
    TextureBase::operator=(std::move(other));

    std::swap(buf, other.buf);

    return *this;
}

Texture::~Texture() noexcept {
    destroy();
}

void Texture::resize(const Vector2d &newSize) {
    Texture result{(unsigned)newSize.x(), (unsigned)newSize.y()};

    result.embed(result.getRect(), *this);

    *this = std::move(result);
}

/// Has to be present in case buffer implementation demands manual flushing after modification
void Texture::update() {}

void Texture::destroy() noexcept {
    if (handle)
        txDeleteDC(handle);  // We don't care about success

    width_ = 0;
    height_ = 0;
    handle = NULL;
    buf = nullptr;
}
#pragma endregion Texture


}


template
class abel::unique_ptr<abel::gui::Window>;

template
class abel::unique_ptr<abel::gui::Texture>;


#endif // AGF_BASE_FRAMEWORK == AGF_BASE_FRAMEWORK_TXLIB


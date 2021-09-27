#include "gui.h"

#include <algorithm>
#include <cassert>


unsigned Window::exists = 0;


Window::Window() :
    Window(800, 600) {};

Window::Window(unsigned width, unsigned height) :
    Window(Rect<unsigned>(100, 100, width, height)) {}

Window::Window(const Rect<unsigned> &pos) :
    width_{pos.w()}, height_{pos.h()} {

    if (exists) {
        throw gui_error("Multiple windows creation attempted");
    }
    exists++;

    if (SDL_Init(SDL_INIT_EVERYTHING)) {
        throw gui_error("SDL init failed");
    }

    window = SDL_CreateWindow("Abel Raycaster", pos.x(), pos.y(), pos.w(), pos.h(), SDL_WINDOW_SHOWN);

    if (!window) {
        throw gui_error("SDL window creation failed");
    }

    renderer = SDL_CreateRenderer(window, -1, 0);

    if (!renderer) {
        throw gui_error("SDL renderer creation failed");
    }

    clear();
}

Window::Window(Window &&other) noexcept {
    std::swap(width_, other.width_);
    std::swap(height_, other.height_);
    std::swap(window, other.window);
    std::swap(renderer, other.renderer);
}

Window::~Window() noexcept {
    destroy();
}

void Window::renderAtInternal(SDL_Texture *sdlTexture, const SDL_Rect *bounds) {
    if (SDL_RenderCopy(renderer, sdlTexture, nullptr, bounds)) {
        SDL_DestroyTexture(sdlTexture);
        throw gui_error("SDL texture rendering failed");
    }

    SDL_DestroyTexture(sdlTexture);
}

void Window::renderAt(const Vector2i &at, const Texture &texture) {
    SDL_Texture *sdlTexture = texture.createSDLTexture(renderer);
    SDL_Rect bounds{(int)at.x(), (int)at.y(), (int)texture.width(), (int)texture.height()};

    renderAtInternal(sdlTexture, &bounds);
}

void Window::renderAt(const Rect<unsigned> &at, const Texture &texture) {
    SDL_Texture *sdlTexture = texture.createSDLTexture(renderer);
    SDL_Rect bounds = castRect(at);

    renderAtInternal(sdlTexture, &bounds);
}

void Window::clear() {
    if (SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0xff, SDL_ALPHA_OPAQUE) ||
        SDL_RenderClear(renderer)) {

        throw gui_error("SDL Screen clearing failed");
    }
}

/// While this specific implementation cannot throw, an abstract implementation could,
/// so marking this as noexcept could give the user a false sense of safety and
/// make them rely on something they shouldn't
void Window::update() {
    SDL_RenderPresent(renderer);
}

void Window::destroy() noexcept {
    assert(exists);

    if (renderer)
        SDL_DestroyRenderer(renderer);
    if (window)
        SDL_DestroyWindow(window);
    SDL_Quit();

    width_ = 0;
    height_ = 0;
    window = nullptr;
    renderer = nullptr;

    exists--;
}


//================================================================================


Texture::Texture() :
    Texture(300, 300) {}

Texture::Texture(unsigned width, unsigned height) :
    width_{width}, height_{height},
    buf(width * height, PackedColor{0xff, 0xff, 0xff}) {}

Texture::Texture(const Window &wnd) :
    Texture(wnd.width(), wnd.height()) {}

Texture::Texture(Texture &&other) noexcept {
    std::swap(width_, other.width_);
    std::swap(height_, other.height_);
    std::swap(buf, other.buf);
}

Texture &Texture::operator=(Texture &&other) noexcept {
    destroy();

    std::swap(width_, other.width_);
    std::swap(height_, other.height_);
    std::swap(buf, other.buf);

    return *this;
}

Texture::~Texture() noexcept {
    destroy();
}

PackedColor *Texture::getBuf() {
    return buf.getBuf();
}

const PackedColor *Texture::getBuf() const {
    return buf.getBuf();
}

SDL_Texture *Texture::createSDLTexture(SDL_Renderer *renderer) const {
    static_assert(sizeof(PackedColor) == 4);

    SDL_Texture *result = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_XRGB8888, SDL_TEXTUREACCESS_STATIC,
                                            width(), height());

    if (!result || SDL_UpdateTexture(result, nullptr, getBuf(), width() * sizeof(PackedColor))) {
        throw gui_error("SDL texture creation failed");
    }

    return result;
}

/// Same as Window::update - could theoretically throw in an abstract implementation
void Texture::update() {}

void Texture::destroy() noexcept {
    width_ = 0;
    height_ = 0;
    buf.~vector();
}

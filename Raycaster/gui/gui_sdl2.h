#ifndef GUI_SDL2_H
#define GUI_SDL2_H

#ifndef GUI_H
#error "Don't include this file manually!"
#endif

#include <SDL2/SDL.h>
#include <ACL/vector.h>


#define DELTA_T deltaT_

#define MAIN_EVENT_LOOP(TICK_CODE, DELAY) {                     \
    unsigned lastTicks_ = 0, curTicks_ = 0;                     \
    double DELTA_T = 0.0d;                                      \
    SDL_Event event_;                                           \
                                                                \
    lastTicks_ = SDL_GetTicks();                                \
    while (true) {                                              \
        curTicks_ = SDL_GetTicks();                             \
                                                                \
        if (SDL_PollEvent(&event_) && event_.type == SDL_QUIT)  \
            break;                                              \
                                                                \
        DELTA_T = (curTicks_ - lastTicks_) / 1000.d;            \
                                                                \
        TICK_CODE;                                              \
                                                                \
        SDL_Delay(DELAY);                                       \
                                                                \
        lastTicks_ = curTicks_;                                 \
    }                                                           \
}


template <typename T>
SDL_Rect castRect(const Rect<T> &self) {
    return SDL_Rect{(int)self.x(), (int)self.y(),
                    (int)self.w(), (int)self.h()};
}

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
    SDL_Window *window;
    SDL_Renderer *renderer;

    void destroy() noexcept;

    void renderAtInternal(SDL_Texture *sdlTexture, const SDL_Rect *bounds);

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

    PackedColor *getBuf();

    const PackedColor *getBuf() const;

    SDL_Texture *createSDLTexture(SDL_Renderer *renderer) const;

    void update();

    inline unsigned width() const noexcept { return width_; }

    inline unsigned height() const noexcept { return height_; }

protected:
    unsigned width_, height_;
    abel::vector<PackedColor> buf;

    void destroy() noexcept;

};



#endif // GUI_SDL2_H

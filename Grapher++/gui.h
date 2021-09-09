#ifndef PLANE_H
#define PLANE_H

#include <stdexcept>

#include "SDL2/SDL.h"

#include "general.h"
#include "vector2.h"
#include "coords.h"


bool clearScreen(SDL_Renderer *renderer);


class Plot {
public:
    class error : std::runtime_error {
    public:
        error(const char *what) : std::runtime_error(what) {}
    };

    class sdl_error : error {
    public:
        sdl_error() : error("SDL Error") {}
    };

    Plot();

    Plot(const Plot &other);

    Plot &operator=(const Plot &other);

    Plot(SDL_Renderer *new_renderer, const Coords &new_coords);

    /// Converts from the virtual coordinates to the screen ones, with rounding if necessary
    inline void virt2screen(const Vector2 &point, int *x, int *y) const {
        coords.virt2screen(point, x, y);
    }

    /// Converts from the screen coordinates to the virtual ones
    void screen2virt(Vector2 &dest, int x, int y) const {
        coords.screen2virt(dest, x, y);
    }

    bool drawBase() const;

    bool drawGraph(double (*func)(double)) const;

    bool drawVector(const Vector2 &pos, const Vector2 &vec) const;

    bool drawArrow(const Vector2 &pos, const Vector2 &vec) const;

    void update() const;

    inline Coords &getCoords() { return coords; }
    inline const Coords &getCoords() const { return coords; }

protected:
    SDL_Renderer *renderer;
    Coords coords;

    enum class DrawType {
        FG, LEGEND, BG
    };

    bool beginDraw(DrawType type = DrawType::FG) const;

    /// The following private draw functions don't set the brush

    bool drawLine_(const Vector2 &start, const Vector2 &end) const;

    bool drawBall_(const Vector2 &pos, unsigned radius = 2) const;

    bool drawDash_(const Vector2 &pos, bool horizontal, unsigned length) const;

};


#endif // PLANE_H

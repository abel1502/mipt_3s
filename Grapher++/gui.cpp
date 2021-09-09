#include "gui.h"

#include <cassert>
#include <cmath>
#include <cstdio>


#define GUI_COLOR_BG        0xe0, 0xe0, 0xe0
#define PLANE_COLOR_BG      0xff, 0xff, 0xff
#define PLANE_COLOR_FG      0,    0,    0
#define PLANE_COLOR_LEGEND  0x80, 0x80, 0x80


bool clearScreen(SDL_Renderer *renderer) {
    TRY_B(SDL_SetRenderDrawColor(renderer, GUI_COLOR_BG, SDL_ALPHA_OPAQUE));
    TRY_B(SDL_RenderClear(renderer));
    return false;
}


Plot::Plot() :
    renderer{nullptr},
    coords{} {}

Plot::Plot(const Plot &other) :
    renderer{other.renderer},
    coords{other.coords} {}

Plot &Plot::operator=(const Plot &other) {
    renderer = other.renderer;
    coords = other.coords;
}

Plot::Plot(SDL_Renderer *new_renderer, const Coords &new_coords) :
    renderer{new_renderer},
    coords{new_coords} {

    assert(renderer);

    if (beginDraw(DrawType::FG)) {
        throw new sdl_error();
    }
}

bool Plot::drawBase() const {
    assert(renderer);

    TRY_B(beginDraw(DrawType::BG));

    SDL_Rect bgRect = coords.clipRect.toSDLRect();
    TRY_B(SDL_RenderFillRect(renderer, &bgRect));

    TRY_B(beginDraw(DrawType::LEGEND));

    const Rect<int> &clipRect = coords.clipRect;
    TRY_B(SDL_RenderDrawLine(renderer, clipRect.x(), clipRect.y() + coords.centerY(),
                                       clipRect.x() + clipRect.w(), clipRect.y() + coords.centerY()));
    TRY_B(SDL_RenderDrawLine(renderer, clipRect.x() + coords.centerX(), clipRect.y(),
                                       clipRect.x() + coords.centerX(), clipRect.y() + clipRect.h()));

    TRY_B(drawBall_(Vector2::ZERO));
    TRY_B(drawDash_(Vector2::ONE_X, false, 5));
    TRY_B(drawDash_(Vector2::ONE_Y, true, 5));

    return false;
}

static inline bool isFinite(double val) {
    return std::isfinite(val) && std::abs(val) < 1e8;
}

bool Plot::drawGraph(double (*func)(double)) const {
    constexpr unsigned STEPS = 10000;

    assert(renderer && func);

    TRY_B(beginDraw(DrawType::FG));

    double screenWidth  = (double)coords.clipRect.w() / coords.ppuH();
    double padding      = screenWidth * 0.01d;  // To get rid of the gaps on the sides
    double centerOffset = (double)coords.centerX() / coords.ppuH();
    double step         = (screenWidth + 2 * padding) / STEPS;
    double startX       = -centerOffset - padding;

    //DBG("Screen width: %lg", screenWidth);

    unsigned curPointIdx = 1;
    int x = 0, y = 0;

    Vector2 points[2] = {
        {startX, func(startX)},
        {startX + step, 0.0d}
    };

    for (unsigned i = 0; i < STEPS; ++i, curPointIdx = 1 - curPointIdx) {
        virt2screen(points[curPointIdx], &x, &y);

        points[curPointIdx].y = func(points[curPointIdx].x);

        //DBG("Step %u: %lg %lg", i, points[curPointIdx].x, points[curPointIdx].y);

        if (isFinite(points[0].y) && isFinite(points[1].y))
            TRY_B(drawLine_(points[0], points[1]));

        points[1 - curPointIdx].x += 2 * step;
    }

    return false;
}

bool Plot::drawVector(const Vector2 &pos, const Vector2 &vec) const {
    assert(renderer);

    TRY_B(beginDraw(DrawType::FG));

    Vector2 end = pos + vec;

    TRY_B(drawLine_(pos, end));

    TRY_B(drawBall_(end));

    return false;
}

bool Plot::drawArrow(const Vector2 &pos, const Vector2 &vec) const {
    constexpr double ARROW_HEAD_ANGLE = 15.0d;  // Degrees
    constexpr double ARROW_HEAD_SCALE = 0.1d;

    assert(renderer);

    TRY_B(beginDraw(DrawType::FG));

    Vector2 end = pos + vec;

    TRY_B(drawLine_(pos, end));

    Vector2 arrowHead = vec * -ARROW_HEAD_SCALE;

    TRY_B(drawLine_(end, end + arrowHead.rotatedDegrees(-ARROW_HEAD_ANGLE)));
    TRY_B(drawLine_(end, end + arrowHead.rotatedDegrees( ARROW_HEAD_ANGLE)));

    return false;
}

void Plot::update() const {
    SDL_RenderPresent(renderer);
}

bool Plot::beginDraw(DrawType type) const {
    #define CASE_(TYPE, COLOR)                                                  \
        case TYPE:                                                              \
            TRY_B(SDL_SetRenderDrawColor(renderer, COLOR, SDL_ALPHA_OPAQUE));   \
            break;

    switch (type) {
        CASE_(DrawType::FG, PLANE_COLOR_FG)
        CASE_(DrawType::LEGEND, PLANE_COLOR_LEGEND)
        CASE_(DrawType::BG, PLANE_COLOR_BG)
    default:
        assert(false);
        return true;
    }

    #undef CASE_

    SDL_Rect clipRect = coords.clipRect.toSDLRect();
    TRY_B(SDL_RenderSetClipRect(renderer, &clipRect));

    return false;
}

bool Plot::drawLine_(const Vector2 &start, const Vector2 &end) const {
    assert(renderer);

    int x1 = 0, y1 = 0, x2 = 0, y2 = 0;

    virt2screen(start, &x1, &y1);
    virt2screen(end, &x2, &y2);

    TRY_B(SDL_RenderDrawLine(renderer, x1, y1, x2, y2));

    return false;
}

bool Plot::drawBall_(const Vector2 &pos, unsigned radius) const {
    assert(renderer);

    int x = 0, y = 0;
    virt2screen(pos, &x, &y);

    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wnarrowing"
    SDL_Rect centerRect{x - radius, y - radius, 2 * radius + 1, 2 * radius + 1};
    #pragma GCC diagnostic pop

    TRY_B(SDL_RenderFillRect(renderer, &centerRect));

    return false;
}

bool Plot::drawDash_(const Vector2 &pos, bool horizontal, unsigned length) const {
    assert(renderer);

    int x = 0, y = 0;
    virt2screen(pos, &x, &y);

    if (horizontal) {
        TRY_B(SDL_RenderDrawLine(renderer, x - length / 2, y, x + length / 2 + (length & 1), y));
    } else {
        TRY_B(SDL_RenderDrawLine(renderer, x, y - length / 2, x, y + length / 2 + (length & 1)));
    }

    return false;
}

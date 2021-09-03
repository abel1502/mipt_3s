#include "gui.h"

#include <cassert>
#include <cmath>


#define GUI_COLOR_BG        0xe0, 0xe0, 0xe0
#define PLANE_COLOR_BG      0xff, 0xff, 0xff
#define PLANE_COLOR_FG      0,    0,    0
#define PLANE_COLOR_LEGEND  0x80, 0x80, 0x80


bool clearScreen(SDL_Renderer *renderer) {
    TRY_B(SDL_SetRenderDrawColor(renderer, GUI_COLOR_BG, SDL_ALPHA_OPAQUE));
    TRY_B(SDL_RenderClear(renderer));
    return false;
}


bool Plane::ctor(SDL_Renderer *rend) {
    return ctor(rend, 0, 0, 300, 300);
}

bool Plane::ctor(SDL_Renderer *rend, int new_leftX, int new_topY, int new_width, int new_height) {
    return ctor(rend, new_leftX, new_topY, new_width, new_height,
                new_width / 2, new_height / 2,
                ((double)new_width) / 10.0d, ((double)new_height) / 10.0d);
}

bool Plane::ctor(SDL_Renderer *rend, int new_leftX, int new_topY, int new_width, int new_height,
                 int new_centerX, int new_centerY, double new_ppuH, double new_ppuV) {
    assert(rend);

    viewport.x = new_leftX;
    viewport.y = new_topY;
    viewport.w = new_width;
    viewport.h = new_height;
    centerX = new_centerX;
    centerY = new_centerY;
    ppuH = new_ppuH;
    ppuV = new_ppuV;

    renderer = rend;

    TRY_B(SDL_SetRenderDrawColor(renderer, PLANE_COLOR_BG, SDL_ALPHA_OPAQUE));
    TRY_B(SDL_RenderSetViewport(renderer, &viewport));

    return false;
}

void Plane::dtor() {
    renderer = nullptr;
}

bool Plane::shiftBox(int deltaX, int deltaY) {
    viewport.x += deltaX;
    viewport.y += deltaY;

    return SDL_RenderSetViewport(renderer, &viewport);
}

void Plane::scale(double byH, double byV) {
    ppuH /= byH;
    ppuV /= byV;
}

void Plane::pointToPos(const Vector2 *point, int *x, int *y) const {
    assert(point);

    if (x)  *x = point->x * ppuH + centerX + viewport.x;
    if (y)  *y = point->y * ppuV + centerY + viewport.y;
}

void Plane::posToPoint(int x, int y, Vector2 *point) const {
    assert(point);

    point->ctor((double)(x - viewport.x - centerX) / ppuH, (double)(y - viewport.y - centerY) / ppuV);
}

bool Plane::drawBase() const {
    assert(renderer);

    TRY_B(SDL_SetRenderDrawColor(renderer, PLANE_COLOR_BG, SDL_ALPHA_OPAQUE));
    TRY_B(SDL_RenderFillRect(renderer, &viewport));

    TRY_B(SDL_SetRenderDrawColor(renderer, PLANE_COLOR_LEGEND, SDL_ALPHA_OPAQUE));
    TRY_B(SDL_RenderDrawLine(renderer, viewport.x, viewport.y + centerY, viewport.x + viewport.w, viewport.y + centerY));
    TRY_B(SDL_RenderDrawLine(renderer, viewport.x + centerY, viewport.y, viewport.x + centerY, viewport.y + viewport.h));

    TRY_B(drawCircle_(&Vector2::ZERO));
    TRY_B(drawDash_(&Vector2::ONE_X, false, 5));

    Vector2 ONE_Y{};
    ONE_Y.ctor(&Vector2::ONE_Y);
    ONE_Y.imul(-1.0d);
    TRY_B(drawDash_(&ONE_Y, true, 5));

    SDL_RenderPresent(renderer);

    return false;
}

bool Plane::drawGraph(double (*func)(double)) const {
    constexpr unsigned STEPS = 100;

    assert(renderer);

    TRY_B(SDL_SetRenderDrawColor(renderer, PLANE_COLOR_FG, SDL_ALPHA_OPAQUE));

    double screenWidth  = (double)viewport.w / ppuH;
    double centerOffset = (double)(centerX - viewport.x) / ppuH;
    double startX       = -centerOffset - screenWidth;
    double step         = screenWidth / STEPS;

    unsigned curPointIdx = 1;
    int x = 0, y = 0;

    Vector2 points[2];
    Vector2 delta;

    points[0].ctor(startX - step, 0.0d);
    points[1].ctor(startX - step, 0.0d);

    for (unsigned i = 0; i < STEPS; ++i) {
        points[curPointIdx].x += step;

        pointToPos(&points[curPointIdx], &x, &y);

        points[curPointIdx].y = func(points[curPointIdx].x);

        TRY_B(drawLine_(&points[1 - curPointIdx], &points[curPointIdx]));

        curPointIdx = 1 - curPointIdx;
        points[curPointIdx].x += step;
    }

    SDL_RenderPresent(renderer);

    return false;
}

/*bool Plane::drawVector(const BoundVector2 *vec) const;

bool Plane::drawArrow(const BoundVector2 *vec) const;*/

bool Plane::drawLine_(const BoundVector2 *vec) const {
    assert(renderer && vec);

    int x1 = 0, y1 = 0, x2 = 0, y2 = 0;

    Vector2 end;
    vec->getEnd(&end);

    pointToPos(vec->getStart(), &x1, &y1);
    pointToPos(&end, &x2, &y2);

    TRY_B(SDL_RenderDrawLine(renderer, x1, y1, x2, y2));

    return false;
}

bool Plane::drawLine_(const Vector2 *start, const Vector2 *end) const {
    assert(renderer && start && end);

    int x1 = 0, y1 = 0, x2 = 0, y2 = 0;

    pointToPos(start, &x1, &y1);
    pointToPos(end,   &x2, &y2);

    TRY_B(SDL_RenderDrawLine(renderer, x1, y1, x2, y2));

    return false;
}

bool Plane::drawCircle_(const Vector2 *pos, unsigned radius) const {
    assert(renderer);

    int x = 0, y = 0;
    pointToPos(pos, &x, &y);
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wnarrowing"
    SDL_Rect centerRect{x - radius, y - radius, 2 * radius + 1, 2 * radius + 1};
    #pragma GCC diagnostic pop
    TRY_B(SDL_RenderFillRect(renderer, &centerRect));

    return false;
}

bool Plane::drawDash_(const Vector2 *pos, bool horizontal, unsigned length) const {
    assert(renderer);

    int x = 0, y = 0;
    pointToPos(pos, &x, &y);

    if (horizontal) {
        TRY_B(SDL_RenderDrawLine(renderer, x - length / 2, y, x + length / 2 + (length & 1), y));
    } else {
        TRY_B(SDL_RenderDrawLine(renderer, x, y - length / 2, x, y + length / 2 + (length & 1)));
    }

    return false;
}

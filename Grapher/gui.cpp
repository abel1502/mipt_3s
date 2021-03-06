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

    clipRect.x = new_leftX;
    clipRect.y = new_topY;
    clipRect.w = new_width;
    clipRect.h = new_height;
    centerX = new_centerX;
    centerY = new_centerY;
    ppuH = new_ppuH;
    ppuV = new_ppuV;

    renderer = rend;

    TRY_B(SDL_SetRenderDrawColor(renderer, PLANE_COLOR_BG, SDL_ALPHA_OPAQUE));
    TRY_B(SDL_RenderSetClipRect(renderer, &clipRect));

    return false;
}

void Plane::dtor() {
    renderer = nullptr;
}

bool Plane::shiftBox(int deltaX, int deltaY) {
    clipRect.x += deltaX;
    clipRect.y += deltaY;

    return SDL_RenderSetClipRect(renderer, &clipRect);
}

void Plane::scale(double byH, double byV) {
    ppuH /= byH;
    ppuV /= byV;
}

void Plane::pointToPos(const Vector2 *point, int *x, int *y) const {
    assert(point);

    if (x)  *x = point->x * ppuH + centerX + clipRect.x;
    if (y)  *y = -point->y * ppuV + centerY + clipRect.y;
}

void Plane::posToPoint(int x, int y, Vector2 *point) const {
    assert(point);

    point->ctor((double)(x - clipRect.x - centerX) / ppuH, -(double)(y - clipRect.y - centerY) / ppuV);
}

bool Plane::drawBase() const {
    assert(renderer);

    TRY_B(SDL_SetRenderDrawColor(renderer, PLANE_COLOR_BG, SDL_ALPHA_OPAQUE));
    TRY_B(SDL_RenderSetClipRect(renderer, &clipRect));

    TRY_B(SDL_RenderFillRect(renderer, &clipRect));

    TRY_B(SDL_SetRenderDrawColor(renderer, PLANE_COLOR_LEGEND, SDL_ALPHA_OPAQUE));
    TRY_B(SDL_RenderDrawLine(renderer, clipRect.x, clipRect.y + centerY, clipRect.x + clipRect.w, clipRect.y + centerY));
    TRY_B(SDL_RenderDrawLine(renderer, clipRect.x + centerX, clipRect.y, clipRect.x + centerX, clipRect.y + clipRect.h));

    TRY_B(drawBall_(&Vector2::ZERO));
    TRY_B(drawDash_(&Vector2::ONE_X, false, 5));
    TRY_B(drawDash_(&Vector2::ONE_Y, true, 5));

    return false;
}

static inline bool isFinite(double val) {
    return std::isfinite(val) && std::abs(val) < 1e8;
}

bool Plane::drawGraph(double (*func)(double)) const {
    constexpr unsigned STEPS = 10000;

    assert(renderer && func);

    TRY_B(SDL_SetRenderDrawColor(renderer, PLANE_COLOR_FG, SDL_ALPHA_OPAQUE));
    TRY_B(SDL_RenderSetClipRect(renderer, &clipRect));

    double screenWidth  = (double)clipRect.w / ppuH;
    double padding      = screenWidth * 0.01d;  // To get rid of the gaps on the sides
    double centerOffset = (double)centerX / ppuH;
    double step         = (screenWidth + 2 * padding) / STEPS;
    double startX       = -centerOffset - padding;

    //DBG("Screen width: %lg", screenWidth);

    unsigned curPointIdx = 1;
    int x = 0, y = 0;

    Vector2 points[2];
    Vector2 delta{};

    points[0].ctor(startX, func(startX));
    points[1].ctor(startX + step, 0.0d);

    //points[0].ctor(startX, 1.0d);
    //points[1].ctor(startX + screenWidth * 2, 1.0d);
    //TRY_B(drawLine_(&points[0], &points[1]));

    for (unsigned i = 0; i < STEPS; ++i, curPointIdx = 1 - curPointIdx) {
        pointToPos(&points[curPointIdx], &x, &y);

        points[curPointIdx].y = func(points[curPointIdx].x);

        //DBG("Step %u: %lg %lg", i, points[curPointIdx].x, points[curPointIdx].y);

        if (isFinite(points[0].y) && isFinite(points[1].y))
            TRY_B(drawLine_(&points[0], &points[1]));

        points[1 - curPointIdx].x += 2 * step;
    }

    return false;
}

bool Plane::drawVector(const BoundVector2 *vec) const {
    assert(renderer && vec);

    TRY_B(SDL_SetRenderDrawColor(renderer, PLANE_COLOR_FG, SDL_ALPHA_OPAQUE));
    TRY_B(SDL_RenderSetClipRect(renderer, &clipRect));

    TRY_B(drawLine_(vec));

    Vector2 end{};
    vec->getEnd(&end);

    TRY_B(drawBall_(&end));

    return false;
}

bool Plane::drawArrow(const BoundVector2 *vec) const {
    constexpr double ARROW_HEAD_ANGLE = 15.0d;  // Degrees
    constexpr double ARROW_HEAD_SCALE = 0.1d;

    assert(renderer && vec);

    TRY_B(SDL_SetRenderDrawColor(renderer, PLANE_COLOR_FG, SDL_ALPHA_OPAQUE));
    TRY_B(SDL_RenderSetClipRect(renderer, &clipRect));

    TRY_B(drawLine_(vec));

    Vector2 end{};
    vec->getEnd(&end);

    Vector2 arrowHead{};
    arrowHead.ctor(vec->getVec());
    arrowHead.scaleBy(-ARROW_HEAD_SCALE);

    BoundVector2 bndArrowHead{};
    bndArrowHead.ctor(&end, &arrowHead);

    arrowHead.rotateDegrees(-ARROW_HEAD_ANGLE);
    TRY_B(drawLine_(&bndArrowHead));

    arrowHead.rotateDegrees(2 * ARROW_HEAD_ANGLE);
    TRY_B(drawLine_(&bndArrowHead));

    return false;
}

void Plane::update() const {
    SDL_RenderPresent(renderer);
}

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

bool Plane::drawBall_(const Vector2 *pos, unsigned radius) const {
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

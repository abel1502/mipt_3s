#include "SDL2/SDL.h"
#include <cassert>

#include "general.h"
#include "gui.h"


constexpr unsigned WND_WIDTH = 800;
constexpr unsigned WND_HEIGHT = 600;


double parabole(double x) {
    return x * x;
}

double hyperbole(double x) {
    return 1 / x;
}


bool drawSampleGraph(Plane *plane, double (*func)(double));
bool drawSampleVectors(Plane *plane, const BoundVector2 *normalVec, const BoundVector2 *fancyVec);


// SDL doesn't compile without these arguments or with const char **...
int main(int, char **) {
    constexpr double ARROW_ROTATION_SPEED = 360.d / 6.d;  // Degrees per second

    verbosity = 2;
    SDL_Window *wnd = nullptr;
    SDL_Renderer *rend = nullptr;
    Plane planeGraph1{};
    Plane planeGraph2{};
    Plane planeVectors{};

    SDL_Event event{};

    Vector2 normalArrowBase{}, normalArrow{};
    Vector2 fancyArrowBase{}, fancyArrow{};
    BoundVector2 boundNormalArrow{}, boundFancyArrow{};

    unsigned lastTicks = 0, curTicks = 0;
    double deltaT = 0.0d;

    #define T_(STMT)   if (STMT)  goto error;

    T_(SDL_Init(SDL_INIT_EVERYTHING));

    T_(!(wnd = SDL_CreateWindow("Abel Grapher", 100, 100, WND_WIDTH, WND_HEIGHT, SDL_WINDOW_SHOWN)));

    T_(!(rend = SDL_CreateRenderer(wnd, -1, 0)));
    T_(clearScreen(rend));

    T_(planeGraph1.ctor(rend, 10, 10, 200, 200, 100, 100, 10, 25));
    T_(planeGraph2.ctor(rend, 310, 10, 300, 150, 40, 110, 20, 20));
    T_(planeVectors.ctor(rend, 10, 220, 250, 250));

    normalArrowBase.ctor(-0.5d, 2.d);
    normalArrow.ctor(2.d, -3.d);
    boundNormalArrow.ctor(&normalArrowBase, &normalArrow);

    fancyArrowBase.ctor(1.5d, -1.d);
    fancyArrow.ctor(-2.d, 3.d);
    boundFancyArrow.ctor(&fancyArrowBase, &fancyArrow);

    T_(drawSampleGraph(&planeGraph1, &parabole));
    planeGraph1.update();

    T_(drawSampleGraph(&planeGraph2, &parabole));
    planeGraph1.update();

    lastTicks = SDL_GetTicks();

    while (true) {
        curTicks = SDL_GetTicks();

        if (SDL_PollEvent(&event) && event.type == SDL_QUIT)
            break;

        deltaT = (curTicks - lastTicks) / 1000.d;
        fancyArrow.rotateDegrees(ARROW_ROTATION_SPEED * deltaT);
        // fancyArrow.scaleBy(std::pow(0.95d, deltaT));

        T_(drawSampleVectors(&planeVectors, &boundNormalArrow, &boundFancyArrow));
        planeVectors.update();

        SDL_Delay(10);

        lastTicks = curTicks;
    }

    SDL_Quit();

    return 0;

error:
    SDL_Log("SDL error: %s", SDL_GetError());

    ERR("An error has occured, shutting down abruptly");

    planeGraph1.dtor();
    planeGraph2.dtor();
    planeVectors.dtor();
    if (rend)
        SDL_DestroyRenderer(rend);
    if (wnd)
        SDL_DestroyWindow(wnd);

    SDL_Quit();

    return 1;

    #undef T_
}


bool drawSampleGraph(Plane *plane, double (*func)(double)) {
    assert(plane && func);

    TRY_B(plane->drawBase());

    TRY_B(plane->drawGraph(func));

    return false;
}


bool drawSampleVectors(Plane *plane, const BoundVector2 *normalVec, const BoundVector2 *fancyVec) {
    assert(plane && normalVec && fancyVec);

    TRY_B(plane->drawBase());

    TRY_B(plane->drawVector(normalVec));
    TRY_B(plane->drawArrow(fancyVec));

    return false;
}


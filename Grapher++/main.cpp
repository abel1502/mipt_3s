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


bool drawSampleGraph(Plot &plane, double (*func)(double));
bool drawSampleVectors(Plot &plane, const Vector2 &normalBase, const Vector2 &normalVec, const Vector2 &fancyBase, const Vector2 &fancyVec);


// SDL doesn't compile without these arguments or with const char **...
int main(int, char **) {
    constexpr double ARROW_ROTATION_SPEED = 360.d / 6.d;  // Degrees per second

    verbosity = 2;
    SDL_Window *wnd = nullptr;
    SDL_Renderer *rend = nullptr;
    Plot planeGraph1{};
    Plot planeGraph2{};
    Plot planeVectors{};

    SDL_Event event{};

    Vector2 normalArrowBase{-0.5d, 2.d}, normalArrow{2.d, -3.d};
    Vector2 fancyArrowBase{1.5d, -1.d}, fancyArrow{-2.d, 3.d};

    unsigned lastTicks = 0, curTicks = 0;
    double deltaT = 0.0d;

    #define T_(STMT)   if (STMT)  goto error;

    T_(SDL_Init(SDL_INIT_EVERYTHING));

    T_(!(wnd = SDL_CreateWindow("Abel Grapher", 100, 100, WND_WIDTH, WND_HEIGHT, SDL_WINDOW_SHOWN)));

    T_(!(rend = SDL_CreateRenderer(wnd, -1, 0)));
    T_(clearScreen(rend));

    planeGraph1 = Plot(rend, Coords(Rect<int>(10, 10, 200, 200), 100, 100, 10, 25));
    planeGraph2 = Plot(rend, Coords(Rect<int>(310, 10, 300, 150), 40, 110, 20, 20));
    planeVectors = Plot(rend, Coords(Rect<int>(10, 220, 250, 250)));

    T_(drawSampleGraph(planeGraph1, &parabole));
    planeGraph1.update();

    T_(drawSampleGraph(planeGraph2, &parabole));
    planeGraph1.update();

    lastTicks = SDL_GetTicks();

    while (true) {
        curTicks = SDL_GetTicks();

        if (SDL_PollEvent(&event) && event.type == SDL_QUIT)
            break;

        deltaT = (curTicks - lastTicks) / 1000.d;
        fancyArrow.rotateDegrees(ARROW_ROTATION_SPEED * deltaT);
        // fancyArrow.scaleBy(std::pow(0.95d, deltaT));

        T_(drawSampleVectors(planeVectors, normalArrowBase, normalArrow, fancyArrowBase, fancyArrow));
        planeVectors.update();

        SDL_Delay(10);

        lastTicks = curTicks;
    }

    SDL_Quit();

    return 0;

error:
    SDL_Log("SDL error: %s", SDL_GetError());

    ERR("An error has occured, shutting down abruptly");

    if (rend)
        SDL_DestroyRenderer(rend);
    if (wnd)
        SDL_DestroyWindow(wnd);

    SDL_Quit();

    return 1;

    #undef T_
}


bool drawSampleGraph(Plot &plane, double (*func)(double)) {
    TRY_B(plane.drawBase());

    TRY_B(plane.drawGraph(func));

    return false;
}


bool drawSampleVectors(Plot &plane, const Vector2 &normalBase, const Vector2 &normalVec, const Vector2 &fancyBase, const Vector2 &fancyVec) {
    TRY_B(plane.drawBase());

    TRY_B(plane.drawVector(normalBase, normalVec));
    TRY_B(plane.drawArrow(fancyBase, fancyVec));

    return false;
}


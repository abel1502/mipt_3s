#include "SDL2/SDL.h"

#include "general.h"
#include "gui.h"


constexpr unsigned WND_WIDTH = 800;
constexpr unsigned WND_HEIGHT = 600;


double parabole(double x) {
    return x * x;
}


// SDL doesn't compile without these arguments or with const char **...
int main(int, char **) {
    verbosity = 0;
    SDL_Window *wnd = nullptr;
    SDL_Renderer *rend = nullptr;
    Plane plane{};
    SDL_Event event{};

    #define T_(STMT)   if (STMT)  goto error;


    T_(SDL_Init(SDL_INIT_EVERYTHING));

    T_(!(wnd = SDL_CreateWindow("Abel Grapher", 100, 100, WND_WIDTH, WND_HEIGHT, SDL_WINDOW_SHOWN)));

    T_(!(rend = SDL_CreateRenderer(wnd, -1, 0)));
    T_(clearScreen(rend));

    T_(plane.ctor(rend));
    T_(plane.drawBase());
    T_(plane.drawGraph(&parabole));

    while (true) {
        T_(!SDL_WaitEvent(&event));

        if (event.type == SDL_QUIT)
            break;
    }

    SDL_Quit();

    return 0;

error:
    SDL_Log("SDL error: %s", SDL_GetError());

    ERR("An error has occured, shutting down abruptly");

    plane.dtor();
    if (rend)
        SDL_DestroyRenderer(rend);
    if (wnd)
        SDL_DestroyWindow(wnd);

    SDL_Quit();

    return 1;

    #undef T_
}



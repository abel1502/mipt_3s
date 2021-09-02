#include <TXLib.h>

#include "general.h"


constexpr unsigned WND_WIDTH = 800;
constexpr unsigned WND_HEIGHT = 600;


double parabole(double x) {
    return x * x;
}


int main() {
    verbosity = 0;

    txCreateWindow(WND_WIDTH, WND_HEIGHT);
    txBegin();

    return 0;
}



#pragma once


#include "types_std.h"
#include "keyboard_std.h"


namespace plugin::Event {

struct Render {
    bool force = false;
};

struct Tick {
    double dt;
};


struct MousePress {
    Vec2f position;
    MouseButton button = MouseButton::none;
};

struct MouseRelease {
    Vec2f position;
    MouseButton button = MouseButton::none;
};

struct MouseMove {
    Vec2f from;
    Vec2f to;
};

struct Click {

};

struct Hide {

};

struct Show {

};

struct FractionChanged {
    Vec2f fraction;
};


struct Scroll {
    Vec2f delta;
    Vec2f position;
};

}


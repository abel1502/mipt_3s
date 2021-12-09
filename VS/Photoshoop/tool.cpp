#include <AGF/llgui.h>
#include "tool.h"


Tool::Tool(ToolManager *manager_) :
    manager(manager_) {}


void Tool::applyLine(Layer &layer, const Vector2d &pos0, const Vector2d &pos1) {
    constexpr unsigned NSTEPS = 100;

    Vector2d cur = pos0;
    const Vector2d step = (pos1 - pos0) / NSTEPS;

    for (unsigned i = 0; i < NSTEPS; ++i, cur += step) {
        applyPoint(layer, cur);
    }
}


Effect::Effect(ToolManager *manager_) :
    manager{manager_} {}

#pragma once

#include <ACL/general.h>
#include "tool_manager.h"
#include "layer.h"


using abel::gui::Rect;
using abel::gui::Color;
using abel::math::Vector2d;


class ToolManager;

class Tool {
public:
    Tool(ToolManager *manager_);

    // TODO: Perhaps not const?
    virtual void applyLine(Layer &layer, const Vector2d &pos0, const Vector2d &pos1);

    virtual void applyPoint(Layer &layer, const Vector2d &pos) = 0;

    inline const ToolManager &getManager() const { return *manager; }
    inline       ToolManager &getManager()       { return *manager; }

protected:
    ToolManager *manager;

};
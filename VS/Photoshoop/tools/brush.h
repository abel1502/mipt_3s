#pragma once

#include "../tool.h"


class BrushTool : public Tool {
public:
    inline BrushTool(ToolManager *manager_) :
        Tool(manager_) {}

    virtual void applyLine(Layer &layer, const Vector2d &pos0, const Vector2d &pos1) override;

    virtual void applyPoint(Layer &layer, const Vector2d &pos) override;

protected:
    double configureTarget(abel::gui::Texture &target) const;

};
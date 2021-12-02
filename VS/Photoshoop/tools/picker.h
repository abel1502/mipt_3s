#pragma once

#include "../tool.h"


class PickerTool : public Tool {
public:
    inline PickerTool(ToolManager *manager_) :
        Tool(manager_) {}

    virtual void applyLine(Layer &layer, const Vector2d &pos0, const Vector2d &pos1)  override;

    virtual void applyPoint(Layer &layer, const Vector2d &pos) override;

protected:
    //

};
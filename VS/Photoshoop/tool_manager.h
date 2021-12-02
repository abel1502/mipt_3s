#pragma once

#include <ACL/general.h>
#include <ACL/vector.h>
#include <ACL/unique_ptr.h>
#include <ACL/signal.h>
#include <AGF/widget.h>
#include <AGF/widgets/all.h>
#include "tool.h"


using abel::gui::Rect;
using abel::gui::Color;
using abel::math::Vector2d;


class Tool;
class BrushTool;
class EraserTool;
class PickerTool;

class ToolManager {
public:
    enum BasicToolType {
        BTT_BRUSH,
        BTT_ERASER,
        BTT_COLOR_PICKER,
    };


    abel::Signal<bool (ToolManager &)> sigConfigChanged{};


    ToolManager(double radius_ = 2.5, const Color &color_ = {}, double alpha_ = 1);

    inline const Tool &getActiveTool() const { return *tools[activeToolIdx]; }
    inline       Tool &getActiveTool()       { return *tools[activeToolIdx]; }

    Tool &selectTool(unsigned idx);

    Tool &selectBasicTool(BasicToolType type);

    template <BasicToolType type>
    decltype(auto) selectBasicTool() {
        Tool &result = selectBasicTool(type);

        if constexpr (type == BTT_BRUSH) {
            return dynamic_cast<BrushTool &>(result);
        }

        if constexpr (type == BTT_ERASER) {
            return dynamic_cast<EraserTool &>(result);
        }

        if constexpr (type == BTT_COLOR_PICKER) {
            return dynamic_cast<PickerTool &>(result);
        }

        // Shouldn't be reachable
        assert(false);

        // But, just in case
        return result;
    }

    constexpr double getRadius() const {
        return radius;
    }

    template <bool Silent = false>
    constexpr void setRadius(double radius_) {
        radius = radius_;

        if constexpr (!Silent) {
            syncBack();
        }
    }

    constexpr const Color &getColor() const {
        return color;
    }

    template <bool Silent = false>
    constexpr void setColor(const Color &color_) {
        color = color_;

        if constexpr (!Silent) {
            syncBack();
        }
    }

    constexpr double getAlpha() const {
        return alpha;
    }

    template <bool Silent = false>
    constexpr void setAlpha(double alpha_) {
        alpha = alpha_;

        if constexpr (!Silent) {
            syncBack();
        }
    }

protected:
    abel::vector<abel::unique_ptr<Tool>> tools;
    unsigned activeToolIdx;
    double radius = 2.5;
    Color color{};
    double alpha{};
    mutable bool isBeingSet = false;


    inline void syncBack() {
        if (isBeingSet)
            return;

        isBeingSet = true;
        sigConfigChanged(*this);
        isBeingSet = false;
    }

};


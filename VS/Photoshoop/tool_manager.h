#pragma once

#include <ACL/general.h>
#include <ACL/vector.h>
#include <ACL/unique_ptr.h>
#include <ACL/signal.h>
#include <AGF/widget.h>
#include <AGF/widgets/all.h>
#include <AGF/helpers/widget_ref.h>
#include "tool.h"
#include "canvas.h"


using abel::gui::Rect;
using abel::gui::Color;
using abel::math::Vector2d;


class Tool;
class BrushTool;
class EraserTool;
class PickerTool;
class Effect;


class ToolManager {
public:
    enum BasicToolType {
        BTT_BRUSH,
        BTT_ERASER,
        BTT_COLOR_PICKER,
    };

    using tool_handle_t = unsigned;
    using effect_handle_t = unsigned;


    abel::Signal<bool (ToolManager &)> sigConfigChanged{};


    ToolManager(double radius_ = 2.5, const Color &color_ = {}, double alpha_ = 1);

    inline const Tool &getTool(tool_handle_t handle) const { return *tools[handle]; }
    inline       Tool &getTool(tool_handle_t handle)       { return *tools[handle]; }

    inline const Effect &getEffect(effect_handle_t handle) const { return *effects[handle]; }
    inline       Effect &getEffect(effect_handle_t handle)       { return *effects[handle]; }

    inline const Tool &getActiveTool() const { return getTool(activeToolIdx); }
    inline       Tool &getActiveTool()       { return getTool(activeToolIdx); }

    inline tool_handle_t getActiveToolHandle() const { return activeToolIdx; }

    tool_handle_t addTool(Tool *tool);

    effect_handle_t addEffect(Effect *effect);

    Tool &selectTool(tool_handle_t handle);

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

    inline bool isBasicToolActive(BasicToolType type);

    constexpr tool_handle_t getBasicToolHandle(BasicToolType type) {
        // A trivial conversion in this case, but doesn't have to be
        return (tool_handle_t)type;
    }

    inline void resetActiveCanvas() {
        setActiveCanvas(nullptr);
    }

    inline void setActiveCanvas(Canvas *canvas) {
        activeCanvas = canvas;
    }

    inline Canvas *getActiveCanvas() {
        return activeCanvas.get();
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
    abel::vector<abel::unique_ptr<Effect>> effects;
    unsigned activeToolIdx;
    abel::gui::WidgetRefTo<Canvas> activeCanvas = nullptr;
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


inline bool ToolManager::isBasicToolActive(BasicToolType type) {
    return activeToolIdx == getBasicToolHandle(type);
}


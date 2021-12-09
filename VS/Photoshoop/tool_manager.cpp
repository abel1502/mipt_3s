#include <AGF/llgui.h>
#include "tool_manager.h"
#include "tools/brush.h"
#include "tools/eraser.h"
#include "tools/picker.h"


ToolManager::ToolManager(double radius_, const Color &color_, double alpha_) :
    tools{}, activeToolIdx{0}, radius{radius_}, color{color_}, alpha{alpha_} {

    tools.appendEmplace(new BrushTool(this));
    tools.appendEmplace(new EraserTool(this));
    tools.appendEmplace(new PickerTool(this));
}

Tool &ToolManager::selectTool(tool_handle_t idx) {
    REQUIRE(idx < tools.getSize());

    activeToolIdx = idx;

    syncBack();

    return getActiveTool();
}

Tool &ToolManager::selectBasicTool(BasicToolType type) {
    return selectTool(getBasicToolHandle(type));
}

ToolManager::tool_handle_t ToolManager::addTool(Tool *tool) {
    tools.appendEmplace(tool);

    return (tool_handle_t)(tools.getSize() - 1);
}

ToolManager::effect_handle_t ToolManager::addEffect(Effect *effect) {
    effects.appendEmplace(effect);

    return (effect_handle_t)(effects.getSize() - 1);
}


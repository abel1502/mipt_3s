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

Tool &ToolManager::selectTool(unsigned idx) {
    REQUIRE(idx < tools.getSize());

    activeToolIdx = idx;

    syncBack();

    return getActiveTool();
}

Tool &ToolManager::selectBasicTool(BasicToolType type) {
    unsigned idx = -1u;

    switch (type) {
    case BTT_BRUSH:
        idx = 0;
        break;

    case BTT_ERASER:
        idx = 1;
        break;

    case BTT_COLOR_PICKER:
        idx = 2;
        break;

    NODEFAULT
    }

    return selectTool(idx);
}

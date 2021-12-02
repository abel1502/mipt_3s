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
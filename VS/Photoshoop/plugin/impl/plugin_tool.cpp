#include <AGF/llgui.h>
#include "plugin_tool.h"
#include "../loader.h"


void PluginTool::applyLine(Layer &layer, const Vector2d &pos0, const Vector2d &pos1) {
    plugin.onToolMove(pos0, pos1);
}

void PluginTool::applyPoint(Layer &layer, const Vector2d &pos) {
    // Does nothing, heh
}

void PluginTool::onBegin(const Vector2d &pos) {
    plugin.onToolPress(pos);
}

void PluginTool::onEnd(const Vector2d &pos) {
    plugin.onToolRelease(pos);
}


void PluginEffect::apply(Layer &layer) {
    plugin.onEffectApply();
}

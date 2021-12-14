#include <AGF/llgui.h>
#include "plugin_tool.h"
#include "../loader.h"


void PluginTool::applyLine(Layer &layer, const Vector2d &pos0, const Vector2d &pos1) {
    plugin.onToolMove(pos0, pos1);

    applyFlushPolicy(layer.getTexture());
}

void PluginTool::applyPoint(Layer &layer, const Vector2d &pos) {
    // Does nothing, heh
    applyFlushPolicy(layer.getTexture());
}

void PluginTool::onBegin(const Vector2d &pos) {
    plugin.onToolPress(pos);
}

void PluginTool::onEnd(const Vector2d &pos) {
    plugin.onToolRelease(pos);
}

void PluginTool::applyFlushPolicy(abel::gui::Texture &texture) {
    plugin.applyFlushPolicy(texture);
}


void PluginEffect::apply(Layer &layer) {
    plugin.onEffectApply();
    applyFlushPolicy(layer.getTexture());
}

void PluginEffect::applyFlushPolicy(abel::gui::Texture &texture) {
    plugin.applyFlushPolicy(texture);
}
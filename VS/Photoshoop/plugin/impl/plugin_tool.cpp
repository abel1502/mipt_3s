#include <AGF/llgui.h>
#include "plugin_tool.h"
#include "../loader.h"


void PluginTool::applyLine(Layer &layer, const Vector2d &pos0, const Vector2d &pos1) {
    plugin.onToolMove(pos0, pos1);

    applyFlushPolicy(layer);
}

void PluginTool::applyPoint(Layer &layer, const Vector2d &pos) {
    // Does nothing, heh
    applyFlushPolicy(layer);
}

void PluginTool::onBegin(const Vector2d &pos) {
    plugin.onToolPress(pos);
}

void PluginTool::onEnd(const Vector2d &pos) {
    plugin.onToolRelease(pos);
}

void PluginTool::applyFlushPolicy(Layer &layer) {
    plugin.applyFlushPolicy(layer);
}


void PluginEffect::apply(Layer &layer) {
    plugin.onEffectApply();
    applyFlushPolicy(layer);
}

void PluginEffect::applyFlushPolicy(Layer &layer) {
    plugin.applyFlushPolicy(layer);
}

bool PluginEffect::supportsSettings() const {
    return true;
}

void PluginEffect::showSettings(bool hidden) {
    if (hidden) {
        // Can't force the plugin to hide its settings
        return;
    }

    plugin.showSettings();
}
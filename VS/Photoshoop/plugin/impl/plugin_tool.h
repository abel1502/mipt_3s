#pragma once

#include "../../tool.h"


class Plugin;

class PluginTool : public Tool {
public:
    inline PluginTool(ToolManager *manager_, const Plugin &plugin_) :
        Tool(manager_), plugin{plugin_} {}

    virtual void applyLine(Layer &layer, const Vector2d &pos0, const Vector2d &pos1) override;

    virtual void applyPoint(Layer &layer, const Vector2d &pos) override;

    virtual void onBegin(const Vector2d &pos) override;

    virtual void onEnd(const Vector2d &pos) override;

protected:
    const Plugin &plugin;

    void applyFlushPolicy(abel::gui::Texture &texture);

};


class PluginEffect : public Effect {
public:
    inline PluginEffect(ToolManager *manager_, const Plugin &plugin_) :
        Effect(manager_), plugin{plugin_} {}

    virtual void apply(Layer &layer) override;

protected:
    const Plugin &plugin;

    void applyFlushPolicy(abel::gui::Texture &texture);

};

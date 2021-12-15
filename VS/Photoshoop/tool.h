#pragma once

#include <ACL/general.h>
#include <ACL/unique_ptr.h>
#include <AGF/widget.h>
#include <AGF/widgets/window.h>
#include <AGF/helpers/widget_ref.h>
#include "tool_manager.h"
#include "layer.h"


using abel::gui::Rect;
using abel::gui::Color;
using abel::math::Vector2d;

namespace widgets = abel::gui::widgets;


class ToolManager;


class Tool {
public:
    Tool(ToolManager *manager_);

    virtual void applyLine(Layer &layer, const Vector2d &pos0, const Vector2d &pos1);

    virtual void applyPoint(Layer &layer, const Vector2d &pos) = 0;

    virtual void onBegin(const Vector2d &pos);

    virtual void onEnd(const Vector2d &pos);

    inline const ToolManager &getManager() const { return *manager; }
    inline       ToolManager &getManager()       { return *manager; }

    virtual ~Tool() = default;

protected:
    ToolManager *manager;

};


class Effect {
public:
    Effect(ToolManager *manager_);

    virtual void apply(Layer &layer) = 0;

    void showSettings(bool hidden = false);

    inline void hideSettings() {
        return showSettings(false);
    }

    inline const ToolManager &getManager() const { return *manager; }
    inline       ToolManager &getManager()       { return *manager; }

    virtual bool supportsSettings() const;

    virtual ~Effect();

protected:
    ToolManager *manager;
    abel::gui::WidgetRefTo<widgets::Window> settingsWnd = nullptr;

    virtual Vector2d getDesiredSettingsWndSize() const;
    virtual void populateSettingsWindow();

};

#pragma once

#include <AGF/llgui.h>
#include <ACL/general.h>
#include <AGF/application.h>
#include <AGF/widgets/window.h>
#include "tool_manager.h"
#include "tools_widget.h"
#include "plugin/loader.h"


class MyApp final : public abel::gui::Application {
public:
    class MainWidget : public widgets::StaticGroup<abel::gui::Widget, widgets::WindowManager> {
    public:
        using Base = widgets::StaticGroup<abel::gui::Widget, widgets::WindowManager>;
        EVENT_HANDLER_USING(Base);


        inline MainWidget(const Rect<double> &region_, Widget *overlay, widgets::WindowManager *wmgr) :
            Base(nullptr, region_, overlay, wmgr) {}

        SGRP_DECLARE_BINDING_I(overlay, 0);
        SGRP_DECLARE_BINDING_I(wmgr, 1);

    };


    ToolManager toolMgr{};
    PluginMgr pluginMgr{};


    MyApp();

    virtual void init(int argc, const char **argv) override;

    virtual void deinit() override;

    virtual ~MyApp() override;

    static inline MyApp &getInstance() {
        return dynamic_cast<MyApp &>(abel::gui::Application::getInstance());
    }

    ToolsWidget &getToolsWidget() {
        assert(toolsWidget);
        return *toolsWidget;
    }

    widgets::WindowManager &getWindowMgrWidget() {
        return dynamic_cast<MainWidget *>(mainWidget.get())->wmgr();
    }

    abel::gui::Widget &getOverlay() {
        return dynamic_cast<MainWidget *>(mainWidget.get())->overlay();
    }

protected:
    ToolsWidget *toolsWidget = nullptr;

};

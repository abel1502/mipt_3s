#pragma once

#include <AGF/llgui.h>
#include <ACL/general.h>
#include <AGF/application.h>
#include "tool_manager.h"


class MyApp final : public abel::gui::Application {
public:
    ToolManager toolMgr{};


    MyApp();

    virtual void init(int argc, const char **argv) override;

    virtual void deinit() override;

    virtual ~MyApp() override;

    static inline MyApp &getInstance() {
        return dynamic_cast<MyApp &>(abel::gui::Application::getInstance());
    }

protected:
    //

};

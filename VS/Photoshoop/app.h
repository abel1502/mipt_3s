#pragma once

#include <AGF/llgui.h>
#include <ACL/general.h>
#include <AGF/application.h>


class MyApp final : public abel::gui::Application {
public:
    MyApp();

    virtual void init(int argc, const char **argv) override;

    virtual void deinit() override;

    virtual ~MyApp() override;

    // TODO: The other one as well

protected:
    //

};

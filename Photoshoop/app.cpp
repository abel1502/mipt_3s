#include <AGF/llgui.h>
#include "app.h"
#include <AGF/widgets/button.h>


abel::gui::Application::app_ptr_t abel::gui::Application::create() {
    return app_ptr_t(new MyApp());
}


MyApp::MyApp() :
    Application() {}


MyApp::~MyApp() = default;


void MyApp::init(int argc, const char **argv) {
    if (initialized)
        return;

    abel::verbosity = 2;

    abel::unique_ptr<abel::gui::widgets::SimpleButton> btn{};
    btn.emplace(nullptr, abel::gui::Rect<double>::wh(20, 20, 100, 60), "Button");
    btn->sigClick += [](){ static unsigned cnt = 3; DBG("Knopochka"); return !(--cnt); };
    mainWidget = std::move(btn);

    Application::init(argc, argv);
    DBG("");
}


void MyApp::deinit() {
    if (!initialized)
        return;

    Application::deinit();
    DBG("");
}

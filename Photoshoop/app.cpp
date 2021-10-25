#include <AGF/llgui.h>
#include "app.h"
#include <AGF/widgets/button.h>


abel::gui::Application::app_ptr_t abel::gui::Application::create() {
    return app_ptr_t(new MyApp());
}


MyApp::MyApp() :
    Application() {

    mainWidget = new abel::gui::widgets::SimpleButton(nullptr, abel::gui::Rect<double>::wh(20, 20, 100, 60), "Button");
}


MyApp::~MyApp() = default;


void MyApp::init(int argc, const char **argv) {
    abel::verbosity = 2;
    Application::init(argc, argv);
    DBG("");
}


void MyApp::deinit() {
    Application::deinit();
    DBG("");
}

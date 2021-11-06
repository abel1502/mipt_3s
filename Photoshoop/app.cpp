#include <AGF/llgui.h>
#include "app.h"
#include <AGF/widgets/all.h>


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

    using namespace abel::gui::widgets;
    using abel::gui::Rect;

    Group *grp = new Group(nullptr, Rect<double>::wh(0, 0, 140, 170));
    Button *btn1 = new Button(nullptr, Rect<double>::wh(20, 20, 100, 60), "Button one");
    btn1->sigClick += [](){ DBG("Knopochka"); return false; };
    Button *btn2 = new Button(nullptr, Rect<double>::wh(20, 90, 100, 60), "Button two");
    btn2->sigClick += [](){ static unsigned cnt = 4; DBG("Another knopochka"); return !(--cnt); };
    grp->addChild(btn1);
    grp->addChild(btn2);
    mainWidget = grp;

    Application::init(argc, argv);
}


void MyApp::deinit() {
    if (!initialized)
        return;

    Application::deinit();
}

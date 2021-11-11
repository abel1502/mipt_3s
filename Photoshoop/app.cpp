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

    Group *grp = new Group(nullptr, Rect<double>::wh(10, 10, 140, 170));

    Button &btn1 = grp->createChild<Button>(Rect<double>::wh(20, 20, 100, 60), "Button one");
    btn1.sigClick += [](){
        DBG("Knopochka"); return false;
    };

    Button &btn2 = grp->createChild<Button>(Rect<double>::wh(20, 90, 100, 60), "Button two");
    btn2.sigClick += [](){
        static unsigned cnt = 5;
        DBG("Another knopochka (%u presses left)", cnt - 1);
        return !(--cnt);
    };

    // mainWidget = grp;

    WindowManager *mgr = new WindowManager(nullptr, Rect<double>::wh(0, 0, 800, 600));
    mgr->createWindow(Rect<double>::wh(140, 50, 250, 200), "Sample window", grp);
    mgr->createWindow(Rect<double>::wh(240, 70, 250, 200), "Sample window 2", new Window::EmptyBody());
    mgr->createWindow(Rect<double>::wh(80, 90, 250, 200),  "Sample window 3", new Window::EmptyBody());


    mainWidget = mgr;

    Application::init(argc, argv);
}


void MyApp::deinit() {
    if (!initialized)
        return;

    Application::deinit();
}

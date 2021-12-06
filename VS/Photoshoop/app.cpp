#include <AGF/llgui.h>
#include <AGF/widgets/all.h>
#include "app.h"
#include "canvas.h"
#include "color_picker.h"
#include "physics/widget.h"


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

    Application::init(argc, argv);

    using namespace abel::gui::widgets;
    using abel::gui::Rect;

    Group *grp = new Group(nullptr, Rect<double>::wh(10, 10, 140, 170));

    Button &btn1 = grp->createChild<Button>(Rect<double>::wh(20, 20, 100, 60), "Button one");
    btn1.sigClick += [](){
        DBG("Knopochka");
        return false;
    };

    Button &btn2 = grp->createChild<Button>(Rect<double>::wh(20, 90, 100, 60), "Button two");
    btn2.sigClick += [](){
        static unsigned cnt = 5;
        DBG("Another knopochka (%u presses left)", cnt - 1);
        return !(--cnt);
    };

    // mainWidget = grp;

    Canvas *canvas = new Canvas(nullptr, Rect<double>::se(0, 0, 300, 200).pad(10));

    // Slider2D *slider = new Slider2D(nullptr, Rect<double>::wh(20, 20, 100, 100), Rect<double>::se(-25, -25, 25, 25));
    SliderH *slider = new SliderH(nullptr, Rect<double>::wh(20, 20, 100, 15), -25, 25);

    // slider->sigChanged += [](Vector2d value) {
    //     DBG("Slider set to %lg", value.x());
    //
    //     return false;
    // };

    ColorPicker *palette = new ColorPicker(nullptr, Rect<double>::wh(0, 0, 120, 175));

    btn2.sigClick += [palette]() {
        // TODO: Very bad, because palette could have been destroyed
        Color color = palette->getColor();

        DBG("The color is %f %f %f %f", color.r(), color.g(), color.b(), palette->getAlpha());
        palette->setColor(color);

        return false;
    };

    MoleculesWidget *molecules = new MoleculesWidget(nullptr, Rect<double>::wh(0, 0, 400, 300));

    WindowManager *mgr = new WindowManager(nullptr, Rect<double>::wh(0, 0, 800, 600));
    mgr->createWindow(Rect<double>::wh(140,  50, 300, 200), "Some buttons", grp)
        .markEssential();
    mgr->createWindow(Rect<double>::wh(240,  70, 300, 200), "A drawing canvas", canvas);
    mgr->createWindow(Rect<double>::wh( 80,  90, 300, 200), "A slider", slider /*new Window::EmptyBody()*/);
    mgr->createWindow(Rect<double>::wh(290, 150, 300, 200), "A color picker, woah!", palette)
        .markEssential();
    mgr->createWindow(Rect<double>::wh(400, 10, 400, 300), "These molecules look familiar", molecules)
        .markEssential();

    // palette->setColor(Color::BLACK);
    // palette->setAlpha(1);


    mainWidget = mgr;
}


void MyApp::deinit() {
    if (!initialized)
        return;

    Application::deinit();
}

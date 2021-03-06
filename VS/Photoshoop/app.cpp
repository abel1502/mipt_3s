#include <AGF/llgui.h>
#include <AGF/widgets/all.h>
#include <AGF/helpers/widget_ref.h>
#include "app.h"
#include "canvas.h"
#include "donut.h"
#include "spline.h"
#include "physics/widget.h"
#include "tools/invert_eff.h"
#include "tools/spline_eff.h"
#include "plugin/loader.h"
#include <ACL/debug.h>


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

    constexpr bool USE_MOLECULES = false;

    using namespace abel::gui::widgets;
    using abel::gui::Rect;
    using abel::gui::WidgetRef;
    using abel::gui::WidgetRefTo;

    Group *grp = new Group(nullptr, Rect<double>::wh(10, 10, 290, 170));

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

    #if 0
    Donut &donut = grp->createChild<Donut>(Rect<double>::wh(5, -10, 150, 190));
    donut.sigClick += []() {
        DBG("You just clicked a donut");

        return false;
    };

    grp->focusChild(donut);
    #endif
    grp->suppressRefocus = true;

    // mainWidget = grp;

    Canvas *canvas = new Canvas(nullptr, Rect<double>::se(0, 0, 300, 200).pad(10));

    // Slider2D *slider = new Slider2D(nullptr, Rect<double>::wh(20, 20, 100, 100), Rect<double>::se(-25, -25, 25, 25));
    // SliderH *slider = new SliderH(nullptr, Rect<double>::wh(20, 20, 100, 15), -25, 25);
    // ScrollbarH *scrollbar = new ScrollbarH(nullptr, Rect<double>::wh(20, 20, 100, 15));
    // ScrollableLayout *box = new ScrollableLayout(nullptr, Rect<double>::wh(20, 20, 100, 150));

    // slider->sigChanged += [](Vector2d value) {
    //     DBG("Slider set to %lg", value.x());
    //
    //     return false;
    // };

    // ColorPicker *palette = new ColorPicker(nullptr, Rect<double>::wh(0, 0, 120, 175));

    // btn2.sigClick += [palette = WidgetRefTo(palette)]() {
    //     if (!palette) {
    //         return true;
    //     }
    //
    //     // TODO: Very bad, because palette could have been destroyed
    //     Color color = palette->getColor();
    //
    //     DBG("The color is %f %f %f %f", color.r(), color.g(), color.b(), palette->getAlpha());
    //     palette->setColor(color);
    //
    //     return false;
    // };

    toolsWidget = new ToolsWidget(nullptr, Rect<double>::wh(0, 0, 350, 225));

    ToolManager::effect_handle_t hInvertEffect = toolMgr.addEffect(new InvertEffect(&toolMgr));
    toolsWidget->addEffectButton(hInvertEffect, "Invert");

    ToolManager::effect_handle_t hSplineEffect = toolMgr.addEffect(new SplineEffect(&toolMgr));
    toolsWidget->addEffectButton(hSplineEffect, "Spline");
    // toolsWidget->addToolButton(0, "Brush 2: Again");
    // toolsWidget->addToolButton(0, "Brush 3: Yup");

    MoleculesWidget *molecules = nullptr;

    if constexpr (USE_MOLECULES) {
        molecules = new MoleculesWidget(nullptr, Rect<double>::wh(0, 0, 400, 300));
    }

    {
        Layout &lay = grp->createChild<Layout>(Rect<double>::wh(140, 0, 150, 170));

        if constexpr (USE_MOLECULES) {
            lay.createChild<Button>(Rect<double>::wh(0, 0, 140, 30), "Spawn a molecule")
                .sigClick += [molecules = WidgetRefTo/*<decltype(molecules)>*/(molecules)]() {

                if (!molecules) {
                    DBG("No more molecules, unhooking");

                    return true;
                }

                molecules->getManager().addRandomMolecule(Molecule::P_BALL);

                return false;
            };
        }

        lay.createChild<Button>(Rect<double>::wh(0, 0, 140, 30), "Create layer")
            .sigClick += [canvas = WidgetRefTo(canvas)]() {
            if (!canvas) {
                return true;
            }

            canvas->addLayer();
            DBG("Now on layer %u", canvas->getActiveLayerIdx());

            return false;
        };

        lay.createChild<Button>(Rect<double>::wh(0, 0, 140, 30), "Next layer")
            .sigClick += [canvas = WidgetRefTo(canvas)]() {
            if (!canvas) {
                return true;
            }

            canvas->nextLayer();
            DBG("Now on layer %u", canvas->getActiveLayerIdx());

            return false;
        };

        lay.createChild<Button>(Rect<double>::wh(0, 0, 140, 30), "Prev layer")
            .sigClick += [canvas = WidgetRefTo(canvas)]() {
            if (!canvas) {
                return true;
            }

            canvas->nextLayer();
            DBG("Now on layer %u", canvas->getActiveLayerIdx());

            return false;
        };

        lay.createChild<Button>(Rect<double>::wh(0, 0, 140, 30), "Load image")
            .sigClick += [canvas = WidgetRefTo(canvas)]() {
            if (!canvas) {
                return true;
            }

            char path[MAX_PATH + 1] = "";
            printf("img > ");
            fgets(path, MAX_PATH, stdin);
            size_t len = strlen(path);
            if (len > 0) {
                path[len - 1] = 0;
            }

            std::fs::path imgPath{path};

            try {
                canvas->loadImage(imgPath);
            } catch (const abel::error &e) {
                ERR("Failed to load image from '%ls': \"%s\"",
                    imgPath.c_str(), e.what());
            }

            return false;
        };

        lay.createChild<TextBox>(Rect<double>::wh(0, 0, 140, 30), "Default")
            .sigSubmit += [](const char *text) {

            DBG("Got \"%s\"", text);

            return false;
        };
    }

    // Spline *spline = new Spline(nullptr, Rect<double>::wh(0, 0, 175, 150));

    WindowManager *mgr = new WindowManager(nullptr, Rect<double>::wh(0, 0, 800, 600));
    mgr->createWindow(Rect<double>::wh(140,  50, 300, 200), "Some buttons", grp);
    mgr->createWindow(Rect<double>::wh(240,  70, 300, 200), "A drawing canvas", canvas);
    mgr->createWindow(Rect<double>::wh(240, 170, 300, 200), "Another canvas",
                      new Canvas(nullptr, Rect<double>::se(0, 0, 300, 200).pad(10)));
    mgr->createWindow(toolsWidget->getRegion() + Vector2d{290, 150}, "Tools for all!", toolsWidget)
        .markEssential();
    if constexpr (USE_MOLECULES) {
        mgr->createWindow(Rect<double>::wh(400,  50, 400, 300), "These molecules look familiar", molecules)
            /*.markEssential() */ ;
    }

    // palette->setColor(Color::BLACK);
    // palette->setAlpha(1);

    mainWidget = new MainWidget(mgr->getRegion(), pluginMgr.createOverlay(mgr->getRegion()), mgr);

    pluginMgr.loadAll();
}


void MyApp::deinit() {
    if (!initialized)
        return;

    Application::deinit();

    pluginMgr.deinitAll();
}

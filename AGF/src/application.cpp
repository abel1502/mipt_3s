#include <AGF/llgui.h>
#include <AGF/application.h>


namespace abel::gui {


Application *Application::instance = nullptr;


Application::Application(Widget *mainWidget_) :
    mainWidget{mainWidget_} {}


void Application::setup() {
    if (instance)
        return;

    instance = create();

    if (!instance) {
        throw error("Failed to create application instance");
    }
}


}



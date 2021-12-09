#include <AGF/llgui.h>
#include <AGF/application.h>


using abel::gui::Application;


/// The `main` function for AGF-based projects should belong to the library;
/// All user-specified entry code should be part of the `Application` class
/// (see `application.h` for details)
extern "C" int main(int argc, const char **argv) {
    MAIN_TRY(
        Application::setup();

        Application &app = Application::getInstance();

        app.init(argc, argv);
        app.run();
        app.deinit();

        app.teardown();

        return 0;
    );
}

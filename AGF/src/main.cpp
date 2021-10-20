#include <AGF/application.h>


using abel::gui::Application;

#if 0
/// The `main` function for AGF-based projects should belong to the library;
/// All user-specified entry code should be part of the `Application` class
/// (see `application.h` for details)
int main(int argc, const char **argv) {
    MAIN_TRY(
        Application::setup();

        Application &app = Application.getInstance();

        app.init();

        app.eventLoop();

        app.deinit();

        return 0;
    )
}
#endif

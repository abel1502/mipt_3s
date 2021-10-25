#include <AGF/llgui.h>
#include <AGF/application.h>


namespace abel::gui {


Application::app_ptr_t Application::instance = nullptr;


Application::Application() {}


void Application::setup() {
    if (instance)
        return;

    instance = create();

    if (!instance) {
        throw error("Failed to create application instance");
    }
}


void Application::teardown() {
    if (!instance)
        return;

    instance = nullptr;
}


void Application::init(int /*argc*/, const char **/*argv*/) {
    if (initialized)
        return;

    initialized = true;

    wnd.emplace();  // Default size
    wnd->setWndProc(&_wndproc);
}


void Application::run() {
    wnd->update();

    while (!quitting) {
        asm volatile ("pause");
    }
}


void Application::deinit() {
    if (!initialized)
        return;

    initialized = false;

    wnd.reset();
}


LRESULT Application::dispatchWindowsEvent(UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if (!mainWidget) {
        ERR("Main widget absent; ignoring event");
        return 0;
    }

    if (!wnd) {
        ERR("Window absent; ignoring event");
        return 0;
    }

    switch (uMsg) {
        // TODO: Translate applicable ones to widget_events
    case WM_PAINT: {
        abel::gui::Texture texture{*wnd};

        mainWidget->dispatchEvent(EVENT_CLS_NAME(Render){texture.getScreenRect(), texture});

        wnd->render(texture);

        // Missing intentionally, to avoid recursive WM_PAINTs
        // wnd->update();
    } return 0;

    case WM_QUIT: {
        quitting = true;
    } return 0;

    default: {
        DBG("Windows message: %u %zu %zu", uMsg, wParam, lParam);
    } return 0;

    }
}


LRESULT CALLBACK Application::_wndproc(HWND /*hWnd*/, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    return Application::getInstance().dispatchWindowsEvent(uMsg, wParam, lParam);
}


}



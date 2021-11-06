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

    while (!finished) {
        asm volatile ("pause");
    }
}


void Application::deinit() {
    if (!initialized)
        return;

    initialized = false;

    mainWidget.reset();
    wnd.reset();
}


// TODO: Maybe something more sophisticated

void Application::releaseMouse() {
    mouseCaptureHolder = nullptr;
}

void Application::releaseMouse(Widget *widget) {
    DBG("Releasing by %p over %p", widget, mouseCaptureHolder);

    REQUIRE(widget == mouseCaptureHolder);  // TODO: || !mouseCaptureHolder ?

    releaseMouse();
}

void Application::captureMouse(Widget *widget) {
    DBG("Capturing by %p over %p", widget, mouseCaptureHolder);

    REQUIRE(!mouseCaptureHolder);

    mouseCaptureHolder = widget;
}


LRESULT Application::dispatchWindowsEvent(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if (finished) {
        return 0;
    }

    if (!mainWidget) {
        ERR("Main widget absent; ignoring event");
        return 0;
    }

    if (!wnd) {
        ERR("Window absent; ignoring event");
        return 0;
    }

    (void)hWnd, (void)uMsg, (void)wParam, (void)lParam;

    MouseBtn mouseBtn = MouseBtn::Left;
    MouseClickType mouseClickType = MouseClickType::Down;

    switch (uMsg) {
        // TODO: Translate applicable ones to widget_events
    case WM_PAINT: {
        abel::gui::Texture texture{*wnd};
        texture.clear(Color::WHITE);  // TODO: Remove?

        mainWidget->dispatchEvent(RenderEvent{texture.getScreenRect(), texture});

        wnd->render(texture);

        // Missing intentionally, to avoid recursive WM_PAINTs
        // wnd->update();
    } return 0;

    case WM_LBUTTONDOWN:
        mouseBtn = MouseBtn::Left;
        mouseClickType = MouseClickType::Down;
        goto mouseClickEvent;

    case WM_MBUTTONDOWN:
        mouseBtn = MouseBtn::Middle;
        mouseClickType = MouseClickType::Down;
        goto mouseClickEvent;

    case WM_RBUTTONDOWN:
        mouseBtn = MouseBtn::Right;
        mouseClickType = MouseClickType::Down;
        goto mouseClickEvent;

    case WM_LBUTTONUP:
        mouseBtn = MouseBtn::Left;
        mouseClickType = MouseClickType::Up;
        goto mouseClickEvent;

    case WM_MBUTTONUP:
        mouseBtn = MouseBtn::Middle;
        mouseClickType = MouseClickType::Up;
        goto mouseClickEvent;

    case WM_RBUTTONUP:
        mouseBtn = MouseBtn::Right;
        mouseClickType = MouseClickType::Up;
        goto mouseClickEvent;

    mouseClickEvent: {
        Widget *target = isMouseCaptured() ? mouseCaptureHolder : mainWidget.get();

        target->dispatchEvent(MouseClickEvent{
            Vector2d{GET_X_LPARAM(lParam),
                     GET_Y_LPARAM(lParam)},
            MouseAttrs{wParam},
            mouseBtn,
            mouseClickType});
    } return 0;

    case WM_CLOSE: {
        DBG("Closing");
        finished = true;
    } return TX_WMSG_HANDLED | 0;

    /*case WM_DESTROY:
    case WM_QUIT: {
        if (initialized) {
            asm volatile ("pause");
        }
    } return 0;*/

    // TODO: WM_QUIT

    default: {
        //DBG("Windows message: %u %zu %zu", uMsg, wParam, lParam);
    } return 0;

    }
}


LRESULT CALLBACK Application::_wndproc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if (!instance)
        return 0;

    return getInstance().dispatchWindowsEvent(hWnd, uMsg, wParam, lParam);
}


}



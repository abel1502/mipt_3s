#include <AGF/llgui.h>
#include <AGF/application.h>


namespace abel::gui {


Application::app_ptr_t Application::instance = nullptr;


Application::Application() {}

void Application::enqueueAction(action_cb_t &&callback, ActionPriority priority) {
    switch (priority) {
    case P_NORMAL:
        actionQueueMutex.lock();
        actionQueue.push_back(std::move(callback));
        actionQueueMutex.unlock();
        break;

    case P_HIGH:
        actionQueueMutex.lock();
        actionQueue.push_front(std::move(callback));
        actionQueueMutex.unlock();
        break;

    case P_IMMEDIATE:
    {
        actionExecMutex.lock();
        callback(*this);
        actionExecMutex.unlock();
    } break;

    NODEFAULT
    }
}

void Application::executeQueuedAction() {
    if (!hasQueuedActions())
        return;

    actionQueueMutex.lock();
    action_cb_t action = std::move(actionQueue.front());
    actionQueue.pop_front();
    actionQueueMutex.unlock();

    actionExecMutex.lock();
    action(*this);
    actionExecMutex.unlock();
}

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

    static constexpr char DEFAULT_STYLE_PATH[] = "resources/default.style";
    try {
        styleMgr.loadDefaultStyle(DEFAULT_STYLE_PATH);
    } catch (abel::error &e) {
        ERR("Failed to load default style from \"%s\" - error \"%s\".\n"
            "Defaulting to system-backed style", DEFAULT_STYLE_PATH, e.what());

        styleMgr.getStyle(styleMgr.ROOT_STYLE_HANDLE).markAllSysDrawn();
    }

    wnd.emplace();  // Default size
    wnd->setWndProc(&_wndproc);
}

void Application::run() {
    wnd->update();

    while (!finished) {
        if (hasQueuedActions()) {
            executeQueuedAction();
        }

        // asm volatile ("pause");
        YieldProcessor();
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
    wantSysMouseCapture = false;
}

void Application::releaseMouse(Widget *widget) {
    // DBG("Releasing by %p over %p", widget, mouseCaptureHolder);
    REQUIRE(widget == mouseCaptureHolder);  // TODO: || !mouseCaptureHolder ?

    releaseMouse();
}

void Application::captureMouse(Widget *widget) {
    // DBG("Capturing by %p over %p", widget, mouseCaptureHolder);
    REQUIRE(!mouseCaptureHolder);

    mouseCaptureHolder = widget;
    wantSysMouseCapture = true;
}

void Application::demandRedraw() {
    if (wnd) {
        // wnd->update();
        wnd->demandRedraw();
    }
}


void Application::_updateSysMouseCapture() {
    if (wantSysMouseCapture && !wnd->isMouseCaptured()) {
        wnd->captureMouse();
    } else if (!wantSysMouseCapture && wnd->isMouseCaptured()) {
        wnd->releaseMouse();
    }
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
        if constexpr (FPS_LIMIT) {
            static uint64_t renderTimer = 0;
            static uint64_t lastTickCount = 0;

            uint64_t curTickCount = GetTickCount64();
            renderTimer += curTickCount - lastTickCount;
            lastTickCount = curTickCount;

            if (renderTimer < 1000 / FPS_LIMIT)
                return 0;
            renderTimer = 0;
        }

        abel::gui::Texture texture{*wnd};
        texture.clear(Color::WHITE);  // TODO: Remove, or maybe move to WM_ERASEBKGND?

        // Render event must be synchronous due to WINAPI's limitations
        enqueueEvent(RenderEvent{texture.getRect(), texture}, P_IMMEDIATE);

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
        _updateSysMouseCapture();

        Vector2d pos{(double)GET_X_LPARAM(lParam),
                     (double)GET_Y_LPARAM(lParam)};

        enqueueEvent(MouseClickEvent{pos, MouseAttrs{wParam},
                                     mouseBtn, mouseClickType});
    } return 0;

    case WM_MOUSEMOVE: {
        _updateSysMouseCapture();

        static bool lastPosSet = false;
        static Vector2d lastPos{0, 0};

        Vector2d curPos{(double)GET_X_LPARAM(lParam),
                        (double)GET_Y_LPARAM(lParam)};

        // curPos = wnd->getMousePos();

        if (!lastPosSet) {
            lastPos = curPos;
            lastPosSet = true;
        }

        enqueueEvent(MouseMoveEvent{lastPos, curPos, MouseAttrs{wParam}});
        lastPos = curPos;
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

    // TODO: WM_QUIT?

    default: {
        // DBG("Windows message: %u %zu %zu", uMsg, wParam, lParam);
    } return 0;

    }
}


LRESULT CALLBACK Application::_wndproc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if (!instance)
        return 0;

    return getInstance().dispatchWindowsEvent(hWnd, uMsg, wParam, lParam);
}


}



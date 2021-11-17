#ifndef AGF_APPLICATION_H
#define AGF_APPLICATION_H

#include <cassert>
#include <ACL/general.h>
#include <AGF/llgui_pre.h>
#include <ACL/type_traits.h>
#include <ACL/signal.h>
#include <ACL/unique_ptr.h>
#include <AGF/mouse.h>
#include <AGF/widget.h>
#include <AGF/widget_event.h>
#include <AGF/events.h>
#include <AGF/style.h>
#include <deque>
#include <functional>
#include <mutex>
#include <atomic>


namespace abel::gui {


/// Inherit from this class to create your own application
class Application {
public:
    DECLARE_ERROR(error, abel::error)

    using app_ptr_t = unique_ptr<Application>;
    using action_cb_t = std::function<void(Application &app)>;

    static constexpr uint64_t FPS_LIMIT = 0;

    enum ActionPriority {
        P_NORMAL,
        P_HIGH,
        P_IMMEDIATE,  /// Ensures synchronous execution in the *calling* thread
    };


    Signal<void (double deltaT)> sigTick{};


    /// =========== [ These should be implemented in some way in user code ] ===========
    /// These should simply be provided an implementation in your code.

    /// Create an application instance and return a pointer to it.
    /// If you allocate it in dynamic memory, make sure to dispose of it properly in
    /// the destroy method (check if this == instance, and in that case, delete this)
    static app_ptr_t create();

    /// ================================================================================
    /// These should (could) be overridden in your child class, that will be constructed
    /// by the `create` method.

    virtual void init(int argc, const char **argv);

    virtual void run();

    virtual void deinit();

    virtual LRESULT dispatchWindowsEvent(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    virtual ~Application() = default;

    /// ================================================================================

    template <typename T>
    inline void dispatchEvent(const T &event) {
        static_assert(std::is_base_of_v<WidgetEvent, T>);
        static_assert(!std::is_same_v<MouseClickEvent, T>);
        static_assert(!std::is_same_v<MouseMoveEvent, T>);

        getMainWidget().dispatchEvent(event);
    }

    inline void dispatchEvent(const MouseClickEvent &event) {
        Widget &target = isMouseCaptured() ? *mouseCaptureHolder : getMainWidget();

        target.dispatchEvent(event);
    }

    inline void dispatchEvent(const MouseMoveEvent &event) {
        Widget &target = isMouseCaptured() ? *mouseCaptureHolder : getMainWidget();

        target.dispatchEvent(event);
    }

    inline bool hasQueuedActions() const noexcept { return !actionQueue.empty(); }

    void enqueueAction(action_cb_t &&callback, ActionPriority priority = P_NORMAL);

    template <typename T>
    inline void enqueueEvent(T &&event, ActionPriority priority = P_NORMAL) {
        static_assert(std::is_base_of_v<WidgetEvent, T>);

        enqueueAction([event = std::forward<T>(event)](Application &app){
            app.dispatchEvent(event);
        }, priority);
    }

    // Called from library main to set up the application initially
    static void setup();

    // Destroys the instance pointer
    static void teardown();

    double getTime();

    static inline Application &getInstance() {
        assert(instance);

        return *instance;
    }

    inline Widget &getMainWidget() {
        assert(mainWidget);

        return *mainWidget;
    }

    constexpr bool isInitialized() const noexcept { return initialized; }
       inline bool isFinished   () const noexcept { return finished;    }

    constexpr bool isMouseCaptured()               const noexcept { return mouseCaptureHolder; }
    constexpr bool isMouseCaptured(Widget *widget) const noexcept { return mouseCaptureHolder == widget; }
    void releaseMouse();
    void releaseMouse(Widget *widget);
    void captureMouse(Widget *widget);

    void demandRedraw();

    inline const StyleManager &getStyleManager() const noexcept { return styleMgr; }
    inline       StyleManager &getStyleManager()       noexcept { return styleMgr; }

protected:
    static app_ptr_t instance;


    unique_ptr<Window> wnd = nullptr;
    unique_ptr<Widget> mainWidget = nullptr;
    bool initialized = false;
    std::atomic<bool> finished = false;

    Widget *mouseCaptureHolder = nullptr;

    std::mutex actionQueueMutex{};
    std::mutex actionExecMutex{};
    std::deque<action_cb_t> actionQueue{};

    // TODO: Adapt more generally...?
    StyleManager styleMgr{};


    static LRESULT CALLBACK _wndproc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    Application();

    void executeQueuedAction();

};


}


#endif // AGF_APPLICATION_H

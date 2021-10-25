#ifndef AGF_APPLICATION_H
#define AGF_APPLICATION_H

#include <cassert>
#include <ACL/general.h>
#include <AGF/llgui_pre.h>
#include <ACL/type_traits.h>
#include <ACL/signal.h>
#include <ACL/unique_ptr.h>
#include <AGF/widget.h>
#include <AGF/widget_event.h>
#include <AGF/events.h>
#include <windef.h>


namespace abel::gui {


/// Inherit from this class to create your own application
class Application {
public:
    DECLARE_ERROR(error, abel::error)

    using app_ptr_t = unique_ptr<Application>;


    Signal<void (double deltaT)> sigTick{};
    Signal<void (EVENT_CLS_NAME(MouseClick))> sigMouseClick{};
    Signal<void (EVENT_CLS_NAME(MouseMove))> sigMouseMove{};


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

    virtual LRESULT dispatchWindowsEvent(UINT uMsg, WPARAM wParam, LPARAM lParam);

    virtual ~Application() = default;

    /// ================================================================================

    template <typename T>
    void dispatchEvent(const T &event) {
        static_assert(std::is_base_of_v<WidgetEvent, T>);

        getMainWidget().dispatchEvent(event);
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
       inline bool isQuitting   () const noexcept { return quitting;    }

protected:
    static app_ptr_t instance;


    unique_ptr<Window> wnd = nullptr;
    unique_ptr<Widget> mainWidget = nullptr;
    bool initialized = false;
    volatile bool quitting = false;


    static LRESULT CALLBACK _wndproc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    Application();

};


}


#endif // AGF_APPLICATION_H

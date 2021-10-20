#ifndef AGF_APPLICATION_H
#define AGF_APPLICATION_H

#include <cassert>
#include <ACL/general.h>
#include <ACL/type_traits.h>
#include <ACL/signal.h>
#include <AGF/widget.h>
#include <AGF/widget_event.h>
#include <AGF/llgui_pre.h>


namespace abel::gui {


/// Inherit from this class to create your own application
class Application {
public:
    DECLARE_ERROR(error, abel::error)


    Signal<void (double deltaT)> tick{};


    /// =========== [ These should be implemented in some way in user code ] ===========
    /// These should simply be provided an implementation in your code.

    // Create an application instance and return a pointer to it.
    // If you allocate it in dynamic memory, make sure to dispose of it properly in
    // the destroy method (check if this == instance, and in that case, delete this)
    static Application *create();

    /// ================================================================================
    /// These should (could) be overridden in your child class, that will be constructed
    /// by the `create` method.

    virtual void init();

    virtual void deinit();

    virtual void eventLoop();

    virtual void destroy();

    /// ================================================================================


    Application() = delete;
    Application(Widget *mainWidget_);

    virtual ~Application();

    template <typename T>
    void dispatchEvent(const T &event) {
        static_assert(std::is_base_of_v<WidgetEvent, T>);

        getMainWidget().dispatchEvent(event);
    }

    inline Application &getInstance() {
        assert(instance);

        return *instance;
    }

    inline Widget &getMainWidget() {
        assert(mainWidget);

        return *mainWidget;
    }

    // Called from library main to set up the application initially
    static void setup();

protected:
    static Application *instance;


    Widget *mainWidget = nullptr;

};


}


#endif // AGF_APPLICATION_H

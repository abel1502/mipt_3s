#ifndef AGF_WIDGET_H
#define AGF_WIDGET_H


#include <ACL/general.h>
#include <ACL/signal.h>
#include <AGF/events.h>
#include <AGF/widget_pos.h>
#include <AGF/llgui_pre.h>
#include <ACL/unique_ptr.h>


#define EVENT_HANDLER_NAME(EVENT_NAME) \
    handler_on##EVENT_NAME

#define EVENT_HANDLER_METHOD(EVENT_NAME) \
    EventStatus EVENT_HANDLER_NAME(EVENT_NAME)(const EVENT_CLS_NAME(EVENT_NAME) &event)

#define EVENT_HANDLER_METHOD_SCOPED(CLS, EVENT_NAME) \
    Widget::EventStatus CLS::EVENT_HANDLER_NAME(EVENT_NAME)(const EVENT_CLS_NAME(EVENT_NAME) &event)

#define EVENT_HANDLER_DECL(EVENT_NAME) \
    virtual EVENT_HANDLER_METHOD(EVENT_NAME);

#define EVENT_HANDLER_OVERRIDE(EVENT_NAME) \
    virtual EVENT_HANDLER_METHOD(EVENT_NAME) override;

#define EVENT_HANDLER_IMPL(CLS, EVENT_NAME) \
    EVENT_HANDLER_METHOD_SCOPED(CLS, EVENT_NAME)

#define EVENT_HANDLER_CALL_BASE(BASE_CLS, EVENT_OBJ) \
    BASE_CLS::dispatchEvent(EVENT_OBJ)

#define EVENT_HANDLER_CALL_INST(INSTANCE, EVENT_OBJ) \
    (INSTANCE).dispatchEvent(EVENT_OBJ)


namespace abel::gui {


class Widget {
public:
    enum EventStatus {
        /// Event has been successfully processed, and should continue its way along the hierarchy
        E_CONTINUE,
        /// Event has been successfully processed, and no further processing should occur
        E_STOP_GLOBAL,
        /// Same as E_STOP_GLOBAL, but with the immediate parent instead of root. Out of its subtree, the processing should continue
        E_STOP_LOCAL,
        /// Event hasn't been processed (because nothing has to or could be done about it)
        E_SKIP,
    };


    Widget() = delete;

    virtual ~Widget() = default;

    #define EVENTS_DSL_ITEM_(NAME) EVENT_HANDLER_DECL(NAME)
    #include <AGF/events.dsl.h>

    /// This is barely a helper method, whose specializations will call the
    /// corresponding on* method. Don't bother implementing extra functionality
    /// in this, please.
    template <typename T>
    EventStatus dispatchEvent(const T &event);

    constexpr const Rect<double> &getRegion() const { return region; }
    constexpr       Rect<double> &getRegion()       { return region; }

protected:
    Widget *parent;
    Rect<double> region;
    bool visible = true;


    Widget(Widget *parent_, const Rect<double> &region_);

};

// TODO: Force-inline?
#define EVENTS_DSL_ITEM_(EVENT_NAME)                                                            \
    template <>                                                                                 \
    inline Widget::EventStatus Widget::dispatchEvent(const EVENT_CLS_NAME(EVENT_NAME) &event) { \
        return EVENT_HANDLER_NAME(EVENT_NAME)(event);                                           \
    }

#include <AGF/events.dsl.h>


}


#endif // AGF_WIDGET_H

#ifndef AGF_WIDGET_H
#define AGF_WIDGET_H


#include <ACL/general.h>
#include <ACL/signal.h>
#include <AGF/events.h>
#include <AGF/widget_pos.h>
#include <AGF/llgui_pre.h>
#include <ACL/unique_ptr.h>

/*
#define EVENT_HANDLER_NAME(EVENT_NAME) \
    handler_on##EVENT_NAME
*/

#define EVENT_HANDLER_USING(BASE_CLS) \
    using BASE_CLS::dispatchEvent;

#define EVENT_HANDLER_METHOD(EVENT_NAME) \
    EventStatus dispatchEvent(const EVENT_CLS_NAME(EVENT_NAME) &event)

#define EVENT_HANDLER_METHOD_SCOPED(CLS, EVENT_NAME) \
    Widget::EventStatus CLS::dispatchEvent(const EVENT_CLS_NAME(EVENT_NAME) &event)

#define EVENT_HANDLER_DECL(EVENT_NAME) \
    virtual EVENT_HANDLER_METHOD(EVENT_NAME);

#define EVENT_HANDLER_OVERRIDE(EVENT_NAME) \
    virtual EVENT_HANDLER_METHOD(EVENT_NAME) override;

#define EVENT_HANDLER_IMPL(CLS, EVENT_NAME) \
    EVENT_HANDLER_METHOD_SCOPED(CLS, EVENT_NAME)


namespace abel::gui {


class Widget {
public:
    #pragma pack(push,1)
    struct EventStatus {
        enum Level : unsigned char {
            CONT = 0,
            NODE = 1,
            SIBL = 2,
            TREE = 3,
        };

        Level stopLvl : 2;
        bool  handled : 1;

        constexpr EventStatus() :
            stopLvl{0}, handled{false} {}

        constexpr EventStatus(Level stopLvl_) :
            stopLvl{stopLvl_}, handled{true} {}

        static constexpr EventStatus skip()            { return EventStatus{     }; }
        static constexpr EventStatus done()            { return EventStatus{CONT }; }
        static constexpr EventStatus stop(Level level) { // assert(level != SIBL);  // TODO: Not sure about this, maybe should ban
                                                         return EventStatus{level}; }

        constexpr bool shouldHandle(Level level) { return level > stopLvl; }

        // TODO: Rework implementation and perhaps usage
        constexpr EventStatus &update(bool handled_ = false) {
            handled |= handled_;
            if (stopLvl <= SIBL)
                stopLvl = CONT;
            return *this;
        }

    };
    #pragma pack(pop)

    static_assert(sizeof(EventStatus) <= 4);


    Widget() = delete;

    virtual ~Widget() = default;

    #define EVENTS_DSL_ITEM_(NAME) EVENT_HANDLER_DECL(NAME)
    #include <AGF/events.dsl.h>

    constexpr const Rect<double> &getRegion() const { return region; }
    constexpr       Rect<double> &getRegion()       { return region; }

    virtual void updateParent(Widget *parent_);

    virtual void staticShift(const Vector2d &by);

protected:
    static constexpr enum {
        DRB_NONE,
        DRB_OBJECT,
        DRB_EVENT,
    } DEBUG_RENDER_BOXES = DRB_OBJECT;


    Widget *parent;
    Rect<double> region;
    bool visible = true;


    Widget(Widget *parent_, const Rect<double> &region_);

    /// Warning: it is preferable to avoid using this with events which
    /// don't demand modification, and dispatch them by reference without copying
    template <typename T>
    inline T translateEvent(const T &event) {
        static_assert(!T::demands_modification, "There should have been a full specialization "
                                                "of this function that would modify the event "
                                                "properly and return its copy.");

        return event;
    }

    // TODO: Maybe create an optimized version to modify only once per function somehow...?
    template <typename T>
    EventStatus dispatchToChild(Widget &child, const T &event) {
        if constexpr (!T::demands_modification) {
            return child.dispatchEvent(event);
        }

        return child.dispatchEvent(translateEvent(event));
    }

    // TODO: Maybe some sort of dispatchToBase, but it doesn't seem to work

};

template <>
inline RenderEvent Widget::translateEvent(const RenderEvent &event) {
    return event.createSubEvent(region);
}

}


#endif // AGF_WIDGET_H

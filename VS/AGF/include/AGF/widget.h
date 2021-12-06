#ifndef AGF_WIDGET_H
#define AGF_WIDGET_H


#include <ACL/general.h>
#include <ACL/signal.h>
#include <AGF/events.h>
#include <AGF/widget_pos.h>
#include <AGF/llgui_pre.h>
#include <AGF/style.h>
#include <ACL/unique_ptr.h>
#include <ACL/signal.h>

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
    virtual EVENT_HANDLER_METHOD(EVENT_NAME)

#define EVENT_HANDLER_OVERRIDE(EVENT_NAME) \
    virtual EVENT_HANDLER_METHOD(EVENT_NAME) override

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
            stopLvl{CONT}, handled{false} {}

        constexpr EventStatus(Level stopLvl_) :
            stopLvl{stopLvl_}, handled{true} {}

        static constexpr EventStatus skip()            noexcept { return EventStatus{     }; }
        static constexpr EventStatus done()            noexcept { return EventStatus{CONT }; }
        static constexpr EventStatus stop(Level level) noexcept { // assert(level != SIBL);  // TODO: Not sure about this, maybe should ban
                                                                  return EventStatus{level}; }

        constexpr bool shouldHandle(Level level) const noexcept { return level > stopLvl; }

        /// Called on the status returned from a child.
        /// Modifies the status to be correct for the current event
        /// Returns whether the children handling should cease.
        constexpr bool update() {
            bool shouldStop = !shouldHandle(SIBL);

            if (stopLvl <= SIBL)
                stopLvl = CONT;

            return shouldStop;
        }

        constexpr EventStatus &markHandled() { handled = true; return *this; }

    };
    #pragma pack(pop)

    static_assert(sizeof(EventStatus) <= 4);


    // Fires once
    Signal<void (Widget &widget)> sigDeath{};


    Widget() = delete;

    virtual ~Widget() = default;

    #define EVENTS_DSL_ITEM_(NAME) EVENT_HANDLER_DECL(NAME);
    #include <AGF/events.dsl.h>

    constexpr const Widget *getParent() const { return parent; }
    constexpr       Widget *getParent()       { return parent; }

    constexpr const Rect<double> &getRegion() const { return region; }
    constexpr       Rect<double> &getRegion()       { return region; }

    inline const Style &getStyle() const {
        return const_cast<Widget *>(this)->getStyle();
    }

    constexpr bool isDead() const { return dead; }

    void die();

    virtual bool updateParent(Widget *parent_);

    virtual bool staticShift(const Vector2d &by);

    virtual bool setStyle(StyleManager::StyleHandle newHandle);

    virtual bool hitTest(const Vector2d &pos) const;

protected:
    static constexpr enum {
        DRB_NONE,
        DRB_OBJECT,
        DRB_EVENT,
    } DEBUG_RENDER_BOXES = DRB_NONE;


    Widget *parent;
    Rect<double> region;
    StyleManager::StyleHandle styleHandle = StyleManager::ROOT_STYLE_HANDLE;
    bool hidden = false;
    bool dead = false;


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

    template <typename T>
    EventStatus dispatchToChild(Widget &child, const T &event) {
        assert(!child.isDead());

        if constexpr (!T::demands_modification) {
            return child.dispatchEvent(event);
        }

        return child.dispatchEvent(translateEvent(event));
    }

    // TODO: Maybe some sort of dispatchToBase, but it doesn't seem to work

    // TODO: Perhaps inline somehow, but I can't see an easy way
    Style &getStyle();

    EventStatus handleMouseOpaque(const MouseMoveEvent  &event, EventStatus baseStatus);
    EventStatus handleMouseOpaque(const MouseClickEvent &event, EventStatus baseStatus);

};

template <>
inline RenderEvent Widget::translateEvent(const RenderEvent &event) {
    return event.createSubEvent(region);
}

}


#endif // AGF_WIDGET_H

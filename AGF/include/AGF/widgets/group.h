#ifndef AGF_WIDGETS_GROUP_H
#define AGF_WIDGETS_GROUP_H

#include <AGF/widget.h>
#include <ACL/list.h>
// #include <ACL/debug.h>


namespace abel::gui::widgets {


// TODO: Maybe inherit virtually
class Group : public /*virtual*/ Widget {
public:
    EVENT_HANDLER_USING(Widget)

    Group(Widget *parent_, const Rect<double> &region_);

    template <typename T, typename ... As>
    T &createChild(const Rect<double> &relRegion, As &&... args) {
        static_assert(std::is_base_of_v<Widget, T>);

        // TODO: Maybe disallow overflow?
        Widget &child = *children.insertBack(new T(this, region.relRect(relRegion, true), std::forward<As>(args)...));

        return dynamic_cast<T &>(child);
    }

    #define EVENTS_DSL_ITEM_(NAME) \
        EVENT_HANDLER_OVERRIDE(NAME)
    #include <AGF/events.dsl.h>

protected:
    list<unique_ptr<Widget>> children{};

    template <typename T>
    static constexpr bool focusOnEvent = false;

    // TODO: A public alternative
    void addChild(Widget *child);

    // TODO: Same
    void clearChildren();

    template <typename T>
    inline EventStatus _processEvent(const T &event) {
        EventStatus status = Widget::dispatchEvent(event);

        if (!status.shouldHandle(status.NODE))
            return status.update();

        auto targetChild = children.begin();
        for (auto &child : children) {
            status = dispatchToChild(*child, event);

            if (!status.shouldHandle(status.SIBL))
                break;

            ++targetChild;
        }

        if constexpr (focusOnEvent<T>) {
            if (targetChild != children.end()) {
                focusChild(targetChild);
            }
        }

        return status.update();
    }

    void focusChild(const decltype(children)::iterator &child);

};

// TODO: Maybe some other specific events as well?
template <>
constexpr bool Group::focusOnEvent<MouseClickEvent> = true;


// Render event has to be handled separately, because it works in reverse
template <>
inline Widget::EventStatus Group::_processEvent(const RenderEvent &event) {
    EventStatus status = Widget::dispatchEvent(event);

    if (!status.shouldHandle(status.NODE))
        return status.update();

    static_assert(RenderEvent::demands_modification);
    RenderEvent subEvent = event.createSubEvent(region);

    auto childrenEnd = children.end();  // We rely on our loop being cyclic
    for (auto iter = --children.end(); iter != childrenEnd; --iter) {
        auto &child = *iter;

        status = child->dispatchEvent(subEvent);

        if (!status.shouldHandle(status.SIBL))
            break;
    }

    static_assert(!focusOnEvent<RenderEvent>);

    return status.update();
}


// FocusUpdates should only be passed to the active child
template <>
inline Widget::EventStatus Group::_processEvent(const FocusUpdateEvent &event) {
    EventStatus status = Widget::dispatchEvent(event);

    if (!status.shouldHandle(status.NODE))
        return status.update();

    if (children.isEmpty())
        return status.update();

    status = dispatchToChild(*children.front(), event);

    return status.update();
}


}


#endif // AGF_WIDGETS_GROUP_H


#ifndef AGF_WIDGETS_GROUP_H
#define AGF_WIDGETS_GROUP_H

#include <AGF/widget.h>
#include <ACL/list.h>


namespace abel::gui::widgets {


template <typename T>
static constexpr bool eventCapturesFocus = false;

// TODO: Maybe some other specific events as well?
template <>
constexpr bool eventCapturesFocus<MouseClickEvent> = true;


// TODO: Maybe inherit virtually
template <typename ITEM>
class GroupOf : public /*virtual*/ Widget {
public:
    EVENT_HANDLER_USING(Widget)

    GroupOf(Widget *parent_, const Rect<double> &region_) :
        Widget(parent_, region_) {}

    template <typename T, typename ... As>
    T &createChild(const Rect<double> &relRegion, As &&... args) {
        static_assert(std::is_base_of_v<ITEM, T>);

        // TODO: Maybe disallow overflow?
        ITEM &child = *children.insertBack(new T(this, region.relRect(relRegion, true), std::forward<As>(args)...));

        return dynamic_cast<T &>(child);
    }

    #define EVENTS_DSL_ITEM_(NAME) \
        EVENT_HANDLER_OVERRIDE(NAME)
    #include <AGF/events.dsl.h>

protected:
    list<unique_ptr<ITEM>> children{};


    // TODO: A public alternative
    void addChild(ITEM *child) {
        assert(child);
        children.insertBack(child);
        child->updateParent(this);
    }

    // TODO: Same
    void clearChildren() {
        children.clear();
    }

    // Render event has to be handled separately, because it works in reverse
    inline EventStatus _processEvent(const RenderEvent &event) {
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

        static_assert(!eventCapturesFocus<RenderEvent>);

        return status.update();
    }

    // FocusUpdates should only be passed to the active child
    inline EventStatus _processEvent(const FocusUpdateEvent &event) {
        EventStatus status = Widget::dispatchEvent(event);

        if (!status.shouldHandle(status.NODE))
            return status.update();

        if (children.isEmpty())
            return status.update();

        status = dispatchToChild(*children.front(), event);

        return status.update();
    }

    template <typename T>
    inline EventStatus _processEvent(const T &event) {
        // Sadly, full specializations aren't possible here...
        // I guess I'll at least provide
        if constexpr (std::is_same_v<T, RenderEvent> || std::is_same_v<T, FocusUpdateEvent>) {
            return _processEvent(event);  // This is NOT recursion, but delegation to specific overloads!
        }

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

        if constexpr (eventCapturesFocus<T>) {
            if (targetChild != children.end()) {
                focusChild(targetChild);
            }
        }

        return status.update();
    }

    void focusChild(const typename decltype(children)::iterator &child)  {
        children.front()->dispatchEvent(FocusUpdateEvent{false});

        children.swapFront(child);

        children.front()->dispatchEvent(FocusUpdateEvent{true});
    }

};


#define EVENTS_DSL_ITEM_(NAME)                  \
    template <typename ITEM>                    \
    EVENT_HANDLER_IMPL(GroupOf<ITEM>, NAME) {   \
        return _processEvent(event);            \
    }

#include <AGF/events.dsl.h>


extern template
class GroupOf<Widget>;


using Group = GroupOf<Widget>;


}


#endif // AGF_WIDGETS_GROUP_H


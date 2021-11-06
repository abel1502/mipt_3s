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

    void addChild(Widget *child);

    void clearChildren();

    #define EVENTS_DSL_ITEM_(NAME) \
        EVENT_HANDLER_OVERRIDE(NAME)
    #include <AGF/events.dsl.h>

protected:
    list<unique_ptr<Widget>> children{};

    template <typename T>
    static constexpr bool focusOnEvent = false;

    template <typename T>
    EventStatus _processEvent(const T &event) {
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
constexpr bool Group::focusOnEvent<EVENT_CLS_NAME(MouseClick)> = true;


template <>
Widget::EventStatus Group::_processEvent(const EVENT_CLS_NAME(Render) &event);
template <>
Widget::EventStatus Group::_processEvent(const EVENT_CLS_NAME(FocusUpdate) &event);


}


#endif // AGF_WIDGETS_GROUP_H


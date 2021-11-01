#include <AGF/llgui.h>
#include <ACL/general.h>
#include <AGF/widgets/group.h>


namespace abel::gui::widgets {


Group::Group(Widget *parent_, const Rect<double> &region_) :
    Widget(parent_, region_) {}

void Group::addChild(Widget *child) {
    children.insertBack(child);
}

void Group::clearChildren() {
    children.clear();
}

void Group::focusChild(const decltype(children)::const_iterator &child) {
    children.swapFront(child);

    children->
}

// Render event has to be handled separately, because it works in reverse
template <>
Widget::EventStatus Group::_processEvent(const EVENT_CLS_NAME(Render) &event) {
    EventStatus status = EVENT_HANDLER_CALL_BASE(Widget, event);

    if (!status.shouldHandle(status.NODE))
        return status.update();

    auto childrenEnd = children.end();  // We rely on our loop bein cyclic
    for (auto iter = --children.end(); iter != childrenEnd; --iter) {
        auto &child = *iter;

        status = EVENT_HANDLER_CALL_INST(child, event);

        if (!status.shouldHandle(status.SIBL))
            break;
    }

    static_assert(!focusOnEvent<EVENT_CLS_NAME(Render)>);

    return status.update();
}

#define EVENTS_DSL_ITEM_(NAME)          \
    EVENT_HANDLER_IMPL(Group, NAME) {   \
        return _processEvent(event);    \
    }

#include <AGF/events.dsl.h>


}

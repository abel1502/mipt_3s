#include <AGF/llgui.h>
#include <ACL/general.h>
#include <AGF/widgets/group.h>


namespace abel::gui::widgets {


Group::Group(Widget *parent_, const Rect<double> &region_) :
    Widget(parent_, region_) {}

void Group::addChild(Widget *child) {
    assert(child);
    children.insertBack(child);
    child->updateParent(this);
}

void Group::clearChildren() {
    children.clear();
}

void Group::focusChild(const decltype(children)::iterator &child) {
    children.front()->dispatchEvent(EVENT_CLS_NAME(FocusUpdate){false});

    children.swapFront(child);

    children.front()->dispatchEvent(EVENT_CLS_NAME(FocusUpdate){true});
}

// Render event has to be handled separately, because it works in reverse
template <>
Widget::EventStatus Group::_processEvent(const EVENT_CLS_NAME(Render) &event) {
    EventStatus status = Widget::dispatchEvent(event);

    if (!status.shouldHandle(status.NODE))
        return status.update();

    auto childrenEnd = children.end();  // We rely on our loop being cyclic
    for (auto iter = --children.end(); iter != childrenEnd; --iter) {
        auto &child = *iter;

        status = dispatchToChild(*child, event);

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

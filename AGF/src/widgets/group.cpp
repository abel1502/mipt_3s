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
    children.front()->dispatchEvent(FocusUpdateEvent{false});

    children.swapFront(child);

    children.front()->dispatchEvent(FocusUpdateEvent{true});
}


#define EVENTS_DSL_ITEM_(NAME)          \
    EVENT_HANDLER_IMPL(Group, NAME) {   \
        return _processEvent(event);    \
    }

#include <AGF/events.dsl.h>


}

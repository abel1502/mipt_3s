#include <AGF/widget.h>


namespace abel::gui {


#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

EVENT_HANDLER_IMPL(Widget, Render) {
    if (!visible)
        return EventStatus::done();

    return EventStatus::skip();
}

EVENT_HANDLER_IMPL(Widget, Move) {
    return EventStatus::skip();
}

/*EVENT_HANDLER_IMPL(Widget, Resize) {
    region = event.newRegion;

    return EventStatus::done();
}*/

EVENT_HANDLER_IMPL(Widget, FocusUpdate) {
    return EventStatus::skip();
}

EVENT_HANDLER_IMPL(Widget, Start) {
    return EventStatus::skip();
}

EVENT_HANDLER_IMPL(Widget, Exit) {
    return EventStatus::skip();
}

EVENT_HANDLER_IMPL(Widget, MouseClick) {
    return EventStatus::skip();
}

EVENT_HANDLER_IMPL(Widget, MouseMove) {
    return EventStatus::skip();
}

EVENT_HANDLER_IMPL(Widget, Keyboard) {
    return EventStatus::skip();
}

EVENT_HANDLER_IMPL(Widget, User) {
    return EventStatus::skip();
}

#pragma GCC diagnostic pop


Widget::Widget(Widget *parent_, const Rect<double> &region_) :
    parent{parent_}, region{region_} {}

void Widget::updateParent(Widget *parent_) {
    parent = parent_;
}


}

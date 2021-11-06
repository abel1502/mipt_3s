#include <AGF/llgui.h>
#include <AGF/widget.h>


namespace abel::gui {


#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

EVENT_HANDLER_IMPL(Widget, Render) {
    if (!visible)
        return EventStatus::done();

    Rect<double> curRect = event.region.relRect(region);

    if constexpr (DEBUG_RENDER_BOXES) {
        event.target.drawLine(curRect.getVertex(0, 0), curRect.getVertex(0, 1), Color::ORANGE);
        event.target.drawLine(curRect.getVertex(0, 1), curRect.getVertex(1, 1), Color::ORANGE);
        event.target.drawLine(curRect.getVertex(1, 1), curRect.getVertex(1, 0), Color::ORANGE);
        event.target.drawLine(curRect.getVertex(1, 0), curRect.getVertex(0, 0), Color::ORANGE);
    }

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

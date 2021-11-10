#include <AGF/llgui.h>
#include <AGF/widget.h>


namespace abel::gui {


#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

EVENT_HANDLER_IMPL(Widget, Render) {
    if (!visible)
        return EventStatus::done();

    if constexpr (DEBUG_RENDER_BOXES == DRB_OBJECT) {
        event.target.drawBounds(region, Color::ORANGE);
    } else if constexpr (DEBUG_RENDER_BOXES == DRB_EVENT) {
        event.target.drawBounds(event.region, Color::GREEN);
    }

    return EventStatus::skip();
}

EVENT_HANDLER_IMPL(Widget, Move) {
    region += event.delta;

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
    Vector2d oldPos = parent  ? parent ->region.getPos() : Vector2d::ZERO;
    Vector2d newPos = parent_ ? parent_->region.getPos() : Vector2d::ZERO;

    parent = parent_;

    staticShift(newPos - oldPos);
}

void Widget::staticShift(const Vector2d &by) {
    region += by;
}


}

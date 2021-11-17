#include <AGF/llgui.h>
#include <AGF/widget.h>
#include <AGF/application.h>


namespace abel::gui {


// #pragma GCC diagnostic push
// #pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma warning(push)
#pragma warning(disable : 4100)

EVENT_HANDLER_IMPL(Widget, Render) {
    if (hidden)
        return EventStatus::stop(EventStatus::NODE);

    if constexpr (DEBUG_RENDER_BOXES == DRB_OBJECT) {
        event.target.drawFrame(region, Color::ORANGE);
    } else if constexpr (DEBUG_RENDER_BOXES == DRB_EVENT) {
        event.target.drawFrame(event.region, Color::GREEN);
    }

    return EventStatus::skip();
}

EVENT_HANDLER_IMPL(Widget, Move) {
    region += event.delta;

    Application::getInstance().demandRedraw();

    return EventStatus::done();
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
    if (hidden)
        return EventStatus::stop(EventStatus::NODE);

    return EventStatus::skip();
}

EVENT_HANDLER_IMPL(Widget, MouseMove) {
    if (hidden)
        return EventStatus::stop(EventStatus::NODE);

    return EventStatus::skip();
}

EVENT_HANDLER_IMPL(Widget, Keyboard) {
    return EventStatus::skip();
}

EVENT_HANDLER_IMPL(Widget, User) {
    return EventStatus::skip();
}

// #pragma GCC diagnostic pop
#pragma warning(pop)


Widget::Widget(Widget *parent_, const Rect<double> &region_) :
    parent{parent_}, region{region_} {}  // We cannot copy parent's style here yet, because it wan't yet set

bool Widget::updateParent(Widget *parent_) {
    if (parent == parent_)
        return true;

    Vector2d oldPos = parent  ?  parent->region.getPos() : Vector2d{};
    Vector2d newPos = parent_ ? parent_->region.getPos() : Vector2d{};
    
    parent = parent_;

    staticShift(newPos - oldPos);

    setStyle(parent ? parent->styleHandle : StyleManager::ROOT_STYLE_HANDLE);

    return false;
}

bool Widget::staticShift(const Vector2d &by) {
    if (by.isZero())
        return true;

    region += by;

    return false;
}

bool Widget::setStyle(StyleManager::StyleHandle newHandle) {
    styleHandle = newHandle;

    return false;
}

Style &Widget::getStyle() {
    return Application::getInstance().getStyleManager().getStyle(styleHandle);
}


}

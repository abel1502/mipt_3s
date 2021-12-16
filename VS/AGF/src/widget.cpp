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
        event.target.setLineColor(Color::ORANGE);
        event.target.drawFrame(region);
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

EVENT_HANDLER_IMPL(Widget, VisibilityUpdate) {
    return EventStatus::skip();
}

EVENT_HANDLER_IMPL(Widget, Start) {
    return EventStatus::skip();
}

EVENT_HANDLER_IMPL(Widget, Exit) {
    return EventStatus::skip();
}

EVENT_HANDLER_IMPL(Widget, MouseClick) {
    if (!hidden && (Application::getInstance().isMouseCaptured(this) ||
                    hitTest(event.pos))) {

        return EventStatus::skip();
    }

    return EventStatus::stop(EventStatus::NODE);
}

EVENT_HANDLER_IMPL(Widget, MouseMove) {
    // TODO: Perhaps remove the hit-test based cutoff
    if (!hidden && (Application::getInstance().isMouseCaptured(this) ||
                    hitTest(event.getCutoffPos()))) {
        return EventStatus::skip();
    }

    return EventStatus::stop(EventStatus::NODE);
}

EVENT_HANDLER_IMPL(Widget, MouseScroll) {
    // TODO: Same
    if (!hidden && (Application::getInstance().isMouseCaptured(this) ||
                    hitTest(event.pos))) {
        return EventStatus::skip();
    }

    return EventStatus::stop(EventStatus::NODE);
}

EVENT_HANDLER_IMPL(Widget, KeyPress) {
    return EventStatus::skip();
}

EVENT_HANDLER_IMPL(Widget, KeyboardInput) {
    return EventStatus::skip();
}

EVENT_HANDLER_IMPL(Widget, User) {
    return EventStatus::skip();
}

// #pragma GCC diagnostic pop
#pragma warning(pop)


Widget::Widget(Widget *parent_, const Rect<double> &region_) :
    parent{parent_}, region{region_} {}  // We cannot copy parent's style here yet, because it wan't yet set

void Widget::die() {
    if (dead) {
        return;
    }

    dead = true;

    sigDeath(*this);
}

bool Widget::testParentsHidden() const {
    return isHidden() || (parent && parent->testParentsHidden());
}

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

bool Widget::setHidden(bool hidden_) {
    bool result = (hidden == hidden_);

    hidden = hidden_;

    if (!result) {
        dispatchEvent(VisibilityUpdateEvent{hidden});
    }

    return result;
}

bool Widget::hitTest(const Vector2d &pos) const {
    return region.contains(pos);
}

Widget::EventStatus Widget::handleMouseOpaque(const MouseMoveEvent  &event, EventStatus baseStatus) {
    if (!baseStatus.shouldHandle(baseStatus.NODE)) {
        return baseStatus;
    }

    if (hitTest(event.getCutoffPos())) {
        return EventStatus::stop(EventStatus::TREE);
    }

    return baseStatus;
}

Widget::EventStatus Widget::handleMouseOpaque(const MouseClickEvent &event, EventStatus baseStatus) {
    if (!baseStatus.shouldHandle(baseStatus.NODE)) {
        return baseStatus;
    }

    if (hitTest(event.pos)) {
        return EventStatus::stop(EventStatus::TREE);
    }

    return baseStatus;
}

Widget::EventStatus Widget::handleMouseOpaque(const MouseScrollEvent &event, EventStatus baseStatus) {
    if (!baseStatus.shouldHandle(baseStatus.NODE)) {
        return baseStatus;
    }

    if (hitTest(event.pos)) {
        return EventStatus::stop(EventStatus::TREE);
    }

    return baseStatus;
}


}

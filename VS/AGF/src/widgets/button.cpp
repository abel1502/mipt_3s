#include <AGF/llgui.h>
#include <AGF/widgets/button.h>
#include <ACL/general.h>
#include <AGF/application.h>


namespace abel::gui::widgets {


Button::Button(Widget *parent_, const Rect<double> &region_, const char *text_) :
    Base(parent_, region_,
         new Label    (this, region_, text_, /*region_.h() * 0.3*/ 16),
         new Rectangle(this, region_, COL_DEFAULT)) {}


EVENT_HANDLER_IMPL(Button, MouseClick) {
    // DBG("[%1zu] %4s at (%3lg %3lg)<=((%3lg %3lg))<=(%3lg %3lg) - %s",
    //     ((size_t)this) % 7,
    //     event.type == decltype(event.type)::Down ? "down" : "up",
    //     region.getStart().x(), region.getStart().y(),
    //     event.pos.x(), event.pos.y(),
    //     region.getEnd().x(), region.getEnd().y(),
    //     region.contains(event.pos) ? "hit" : "miss");

    assert(areChildrenSet());

    // Intentionally skipping StaticGroup::dispatchEvent, not to pass this to our children
    EventStatus status = Widget::dispatchEvent(event);

    if (!status.shouldHandle(status.NODE))
        return status;

    bool hit = region.contains(event.pos);

    if (!hit && !Application::getInstance().isMouseCaptured(this))
        return status;

    if (event.button != decltype(event.button)::Left)
        return status;

    if (event.type == decltype(event.type)::Down) {
        onMouseDown(event, hit);
    } else {
        onMouseUp(event, hit);
    }

    return EventStatus::stop(EventStatus::TREE);
}


void Button::onMouseDown(const MouseClickEvent &, bool hit) {
    assert(hit && !isDown);  // The other way shouldn't be possible

    isDown = true;

    body().recolor(COL_PRESSED);
    Application::getInstance().captureMouse(this);
}

void Button::onMouseUp(const MouseClickEvent &, bool hit) {
    if (!isDown)
        return;

    isDown = false;

    body().recolor(COL_DEFAULT);
    Application::getInstance().releaseMouse(this);

    if (hit) {
        sigClick();
    }
}


}

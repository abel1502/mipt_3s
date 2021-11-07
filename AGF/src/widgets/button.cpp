#include <AGF/llgui.h>
#include <AGF/widgets/button.h>
#include <ACL/general.h>
#include <AGF/application.h>


namespace abel::gui::widgets {


Button::Button(Widget *parent_, const Rect<double> &region_, const char *text_) :
    Base(parent_, region_,
         new Label    (this, region_, text_, region_.h() * 0.3),
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

    // Intentionally skipping Button::dispatchEvent, not to pass this to our children
    EventStatus status = Widget::dispatchEvent(event);

    if (!status.shouldHandle(status.NODE))
        return status.update();

    if (!region.contains(event.pos))
        return status.update();

    if (event.type == decltype(event.type)::Down) {
        onMouseDown(event);
    } else {
        onMouseUp(event);
    }

    return EventStatus::stop(EventStatus::TREE);
}


void Button::onMouseDown(const MouseClickEvent &) {
    child<0>().recolor(COL_PRESSED);
    Application::getInstance().captureMouse(this);
}

void Button::onMouseUp(const MouseClickEvent &) {
    child<0>().recolor(COL_DEFAULT);
    Application::getInstance().releaseMouse(this);
    sigClick();
}


}

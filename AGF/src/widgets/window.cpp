#include <AGF/llgui.h>
#include <AGF/widgets/window.h>


namespace abel::gui::widgets {


namespace _impl {

Header::Header(Window *parent_, const Rect<double> &region_, const char *title_) :
    Base(parent_, region_,
         new Label(this, region_.padded(LABEL_LPAD, LABEL_RPAD, LABEL_TPAD, LABEL_BPAD),
                   title_, 16, DT_LEFT | DT_VCENTER | DT_END_ELLIPSIS),
         new Button(this, Rect<double>::se(region_.x1() - BTN_CLOSE_LDX, region_.y0() + BTN_TPAD,
                                           region_.x1() - BTN_CLOSE_RDX, region_.y1()), "x"),
         new Button(this, Rect<double>::se(region_.x1() - BTN_MINIMIZE_LDX, region_.y0() + BTN_TPAD,
                                           region_.x1() - BTN_MINIMIZE_RDX, region_.y1()), "_")
         ) {}

EVENT_HANDLER_IMPL(Header, MouseMove) {
    if (grabbed) {
        sigDrag(event.getDelta());
        return EventStatus::stop(EventStatus::TREE);
    }

    EventStatus status = Base::dispatchEvent(event);

    /*if (!status.shouldHandle(status.NODE)) {
        return status;
    }*/

    return status;
}

EVENT_HANDLER_IMPL(Header, MouseClick) {
    EventStatus status = Base::dispatchEvent(event);

    if (!status.shouldHandle(status.NODE))
        return status;

    if (!region.contains(event.pos) && !Application::getInstance().isMouseCaptured(this))
        return status;

    if (event.button != decltype(event.button)::Left)
        return status;

    if (event.type == decltype(event.type)::Down) {
        onMouseDown(event);
    } else {
        onMouseUp(event);
    }

    return EventStatus::stop(EventStatus::TREE);
}

void Header::onMouseDown(const MouseClickEvent &) {
    assert(!grabbed);  // The other way shouldn't be possible

    grabbed = true;

    Application::getInstance().captureMouse(this);
}

void Header::onMouseUp(const MouseClickEvent &) {
    if (!grabbed)
        return;

    grabbed = false;

    Application::getInstance().releaseMouse(this);
}


Borders::Borders(Window *parent_, const Rect<double> &region_) :
    Base(parent_, region_,
         new Rectangle{this, region_, Color::LBLUE}
         ) {}

}  // namespace _impl


}

WindowManager(Widget *parent_, const Rect<double> &region_) :
    Base(parent_, region_) {}


template
class abel::unique_ptr<abel::gui::widgets::Window>;

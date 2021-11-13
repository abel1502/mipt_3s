#include <AGF/llgui.h>
#include <AGF/widgets/window.h>
#include <AGF/application.h>


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

EVENT_HANDLER_IMPL(Borders, MouseClick) {
    EventStatus status = Base::dispatchEvent(event);

    if (!status.shouldHandle(status.NODE))
        return status;

    if (region.contains(event.pos))
        return EventStatus::stop(EventStatus::TREE);

    return EventStatus::skip();
}


}  // namespace _impl


WindowManager::WindowManager(Widget *parent_, const Rect<double> &region_) :
    Base(parent_, region_) {}

Window::Window(WindowManager *parent_, const Rect<double> &region_,
       const char *title_, Content *contents_) :
    Base(parent_, region_, contents_,
         new _impl::Header(this, Rect<double>::wh(region_.x(), region_.y(), region_.w(), _impl::Header::HEADER_HEIGHT),
                           title_),
         new _impl::Borders(this, region_)) {
    REQUIRE(region.h() >= _impl::Header::HEADER_HEIGHT + _impl::Borders::BORDER_WIDTH);
    REQUIRE(region.w() >= _impl::Borders::BORDER_WIDTH * 2 + _impl::Header::MIN_WIDTH);

    contents().staticShift(Vector2d(0, _impl::Header::HEADER_HEIGHT));

    header().closeBtn().sigClick += [](){
        DBG("Closing.");
        /*Application::getInstance().enqueueAction(
            [](Application &app){
                //
            }
        );*/
        return false;
    };

    header().minimizeBtn().sigClick += [](){
        DBG("Minimizing.");
        return false;
    };

    header().sigDrag += [this](const Vector2d &delta){
        dispatchEvent(MoveEvent(delta));
    };
}

Window::EmptyBody::EmptyBody(Window *parent_) :
    Widget(parent_, Rect<double>::wh(0, 0, 0, 0)) {}

}


template
class abel::unique_ptr<abel::gui::widgets::Window>;



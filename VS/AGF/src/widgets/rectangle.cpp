#include <AGF/llgui.h>
#include <ACL/general.h>
#include <AGF/widgets/rectangle.h>
#include <AGF/application.h>
#include <cassert>


namespace abel::gui::widgets {


Rectangle::Rectangle(Widget *parent_, const Rect<double> &region_, const Color &color_) :
    Widget(parent_, region_), color{color_}, texture{new Texture((unsigned)region.w(), (unsigned)region.h())} {

    bakeTexture();
}

void Rectangle::recolor(const Color &color_) {
    color = color_;
    bakeTexture();
}

void Rectangle::bakeTexture() {
    assert(texture);

    texture->setFillColor(color);
    texture->clear();

    Application::getInstance().demandRedraw();
}

EVENT_HANDLER_IMPL(Rectangle, Render) {
    EventStatus status = Widget::dispatchEvent(event);

    if (!status.shouldHandle(status.NODE))
        return status;

    assert(texture);
    event.target.embed(region, *texture);

    return EventStatus::done();
}

}

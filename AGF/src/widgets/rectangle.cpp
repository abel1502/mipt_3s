#include <AGF/llgui.h>
#include <ACL/general.h>
#include <AGF/widgets/rectangle.h>
#include <cassert>


namespace abel::gui::widgets {


Rectangle::Rectangle(Widget *parent_, const Rect<double> &region_, const Color &color_) :
    Widget(parent_, region_), color{color_}, texture{new Texture((unsigned)region.w(), (unsigned)region.x())} {

    bakeTexture();
}

void Rectangle::recolor(const Color &color_) {
    color = color_;
    bakeTexture();
}

void Rectangle::bakeTexture() {
    assert(texture);

    texture->clear(color);
    texture->update();
}

EVENT_HANDLER_IMPL(Rectangle, Render) {
    EventStatus status = EVENT_HANDLER_CALL_BASE(Widget, event);

    assert(texture);
    event.target.embed(event.region, *texture);

    return status == E_SKIP ? E_CONTINUE : status;
}

EVENT_HANDLER_IMPL(Rectangle, Resize) {
    EventStatus status = EVENT_HANDLER_CALL_BASE(Widget, event);

    texture->resize(event.newRegion.getDiag());
    bakeTexture();

    return status == E_SKIP ? E_CONTINUE : status;
}

Rectangle::~Rectangle() = default;

}